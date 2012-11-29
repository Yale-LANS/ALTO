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


#include "main.h"

#include <p4p/protocol-aoe/aoe.h>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "options.h"

using namespace p4p;
using namespace p4p::protocol;
using namespace p4p::protocol::aoe;

void handle_ssm(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError);
void handle_ssmcm(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError);
void handle_custom(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError);
void handle_generic(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError);

int main(int argc, char** argv)
{
	handle_options(argc, argv);

	AOEProtocol api(OPTIONS["server"].as<std::string>(), OPTIONS["port"].as<unsigned short>());
	WeightsMap weights;
	PIDClassPercentages pcts;

	std::string portal = OPTIONS["portal"].as<std::string>();

	/* Get the appropriate set of weights */
	std::string op = OPTIONS["op"].as<std::string>();
	try
	{
		if (op == "generic")
			handle_generic(api, portal, weights, pcts);
		else if (op == "custom")
			handle_custom(api, portal, weights, pcts);
		else if (op == "ssm")
			handle_ssm(api, portal, weights, pcts);
		else if (op == "ssmcm")
			handle_ssmcm(api, portal, weights, pcts);
		else
		{
			std::cerr << "Invalid operation: " << op << std::endl;
			return 1;
		}
	}
	catch (P4PProtocolError& e)
	{
		std::cerr << "API Error: " << e.what() << std::endl;
		return 1;
	}

	/* Print the results */
	std::set<PID> srcs;
	weights.get_srcs(std::inserter(srcs, srcs.end()));
	BOOST_FOREACH(const PID& src, srcs)
	{
		std::cout << src
			  << "\tintrapid\t" << pcts.get_intrapid(src)
			  << "\tintradomain\t" << pcts.get_intradomain(src)
			  << "\tinterdomain\t" << pcts.get_interdomain(src);
		const WeightsMap::Row& row = weights.get_row(src);
		BOOST_FOREACH(const WeightsMap::RowEntry& e, row)
		{
			std::cout << '\t' << e.first << '\t' << e.second;
		}
		std::cout << std::endl;
	}

	return 0;
}

void handle_ssm(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError)
{
	double channel_rate, relax;
	std::cin >> channel_rate >> relax;
	channel_rate *= 1024;

	SwarmState swarm;
	PID src;
	unsigned int seeds, leeches;
	double ul_cap, dl_cap;
	double ul_rate, dl_rate;
	unsigned int active_peers;
	while (std::cin >> src >> ul_cap >> dl_cap >> seeds >> leeches >> ul_rate >> dl_rate >> active_peers)
		swarm.add(src, seeds, leeches, ul_cap, dl_cap, ul_rate, dl_rate, active_peers);

	api.get_weights(portal, swarm, std::string("ssm"), channel_rate, relax, weights, pcts);
}

void handle_ssmcm(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError)
{
	double channel_rate;
	std::cin >> channel_rate;

	SwarmState swarm;
	PID src;
	unsigned int seeds, leeches;
	double ul_cap, dl_cap;
	double ul_rate, dl_rate;
	unsigned int active_peers;
	while (std::cin >> src >> ul_cap >> dl_cap >> seeds >> leeches >> ul_rate >> dl_rate >> active_peers)
		swarm.add(src, seeds, leeches, ul_cap, dl_cap, ul_rate, dl_rate, active_peers);

	api.get_weights(portal, swarm, std::string("ssmcm"), channel_rate, weights, pcts);
}

void handle_custom(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError)
{
	SwarmState swarm;

	PID src;
	unsigned int seeds, leeches;
	double ul_cap, dl_cap;
	double ul_rate, dl_rate;
	unsigned int active_peers;
	while (std::cin >> src >> ul_cap >> dl_cap >> seeds >> leeches >> ul_rate >> dl_rate >> active_peers)
		swarm.add(src, seeds, leeches, ul_cap, dl_cap, ul_rate, dl_rate, active_peers);

	api.get_weights(portal, swarm, weights, pcts);
}

void handle_generic(AOEProtocol& api, const std::string& portal, WeightsMap& weights, PIDClassPercentages& pcts) throw (P4PProtocolError)
{
	std::vector<PID> srcs;
	PID src;
	while (std::cin >> src)
		srcs.push_back(src);

	api.get_weights(portal, srcs.begin(), srcs.end(), weights, pcts);
}

