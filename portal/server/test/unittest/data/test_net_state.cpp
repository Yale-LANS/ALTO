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

#include "net_state.h"

BOOST_AUTO_TEST_CASE ( net_state_add_node )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		BOOST_CHECK(net_state.add_node("myvertex", v, lock));
	}

	{
		BlockReadLock lock(net_state);
		NetVertex v;
		BOOST_CHECK(net_state.get_node("myvertex", v, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 1);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_add_node_duplicate_fail )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v_orig;
		BOOST_CHECK(net_state.add_node("myvertex", v_orig, lock));
		BOOST_CHECK(!net_state.add_node("myvertex", v_orig, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 1);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_remove_node )
{
	NetState net_state;

	{
		NetVertex v;
		BlockWriteLock lock(net_state);
		BOOST_CHECK(net_state.add_node("myvertex", v, lock));
		BOOST_CHECK(net_state.get_node("myvertex", v, lock));
		BOOST_CHECK(net_state.remove_node("myvertex", lock));
		BOOST_CHECK(!net_state.get_node("myvertex", v, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 0);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_remove_node_descriptor )
{
	NetState net_state;

	{
		NetVertex v1, v2;
		NetEdge e;
		BlockWriteLock lock(net_state);
		BOOST_CHECK(net_state.add_node("src", v1, lock));
		BOOST_CHECK(net_state.add_node("dst", v2, lock));
		BOOST_CHECK(net_state.add_edge(v1, v2, e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		net_state.remove_node(v1, lock);
		BOOST_CHECK(!net_state.get_node("src", v1, lock));
		BOOST_CHECK(net_state.get_node("dst", v2, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 1);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_add_edge )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
	}

	{
		BlockReadLock lock(net_state);
		NetEdge e;
		BOOST_CHECK(net_state.get_edge("src", "dst", e, lock));
	}
}

BOOST_AUTO_TEST_CASE ( net_state_add_edge_duplicate_fail )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK(!net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_add_edge_directed_succeed )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK(net_state.add_edge("dst", "src", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_add_edge_invalid_node )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(!net_state.add_edge("src1", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_remove_edge )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK(net_state.remove_edge("src", "dst", lock));
		BOOST_CHECK(!net_state.get_edge("src", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_remove_node_destroy_edge )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK(net_state.remove_node("src", lock));
		BOOST_CHECK(!net_state.get_node("src", v, lock));
		BOOST_CHECK(!net_state.get_edge("src", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 1);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 0);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_capacity )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		net_state.set_capacity(e_out, 30.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_out, lock), 30.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_capacity_directed )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out, e_in;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK(net_state.add_edge("dst", "src", e_in, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
		net_state.set_capacity(e_out, 30.0, lock);
		net_state.set_capacity(e_in, 15.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_out, lock), 30.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_in, lock), 15.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_traffic )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		net_state.set_traffic(e_out, 30.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_out, lock), 30.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_traffic_directed )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out, e_in;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK(net_state.add_edge("dst", "src", e_in, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
		net_state.set_traffic(e_out, 30.0, lock);
		net_state.set_traffic(e_in, 15.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_out, lock), 30.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_in, lock), 15.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_props_independent )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		net_state.set_capacity(e_out, 30.0, lock);
		net_state.set_traffic(e_out, 15.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_out, lock), 30.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_out, lock), 15.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_edge_props_independent_directed )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e_out, e_in;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e_out, lock));
		BOOST_CHECK(net_state.add_edge("dst", "src", e_in, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
		net_state.set_capacity(e_out, 30.0, lock);
		net_state.set_capacity(e_in, 500.0, lock);
		net_state.set_traffic(e_out, 15.0, lock);
		net_state.set_traffic(e_in, 250.0, lock);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_out, lock), 30.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e_in, lock), 500.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_out, lock), 15.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_traffic(e_in, lock), 250.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_aggregate_line )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v1, v2, v3, v4;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("hop1", v1, lock));
		BOOST_CHECK(net_state.add_node("hop2", v2, lock));
		BOOST_CHECK(net_state.add_node("hop3", v3, lock));
		BOOST_CHECK(net_state.add_node("hop4", v4, lock));
		BOOST_CHECK(net_state.add_edge("hop1", "hop2", e, lock));
		BOOST_CHECK(net_state.add_edge("hop2", "hop3", e, lock));
		BOOST_CHECK(net_state.add_edge("hop3", "hop4", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 4);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 3);
		NetVertexSet v_set;
		v_set.insert(v2);
		v_set.insert(v3);
		NetVertex vh;
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 3);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
	}

	{
		BlockReadLock lock(net_state);
		NetVertex v1, v4;
		BOOST_CHECK(net_state.get_node("hop1", v1, lock));
		BOOST_CHECK(net_state.get_node("hop4", v4, lock));
	}
}


