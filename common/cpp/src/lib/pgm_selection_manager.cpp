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

#include <iostream>

#include "p4p/app/pgm_selection_manager.h"

#include <p4p/app/peering_guidance_matrix_manager.h>
#include <p4p/app/detail/perisp_peer_distribution.h>
#include <p4p/app/detail/pid_peer_selection_helper.h>
#include <p4p/app/errcode.h>
#include <p4p/logging.h>

#include <stdexcept>

namespace p4p {
namespace app {

/* Global peering guidance matrix manager. This instance is used if not explicitly
 * overridden in the PGMSelectionManager constructor (currently such a constructor
 * is not provided).
 */
static PeeringGuidanceMatrixManager GLOBAL_PGM_MANAGER;

const time_t PGMSelectionManager::TIME_INVALID = ~((time_t)0);

PGMSelectionManager::PGMSelectionManager()
	: m_guidance_initialized(false),
	  m_pgm_mgr(&GLOBAL_PGM_MANAGER)
{

}

PGMSelectionManager::PGMSelectionManager(const ISPManager* isp_mgr)
	: m_guidance_initialized(false),
	  m_pgm_mgr(&GLOBAL_PGM_MANAGER)
{
	if (!isp_mgr)
		throw std::invalid_argument("isp_mgr must not be NULL");
	setISPManager(isp_mgr);
}

PGMSelectionManager::~PGMSelectionManager()
{
	/* Cleanup all guidance matrices */
	for (ISPGuidanceMap::iterator itr = m_guidance.begin(); itr != m_guidance.end(); ++itr)
		m_pgm_mgr->releaseGuidanceMatrix(itr->second);
}

bool PGMSelectionManager::setISPManager(const ISPManager* isp_mgr)
{
	/* Be sure we tell our parent class about the ISPManager as well */
	if (!P4PSelectionManager::setISPManager(isp_mgr))
		return false;

	/* Initialize map of peering guidance matrices using the current ISPs. Also
	 * add each ISP to the PeerDistributionManager so it can manage peers according
	 * to the ISP. */
	std::vector<ISP*> isp_list;
	m_isp_mgr->listISPs(isp_list);

	for (unsigned int i = 0; i < isp_list.size(); ++i)
		m_guidance[isp_list[i]] = NULL;

	return true;
}

void PGMSelectionManager::setDefaultOptions(const PeeringGuidanceMatrixOptions& options)
{
	p4p::detail::ScopedExclusiveLock lock(m_guidance_mutex);
	m_guidance_opts_default = options;
}

PeeringGuidanceMatrixOptions PGMSelectionManager::getDefaultOptions() const
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);
	return m_guidance_opts_default;
}

int PGMSelectionManager::setISPOptions(const ISP* isp, const PeeringGuidanceMatrixOptions& options)
{
	p4p::detail::ScopedExclusiveLock lock(m_guidance_mutex);

	if (m_guidance.find(isp) == m_guidance.end())
		return ERR_INVALID_ISP;

	m_guidance_opts[isp] = options;
	return 0;
}

int PGMSelectionManager::clearISPOptions(const ISP* isp)
{
	p4p::detail::ScopedExclusiveLock lock(m_guidance_mutex);

	if (m_guidance.find(isp) == m_guidance.end())
		return ERR_INVALID_ISP;

	m_guidance_opts.erase(isp);
	return 0;
}

int PGMSelectionManager::getEffectiveISPOptions(const ISP* isp, PeeringGuidanceMatrixOptions& out_options) const
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	if (m_guidance.find(isp) == m_guidance.end())
		return ERR_INVALID_ISP;

	out_options = getConfiguredOptions(isp);
	return 0;
}

bool PGMSelectionManager::initGuidance(const std::string& optEngineAddr, unsigned short optEnginePort)
{
	if (m_guidance_initialized)
		return false;

	/* Assign optimization engine address/port and run through normal update procedure */
	m_optengine_addr = optEngineAddr;
	m_optengine_port = optEnginePort;
	
	m_guidance_initialized = true;

	computeGuidance();
	return true;
}

const PeeringGuidanceMatrix* PGMSelectionManager::getGuidanceMatrix(const ISP* isp)
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	/* Return error on invalid ISP */
	ISPGuidanceMap::iterator itr = m_guidance.find(isp);
	if (itr == m_guidance.end())
		return NULL;

	/* Return error if guidance matrix doesn't yet exist */
	if (!itr->second)
		return NULL;

	/* Increment reference count for the returned guidance matrix. Caller
	 * is responsible for calling releaseGuidanceMatrix() to decrement
	 * the reference count. */
	if (!m_pgm_mgr->addGuidanceMatrixRef(itr->second))
		throw std::runtime_error("Illegal state: failed to increment reference count for guidance matrix");

	return itr->second;
}

