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
 * P4P API Demonstration Program
 * Copyright 2009, Yale Laboratory of Networked Systems
 *
 * This example program illustrates how the P4P APIs for both
 * the Portal Location and PDistance interfaces, as well as
 * the Application Optimization Engine.  The program can perform
 * one of many operations, as indicated by the 'operation'
 * command-line argument.  Here are some examples:
 *
 * Display the PIDs corresponding to set of IP addresses:
 *   p4pcli --server=p4p.isp.net --port=6671 --operation=pid < ip-list.txt
 *
 *   where 'ip-list.txt' contains a list of IP addresses, such as:
 *
 *   1.2.3.4
 *   1.2.3.5
 *
 *   Supplying an empty list of IP addresses returns the PID of the requesting
 *   IP address.
 *
 * Display the pDistances for PID pairs exposed by a Portal server:
 *   p4pcli --server=p4p.isp.net --port=6671 --operation=pdistance < pid-endpoints.txt
 *
 *   where 'pid-endpoints.txt' contains a list of PID Endpoint pairs, such as:
 *
 *   0.i.isp.net 0.i.isp.net
 *   0.i.isp.net 1.i.isp.net
 *   1.i.isp.net 0.i.isp.net
 *   1.i.isp.net 1.i.isp.net
 *
 * Display the generic peering weights for PID pairs of a particular ISP:
 *   p4pcli --server=p4p.isp.net --port=6671 --operation=generic-guidance --guidance-server=aoe.application.com --guidance-port=6673 < pid-endpoints.txt
 *
 *   where 'pid-endpoints.txt' contains a list of PID Endpoint pairs, such as:
 *   
 *   0.i.isp.net 0.i.isp.net
 *   0.i.isp.net 1.i.isp.net
 *   1.i.isp.net 0.i.isp.net
 *   1.i.isp.net 1.i.isp.net
 *
 * Display the custom peering guidance for the PIDs of a particular ISP:
 *   p4pcli --server=p4p.isp.net --port=6671 --operation=custom-guidance --guidance-server=aoe.application.com --guidance-port=6673 < swarm-state.txt
 *
 * where 'swarm-state.txt' contains the list of PIDs, and for each PID the following information:
 *     - aggregate PID upload capacity (kbps)
 *     - aggregate PID download capacity (kbps)
 *     - number of seeders in PID
 *     - number of leechers in PID
 *   for example, 'swarm-state.txt' may be:
 *
 *   0.i.isp.net 1024 2048 2 3
 *   1.i.isp.net 2048 4096 3 7
 *   2.i.isp.net 1024 2048 1 4
 *
 */

#include <set>					/* STL includes */
#include <map>
#include <vector>
#include <iterator>
#include <iostream>

#include <boost/foreach.hpp>			/* Boost includes */

#include <p4p/p4p.h>				/* P4P Common API */
#include <p4p/app/p4papp.h>			/* P4P Application API */

#include "options.h"				/* Command-line argument parsing */

using namespace p4p;
using namespace p4p::app;

/* Function prototypes */
void handle_pdistance(const std::string& portalAddr, unsigned short portalPort);
void handle_pid(const std::string& portalAddr, unsigned short portalPort);

void handle_guidance_generic(const std::string& portalAddr, unsigned short portalPort, const std::string& optEngineAddr, unsigned short optEnginePort);
void handle_guidance_custom(const std::string& portalAddr, unsigned short portalPort, const std::string& optEngineAddr, unsigned short optEnginePort);