BOOST_AUTO_TEST_CASE ( net_state_aggregate_pair )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v1, v2, v3;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("hop1", v1, lock));
		BOOST_CHECK(net_state.add_node("hop2", v2, lock));
		BOOST_CHECK(net_state.add_node("hop3", v3, lock));
		BOOST_CHECK(net_state.add_edge("hop1", "hop2", e, lock));
		net_state.set_traffic(e, 10.0, lock);
		net_state.set_capacity(e, 10.0, lock);
		BOOST_CHECK(net_state.add_edge("hop1", "hop3", e, lock));
		net_state.set_traffic(e, 5.0, lock);
		net_state.set_capacity(e, 5.0, lock);
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 3);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
		NetVertexSet v_set;
		v_set.insert(v2);
		v_set.insert(v3);
		NetVertex vh;
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		BOOST_CHECK(net_state.get_edge("hop1", "HOP", e, lock));
		BOOST_CHECK_CLOSE(net_state.get_traffic(e, lock), 15.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e, lock), 15.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_aggregate_2pair )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v1, v2, v3, v4;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("hop1", v1, lock));
		BOOST_CHECK(net_state.add_node("hop2", v2, lock));
		BOOST_CHECK(net_state.add_node("hop3", v3, lock));
		BOOST_CHECK(net_state.add_node("hop4", v4, lock));
		BOOST_CHECK(net_state.add_edge("hop1", "hop3", e, lock));
		net_state.set_traffic(e, 1.0, lock);
		net_state.set_capacity(e, 1.0, lock);
		BOOST_CHECK(net_state.add_edge("hop1", "hop4", e, lock));
		net_state.set_traffic(e, 2.0, lock);
		net_state.set_capacity(e, 2.0, lock);
		BOOST_CHECK(net_state.add_edge("hop2", "hop3", e, lock));
		net_state.set_traffic(e, 3.0, lock);
		net_state.set_capacity(e, 3.0, lock);
		BOOST_CHECK(net_state.add_edge("hop2", "hop4", e, lock));
		net_state.set_traffic(e, 4.0, lock);
		net_state.set_capacity(e, 4.0, lock);
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 4);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 4);
		NetVertexSet v_set;
		v_set.insert(v1);
		v_set.insert(v2);
		NetVertex vh;
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP1", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 3);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 2);
		v_set.clear();
		v_set.insert(v3);
		v_set.insert(v4);
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP2", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
		BOOST_CHECK(net_state.get_edge("HOP1", "HOP2", e, lock));
		BOOST_CHECK_CLOSE(net_state.get_traffic(e, lock), 10.0, 0.001);
		BOOST_CHECK_CLOSE(net_state.get_capacity(e, lock), 10.0, 0.001);
	}
}

