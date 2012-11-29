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


#include "p4p/app/detail/perisp_peer_distribution.h"

#include <limits.h>
#include <iterator>
#include <stdlib.h>
#include <assert.h>
#include <p4p/isp.h>
#include <p4p/errcode.h>

namespace p4p {
namespace app {
namespace detail {

PerISPPeerDistribution::PerISPPeerDistribution(const PeerDistributionManager* manager, const p4p::ISP* isp)
	: m_manager(manager), m_isp(isp)
{
}

PerISPPeerDistribution::~PerISPPeerDistribution()
{
}

void PerISPPeerDistribution::lock() const
{
	m_peers_mutex.shared_lock();
}

void PerISPPeerDistribution::unlock() const
{
	m_peers_mutex.shared_unlock();
}

bool PerISPPeerDistribution::addPeer(P4PPeerBase* peer, int pid)
{
	p4p::detail::ScopedExclusiveLock lock(m_peers_mutex);

	/* Construct initial stats from the peer */
	PeerDistributionManager::PeerStats stats(peer->isSeeder(), peer->getUploadCapacity(), peer->getDownloadCapacity());

	return addPeerInternal(peer, pid, stats);
}

bool PerISPPeerDistribution::addPeerInternal(P4PPeerBase* peer, int pid, const PeerDistributionManager::PeerStats& stats)
{
	/* If this is our first peer, we'll need to allocate the necessary
	 * data structures. */
	if (m_peer_to_pid.empty())
	{
		unsigned int num_total_pids;
		m_isp->getNumPIDs(NULL, &num_total_pids);

		/* Initialize new PID-level stats and per-PID list of peers */
		m_swarm_state = SwarmState(num_total_pids);
		m_pid_peers = PIDPeerMap(num_total_pids + 1);	/* Allocate additional entry for peers without any PID */
	}

	/* Ensure the PID is in range. If not, it gets set to the unknown PID */
	if (pid >= 0 && (unsigned int)pid >= m_pid_peers.size() - 1)
		pid = ERR_UNKNOWN_PID;

	/* Insert mapping from peer to it's PID. Ignore
	 * addition if we already have the peer.
	 */
	if (!m_peer_to_pid.insert(std::make_pair(peer, pid)).second)
		return false;

	/* Insert peer into set of peers for this particular PID */
	if (pid >= 0)
		m_pid_peers[pid].push_back(peer);
	else
		m_pid_peers.back().push_back(peer);

	if (pid >= 0)
	{
		/* Update swarm statistics */
		if (stats.is_seed)
			m_swarm_state.update(pid, 1, 0, stats.upload_cap, stats.download_cap, stats.upload_rate, stats.download_rate, stats.num_active_peers);
		else
			m_swarm_state.update(pid, 0, 1, stats.upload_cap, stats.download_cap, stats.upload_rate, stats.download_rate, stats.num_active_peers);
	}

	return true;
}

void PerISPPeerDistribution::addPeersFromManager()
{
	/* Lock the peers collection in the manager */
	p4p::detail::ScopedSharedLock mgr_lock(m_manager->m_peers_mutex);

	/* Lock our own peers collection */
	p4p::detail::ScopedExclusiveLock lock(m_peers_mutex);

	/* Clear existing mappings between peers and PIDs. They will
	 * be re-allocated on-demand when adding individual peers. */
	m_peer_to_pid = PeerPIDMap();
	m_pid_peers = PIDPeerMap();
	m_swarm_state = SwarmState();

	/* Iterate through all peers, look up their new PIDs and re-add them */
	for (PeerDistributionManager::PeerStatsMap::const_iterator itr = m_manager->m_peers.begin();
		itr != m_manager->m_peers.end();
		++itr)
	{
		P4PPeerBase* peer = itr->first;
		const PeerDistributionManager::PeerStats& stats = itr->second;

		/* Lookup the new PID for the peer. See note in addPeer()
		 * concerning what happens if the lookup result is an error.
		 */
		int pid = m_isp->lookup(peer->getIPAddress());
		if (pid < 0)
			pid = ERR_UNKNOWN_PID;

		/* Add peer with the new PID */
		addPeerInternal(peer, pid, stats);

		/* If we are the Home ISP for the peer, update its home PID as well */
		if (peer->getHomeISP() == m_isp)
			peer->setHome(m_isp, pid);
	}
}

void PerISPPeerDistribution::updatePeerDistribution()
{
	addPeersFromManager();
}

bool PerISPPeerDistribution::removePeer(const P4PPeerBase* peer)
{
	p4p::detail::ScopedExclusiveLock lock(m_peers_mutex);

	/* Find the PID for the peer. Ignore removal
	 * if we don't know about the peer. */
	PeerPIDMap::iterator peer_pid_itr = m_peer_to_pid.find(peer);
	if (peer_pid_itr == m_peer_to_pid.end())
		return false;

	/* Save the PID */
	int pid = peer_pid_itr->second;

	/* Remove entry in Peer->PID mapping */
	m_peer_to_pid.erase(peer_pid_itr);

	/* Remove peer from the set of peers for the PID. */
	if (pid < 0)
		pid = m_pid_peers.size() - 1;

	PeerPtrCollection& pid_peers = m_pid_peers[pid];
	pid_peers.erase(peer);

	/* If this was the last peer, we no longer need to have memory
	 * allocated to store peers amongst the PIDs */
	if (m_peer_to_pid.empty())
	{
		m_swarm_state = SwarmState();
		m_pid_peers = PIDPeerMap();
	}

	return true;
}

bool PerISPPeerDistribution::updatePeerStats(const P4PPeerBase* peer,
				  bool is_seed_changed,
				  float upload_cap_change,
				  float download_cap_change,
				  float upload_rate_change,
				  float download_rate_change,
				  int active_peers_change)
{
	p4p::detail::ScopedExclusiveLock lock(m_peers_mutex);

	/* Determine which PID the peer was in */
	PeerPIDMap::const_iterator itr = m_peer_to_pid.find(peer);
	if (itr == m_peer_to_pid.end())
		return false;
	int pid = itr->second;

	/* Update swarm statistics */
	if (is_seed_changed)
		m_swarm_state.update(pid, 1, -1, upload_cap_change, download_cap_change, upload_rate_change, download_rate_change, active_peers_change);
	else
		m_swarm_state.update(pid, 0, 0, upload_cap_change, download_cap_change, upload_rate_change, download_rate_change, active_peers_change);

	return true;
}

p4p::app::SwarmState PerISPPeerDistribution::getSwarmState() const
{
	p4p::detail::ScopedSharedLock lock(m_peers_mutex);
	return m_swarm_state;
}

bool PerISPPeerDistribution::listPIDPeers(int pid, std::vector<const P4PPeerBase*>& out_peers) const
{
	/* Do nothing if PID not found */
	if (pid < 0)
		pid = m_pid_peers.size() - 1;
	if ((unsigned int)pid >= m_pid_peers.size())
		return false;

	/* Copy to results */
	const PeerPtrCollection& pid_peers = m_pid_peers[pid];
	unsigned int prev_size = out_peers.size();
	out_peers.resize(prev_size + pid_peers.size());
	if (!pid_peers.empty())
		memcpy(&out_peers[prev_size], &pid_peers[0], pid_peers.size() * sizeof(pid_peers[0]));

	return true;
}
	
void PerISPPeerDistribution::listIntraISPPeers(std::vector<const P4PPeerBase*>& out_peers) const
{
	/* Add peers in each internal PID to results */
	unsigned int numIntraISPPIDs;
	m_isp->getNumPIDs(&numIntraISPPIDs, NULL);
	for (unsigned int i = 0; i < numIntraISPPIDs; ++i)
		listPIDPeers(i, out_peers);
}
	
void PerISPPeerDistribution::listExternalPeers(std::vector<const P4PPeerBase*>& out_peers) const
{
	unsigned int numIntraISPPIDs, numTotalPIDs;
	m_isp->getNumPIDs(&numIntraISPPIDs, &numTotalPIDs);

	/* Add peers in each external PID to results */
	for (unsigned int i = numIntraISPPIDs; i < numTotalPIDs; ++i)
		listPIDPeers(i, out_peers);
}

const P4PPeerBase* PerISPPeerDistribution::getRandomPeer(int pid) const
{
	if (pid < 0)
		pid = m_pid_peers.size() - 1;
	if ((unsigned int)pid >= m_pid_peers.size())
		return NULL;

	const PeerPtrCollection& pid_peers = m_pid_peers[pid];
	if (pid_peers.empty())
		return NULL;

	return pid_peers[rand() % pid_peers.size()];
}

const P4PPeerBase* PerISPPeerDistribution::getRandomPeer(const std::vector<int> pids) const
{
	if( pids.empty() ) 
		return NULL;

	//FIXME: generate i accoridng to each pid's size
	int i = rand() % pids.size();

	return getRandomPeer(pids[i]);
}

int PerISPPeerDistribution::getNumPeers(int pid) const
{
	if (pid < 0)
		pid = m_pid_peers.size() - 1;
	if ((unsigned int)pid >= m_pid_peers.size())
		return -1;

	return m_pid_peers[pid].size();
}

};
};
};
