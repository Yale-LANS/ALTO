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


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "mlu.h"
#include "temp_file.h"

/*
 * Currently disabled since code structure has changed greatly.
 */
#if 0

struct MLUPluginFixture {
	NetState network;
	NetVertexNameVector node;
	NetVertexVector v;
	NetEdgeVector e;
	NetLocationVector pid;
	View::NetLocationVertexMap pid_map;

	MLUPluginFixture() : node(4), v(4), e(8), pid(4) {
		BlockWriteLock network_lock(network);

		// setup nodes
		node[0] = "0";
		node[1] = "1";
		node[2] = "2";
		node[3] = "3";
		for (int i = 0; i < 4; ++i) {
			network.add_node(node[i], v[i], network_lock);
			network.set_external(v[i], false, network_lock);
			network.add_autoroute_node(v[i], network_lock);
		}

		// setup links and routing
		int link[8][2] = { {0, 1}, {1, 0}, {1, 2}, {2, 1}, {2, 3}, {3, 2}, {3, 0}, {0, 3}};
		double capacity[8] = {2, 4, 4, 4, 4, 4, 4, 4};
		double weight[8] = {1, 1, 1, 1, 1, 1, 1, 1};
		for (int i = 0; i < 8; ++i) {
			network.add_edge(node[ link[i][0] ], node[ link[i][1] ], e[i], network_lock);
			network.set_capacity(e[i], capacity[i], network_lock);
			network.set_weight(e[i], weight[i], network_lock);
		}
		network.update_autoroutes(network_lock);

		// setup traffic
		// 0 -> 1: 1 unit of traffic
		network.set_traffic(e[0], 1.2, network_lock);

		// setup PIDs
		pid[0] = NetLocation(1, 0);
		pid[1] = NetLocation(1, 1);
		pid[2] = NetLocation(1, 2);
		pid[3] = NetLocation(1, 3);
		for (int i = 0; i < 4; ++i) {
			pid_map[ pid[i] ].insert( v[i] );
		}
	}
};

BOOST_AUTO_TEST_CASE ( test_compute_prices_singleton_pid_all_internal_convergence )
{
	MLUPluginFixture f;
	NetState& network(f.network);
	BlockWriteLock network_lock(network);
	NetVertexVector& v(f.v);
	NetEdgeVector& e(f.e);
	NetLocationVector& pid(f.pid);
	View::NetLocationVertexMap& pid_map(f.pid_map);

	PIDMatrix internal_price;
	BlockWriteLock internal_price_lock(internal_price);
	SparsePIDMatrix external_price;
	BlockWriteLock external_price_lock(external_price);

	internal_price.set_pids(pid, internal_price_lock);
	external_price.set_pids(pid, external_price_lock);

	MLUPlugin optimizer(OptPluginMLUDescriptor);
	init_logger(2);

	// we should converge in 1000 iterations
	for (int i = 0; i < 2; ++i) {
		//optimizer.set_mu((double) 1.0 / (i + 1));
		optimizer.compute_prices(network, network_lock,
					 pid_map,
					 internal_price, internal_price_lock,
					 external_price, external_price_lock);
	}

	BOOST_CHECK_EQUAL(optimizer.get_alpha(), 0.6);
	BOOST_CHECK_EQUAL(optimizer.get_scaled_edge_price(network, network_lock, v[0], v[1]), 1.0);
	BOOST_CHECK_EQUAL(optimizer.get_scaled_edge_price(network, network_lock, v[1], v[2]), 0.0);
	BOOST_CHECK_EQUAL(optimizer.get_scaled_edge_price(network, network_lock, v[3], v[2]), 0.0);
}

BOOST_AUTO_TEST_CASE ( test_maintain_price_across_serialization )
{
	MLUPluginFixture f;
	NetState& network(f.network);
	BlockWriteLock network_lock(network);
	NetVertexNameVector& node_name(f.node);

	MLUPlugin optimizer(OptPluginMLUDescriptor);
	double mlu = optimizer.compute_mlu(network, network_lock);
	optimizer.update_scaled_edge_price(network, network_lock, mlu);
	NetVertex from_1, to_1;
	network.get_node(node_name[0], from_1, network_lock);
	network.get_node(node_name[1], to_1, network_lock);
	double price_1 = optimizer.get_scaled_edge_price(network, network_lock, from_1, to_1);
	BOOST_CHECK_CLOSE(price_1, 0.125, 1e-6);

	temp_file file("/tmp/net_state.XXXXXX");

	{
		std::ofstream ofs(file.name());
		boost::archive::text_oarchive oa(ofs);
		network.do_save(oa, network_lock);
	}

	{
		std::ifstream ifs(file.name());
		boost::archive::text_iarchive ia(ifs);
		network.do_load(ia, network_lock);
	}

	NetVertex from_2, to_2;
	network.get_node(node_name[0], from_2, network_lock);
	network.get_node(node_name[1], to_2, network_lock);
	double price_2 = optimizer.get_scaled_edge_price(network, network_lock, from_2, to_2);
	BOOST_CHECK_CLOSE(price_2, 0.125, 1e-6);
}

#endif