bool PGMSelectionManager::releaseGuidanceMatrix(const PeeringGuidanceMatrix* matrix)
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	return m_pgm_mgr->releaseGuidanceMatrix(matrix);
}

#if 0
/* NOTE: Currently commented-out since functionality not fully implemented */
bool PGMSelectionManager::initGuidance()
{
	if (m_guidance_initialized)
		return false;

	/* Clear optimization engine address/port and run through normal update procedure */
	m_optengine_addr.clear();
	m_optengine_port = 0;

	m_guidance_initialized = true;

	computeGuidance();
	return true;
}
#endif

void PGMSelectionManager::computeGuidance()
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	/* Iterate over the current set of ISPs. computeGuidanceInternal()
	 * does not insert/remove items from the map, so it is safe to
	 * iterate here. */
	for (ISPGuidanceMap::iterator itr = m_guidance.begin(); itr != m_guidance.end(); ++itr)
		computeGuidanceInternal(itr->first);
}

int PGMSelectionManager::computeGuidance(const ISP* isp)
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	return computeGuidanceInternal(isp);
}

time_t PGMSelectionManager::getNextComputeTime(const ISP* isp) const
{
	p4p::detail::ScopedSharedLock lock(m_guidance_mutex);

	ISPGuidanceMap::const_iterator itr = m_guidance.find(isp);
	if (itr != m_guidance.end())
		return ~(time_t)0;

	return itr->second->getNextComputeTime();
}

PeeringGuidanceMatrixOptions PGMSelectionManager::getConfiguredOptions(const ISP* isp) const
{
	/* Check if there are specific guidance options for the ISP. */
	ISPGuidanceOptionMap::const_iterator itr = m_guidance_opts.find(isp);
	if (itr != m_guidance_opts.end())
		return itr->second;

	/* If not, return the default options */
	return m_guidance_opts_default;
}

int PGMSelectionManager::computeGuidanceInternal(const ISP* isp)
{
	/* Get configured options for this ISP */
	PeeringGuidanceMatrixOptions options = getConfiguredOptions(isp);

	/* Locate existing guidance matrix. It is a programming error
	 * for the ISP to not have an entry in the map of guidance matrics. */
	ISPGuidanceMap::iterator itr = m_guidance.find(isp);
	if (itr == m_guidance.end())
		throw std::runtime_error("Illegal state: no guidance matrix map entry exists for desired ISP");

	if (!itr->second)
	{
		/* No existing guidance matrix exists; create one */
		itr->second = m_pgm_mgr->getGuidanceMatrix(isp, options, m_optengine_addr, m_optengine_port);
	}
	else
	{
		/* Adjust options for guidance matrix. Note that the PeeringGuidanceMatrixManager
		 * will do nothing if options have not changed (and it will return the existing
		 * matrix) so we do not need to check here. */
		itr->second = m_pgm_mgr->setOptions(itr->second, options);
	}

	const PeeringGuidanceMatrix* matrix = itr->second;

	/* Recompute the matrix */
	if (matrix->getOptions().isSwarmDependent())
	{
		/* Report that no guidance is available if there is no swarm state available */
		SwarmState swarm_state = m_isp_peers.getSwarmState(matrix->getISP());
		if (swarm_state.getNumPIDs() == 0)
			return ERR_GUIDANCE_UNAVAILABLE;

		return m_pgm_mgr->compute(matrix, swarm_state);
	}
	else
	{
		/* Matrix is not swarm-dependent; just call compute */

		return m_pgm_mgr->compute(matrix);
	}
}

