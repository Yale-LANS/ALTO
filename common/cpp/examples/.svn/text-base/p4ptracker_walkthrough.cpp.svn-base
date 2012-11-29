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


/*
 * P4P Tracker Walkthrough
 *
 * This is a small example program that walks through the basics of
 * integrating P4P into an existing P2P tracker.  Steps for integrating
 * P4P are marked in comments with the text "*** STEP X ***".
 */

/* *** STEP 1 ***
 * Include P4P header files and namespaces.  There are two components
 * of the P4P library:
 * - Core classes that manage P4P information from ISPs
 * - Application integration classes which encode common data structures
 *   and concepts used when integration P4P into applications.
 *
 * We use both components in this example.
 */

#include <p4p/p4p.h>			/* P4P core includes */
#include <p4p/app/p4papp.h>		/* P4P application includes */

using namespace p4p;			/* Import P4P core namespace */
using namespace p4p::app;		/* Import P4P application namespace */

#ifdef WIN32				/* Platform-dependent includes */
	#include <windows.h>
#else
	#include <pthread.h>
#endif

/* *** STEP 2a ***
 * Extend your peer class from P4PPeerBase. In this walkthrough, we
 * assume your peer class is named 'YourPeerClass'. This allows
 * your peer class to be managed by the P4P Application library
 * data structures, such as those that help with peer selection.
 */
class YourPeerClass : public P4PPeerBase
{
public:
	/* Constructor */
	YourPeerClass(const std::string& id, const std::string& addr, unsigned short port)
		: m_id(id), m_addr(addr), m_port(port), m_downloading(true)
	{}

	/* Your accessor methods */
	const std::string& getID() const		{ return m_id; }
	const std::string& getAddr() const		{ return m_addr; }
	unsigned short getPort() const			{ return m_port; }
	bool isDownloading() const			{ return m_downloading; }

	/* Your methods for updating peer state and statistics */
	void setDownloading(bool value)			{ m_downloading = value; }
	void setCapacityEstimates(float uploadcapacity, float downloadcapacity)
	{
		m_uploadcapacity = uploadcapacity;
		m_downloadcapacity = downloadcapacity;
	}

	/* *** STEP 2b ***
	 * Provide additional methods in P4PPeerBase interface
	 */
	virtual IPAddress getIPAddress() const		{ return m_addr; }
	virtual float getUploadCapacity() const		{ return m_uploadcapacity; }
	virtual float getDownloadCapacity() const	{ return m_downloadcapacity; }
	virtual bool isSeeder() const			{ return !m_downloading; }

private:
	std::string m_id;		/** Unique peer ID */
	std::string m_addr;		/** IP address */
	unsigned short m_port;		/** Port number */
	bool m_downloading;		/** True if peer is downloading (leeching), false if seeding */
	float m_uploadcapacity;		/** Estimated upload capacity (in Kbps) */
	float m_downloadcapacity;	/** Estimated download capacity (in Kbps) */
};

/*
 * Data structure encapsulating a single swarm. In this walkthrough, we
 * assume that your swarm data structure is named 'YourSwarmClass'.
 */
class YourSwarmClass
{
private:
	/* *** STEP 3a ***
	 * Add data member for P4P peer selection manager. The PGMSelectionManager
	 * provides guidance using an Optimization Engine which influences the
	 * peer selection process.
	 */
	PGMSelectionManager* m_selection_mgr;

	/* *** STEP 3b ***
	 * Add data member to store a reference to the P4P update manager, which
	 * handles updating of P4P guidance information.
	 */
	P4PUpdateManager* m_update_mgr;

public:
	/* *** STEP 4a ***
	 * Extend swarm constructors to initialize the selection manager. Additional
	 * arguments are added for:
	 * - An ISPManager, which manages the global collection of P4P-capable ISPs.
	 * - An Update Manager, which handles updating P4P and guidance information
	 * - Address and port number of the Application Optimization Engine Server
	 *   which provides peering guidance
	 */
	YourSwarmClass(const std::string& id,
		       const ISPManager& isp_mgr,
		       P4PUpdateManager* update_mgr)
		: m_update_mgr(update_mgr), m_id(id)
	{
		/* Selection manager takes the ISPManager as a constructor argument. */
		m_selection_mgr = new PGMSelectionManager(&isp_mgr);

		/* Provide the update manager a reference to the selection manager we
		 * have just created.  The update manager will handle updating peering
		 * guidance according to the configured options. */
		m_update_mgr->addSelectionManager(m_selection_mgr);
	}

