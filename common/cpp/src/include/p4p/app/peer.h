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


#ifndef P4P_PEER_INFO_H
#define P4P_PEER_INFO_H

#include <stdlib.h>
#include <p4p/isp.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Interface defining necessary functionality for a peer class to be managed by the P4P Application library data structures
/**
 * This class implements primitives such as storing a reference
 * to the peer's "home" ISP (indicating which ISP's my-Internet
 * view is consulted during peer selection) and PID.
 *
 * Other methods are defined, which allow necessary statistics to
 * be extracted from a peer. These statistics are used for certain
 * types of peering guidance.
 *
 * The setHome() method is should be called to assign the
 * home ISP and PID when the peer joins. This is done
 * automatically if PeerDistributionManager or PGMSelectionManager
 * is used to maintain the set of peers.
 *
 * You should add P4PPeerBase as a base class to your existing
 * peer data structure, as in the following example:
 * \code
 * class YourPeerClass : public P4PPeerBase {
 * public:
 *	// Your constructor
 *	YourPeerClass(const std::string& id, const std::string& addr, unsigned short port)
 *		: m_id(id), m_addr(addr), m_port(port), m_downloading(true)
 *	{}
 *
 *	// Your accessor methods
 *	const std::string& getID() const		{ return m_id; }
 *	...
 *
 *	// Provide additional methods in P4PPeerBase interface
 *	virtual IPAddress getIPAddress() const		{ return m_addr; }
 *	virtual float getUploadCapacity() const		{ return m_uploadcapacity; }
 *	virtual float getDownloadCapacity() const	{ return m_downloadcapacity; }
 *	virtual bool isSeeder() const			{ return !m_downloading; }
 *
 * private:
 *	std::string m_id;		// Unique peer ID
 *	std::string m_addr;		// IP address
 *	unsigned short m_port;		// Port number
 *	bool m_downloading;		// True if peer is downloading
 *	float m_uploadcapacity;		// Estimated upload capacity (in Kbps)
 *	float m_downloadcapacity;	// Estimated download capacity (in Kbps)
 * };
 * \endcode
 */
class p4p_common_cpp_EXPORT P4PPeerBase
{
public:
	/**
	 * Constructor. ISP is initially NULL and PID is
	 * initially set to ERR_UNKNOWN_PID.
	 */
	P4PPeerBase();

	/**
	 * Destructor.
	 */
	virtual ~P4PPeerBase();

	/**
	 * Get home ISP.
	 *
	 * @returns Home ISP, or NULL if home ISP has not
	 * yet been assigned.
	 */
	const p4p::ISP* getHomeISP() const		{ return m_home_isp; }

	/**
	 * Get home PID.
	 *
	 * @returns Returns PID index of home PID, or ERR_UNKNOWN_PID
	 * if home PID has not yet been assigned.
	 */
	int getHomePID() const				{ return m_home_pid; }


	/**
	 * Set home ISP and PID. This method should be called
	 * when the peer joins.
	 */
	void setHome(const p4p::ISP* isp, int pid);

	/**
	 * Get IP Address for the peer.
	 */
	virtual IPAddress getIPAddress() const = 0;

	/**
	 * Get estimate of upload capacity of this peer in the swarm in which it is participating.
	 *
	 * @returns Estimate of upload capacity (in Kbps)
	 */
	virtual float getUploadCapacity() const		{ return 0; }

	/**
	 * Get estimate of download capacity of this peer in the swarm in which it is participating.
	 *
	 * @returns Estimate of download capacity (in Kbps)
	 */
	virtual float getDownloadCapacity() const	{ return 0; }

	/**
	 * Get current rate at which this peer is uploading data in the swarm in which it is participating.
	 *
	 * @returns Estimate of upload rate (in Kbps)
	 */
	virtual float getCurrentUploadRate() const	{ return 0; }

	/**
	 * Get current rate at which this peer is downloading data in the swarm in which it is participating.
	 *
	 * @returns Estimate of download rate (in Kbps)
	 */
	virtual float getCurrentDownloadRate() const	{ return 0; }

	/**
	 * Get an estimate of the number of active peers to which this peer is connected in the swarm. An
	 * active peer is defined as a peer with which data is being exchanged.
	 *
	 * @returns Estimate of the number of active peers
	 */
	virtual unsigned int getNumActivePeers() const	{ return 0; }

	/**
	 * Get indication whether this node is a seeder. A node is considered a seeder if it has
	 * the full content and thus does not ened to download.
	 *
	 * @returns True if peer is a seeder, false otherwise.
	 */
	virtual bool isSeeder() const			{ return false; }

private:
	const p4p::ISP* m_home_isp;
	int m_home_pid;
};

}; // namespace app
}; // namespace p4p

#endif

