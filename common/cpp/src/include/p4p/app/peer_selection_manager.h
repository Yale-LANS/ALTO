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


#ifndef P4P_PEER_SELECTION_MANAGER_H
#define P4P_PEER_SELECTION_MANAGER_H

#include <vector>
#include <p4p/ip_addr.h>
#include <p4p/app/peer.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Abstract base class for managing a peer selection implementation.
/**
 * Particular peer selection implementations may derive from this base
 * class.  addPeer() and removePeer() are called when peers join and leave, respectively.
 */
class p4p_common_cpp_EXPORT PeerSelectionManager
{
public:
	/**
	 * Destructor
	 */
	virtual ~PeerSelectionManager() {}

	/**
	 * Add peer to make it available for peer selection (e.g., when the
	 * peer joins a swarm).
	 *
	 * @param peer Pointer to peer object, which inherits from P4PPeerBase
	 * @returns Returns true if the peer was added, or false if the peer already existed.
	 */
	virtual bool addPeer(P4PPeerBase* peer) = 0;

	/**
	 * Select peers for a particular peer.  
	 *
	 * @param peer Pointer to peer object for which peers are to be selected.
	 * @param num_peers Maximum number of peers to be selected.  The final size of out_peers
	 *   will be the original out_peers.size() + num_peers.
	 * @param out_peers Output parameter; vector to which pointers of selected peers will be written.
	 *   Vector will be resized to fill in the number of peers selected (potentailly the original 
	 *   out_peers.size() plus num_peers). 
	 * @returns Returns 0 on success, and a non-zero error code if peer selection failed.
	 */
	virtual int selectPeers(const P4PPeerBase* peer, unsigned int num_peers, std::vector<const P4PPeerBase*>& out_peers) = 0;

	/**
	 * Remove peer (e.g., when the peer leaves a swarm).
	 *
	 * @param peer Pointer to peer object to be removed.
	 * @returns Returns true of the peer was removed, or false if the peer did not exist.
	 */
	virtual bool removePeer(const P4PPeerBase* peer) = 0;
};

}; // namespace app
}; // namespace p4p

#endif

