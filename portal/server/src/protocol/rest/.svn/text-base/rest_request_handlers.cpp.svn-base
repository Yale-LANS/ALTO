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

#include <stdexcept>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <p4p/ip_addr.h>

#include <vector>
#include <iostream>
#include <sstream>
#include <set>

const char* RESTHandler::HDR_CACHE_CONTROL = "Cache-Control";
const char* RESTHandler::HDR_PIDMAP_SEQNO = "X-P4P-PIDMap";

InfoResourceDirectory RESTHandler::INFO_RES_DIRECTORY = InfoResourceDirectory();
std::string RESTHandler::VerTag = std::string("1266506139");
std::set<std::string> RESTHandler::CostModeSet = std::set<std::string>();
std::set<std::string> RESTHandler::CostTypeSet = std::set<std::string>();
std::set<std::string> RESTHandler::EndPointsPropertySet = std::set<std::string>();

void RESTHandler::InitInfoRes()
{
	ResourceEntry* res = NULL;
	Capability cap;

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        cap.insertCostMode(COST_MODE_NUM);
        cap.insertCostType(COST_TYPE_RTC);
        res->setCapability(cap);
        cap.clear();
        res->insertMediaType(MEDIA_TYPE_COSTMAP);
        res->setUri("/costmap/numerical/routingcost");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        cap.insertCostMode(COST_MODE_ORD);
        cap.insertCostType(COST_TYPE_RTC);
        res->setCapability(cap);
        cap.clear();
        res->insertMediaType(MEDIA_TYPE_COSTMAP);
        res->setUri("/costmap/ordinal/routingcost");

        CostModeSet.insert("numerical");
        CostModeSet.insert("ordinal");
        CostTypeSet.insert("routingcost");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertMediaType(MEDIA_TYPE_NETMAP);
        res->setUri("/networkmap");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertMediaType(MEDIA_TYPE_SEVINFO);
        res->setUri("/serverinfo");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertAccept(MEDIA_TYPE_COSTMAP_FT);
        cap.setCostConstraint(true);
        cap.insertCostMode(COST_MODE_NUM);
        cap.insertCostMode(COST_MODE_ORD);
        cap.insertCostType(COST_TYPE_RTC);
        res->setCapability(cap);
        cap.clear();
        res->insertMediaType(MEDIA_TYPE_COSTMAP);
        res->setUri("/costmap/filtered");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertAccept(MEDIA_TYPE_EPCOST_PM);
        cap.setCostConstraint(true);
        cap.insertCostMode(COST_MODE_NUM);
        cap.insertCostMode(COST_MODE_ORD);
        cap.insertCostType(COST_TYPE_RTC);
        res->setCapability(cap);
        cap.clear();
        res->insertMediaType(MEDIA_TYPE_EPCOST);
        res->setUri("/endpoints/cost");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertAccept(MEDIA_TYPE_EPPROP_PM);
        cap.insertPropType("pid");
        res->setCapability(cap);
        cap.clear();
        res->insertMediaType(MEDIA_TYPE_EPPROP);
        res->setUri("/endpoints/property");
        EndPointsPropertySet.insert("pid");

        res = INFO_RES_DIRECTORY.ResEtryFactory();
        res->insertAccept(MEDIA_TYPE_NETMAP_FT);
        res->insertMediaType(MEDIA_TYPE_NETMAP);
        res->setUri("/networkmap/filtered");

        INFO_RES_DIRECTORY.RefreshJson();
}

