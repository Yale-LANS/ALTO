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


#include "rest_request_handlers.h"

#include <boost/foreach.hpp>
#include <boost/iostreams/get.hpp>
#include <boost/iostreams/seek.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <p4pserver/net_state.h>
#include <p4pserver/logging.h>
#include "view.h"
#include "state.h"
#include "admin_net.h"
#include "admin_view.h"

#include <iostream>

bool parse_prefix(const std::string& prefix, const char* len_str, std::string& result)
{
	if (len_str == NULL)
	{
		result = prefix;
		return true;
	}

	try
	{
		unsigned short len = boost::lexical_cast<unsigned short>(len_str);
		result = prefix + '/' + boost::lexical_cast<std::string>(len);
		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		return false;
	}
}

bool parse_qsboolean(RESTHandler::PortalRESTServer* server, RESTRequestState* state, const char* arg)
{
	const char* v = state->get_qsargv(arg);
	if (!v)
		return false;

	return strcasecmp(v, "") == 0
		|| strcasecmp(v, "1") == 0
		|| strcasecmp(v, "yes") == 0
		|| strcasecmp(v, "true") == 0;
}

void RESTHandler::NetReaderFree(NetReaderResponseState* data)
{
	delete data;
}

void RESTHandler::ViewsReaderFree(ViewsReaderResponseState* data)
{
	delete data;
}

void RESTHandler::ViewReaderFree(ViewReaderResponseState* data)
{
	delete data;
}

void RESTHandler::ViewPrefixesReaderFree(ViewPrefixesReaderResponseState* data)
{
	delete data;
}

void RESTHandler::ViewNodesReaderFree(ViewNodesReaderResponseState* data)
{
	delete data;
}

int RESTHandler::AdminNetGetNodesWrite(NetReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	AdminState::ReadLockRecord net_rec = data->action->get_net_read();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);

	if (data->cur_idx == net->get_num_nodes(*net_rec.second))
		return -1;

	/* FIXME: Get rid of the linear search for the next unread item */
	unsigned int i = 0;
	BOOST_FOREACH(const NetVertex& v, net->get_nodes(*net_rec.second))
	{
		/* Skip items we've already written */
		if (i++ != data->cur_idx)
			continue;

		if (!(rsp << net->get_name(v, *net_rec.second) << (net->get_external(v, *net_rec.second) ? "\texternal\r\n" : "\tinternal\r\n")))
			break;
		rsp.mark();

		++data->cur_idx;
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminNetGetNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminNetBasePtr a = boost::dynamic_pointer_cast<AdminNetBase>(ADMIN_STATE->txn_apply(token, AdminNetBasePtr(new ::AdminNetBase())));
	if (!a)
		goto invalid;

	state->set_callback_data(new NetReaderResponseState(a));
	state->set_callback_response((RESTContentReaderCallback)AdminNetGetNodesWrite);
)
}

void RESTHandler::AdminNetAddNodeFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminNetNodeAdd(state->get_argv(4), parse_qsboolean(server, state, "ext")))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminNetDeleteNodeFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminNetNodeDelete(state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

int RESTHandler::AdminNetGetLinksWrite(NetReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	AdminState::ReadLockRecord net_rec = data->action->get_net_read();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);

	if (data->cur_idx == net->get_num_edges(*net_rec.second))
		return -1;

	/* FIXME: Get rid of the linear search for the next unread item */
	unsigned int i = 0;
	BOOST_FOREACH(const NetEdge& e, net->get_edges(*net_rec.second))
	{
		/* Skip items we've already written */
		if (i++ != data->cur_idx)
			continue;

		NetVertexName src, dst;
		net->get_edge_vert_name(e, src, dst, *net_rec.second);

		if (!(rsp << net->get_name(e, *net_rec.second) << '\t' << src << '\t' << dst << "\r\n"))
			break;
		rsp.mark();

		++data->cur_idx;
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminNetGetLinksFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminNetBasePtr a = boost::dynamic_pointer_cast<AdminNetBase>(ADMIN_STATE->txn_apply(token, AdminNetBasePtr(new ::AdminNetBase())));
	if (!a)
		goto invalid;

	state->set_callback_data(new NetReaderResponseState(a));
	state->set_callback_response((RESTContentReaderCallback)AdminNetGetLinksWrite);
)
}