static void fillPeers(std::vector<int>& pids,				/* PID Indexes */
		      std::vector<unsigned int>& peercount,		/* Number of peers in the PID */
		      std::vector<double>& weights,			/* Weight for each PID */
		      unsigned int max_peers,				/* Number of peers to select */
		      detail::PIDPeerSelectionHelper& peer_sel_helper,	/* Helper for random peer selection */
		      std::vector<const P4PPeerBase*>& out_peers)
{
	/* Skip if no PIDs */
	if (pids.empty())
		return;

	/* Fail to fill if vectors mismatch */
	if (pids.size() != peercount.size() || pids.size() != weights.size())
		return;

	/* Compute total available peers */
	unsigned int total_peercount = 0;
	for (unsigned int i = 0; i < peercount.size(); ++i)
		total_peercount += peercount[i];

	/* Compute total weight (cummulative probability) */
	double total_weight = 0.0;
	for (unsigned int i = 0; i < weights.size(); ++i)
		total_weight += weights[i];

	do
	{
		/* Choose a random number r and determine which PID r falls into */
		double r = (rand() / (double)RAND_MAX) * total_weight;
		unsigned int j = 0;
		while (r >= weights[j] && j < (weights.size() - 1))
		{
			r -= weights[j];
			++j;
		}

		/* Randomly select peer from the bin */
		// FIXME: Ask Hongqiang why the 'throw' was commented out and we were inserting null peers into the vector
		const P4PPeerBase* peer = peer_sel_helper.selectPeer(pids[j]);
		if (peer == NULL)
			throw std::runtime_error("illegal state: expected to retrieve non-null peer");

		out_peers.push_back(peer);
		P4P_LOG_TRACE("Selected peer from PID " << pids[j] << ": " << peer->getIPAddress());
		peercount[j] -= 1;
		total_peercount -= 1;

		/* If empty bin or fail to select, skip in subsequent runs */
		if (peercount[j] <= 0 || peer == NULL)
		{
			total_weight -= weights[j];
			weights[j] = 0;
		}
	} while (out_peers.size() < max_peers && total_peercount > 0);
}

static void logSelectedPeers(const PGMSelectionManager* mgr, const char* alg, const ISP* isp, const P4PPeerBase* peer, const std::vector<const P4PPeerBase*>& out_peers, int offset)
{
	if (!isLogEnabled(LOG_DEBUG))
		return;

	std::ostringstream selected_peers_str;
	selected_peers_str << '[';
	if (out_peers.size() > 0)
	{
		unsigned int i;
		for(i = (unsigned int) offset; i < out_peers.size() - 1; i++)
			selected_peers_str << '{' << out_peers[i] << ',' << out_peers[i]->getIPAddress() << "},";
		selected_peers_str << '{' << out_peers[i] << ',' << out_peers[i]->getIPAddress() << '}';
	}
	selected_peers_str << "]";
	P4P_LOG_DEBUG("event:selected_peers,sel_mgr:" << mgr << ",alg:" << alg << ",isp:" << isp << ",peer:{" << peer << ',' << peer->getIPAddress() << "},selected:" << selected_peers_str);
}

