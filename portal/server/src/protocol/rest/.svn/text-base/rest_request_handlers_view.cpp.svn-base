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

const char* get_view_name(RESTRequestState* req)
{
	const char* v = req->get_qsargv("view");
	return v ? v : DEFAULT_VIEW_NAME;
}

int RESTHandler::GetPIDsWrite(GetPIDsState* data, uint64_t pos, char *buf, int max)
{
	if (data->addrs.empty())
		return -1;

	const PIDMap& prefixes		= *data->view->get()->get_prefixes(data->view->get_view_lock());
	const ReadableLock& prefixes_lock	= data->view->get_prefixes_lock();

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	while (!data->addrs.empty())
	{
		const p4p::IPPrefix& ip = data->addrs.front();
		const p4p::PID* l = prefixes.lookup(ip, prefixes_lock);

		if (!(rsp << ip << '\t' << (l ? *l : p4p::PID::DEFAULT) << "\r\n"))
			return rsp.get_mark();
		rsp.mark();

		data->addrs.pop_front();
	}
	RESPONSE_WRITE_END

	/* We're done with this chunk */
	return rsp.get_mark();
}

bool RESTHandler::GetPIDsProcess(PortalRESTServer* server, RESTRequestState* state, GetPIDsState* data, RequestStream& req)
{
	/* Gather all of the lookup results */
	p4p::IPPrefix ip;
	while (req >> ip)
	{
		data->addrs.push_back(ip);
		req.mark();
	}
	REQUEST_READ_ERR(req, state);

	return true;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return true;
}

void RESTHandler::GetPIDsFinish(PortalRESTServer* server, RESTRequestState* state, GetPIDsState* data)
{
	if (!state->get_qsargv("admin"))
	{
		try
		{
			ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
			if (!view)
				goto invalid_view;

			data->view = new GetPIDsViewState(view);
		}
		catch (TryLockFailed& e)
		{
			return;
		}
	}
	else
	{
		ADMIN_CLIENT_TOKEN_METHOD(server, state,
			data->view = new GetPIDsViewState(boost::dynamic_pointer_cast<View>(admin_view->get_view_read().first),
							  admin_view->get_view_read().second,
							  boost::shared_ptr<NoLock>(),
							  admin_view->get_prefixes_read().second);
		)
	}

	/* If no IP address in request, then insert requesting client's address */
	if (data->addrs.empty())
	{
		p4p::IPPrefix sender;
	        if (!state->get_client_addr(sender))
			goto internal_error;
		data->addrs.push_back(sender);
	}

	state->set_callback_response((RESTContentReaderCallback)GetPIDsWrite);
	state->add_response_header(HDR_CACHE_CONTROL, "max-age=" + boost::lexical_cast<std::string>(data->view->get()->get_pid_ttl(data->view->get_view_lock())));
	state->add_response_header(HDR_PIDMAP_SEQNO, boost::lexical_cast<std::string>(data->view->get()->get_prefixes(data->view->get_view_lock())->get_version(data->view->get_prefixes_lock())));
	return;

invalid_view:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return;

internal_error:
	state->set_empty_response(MHD_HTTP_INTERNAL_SERVER_ERROR);
	return;
}

void RESTHandler::GetPIDsFree(GetPIDsState* data)
{
	delete data;
}