void RESTHandler::AdminNetAddLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* src;
	const char* dst;

	if (!(src = state->get_qsargv("src")))
		goto invalid;

	if (!(dst = state->get_qsargv("dst")))
		goto invalid;

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminNetLinkAdd(state->get_argv(4), src, dst))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminNetDeleteLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminNetLinkDelete(state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

int RESTHandler::AdminViewGetPIDPrefixesWrite(ViewPrefixesReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (data->cur_idx == data->prefixes.size())
		return -1;

	for ( ; data->cur_idx < data->prefixes.size(); ++data->cur_idx)
	{
		if (!(rsp << data->prefixes[data->cur_idx] << "\r\n"))
			break;
		rsp.mark();
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminViewGetPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminViewPIDPrefixBasePtr a = boost::dynamic_pointer_cast<AdminViewPIDPrefixBase>(ADMIN_STATE->txn_apply(token, AdminViewPIDPrefixBasePtr(new ::AdminViewPIDPrefixBase(state->get_argv(2), state->get_argv(4), p4p::IPPrefixSet()))));
	if (!a)
		goto invalid;

	AdminState::ReadLockRecord prefixes_rec = a->get_prefixes_read();
	if (!prefixes_rec.first)
		goto invalid;
	PIDMapPtr prefixes = boost::dynamic_pointer_cast<PIDMap>(prefixes_rec.first);

	ViewPrefixesReaderResponseState* rsp_state = new ViewPrefixesReaderResponseState();
	try
	{
		prefixes->get_prefixes(a->lookup_pid(a->get_pid()), rsp_state->prefixes, *prefixes_rec.second);
	}
	catch (admin_error& e)
	{
		delete rsp_state;
		throw;
	}

	state->set_callback_data(rsp_state);
	state->set_callback_response((RESTContentReaderCallback)AdminViewGetPIDPrefixesWrite);
)
}

void RESTHandler::AdminViewAddPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	p4p::IPPrefixSet prefixes;
	std::string prefix_str;

	if (!parse_prefix(state->get_argv(6), state->get_qsargv("len"), prefix_str))
		goto invalid;

	p4p::IPPrefix prefix(prefix_str);

	if (prefix == p4p::IPPrefix::INVALID)
		goto invalid;

	prefixes.insert(prefix);

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewAddPIDPrefixes(state->get_argv(2), state->get_argv(4), prefixes))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewDeletePIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	p4p::IPPrefixSet prefixes;
	std::string prefix_str;

	if (!parse_prefix(state->get_argv(6), state->get_qsargv("len"), prefix_str))
		goto invalid;

	p4p::IPPrefix prefix = p4p::IPPrefix(prefix_str);
	if (prefix == p4p::IPPrefix::INVALID)
		goto invalid;

	prefixes.insert(prefix);

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewDeletePIDPrefixes(state->get_argv(2), state->get_argv(4), prefixes))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewClearPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewClearPIDPrefixes(state->get_argv(2), state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

int RESTHandler::AdminViewGetPIDNodesWrite(ViewNodesReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (data->cur_idx == data->nodes.size())
		return -1;

	for ( ; data->cur_idx < data->nodes.size(); ++data->cur_idx)
	{
		if (!(rsp << data->nodes[data->cur_idx] << "\r\n"))
			break;
		rsp.mark();
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminViewGetPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminViewPIDNodesBasePtr a = boost::dynamic_pointer_cast<AdminViewPIDNodesBase>(ADMIN_STATE->txn_apply(token, AdminViewPIDNodesBasePtr(new ::AdminViewPIDNodesBase(state->get_argv(2), state->get_argv(4), NetVertexNameSet()))));
	if (!a)
		goto invalid;

	AdminState::ReadLockRecord aggregation_rec = a->get_aggregation_read();
	if (!aggregation_rec.first)
		goto invalid;
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	/* FIXME: Use output iterator instead of requiring copy from set to vector */
	NetVertexNameSet vertices_set;
	aggregation->get_vertices(a->lookup_pid(a->get_pid()), vertices_set, *aggregation_rec.second);
	ViewNodesReaderResponseState* rsp_state = new ViewNodesReaderResponseState();
	std::copy(vertices_set.begin(), vertices_set.end(), std::back_inserter(rsp_state->nodes));

	state->set_callback_data(rsp_state);
	state->set_callback_response((RESTContentReaderCallback)AdminViewGetPIDNodesWrite);
)
}

void RESTHandler::AdminViewAddPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	NetVertexNameSet nodes;
	nodes.insert(state->get_argv(6));

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewAddPIDNodes(state->get_argv(2), state->get_argv(4), nodes))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
	RESTHandler::UpdateVerTag();
)
}