	/* This demo also illustrates a mode in which a swarm is configured to
	 * only perform its native peer selection without using P4P. In this case,
	 * the pointers to the selection manager and update manager are both set
	 * to NULL. */
	YourSwarmClass(const std::string& id)
		: m_selection_mgr(NULL), m_update_mgr(NULL), m_id(id)
	{}

	/* *** STEP 4b ***
	 * Provide methods to set options for computing peering guidance.  In this
	 * walkthrough, we provide two methods for configuring options:
	 * - setDefaultGuidanceOptions(): Configures default peering guidance options
	 *   across all ISPs.
	 * - setSpecificISPGuidanceOptions(): Allows peering guidance options to be
	 *   overridden for particular ISPs.
	 *
	 * The peer selection guidance used for a peer is controlled by the options
	 * specified for its Home ISP.  If the ISP's guidance options
	 * are not explicitly overridden, the default options are used.
	 *
	 * Finally, we provide an initGuidance() method that instructs the P4P selection
	 * manager to initialize its guidance matrices after all options have been
	 * configured.
	 */

	/* Provide a method for configuring default peering guidance options */
	void setDefaultGuidanceOptions(const PeeringGuidanceMatrixOptions& options)
	{
		/* Configure default guidance matrix options. */
		m_selection_mgr->setDefaultOptions(options);
	}
	
	/* Provide a method allowing guidance options to be overridden for specific ISPs. */
	void setSpecificISPGuidanceOptions(const std::map<const ISP*, PeeringGuidanceMatrixOptions> specific_isp_options)
	{
		std::map<const ISP*, PeeringGuidanceMatrixOptions>::const_iterator isp_itr;
		for(isp_itr = specific_isp_options.begin(); isp_itr != specific_isp_options.end(); isp_itr++)
		{
			const ISP* isp = isp_itr->first;
			const PeeringGuidanceMatrixOptions& options = isp_itr->second;
			m_selection_mgr->setISPOptions(isp, options);
		}
	}

	/* Initialize the guidance matrices. In this example, we use a remote
	 * Optimization Engine server to provide peering guidance. */
	void initGuidance(const std::string& optEngineAddr, unsigned short optEnginePort)
	{
		m_selection_mgr->initGuidance(optEngineAddr, optEnginePort);
	}

	/* Your accessor methods */
	const std::string& getID() const		{ return m_id; }

	/* Your peer management functions */

	YourPeerClass* findPeer(const std::string& peerid)
	{
		/* Locate peer data structure with the given ID. Return NULL if not found */
		PeerCollection::const_iterator peer_itr = m_peers.find(peerid);
		if (peer_itr == m_peers.end())
			return NULL;
		else
			return peer_itr->second;
	}

	void peerJoined(YourPeerClass* peer)
	{
		m_peers[peer->getID()] = peer;

		/* *** STEP 5a ***
		 * Extend peer join event handler to add peer to selection manager
		 */
		if (m_selection_mgr)
			m_selection_mgr->addPeer(peer);
	}

	void peerStatsChanged(const std::string& peerid, bool downloading, float uploadcapacity, float downloadcapacity)
	{
		YourPeerClass* peer = findPeer(peerid);
		if (!peer)
			return;

		peer->setDownloading(downloading);
		peer->setCapacityEstimates(uploadcapacity, downloadcapacity);

		/* *** STEP 5b ***
		 * Extend peer statistics updated event to update stats in selection manager,
		 * notifying it that the peers statistics have changed.
		 */
		if (m_selection_mgr)
			m_selection_mgr->updatePeerStats(peer);
	}