int RESTHandler::GetPIDMapWrite(GetPIDMapState* data, uint64_t pos, char *buf, int max)
{
	unsigned int& i = data->i;
	unsigned int& j = data->j;

	unsigned int pids_size = data->pids.size();

	if (i >= pids_size)
		return -1;

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	for ( ; i < pids_size; ++i)
	{
		switch (data->pid_state)
		{
		case 0:
		{
			if (!(rsp << data->pids[i].first << '\t' << data->pids[i].second.size()))
				return rsp.get_mark();
			rsp.mark();
			data->pid_state = 1;
		}
		case 1:
		{
			unsigned int prefixes_size = data->pids[i].second.size();
			for ( ; j < prefixes_size; ++j)
			{
				if (!(rsp << '\t' << data->pids[i].second[j]))
					return rsp.get_mark();
				rsp.mark();
			}
			j = 0;
			data->pid_state = 2;
		}
		case 2:
		{
			if (!(rsp << "\r\n"))
				return rsp.get_mark();
			rsp.mark();
			data->pid_state = 0;
		}
		}
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

bool RESTHandler::GetPIDMapProcess(PortalRESTServer* server, RESTRequestState* state, GetPIDMapState* data, RequestStream& req)
{
	/* Gather all of the PIDs */
	p4p::PID p;
	while (req >> p)
	{
		data->pids.push_back(std::make_pair(p, std::vector<p4p::IPPrefix>()));
		req.mark();
	}
	REQUEST_READ_ERR(req, state);

	return true;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return true;
}

void RESTHandler::GetPIDMapFinish(PortalRESTServer* server, RESTRequestState* state, GetPIDMapState* data)
{
	unsigned int ttl = 0;
	unsigned int seqno = 0;

	if (!state->get_qsargv("admin"))
	{
		try
		{
			ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
			if (!view)
				goto invalid_view;

			GetPIDMapViewState view_state(view);
			ttl = view_state.get()->get_pid_ttl(view_state.get_view_lock());
			seqno = view_state.get()->get_prefixes(view_state.get_view_lock())->get_version(view_state.get_prefixes_lock());
			view_state.get()->get_prefixes(view_state.get_view_lock())->enumerate(data->pids, view_state.get_prefixes_lock());
		}
		catch (TryReadLock& e)
		{
			return;
		}
	}
	else
	{
		ADMIN_CLIENT_TOKEN_METHOD(server, state,
			GetPIDMapViewState view_state(boost::dynamic_pointer_cast<View>(admin_view->get_view_read().first),
						      admin_view->get_view_read().second,
						      boost::shared_ptr<NoLock>(),
						      admin_view->get_prefixes_read().second);
			ttl = view_state.get()->get_pid_ttl(view_state.get_view_lock());
			seqno = view_state.get()->get_prefixes(view_state.get_view_lock())->get_version(view_state.get_prefixes_lock());
			view_state.get()->get_prefixes(view_state.get_view_lock())->enumerate(data->pids, view_state.get_prefixes_lock());
		)
	}

	state->set_callback_response((RESTContentReaderCallback)GetPIDMapWrite);
	state->add_response_header(HDR_CACHE_CONTROL, "max-age=" + boost::lexical_cast<std::string>(ttl));
	state->add_response_header(HDR_PIDMAP_SEQNO, boost::lexical_cast<std::string>(seqno));
	return;

invalid_view:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return;
}

void RESTHandler::GetPIDMapFree(GetPIDMapState* data)
{
	delete data;
}

int RESTHandler::GetCostsWrite(RESTHandler::GetCostsState* data, uint64_t pos, char *buf, int max)
{

	if (data->pids_cur == data->pids.end())
		return -1;

	ResponseStream rsp(buf, max);
	RESPONSE_WRITE_BEGIN
	for ( ; data->pids_cur != data->pids.end(); ++data->pids_cur)
	{
		const GetCostsState::RequestMapEntry& e = *data->pids_cur;
		if (e.second.second.empty())
			continue;

		switch (data->pid_state)
		{
		case 0:
			if (!(rsp << e.first << '\t' << (e.second.first ? "inc-reverse" : "no-reverse") << '\t' << e.second.second.size()))
				return rsp.get_mark();
			rsp.mark();
			data->entry_cur = e.second.second.begin();
			++data->pid_state;
		case 1:
			for ( ; data->entry_cur != e.second.second.end(); ++data->entry_cur)
			{
				if (!(rsp << '\t' << *data->entry_cur << '\t' << p4p::detail::clip(round_int(data->view->get_pdistance(e.first, *data->entry_cur)), View::MIN_PDISTANCE, View::MAX_PDISTANCE)))
					return rsp.get_mark();
				if (e.second.first && !(rsp << '\t' << p4p::detail::clip(round_int(data->view->get_pdistance(*data->entry_cur, e.first)), View::MIN_PDISTANCE, View::MAX_PDISTANCE)))
					return rsp.get_mark();
				rsp.mark();
			}
			++data->pid_state;
		case 2:
			if (!(rsp << "\r\n"))
				return rsp.get_mark();
			rsp.mark();
			data->pid_state = 0;
		}
	}
	RESPONSE_WRITE_END

	return rsp.get_mark();
}

bool RESTHandler::GetCostsProcess(PortalRESTServer* server, RESTRequestState* state, GetCostsState* data, RequestStream& req)
{
	p4p::PID src;
	std::string reverse;
	unsigned int dst_count;
	while (req >> src >> reverse >> dst_count)
	{
		bool reverse_bool;
		if (reverse == "inc-reverse")
			reverse_bool = true;
		else if (reverse == "no-reverse")
			reverse_bool = false;
		else
			goto invalid_argument;

		/* Initialize the entry */
		data->pids[src] = std::make_pair(reverse_bool, p4p::PIDSet());

		if (dst_count == 0)
		{
			/* Flag that we'll need the set of all available PIDs */
			data->need_all = true;
		}
		else
		{
			/* Read explicit list of destinations */

			/* Reset to empty list */
			p4p::PIDSet& dsts = data->pids[src].second;
			dsts.clear();

			/* Read all of the destination PIDs */
			p4p::PID dst;
			for (unsigned int i = 0; i < dst_count; ++i)
			{
				if (!(req >> dst))
					REQUEST_READ_ERR(req, state);
				dsts.insert(dst);
			}
		}

		/* Finished with this entry */
		req.mark();
	}
	REQUEST_READ_ERR(req, state);

	return true;

invalid_argument:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return true;
}

void RESTHandler::GetCostsFinish(PortalRESTServer* server, RESTRequestState* state, GetCostsState* data)
{
	if (!state->get_qsargv("admin"))
	{
		try
		{
			ViewPtr view = GlobalView<TryReadLock>(get_view_name(state))();
			if (!view)
				goto invalid_view;

			data->view = new GetCostsViewState(view);
		}
		catch (TryLockFailed& e)
		{
			return;
		}
	}
	else
	{
		ADMIN_CLIENT_TOKEN_METHOD(server, state,
			data->view = new GetCostsViewState(boost::dynamic_pointer_cast<View>(admin_view->get_view_read().first),
							   admin_view->get_view_read().second,
							   admin_view->get_aggregation_read().second,
							   admin_view->get_prefixes_read().second,
							   admin_view->get_intradomain_routing_read().second,
							   admin_view->get_link_pdistances_read().second,
							   admin_view->get_intradomain_pdistances_read().second,
							   admin_view->get_interdomain_pdistances_read().second);
		)
	}

	{
		GetCostsState::RequestMap& pids = data->pids;
		const PIDMap& pidmap = *data->view->get()->get_prefixes(data->view->get_view_lock());
		const ReadableLock& pidmap_lock = data->view->get_prefixes_lock();

		if (pids.empty())
			data->need_all = true;

		/* Only copy list of all PIDs if needed. Note that all PIDs
		 * in this list are guaranteed to have prefixes (due to PIDMap
		 * data structure) */
		p4p::PIDSet all_pids;
		if (data->need_all)
			pidmap.enumerate_pids(std::inserter(all_pids, all_pids.end()), pidmap_lock);

		/* Fill in all source PIDs if required; reverse flag automatically
		 * set to false since we're returning the full matrix */
		if (pids.empty())
		{
			BOOST_FOREACH(const p4p::PID&  pid, all_pids)
			{
				pids[pid] = std::make_pair(false, p4p::PIDSet());
			}
		}

		BOOST_FOREACH(GetCostsState::RequestMapEntry& e, pids)
		{
			/* Ignore if source PID has no prefixes */
			if (!pidmap.has_prefixes(e.first, pidmap_lock))
			{
				e.second.second.clear();
				continue;
			}

			/* Fill in all destination PIDs where required */
			if (e.second.second.empty())
				e.second.second = all_pids;
		}

		data->pids_cur = data->pids.begin();
	}

	state->set_callback_response((RESTContentReaderCallback)GetCostsWrite);
	state->add_response_header(HDR_CACHE_CONTROL, "max-age=" + boost::lexical_cast<std::string>(data->view->get()->get_pdistance_ttl(data->view->get_view_lock())));
	state->add_response_header(HDR_PIDMAP_SEQNO, boost::lexical_cast<std::string>(data->view->get()->get_prefixes(data->view->get_view_lock())->get_version(data->view->get_prefixes_lock())));
	return;

invalid_view:
	state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
	return;
}

void RESTHandler::GetCostsFree(GetCostsState* data)
{
	delete data;
}