int PGMSelectionManager::selectPeersLocationOnly(const P4PPeerBase* peer, const ISP* home_isp, int home_pid, unsigned int num_peers, std::vector<const P4PPeerBase*>& out_peers)
{
	/* Get the peer distribution manager for the ISP */
	const detail::PerISPPeerDistribution* isp_peers = m_isp_peers.getPeerDistribution(home_isp);
	if (!isp_peers)
		return ERR_GUIDANCE_UNAVAILABLE;
	
	/* We will be reading from the peer distribution, so we must first acquire a
	 * read lock. This will be released before we exit. */
	logTrace("Locking peers for ISP");
	isp_peers->lock();
	logTrace("Locked peers for ISP");
	std::vector<const P4PPeerBase*> internal_peers, external_peers;
	isp_peers->listIntraISPPeers(internal_peers);
	isp_peers->listExternalPeers(external_peers);
	if (isLogEnabled(LOG_TRACE))
	{
		logTrace("Num Internal Peers: %u", (unsigned int)internal_peers.size());
		for (unsigned int i = 0; i < internal_peers.size(); ++i)
			P4P_LOG_TRACE("\t" << internal_peers[i]->getIPAddress() << " (PID: " << internal_peers[i]->getHomePID() << ")");

		logTrace("Num External Peers: %u", (unsigned int)external_peers.size());
		for (unsigned int i = 0; i < external_peers.size(); ++i)
			P4P_LOG_TRACE("\t" << external_peers[i]->getIPAddress() << " (PID: " << external_peers[i]->getHomePID() << ")");
	}
	/* Reserve space in output list */
	int num_prefilled = out_peers.size();
	out_peers.reserve(num_peers + num_prefilled + 1);

	/* Distinguish intra-ISP PIDs and external PIDs */
	unsigned int num_intraisp_pids, num_total_pids;
	home_isp->getNumPIDs(&num_intraisp_pids, &num_total_pids);

	/* Cannot select peers for external peer */
	if ((unsigned int)home_pid >= num_intraisp_pids)
		return ERR_GUIDANCE_UNAVAILABLE;

	/* Retrieve intra-PID peer selection percentage and intra-ISP percentage */
	double intrapid_pct = getConfiguredOptions(home_isp).getDefaultIntraPIDPercentage();
	double intraisp_pct = getConfiguredOptions(home_isp).getDefaultIntraISPPercentage();
	
	int num_internal_pids = 0;
	std::vector<PID> pids;
	home_isp->listPIDs(pids);
	for (unsigned int i = 0; i < pids.size(); i++)
	{
		if (!pids[i].get_external())
			num_internal_pids ++;
	}
	
	if ( intrapid_pct > intraisp_pct || num_internal_pids == 1)
		intrapid_pct = intraisp_pct;

	/* Create helper object for peer selection. See PIDPeerSelectionHelper for
	 * details about why we need this. */
	detail::PIDPeerSelectionHelper peer_sel_helper(isp_peers, num_total_pids, num_peers + 1);

	/* Select intra-PID peers */
	/* TODO:
	 * Select LAN peers first before other intra-PID peers
	 */
	unsigned int num_intrapid = (unsigned int)(num_peers * intrapid_pct + 0.5);
	if (isLogEnabled(LOG_TRACE))
	{
		logTrace("Num pre-filled peers: %u", (unsigned int)out_peers.size());
		logTrace("Num Total Desired Peers: %u", num_peers);
		logTrace("Home-PID: %d", home_pid);
		logTrace("Intra-PID Percentage: %lf", intrapid_pct);
		logTrace("Max Intra-PID peers: %u", num_intrapid);
	}

	unsigned int num_intrapid_selected = 0;
	while(num_intrapid_selected < num_intrapid)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(home_pid);
		if (!cand_peer){
			logTrace("Cannot find any more peers in Home PID");
			break;
		}

		/* Avoid selecting self */
		if (cand_peer == peer)
			continue;

		out_peers.push_back(cand_peer);
		++num_intrapid_selected;

		P4P_LOG_TRACE("New Home-PID peer: " << cand_peer->getIPAddress());
	}

	/* Select intra-ISP peers */
	unsigned int num_intraisp = (unsigned int)(num_peers * intraisp_pct + num_prefilled - out_peers.size() + 0.5);
	if (isLogEnabled(LOG_TRACE))
	{
		logTrace("Intra-ISP Percentage: %lf", intraisp_pct);
		logTrace("Max Intra-ISP peers: %u", num_intraisp);
	}

	std::vector<int> intra_isp_pids;
	for(unsigned int i = 0; i < num_intraisp_pids; i++ )
		if (i != (unsigned int)home_pid)
			intra_isp_pids.push_back(i);

	for(unsigned int num_intraisp_selected = 0; num_intraisp_selected < num_intraisp; num_intraisp_selected++)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(intra_isp_pids);
		if (!cand_peer) {
			logTrace("Cannot find any more peers within ISP");
			break;
		}

		out_peers.push_back(cand_peer);

		P4P_LOG_TRACE("New Intra-ISP peer: " << cand_peer->getIPAddress());
	}

	/* Select external peers */
	unsigned int num_external = (unsigned int)(num_peers + num_prefilled - out_peers.size());
	std::vector<int> external_pids;
	for(unsigned int i = num_intraisp_pids; i < num_total_pids; i++)
		external_pids.push_back(i);

	for(unsigned int num_external_selected = 0; num_external_selected < num_external; num_external_selected++)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(external_pids);
		if (!cand_peer){
			logTrace("Cannot find any more peers in external PIDs");
			break;
		}

		out_peers.push_back(cand_peer);

		P4P_LOG_TRACE("New External peer: " << cand_peer->getIPAddress());
	}

	/* Fill in remaining slots by uniformly randomly selected peers */
	unsigned int num_random = (unsigned int)(num_peers + num_prefilled - out_peers.size()); 
	std::vector<int> all_pids;
	for(unsigned int i = 0; i < num_total_pids; i++)
		all_pids.push_back(i);
	//TODO: all_pids.push_back(UNKNOWN_PID);

	for(unsigned int num_random_selected = 0; num_random_selected < num_random; num_random_selected++)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(all_pids);
		if (!cand_peer)
			break;

		out_peers.push_back(cand_peer);
	}

	/* Cleanup */
	isp_peers->unlock();
	m_isp_peers.releasePeerDistribution(home_isp);

	logSelectedPeers(this, "location-only", home_isp, peer, out_peers, num_prefilled);

	return 0;
}