// Path: /admin/<token>/net/node/
int RESTHandler::parse_request_header_admin_net_node(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 4
		&& state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)AdminNetGetNodesFinish, (RESTRequestFree)NetReaderFree);
	}
	else if (state->get_argc() == 5)
	{
		/* The last argument will be the node name */
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT){
			state->set_callbacks((RESTRequestFinish)AdminNetAddNodeFinish);
		}
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
		{
			state->set_callbacks((RESTRequestFinish)AdminNetDeleteNodeFinish);
		}
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/net/link/
int RESTHandler::parse_request_header_admin_net_link(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 4
		&& state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)AdminNetGetLinksFinish, (RESTRequestFree)NetReaderFree);
	}
	else if (state->get_argc() == 5)
	{
		/* The last argument will be the link name */
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		{
			state->set_callbacks((RESTRequestFinish)AdminNetAddLinkFinish);
		}
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
		{
			state->set_callbacks((RESTRequestFinish)AdminNetDeleteLinkFinish);
		}
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/net/
int RESTHandler::parse_request_header_admin_net(PortalRESTServer* server, RESTRequestState* state)
{
	const char* arg;

	if (state->get_argc() < 4)
		goto invalid_argument;

	arg = state->get_argv(3);

	if (strcmp(arg, "link") == 0)
		parse_request_header_admin_net_link(server, state);
	else if (strcmp(arg, "node") == 0)
		parse_request_header_admin_net_node(server, state);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/pid/<name>/prefixes/
int RESTHandler::parse_request_header_admin_view_pid_modify_prefixes(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 6)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
			state->set_callbacks((RESTRequestFinish)AdminViewGetPIDPrefixesFinish, (RESTRequestFree)ViewPrefixesReaderFree);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
			state->set_callbacks((RESTRequestFinish)AdminViewClearPIDPrefixesFinish);
		else
			goto invalid_argument;
	}
	else if (state->get_argc() == 7)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
			state->set_callbacks((RESTRequestFinish)AdminViewAddPIDPrefixesFinish);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
			state->set_callbacks((RESTRequestFinish)AdminViewDeletePIDPrefixesFinish);
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/pid/<name>/nodes/
int RESTHandler::parse_request_header_admin_view_pid_modify_nodes(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 6)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
			state->set_callbacks((RESTRequestFinish)AdminViewGetPIDNodesFinish, (RESTRequestFree)ViewNodesReaderFree);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
			state->set_callbacks((RESTRequestFinish)AdminViewClearPIDNodesFinish);
		else
			goto invalid_argument;
	}
	else if (state->get_argc() == 7)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
			state->set_callbacks((RESTRequestFinish)AdminViewAddPIDNodesFinish);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
			state->set_callbacks((RESTRequestFinish)AdminViewDeletePIDNodesFinish);
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/pid/<name>/
int RESTHandler::parse_request_header_admin_view_pid_modify(PortalRESTServer* server, RESTRequestState* state)
{
	const char* arg;

	if (state->get_argc() < 6)
		goto invalid_argument;

	arg = state->get_argv(5);

	if (strcmp(arg, "prefixes") == 0)
		parse_request_header_admin_view_pid_modify_prefixes(server, state);
	else if (strcmp(arg, "nodes") == 0)
		parse_request_header_admin_view_pid_modify_nodes(server, state);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/pid/
int RESTHandler::parse_request_header_admin_view_pid(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 4
		&& state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)AdminViewGetPIDsFinish, (RESTRequestFree)ViewReaderFree);
	}
	else if (state->get_argc() == 5)
	{
		// Adding/deleting PIDs (PID name is argument index 4)
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewAddPIDFinish);
		}
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewDeletePIDFinish);
		}
		else
			goto invalid_argument;
	}
	else if (state->get_argc() > 5)
	{
		parse_request_header_admin_view_pid_modify(server, state);
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/link/<name>/pdistance/
int RESTHandler::parse_request_header_admin_view_link_modify_cost(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() != 6)
		goto invalid_argument;

	if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
		state->set_callbacks((RESTRequestFinish)AdminViewGetLinkCostFinish);
	else if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		state->set_callbacks((RESTRequestFinish)AdminViewSetLinkCostFinish);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/link/<name>/weight/
int RESTHandler::parse_request_header_admin_view_link_modify_weight(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() != 6)
		goto invalid_argument;

	if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
		state->set_callbacks((RESTRequestFinish)AdminViewGetLinkWeightFinish);
	else if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		state->set_callbacks((RESTRequestFinish)AdminViewSetLinkWeightFinish);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/link/<name>/
int RESTHandler::parse_request_header_admin_view_link_modify(PortalRESTServer* server, RESTRequestState* state)
{
	const char* arg;

	if (state->get_argc() < 6)
		goto invalid_argument;

	arg = state->get_argv(5);

	if (strcmp(arg, "weight") == 0)
		parse_request_header_admin_view_link_modify_weight(server, state);
	else if (strcmp(arg, "pdistance") == 0)
		parse_request_header_admin_view_link_modify_cost(server, state);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/link/
int RESTHandler::parse_request_header_admin_view_link(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 4
		   && state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)AdminViewGetLinksFinish, (RESTRequestFree)ViewReaderFree);
	}
	else if (state->get_argc() == 5)
	{
		// Adding/deleting Links (Link name is argument index 4)
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewAddLinkFinish);
		}
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewDeleteLinkFinish);
		}
		else
			goto invalid_argument;
	}
	else if (state->get_argc() > 5)
	{
		parse_request_header_admin_view_link_modify(server, state);
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/prop/
int RESTHandler::parse_request_header_admin_view_prop(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 5)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
			state->set_callbacks((RESTRequestFinish)AdminViewGetPropFinish);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
			state->set_callbacks((RESTRequestFinish)AdminViewSetPropFinish);
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/pdistance/
int RESTHandler::parse_request_header_admin_view_pdistance(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 4)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST)
			state->set_callbacks((RESTRequestFinish)AdminViewPDistanceUpdateFinish);
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /admin/<token>/<view>/
int RESTHandler::parse_request_header_admin_view(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 3)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_PUT)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewAddFinish);
		}
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
		{
			state->set_callbacks((RESTRequestFinish)AdminViewDeleteFinish);
		}
		else
			goto invalid_argument;
	}
	else if (state->get_argc() > 3)
	{
		const char* arg = state->get_argv(3);
		if (strcmp(arg, "pid") == 0)
			parse_request_header_admin_view_pid(server, state);
		else if (strcmp(arg, "link") == 0)
			parse_request_header_admin_view_link(server, state);
		else if (strcmp(arg, "prop") == 0)
			parse_request_header_admin_view_prop(server, state);
		else if (strcmp(arg, "pdistance") == 0)
			parse_request_header_admin_view_pdistance(server, state);
		else
			goto invalid_argument;
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

int RESTHandler::parse_request_header_admin(PortalRESTServer* server, RESTRequestState* state)
{
	/* Ensure admin functions are enabled */
	if (!server->is_enabled(PORTAL_MSG_ADMIN_REQ))
		goto invalid_argument;

	if (state->get_argc() == 1
		&& state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		/* Get a new administration token */
		state->set_callbacks((RESTRequestFinish)AdminGetTokenFinish);
	}
	else if (state->get_argc() == 2)
	{
		if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST)
			state->set_callbacks((RESTRequestFinish)AdminCommitFinish);
		else if (state->get_method() == PortalRESTServer::HTTP_METHOD_DELETE)
			state->set_callbacks((RESTRequestFinish)AdminCancelFinish);
		else
			goto invalid_argument;
	}
	else if (state->get_argc() >= 3)
	{
		/* Token is argument index 1; view to be administered is index 2 */
		const char* view = state->get_argv(2);

		/* The reserved name "net" is reserved for administering network state */
		if (strcmp(view, "net") == 0)
		{
			parse_request_header_admin_net(server, state);
		}
		else
		{
			parse_request_header_admin_view(server, state);
		}
	}
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /directory/
int RESTHandler::parse_request_header_directory(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() != 1)
		goto invalid_argument;
	
	if (state->get_method() != PortalRESTServer::HTTP_METHOD_GET)
		goto invalid_argument;
	
	state->set_callbacks((RESTRequestFinish)GetIRDFinish, (RESTRequestFree)GetIRDFree, new GetIRDState(), (RESTRequestProcess)GetIRDProcess);
	return MHD_YES;

invalid_argument:
	ReplyError(state, E_SYNTAX);
	return MHD_YES;
}

