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

#include "peer.h"
#include "channel.h"

#include "options.h"

Channel::Channel(const std::string& id)
	: m_id(id)
#ifndef NO_P4P
	, m_update_mgr(NULL)
	, m_selection_mgr(NULL)
#endif
{
}

#ifndef NO_P4P
Channel::Channel(const std::string& id,
		 const ISPManager& isp_mgr,
		 P4PUpdateManager* update_mgr,
		 const PeeringGuidanceMatrixOptions& options,
		 const std::string& optEngineAddr,
		 unsigned short optEnginePort)
	: m_id(id),
	  m_update_mgr(update_mgr)
{
	m_selection_mgr = new PGMSelectionManager(&isp_mgr);
	m_selection_mgr->setDefaultOptions(options);
	m_selection_mgr->initGuidance(optEngineAddr, optEnginePort);
	m_update_mgr->addSelectionManager(m_selection_mgr);
}
#endif

Channel::~Channel()
{
#ifndef NO_P4P
	if (m_selection_mgr)
	{
		m_update_mgr->removeSelectionManager(m_selection_mgr);
		delete m_selection_mgr;
	}
#endif

	/* Free resources for all peers */
	for (PeerCollection::const_iterator itr = m_peers.begin(); itr != m_peers.end(); ++itr)
		delete itr->second;
}

unsigned int Channel::getNumPeers() const
{
	return m_peers.size();
}

Peer* Channel::findPeer(const std::string& peerid) const
{
	PeerCollection::const_iterator itr = m_peers.find(peerid);
	if (itr == m_peers.end())
		return NULL;

	return itr->second;
}

bool Channel::addPeer(Peer* peer)
{
	if (!m_peers.insert(std::make_pair(peer->getID(), peer)).second)
		return false;

#ifndef NO_P4P
	if (m_selection_mgr)
		if (!m_selection_mgr->addPeer(peer))
			throw std::runtime_error("Illegal state: PeerSelectionManager::addPeer failed");
#endif

	return true;
}

bool Channel::deletePeer(const std::string& peerid)
{
	PeerCollection::iterator itr = m_peers.find(peerid);
	if (itr == m_peers.end())
		return false;

	Peer* peer = itr->second;

#ifndef NO_P4P
	if (m_selection_mgr)
		if (!m_selection_mgr->removePeer(peer))
			throw std::runtime_error("Illegal state: PeerSelectionManager::removePeer failed");
#endif

	m_peers.erase(itr);
	delete peer;
	return true;
}

Peer* Channel::findPeer(const std::string& peerid)
{
	PeerCollection::iterator itr = m_peers.find(peerid);
	if (itr == m_peers.end())
		return NULL;

	return itr->second;
}

bool Channel::updatePeerStats(const std::string& peerid, bool downloading, float upcap, float downcap)
{
	Peer* peer = findPeer(peerid);
	if (!peer)
		return false;

	peer->setDownloading(downloading);
	peer->setCapacityStats(upcap, downcap);

#ifndef NO_P4P
	if (m_selection_mgr)
		if (!m_selection_mgr->updatePeerStats(peer))
			throw std::runtime_error("Illegal state: PeerSelectionManager::updatePeerStats failed");
#endif

	return true;
}

bool Channel::selectPeers(const std::string& peerid, unsigned int num_peers, std::vector<Peer*>& selected_peers)
{
	Peer* peer = findPeer(peerid);
	if (!peer)
		return false;

#ifndef NO_P4P
	/* Do P4P peer selection if desired */
	if (m_selection_mgr)
	{
		std::vector<const P4PPeerBase*> p4p_selected_peers;
		int rc = m_selection_mgr->selectPeers(peer, num_peers, p4p_selected_peers);
		if (rc == 0)
		{
			for (unsigned int i = 0; i < p4p_selected_peers.size(); ++i)
				selected_peers.push_back((Peer*)p4p_selected_peers[i]);
			return true;
		}
		else if (rc != ERR_GUIDANCE_UNAVAILABLE)
			throw std::runtime_error("Illegal state: PeerSelectionManager::selectPeers returned unexpected error code " + p4p::detail::p4p_token_cast<std::string>(rc));
	}
#endif

	/* Do native peer selection (we just return an arbitrary
	 * set here -- not really random) */
	if (OPTION_VERBOSE)
		std::cerr << "Reverting to native peer selection" << std::endl;

	unsigned int num_selected = 0;
	for (PeerCollection::const_iterator itr = m_peers.begin(); num_selected < num_peers && itr != m_peers.end(); ++itr)
	{
		Peer* candidate = itr->second;
		if (candidate == peer)  /* Avoid selecting peer for itself */
			continue;

		selected_peers.push_back(candidate);
		++num_selected;
	}

	return true;
}
