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


#ifndef P4P_API_ISP_VIEW_MANAGER_H
#define P4P_API_ISP_VIEW_MANAGER_H

#include <map>
#include <p4p/ip_addr.h>
#include <p4p/isp.h>
#include <p4p/app/peer.h>
#include <p4p/app/swarm_state.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

/* Forward declaration */
namespace detail {
class PerISPPeerDistribution;
};

//! Maintain peers according to my-Internet views of a set of ISPs for a single swarm/channel
/**
 * Within a swarm (or channel), an PeerDistributionManager object maintains
 * the "my-Internet" views for all ISPs (or a subset of the ISPs).
 *
 * The addPeer() method should be called when a peer joins a swarm. The following
 * operations are done for the newly-joined peer <em>p</em>:
 * - Identify the "home" ISP and PID for peer <em>p</em>, and call P4PPeerBase::setHome()
 *   to store the home ISP and PID within <em>p</em>'s P4PPeerBase data structure.
 * - Add peer <em>p</em> to the "my-Internet" view of each ISP (including its home ISP).  When selecting peers
 *   for another peer <em>q</em>, the my-Internet view for peer <em>q</em>'s home
 *   ISP is consulted. For peer <em>q</em> to be able to select peer <em>p</em>,
 *   peer <em>p</em> must be contained in the my-Internet view of peer <em>q</em>'s ISP.
 *
 * The removePeer() method should be called when a peer leaves a swarm. This
 * removes the peer from the my-Internet view of each ISP and clears the
 * "home" ISP stored by the peer.
 *
 * NOTE: All operations provided by this class are thread-safe.
 */
class p4p_common_cpp_EXPORT PeerDistributionManager
{
public:
	/**
	 * Constructor
	 */
	PeerDistributionManager();

	/**
	 * Destructor
	 */
	~PeerDistributionManager();

	/**
	 * Returns the peer distribution manager for the requested ISP.  The returned
	 * peer distribution manager contains the peers sorted according to the my-Internet
	 * view of the requested ISP, and thus may be used for peer selection for
	 * peers within that ISP.
	 *
	 * When you are finished with the returned object, you must call releasePeerDistribution()
	 * to indicate that it is no longer needed.
	 *
	 * If you are performing an operation (such as peer selection) that makes
	 * repeated references to the peer distribution for a single ISP, it is
	 * suggested that you use the object returned by this method.  Calling
	 * the listPIDPeers(), getRandomPeer(), etc methods in this class directly
	 * have the effect of locking and unlocking the peer distribution each time
	 * they are called, which may introduce substantial overhead.
	 *
	 * If it returns successfully, this method has the side-effect of acquiring
	 * a shared lock on the collection of my-Internet views for all ISPs.  This
	 * means that calls to addISP() and removeISP() will block until you have
	 * called releasePeerDistribution().
	 *
	 * @param isp Pointer to ISP
	 * @returns Returns peer distribution manager for the requested ISP. Returns
	 *   NULL if the specified ISP was not found, or if there were no peers
	 *   available.
	 */
	const detail::PerISPPeerDistribution* getPeerDistribution(const ISP* isp) const;

	/**
	 * Release a reference to the peer distribution manager for the specified
	 * ISP.  This MUST be called when you are finished with the peer distribution
	 * manager object returned by getPeerDistribution(). See getPeerDistribution()
	 * for further details.
	 *
 	 * @param isp Pointer to ISP
	 * @returns Returns true if ISP was found successfully, and false otherwise.
	 */
	bool releasePeerDistribution(const ISP* isp) const;

	/**
	 * Assign the peer's "home" ISP and add peer to the "my-Internet" view
	 * of each ISP.
	 *
	 * After calling this method, peer->getHomeISP() will return a non-NULL
	 * value if it was determined to be within one of the ISPs.  If no ISP's
	 * PIDMap contained the peer, then peer->getHomeISP() will return NULL.
	 */
	bool addPeer(P4PPeerBase* peer);

	/**
	 * Remove peer from the "my-Internet" view of each ISP and clear the "home"
	 * ISP stored for the peer.
	 */
	bool removePeer(const P4PPeerBase* peer);

	/**
	 * Update statistics for a peer in view of each ISP. This should be
	 * called when peer's properties (e.g., upload / download capacity estimates,
	 * seed / leecher status) have changed. See PerISPPeerDistribution::updatePeerStats for
	 * more information.
	 */
	bool updatePeerStats(const P4PPeerBase* peer);

	/**
	 * List the peers within a PID of an ISP.
	 *
	 * @param isp Pointer to ISP
	 * @param pid PID index
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 * @returns Returns true if ISP and PID were found successfully, and false otherwise.
	 */
	bool listPIDPeers(const ISP* isp, int pid, std::vector<const P4PPeerBase*>& out_peers) const;
	