	void selectPeers(const std::string& peerid, std::vector<YourPeerClass*>& selected_peers)
	{
		YourPeerClass* peer = findPeer(peerid);
		if (!peer)
			return;

		/* *** Step 5c ***
		 * Try peer selection through the P4P peer selection manager. If it
		 * is not available (e.g., if P4P information is currently being refreshed,
		 * ISP Portal servers are not avalable, etc), then continue with your
		 * native peer selection.
		 * 
		 * P4P peer selection takes an argument indicating how many peers to
		 * select. The output parameter the peer selection is pointers to
		 * P4PPeerBase. These should be typecasted back to your peers.
		 */
		if (m_selection_mgr)
		{
			/* P4P-guided peer selection is enabled */
			std::vector<const P4PPeerBase*> p4p_selected_peers;
			if (m_selection_mgr->selectPeers(peer, 50, p4p_selected_peers) == 0)
			{
				/* Typecast P4PPeerBase pointers back to YourPeerClass */
				for (unsigned int i = 0; i < p4p_selected_peers.size(); ++i)
					selected_peers.push_back((YourPeerClass*)p4p_selected_peers[i]);
				return;
			}
		}

		/* One of three things happened:
		 * - Swarm was configured to perform native peer selection (e.g., not
		 *   use P4P guidance)
		 * - No guidance is currently available
		 * - Some other error occurred.
		 *
		 * Now, continue with your native peer selection process
		 */

		/* Your native peer selection logic ... */

		/* For simplicity in this walkthrough, we choose the first 50 peers
		 * in the peers collection.
		 */
		std::cout << "Using native peer selection algorithm" << std::endl;
		unsigned int num_selected = 0;
		for (PeerCollection::const_iterator itr = m_peers.begin();
		     num_selected < 50 && itr != m_peers.end();
		     ++itr)
		{
			YourPeerClass* candidate = itr->second;
			if (candidate == peer)	/* Avoid selecting peer for itself */
				continue;
			
			selected_peers.push_back(candidate);
			++num_selected;
		}

		/* Finished with native peer selection */
	}

	void peerDeparted(const std::string& peerid)
	{
		YourPeerClass* peer = findPeer(peerid);
		if (!peer)
			return;

		/* *** STEP 5d ***
		 * Extend peer leave event handler to remove peer from selection manager
		 */
		if (m_selection_mgr)
			m_selection_mgr->removePeer(peer);

		delete peer;
		m_peers.erase(peerid);
	}

	/* *** STEP 6 ***
	 * Extend swarm destructor to cleanup the selection manager
	 */
	~YourSwarmClass()
	{
		/* Notify the update manager that the selection manager will be destroyed, then
		 * free its resources. */
		if (m_selection_mgr)
		{
			m_update_mgr->removeSelectionManager(m_selection_mgr);
			delete m_selection_mgr;
		}

		/* Your normal destructor code ... */
	}


private:
	/* Mapping from peer ID to peer data structure */
	typedef std::map<std::string, YourPeerClass*> PeerCollection;
	std::string m_id;			/* Swarm ID */
	PeerCollection m_peers;			/* Peers in the channel */
};

/* *** STEP 7 ***
 * Provide a thread in which the P4P update manager may run. The update
 * manager makes network connections to fetch information and may perform
 * longer-running computations, so such operations should not block peer join/leave
 * events, peer selection, etc.
 */
#ifdef WIN32
DWORD WINAPI
#else
void*
#endif
p4p_update_thread(void* arg)
{
	P4PUpdateManager* update_mgr = (P4PUpdateManager*)arg;
	update_mgr->run();
	return NULL;
}

/* Utility method for printing a collection of selected peers. Peer ID and IP
 * address are displayed for each selected peer. */
void printSelectedPeers(const std::string& peer, const std::vector<YourPeerClass*>& selected_peers)
{
	std::cout << "Selected peers for " << peer << std::endl;
	for (unsigned int i = 0; i < selected_peers.size(); ++i)
	{
		YourPeerClass* sel_peer = selected_peers[i];
		std::cout << '\t' << sel_peer->getID() << " ( " << sel_peer->getAddr() << " )" << std::endl;
	}
}

/* *** STEP 8 ***
 * Read configuration options; these may be read from a configuration file, database,
 * or command-line options.  For simplicity, they are defined here as global
 * variables.
 *
 * We have configuration options for:
 * - The set of configured Portal Servers for P4P-capable ISPs
 * - The address and port of the Application Optimization Engine Server we're using
 */
const std::string CTZJ_PORTAL_ADDR = "p4p-8.cs.yale.edu";	/* Portal server address for China Telecom Zhejiang */
const unsigned short CTZJ_PORTAL_PORT = 6671;			/* Portal server port for China Telecom Zhejiang */

const std::string CTOUTZJ_PORTAL_ADDR = "p4p-8.cs.yale.edu";	/* Portal server address for China Telecom (out of Zhejiang) */
const unsigned short CTOUTZJ_PORTAL_PORT = 6771;		/* Portal server port for China Telecom (out of Zhejiang) */

const std::string CNC_PORTAL_ADDR = "p4p-8.cs.yale.edu";	/* Portal server address for China NetCom */
const unsigned short CNC_PORTAL_PORT = 6681;			/* Portal server port for China NetCom */

const std::string CERNET_PORTAL_ADDR = "p4p-8.cs.yale.edu";	/* Portal server address for China NetCom */
const unsigned short CERNET_PORTAL_PORT = 6691;			/* Portal server port for China NetCom */

