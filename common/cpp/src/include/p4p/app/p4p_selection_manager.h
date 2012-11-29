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


#ifndef P4P_PEER_SELECTION_MANAGER_P4P_H
#define P4P_PEER_SELECTION_MANAGER_P4P_H

#include <vector>
#include <p4p/isp_manager.h>
#include <p4p/app/peer.h>
#include <p4p/app/peer_selection_manager.h>
#include <p4p/app/peer_distribution_manager.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Base class for doing peer selection based on P4P information
/**
 * This class provides the basic services to manage peers within a collection.
 * of P4P-capable ISPs using the PeerDistributionManager class.  The addPeer()
 * and removePeer() methods are overridden to add members to this class. Thus,
 * when deriving from this class, it is important to first call the addPeer()
 * and removePeer() methods of this base class.
 *
 * The updatePeerStats() method is also provided to notify the selection manager
 * that a peer's statistics have changed.
 */
class p4p_common_cpp_EXPORT P4PSelectionManager : public PeerSelectionManager
{
public:

	/**
	 * Default constructor; construct P4PSelectionManager without an
	 * ISPManager.  Before peers can be added, an ISPManager must be
	 * assigned using setISPManager().
	 */
	P4PSelectionManager();

	/**
	 * Constructor 
	 *
	 * NOTE: Currently, the API only supports the case when all ISPs are
	 *   pre-configured. That is, we do not support adding ISPs to the ISPManager
	 *   after a P4PSelectionManager instance is already constructed using
	 *   this constructor.
	 *
	 * @param isp_mgr ISPManager which manages global collection of ISPs.
	 */
	P4PSelectionManager(const ISPManager* isp_mgr);

	/**
	 * Destructor
	 */
	virtual ~P4PSelectionManager();

	/**
	 * Set the ISPManager by this Selection Manager.  The ISPManager may
	 * only be assigned once during the lifetime of a Selection Manager object.
	 * Thus, this method returns false if the ISPManager has already been
	 * assigned, either by a constructor or a previous call to setISPManager().
	 *
	 * NOTE: Currently, the API only supports the case when all ISPs are
	 *   pre-configured. That is, we do not support adding ISPs to the ISPManager
	 *   after setISPManager() is called.
	 *
	 * @param isp_mgr ISPManager which manages global collection of ISPs.
	 * @returns Returns true if the ISPManager was assigned sucessfully, or
	 *   false if the ISPManager for this Selection Manager object
	 *   had already been assigned.
	 */
	virtual bool setISPManager(const ISPManager* isp_mgr);

	/**
	 * Update the peer distribution within a single ISP. This should be called
	 * when the ISP's PID Map has changed.
	 *
	 * @param isp Pointer to ISP
	 */
	virtual int updatePeerDistribution(const ISP* isp);

	/**
	 * \copydoc PeerSelectionManager::addPeer
	 *
	 * NOTE: This method also returns false if an ISPManager has not yet
	 * been configured.
	 */
	virtual bool addPeer(P4PPeerBase* peer);

	/**
	 * \copydoc PeerSelectionManager::removePeer
	 */
	virtual bool removePeer(const P4PPeerBase* peer);

	/**
	 * Update peer statistics (e.g., when peer statistics such as upload/download capacity
	 * or leecher/seed status have changed).
	 *
	 * @param peer Peer whose statistics have changed
	 * @returns Returns true if peer's stats were updated, or false if the peer
	 *   did not exist.
	 */
	bool updatePeerStats(const P4PPeerBase* peer);

protected:
	const ISPManager* m_isp_mgr;			/**< Collection of all ISPs */

	PeerDistributionManager m_isp_peers;		/**< Manages peers within ISP my-Internet views */
};

}; // namespace app
}; // namespace p4p

#endif

