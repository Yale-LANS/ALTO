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


#ifndef REST_REQUEST_HANDLERS_H
#define REST_REQUEST_HANDLERS_H

#include <map>
#include <p4p/protocol-portal/portal-msgs.h>
#include <p4pserver/protocol_server_rest.h>
#include <p4pserver/pid_map.h>
#include <json_infores.h>

#include "view.h"
#include "admin_net.h"
#include "admin_view.h"

class RESTHandler
{
public:
	// Json from Harry
	static std::string VerTag;
	static InfoResourceDirectory INFO_RES_DIRECTORY;
	static std::set<std::string> CostModeSet;
	static std::set<std::string> CostTypeSet;
	static std::set<std::string> EndPointsPropertySet;

	static void InitInfoRes();
	static const char* GetVerTag() { return VerTag.c_str(); }
	static void UpdateVerTag() {
		uint64_t Ver = atoi( VerTag.c_str() );
		Ver = (uint64_t) time(NULL);
		VerTag.erase();
		std::stringstream convert;
		convert << Ver;
		VerTag = convert.str();
	}

	// Headers from Rich
	static const char* HDR_CACHE_CONTROL;
	static const char* HDR_PIDMAP_SEQNO;

	typedef ProtocolServerREST<PORTAL_MSG_MAX, RESTHandler> PortalRESTServer;

