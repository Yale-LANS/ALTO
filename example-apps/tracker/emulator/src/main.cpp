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
 * P4P Tracker Emulator
 * Copyright 2009, Yale Laboratory of Networked Systems
 *
 * This example program emulates the operations of a real P2P
 * tracker that is integrated with P4P.  It emulates peer joins
 * and departures, as well as peer selection.  It also
 * implements background tasks such as retrieving updated PID maps
 * from Portal servers and retrieving guidance from an Optimization
 * Engine.
 * 
 * The emulator reads from standard input the events it should
 * execute, such as adding and removing channels and peers.
 */

#include <iostream>			/* C++ includes */

#include "peer.h"			/* Our data structures */
#include "channel.h"
#include "tracker.h"

#include "meminfo.h"			/* Memory usage information */
#include "options.h"			/* Options parsing */

#ifndef NO_P4P
	#include <p4p/p4p.h>		/* Common P4P includes */
	#include <p4p/app/p4papp.h>
	#include <p4p/app/client_peer_selection_location.h>

	using namespace p4p;		/* Import P4P core namespace */
	using namespace p4p::app;	/* Import P4P application namespace */
#endif

const char* USAGE =			/* String describing program usage */
	"Usage:                                                                     \n"
	"                                                                           \n"
	"   p4ptracker                                                              \n"
	"       [ -i NAME HOST PORT ... -i NAME HOST PORT ]                         \n"
	"       -o HOST PORT                                                        \n"
	"                                                                           \n"
	"Options:                                                                   \n"
	"                                                                           \n"
	"  -i NAME HOST PORT                                                        \n"
	"      Add an ISP with the desired name, and Portal server host and port    \n"
	"      number. The '-i' option may be specified multiple times to add       \n"
	"      multiple ISPs.                                                       \n"
	"                                                                           \n"
	"  -o HOST PORT                                                             \n"
	"      Set the hostname and port number for the Optimization Engine server  \n"
	"                                                                           \n"
	"  -l INTRA_PID_PCT INTRA_ISP_PCT                                           \n"
	"      Set client-side peer selection intra-PID and intra-ISP pencentage    \n"
	"                                                                           \n"
	"  -v                                                                       \n"
	"      Print detailed information about emulation, such as details about    \n"
	"      completed events (channel added, peer joined, peer selection         \n"
	"      results, etc).                                                       \n";

/**
 * Simple function to check whether an input operation was
 * successful, and if not, throw an exception.
 */
void check_input(bool status, const std::string& event, const std::string& msg)
{
	if (status)
		return;

	throw std::runtime_error("Input Failed: " + event + ": " + msg);
}

std::ostream& evt_success(const std::string& event)
{
	return std::cout << event << " succeeded ";
}

std::ostream& evt_failure(const std::string& event)
{
	return std::cout << event << " failed ";
}

#ifndef NO_P4P
/**
 * Simple function to update P4P information. This function is
 * executed in its own thread.
 */
void* update_thread(void* arg)
{
	P4PUpdateManager* update_mgr = (P4PUpdateManager*)arg;
	update_mgr->run();
	return NULL;
}
#endif

