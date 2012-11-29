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


#ifndef P4P_PEER_SELECTION_LOCATION_H
#define P4P_PEER_SELECTION_LOCATION_H

#include <vector>
#include <p4p/isp_pidmap.h>
#include <p4p/app/peer.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! A simple function call class for P4P peer selection at client side
/**
 * Peer selection is using Location-based algorithm.
 */
class p4p_common_cpp_EXPORT P4PClientPeerSelectionLocation
{
public:
	/**
	 * Constructor
	 *
	 * @param self A peer represents the client itself, used to determine home ISP and home PID.
	 * @param pidmap_files A vector of PIDMap files stored locally.
	 * @param intra_pid_pct Intra-PID peer selection upper bound percentage.
	 * @param intra_isp_pct Intra-ISP peer selection upper bound percentage.
	 */
	P4PClientPeerSelectionLocation(P4PPeerBase *self, std::vector<std::string>& pidmap_files, const double intra_pid_pct = 0.7, const double intra_isp_pct = 0.9);

	/**
	 * Destructor
	 */
	~P4PClientPeerSelectionLocation() {}

	/**
	 * Select peers from a candidate list.
	 *
	 * @param candidates A vector of candidate peers (self is not included).
	 * @param num_to_select Maximum number of peers to be selected.  The final size of out_peers will be 
	 *   the original out_peers.size() + num_to_select
	 * @param out_peers Output parameter; vector to which pointers of selected peers will be written.
	 *   Vector will be resized to fill in the number of peers selected (potentailly the original 
	 *   out_peers.size() plus num_peers). 
	 * @returns Returns 0 on success, and a non-zero error code if peer selection failed.
	 */
	int selectPeers(std::vector<const P4PPeerBase*>& candidates, unsigned int num_to_select, std::vector<const P4PPeerBase*>& out_peers);

	/**
	 * Select NAT/LAN peers
	 *
	int selectLANPeers(std::vector<const P4PPeerBase*>& candidates, unsigned int num_to_select, std::vector<const P4PPeerBase*>& out_peers);
	 */

private:
	P4PPeerBase *m_self;
	int m_self_pid;

	ISPPIDMap *m_pidmap;
	unsigned int m_intra_isp_pids;
	unsigned int m_pids;

	double m_intra_pid_pct;
	double m_intra_isp_pct;
};

}; // namespace app
}; // namespace p4p

#endif


