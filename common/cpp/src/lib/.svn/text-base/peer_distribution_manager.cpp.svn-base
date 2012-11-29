/*
 * Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of Yale University nor the names of its contributors may
 *       be used to endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "p4p/app/peer_distribution_manager.h"

#include <limits.h>
#include <iterator>
#include <stdlib.h>
#include <assert.h>
#include <p4p/app/detail/perisp_peer_distribution.h>
#include <p4p/errcode.h>
#include <p4p/app/errcode.h>

namespace p4p {
namespace app {

PeerDistributionManager::PeerDistributionManager()
{
}

PeerDistributionManager::~PeerDistributionManager()
{
	/* Iterate through the existing PerISPPeerDistributions and free resources for each one. */
	for (PerISPPeerDistributionMap::iterator itr = m_views.begin(); itr != m_views.end(); ++itr)
		delete itr->second;
}

const detail::PerISPPeerDistribution* PeerDistributionManager::getPeerDistribution(const ISP* isp) const
{
	m_views_mutex.shared_lock();

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
	{
		m_views_mutex.shared_unlock();
		return NULL;
	}

	/* NOTE: We leave the lock aquired here. It will be released when
	 * releasePeerDistribution() is called. */

	return view;
}

bool PeerDistributionManager::releasePeerDistribution(const ISP* isp) const
{
	/* Ensure that the ISP exists.  If it doesn't, then the lock would
	 * not have been acquired in getPeerDistribution(). */
	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return false;

	m_views_mutex.shared_unlock();
	return true;
}

bool PeerDistributionManager::addPeer(P4PPeerBase* peer)
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	{
		/* Add peer to our collection with its existing stats */
		PeerStats stats(peer->isSeeder(), peer->getUploadCapacity(), peer->getDownloadCapacity());

		p4p::detail::ScopedExclusiveLock peers_lock(m_peers_mutex);
		if (!m_peers.insert(std::make_pair(peer, stats)).second)
			return false;
	}

	/* This loop does two things:
	 * - Lookup the peer's PID within each ISP, and add the peer to
	 *   the my-Internet view for each ISP.
	 * - Determine the most-specific match for the peer's IP address
	 *   across all ISPs' PID Maps. The ISP with the most specific
	 *   matching prefix for an Internal PID is considered the peer's
	 *   Home ISP, and the matching PID is the peer's Home PID.
	 */
	const ISP* home_isp = NULL;
	int home_pid = p4p::ERR_UNKNOWN_PID;

	/* Keep track of the length (in bits) of the longest matching
	 * prefix.  The internal PID (across all ISPs) with the longest
	 * matching prefix is considered the peer's Home PID. */
	unsigned int longest_prefix_length = 0;

	/* Iterate through the existing ISPs */
	for (PerISPPeerDistributionMap::iterator itr = m_views.begin(); itr != m_views.end(); ++itr)
	{
		const ISP* isp = itr->first;
		detail::PerISPPeerDistribution* view = itr->second;

		/* Lookup the peer's PID within this ISP. Treat an erroneous
		 * result as an unknown PID.
		 */
		IPPrefix matching_prefix;
		int pid = isp->lookup(peer->getIPAddress(), &matching_prefix);

		/* Treat an erroneous result as an unknown PID. This is
		 * not a candidate for the home ISP or home PID for the peer,
		 * so we immediately continue on with the next ISP.
		 */
		if (pid < 0)
		{
			view->addPeer(peer, p4p::ERR_UNKNOWN_PID);
			continue;
		}

		/* Add the peer to the ISP's my-Internet view. */
		view->addPeer(peer, pid);

		/* Check if the matching PID is an Internal PID.  If it isn't,
		 * then this ISP and PID is not a candidate for the peer's
		 * Home ISP or PID, and we can continue on with the next ISP.
		 */
		unsigned int num_intraisp_pids;
		isp->getNumPIDs(&num_intraisp_pids, NULL);
		if (pid >= (int)num_intraisp_pids)
			continue;

		/* If the matching prefix is less-specific than one we've
		 * previously matched, then this is not considered the
		 * Home ISP/PID. Continue on with the next ISP. */
		if (matching_prefix.get_length() < longest_prefix_length)
			continue;

		/* This is the most-specific prefix thus far. */
		home_isp = isp;
		home_pid = pid;
		longest_prefix_length = matching_prefix.get_length();
	}

	/* If a Home ISP/PID was found for the peer, then store them in
	 * the peer's data structure. */
	if (home_isp)
		peer->setHome(home_isp, home_pid);

	return true;
}

bool PeerDistributionManager::removePeer(const P4PPeerBase* peer)
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	/* Iterate through the existing PerISPPeerDistributions and remove the peer from each. */
	for (PerISPPeerDistributionMap::iterator itr = m_views.begin(); itr != m_views.end(); ++itr)
		itr->second->removePeer(peer);

	/* Remove from our master list of peers */
	p4p::detail::ScopedExclusiveLock peers_lock(m_peers_mutex);
	if (!m_peers.erase(const_cast<P4PPeerBase*>(peer)))
		return false;

	return true;
}