int PGMSelectionManager::selectPeers(const P4PPeerBase* peer, unsigned int num_peers, std::vector<const P4PPeerBase*>& out_peers)
{
	P4P_LOG_DEBUG("event:select_peers,IP:" << peer->getIPAddress());

	/* Extract Home ISP and Home PID of the peer. Return error if peer
	 * does not have home ISP. */
	logTrace("Locating Home ISP");
	const ISP* home_isp = peer->getHomeISP();
	if (!home_isp)
	{
		logTrace("Failed to locate Home ISP");
		return ERR_GUIDANCE_UNAVAILABLE;
	}

	P4P_LOG_TRACE("Located Home ISP with portal server " << home_isp->getLocationPortalAddr() << ':' << home_isp->getLocationPortalPort());
	
	logTrace("Locating Home PID");
	int home_pid = peer->getHomePID();
	if (home_pid < 0)
	{
		logTrace("Failed to locate Home PID");
		return ERR_GUIDANCE_UNAVAILABLE;
	}

	logTrace("Located Home PID %d", home_pid);

	/* Get information from ISP about which PIDs are intra-ISP and which are external. */
	unsigned int num_intraisp_pids, num_total_pids;
	home_isp->getNumPIDs(&num_intraisp_pids, &num_total_pids);
	logTrace("Num Intra-ISP PIDs: %u", num_intraisp_pids);
	logTrace("Num Total PIDs: %u", num_total_pids);

	/* Ensure current PID fits within PID Map. Note that this only ensures that
	 * the index is valid, NOT whether it actual matches the same PID.  This
	 * case may happen when the PID Map has changed, but the peer's PID has
	 * not yet been updated. */
	if ((unsigned int)home_pid >= num_total_pids)
	{
		logTrace("Home PID is not a valid PID");
		return ERR_GUIDANCE_UNAVAILABLE;
	}

	/* Get configured options for home ISP */
	PeeringGuidanceMatrixOptions options = getConfiguredOptions(home_isp);
	
	/* Peer selection using location-only guidance */
	if (options.getOptType() == PeeringGuidanceMatrixOptions::OPT_LOCATION_ONLY)
	{
		logTrace("Performing location-only selection");
		return selectPeersLocationOnly(peer, home_isp, home_pid, num_peers, out_peers);
	}
	
	/* Peer selection using weights */
	/* Lookup the guidance matrix and extract peering weights */
	double intrapid_pct;
	double intraisp_pct;
	std::vector<double> weights;
	{
		/* We get an exclusive lock on the set of guidance matrices. Even though
		 * we are only reading from the matrices here, we must ensure that
		 * a guidance matrix is not removed as we are reading (which could happen
		 * if the guidance matrix is configured with different options and
		 * recomputed).
		 * 
		 * Another possibility to requiring an exclusive lock, is to separate
		 * this into two different mutexes:
		 * - Mutex protecting the entries in the guidance map
		 * - Mutex protecting the guidance matrices stored in the guidance map
		 * 
		 * In such a case, we only require a shared lock on both during peer
		 * selection.  When guidance matrices are replaced (in
		 * computeGuidanceInternal()), then the second
		 * mutex would require an exclusive lock.
		 *
		 * We opt for using a single mutex to reduce resource consumption.
		 */
		p4p::detail::ScopedExclusiveLock lock(m_guidance_mutex);

		/* Return error if no guidance matrix maintained */
		ISPGuidanceMap::const_iterator itr = m_guidance.find(home_isp);
		if (itr == m_guidance.end())
			return ERR_GUIDANCE_UNAVAILABLE;

		/* Get pointer to guidance matrix. If no guidance matrix
		 * has been created yet, this will be NULL. */
		const PeeringGuidanceMatrix* guidance = itr->second;
		if (!guidance)
			return ERR_GUIDANCE_UNAVAILABLE;

		/* Extract weights from guidance matrix */
		if (guidance->getWeights(home_pid, intrapid_pct, intraisp_pct, weights) < 0)
			return ERR_GUIDANCE_UNAVAILABLE;

		/* Ensure that the number of weights provided is consistent with the
		 * current PID Map.  Note that is a rough check, and does not check
		 * whether the weights and PID Map correspond to exactly the same
		 * set of PIDs. This can happen when the PID Map has changed, but
		 * the guidance has not yet been updated. */
		if (weights.size() != num_total_pids)
			return ERR_GUIDANCE_UNAVAILABLE;
	}

	/* Get the peer distribution manager for the ISP */
	const detail::PerISPPeerDistribution* isp_peers = m_isp_peers.getPeerDistribution(home_isp);
	if (!isp_peers)
		return ERR_GUIDANCE_UNAVAILABLE;

	/* We will be reading from the peer distribution, so we must first acquire a
	 * read lock. This will be released before we exit. */
	isp_peers->lock();

	/* Filter out zero-valued weights and PIDs that contain no peers. The PIDs
	 * passing this filter (along with the number of peers and the weight for each)
	 * are put into separate vectors. */

	/* Intra-ISP PIDs */
	std::vector<int>          intraisp_pids;
	std::vector<unsigned int> intraisp_peercount;
	std::vector<double>       intraisp_pidweight;

	/* External PIDs */
	std::vector<int>          external_pids;
	std::vector<unsigned int> external_peercount;
	std::vector<double>       external_pidweight;

	/* Iterate through the PIDs */
	for (int i = 0; i < (int)num_total_pids; ++i)
	{
		/* Skip if weight is 0 */
		if (weights[i] <= 0)
			continue;

		/* Skip if PID contains no peers */
		int peercount = isp_peers->getNumPeers(i);
		if (peercount == 0)
			continue;

		/* Skip if this is the same PID as the peer's PID. Intra-PID
		 * peer selection is handled as a special case below. */
		if (i == home_pid)
			continue;

		/* Classify PID according to intradomain and interdomain */
		if (i < (int)num_intraisp_pids)
		{
			/* This is an intra-ISP PID */
			intraisp_pids.push_back(i);
			intraisp_peercount.push_back(peercount);
			intraisp_pidweight.push_back(weights[i]);
		}
		else
		{
			/* This is an external PID */
			external_pids.push_back(i);
			external_peercount.push_back(peercount);
			external_pidweight.push_back(weights[i]);
		}
	}

	/* Reserve space in output list */
	int num_prefilled = out_peers.size();
	out_peers.reserve(num_peers + num_prefilled + 1);

	/* Create helper object for peer selection. See PIDPeerSelectionHelper for
	 * details about why we need this. */
	detail::PIDPeerSelectionHelper peer_sel_helper(isp_peers, num_total_pids, num_peers + 1);

	/* Select intra-PID peers first */
	/* TODO:
	 * Select LAN peers first before other intra-PID peers
	 */
	unsigned int num_intrapid = (unsigned int)(num_peers * intrapid_pct + 0.5);
	unsigned int num_intrapid_selected = 0;
	while (num_intrapid_selected < num_intrapid)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(home_pid);
		if (!cand_peer)
			break;

		/* Avoid selecting self */
		if (cand_peer == peer)
			continue;

		out_peers.push_back(cand_peer);
		++num_intrapid_selected;

		P4P_LOG_TRACE("Selected Intra-PID peer: " << cand_peer->getIPAddress());
	}

	/* Next, select intra-ISP peers */
	fillPeers(intraisp_pids, intraisp_peercount, intraisp_pidweight,
		  (unsigned int)(intraisp_pct * num_peers + num_prefilled + 0.5),
		  peer_sel_helper,
		  out_peers);

	/* Next, select external peers */
	fillPeers(external_pids, external_peercount, external_pidweight,
		  num_peers + num_prefilled,
		  peer_sel_helper,
		  out_peers);

	/* Fill In remaining slots by uniformly randomly selected peers */
	unsigned int num_random = (unsigned int)(num_peers + num_prefilled - out_peers.size()); 
	std::vector<int> all_pids;
	for(unsigned int i = 0; i < num_total_pids; i++)
		all_pids.push_back(i);
	//TODO: all_pids.push_back(UNKNOWN_PID);

	for(unsigned int num_random_selected = 0; num_random_selected < num_random; num_random_selected++)
	{
		const P4PPeerBase* cand_peer = peer_sel_helper.selectPeer(all_pids);
		if (!cand_peer)
			break;

		out_peers.push_back(cand_peer);
	}

	/* Cleanup */
	isp_peers->unlock();
	m_isp_peers.releasePeerDistribution(home_isp);

	logSelectedPeers(this, "pgm", home_isp, peer, out_peers, num_prefilled);
	return 0;
}

};
};

