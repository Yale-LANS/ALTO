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


#include "p4pserver/net_state.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

NetState::NetState(const bfs::path& dist_file)
	: DistributedObject(dist_file)
{
	after_construct();
}

NetState::~NetState()
{
	before_destruct();
}

#ifdef P4P_CLUSTER
void NetState::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	stream >> graph_;
	refresh_names(lock);

	changed(lock);
}

void NetState::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	
	stream << graph_;
}
#endif

bool NetState::add_node(const NetVertexName& name, NetVertex& result, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*
	 * Ensure it doesn't exist already
	 */
	if (get_node(name, result, lock))
		return false;

	/*
	 * Update graph and bookkeeping
	 */
	result = add_vertex(graph_);
	name_vert_map_.insert(std::make_pair(name, result));
	boost::put(boost::get(netvertex_name, graph_), result, name);

	changed(lock);

	return true;
}

bool NetState::aggregate_node(const NetVertexSet& vertices, const std::string& name, NetVertex& result, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*
	 * Fast-path for when 'vertices' contains only a single element. We just
	 * need to rename the vertex.
	 */
	if (vertices.size() == 1)
	{
		set_name(*vertices.begin(), name, lock);
		result = *vertices.begin();
		return true;
	}

	if (!add_node(name, result, lock))
		return false;

	/*
	 * Fix up the edges
	 */
	BOOST_FOREACH(const NetVertex& v, vertices)
	{
		/*
		 * Copy incoming edges
		 */
		BOOST_FOREACH(const NetEdge& e, boost::in_edges(v, graph_))
		{
			NetVertex e_src = boost::source(e, graph_);
			if (vertices.find(e_src) != vertices.end())
				continue;

			std::pair<NetEdge, bool> e_new = boost::add_edge(e_src, result, graph_);
			if (e_new.second)
			{
				set_traffic(e_new.first, get_traffic(e, lock), lock);
				set_capacity(e_new.first, get_capacity(e, lock), lock);
			}
			else
			{
				set_traffic(e_new.first, get_traffic(e_new.first, lock) + get_traffic(e, lock), lock);
				set_capacity(e_new.first, get_capacity(e_new.first, lock) + get_capacity(e, lock), lock);
			}
		}

		/*
		 * Copy outgoing edges
		 */
		BOOST_FOREACH(const NetEdge& e, boost::out_edges(v, graph_))
		{
			NetVertex e_dst = boost::target(e, graph_);
			if (vertices.find(e_dst) != vertices.end())
				continue;

			std::pair<NetEdge, bool> e_new = boost::add_edge(result, e_dst, graph_);
			if (e_new.second)
			{
				set_traffic(e_new.first, get_traffic(e, lock), lock);
				set_capacity(e_new.first, get_capacity(e, lock), lock);
			}
			else
			{
				set_traffic(e_new.first, get_traffic(e_new.first, lock) + get_traffic(e, lock), lock);
				set_capacity(e_new.first, get_capacity(e_new.first, lock) + get_capacity(e, lock), lock);
			}
		}
	}

	/*
	 * Remove the vertices we aggregated.
	 */
	BOOST_FOREACH(const NetVertex& v, vertices)
	{
		remove_node(v, lock);
	}

	return true;
}

bool NetState::remove_node(const NetVertexName& name, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*
	 * Locate the vertex
	 */
	NetVertex v;
	if (!get_node(name, v, lock))
		return false;

	/*
	 * Update graph and bookkeeping
	 */
	remove_node(v, lock);
	return true;
}

void NetState::remove_node(const NetVertex& vertex, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	NetVertexName v_name = get_name(vertex, lock);

	/*
	 * Update graph and bookkeeping
	 */
	clear_vertex(vertex, graph_);
	remove_vertex(vertex, graph_);
	name_vert_map_.erase(v_name);

	changed(lock);
}

bool NetState::add_edge(const NetVertexName& src, const NetVertexName& dst, NetEdge& result, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*
	 * Find the necessary vertices
	 */
	NetVertex v_src;
	if (!get_node(src, v_src, lock))
		return false;

	NetVertex v_dst;
	if (!get_node(dst, v_dst, lock))
		return false;

	return add_edge(v_src, v_dst, result, lock);
}

bool NetState::add_edge(const NetVertex& src, const NetVertex& dst, NetEdge& result, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	
	/*
	 * Add the edge
	 */
	std::pair<NetEdge, bool> res = boost::add_edge(src, dst, graph_);

	/*
	 * Fail if edge already existed
	 */
	if (!res.second)
		return false;

	/*
	 * Set default properties
	 */
	set_traffic(res.first, 0.0, lock);
	set_capacity(res.first, 1.0, lock);
	result = res.first;

	changed(lock);
	return true;
}

bool NetState::remove_edge(const NetVertexName& src, const NetVertexName& dst, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*
	 * Locate the edge
	 */
	NetEdge e;
	if (!get_edge(src, dst, e, lock))
		return false;

	remove_edge(e, lock);
	return true;
}

void NetState::remove_edge(const NetEdge& e, const WritableLock& lock)
{
	/*
	 * Remove the edge from the graph
	 */
	boost::remove_edge(e, graph_);
}

void NetState::compute_shortest_paths(const NetVertex& src,
				      NetVertexIndexMap& vert_idxs,
				      NetVertexPredMap& vert_preds,
				      NetEdgeWeightMap& edge_weights,
				      const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	boost::associative_property_map<NetVertexIndexMap> vert_idx_map(vert_idxs);
	boost::associative_property_map<NetVertexPredMap> vert_pred_map(vert_preds);
	boost::associative_property_map<NetEdgeWeightMap> edge_weight_map(edge_weights);

	dijkstra_shortest_paths(graph_, src,
				boost::predecessor_map(vert_pred_map)
				.weight_map(edge_weight_map)
				.vertex_index_map(vert_idx_map));
}

void NetState::refresh_names(const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	name_vert_map_.clear();

	BOOST_FOREACH(const NetVertex& v, get_nodes(lock))
	{
		name_vert_map_[get_name(v, lock)] = v;
	}

	BOOST_FOREACH(const NetEdge& e, get_edges(lock))
	{
		name_edge_map_[get_name(e, lock)] = e;
	}

	changed(lock);
}

DistributedObjectPtr NetState::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());

	NetStatePtr new_obj = boost::dynamic_pointer_cast<NetState>(NetState::create_empty_instance());
	new_obj->graph_ = graph_;
	BlockWriteLock new_obj_lock(*new_obj);
	new_obj->refresh_names(new_obj_lock);
	return new_obj;
}
