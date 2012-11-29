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


#ifndef NET_STATE_H
#define NET_STATE_H

#include <boost/shared_ptr.hpp>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>
#include <vector>
#include <string>
#include <p4p/pid.h>
#include <p4pserver/locking.h>
#include <p4pserver/dist_obj.h>
#include <p4pserver/compiler.h>

/*
 * FIXME: Support multigraphs?
 */

typedef std::string NetVertexName;
typedef std::string NetEdgeName;

/*
 * Vertex and Edge properties
 */
struct netvertex_external_t  { typedef boost::vertex_property_tag kind; };
struct netvertex_name_t  { typedef boost::vertex_property_tag kind; };
struct netvertex_pid_t  { typedef boost::vertex_property_tag kind; };
struct netedge_name_t  { typedef boost::edge_property_tag kind; };
struct netedge_traffic_t  { typedef boost::edge_property_tag kind; };
struct netedge_capacity_t { typedef boost::edge_property_tag kind; };
typedef boost::property<netvertex_external_t, bool,		/* Vertex is external (i.e., interdomain)? */
	boost::property<netvertex_name_t, NetVertexName,	/* Vertex name */
	boost::property<netvertex_pid_t, p4p::PID	/* Vertex's associated pid (used during view updates) */
	> > > NetGraphVertexProps;
typedef boost::property<netedge_name_t, NetEdgeName,		/* Edge name */
	boost::property<netedge_traffic_t, double,		/* Edge capacity */
	boost::property<netedge_capacity_t, double		/* Edge capacity */
	> > > NetGraphEdgeProps;

/*
 * Graph type.
 * NOTE: It is important that the vertex list be a type that
 *       does not cause vertex descriptors to become invalidated
 *       by graph modifications.  This property is upheld by listS.
 */
typedef boost::adjacency_list<
		boost::setS,					/* Data structure for edge lists */
		boost::listS,					/* Data structure for vertex lists */
		boost::bidirectionalS,				/* Directed graph */
		NetGraphVertexProps,				/* Vertex properties */
		NetGraphEdgeProps				/* Edge properties */
	> NetGraph;

/*
 * Convenience types
 */
typedef boost::graph_traits<NetGraph>::vertex_descriptor NetVertex;
typedef boost::graph_traits<NetGraph>::edge_descriptor NetEdge;
typedef boost::graph_traits<NetGraph>::vertex_iterator NetVertexItr;
typedef boost::graph_traits<NetGraph>::edge_iterator NetEdgeItr;
typedef boost::graph_traits<NetGraph>::in_edge_iterator NetInEdgeItr;
typedef boost::graph_traits<NetGraph>::out_edge_iterator NetOutEdgeItr;

typedef std::pair<NetVertexItr, NetVertexItr> NetVertexItrPair;
typedef std::pair<NetEdgeItr, NetEdgeItr> NetEdgeItrPair;
typedef std::pair<NetInEdgeItr, NetInEdgeItr> NetInEdgeItrPair;
typedef std::pair<NetOutEdgeItr, NetOutEdgeItr> NetOutEdgeItrPair;
typedef std::vector<NetVertex> NetVertexVector;
typedef std::pair<NetVertex, NetVertex> NetVertexPair;
typedef std::set<NetVertex> NetVertexSet;
typedef std::vector<NetVertex> NetVertexVector;
typedef std::set<NetVertexName> NetVertexNameSet;
typedef std::vector<NetVertexName> NetVertexNameVector;
typedef std::vector<NetEdge> NetEdgeVector;

typedef std::tr1::unordered_map<NetVertex, unsigned int, boost::hash<NetVertex> > NetVertexIndexMap;
typedef std::tr1::unordered_map<NetVertex, NetVertex, boost::hash<NetVertex> > NetVertexPredMap;
typedef std::map<NetEdge, double> NetEdgeWeightMap;

/*
 * Specialized p4p::PID* that is pinned to at least one vertex
 */
class p4p_common_server_EXPORT PinnedPID : public p4p::PID
{
public:
	PinnedPID(const p4p::PID&  l, const NetVertex& v)
		: PID(l)
	{
		v_set_.insert(v);
	}
	PinnedPID(const p4p::PID&  l, const NetVertexSet& v_set)
		: PID(l), v_set_(v_set)
	{
		if (v_set_.empty())
			throw std::runtime_error("PinnedPID constructed with empty vertex set");
	}

	const NetVertex& get_vertex() const { return *v_set_.begin(); }
	const NetVertexSet& get_vertices() const { return v_set_; }
private:
	PinnedPID() {}
	NetVertexSet v_set_;
};

typedef std::tr1::unordered_set<PinnedPID, boost::hash<p4p::PID> > PinnedPIDSet;

class NetState;
typedef boost::shared_ptr<NetState> NetStatePtr;
typedef boost::shared_ptr<const NetState> NetStateConstPtr;

class p4p_common_server_EXPORT NetState : public DistributedObject
{
public:
	NetState(const bfs::path& dist_file = bfs::path());
	virtual ~NetState();

	bool get_node(const NetVertexName& name, NetVertex& result, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
	
		NameToVertexMap::const_iterator i = name_vert_map_.find(name);
		if (i == name_vert_map_.end())
			return false;
	
		result = i->second;
		return true;
	}

