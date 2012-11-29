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


#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include "peer.h"
#include "options.h"

#ifndef NO_P4P
	#include <p4p/p4p.h>
	#include <p4p/app/p4papp.h>

	using namespace p4p;
	using namespace p4p::app;
#endif

/**
 * Channel data structure already maintained by the tracker
 */
class Channel
{
public:
	Channel(const std::string& id);

#ifndef NO_P4P
	Channel(const std::string& id,
		const ISPManager& isp_mgr,
		P4PUpdateManager* update_mgr,
		const PeeringGuidanceMatrixOptions& options,
		const std::string& optEngineAddr,
		unsigned short optEnginePort);
#endif

	~Channel();

	/* Accessor methods */
	const std::string& getID() const { return m_id; }

	unsigned int getNumPeers() const;

	/* Peer management functions */
	bool addPeer(Peer* peer);
	bool deletePeer(const std::string& peerid);
	bool updatePeerStats(const std::string& peerid, bool downloading, float upcap, float downcap);
	bool selectPeers(const std::string& peerid, unsigned int num_peers, std::vector<Peer*>& selected_peers);
	Peer* findPeer(const std::string& peerid);

private:
	/* Collection of peers in the swarm */
	typedef std::map<std::string, Peer*> PeerCollection;

	Peer* findPeer(const std::string& peerid) const;

	std::string m_id;			/**< Globally-unique channel ID */
	PeerCollection m_peers;			/**< Collection of peers in channel */
#ifndef NO_P4P
	P4PUpdateManager* m_update_mgr;		/**< Local reference to P4P update manager */
	PGMSelectionManager* m_selection_mgr;	/**< Manager for P4P peer selection */
#endif
};

#endif
