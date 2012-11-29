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


#include "p4p/app/client_peer_selection_location.h"

#include <p4p/isp_pidmap.h>
#include <p4p/app/errcode.h>
#include <p4p/logging.h>

namespace p4p {
namespace app {

/**
 */
P4PClientPeerSelectionLocation::P4PClientPeerSelectionLocation(P4PPeerBase *self, std::vector<std::string>& pidmap_filenames, const double intra_pid_pct, const double intra_isp_pct):
	m_self(self),
	m_self_pid(-1),
	m_pidmap(NULL),
	m_intra_isp_pids(0),
	m_pids(0)
{
	/* Configure intra-PID and intra-ISP peer selection ratio */
	m_intra_pid_pct = (intra_pid_pct < 0.0 || intra_pid_pct > intra_isp_pct) ? m_intra_pid_pct : intra_pid_pct;
	m_intra_isp_pct = (intra_isp_pct < intra_pid_pct || intra_isp_pct > 0.99) ? m_intra_isp_pct : intra_isp_pct;

	/* Look up home ISP,
	 * assuming PIDMap files are sorted by priority in the vector
	 */
	for( unsigned int i = 0; i < pidmap_filenames.size(); i++ )
	{
		/* Construct ISP PIDMap, no other ISP information is used */
		ISPPIDMap *isp_pidmap;
		isp_pidmap = new ISPPIDMap(NULL, pidmap_filenames[i]);
		if( isp_pidmap->loadP4PInfo() != 0 )
		{
			delete isp_pidmap;
			continue;
		}

		/* Check whether this ISP is home ISP */
		int self_pid = isp_pidmap->lookup(self->getIPAddress());
		if( isp_pidmap->getPIDInfo(self_pid).get_external() == false || self_pid < 0)
		{
			m_self_pid = self_pid;
			m_pidmap = isp_pidmap;
			m_pidmap->getNumPIDs(&m_intra_isp_pids, &m_pids);
			if( m_intra_isp_pids == 1 ) 
				m_intra_pid_pct = m_intra_isp_pct;

			return;
		}

		delete isp_pidmap;
	}
}
	
/**
 * Select NAT/LAN peers 
 *
int P4PClientPeerSelectionLocation::selectLANPeers(std::vector<const P4PPeerBase*> candidates, unsigned int num_to_select, std::vector<const P4PPeerBase*>& out_peers)
{
	return 0;
}
 */

/**
 */
int P4PClientPeerSelectionLocation::selectPeers(std::vector<const P4PPeerBase*>& candidates, unsigned int num_to_select, std::vector<const P4PPeerBase*>& out_peers)
{
	/* Cannot select peers if there is no home ISP or no candidates */
	/* TODO: random selection? */
	if( m_self_pid < 0 || m_self_pid >= (int)m_intra_isp_pids || m_pidmap == NULL || candidates.empty())
		return ERR_GUIDANCE_UNAVAILABLE;

	/* First select NAT/LAN peers from candidates, the selected peers will be excluded from candidates */
	/* num_to_select -= selectLANPeers(candidates, num_to_select, out_peers); */

	/* Reserve space for output peer list */
	unsigned int num_prefilled = out_peers.size();
	out_peers.reserve(num_to_select + num_prefilled + 1);

	/* Too few candidates */
	if( candidates.size() < num_to_select )
	{
		out_peers.resize(num_prefilled + candidates.size());
		memcpy(&out_peers[num_prefilled], &candidates[0], candidates.size() * sizeof(candidates[0]));
	}

	/* Group peers into intra-PID peers, intra-ISP peers, and external peers */
	std::vector<const P4PPeerBase*> intra_pid_peers;
	std::vector<const P4PPeerBase*> intra_isp_peers;
	std::vector<const P4PPeerBase*> external_peers;

	for( unsigned int p = 0; p < candidates.size(); p++ )
	{
		const P4PPeerBase *peer = candidates[p];
		int pid = m_pidmap->lookup(peer->getIPAddress());

		if( pid == m_self_pid )
			intra_pid_peers.push_back(peer);
		else if ( pid >= 0 && pid < (int)m_intra_isp_pids)
			intra_isp_peers.push_back(peer);
		else
			external_peers.push_back(peer);
	}

	/* Select intra-PID peers */
	unsigned int num_intra_pid = (unsigned int)(num_to_select * m_intra_pid_pct + 0.5);
	unsigned int num_intra_pid_selected = 0;
	while( num_intra_pid_selected < num_intra_pid )
	{
		if( intra_pid_peers.empty()) 
			break;

		unsigned int peer_idx = rand() % intra_pid_peers.size();
		const P4PPeerBase *cand_peer = intra_pid_peers[peer_idx];

		/* Assuming self is not in candidates 
		 * if (cand_peer == peer)
		 * continue;
		 */

		out_peers.push_back(cand_peer);

		P4P_LOG_TRACE("Candidate Peer: " << cand_peer->getIPAddress());
		++num_intra_pid_selected;

		intra_pid_peers[peer_idx] = intra_pid_peers.back();
		intra_pid_peers.pop_back();
	}

	/* Select intra-ISP peers */
	unsigned int num_intra_isp = (unsigned int)(num_to_select * m_intra_isp_pct + num_prefilled - out_peers.size() + 0.5);
	unsigned int num_intra_isp_selected = 0;
	while( num_intra_isp_selected < num_intra_isp )
	{
		if( intra_isp_peers.empty()) 
			break;

		unsigned int peer_idx = rand() % intra_isp_peers.size();
		const P4PPeerBase *cand_peer = intra_isp_peers[peer_idx];

		intra_isp_peers[peer_idx] = intra_isp_peers.back();
		intra_isp_peers.pop_back();

		out_peers.push_back(cand_peer);
		++num_intra_isp_selected;
	}

	/* Select external peers */
	unsigned int num_external = (unsigned int)(num_to_select + num_prefilled - out_peers.size());
	unsigned int num_external_selected = 0;
	while( num_external_selected < num_external )
	{
		if( external_peers.empty()) 
			break;

		unsigned int peer_idx = rand() % external_peers.size();
		const P4PPeerBase *cand_peer = external_peers[peer_idx];

		external_peers[peer_idx] = external_peers.back();
		external_peers.pop_back();

		out_peers.push_back(cand_peer);
		++num_external_selected;
	}

	/* TODO: Randomly select peers back from intra-PID and intra-ISP */

	return 0;
}

}; // namespace app
}; // namespace p4p