int main(int argc, char** argv)
{
	/* Parse and validate program options */
	handle_options(argc, argv);

	/* Extract desired operation and handle appropriately */
	std::string operation = OPTIONS["operation"].as<std::string>();

	/* Extract configuration parameters */
	std::string portalAddr = OPTIONS["server"].as<std::string>();
	unsigned short portalPort = OPTIONS["port"].as<unsigned short>();
	std::string optEngineAddr = OPTIONS["guidance-server"].as<std::string>();
	unsigned short optEnginePort = OPTIONS["guidance-port"].as<unsigned short>();

	try
	{
		if (operation == "pid")				/* Query PIDs for IP addresses */
			handle_pid(portalAddr, portalPort);

		else if (operation == "pdistance")		/* Query PDistance between PID pairs */
			handle_pdistance(portalAddr, portalPort);

		else if (operation == "guidance-generic")	/* Query AOE for Generic Peering Guidance */
			handle_guidance_generic(portalAddr, portalPort, optEngineAddr, optEnginePort);

		else if (operation == "guidance-custom")	/* Customized usage of AOE */
			handle_guidance_custom(portalAddr, portalPort, optEngineAddr, optEnginePort);

		else
		{
			std::cerr << "Invalid operation: " << operation << std::endl;
			return 1;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void handle_pid(const std::string& portalAddr, unsigned short portalPort)
{
	/* Initialize ISP */
	ISP my_isp(portalAddr, portalPort);
	my_isp.loadP4PInfo();

	/* Lookup PID for each input IP address */
	std::string ip;
	while (std::cin >> ip)
	{
		int pid = my_isp.lookup(ip);
		if (pid == ERR_UNKNOWN_PID)
			std::cout << "IP: " << ip << '\t' << "PID Unknown" << std::endl;
		else
			std::cout << "IP: " << ip << '\t' << my_isp.getPIDInfo(pid) << pid << std::endl;
	}
}

void handle_pdistance(const std::string& portalAddr, unsigned short portalPort)
{
	/* Initialize ISP */
	ISP my_isp(portalAddr, portalPort);
	my_isp.loadP4PInfo();

	/* Retrieve PDistance between each pair of input PIDs */
	PID src, dst;
	while (std::cin >> src >> dst)
	{
		/* Lookup indexes of the PIDs */
		int src_pid = my_isp.getPIDIndex(src);
		if (src_pid == p4p::ERR_UNKNOWN_PID)
		{
			std::cerr << "Unknown PID: " << src << std::endl;
			return;
		}
	
		int dst_pid = my_isp.getPIDIndex(dst);
		if (dst_pid == p4p::ERR_UNKNOWN_PID)
		{
			std::cerr << "Unknown PID: " << dst << std::endl;
			return;
		}

		/* Lookup the pDistance */
		unsigned int pdistance = my_isp.getPDistance(src_pid, dst_pid);
		std::cout << "SourcePID: " << src << '\t' << "DestPID: " << dst << '\t' << "PDistance: " << pdistance << std::endl;
	}
}

void handle_guidance_generic(const std::string& portalAddr, unsigned short portalPort, const std::string& optEngineAddr, unsigned short optEnginePort)
{
	/* Initialize ISP */
	ISP my_isp(portalAddr, portalPort);
	my_isp.loadP4PInfo();

	/* Retrieve generic peering guidance */
	PeeringGuidanceMatrix my_guidance(&my_isp, optEngineAddr, optEnginePort);
	my_guidance.setOptions(PeeringGuidanceMatrixOptions::FilesharingGeneric());
	my_guidance.compute();

	/* Retrieve peering guidance for each pair of input PIDs */
	PID src, dst;
	while (std::cin >> src >> dst)
	{
		int src_pid = my_isp.getPIDIndex(src);
		if (src_pid == p4p::ERR_UNKNOWN_PID)
		{
			std::cerr << "Unknown PID: " << src << std::endl;
			return;
		}

		int dst_pid = my_isp.getPIDIndex(dst);
		if (dst_pid == p4p::ERR_UNKNOWN_PID)
		{
			std::cerr << "Unknown PID: " << dst << std::endl;
			return;
		}

		double intrapid_pct, intraisp_pct;
		std::vector<double> weights;
		if (my_guidance.getWeights(src_pid, intrapid_pct, intraisp_pct, weights) < 0)
		{
			std::cerr << "Failed to get peering weights" << std::endl;
			return;
		}

		std::cout << "SourcePID: " << src << '\t' << "DestPID: " << dst << '\t' << "GenericWeight: " << weights[dst_pid] << std::endl;
	}
}

void handle_guidance_custom(const std::string& portalAddr, unsigned short portalPort, const std::string& optEngineAddr, unsigned short optEnginePort)
{
	/* Initialize ISP */
	ISP my_isp(portalAddr, portalPort);
	my_isp.loadP4PInfo();

	/* Read from standard input the swarm state to be used for computing guidance. The
	 * PID in the input is in human-readable form. */
	SwarmState swarm;
	PID swarm_pid;
	unsigned int seeds, leeches;
	double ul_cap, dl_cap;
	while (std::cin >> swarm_pid >> ul_cap >> dl_cap >> seeds >> leeches)
	{
		/* Lookup the index for the PID */
		int swarm_pid_idx = my_isp.getPIDIndex(swarm_pid);
		if (swarm_pid_idx == p4p::ERR_UNKNOWN_PID)
		{
			std::cerr << "Unknown PID: " << swarm_pid << std::endl;
			return;
		}
		swarm.setNumSeeds(swarm_pid_idx, seeds);
		swarm.setNumLeechers(swarm_pid_idx, leeches);
		swarm.setUploadCapacity(swarm_pid_idx, ul_cap);
		swarm.setDownloadCapacity(swarm_pid_idx, dl_cap);
	}

	/* Retrieve swarm-dependent peering guidance */
	PeeringGuidanceMatrix my_guidance(&my_isp, optEngineAddr, optEnginePort);
	my_guidance.setOptions(PeeringGuidanceMatrixOptions::FilesharingSwarmDependent());
	my_guidance.compute(swarm);

	/* Print out the returned peering guidance */
	unsigned int num_intraisp_pids, num_total_pids;
	my_isp.getNumPIDs(&num_intraisp_pids, &num_total_pids);
	for (int src_pid = 0; src_pid < (int)num_intraisp_pids; ++src_pid)
	{
		PID src = my_isp.getPIDInfo(src_pid);

		double intrapid_pct, intraisp_pct;
		std::vector<double> weights;
		if (my_guidance.getWeights(src_pid, intrapid_pct, intraisp_pct, weights) < 0)
		{
			std::cerr << "Failed to get peering weights for source " << src << std::endl;
			return;
		}

		std::cout << src << "\tintrapid\t" << intrapid_pct << "\tintradomain\t" << intraisp_pct;

		for (int dst_pid = 0; dst_pid < (int)num_total_pids; ++dst_pid)
			std::cout << '\t' << my_isp.getPIDInfo(dst_pid) << '\t' << weights[dst_pid];

		std::cout << std::endl;
	}
}