void RESTHandler::AdminViewDeletePIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	NetVertexNameSet nodes;
	nodes.insert(state->get_argv(6));

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewDeletePIDNodes(state->get_argv(2), state->get_argv(4), nodes))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewClearPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewClearPIDNodes(state->get_argv(2), state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

int RESTHandler::AdminViewGetLinksWrite(ViewReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	AdminState::ReadLockRecord aggregation_rec = data->action->get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	const PIDAggregation::LinkRecordsByName& link_records = aggregation->get_link_map(*aggregation_rec.second);

	if (data->cur_idx == link_records.size())
		return -1;

	/* FIXME: Get rid of the linear search for the next unread item */
	unsigned int i = 0;
	BOOST_FOREACH(const PIDAggregation::LinkRecord& r, link_records)
	{
		/* Skip items we've already written */
		if (i++ != data->cur_idx)
			continue;

		std::string src_name, dst_name;
		if (aggregation->reverse_lookup(r.src, src_name, *aggregation_rec.second)
			&& aggregation->reverse_lookup(r.dst, dst_name, *aggregation_rec.second))
		{
			if (!(rsp << r.name << '\t' << src_name << '\t' << dst_name << "\r\n"))
				break;
			rsp.mark();
		}

		++data->cur_idx;
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminViewGetLinksFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminViewBasePtr a = boost::dynamic_pointer_cast<AdminViewBase>(ADMIN_STATE->txn_apply(token, AdminViewBasePtr(new ::AdminViewBase(state->get_argv(2)))));
	if (!a)
		goto invalid;

	if (!a->get_view_read().first)
		goto invalid;

	state->set_callback_data(new ViewReaderResponseState(a));
	state->set_callback_response((RESTContentReaderCallback)AdminViewGetLinksWrite);
)
}

void RESTHandler::AdminViewAddLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* src;
	const char* dst;

	if (!(src = state->get_qsargv("src")))
		goto invalid;

	if (!(dst = state->get_qsargv("dst")))
		goto invalid;

	try
	{
		if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewAddPIDLink(state->get_argv(2), state->get_argv(4), src, dst))))
			goto invalid;
	}
	catch (boost::bad_lexical_cast& e)
	{
		goto invalid;
	}

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewDeleteLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewDeletePIDLink(state->get_argv(2), state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

int RESTHandler::AdminViewGetPIDsWrite(ViewReaderResponseState* data, uint64_t pos, char *buf, int max)
{
	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	AdminState::ReadLockRecord aggregation_rec = data->action->get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	const PIDAggregation::PIDRecordsByPID& pid_records = aggregation->get_pid_map(*aggregation_rec.second);

	if (data->cur_idx == pid_records.size())
		return -1;

	/* FIXME: Get rid of the linear search for the next unread item */
	unsigned int i = 0;
	BOOST_FOREACH(const PIDAggregation::PIDRecord& r, pid_records)
	{
		/* Skip items we've already written */
		if (i++ != data->cur_idx)
			continue;

		if (!(rsp << r.name << '\t' << r.pid << "\r\n"))
			break;
		rsp.mark();

		++data->cur_idx;
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

void RESTHandler::AdminViewGetPIDsFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminViewBasePtr a = boost::dynamic_pointer_cast<AdminViewBase>(ADMIN_STATE->txn_apply(token, AdminViewBasePtr(new ::AdminViewBase(state->get_argv(2)))));
	if (!a)
		goto invalid;

	if (!a->get_view_read().first)
		goto invalid;

	state->set_callback_data(new ViewReaderResponseState(a));
	state->set_callback_response((RESTContentReaderCallback)AdminViewGetPIDsWrite);
)
}

void RESTHandler::AdminViewAddPIDFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* val;
	if (!(val = state->get_qsargv("v")))
		goto invalid;

	try
	{
		if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewAddPID(state->get_argv(2), state->get_argv(4),
									boost::lexical_cast<p4p::PID>(val)))))
			goto invalid;
	}
	catch (boost::bad_lexical_cast& e)
	{
		goto invalid;
	}

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewDeletePIDFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewDeletePID(state->get_argv(2), state->get_argv(4)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewGetLinkCostFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	p4p::PID src;
	p4p::PID dst;

	AdminViewBasePtr a = boost::dynamic_pointer_cast<AdminViewBase>(ADMIN_STATE->txn_apply(token, AdminViewBasePtr(new ::AdminViewBase(state->get_argv(2)))));
	if (!a)
		goto invalid;

	AdminState::ReadLockRecord aggregation_rec = a->get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	AdminState::ReadLockRecord link_pdistances_rec = a->get_link_pdistances_read();
	SparsePIDMatrixPtr link_pdistances = boost::dynamic_pointer_cast<SparsePIDMatrix>(link_pdistances_rec.first);

	if (!aggregation->get_link(state->get_argv(4), src, dst, *aggregation_rec.second))
		goto invalid;

	double cost = link_pdistances->get_by_pid(src, dst, *link_pdistances_rec.second);
	state->set_text_response(MHD_HTTP_OK, std::isnan(cost) ? "dynamic" : boost::lexical_cast<std::string>(cost));
)
}

void RESTHandler::AdminViewSetLinkCostFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* cost;
	if (!(cost = state->get_qsargv("v")))
		goto invalid;

	try
	{
		double cost_val = strcmp(cost, "dynamic") == 0 ? NAN : boost::lexical_cast<double>(cost);
		if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewSetLinkCost(state->get_argv(2), state->get_argv(4), cost_val))))
			goto invalid;
	}
	catch (boost::bad_lexical_cast& e)
	{
		goto invalid;
	}

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewGetLinkWeightFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	p4p::PID src;
	p4p::PID dst;

	AdminViewBasePtr a = boost::dynamic_pointer_cast<AdminViewBase>(ADMIN_STATE->txn_apply(token, AdminViewBasePtr(new ::AdminViewBase(state->get_argv(2)))));
	if (!a)
		goto invalid;

	AdminState::ReadLockRecord aggregation_rec = a->get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	AdminState::ReadLockRecord intradomain_routing_rec = a->get_intradomain_routing_read();
	PIDRoutingPtr intradomain_routing = boost::dynamic_pointer_cast<PIDRouting>(intradomain_routing_rec.first);

	if (!aggregation->get_link(state->get_argv(4), src, dst, *aggregation_rec.second))
		goto invalid;

	state->set_text_response(MHD_HTTP_OK, boost::lexical_cast<std::string>(intradomain_routing->get_weight(src, dst, *intradomain_routing_rec.second)));
)
}

void RESTHandler::AdminViewSetLinkWeightFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* weight;

	if (!(weight = state->get_qsargv("v")))
		goto invalid;

	try
	{
		if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewSetLinkWeight(state->get_argv(2), state->get_argv(4), boost::lexical_cast<double>(weight)))))
			goto invalid;
	}
	catch (boost::bad_lexical_cast& e)
	{
		goto invalid;
	}

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewGetPropFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	AdminViewPropGetPtr a = boost::dynamic_pointer_cast<AdminViewPropGet>(ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewPropGet(state->get_argv(2), state->get_argv(4)))));
	if (!a)
		goto invalid;

	if (!a->get_prop_valid())
		goto invalid;

	state->set_text_response(MHD_HTTP_OK, a->get_value());
)
}

void RESTHandler::AdminViewSetPropFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	const char* val = state->get_qsargv("v");
	if (!val)
		goto invalid;

	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewPropSet(state->get_argv(2), state->get_argv(4), val))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewAddFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewAdd(state->get_argv(2)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewDeleteFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewDelete(state->get_argv(2)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminViewPDistanceUpdateFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_apply(token, AdminActionPtr(new ::AdminViewPDistanceUpdate(state->get_argv(2)))))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminGetTokenFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_METHOD(server, state,
	std::string t_str;

	AdminState::Token t = ADMIN_STATE->txn_begin(AdminState::TXN_COPY_ON_WRITE);
	if (t == AdminState::INVALID_TOKEN)
		goto invalid;

	state->set_text_response(MHD_HTTP_OK, AdminState::token_to_string(t));
)
}

void RESTHandler::AdminCommitFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	if (!ADMIN_STATE->txn_commit(token))
		goto invalid;

	state->set_empty_response(MHD_HTTP_OK);
)
}

void RESTHandler::AdminCancelFinish(PortalRESTServer* server, RESTRequestState* state, void* data)
{
ADMIN_TOKEN_METHOD(server, state,
	ADMIN_STATE->txn_rollback(token);

	state->set_empty_response(MHD_HTTP_OK);
)
}
