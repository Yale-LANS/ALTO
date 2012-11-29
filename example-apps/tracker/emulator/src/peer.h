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


#ifndef PEER_H
#define PEER_H

#include <string>
#include <vector>
#include <netinet/ip.h>
#include <arpa/inet.h>

#ifndef NO_P4P
	#include <p4p/p4p.h>
	#include <p4p/app/p4papp.h>
	#include <p4p/app/client_peer_selection_location.h>

	using namespace p4p;
	using namespace p4p::app;
#endif

/**
 * Peer data structure inherits from P4PPeerBase
 */
class Peer
#ifndef NO_P4P
	: public P4PPeerBase
#endif
{
public:
	Peer(const std::string& id, const in_addr& addr, unsigned short port)
		: m_id(id),
		  m_addr(addr),
		  m_port(port),
		  m_upcap(0),
		  m_downcap(0),
		  m_downloading(true)
	{
	}

#ifndef NO_P4P
	Peer(const std::string& id, const in_addr& addr, unsigned short port, std::vector<std::string> pidmap_filenames, const double intra_pid_pct, const double intra_isp_pct)
		: m_id(id),
		  m_addr(addr),
		  m_port(port),
		  m_upcap(0),
		  m_downcap(0),
		  m_downloading(true)
	{
		m_p4pclient = new P4PClientPeerSelectionLocation(this, pidmap_filenames, intra_pid_pct, intra_isp_pct);
	}
#endif

	/* Accessor methods */
	const std::string& getID() const		{ return m_id; }
	const std::string& getAddr() const
	{
		if (!m_addr_string.empty())
			return m_addr_string;

		char buf[128];
		const_cast<Peer*>(this)->m_addr_string = inet_ntop(AF_INET, &m_addr, buf, 127);
		return m_addr_string;
	}

	unsigned short getPort() const			{ return m_port; }

	void setDownloading(bool value)			{ m_downloading = value; }
	bool getDownloading() const			{ return m_downloading; }

	void setCapacityStats(float upcap, float downcap)
	{
		m_upcap = upcap;
		m_downcap = downcap;
	}

	void getCapacityStats(float& upcap, float& downcap)
	{
		upcap = m_upcap;
		downcap = m_downcap;
	}

#ifndef NO_P4P
	/* additional P4PPeerBase methods */
	virtual IPAddress getIPAddress() const		{ return IPAddress(AF_INET, &m_addr); }
	virtual float getUploadCapacity() const		{ return m_upcap; }
	virtual float getDownloadCapacity() const	{ return m_downcap; }
	virtual bool isSeeder() const			{ return !m_downloading; }

	bool selectConnection(std::vector<Peer*>& candidates, unsigned int num_to_select, std::vector<Peer*>& out_peers)
	{
		std::vector<const P4PPeerBase*> p4p_candidates;
		std::vector<const P4PPeerBase*> p4p_peers;

		for(unsigned int i = 0; i < candidates.size(); i++)
			p4p_candidates.push_back((P4PPeerBase *)candidates[i]);

		if (m_p4pclient->selectPeers(p4p_candidates, num_to_select, p4p_peers) < 0)
			return false;

		out_peers.reserve(out_peers.size());
		for(unsigned int i = 0; i < p4p_peers.size(); i++)
			out_peers.push_back((Peer *)p4p_peers[i]);

		return true;
	}
#endif

private:
	std::string m_id;			/**< Globally-unique Peer ID */
	in_addr m_addr;				/**< IPv4 Address */
	std::string m_addr_string;		/**< Address as string */
	unsigned short m_port;			/**< Port number */
	float m_upcap;				/**< Upload capacity estimate (Kbps) */
	float m_downcap;			/**< Download capacity estimate (Kbps) */
	bool m_downloading;			/**< Is peer currently downloading? */

#ifndef NO_P4P
	P4PClientPeerSelectionLocation *m_p4pclient;	/**< Client-side integration */
#endif
};

#endif
