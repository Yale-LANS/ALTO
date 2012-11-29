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


#include <string>
#include <stdexcept>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/foreach.hpp>
#include <p4p/protocol-portal/admin.h>
#include "options.h"

using namespace p4p;
using namespace p4p::protocol;
using namespace p4p::protocol::portal;

/*
 * NOTE: These typedefs and graph data structures are mostly copied
 *       from the iTracker code.  See the original code there for
 *       more details.
 */

struct netvertex_external_t  { typedef boost::vertex_property_tag kind; };
struct netvertex_name_t  { typedef boost::vertex_property_tag kind; };
typedef boost::property<netvertex_external_t, bool,		/* Vertex is external (i.e., interdomain)? */
	boost::property<netvertex_name_t, std::string,		/* Vertex name */
	boost::property<boost::vertex_index_t, unsigned int	/* Vertex index (unused, but boost wants it) */
	> > > NetGraphVertexProps;

typedef boost::adjacency_list<
		boost::setS,					/* Data structure for edge lists */
		boost::listS,					/* Data structure for vertex lists */
		boost::directedS,				/* Directed graph */
		NetGraphVertexProps				/* Vertex properties */
	> NetGraph;

typedef boost::graph_traits<NetGraph>::vertex_descriptor NetVertex;
typedef boost::graph_traits<NetGraph>::edge_descriptor NetEdge;

bool is_quote(char c) { return c == '\'' || c == '"'; }

std::string cleanse_name(const std::string& s)
{
	std::string result;
	std::remove_copy_if(s.begin(), s.end(), std::back_inserter(result), is_quote);
	return result;
}

void read_topology(const std::string& server, unsigned short port, NetGraph& topo)
{
	AdminPortalProtocol portal(server, port);

	portal.admin_begin_txn();

	std::map<std::string, NetVertex> node_names;

	/* Read the nodes */
	std::vector<NetNode> net_nodes;
	portal.admin_net_list_nodes(std::back_inserter(net_nodes));
	unsigned int v_idx = 0;
	BOOST_FOREACH(const NetNode& node, net_nodes)
	{
		NetVertex v_desc = boost::add_vertex(topo);
		std::string v_name = cleanse_name(node.name);
		boost::put(boost::get(netvertex_name_t(), topo), v_desc, v_name);
		boost::put(boost::get(netvertex_external_t(), topo), v_desc, node.external);
		boost::put(boost::get(boost::vertex_index, topo), v_desc, v_idx++);
		node_names[v_name] = v_desc;
	}

	/* Read the links */
	std::vector<NamedNetLink> net_links;
	portal.admin_net_list_links(std::back_inserter(net_links));
	BOOST_FOREACH(const NamedNetLink& link, net_links)
	{
		std::map<std::string, NetVertex>::const_iterator v_src_itr = node_names.find(cleanse_name(link.link.src));
		if (v_src_itr == node_names.end())
			throw std::runtime_error("Failed to locate vertex " + link.link.src);
		std::map<std::string, NetVertex>::const_iterator v_dst_itr = node_names.find(cleanse_name(link.link.dst));
		if (v_dst_itr == node_names.end())
			throw std::runtime_error("Failed to locate vertex " + link.link.dst);
		boost::add_edge(v_src_itr->second, v_dst_itr->second, topo);
	}

	portal.admin_cancel_txn();
}


class NetGraphWriter
{
public:
	NetGraphWriter(const NetGraph& g) : g_(g) {}

	void operator()(std::ostream& os, const NetVertex& v) const
	{
		std::string label = boost::get(boost::get(netvertex_name_t(), g_), v);
		bool external = boost::get(boost::get(netvertex_external_t(), g_), v);
		std::string color = external ? "black" : "blue";
		os << "["
		   << "height = 0.40,"
		   << "width = 0.40,"
		   << "fixedsize = true,"
		   << "fontsize = 8,"
		   << "label = \"" << label << "\","
		   << "fontcolor = " << color << ","
		   << "]";
	}

	void operator()(std::ostream& os, const NetEdge& e) const
	{
		os << "["
		   << "arrowsize = 0.35,"
		   << "label = \"\","
		   << "]";
	}

	void operator()(std::ostream& os) const
	{
		os << "node []" << std::endl;
		os << "edge []" << std::endl;
	}

private:
	const NetGraph& g_;
};

int main(int argc, char** argv)
{
	handle_options(argc, argv);

	NetGraph topo;
	try
	{
		read_topology(OPTIONS["server"].as<std::string>(), OPTIONS["port"].as<unsigned short>(), topo);
	}
	catch (P4PProtocolHTTPError& e)
	{
		std::cout << "Portal returned HTTP status code: " << e.get_status() << std::endl;
		return 1;
	}
	boost::write_graphviz(std::cout, topo, NetGraphWriter(topo), NetGraphWriter(topo), NetGraphWriter(topo));

	return 0;
}