const std::string OPT_ENGINE_ADDR = "p4p-8.cs.yale.edu";	/* Optimization Engine server address */
const unsigned short OPT_ENGINE_PORT = 6673;			/* Optimization Engine server port */

int main()
{
	p4p::setLogLevel(LOG_DEBUG);

	/* *** STEP 9a ***
	 * Create manager for P4P-capable ISPs, and configured ISPs
	 */
	ISPManager isp_manager;
	isp_manager.addISP("ct-zj", CTZJ_PORTAL_ADDR, CTZJ_PORTAL_PORT);
	isp_manager.addISP("ct-outzj", CTOUTZJ_PORTAL_ADDR, CTOUTZJ_PORTAL_PORT);
	isp_manager.addISP("cnc", CNC_PORTAL_ADDR, CNC_PORTAL_PORT);
	isp_manager.addISP("cernet", CERNET_PORTAL_ADDR, CERNET_PORTAL_PORT);

	/* *** Step 9b ***
	 * Create P4P Update Manager and spawn update manager thread
	 */
	P4PUpdateManager p4p_update_manager(&isp_manager);
#ifdef WIN32
	/* Create thread under Windows */
	HANDLE p4p_update_thd = CreateThread(NULL, 0, p4p_update_thread, &p4p_update_manager, 0, NULL);
	if (p4p_update_thd == NULL)
#else
	/* Create thread using pthreads */
	pthread_t p4p_update_thd;
	if (pthread_create(&p4p_update_thd, NULL, p4p_update_thread, &p4p_update_manager) != 0)
#endif
	{
		std::cerr << "Failed to spawn P4P update thread" << std::endl;
		exit(1);
	}

	/* For the purposes of this demo, sleep for a few seconds to let P4P
	 * information be refreshed. */
#ifdef WIN32
	Sleep(3000);
#else
	sleep(3);
#endif

	/* *** Step 10 ***
	 * When creating swarms, you may choose to use either your native
	 * peer selection, or use P4P-guided peer selection.  You may specify
	 * different peer selection algorithms for different ISPs.
	 *
	 * In this walkthrough, we will be configuring peer selection for
	 * peers within China Netcom and CERNET to use only network location
	 * information from P4P. Peer selection for peers within other ISPs
	 * (in this case, China Telecom Zhejiang) will use peering
	 * guidance from an Application Optimization Engine.
	 */

	/* As previously mentioned, peers in China Netcom and CERNET use
	 * only network location information for peering guidance in
	 * this walkthrough.
	 *
	 * The guidance options are configured such that
	 * for a peer outside Zhejiang but in China Telecom,
	 * or a peer in China Netcom, up to 90% of its neighbors will be selected 
	 * from the same PID, and up to 95% of its neighbors will be selected 
	 * from the same network (China Netcom).  For a CERNET peer, up to 50% 
	 * of its neighbors will be selected locally from the same PID,
	 * and up to 70% of its neighbors will be selected from CERNET.
	 */
	std::map<const ISP*, PeeringGuidanceMatrixOptions> isp_options;
	isp_options[isp_manager.getISP("ct-outzj")] = PeeringGuidanceMatrixOptions::LocationOnly(0.9, 0.95);
	isp_options[isp_manager.getISP("cnc")] = PeeringGuidanceMatrixOptions::LocationOnly(0.9, 0.95);
	isp_options[isp_manager.getISP("cernet")] = PeeringGuidanceMatrixOptions::LocationOnly(0.5, 0.7);

	/* Construct four swarms.  In this example, we specify different
	 * default peering guidance options for each.  These options
	 * will apply to all ISPs whose options have not been explicitly
	 * overridden (in this case, China Telecom Zhejiang). The different
	 * guidance options for China Telecom Zhejiang are:
	 * - Native peer selection behavior for the entire swarm
	 * - P4P-guided peer selection using swarm-independent guidance for
	 *   filesharing
	 * - P4P-guided peer selection using swarm-dependent guidance for
	 *   filesharing, where guidance will be tuned according to the current
	 *   swarm information (e.g., number of peers, peer capacity estimates)
	 * - P4P-guided peer selection using swarm-dependent guidance for
	 *   streaming.
	 */
	YourSwarmClass* swarms[4];
	swarms[0] = new YourSwarmClass("native");

	swarms[1] = new YourSwarmClass("p4p-filesharing-swarm-independent", isp_manager, &p4p_update_manager);
	swarms[1]->setDefaultGuidanceOptions(PeeringGuidanceMatrixOptions::FilesharingGeneric());
	swarms[1]->setSpecificISPGuidanceOptions(isp_options);
	swarms[1]->initGuidance(OPT_ENGINE_ADDR, OPT_ENGINE_PORT);

	swarms[2] = new YourSwarmClass("p4p-filesharing-swarm-dependent", isp_manager, &p4p_update_manager);
	swarms[2]->setDefaultGuidanceOptions(PeeringGuidanceMatrixOptions::FilesharingSwarmDependent());
	swarms[2]->setSpecificISPGuidanceOptions(isp_options);
	swarms[2]->initGuidance(OPT_ENGINE_ADDR, OPT_ENGINE_PORT);

	swarms[3] = new YourSwarmClass("p4p-streaming-swarm-dependent", isp_manager, &p4p_update_manager);
	swarms[3]->setDefaultGuidanceOptions(PeeringGuidanceMatrixOptions::StreamingSwarmDependent(
							50000 /* channel streaming rate (Byte per second) */, 
							PeeringGuidanceMatrixOptions::OPT_STREAMING_SSMCM));
	swarms[3]->setSpecificISPGuidanceOptions(isp_options);
	swarms[3]->initGuidance(OPT_ENGINE_ADDR, OPT_ENGINE_PORT);

	/* Your code for constructing, adding, and removing peers.  This demo
	 * simulates 3 peers joining and leaving each of the swarms. */
	for (unsigned int i = 0; i < 4; ++i)
	{
		YourSwarmClass* swarm = swarms[i];

		std::cout << "Simulating swarm " << swarm->getID() << std::endl;

		/* Peer 1 joins swarm. Peer 1 is inside China Telecom Zhejiang. */
		YourPeerClass* peer1 = new YourPeerClass("peer1", "202.75.218.179", 1234);
		swarm->peerJoined(peer1);
		std::cout << peer1->getID() << " joined with Home PID " << peer1->getHomePID() << std::endl;

		/* Select peers for Peer 1. No peers should be selected since
		 * no other peers are currently in swarm) */
		std::vector<YourPeerClass*> peer1_selected_peers;
		swarm->selectPeers("peer1", peer1_selected_peers);
		printSelectedPeers("peer1", peer1_selected_peers);

		/* Peer 2 joins swarm.  Peer 2 is inside CERNET. */
		YourPeerClass* peer2 = new YourPeerClass("peer2", "59.66.122.38", 4321);
		swarm->peerJoined(peer2);
		std::cout << peer2->getID() << " joined with Home PID " << peer2->getHomePID() << std::endl;

		/* Select peers for Peer 2. Native peer selection algorithm
		 * is used since it is not within a P4P-capable ISP. Only
		 * Peer 1 should be selected. */
		std::vector<YourPeerClass*> peer2_selected_peers;
		swarm->selectPeers("peer2", peer2_selected_peers);
		printSelectedPeers("peer2", peer2_selected_peers);

		/* Peer 3 joins swarm. Peer is inside China Telecom Zhejiang. */
		YourPeerClass* peer3 = new YourPeerClass("peer3", "218.71.160.21", 5678);
		swarm->peerJoined(peer3);
		std::cout << peer3->getID() << " joined with Home PID " << peer3->getHomePID() << std::endl;

		/* Select peers for Peer 3.  Peers 1 and 2 should be selected,
		 * with Peer 1 first since it is also inside of China Telecom Zhejiang. */
		std::vector<YourPeerClass*> peer3_selected_peers;
		swarm->selectPeers("peer3", peer3_selected_peers);
		printSelectedPeers("peer3", peer3_selected_peers);

		/* Peers leave the swarm */
		swarm->peerDeparted("peer1");
		swarm->peerDeparted("peer2");
		swarm->peerDeparted("peer3");
	}

	/* Cleanup resources for the swarms */
	for (unsigned int i = 0; i < 4; ++i)
		delete swarms[i];
	
	/* *** Step 11 ***
	 * During system shutdown, stop the P4P update manager and wait
	 * for the thread to complete.
	 */
	p4p_update_manager.stop();
#ifdef WIN32
	if (WaitForSingleObject(p4p_update_thd, INFINITE) != WAIT_OBJECT_0)
#else
	if (pthread_join(p4p_update_thd, NULL) != 0)
#endif
	{
		std::cerr << "Failed to join P4P update thread" << std::endl;
		exit(1);
	}
}