bool PeerDistributionManager::updatePeerStats(const P4PPeerBase* peer)
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	PeerStats new_stats(peer->isSeeder(), peer->getUploadCapacity(), peer->getDownloadCapacity());
	PeerStats prev_stats;
	{
		/* Lookup existing stats and insert new ones */
		p4p::detail::ScopedExclusiveLock peers_lock(m_peers_mutex);

		P4PPeerBase* peerNonConst = const_cast<P4PPeerBase*>(peer);

		PeerStatsMap::const_iterator itr = m_peers.find(peerNonConst);
		if (itr == m_peers.end())
			return false;

		/* Save copy of old stats */
		prev_stats = itr->second;

		/* Insert new stats */
		m_peers[peerNonConst] = new_stats;
	}

	/* Compute changes in stats */
	bool is_seed_changed = (new_stats.is_seed != prev_stats.is_seed);
	float upload_cap_change = (new_stats.upload_cap - prev_stats.upload_cap);
	float download_cap_change = (new_stats.download_cap - prev_stats.download_cap);
	float upload_rate_change = (new_stats.upload_rate - prev_stats.upload_rate);
	float download_rate_change = (new_stats.download_rate - prev_stats.download_rate);
	int active_peers_change = (new_stats.num_active_peers - prev_stats.num_active_peers);

	/* Iterate through the existing PerISPPeerDistributions and update stats in each. */
	for (PerISPPeerDistributionMap::iterator itr = m_views.begin(); itr != m_views.end(); ++itr)
	{
		detail::PerISPPeerDistribution* view = itr->second;
		view->updatePeerStats(peer, is_seed_changed, upload_cap_change, download_cap_change, upload_rate_change, download_rate_change, active_peers_change);
	}

	return true;
}

bool PeerDistributionManager::addISP(const ISP* isp)
{
	p4p::detail::ScopedExclusiveLock lock(m_views_mutex);

	/* Check if we already maintain a view for the ISP. If so,
	 * return it. */
	PerISPPeerDistributionMap::iterator itr = m_views.find(isp);
	if (itr != m_views.end())
		return false;

	/* Create new PerISPPeerDistribution and add the mapping */
	detail::PerISPPeerDistribution* view = new detail::PerISPPeerDistribution(this, isp);
	m_views.insert(std::make_pair(isp, view));

	return true;
}

void PeerDistributionManager::removeISP(const ISP* isp)
{
	p4p::detail::ScopedExclusiveLock lock(m_views_mutex);

	/* Do nothing if no view exists. */
	PerISPPeerDistributionMap::iterator itr = m_views.find(isp);
	if (itr == m_views.end())
		return;

	/* Free memory and remove the mapping */
	delete itr->second;
	m_views.erase(itr);
}

int PeerDistributionManager::updatePeerDistribution(const ISP* isp)
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	PerISPPeerDistributionMap::const_iterator itr = m_views.find(isp);
	if (itr == m_views.end())
		return ERR_INVALID_ISP;

	if (itr->second)
		itr->second->updatePeerDistribution();

	return 0;
}

SwarmState PeerDistributionManager::getSwarmState(const ISP* isp) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return SwarmState();

	return view->getSwarmState();
}

bool PeerDistributionManager::listPIDPeers(const ISP* isp, int pid, std::vector<const P4PPeerBase*>& out_peers) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return false;

	view->lock();
	bool result = view->listPIDPeers(pid, out_peers);
	view->unlock();
	return result;
}
	
bool PeerDistributionManager::listIntraISPPeers(const ISP* isp, std::vector<const P4PPeerBase*>& out_peers) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return false;

	view->lock();
	view->listIntraISPPeers(out_peers);
	view->unlock();
	return true;
}

bool PeerDistributionManager::listExternalPeers(const ISP* isp, std::vector<const P4PPeerBase*>& out_peers) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return false;

	view->lock();
	view->listExternalPeers(out_peers);
	view->unlock();
	return true;
}

const P4PPeerBase* PeerDistributionManager::getRandomPeer(const ISP* isp, int pid) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return NULL;

	view->lock();
	const P4PPeerBase* result = view->getRandomPeer(pid);
	view->unlock();
	return result;
}

int PeerDistributionManager::getNumPeers(const ISP* isp, int pid) const
{
	p4p::detail::ScopedSharedLock lock(m_views_mutex);

	detail::PerISPPeerDistribution* view = getISPViewInternal(isp);
	if (!view)
		return -1;

	view->lock();
	int result = view->getNumPeers(pid);
	view->unlock();
	return result;
}

detail::PerISPPeerDistribution* PeerDistributionManager::getISPViewInternal(const ISP* isp) const
{
	PerISPPeerDistributionMap::const_iterator itr = m_views.find(isp);
	if (itr != m_views.end())
		return itr->second;

	return NULL;
}

};
};
