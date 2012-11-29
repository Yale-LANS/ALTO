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

#ifndef PID_PEER_SELECTION_HELPER_H
#define	PID_PEER_SELECTION_HELPER_H

#include <vector>
#include <set>
#include <p4p/detail/mutex.h>
#include <p4p/app/detail/perisp_peer_distribution.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {
namespace detail {

//! Helper class to select peers from a PID and avoid duplicate selections
/**
 * Peer selection with P4P commonly involves selecting random peers from
 * a single PID.  Depending on the total number of peers within a PID, different
 * algorithms may be better.  For example:
 * - If the list of peers is "small" relative to the number of peers we're selecting,
 *   then we do the following:
 *   -# Copy the list of peers in the PID to a vector (only at initialization)
 *   -# Select a random peer from the vector, and save reference to it
 *   -# Swap it with the back element
 *   -# Pop the back element
 *   -# Return reference to the selected peer
 * - Otherwise, if the list of peers is "large" relative to the number of peers we're
 *   selecting, then we do the following:
 *   -# Initialize a set of previously-selected peers (only at initialization)
 *   -# Select random peer from the PID (without removing it)
 *   -# Store peer in temporary collection.  If peer already exists (was already selected),
 *      select a new one
 *   -# Return the selected peer
 *
 * This class exposes a simple interface for peer selection, and the tradeoff
 * between these different algorithms is handled internally.
 */
class PIDPeerSelectionHelper
{
public:
	/**
	 * Constructor
	 *
	 * @param isp_peers Peer distribution for a single ISP.  It is assumed that
	 *   this instance has been returned by PeerDistributionManager::getPeerDistribution()
	 *   and has already been locked for read access by calling
	 *   PerISPPeerDistribution::lock().
	 * @param num_pids Number of total PIDs
	 * @param num_total_peers Estimate of total number of peers that will be selected.  This is
	 *   internally used to determine which peer selection strategy will be
	 *   used.
	 */
	PIDPeerSelectionHelper(const PerISPPeerDistribution* isp_peers,
			    unsigned int num_pids,
			    unsigned int num_total_peers);

	/**
	 * Select a random peer from a PID
	 *
	 * @param pid PID Index from which peer should be selected. Note that PID can also
	 *   be ERR_UNKNOWN_PID to indicate that a random peer without an assigned PID should
	 *   be selected (e.g., if the ISP does not define a PID).
	 * @returns Returns selected peer, or NULL if selection failed (e.g., invalid
	 *   PID or no peers left in the PID).
	 */
	const P4PPeerBase* selectPeer(int pid);

	/**
	 * Select a random peer from a group of PIDs
	 *
	 * @param pids PID Index Vector from which peer should be selected. Note that 
	 *   ERR_UNKNOWN_PID can also be included to indicate that a random peer without 
	 *   an assigned PID can be selected 
	 * @returns Returns selected peer, or NULL if selection failed (e.g., invalid
	 *   PID or no peers left in the PID).
	 */
	const P4PPeerBase* selectPeer(const std::vector<int> pids);

private:

	/** Indicates the peer selection strategy used for a single PID. */
	enum Strategy
	{
		ST_UNKNOWN,	/**< Strategy not yet determined */
		ST_COPY,	/**< Strategy 1: Copy peers to vector (small number of peers in PID) */
		ST_NOCOPY,	/**< Strategy 1: Don't copy to vector (large number of peers in PID) */
	};

	/** Internal storage for per-PID state */
	struct PIDInfo
	{
		/** Random acce */
		typedef std::vector<const P4PPeerBase*> PeerList;
		typedef std::set<const P4PPeerBase*> PeerSet;

		/** Constructor */
		PIDInfo() : strategy(ST_UNKNOWN) {}

		int numPeers;		/**< Number of peers remaining in this PID */
		Strategy strategy;	/**< Strategy in use for this PID */
		PeerList candidates;	/**< Candidate peers remaining (used if strategy == ST_COPY) */
		PeerSet selected;	/**< Selected peers (used if strategy == ST_NOCOPY) */
	};

	const PerISPPeerDistribution* m_isp_peers;	/**< ISP's peer distribution */
	unsigned int m_total_peers;			/**< Total number of peers being selected */
	std::vector<PIDInfo> m_pidinfo;			/**< Vector of per-PID information */
};

}; //namespace detail
}; //namespace app
}; //namespace p4p

#endif	