// Path: /networkmap
int RESTHandler::parse_request_header_networkmap(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)GetNetMapFinish, (RESTRequestFree)GetNetMapFree, new GetNetMapState(), (RESTRequestProcess)GetNetMapProcess);
	}
	else if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST &&
		state->get_argc() == 2 && 
		strcmp(state->get_argv(1), "filtered") == 0)
	{
		state->set_callbacks((RESTRequestFinish)GetNetMapFilteredFinish, (RESTRequestFree)GetNetMapFilteredFree, new GetNetMapFilteredState(), (RESTRequestProcess)GetNetMapFilteredProcess);
	}
	else
		goto invalid_argument;
	return MHD_YES;

invalid_argument:
	ReplyError(state, E_SYNTAX);
	return MHD_YES;
}

// Path: /costmap
int RESTHandler::parse_request_header_costmap(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		if (state->get_argc() == 3)
		{
			state->set_callbacks((RESTRequestFinish)GetCostMapFinish, (RESTRequestFree)GetCostMapFree, new GetCostMapState(state->get_argv(1), state->get_argv(2)), (RESTRequestProcess)GetCostMapProcess);
		}
		else
			goto invalid_argument;
	}
	else if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST)
	{
		if (state->get_argc() == 2 && strcmp(state->get_argv(1), "filtered") == 0)
		{
			state->set_callbacks((RESTRequestFinish)GetCostMapFilteredFinish, (RESTRequestFree)GetCostMapFilteredFree, new GetCostMapFilteredState(), (RESTRequestProcess)GetCostMapFilteredProcess);
		}
		else
			goto invalid_argument;
	}
	else 
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	ReplyError(state, E_SYNTAX);
	return MHD_YES;
}

