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

#include "p4p/app/detail/pid_peer_selection_helper.h"

#include <p4p/errcode.h>

namespace p4p {
namespace app {
namespace detail {

PIDPeerSelectionHelper::PIDPeerSelectionHelper(const PerISPPeerDistribution* isp_peers,
					       unsigned int num_pids,
					       unsigned int num_total_peers)
	: m_isp_peers(isp_peers),
	  m_total_peers(num_total_peers)
{
	/* There is one entry for each PID, plus an extra standing
	 * for ERR_UNKNOWN_PID (peers without a PID). */
	m_pidinfo.resize(num_pids + 1);

	/* Count number of peers in each PID */
	for( unsigned int i = 0; i < num_pids; i++ )
		m_pidinfo[i].numPeers = m_isp_peers->getNumPeers(i);
	m_pidinfo[num_pids].numPeers = m_isp_peers->getNumPeers(ERR_UNKNOWN_PID);
}

const P4PPeerBase* PIDPeerSelectionHelper::selectPeer(int pid)
{
	/* Normalize to correct PID index. When ERR_UNKNOWN_PID is supplied,
	 * we choose the special entry at the end of the vector. */
	int pid_idx = (pid == p4p::ERR_UNKNOWN_PID)
		    ? (m_pidinfo.size() - 1)
		    : pid;

	/* Get reference to the appropriate vector entry */
	PIDInfo& pid_info = m_pidinfo[pid_idx];

	/* Choose a strategy if we haven't already */
	if (pid_info.strategy == ST_UNKNOWN)
	{
		/* Determine how many peers in the PID. Return error
		 * on invalid PID. */
		if (pid_info.numPeers < 0)
			return NULL;

		if ((unsigned int)pid_info.numPeers > 4 * m_total_peers)
		{
			/* There are a large number of peers in the pid. Use 'nocopy' strategy. */
			pid_info.strategy = ST_NOCOPY;
		}
		else
		{
			/* There are a small number of peers in the pid. Use 'copy' strategy */
			pid_info.strategy = ST_COPY;

			/* Copy to initial vector */
			if (!m_isp_peers->listPIDPeers(pid, pid_info.candidates))
				return NULL;
		}
	}

	/* Follow the desired strategy */
	if (pid_info.strategy == ST_COPY)
	{
		/* Return immediately if nothing left */
		if (pid_info.candidates.empty())
			return NULL;

		/* Choose a random element */
		unsigned int peer_idx = rand() % pid_info.candidates.size();
		const P4PPeerBase* peer = pid_info.candidates[peer_idx];

		/* Replace with the last element and pop the back element */
		pid_info.candidates[peer_idx] = pid_info.candidates.back();
		pid_info.candidates.pop_back();

		/* Consumed one peer */
		pid_info.numPeers--;

		/* Return selected peer */
		return peer;
	}
	else /* strategy == ST_NOCOPY */
	{
		/* Keep choosing a random element util we've got one that
		 * we haven't already selected. We are guaranteed this
		 * succeeds since we selected this strategy based on there
		 * being many more available clients than we needed to
		 * select overall. */
		const P4PPeerBase* peer = NULL;
		do
		{
			peer = m_isp_peers->getRandomPeer(pid);
		} while (pid_info.selected.find(peer) != pid_info.selected.end());

		/* Record that we have selected this peer */
		pid_info.selected.insert(peer);

		/* Consumed one peer */
		pid_info.numPeers--;

		/* Return selected peer */
		return peer;
	}
}

const P4PPeerBase* PIDPeerSelectionHelper::selectPeer(const std::vector<int> pids)
{
	/* Calcuate sum of peers */
	int n = pids.size();
	int sum = 0;
	for( int i = 0; i < n; i++ )
	{
		sum += m_pidinfo[pids[i]].numPeers;
	}

	if (sum <= 0) 
		return NULL;

	/* Pick a PID (pids[0]..pids[n-1]) */
	int coin = rand() % sum;
	int pick_pid = 0;
	while( pick_pid < n - 1 )
	{
		coin -= m_pidinfo[ pids[pick_pid] ].numPeers; 
		if( coin < 0 )
			break;

		pick_pid++;
	}

	/* Select peer from the selected bin */
	return selectPeer(pids[pick_pid]);
}

}; //namespace detail
}; //namespace app
}; //namespace p4p