	/**
	 * List peers in intra-ISP PIDs of an ISP.
	 *
	 * @param isp Pointer to ISP
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 * @returns Returns true if ISP was found successfully, and false otherwise.
	 */
	bool listIntraISPPeers(const ISP* isp, std::vector<const P4PPeerBase*>& out_peers) const;

	/**
	 * List peers in external PIDs of an ISP
	 *
	 * @param isp Pointer to ISP
	 * @param out_peers Output parameter; Vector will be populated with pointers to the peers.
	 * @returns Returns true if ISP was found successfully, and false otherwise.
	 */
	bool listExternalPeers(const ISP* isp, std::vector<const P4PPeerBase*>& out_peers) const;
	
	/**
	 * Select a random peer from a PID of an ISP.
	 *
	 * @param isp Pointer to ISP
	 * @param pid PID index.
	 * @returns Returns NULL if ISP or PID index is not valid or does not contain any peers, and
	 *   a pointer to a peer otherwise.
	 */
	const P4PPeerBase* getRandomPeer(const ISP* isp, int pid) const;

	/**
	 * Get the number of peers in a PID.
	 *
	 * @param isp Pointer to ISP
	 * @param pid PID Index.
	 * @returns Returns -1 if ISP or PID is invalid, and number of peers in PID otherwise.
	 */
	int getNumPeers(const ISP* isp, int pid) const;

	/**
	 * Get current swarm state for an ISP. The swarm state provides per-PID
	 * statistics for an ISP.
	 *
	 * @param isp ISP for which statistics should be returned.
	 * @returns SwarmState with current per-PID statistics for the ISP. If the ISP
	 *   was not found (e.g., it has not been added with the addISP() method), then
	 *   a SwarmState with no PIDs is returned.
	 */
	SwarmState getSwarmState(const ISP* isp) const;

	/**
	 * Create a my-Internet view for an ISP.  Returns pointer to the newly-created
	 * PerISPPeerDistribution.  If a view already existed for the ISP, a pointer to the
	 * existing view is returned.
	 */
	bool addISP(const ISP* isp);

	/**
	 * Remove the my-Internet view for an ISP.
	 */
	void removeISP(const ISP* isp);

	/**
	 * Update peer distributions within an ISP. This should be done when an ISP's
	 * PID Map has changed.
	 *
	 * @param isp Pointer to ISP
	 * @returns Returns 0 if update completed successfully, and ERR_INVALID_ISP if
	 *   ISP was not found (e.g., it had not been added with the addISP() method).
	 */
	int updatePeerDistribution(const ISP* isp);

private:
	/**
	 * Mapping from ISP to the corresponding PerISPPeerDistribution.
	 */
	typedef std::map<const ISP*, detail::PerISPPeerDistribution*> PerISPPeerDistributionMap;

	/**
	 * Stats maintained for a peer
	 */
	struct PeerStats
	{
		PeerStats(bool is_seed_ = false,
			  float upload_cap_ = 0,
			  float download_cap_ = 0,
			  float upload_rate_ = 0,
			  float download_rate_ = 0,
			  int num_active_peers_ = 0)
			: is_seed(is_seed_),
			  upload_cap(upload_cap_),
			  download_cap(download_cap_),
			  upload_rate(upload_rate_),
			  download_rate(download_rate_),
			  num_active_peers(num_active_peers_)
		{}

		bool is_seed;
		float upload_cap;
		float download_cap;
		float upload_rate;
		float download_rate;
		int num_active_peers;
	};

	/**
	 * Set of existing peers and last peer stats
	 * remembered for them.
	 */
	typedef std::map<P4PPeerBase*, PeerStats> PeerStatsMap;

	/**
	 * PerISPPeerDistribution accesses some of our private data structures such
	 * as our master-list of peers and the PeerStats struct.
	 */
	friend class detail::PerISPPeerDistribution;

	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	PeerDistributionManager(const PeerDistributionManager& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	PeerDistributionManager& operator=(const PeerDistributionManager& rhs) { return *this; }

	/**
	 * Get pointer to the my-Internet view for an ISP.  Returns
	 * NULL if no PerISPPeerDistribution existed for the ISP. Does not do any
	 * locking.
	 */
	detail::PerISPPeerDistribution* getISPViewInternal(const ISP* isp) const;

	p4p::detail::SharedMutex m_views_mutex;		/**< Mutex protecting PerISPPeerDistribution collection */
	PerISPPeerDistributionMap m_views;		/**< Map from ISP to the corresponding distribution */

	p4p::detail::SharedMutex m_peers_mutex;		/**< Mutex protecting peers collection */
	PeerStatsMap m_peers;				/**< Set of existing peers and their last stats */
};

}; // namespace app
}; // namespace p4p

#endif