// Path: /endpoints
int RESTHandler::parse_request_header_endpoints(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_method() != PortalRESTServer::HTTP_METHOD_POST || state->get_argc() != 2)
		goto invalid_argument;
	
	if (strcmp(state->get_argv(1), "property") == 0)
		state->set_callbacks((RESTRequestFinish)GetEndPointsPropertyFinish, (RESTRequestFree)GetEndPointsPropertyFree, new GetEndPointsPropertyState(), (RESTRequestProcess)GetEndPointsPropertyProcess);
	else if (strcmp(state->get_argv(1), "cost") == 0)
		state->set_callbacks((RESTRequestFinish)GetEndPointsCostFinish, (RESTRequestFree)GetEndPointsCostFree, new GetEndPointsCostState(), (RESTRequestProcess)GetEndPointsCostProcess);
	else
		goto invalid_argument;

	return MHD_YES;

invalid_argument:
	ReplyError(state, E_SYNTAX);
	return MHD_YES;
}

// Path: /pid/ 
int RESTHandler::parse_request_header_client(PortalRESTServer* server, RESTRequestState* state)
{
	if (state->get_argc() == 1)
	{
		if (!server->is_enabled(PORTAL_MSG_GET_PID_REQ))
			goto invalid_argument;

		if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST
			|| state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
		{
			state->set_callbacks((RESTRequestFinish)GetPIDsFinish, (RESTRequestFree)GetPIDsFree, new GetPIDsState(), (RESTRequestProcess)GetPIDsProcess);
		}
		else
			goto invalid_argument;
	}
	else if (state->get_argc() == 2)
	{
		if (!server->is_enabled(PORTAL_MSG_GET_PIDMAP_REQ))
			goto invalid_argument;

		if (state->get_method() == PortalRESTServer::HTTP_METHOD_GET
			&& strcmp(state->get_argv(1), "map") == 0)
		{
			state->set_callbacks((RESTRequestFinish)GetPIDMapFinish, (RESTRequestFree)GetPIDMapFree, new GetPIDMapState(), (RESTRequestProcess)GetPIDMapProcess);
		}
		else
			goto invalid_argument;
	}

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

// Path: /pdistance/
int RESTHandler::parse_request_header_pdistance(PortalRESTServer* server, RESTRequestState* state)
{
	if (!server->is_enabled(PORTAL_MSG_GET_PDISTANCES_REQ))
		goto invalid_argument;

	if (state->get_argc() != 1)
		goto invalid_argument;

	if (state->get_method() == PortalRESTServer::HTTP_METHOD_POST
		|| state->get_method() == PortalRESTServer::HTTP_METHOD_GET)
	{
		state->set_callbacks((RESTRequestFinish)GetCostsFinish, (RESTRequestFree)GetCostsFree, new GetCostsState(), (RESTRequestProcess)GetCostsProcess);
	}
	else
	{
		goto invalid_argument;
	}

	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return MHD_YES;
}