/** Main function */
int main(int argc, const char** argv)
{
	try
	{
		/* Parse and validate program options */
		parse_command_line(argc, argv);

#ifndef NO_P4P
		/* create ISP PIDMap dump files */
		std::vector<std::string> pidmap_filenames;

		/* Add configured P4P-capable ISPs */
		ISPManager isp_mgr;
		for (unsigned int i = 0; i < OPTION_ISPS.size(); ++i)
		{
			const ISPOptions& isp = OPTION_ISPS[i];
			isp_mgr.addISP(isp.name, isp.host, isp.port);
			pidmap_filenames.push_back(isp.name);
		}

		/* Create P4P update manager */
		P4PUpdateManager update_mgr(&isp_mgr);

		/* Spawn thread to execute P4P update manager.  The Update Manager automatically
		 * handles refreshing P4P information when required. */
		pthread_t update_mgr_thd;
		if (pthread_create(&update_mgr_thd, NULL, update_thread, &update_mgr) != 0)
			throw std::runtime_error("Failed to create P4P Update Manager thread");
#endif


		/* Create tracker */
#ifndef NO_P4P
		Tracker tracker(isp_mgr, &update_mgr, pidmap_filenames, OPTION_INTRA_PID_PCT, OPTION_INTRA_ISP_PCT);
#else
		Tracker tracker;
#endif

		/* Snapshot of current stats */
		unsigned long long time_snapshot = GetCurrentTimeMicrosec();
		MemInfo meminfo_snapshot = GetCurrentMemInfo();
		TrackerInfo tracker_snapshot = tracker.getInfo();

		/* This example program reads events from standard input. This is the
		 * main loop which reads the next event, and then acts upon it.
		 */
		std::string event;
		while (std::cin >> event)
		{

			/* Event: add_channel [channel id] [filesharing-generic | filesharing-swarm-dependent | streaming-swarm-dependent] [channel rate] */
			if (event == "add_channel")
			{
				std::string id;		check_input(std::cin >> id,		event, "reading channel id");
				std::string options;	check_input(std::cin >> options,	event, "reading optimization type");

				bool result = false;
				if (options == "native")
					result = tracker.createChannel(id);
#ifndef NO_P4P
				else if (options == "location-only")
				{
					double intrapid, intraisp;	
					check_input(std::cin >> intrapid, event, "reading intra-PID peer selection percentage");
					check_input(std::cin >> intraisp, event, "reading intra-ISP peer selection percentage");

					result = tracker.createChannel(id,
							PeeringGuidanceMatrixOptions::LocationOnly(intrapid, intraisp),
							OPTION_OPTENGINE_ADDR, OPTION_OPTENGINE_PORT);
				}
				else if (options == "filesharing-generic")
					result = tracker.createChannel(id,
							PeeringGuidanceMatrixOptions::FilesharingGeneric(),
							OPTION_OPTENGINE_ADDR, OPTION_OPTENGINE_PORT);
				else if (options == "filesharing-swarm-dependent")
					result = tracker.createChannel(id,
							PeeringGuidanceMatrixOptions::FilesharingSwarmDependent(),
							OPTION_OPTENGINE_ADDR, OPTION_OPTENGINE_PORT);
				else if (options == "streaming-swarm-dependent")
				{
					double channel_rate;	check_input(std::cin >> channel_rate,	event, "reading channel rate");

					result = tracker.createChannel(id,
							PeeringGuidanceMatrixOptions::StreamingSwarmDependent(channel_rate),
							OPTION_OPTENGINE_ADDR, OPTION_OPTENGINE_PORT);
				}
#endif
				else
					throw std::runtime_error("Invalid Option Set: " + options);

				if (result && OPTION_VERBOSE)
					evt_success(event) << id << ' ' << options << std::endl;
				else if (!result)
					evt_failure(event) << id << std::endl;
			}

			/* Event: delete_channel [channel id] */
			else if (event == "delete_channel")
			{
				std::string id;		check_input(std::cin >> id,		event, "reading channel id");

				/* Remove the channel */
				bool result = tracker.removeChannel(id);
				if (result && OPTION_VERBOSE)
					evt_success(event) << id << std::endl;
				else if (!result)
					evt_failure(event) << id << std::endl;
			}

			/* Event: peer_join [channel id] [peer id] [ip address] [port] */
			else if (event == "peer_join")
			{
				std::string channelid;	check_input(std::cin >> channelid,	event, "reading channel id");
				std::string peerid;	check_input(std::cin >> peerid,		event, "reading peer id");
				std::string ip;		check_input(std::cin >> ip,		event, "reading peer ip");
				unsigned short port;	check_input(std::cin >> port,		event, "reading peer port");

				/* Convert address to struct */
				in_addr ipaddr;
				if (inet_pton(AF_INET, ip.c_str(), &ipaddr) == 0)
					throw std::runtime_error("Invalid IP Address: " + ip);

				/* Peer join */
				bool result = tracker.peerJoin(peerid, channelid, ipaddr, port);
				if (result && OPTION_VERBOSE)
					evt_success(event) << peerid << ' ' << ip << ':' << port << std::endl;
				else if (!result)
					evt_failure(event) << peerid << ' ' << ip << ':' << port << std::endl;
			}

			/* Event; peer_leave [channel id] [peer id] */
			else if (event == "peer_leave")
			{
				std::string channelid;	check_input(std::cin >> channelid,	event, "reading channel id");
				std::string peerid;	check_input(std::cin >> peerid,		event, "reading peer id");

				/* Peer leave */
				bool result = tracker.peerLeave(peerid, channelid);
				if (result && OPTION_VERBOSE)
					evt_success(event) << peerid << std::endl;
				else if (!result)
					evt_failure(event) << peerid << std::endl;
			}

			/* Event: select_peers [channel id] [peer id] [max peers] */
			else if (event == "select_peers")
			{
				std::string channelid;	check_input(std::cin >> channelid,	event, "reading channel id");
				std::string peerid;	check_input(std::cin >> peerid,		event, "reading peer id");
				unsigned int max_peers;	check_input(std::cin >> max_peers,	event, "reading max peers");

				/* Peer query peers */
				std::vector<Peer*> selected_peers;
				bool result = tracker.peerQuery(peerid, channelid, max_peers, selected_peers);
				if (result && OPTION_VERBOSE)
				{
					evt_success(event) << peerid << '\t' << selected_peers.size() << std::endl;
					for (unsigned int i = 0; i < selected_peers.size(); ++i)
					{
						Peer* peer = selected_peers[i];
						std::cout << '\t' << peer->getID() << ' ' << peer->getAddr() << ' ' << peer->getPort() << std::endl;
					}
				}
				else if (!result)
					evt_failure(event) << peerid << std::endl;
			}

			/* Event: client-side peer selection */
			else if (event == "client_select_peers")
			{
				std::string channelid;	check_input(std::cin >> channelid,	event, "reading channel id");
				std::string peerid;	check_input(std::cin >> peerid,		event, "reading peer id");
				unsigned int max_peers;	check_input(std::cin >> max_peers,	event, "reading max peers");
				unsigned int max_candidates; check_input(std::cin >> max_candidates, event, "reading max candidates");

				if (OPTION_VERBOSE)
					std::cout << "client_peer_selection " << peerid << " start..." << std::endl;

				std::vector<Peer*> candidate_peers;
				std::string candidateid;
				for (unsigned int i = 0; i < max_candidates; i++)
				{
					check_input(std::cin >> candidateid, event, "reading a candidate");
					Peer *candidate;
					if((candidate = tracker.findPeer(candidateid, channelid)) != NULL) 
						candidate_peers.push_back(candidate);
				}

				if (OPTION_VERBOSE)
					std::cout << "client_peer_selection " << peerid << " read " << max_candidates << " candidates" << std::endl;

				/* Peer select peers */
				std::vector<Peer*> selected_peers;
				Peer *peer;
				if ((peer = tracker.findPeer(peerid, channelid)) != NULL) 
				{
#ifndef NO_P4P
					bool result = peer->selectConnection(candidate_peers, max_peers, selected_peers);
					if (result && OPTION_VERBOSE)
					{
						evt_success(event) << peerid << '\t' << selected_peers.size() << std::endl;
						for (unsigned int i = 0; i < selected_peers.size(); ++i)
						{
							Peer* peer = selected_peers[i];
							std::cout << '\t' << peer->getID() << ' ' << peer->getAddr() << ' ' << peer->getPort() << std::endl;
						}
					}
					else if (!result)
						evt_failure(event) << peerid << std::endl;
#endif
				}

				if (OPTION_VERBOSE)
					std::cout << "client_peer_selection " << peerid << " finish..." << std::endl;
			}

			/* Event: peer_update_stats [channel id] [peer id] [downloading (true if non-zero)] [upload capacity Kbps] [download capacity Kbps] */
			else if (event == "peer_update_stats")
			{
				std::string channelid;	check_input(std::cin >> channelid,	event, "reading channel id");
				std::string peerid;	check_input(std::cin >> peerid,		event, "reading peer id");
				int downloading;	check_input(std::cin >> downloading,	event, "reading downloading state: non-zero => downloading");
				float upcap;		check_input(std::cin >> upcap,		event, "reading peer upload capacity");
				float downcap;		check_input(std::cin >> downcap,	event, "reading peer download capacity");

				bool result = tracker.peerReport(peerid, channelid, downloading, upcap, downcap);
				if (result && OPTION_VERBOSE)
					evt_success(event) << peerid << std::endl;
				else if (!result)
					evt_failure(event) << peerid << std::endl;
			}

			/* Event: sleep [time in milliseconds] */
			else if (event == "sleep")
			{
				unsigned int time_msec;	check_input(std::cin >> time_msec,	event, "reading time");

				usleep(1000 * time_msec);
			}

			else if (event == "mem_snapshot")
			{
				meminfo_snapshot = GetCurrentMemInfo();
				if (OPTION_VERBOSE)
					evt_success(event) << std::endl;
			}

			else if (event == "time_snapshot")
			{
				time_snapshot = GetCurrentTimeMicrosec();
				if (OPTION_VERBOSE)
					evt_success(event) << std::endl;
			}

			else if (event == "tracker_snapshot")
			{
				tracker_snapshot = tracker.getInfo();
				if (OPTION_VERBOSE)
					evt_success(event) << std::endl;
			}

			else if (event == "show_current")
			{
				evt_success(event) << GetCurrentTimeMicrosec() << " " << tracker.getInfo() << " " << GetCurrentMemInfo() << std::endl;
			}

			else if (event == "show_diff")
			{
				unsigned long long time_diff = GetCurrentTimeMicrosec() - time_snapshot;
				MemInfo meminfo_diff = GetCurrentMemInfo() - meminfo_snapshot;
				TrackerInfo trackerinfo_diff = tracker.getInfo() - tracker_snapshot;
				evt_success(event) << time_diff << " " << trackerinfo_diff << " " << meminfo_diff << std::endl;
			}

			/* Handle invalid event */
			else
			{
				std::cerr << "Invalid event: " << event << std::endl;
				exit(1);
			}

			std::cout.flush();
		}

#ifndef NO_P4P
		/* Signal P4P update manager to stop and wait for it to complete */
		update_mgr.stop();
		if (pthread_join(update_mgr_thd, NULL) != 0)
			throw std::runtime_error("Failed to join update manager thread");
#endif

		return 0;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception Raised: " << e.what() << std::endl;
		return 1;
	}
}

