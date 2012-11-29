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

#include <boost/iostreams/get.hpp>
#include <boost/iostreams/seek.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <stdio.h>
#include <limits.h>
#include <p4p/detail/util.h>
#include "state.h"

extern const char* get_view_name(RESTRequestState* req);

/* Harry: ALTO Error Code */
void RESTHandler::ReplyError(RESTRequestState* state, ALTOErrorCode code)
{
	ErrorResourceEntity ere(code);
	int http_code;
	switch (code)
	{
		case E_SERVICE_UNAVAILABLE:
			http_code = MHD_HTTP_SERVICE_UNAVAILABLE;
			break;
		case E_TEMPORARY_REDIRECT:
			http_code = MHD_HTTP_TEMPORARY_REDIRECT;
			break;
		default:
			http_code = MHD_HTTP_BAD_REQUEST;
	}
	state->set_text_response(http_code, ere.toJson(), MEDIA_TYPE_ERROR_JSON);
}

/* Harry: Information Resource Directory*/
int RESTHandler::GetIRDWrite(GetIRDState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->ird[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();
}

bool RESTHandler::GetIRDProcess(PortalRESTServer* server, RESTRequestState* state, GetIRDState* data, RequestStream& req)
{
	data->ird = INFO_RES_DIRECTORY.getRspString().c_str();
	data->len = INFO_RES_DIRECTORY.getRspString().size();
	req.mark();
	return true;
}

void RESTHandler::GetIRDFinish(PortalRESTServer* server, RESTRequestState* state, GetIRDState* data)
{
	state->set_callback_response((RESTContentReaderCallback)GetIRDWrite, MEDIA_TYPE_IRD);
	return;
}

void RESTHandler::GetIRDFree(GetIRDState* data)
{
	delete data;
}

/* Harry: Network Map */
int RESTHandler::GetNetMapWrite(GetNetMapState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->net_map.c_str()[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();
}

bool RESTHandler::GetNetMapProcess(PortalRESTServer* server, RESTRequestState* state, GetNetMapState* data, RequestStream& req)
{
	/* Gather all of the PIDs */
	std::vector<std::pair<p4p::PID, std::vector<p4p::IPPrefix> > > pids;
	req.mark();

	InfoResourceNetworkMap netmap;
        netmap.setVerTag(GetVerTag());

	try
	{
		ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
		if (!view)
		{
			ReplyError(state, E_SERVICE_UNAVAILABLE);
			return true;
		}
		GetNetMapViewState view_state(view);
		view_state.get()->get_prefixes(view_state.get_view_lock())->enumerate(pids, view_state.get_prefixes_lock());

		for (unsigned int i = 0; i < pids.size(); i++)
		{
			std::string pid_name;
			view_state.get()->get_aggregation(view_state.get_view_lock())->reverse_lookup(pids[i].first, pid_name, view_state.get_aggregation_lock());
			unsigned int prefixes_size = pids[i].second.size();
			for (unsigned int j = 0; j < prefixes_size; j++)
			{
				std::stringstream ip;
				ip << pids[i].second[j];
				std::string ip_name = ip.str();
				netmap.addIP(pid_name.c_str(), ip_name.c_str());
			}
			if (pid_name == "defaultpid")
				netmap.addIP(pid_name.c_str(), "::/0");
		}
		netmap.commit();
	}
	catch (TryReadLock& e)
	{
		state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
		return true;
	}

		data->net_map = InfoResourceEntity::MakeJsonStr(netmap);
	data->len = data->net_map.size();
	return true;
}

void RESTHandler::GetNetMapFinish(PortalRESTServer* server, RESTRequestState* state, GetNetMapState* data)
{
	state->set_callback_response((RESTContentReaderCallback)GetNetMapWrite, MEDIA_TYPE_NETMAP);
	return;
}

void RESTHandler::GetNetMapFree(GetNetMapState* data)
{
	delete data;
}

// Harry: Network Map Filtered
int RESTHandler::GetNetMapFilteredWrite(GetNetMapFilteredState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->net_map.c_str()[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();
}

bool RESTHandler::GetNetMapFilteredProcess(PortalRESTServer* server, RESTRequestState* state, GetNetMapFilteredState* data, RequestStream& req)
{
	/* Gather all of the post data */
	std::string parts;
	while (req >> parts)
	{
		data->filter += parts;
		req.mark();
	}
	return true;
}

void RESTHandler::GetNetMapFilteredFinish(PortalRESTServer* server, RESTRequestState* state, GetNetMapFilteredState* data)
{
	// Harry: Reading POST PIDs
	JsonPIDSet pid_set;
	ALTOErrorCode err_code = pid_set.readJson(data->filter);
	if (err_code != E_OK)
	{
		ReplyError(state, err_code);
		return;
	}

	ViewPtr view;
	std::vector<std::pair<p4p::PID, std::vector<p4p::IPPrefix> > > pids;

	InfoResourceNetworkMap netmap;
        netmap.setVerTag(GetVerTag());

	try
	{
		ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
		if (!view)
		{
			ReplyError(state, E_SERVICE_UNAVAILABLE);
			return;
		}
		GetNetMapFilteredViewState view_state(view);
		view_state.get()->get_prefixes(view_state.get_view_lock())->enumerate(pids, view_state.get_prefixes_lock());


		for (unsigned int i = 0; i < pids.size(); i++)
		{
			std::string pid_name;
			view_state.get()->get_aggregation(view_state.get_view_lock())->reverse_lookup(pids[i].first, pid_name, view_state.get_aggregation_lock());

			if (pid_set.getPIDSet().find(pid_name) == pid_set.getPIDSet().end())
				continue;
			unsigned int prefixes_size = pids[i].second.size();
			for (unsigned int j = 0; j < prefixes_size; j++)
			{
				std::stringstream ip;
				ip << pids[i].second[j];
				std::string ip_name = ip.str();
				netmap.addIP(pid_name.c_str(), ip_name.c_str());
			}
		}
		netmap.commit();

	}
	catch (TryReadLock& e)
	{
		state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
		return;
	}


	data->net_map = InfoResourceEntity::MakeJsonStr(netmap);
	data->len = data->net_map.size();

	state->set_callback_response((RESTContentReaderCallback)GetNetMapFilteredWrite, MEDIA_TYPE_NETMAP);
	return;
}

void RESTHandler::GetNetMapFilteredFree(GetNetMapFilteredState* data)
{
	delete data;
}

// Harry: Cost Map
int RESTHandler::GetCostMapWrite(RESTHandler::GetCostMapState* data, uint64_t pos, char *buf, int max)
{

	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->cost_map[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();

}

bool RESTHandler::GetCostMapProcess(PortalRESTServer* server, RESTRequestState* state, GetCostMapState* data, RequestStream& req)
{
	req.mark();
	return true;
}

void RESTHandler::GetCostMapFinish(PortalRESTServer* server, RESTRequestState* state, GetCostMapState* data)
{
	if (CostModeSet.find(data->cost_mode) == CostModeSet.end())
	{
		ReplyError(state, E_INVALID_COST_MODE);
		return;
	}
	if (CostTypeSet.find(data->cost_type) == CostTypeSet.end())
	{
		ReplyError(state, E_INVALID_COST_TYPE);
		return;
	}

	InfoResourceCostMap ircm;	
	ircm.addCostMode(data->cost_mode);
	ircm.addCostType(data->cost_type);
	ircm.addVertionTag(GetVerTag());

	try
	{
		ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
		if (!view)
		{
			ReplyError(state, E_SERVICE_UNAVAILABLE);
			return; 
		}

		GetCostMapViewState view_state(view);

		PIDMapPtr pidmap = view_state.get()->get_prefixes(view_state.get_view_lock());
		const ReadableLock& pidmap_lock = view_state.get_prefixes_lock();
		p4p::PIDSet all_src, all_dst;
		pidmap->enumerate_pids(std::inserter(all_src, all_src.end()), pidmap_lock);
		pidmap->enumerate_pids(std::inserter(all_dst, all_dst.end()), pidmap_lock);

		SparsePIDMatrixPtr spmp = view_state.get()->get_link_pdistances(view_state.get_view_lock());
		const ReadableLock& ln_pdis_lock = view_state.get_link_pdistances_lock();

		PIDRoutingPtr prp = view_state.get()->get_intradomain_routing(view_state.get_view_lock());
		const ReadableLock& route_lock = view_state.get_intradomain_routing_lock();

		PIDAggregationPtr agg = view_state.get()->get_aggregation(view_state.get_view_lock());
		const ReadableLock& agg_lock = view_state.get_aggregation_lock();

		for (p4p::PIDSet::const_iterator it = all_src.begin(); it != all_src.end(); it++)
		{
		
			std::string src;
			agg->reverse_lookup(*it, src, agg_lock);

			for (p4p::PIDSet::const_iterator it2 = all_dst.begin(); it2 != all_dst.end(); it2++)
			{
				std::string dst;
				agg->reverse_lookup(*it2, dst, agg_lock);
			
				double cost = spmp->get_by_pid(*it, *it2, ln_pdis_lock, -1.0);
				if (cost < 0.0)
					continue;
				if (data->cost_mode == "numerical")
				{
					cost = prp->get_weight(*it, *it2, route_lock);
				}

				ircm.addCost(src, dst, cost);
			}
		}
		ircm.commit();
	}
	catch (TryLockFailed& e)
	{
		state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);	
		return;
	}

	data->cost_map = InfoResourceEntity::MakeJsonStr(ircm);
	data->len = data->cost_map.size();

	state->set_callback_response((RESTContentReaderCallback)GetCostMapWrite, MEDIA_TYPE_COSTMAP);
	return;
}

void RESTHandler::GetCostMapFree(GetCostMapState* data)
{
	delete data;
}

// Harry: Cost Map Filtered
int RESTHandler::GetCostMapFilteredWrite(RESTHandler::GetCostMapFilteredState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->cost_map[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();

}

bool RESTHandler::GetCostMapFilteredProcess(PortalRESTServer* server, RESTRequestState* state, GetCostMapFilteredState* data, RequestStream& req)
{
	/* Gather all of the post data */
	std::string parts;
	while (req >> parts)
	{
		data->filter += parts;
		req.mark();
	}
	return true;
}

void RESTHandler::GetCostMapFilteredFinish(PortalRESTServer* server, RESTRequestState* state, GetCostMapFilteredState* data)
{
	// Harry: Reading POST PIDs
	JsonPIDMatrix pid_mat;
	ALTOErrorCode err_code = pid_mat.readJson(data->filter);
	if (err_code != E_OK)
	{
		ReplyError(state, err_code);
		return;
	}

	if (CostModeSet.find(pid_mat.getCostMode()) == CostModeSet.end())
	{
		ReplyError(state, E_INVALID_COST_MODE);
		return;
	}
	if (CostTypeSet.find(pid_mat.getCostType()) == CostTypeSet.end())
	{
		ReplyError(state, E_INVALID_COST_TYPE);
		return;
	}

	InfoResourceCostMap ircm;	
	ircm.addCostMode(pid_mat.getCostMode());
	ircm.addCostType(pid_mat.getCostType());
	ircm.addVertionTag(GetVerTag());

	try
	{
		ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
		if (!view)
		{
			ReplyError(state, E_SERVICE_UNAVAILABLE);
			return; 
		}

		GetCostMapViewState view_state(view);

		SparsePIDMatrixPtr spmp = view_state.get()->get_link_pdistances(view_state.get_view_lock());
		const ReadableLock& ln_pdis_lock = view_state.get_link_pdistances_lock();

		PIDRoutingPtr prp = view_state.get()->get_intradomain_routing(view_state.get_view_lock());
		const ReadableLock& route_lock = view_state.get_intradomain_routing_lock();

		PIDAggregationPtr agg = view_state.get()->get_aggregation(view_state.get_view_lock());
		const ReadableLock& agg_lock = view_state.get_aggregation_lock();

		p4p::PIDSet all_src, all_dst;
		std::set<std::string>& srcs = pid_mat.getPIDSrcs();
		std::set<std::string>& dsts = pid_mat.getPIDDsts();

		for (std::set<std::string>::const_iterator it = srcs.begin(); it != srcs.end(); it++)
		{
			all_src.insert(agg->lookup(*it, agg_lock));
		}

		for (std::set<std::string>::const_iterator it = dsts.begin(); it != dsts.end(); it++)
		{
			all_dst.insert(agg->lookup(*it, agg_lock));
		}

		for (p4p::PIDSet::const_iterator it = all_src.begin(); it != all_src.end(); it++)
		{
		
			std::string src;
			agg->reverse_lookup(*it, src, agg_lock);

			for (p4p::PIDSet::const_iterator it2 = all_dst.begin(); it2 != all_dst.end(); it2++)
			{
				std::string dst;
				agg->reverse_lookup(*it2, dst, agg_lock);
			
				double cost = spmp->get_by_pid(*it, *it2, ln_pdis_lock, -1.0);
				if (cost < 0.0)
					continue;
				if (pid_mat.getCostMode() == "numerical")
				{
					cost = prp->get_weight(*it, *it2, route_lock);
				}

				if (!pid_mat.inConstraints(cost))
					continue;
				ircm.addCost(src, dst, cost);
			}
		}
		ircm.commit();
	}
	catch (TryLockFailed& e)
	{
		state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
		return;
	}

	data->cost_map = InfoResourceEntity::MakeJsonStr(ircm);
	data->len = data->cost_map.size();

	state->set_callback_response((RESTContentReaderCallback)GetCostMapFilteredWrite, MEDIA_TYPE_COSTMAP);
	return;
}

void RESTHandler::GetCostMapFilteredFree(GetCostMapFilteredState* data)
{
	delete data;
}

// Harry: End Point Property
int RESTHandler::GetEndPointsPropertyWrite(GetEndPointsPropertyState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->end_property[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();
}

bool RESTHandler::GetEndPointsPropertyProcess(PortalRESTServer* server, RESTRequestState* state, GetEndPointsPropertyState* data, RequestStream& req)
{
	/* Gather all of the post data */
	std::string parts;
	while (req >> parts)
	{
		data->filter += parts;
		req.mark();
	}
	return true;
}

void RESTHandler::GetEndPointsPropertyFinish(PortalRESTServer* server, RESTRequestState* state, GetEndPointsPropertyState* data)
{
	JsonEndPointsProperty json_epp;
	ALTOErrorCode err_code = json_epp.readJson(data->filter);
	if (err_code != E_OK)
	{
		ReplyError(state, err_code);
		return;
	}

	std::set<std::string>& prop_set = json_epp.getProperties();
	EndPointsProperty epp;

	for (std::set<std::string>::const_iterator it = prop_set.begin(); it != prop_set.end(); it++)
	{
		if (EndPointsPropertySet.find(*it) == EndPointsPropertySet.end())
		{
			ReplyError(state, E_INVALID_PROPERTY_TYPE);
			return;
		}
		try
		{
			ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
			if (!view)
			{
				ReplyError(state, E_SERVICE_UNAVAILABLE);
				return; 
			}

			GetEndPointsViewState view_state(view);

			PIDMapPtr pidmap = view_state.get()->get_prefixes(view_state.get_view_lock());
			const ReadableLock& pidmap_lock = view_state.get_prefixes_lock();

			PIDAggregationPtr agg = view_state.get()->get_aggregation(view_state.get_view_lock());
			const ReadableLock& agg_lock = view_state.get_aggregation_lock();

			std::set<std::string>& ip_set = json_epp.getEndPoints();
			for(std::set<std::string>::const_iterator it2 = ip_set.begin(); it2 != ip_set.end(); it2++)
			{
				std::string ip(PostFilter::StripIP(*it2));
				const p4p::PID* pid = pidmap->lookup(ip.c_str(), pidmap_lock);
				if (pid == NULL)
					continue; // ignore bad ips
				std::string pid_name;
				agg->reverse_lookup(*pid, pid_name, agg_lock);
				epp.setProp(*it2, pid_name, *it);
			}
		}
		catch (TryLockFailed& e)
		{
			state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
			return;
		}
	}
	
	InfoResourceEndPointsProperty iepp;
	iepp.setVtag(GetVerTag());
	iepp.setMap(epp);

	data->end_property = InfoResourceEntity::MakeJsonStr(iepp);
	data->len = data->end_property.size();
	state->set_callback_response((RESTContentReaderCallback)GetEndPointsPropertyWrite, MEDIA_TYPE_EPPROP);
	return;

}

void RESTHandler::GetEndPointsPropertyFree(GetEndPointsPropertyState* data)
{
	delete data;
}

// Harry: End Point Cost
int RESTHandler::GetEndPointsCostWrite(GetEndPointsCostState* data, uint64_t pos, char *buf, int max)
{
	if (data->pos >= data->len)
		return -1; // stop the writing

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	if (!(rsp << &data->end_cost[data->pos]))
	{	
		data->pos = rsp.get_mark();
		return rsp.get_mark();
	}
	rsp.mark();
	data->pos = rsp.get_mark();
	RESPONSE_WRITE_END
	return rsp.get_mark();
}

bool RESTHandler::GetEndPointsCostProcess(PortalRESTServer* server, RESTRequestState* state, GetEndPointsCostState* data, RequestStream& req)
{
	/* Gather all of the post data */
	std::string parts;
	while (req >> parts)
	{
		data->filter += parts;
		req.mark();
	}
	return true;
}

void RESTHandler::GetEndPointsCostFinish(PortalRESTServer* server, RESTRequestState* state, GetEndPointsCostState* data)
{
	// Harry: Reading POST PIDs
	JsonEndPointsCost json_epcost;
	ALTOErrorCode err_code = json_epcost.readJson(data->filter);
	if (err_code != E_OK)
	{
		ReplyError(state, err_code);
		return;
	}

	if (CostModeSet.find(json_epcost.getCostMode()) == CostModeSet.end())
	{
		ReplyError(state, E_INVALID_COST_MODE);
		return;
	}
	if (CostTypeSet.find(json_epcost.getCostType()) == CostTypeSet.end())
	{
		ReplyError(state, E_INVALID_COST_TYPE);
		return;
	}

	EndPointsCost epcost;	
	epcost.addCostMode(json_epcost.getCostMode());
	epcost.addCostType(json_epcost.getCostType());
	epcost.addVertionTag(GetVerTag());

	try
	{
		ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
		if (!view)
		{
			ReplyError(state, E_SERVICE_UNAVAILABLE);
			return;
		}

		GetEndPointsViewState view_state(view);

		PIDMapPtr pidmap = view_state.get()->get_prefixes(view_state.get_view_lock());
		const ReadableLock& pidmap_lock = view_state.get_prefixes_lock();

		SparsePIDMatrixPtr spmp = view_state.get()->get_link_pdistances(view_state.get_view_lock());
		const ReadableLock& ln_pdis_lock = view_state.get_link_pdistances_lock();

		PIDRoutingPtr prp = view_state.get()->get_intradomain_routing(view_state.get_view_lock());
		const ReadableLock& route_lock = view_state.get_intradomain_routing_lock();

		std::set<std::string>& srcs = json_epcost.getEndPointsSrcs();
		std::set<std::string>& dsts = json_epcost.getEndPointsDsts();

		for (std::set<std::string>::const_iterator sit = srcs.begin(); sit != srcs.end(); sit++)
		{
			const p4p::PID* src_pid = pidmap->lookup(PostFilter::StripIP(*sit).c_str(), pidmap_lock);
			for (std::set<std::string>::const_iterator dit = dsts.begin(); dit != dsts.end(); dit++)
			{
				const p4p::PID* dst_pid = pidmap->lookup(PostFilter::StripIP(*dit).c_str(), pidmap_lock);
				double cost = spmp->get_by_pid(*src_pid, *dst_pid, ln_pdis_lock, -1.0);
				if (cost < 0.0)
					continue; // ignore
				if (json_epcost.getCostMode() == "numerical")
				{
					cost = prp->get_weight(*src_pid, *dst_pid, route_lock);
				}
				if (!json_epcost.inConstraints(cost))
					continue;
				epcost.addCost(*sit, *dit, cost);
			}
		}
		epcost.commit();
	}
	catch (TryLockFailed& e)
	{
		state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
		return;
	}

	data->end_cost = InfoResourceEntity::MakeJsonStr(epcost);
	data->len = data->end_cost.size();

	state->set_callback_response((RESTContentReaderCallback)GetEndPointsCostWrite, MEDIA_TYPE_EPCOST);
	return;
}

void RESTHandler::GetEndPointsCostFree(GetEndPointsCostState* data)
{
	delete data;
}