BOOST_AUTO_TEST_CASE ( net_state_aggregate_line_cycle )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v1, v2, v3, v4, v5;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("hop1", v1, lock));
		BOOST_CHECK(net_state.add_node("hop2", v2, lock));
		BOOST_CHECK(net_state.add_node("hop3", v3, lock));
		BOOST_CHECK(net_state.add_node("hop4", v4, lock));
		BOOST_CHECK(net_state.add_node("hop5", v5, lock));
		BOOST_CHECK(net_state.add_edge("hop1", "hop2", e, lock));
		BOOST_CHECK(net_state.add_edge("hop2", "hop3", e, lock));
		BOOST_CHECK(net_state.add_edge("hop3", "hop4", e, lock));
		BOOST_CHECK(net_state.add_edge("hop4", "hop5", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 5);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 4);
		NetVertexSet v_set;
		v_set.insert(v2);
		v_set.insert(v4);
		NetVertex vh;
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 4);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 4);
		BOOST_CHECK(net_state.get_edge("hop1", "HOP", e, lock));
		BOOST_CHECK(net_state.get_edge("HOP", "hop3", e, lock));
		BOOST_CHECK(net_state.get_edge("hop3", "HOP", e, lock));
		BOOST_CHECK(net_state.get_edge("HOP", "hop5", e, lock));
	}

	{
		BlockReadLock lock(net_state);
		NetVertex v1, v5;
		BOOST_CHECK(net_state.get_node("hop1", v1, lock));
		BOOST_CHECK(net_state.get_node("hop5", v5, lock));
	}
}

BOOST_AUTO_TEST_CASE ( net_state_aggregate_line_cycle_bidirectional )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v1, v2, v3, v4, v5;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("hop1", v1, lock));
		BOOST_CHECK(net_state.add_node("hop2", v2, lock));
		BOOST_CHECK(net_state.add_node("hop3", v3, lock));
		BOOST_CHECK(net_state.add_node("hop4", v4, lock));
		BOOST_CHECK(net_state.add_node("hop5", v5, lock));
		BOOST_CHECK(net_state.add_edge("hop1", "hop2", e, lock));
		BOOST_CHECK(net_state.add_edge("hop2", "hop3", e, lock));
		BOOST_CHECK(net_state.add_edge("hop3", "hop4", e, lock));
		BOOST_CHECK(net_state.add_edge("hop4", "hop5", e, lock));
		BOOST_CHECK(net_state.add_edge("hop5", "hop4", e, lock));
		BOOST_CHECK(net_state.add_edge("hop4", "hop3", e, lock));
		BOOST_CHECK(net_state.add_edge("hop3", "hop2", e, lock));
		BOOST_CHECK(net_state.add_edge("hop2", "hop1", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 5);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 8);
		NetVertexSet v_set;
		v_set.insert(v2);
		v_set.insert(v4);
		NetVertex vh;
		BOOST_CHECK(net_state.aggregate_node(v_set, "HOP", vh, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 4);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 6);
		BOOST_CHECK(net_state.get_edge("hop1", "HOP", e, lock));
		BOOST_CHECK(net_state.get_edge("HOP", "hop1", e, lock));
		BOOST_CHECK(net_state.get_edge("HOP", "hop3", e, lock));
		BOOST_CHECK(net_state.get_edge("hop3", "HOP", e, lock));
		BOOST_CHECK(net_state.get_edge("hop5", "HOP", e, lock));
		BOOST_CHECK(net_state.get_edge("HOP", "hop5", e, lock));
	}

	{
		BlockReadLock lock(net_state);
		NetVertex v1, v5;
		BOOST_CHECK(net_state.get_node("hop1", v1, lock));
		BOOST_CHECK(net_state.get_node("hop5", v5, lock));
	}
}

BOOST_AUTO_TEST_CASE ( net_state_get_edge_vert_name )
{
	NetState net_state;

	{
		BlockWriteLock lock(net_state);
		NetVertex v;
		NetEdge e;
		BOOST_CHECK(net_state.add_node("src", v, lock));
		BOOST_CHECK(net_state.add_node("dst", v, lock));
		BOOST_CHECK(net_state.add_edge("src", "dst", e, lock));
		BOOST_CHECK_EQUAL(net_state.get_num_nodes(lock), 2);
		BOOST_CHECK_EQUAL(net_state.get_num_edges(lock), 1);
	}

	{
		BlockReadLock lock(net_state);
		NetEdge e;
		BOOST_CHECK(net_state.get_edge("src", "dst", e, lock));
		NetVertexName from, to;
		net_state.get_edge_vert_name(e, from, to, lock);
		BOOST_CHECK_EQUAL(from, "src");
		BOOST_CHECK_EQUAL(to, "dst");
	}
}