	int operator()(PortalRESTServer* server, RESTRequestState* state) const
	{
		const char* arg;

		/* Check for admin requests */
		if (state->get_argc() >= 1 && strcmp(state->get_argv(0), "admin") == 0)
		{
			if (!parse_request_header_admin(server, state))
				goto invalid_argument;
			return MHD_YES;
		}

		/* Process request from a normal client */
		if (state->get_argc() < 1)
			goto invalid_argument;

		arg = state->get_argv(0);

		/* Harry: The new version (-13) ALTO protocol */
		if (strcmp(arg, "directory") == 0)
			parse_request_header_directory(server, state);
		else if (strcmp(arg, "networkmap") == 0)
			parse_request_header_networkmap(server, state);
		else if (strcmp(arg, "costmap") == 0)
			parse_request_header_costmap(server, state);
		else if (strcmp(arg, "endpoints") == 0)
			parse_request_header_endpoints(server, state);
		/* Legacy View Queries */
		else if (strcmp(arg, "pid") == 0)
			parse_request_header_client(server, state);
		else if (strcmp(arg, "pdistance") == 0)
			parse_request_header_pdistance(server, state);
		else
			goto invalid_argument;

		return MHD_YES;

	invalid_argument:
		ReplyError(state, E_SYNTAX);
		return MHD_YES;
	}

private:
	static int parse_request_header_admin_net_node(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_net_link(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_net(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_pid_modify_prefixes(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_pid_modify_nodes(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_pid_modify(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_pid(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_link_modify_cost(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_link_modify_weight(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_link_modify(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_link(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_prop(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view_pdistance(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin_view(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_admin(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_client(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_pdistance(PortalRESTServer* server, RESTRequestState* state);
	/* Harry Updates for ALTO Protocol v13 */
	static int parse_request_header_directory(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_networkmap(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_costmap(PortalRESTServer* server, RESTRequestState* state);
	static int parse_request_header_endpoints(PortalRESTServer* server, RESTRequestState* state);

	struct NetReaderResponseState
	{
		NetReaderResponseState(AdminNetBasePtr _action) : action(_action), cur_idx(0) {}
		AdminNetBasePtr action;
		unsigned int cur_idx;
	};
	static void NetReaderFree(NetReaderResponseState* data);

	struct ViewsReaderResponseState
	{
		ViewsReaderResponseState(AdminViewsBasePtr _action) : action(_action), cur_idx(0) {}
		AdminViewsBasePtr action;
		unsigned int cur_idx;
	};
	static void ViewsReaderFree(ViewsReaderResponseState* data);

	struct ViewReaderResponseState
	{
		ViewReaderResponseState(AdminViewBasePtr _action) : action(_action), cur_idx(0) {}
		AdminViewBasePtr action;
		unsigned int cur_idx;
	};
	static void ViewReaderFree(ViewReaderResponseState* data);

	struct ViewPrefixesReaderResponseState
	{
		ViewPrefixesReaderResponseState() : cur_idx(0) {}
		p4p::IPPrefixVector prefixes;
		unsigned int cur_idx;
	};
	static void ViewPrefixesReaderFree(ViewPrefixesReaderResponseState* data);

	struct ViewNodesReaderResponseState
	{
		ViewNodesReaderResponseState() : cur_idx(0) {}
		NetVertexNameVector nodes;
		unsigned int cur_idx;
	};
	static void ViewNodesReaderFree(ViewNodesReaderResponseState* data);

	static int AdminNetGetNodesWrite(NetReaderResponseState* cls, uint64_t pos, char *buf, int max);
	static int AdminNetGetLinksWrite(NetReaderResponseState* data, uint64_t pos, char *buf, int max);
	static int AdminViewGetPIDPrefixesWrite(ViewPrefixesReaderResponseState* data, uint64_t pos, char *buf, int max);
	static int AdminViewGetPIDNodesWrite(ViewNodesReaderResponseState* data, uint64_t pos, char *buf, int max);
	static int AdminViewGetLinksWrite(ViewReaderResponseState* data, uint64_t pos, char *buf, int max);
	static int AdminViewGetPIDsWrite(ViewReaderResponseState* data, uint64_t pos, char *buf, int max);

	static void AdminNetGetNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminNetAddNodeFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminNetDeleteNodeFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminNetGetLinksFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminNetAddLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminNetDeleteLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewAddPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewDeletePIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewClearPIDPrefixesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewAddPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewDeletePIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewClearPIDNodesFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetPIDsFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewAddPIDFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewDeletePIDFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetLinksFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewAddLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewDeleteLinkFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetLinkWeightFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewSetLinkWeightFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetLinkCostFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewSetLinkCostFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewGetPropFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewSetPropFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewAddFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewDeleteFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminViewPDistanceUpdateFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminGetTokenFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminCommitFinish(PortalRESTServer* server, RESTRequestState* state, void* data);
	static void AdminCancelFinish(PortalRESTServer* server, RESTRequestState* state, void* data);

	/* Harry: ALTO Error Codes */
	static void ReplyError(RESTRequestState* state, ALTOErrorCode code);
	
	/* Harry: Information Resource Directory */
	struct GetIRDState
	{
		GetIRDState() : ird(NULL), len(0), pos(0) {}
		~GetIRDState() {}
		const char*	ird;
		int		len;
		int		pos;
	};

	static int GetIRDWrite(GetIRDState* data, uint64_t pos, char *buf, int max);
	static bool GetIRDProcess(PortalRESTServer* server, RESTRequestState* state, GetIRDState* data, RequestStream& req);
	static void GetIRDFinish(PortalRESTServer* server, RESTRequestState* state, GetIRDState* data);
	static void GetIRDFree(GetIRDState* data);

	/* Harry: Networkmap */
	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock
		> GetNetMapViewState;


	struct GetNetMapState
	{
		GetNetMapState() : len(0), pos(0) {}
		~GetNetMapState() {}
		std::string	net_map;
		int		len;
		int		pos;
	};

	static int GetNetMapWrite(GetNetMapState* data, uint64_t pos, char *buf, int max);
	static bool GetNetMapProcess(PortalRESTServer* server, RESTRequestState* state, GetNetMapState* data, RequestStream& req);
	static void GetNetMapFinish(PortalRESTServer* server, RESTRequestState* state, GetNetMapState* data);
	static void GetNetMapFree(GetNetMapState* data);
	
	/* Harry: Networkmap Filtered */
	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock
		> GetNetMapFilteredViewState;


	struct GetNetMapFilteredState
	{
		GetNetMapFilteredState() : len(0), pos(0) {}
		~GetNetMapFilteredState() {}
		std::string	net_map;
		int		len;
		int		pos;
		std::string	filter;
	};

	static int GetNetMapFilteredWrite(GetNetMapFilteredState* data, uint64_t pos, char *buf, int max);
	static bool GetNetMapFilteredProcess(PortalRESTServer* server, RESTRequestState* state, GetNetMapFilteredState* data, RequestStream& req);
	static void GetNetMapFilteredFinish(PortalRESTServer* server, RESTRequestState* state, GetNetMapFilteredState* data);
	static void GetNetMapFilteredFree(GetNetMapFilteredState* data);

	/* Harry: Costmap */
	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock
		> GetCostMapViewState;

	struct GetCostMapState
	{
		typedef std::pair<bool, p4p::PIDSet> PIDDests;
		typedef std::map<p4p::PID, PIDDests> RequestMap;
		typedef std::pair<const p4p::PID, PIDDests> RequestMapEntry;
		GetCostMapState(const char* cm, const char* ct)
			: len(0),
			  pos(0),
			  cost_mode(cm),
			  cost_type(ct)
			
		{}
		~GetCostMapState() { }
		int len;
		int pos;
		std::string cost_map;
		std::string cost_mode;
		std::string cost_type;
	};

	static int GetCostMapWrite(GetCostMapState* data, uint64_t pos, char *buf, int max);
	static bool GetCostMapProcess(PortalRESTServer* server, RESTRequestState* state, GetCostMapState* data, RequestStream& req);
	static void GetCostMapFinish(PortalRESTServer* server, RESTRequestState* state, GetCostMapState* data);
	static void GetCostMapFree(GetCostMapState* data);

	/* Harry: Costmap Filtered */
	struct GetCostMapFilteredState
	{
		GetCostMapFilteredState()
			: len(0),
			  pos(0)
		{}
		~GetCostMapFilteredState() { }
		int len;
		int pos;
		std::string cost_map;
		std::string filter;
	};

	static int GetCostMapFilteredWrite(GetCostMapFilteredState* data, uint64_t pos, char *buf, int max);
	static bool GetCostMapFilteredProcess(PortalRESTServer* server, RESTRequestState* state, GetCostMapFilteredState* data, RequestStream& req);
	static void GetCostMapFilteredFinish(PortalRESTServer* server, RESTRequestState* state, GetCostMapFilteredState* data);
	static void GetCostMapFilteredFree(GetCostMapFilteredState* data);

	/* Harry: End Point Property */
	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock
		> GetEndPointsViewState;

	struct GetEndPointsPropertyState
	{
		GetEndPointsPropertyState()
			: len(0),
			  pos(0)
		{}
		~GetEndPointsPropertyState() { }
		int len;
		int pos;
		std::string end_property;
		std::string filter;
	};

	static int GetEndPointsPropertyWrite(GetEndPointsPropertyState* data, uint64_t pos, char *buf, int max);
	static bool GetEndPointsPropertyProcess(PortalRESTServer* server, RESTRequestState* state, GetEndPointsPropertyState* data, RequestStream& req);
	static void GetEndPointsPropertyFinish(PortalRESTServer* server, RESTRequestState* state, GetEndPointsPropertyState* data);
	static void GetEndPointsPropertyFree(GetEndPointsPropertyState* data);

	/* Harry: Endpoint Cost */
	struct GetEndPointsCostState
	{
		GetEndPointsCostState()
			: len(0),
			  pos(0)
		{}
		~GetEndPointsCostState() { }
		int len;
		int pos;
		std::string end_cost;
		std::string filter;
	};

	static int GetEndPointsCostWrite(GetEndPointsCostState* data, uint64_t pos, char *buf, int max);
	static bool GetEndPointsCostProcess(PortalRESTServer* server, RESTRequestState* state, GetEndPointsCostState* data, RequestStream& req);
	static void GetEndPointsCostFinish(PortalRESTServer* server, RESTRequestState* state, GetEndPointsCostState* data);
	static void GetEndPointsCostFree(GetEndPointsCostState* data);

	///////////////////////////////////////////////////

	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const EmptyLock, const NoLock,
			const ReadableLock, const TryReadLock
		> GetPIDsViewState;
	struct GetPIDsState
	{
		GetPIDsState() : view(NULL) {}
		~GetPIDsState() { delete view; }
		GetPIDsViewState* view;
		std::list<p4p::IPPrefix> addrs;
		std::vector<p4p::PID> pids;
	};

	static int GetPIDsWrite(GetPIDsState* data, uint64_t pos, char *buf, int max);
	static bool GetPIDsProcess(PortalRESTServer* server, RESTRequestState* state, GetPIDsState* data, RequestStream& req);
	static void GetPIDsFinish(PortalRESTServer* server, RESTRequestState* state, GetPIDsState* data);
	static void GetPIDsFree(GetPIDsState* data);

	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const EmptyLock, const NoLock,
			const ReadableLock, const TryReadLock
		> GetPIDMapViewState;
	struct GetPIDMapState
	{
		GetPIDMapState() : i(0), j(0), pid_state(0) {}
		std::vector<std::pair<p4p::PID, std::vector<p4p::IPPrefix> > > pids;
		unsigned int i;
		unsigned int j;
		unsigned int pid_state;
	};

	static int GetPIDMapWrite(GetPIDMapState* data, uint64_t pos, char *buf, int max);
	static bool GetPIDMapProcess(PortalRESTServer* server, RESTRequestState* state, GetPIDMapState* data, RequestStream& req);
	static void GetPIDMapFinish(PortalRESTServer* server, RESTRequestState* state, GetPIDMapState* data);
	static void GetPIDMapFree(GetPIDMapState* data);

	typedef ViewWrapper<
			const ReadableLock, const TryReadLock,
			const EmptyLock, const NoLock,
			const ReadableLock, const TryReadLock,
			const EmptyLock, const NoLock,
			const EmptyLock, const NoLock,
			const ReadableLock, const TryReadLock,
			const ReadableLock, const TryReadLock
		> GetCostsViewState;
	struct GetCostsState
	{
		typedef std::pair<bool, p4p::PIDSet> PIDDests;
		typedef std::map<p4p::PID, PIDDests> RequestMap;
		typedef std::pair<const p4p::PID, PIDDests> RequestMapEntry;
		GetCostsState()
			: view(NULL),
			  need_all(false),
			  pid_state(0)
		{}
		~GetCostsState() { delete view; }
		GetCostsViewState* view;
		bool need_all;
		unsigned int pid_state;
		RequestMap pids;
		RequestMap::const_iterator pids_cur;
		p4p::PIDSet::const_iterator entry_cur;
	};

	static int GetCostsWrite(GetCostsState* data, uint64_t pos, char *buf, int max);
	static bool GetCostsProcess(PortalRESTServer* server, RESTRequestState* state, GetCostsState* data, RequestStream& req);
	static void GetCostsFinish(PortalRESTServer* server, RESTRequestState* state, GetCostsState* data);
	static void GetCostsFree(GetCostsState* data);
};

#define ADMIN_METHOD(server, state, x)									\
	do {												\
		try											\
		{											\
			 do { x; } while (0);								\
			 break;										\
		}											\
		catch (admin_error& e)									\
		{											\
			server->get_logger()->warn("%s error: %s", __func__, e.what());			\
			goto invalid;									\
		}											\
	invalid:											\
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);				\
		return;											\
	} while (0);

#define ADMIN_TOKEN_METHOD(server, state, x)								\
	do {												\
		try											\
		{											\
			AdminState::Token token;							\
			if (!AdminState::string_to_token(state->get_argv(1), token))			\
				 goto invalid;								\
			do { x; } while (0);								\
			break;										\
		}											\
		catch (admin_error& e)									\
		{											\
			server->get_logger()->warn("%s error: %s", __func__, e.what());			\
			goto invalid;									\
		}											\
	invalid:											\
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);				\
		return;											\
	} while (0);

#define ADMIN_CLIENT_TOKEN_METHOD(server, state, x)							\
	do {												\
		try											\
		{											\
			AdminState::Token token;							\
			if (!AdminState::string_to_token(state->get_qsargv("admin"), token))		\
				 goto invalid;								\
 			AdminViewBasePtr admin_view = boost::dynamic_pointer_cast<AdminViewBase>(	\
				ADMIN_STATE->txn_apply(token,						\
					AdminViewBasePtr(new ::AdminViewBase(get_view_name(state)))));	\
			if (!admin_view)								\
				goto invalid;								\
			if (!admin_view->get_view_read().first)						\
				goto invalid_view;							\
			do { x; } while (0);								\
			break;										\
		}											\
		catch (admin_error& e)									\
		{											\
			server->get_logger()->warn("%s error: %s", __func__, e.what());			\
			goto invalid;									\
		}											\
	invalid:											\
		state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);				\
		return;											\
	} while (0);

#endif
