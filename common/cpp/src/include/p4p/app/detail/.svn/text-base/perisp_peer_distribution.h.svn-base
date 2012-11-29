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


#ifndef P4P_API_ISP_VIEW_H
#define P4P_API_ISP_VIEW_H

#include <set>
#include <vector>
#include <map>
#include <p4p/ip_addr.h>
#include <p4p/isp.h>
#include <p4p/app/peer.h>
#include <p4p/app/swarm_state.h>
#include <p4p/app/peer_distribution_manager.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/random_access_set.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {
namespace detail {

//! Maintain a collection of P4PPeerBase objects for a single ISP's my-Internet view.
/**
 * Peers may be added (e.g., when they join a channel/swarm) and removed (e.g., when
 * the leave a channel/swarm).  This class implments the common case of grouping
 * peers by their PID.
 *
 * NOTE: All operations provided by this class are thread-safe.
 */
class p4p_common_cpp_EXPORT PerISPPeerDistribution
{
public:
	/**
	 * Constructor
	 * @param manager The parent PeerDistributionManager
	 * @param isp The ISP for which the my-Internet view of the peers is maintained
	 * NOTE: The ISP object must not be freed during the lifetime of
	 *       this PerISPPeerDistribution object.
	 */
	PerISPPeerDistribution(const p4p::app::PeerDistributionManager* manager, const p4p::ISP* isp);

	/**
	 * Destructor
	 */
	~PerISPPeerDistribution();

	/**
	 * Get the ISP object for this view.
	 */
	const p4p::ISP* getISP() const	{ return m_isp; }

	/**
	 * Add peer to the my-Internet view. The peer's PID is specified
	 * as a parameter (e.g., if it has already been looked-up).
	 *
	 * @param peer Peer to be added.
	 * @param pid Index of PID containing the peer. It is assumed this
	 *   PID has already been looked up in the ISP's PID Map.
	 * @returns Returns false if the peer was already contained in
	 *   the view, and true if it was successfully added.
	 * NOTE: The peer object must not be freed either until
	 *       it is first removed using the removePeer() method.
	 */
	bool addPeer(P4PPeerBase* peer, int pid);

	/**
	 * Remove peer from the my-Internet view.
	 *
	 * @param peer Peer to be removed.
	 * @returns Returns true of peer was removed, and false otherwise (e.g., it
	 *   was not contained in the view).
	 */
	bool removePeer(const P4PPeerBase* peer);

	/**
	 * Update statistics for a peer.
	 *
	 * @param peer Peer whose statistics should be updated.
	 * @param is_seed_changed True of seeder status has changed (moved from a leecher to a seeder)
	 * @param upload_cap_change Change in upload capacity
	 * @param download_cap_change Change in download capacity
	 * @param upload_rate_change Change in upload rate
	 * @param download_rate_change Change in download rate
	 * @param active_peers_change Change in number of active peers
	 * @returns Returns True if peer was successfully updated, or false otherwise (e.g., it was
	 *   not previously added with addPeer() method)
	 */
	bool updatePeerStats(const P4PPeerBase* peer,
			     bool is_seed_changed,
			     float upload_cap_change,
			     float download_cap_change,
			     float upload_rate_change,
			     float download_rate_change,
			     int active_peers_change);

	/**
	 * Get the current swarm state
	 */
	p4p::app::SwarmState getSwarmState() const;

	/**
	 * Acquire a shared lock for this data structure.  This function MUST
	 * be called before calling any of the following functions:
	 * - listPIDPeers()
	 * - listIntraISPPeers()
	 * - listExternalPeers()
	 * - getRandomPeer()
	 * - getNumPeers()
	 *
	 * When you are finished reading data from this data structure using
	 * any of the above methods, you MUST call the unlock() method.  Other
	 * methods of this class may block until unlock() is called.
	 *
	 * The reason for providing lock() and unlock() is that the above
	 * methods may need to be called many times for performing peer selection,
	 * and it may be too much overhead to perform the necessary locking and
	 * unlocking when calling them each time.
	 */
	void lock() const;

	/**
	 * Release a shared lock for this data structure. This function MUST
	 * be called after calling lock().  See lock() for more details.
	 */
	void unlock() const;

	/**
	 * List the peers in a PID.
	 *
	 * @param pid PID index.
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 * @returns Returns true if PID was found successfully, and false otherwise.
	 */
	bool listPIDPeers(int pid, std::vector<const P4PPeerBase*>& out_peers) const;
	
	/**
	 * List peers in intra-ISP PIDs.
	 *
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 */
	void listIntraISPPeers(std::vector<const P4PPeerBase*>& out_peers) const;

	/**
	 * List peers in external PIDs.
	 *
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 */
	void listExternalPeers(std::vector<const P4PPeerBase*>& out_peers) const;
	
	/**
	 * Select a random peer from a PID.
	 *
	 * @param pid PID index.
	 * @returns Returns NULL if PID index is not valid or does not contain any peers, and
	 *   a pointer to a peer otherwise.
	 */
	const P4PPeerBase* getRandomPeer(int pid) const;

	/**
	 * Select a random peer from a group of PIDs.
	 *
	 * @param pids PID index vector.
	 * @returns Returns NULL if PID index is not valid or does not contain any peers, and
	 *   a pointer to a peer otherwise.
	 */
	const P4PPeerBase* getRandomPeer(const std::vector<int> pids) const;

	/**
	 * Get the number of peers in a PID.
	 *
	 * @param pid PID Index.
	 * @returns Returns -1 if PID is invalid, and number of peers in PID otherwise.
	 */
	int getNumPeers(int pid) const;

	/**
	 * Update peer distributions according to ISP's current PID Map. This should be called
	 * when the ISP's PID Map has changed.
	 */
	void updatePeerDistribution();

private:

	/**
	 * Collection of peer pointers. Use a customized container that
	 * allows random access in constant time, and insertion and
	 * deletion in logarithmic time.
	 */
	typedef p4p::detail::RandomAccessSet<const P4PPeerBase*> PeerPtrCollection;

	/**
	 * Mapping from peers to their corresponding PID.
	 */
	typedef std::map<const P4PPeerBase*, int> PeerPIDMap;

	/**
	 * Mapping from PID to the peers within the PID.
	 */
	typedef std::vector<PeerPtrCollection> PIDPeerMap;

	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	PerISPPeerDistribution(const PerISPPeerDistribution& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	PerISPPeerDistribution& operator=(const PerISPPeerDistribution& rhs) { return *this; }

	/**
	 * Add peers existing in the parent PeerDistributionManager. This is
	 * used when initializing the view, and also when the PID Map
	 * has been changed.
	 */
	void addPeersFromManager();

	/**
	 * Logic for adding a peer to the internal data structures. Same
	 * logic is used by both addPeer() and update().
	 */
	bool addPeerInternal(P4PPeerBase* peer, int pid, const p4p::app::PeerDistributionManager::PeerStats& stats);

	const p4p::app::PeerDistributionManager* m_manager;	/**< Parent PeerDistributionManager */
	const p4p::ISP* m_isp;					/**< ISP for which we are maintaining the view */
	p4p::detail::SharedMutex m_peers_mutex;			/**< Mutex protecting peers collection */
	PeerPIDMap m_peer_to_pid;				/**< Map from peers to their PIDs */
	PIDPeerMap m_pid_peers;					/**< Map from PID to peers within the PID */
	SwarmState m_swarm_state;				/**< Current PID-level stats */
};

}; // namespace detail
}; // namespace app
}; // namespace p4p

#endif