	bool add_node(const NetVertexName& name, NetVertex& result, const WritableLock& lock);
	bool remove_node(const NetVertexName& name, const WritableLock& lock);
	void remove_node(const NetVertex& vertex, const WritableLock& lock);

	bool aggregate_node(const NetVertexSet& vertices, const std::string& name, NetVertex& result, const WritableLock& lock);

	bool get_edge(const NetEdgeName& name, NetEdge& result, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());

		NameToEdgeMap::const_iterator i = name_edge_map_.find(name);
		if (i == name_edge_map_.end())
			return false;

		result = i->second;
		return true;
	}

	bool get_edge(const NetVertexName& src, const NetVertexName& dst, NetEdge& result, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
	
		NetVertex v_src;
		if (!get_node(src, v_src, lock))
			return false;
	
		NetVertex v_dst;
		if (!get_node(dst, v_dst, lock))
			return false;
	
		return get_edge(v_src, v_dst, result, lock);	
	}

	bool get_edge(const NetVertex& src, const NetVertex& dst, NetEdge& result, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
	
		std::pair<NetEdge, bool> res = edge(src, dst, graph_);
		if (!res.second)
			return false;
	
		result = res.first;
		return true;
	}

	bool add_edge(const NetVertexName& src, const NetVertexName& dst, NetEdge& result, const WritableLock& lock);
	bool add_edge(const NetVertex& src, const NetVertex& dst, NetEdge& result, const WritableLock& lock);
	bool remove_edge(const NetVertexName& src, const NetVertexName& dst, const WritableLock& lock);
	void remove_edge(const NetEdge& e, const WritableLock& lock);

	void get_edge_vert(const NetEdge& edge, NetVertex& result_src, NetVertex& result_dst, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		result_src = boost::source(edge, graph_);
		result_dst = boost::target(edge, graph_);
	}

	void get_edge_vert_name(const NetEdge& edge, NetVertexName& result_src, NetVertexName& result_dst, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		NetVertex from, to;
		get_edge_vert(edge, from, to, lock);
		result_src = get_name(from, lock);
		result_dst = get_name(to, lock);
	}

	double get_capacity(const NetEdge& edge, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netedge_capacity, graph_), edge);
	}

	void set_capacity(const NetEdge& edge, double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		boost::put(boost::get(netedge_capacity, graph_), edge, value);
		changed(lock);
	}

	double get_traffic(const NetEdge& edge, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netedge_traffic, graph_), edge);
	}

	void set_traffic(const NetEdge& edge, double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		boost::put(boost::get(netedge_traffic, graph_), edge, value);
		changed(lock);
	}

	bool get_external(const NetVertex& vertex, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netvertex_external, graph_), vertex);
	}

	void set_external(const NetVertex& vertex, bool value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		boost::put(boost::get(netvertex_external, graph_), vertex, value);
		changed(lock);
	}

	const NetVertexName& get_name(const NetVertex& vertex, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netvertex_name, graph_), vertex);
	}

	void set_name(const NetVertex& vertex, const NetVertexName& value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		name_vert_map_.erase(get_name(vertex, lock));
		boost::put(boost::get(netvertex_name, graph_), vertex, value);
		name_vert_map_.insert(std::make_pair(value, vertex));
		changed(lock);
	}

	const NetEdgeName& get_name(const NetEdge& edge, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netedge_name, graph_), edge);
	}

	void set_name(const NetEdge& edge, const NetEdgeName& value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		name_edge_map_.erase(get_name(edge, lock));
		boost::put(boost::get(netedge_name, graph_), edge, value);
		name_edge_map_.insert(std::make_pair(value, edge));
		changed(lock);
	}

	const p4p::PID&  get_pid(const NetVertex& vertex, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::get(boost::get(netvertex_pid, graph_), vertex);
	}

	void set_pid(const NetVertex& vertex, const p4p::PID&  value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		boost::put(boost::get(netvertex_pid, graph_), vertex, value);
		changed(lock);
	}

	unsigned int get_num_nodes(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::num_vertices(graph_);
	}

	unsigned int get_num_edges(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::num_edges(graph_);
	}

	NetVertexItrPair get_nodes(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::vertices(graph_);
	}

	NetEdgeItrPair get_edges(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::edges(graph_);
	}

	NetInEdgeItrPair get_in_edges(const NetVertex& dst, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::in_edges(dst, graph_);
	}

	NetOutEdgeItrPair get_out_edges(const NetVertex& src, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return boost::out_edges(src, graph_);
	}

	void compute_shortest_paths(const NetVertex& src,
				    NetVertexIndexMap& vert_idxs,
				    NetVertexPredMap& vert_preds,
				    NetEdgeWeightMap& edge_weights,
				    const ReadableLock& lock) const;

protected:
	virtual LocalObjectPtr create_empty_instance() const { return NetStatePtr(new NetState()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
#endif

private:
	typedef std::map<NetVertexName, NetVertex> NameToVertexMap;
	typedef std::map<NetEdgeName, NetEdge> NameToEdgeMap;

	static const netvertex_external_t netvertex_external;
	static const netvertex_name_t netvertex_name;
	static const netvertex_pid_t netvertex_pid;
	static const netedge_name_t netedge_name;
	static const netedge_capacity_t netedge_capacity;
	static const netedge_traffic_t netedge_traffic;

	void refresh_names(const WritableLock& lock);

	NetGraph graph_;
	NameToVertexMap name_vert_map_;
	NameToEdgeMap name_edge_map_;
};

#endif
