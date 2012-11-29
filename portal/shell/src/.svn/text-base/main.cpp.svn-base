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


/* 
 * P4P portal shell
 *   for configuration and P4P system debugging
 *
 *   Last updated: 02/10/2009
 */

/* TODO: 
 *   1. configuration delete and mofiy
 *   2. configuration file generation and save
 *   3. more help information
 */

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <p4p/detail/util.h>
#include <p4p/protocol-portal/admin.h>

//default configurations
#define DEFAULT_SERVER	"localhost"
#define	DEFAULT_PORT	6672
#define DEFAULT_ISP	"isp.net" 


//shell attributes
const char* PROMPT = "p4p# ";
const int MAX_COMMAND_LENGTH = 4 * 1024 * 1024;		//4MB
const int MAX_HISTORY_MEMORY = 5 *1024 * 1024;		//10MB

// Command
const int CMD_EXIT = -100;
const int CMD_INVALID = -101;

static int txn_auto = 1;
static int verbose_output = 0;
static std::string isp = DEFAULT_ISP;

using namespace p4p;
using namespace p4p::detail;
using namespace p4p::protocol;
using namespace p4p::protocol::portal;

// API
AdminPortalProtocol* api = NULL;


/********************************************************************
 * shell functions
 ********************************************************************/
/*
 * help information
 */
#define FORMAT_SERVER		"server <hostname>[:port]"
#define FORMAT_LOAD		"load <configuration file>"
#define FORMAT_VERBOSE		"verbose {on | off}"
#define FORMAT_TXN_AUTO		"txn auto {on | off}"
#define FORMAT_TXN_BEGIN	"txn begin"
#define FORMAT_TXN_COMMIT	"txn commit"
#define FORMAT_TXN_CANCEL	"txn cancel"
#define FORMAT_ISP		"isp default <ISP>"
//#define	FORMAT_PID		"pid {internal | external} <PID-NUM> <PID-NAME> [\"<description>\"] [prefixes <list of prefixes>]"
#define	FORMAT_PID		"pid {internal | external} <PID-NUM> <PID-NAME> [prefixes <list of prefixes>] [description <text>]"
//#define FORMAT_PID_LINK 	"pid link <PID-NAME> <PID-NAME> <PID-link> [capacity <capacity>] [traffic {static <volume> | dynamic}] routing-weight <weight> [\"<description>\"]"
#define FORMAT_PID_LINK 	"pid link <PID-NAME> <PID-NAME> <PID-LINK> [capacity <capacity>] [traffic {static <volume> | dynamic}] routing-weight <weight> [description <text>]"
//#define	FORMAT_PID_ROUTING 	"pid routing {weights | static}"
#define	FORMAT_PID_ROUTING 	"pid routing weights"
#define	FORMAT_PID_PATH		"pid path <source PID> <destination PID> : <intermediate PID-link> ... <intermediate PID-link>"
#define	FORMAT_PID_TTL		"pid ttl <seconds>"
#define FORMAT_PID_DELETE	"pid del <PID-NAME>"
#define FORMAT_RULE		"dynamic-update-rule {intradomain {MLU | congestion-volume} | interdomain multihoming-cost}"
#define	FORMAT_PDISTANCE_LINK	"pdistance link <PID-link> {static <value (0.0-100.0)> | dynamic}"
#define	FORMAT_PDISTANCE_DEFAULT	"pdistance {intra-pid | inter-pid | pid-link | interdomain} default <value (0.0-100.0)>"
#define FORMAT_PDISTANCE_TTL	"pdistance ttl <seconds>"
#define FORMAT_PDISTANCE_UPDATE	"pdistance update"
#define FORMAT_SHOW_ISP		"show isp"
#define FORMAT_SHOW_TOPOLOGY	"show topology {nodes | links}"
#define FORMAT_SHOW_PID		"show pid {nodes | links}"
#define FORMAT_SHOW_PDISTANCE	"show pdistance [<Source PID> <Destination PID> <Destination PID>...]"
#define FORMAT_SHOW_PIDMAP	"show pidmap"
#define FORMAT_SHOW_GETPID	"show getpid [<IP address> <IP address> ...]"

int help()
{
	printf(	"P4P portal shell for configuration\n"
		"  version 2.1\n\n"
		"\tshell commands:\n"
		"\t\thelp\n"
		"\t\texit\n"
		"\t\t" FORMAT_SERVER "\n"
		"\t\t" FORMAT_LOAD "\n"
		"\t\t" FORMAT_VERBOSE "\n"
		"\ttransaction management commands:\n"
		"\t\t" FORMAT_TXN_AUTO "\n"
		"\t\t" FORMAT_TXN_BEGIN "\n"
		"\t\t" FORMAT_TXN_COMMIT "\n"
		"\t\t" FORMAT_TXN_CANCEL "\n"
		"\tconfiguration commands:\n"
		"\t\t" FORMAT_ISP "\n"
		"\t\t" FORMAT_PID "\n"
		"\t\t" FORMAT_PID_LINK "\n"
		"\t\t" FORMAT_PID_ROUTING "\n"
		"\t\t" FORMAT_PID_TTL "\n"
		"\t\t" FORMAT_PID_DELETE "\n"
		//"\t\t" FORMAT_PID_PATH "\n"
		"\t\t" FORMAT_RULE "\n"
		"\t\t" FORMAT_PDISTANCE_LINK "\n"
		"\t\t" FORMAT_PDISTANCE_DEFAULT "\n"
		"\t\t" FORMAT_PDISTANCE_UPDATE "\n"
		"\t\t" FORMAT_PDISTANCE_TTL "\n"
		"\tshow commands:\n"
		"\t\t" FORMAT_SHOW_ISP "\n"
		"\t\t" FORMAT_SHOW_TOPOLOGY "\n"
		"\t\t" FORMAT_SHOW_PID "\n"
		"\t\t" FORMAT_SHOW_PDISTANCE "\n"
		"\t\t" FORMAT_SHOW_PIDMAP "\n"
		"\t\t" FORMAT_SHOW_GETPID "\n"
		);

	return 0;
}


#define API_ACTION(action)				\
	bool API_ACTION_RESULT = true;			\
	std::string API_ACTION_ERROR;			\
	try						\
	{						\
		action;					\
	}						\
	catch (P4PProtocolError& e)				\
	{						\
		API_ACTION_RESULT = false;		\
		API_ACTION_ERROR = e.what();		\
	}						\
	if (!API_ACTION_RESULT)


/********************************************************************
 * ISP configuration functions
********************************************************************/

/*
 * version
 */
int config_version()
{
	/* TODO: We only handle a single configuration version for now */
	return 0;
}

int load_config(const std::string& file);

bool is_txn_cmd(const std::string& cmd)
{
	std::istringstream str(cmd);
	std::string token;
	if (!(str >> token))
		return false;
	return (token == "txn" || token == "server" || token == "load" || token == "exit" || token == "help");
}

/*
 * begin
 */
int txn_begin()
{
	API_ACTION(api->admin_begin_txn())
	{
		fprintf(stderr, "Failed to begin transaction: %s\n", API_ACTION_ERROR.c_str());
		return -1;
	}

	return 0;
}

/*
 * commit
 */
int txn_commit()
{
	API_ACTION(api->admin_commit_txn())
	{
		fprintf(stderr, "Failed to commit configuration changes: %s\n", API_ACTION_ERROR.c_str());
		return -1;
	}

	return 0;
}


/*
 * rollback
 */
int txn_rollback()
{
	API_ACTION(api->admin_cancel_txn())
	{
		fprintf(stderr, "Failed to rollback configuration changes: %s\n", API_ACTION_ERROR.c_str());
		return -1;
	}

	printf("P4P portal configuration rolled back\n");

	return 0;
}


/* 
 * isp
 */
int config_isp(std::istream& is)
{
	std::string p;

	//default
	if (!(is >> p) || p != "default")
	{
		fprintf(stderr, "Format: " FORMAT_ISP "\n");
		return -1;
	}

	//<ISP>
	if (!(is >> isp))
	{
		fprintf(stderr, "Format: " FORMAT_ISP "\n");
		return -1;
	}

	return 0;	
}


/* 
 * server
 */
int config_server(std::istream& is)
{
	std::string server;
	char host[255];
	unsigned short port = DEFAULT_PORT;

	//<hostname>[:port]
	if (!(is >> server))
	{
		fprintf(stderr, "Format: " FORMAT_SERVER "\n");
		return -1;
	}

	const char *p = server.c_str();
	const char *sep = strchr(p, ':');
	if (sep)
	{
		strncpy(host, p, sep - p);
		host[sep - p] = '\0';
		if (sscanf(sep + 1, "%hu", &port) != 1)
		{
			fprintf(stderr, "Invalid hostname\n");
			return -1;
		}
	}
	else
	{
		strncpy(host, p, 255);
		host[254] = '\0';
	}

	if (api) delete(api);

	api = new AdminPortalProtocol(host, port);

	return 0;	
}


/* 
 * pid link
 */
int config_pid_link(std::istream& is)
{
	//configure PID link:
	//  1. Add the underlying link
	//  2. Configure the PID link

	std::string src, dst, link, attr;
	double val;

	if (!(is >> src >> dst >> link))
		goto format_error;

	//P4P handler: add link and PID link
	{
		API_ACTION(api->admin_net_add_link(NamedNetLink(link, NetLink(src, dst))))
		{
			fprintf(stderr, "Failed to add link %s(%s->%s) to topology: %s\n", link.c_str(), src.c_str(), dst.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	{
		API_ACTION(api->admin_view_add_pidlink("DEFAULT", NamedPIDLink(link, PIDNameLink(src, dst))))
		{
			fprintf(stderr, "Failed to create PID link %s(%s->%s): %s\n", link.c_str(), src.c_str(), dst.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	while (is >> attr)
	{
		if (attr == "capacity")
		{
			/* TODO: Not yet implemented */
			is >> val;
		}
		else if (attr == "traffic")
		{
			if (!(is >> attr))
				goto format_error;

			if (attr == "static")
			{
				/* TODO: Not yet implemented */
				is >> val;
			}
			else if (attr == "dynamic")
			{
				/* TODO: Not yet implemented */
			}
			else
				goto format_error;
		}
		else if (attr == "routing-weight")
		{
			if (!(is >> val))
				goto format_error;

			API_ACTION(api->admin_view_set_pidlink_weight("DEFAULT", link, val))
			{
				fprintf(stderr, "Failed to assign routing weight %lf to PID link %s: %s\n", val, link.c_str(), API_ACTION_ERROR.c_str());
				return -1;
			}
		}
		else if (attr == "description")
		{
		}
		else
			goto format_error;
	}

	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_PID_LINK "\n");
	return -1;
}

/* 
 * pid routing
 */
int config_pid_routing(std::istream& is)
{
	std::string p;
	if (!(is >> p))
		goto format_error;

	//configure PID routing:
	//  set P4P portal routemode to computing shortest path routing

	//weights
	if (p == "weights")
	{
		/* TODO: Set explicitly */
	}
	else if (p == "static")
	{
		/* TODO: Not yet implemented */
	}
	else
		goto format_error;

	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_PID_ROUTING "\n");
	return -1;
}


/* 
 * pid path
 */
int config_pid_path(std::istream& is)
{
	//configure PID routing path:
	//  1. Set P4P portal routemode to static routing
	//  2. Explicitly configure routing paths for each Src PID and Dst PID

	std::string src, dst, sep, hop;
	std::vector<std::string> links;

	if (!(is >> src >> dst))
		goto format_error;

	if (!(is >> sep) || sep != ":")
		goto format_error;

	while (is >> hop)
		links.push_back(hop);

	/* TODO: Not yet implemented */

	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_PID_PATH "\n");
	return -1;
}

/* 
 * pid ttl
 */
int config_pid_ttl(std::istream& is)
{
	//configure PID TTL

	unsigned int ttl;

	if (!(is >> ttl))
		goto format_error;

	{
		API_ACTION(api->admin_view_set_prop("DEFAULT", "pid_ttl", p4p_token_cast<std::string>(ttl)))
		{
			fprintf(stderr, "Failed to set PID TTL (%u): %s\n", ttl, API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_PID_TTL "\n");
	return -1;
}

int config_pid_del(std::istream& is)
{
	std::string name;

	/* Name */
	if (!(is >> name))
		goto format_error_pid;

	//P4P handler: delete prefixes
	{
		API_ACTION(api->admin_view_clear_pid_prefixes("DEFAULT", name))
		{
			fprintf(stderr, "Failed to del prefixes of PID %s in topology: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	//P4P handler: delete node and PID
	{
		API_ACTION(api->admin_view_del_pid_node("DEFAULT", name, name))
		{
			fprintf(stderr, "Failed to del PID %s in topology: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	{
		API_ACTION(api->admin_view_del_pid("DEFAULT", name))
		{
			fprintf(stderr, "Failed to del PID %s: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	{
		API_ACTION(api->admin_net_del_node(name))
		{
			fprintf(stderr, "Failed to del node %s in topology: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	return 0;

format_error_pid:
	fprintf(stderr, "Format: " FORMAT_PID "\n");
	return -1;
}

/* 
 * pid 
 */
int config_pid(std::istream& is)
{
	//configure PID:
	//  1. Add underlying node 
	//  2. Associate PID with underlying node
	//  3. Associate IP prefixes with aggregation-node PID

	std::string token;
	std::string name;
	bool external;
	unsigned int num;

	if (!(is >> token))
		goto format_error_pid;
	if (token == "link")
		return config_pid_link(is);
	else if (token == "routing")
		return config_pid_routing(is);
	else if (token == "path")
		return config_pid_path(is);
	else if (token == "ttl")
		return config_pid_ttl(is);
	else if (token == "del")
		return config_pid_del(is);

	/* PID External or Internal */
	if (token == "internal")
		external = false;
	else if (token == "external")
		external = true;
	else
		goto format_error_pid;

	/* PID Number */
	if (!(is >> num))
		goto format_error_pid;

	/* Name */
	if (!(is >> name))
		goto format_error_pid;

	//P4P handler: new node and PID
	{
		API_ACTION(api->admin_net_add_node(name, external))
		{
			fprintf(stderr, "Failed to add node %s in topology: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	{
		API_ACTION(api->admin_view_add_pid("DEFAULT", NamedPID(name, PID(isp, num, external))))
		{
			fprintf(stderr, "Failed to create PID %s: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	{
		API_ACTION(api->admin_view_add_pid_node("DEFAULT", name, name))
		{
			fprintf(stderr, "Failed to add PID %s in topology: %s\n", name.c_str(), API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	if (!(is >> token))
		return 0;

	if (token == "description")
		return 0;

	if (token == "prefixes")
	{
		/* Read prefixes
		 * TODO: Support IPv6 */
		while (is >> token)
		{
			/* Ignore description */
			if (token == "description")
			{
				break;
			}

			/* Extract prefix length */
			unsigned int len = 32;
			std::string::size_type slash = token.rfind('/');
			if (slash != std::string::npos && slash > 0)
			{
				try
				{
					len = p4p_token_cast<unsigned int>(token.substr(slash + 1));
				}
				catch (std::invalid_argument& e)
				{
					fprintf(stderr, "Invalid prefix %s in PID %s\n", token.c_str(), name.c_str());
					return -1;
				}
				token = token.substr(0, slash);
			}

			API_ACTION(api->admin_view_add_pid_prefix("DEFAULT", name, token, len))
			{
				fprintf(stderr, "Failed to add prefix %s to PID %s: %s\n", token.c_str(), name.c_str(), API_ACTION_ERROR.c_str());
				return -1;
			}
		}
	}
	else
		goto format_error_pid;

	return 0;

format_error_pid:
	fprintf(stderr, "Format: " FORMAT_PID "\n");
	return -1;
}


/* 
 * dynamic-update-rule 
 */
int config_rule(std::istream& is)
{
	std::string token;

	if (!(is >> token))
		goto format_error_rule;

	if (token == "interdomain")
	{
		if (!(is >> token))
			goto format_error_rule;

		if (token == "multihoming-cost")
		{
			/* TODO: Handle this */
		}
		else
			goto format_error_rule;
	}
	else if (token == "intradomain")
	{
		if (!(is >> token))
			goto format_error_rule;

		if (token == "congestion-volume")
		{
			/* TODO: Handle this */
		}
		else if (token == "MLU")
		{
			/* TODO: Handle this */
		}
		else
			goto format_error_rule;
	}
	else
		goto format_error_rule;

	/* TODO: Not Yet Implemented */

	return 0;

format_error_rule:
	fprintf(stderr, "Format: " FORMAT_RULE "\n");
	return -1;
}

/* 
 * pdistance ttl
 */
int config_pdistance_ttl(std::istream& is)
{
	//configure pDistance TTL

	unsigned int ttl;

	if (!(is >> ttl))
		goto format_error;

	{
		API_ACTION(api->admin_view_set_prop("DEFAULT", "pdistance_ttl", p4p_token_cast<std::string>(ttl)))
		{
			fprintf(stderr, "Failed to set pDistance TTL (%u): %s\n", ttl, API_ACTION_ERROR.c_str());
			return -1;
		}
	}

	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_PDISTANCE_TTL "\n");
	return -1;
}



/* 
 * pdistance
 */
int config_pdistance(std::istream& is)
{
	//configure virtual costs:
	//  Configure default costs, link costs

	std::string token, link;
	double pdistance;

	//subkeyword
	if (!(is >> token))
		goto format_error_pdistance_all;

	if (token == "link")
	{
		if (!(is >> link))
			goto format_error_pdistance_link;

		//static | dynamic
		if (!(is >> token))
			goto format_error_pdistance_link;

		if (token == "static")
		{
			//<cost>
			if (!(is >> pdistance) || pdistance < 0.0 || pdistance > 100.0)
				goto format_error_pdistance_link;

			API_ACTION(api->admin_view_set_pidlink_pdistance("DEFAULT", link, PDistanceStatic(pdistance)))
			{
				fprintf(stderr, "Failed to set cost (%lf) for PID link %s: %s\n", pdistance, link.c_str(), API_ACTION_ERROR.c_str());
				return -1;
			}
		} 
		else if (token == "dynamic")
		{
			/* TODO: Not Yet Implemented */
		}
		else
			goto format_error_pdistance_link;
	}
	else if (token == "intra-pid")
	{
		//default
		if (!(is >> token) || token != "default")
			goto format_error_pdistance_default;

		//<cost>
		if (!(is >> pdistance) || pdistance < 0.0 || pdistance > 100.0)
			goto format_error_pdistance_default;

		API_ACTION(api->admin_view_set_prop("DEFAULT", "default_intrapid", p4p_token_cast<std::string>(pdistance)))
		{
			fprintf(stderr, "Failed to set default intra-pid cost (%lf): %s\n", pdistance, API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	else if (token == "inter-pid")
	{
		//default
		if (!(is >> token) || token != "default")
			goto format_error_pdistance_default;

		//<cost>
		if (!(is >> pdistance) || pdistance < 0.0 || pdistance > 100.0)
			goto format_error_pdistance_default;

		API_ACTION(api->admin_view_set_prop("DEFAULT", "default_interpid", p4p_token_cast<std::string>(pdistance)))
		{
			fprintf(stderr, "Failed to set default inter-pid cost (%lf): %s\n", pdistance, API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	else if (token == "pid-link")
	{
		//default
		if (!(is >> token) || token != "default")
			goto format_error_pdistance_default;

		//<cost>
		if (!(is >> pdistance) || pdistance < 0.0 || pdistance > 100.0)
			goto format_error_pdistance_default;

		API_ACTION(api->admin_view_set_prop("DEFAULT", "default_pidlink", p4p_token_cast<std::string>(pdistance)))
		{
			fprintf(stderr, "Failed to set default pid-link cost (%lf): %s\n", pdistance, API_ACTION_ERROR.c_str());
			return -1;
		}
	}
	else if (token == "interdomain")
	{
		if (!(is >> token))
			goto format_error_pdistance_default;

		if (token == "default")
		{
			//<cost>
			if (!(is >> pdistance) || pdistance < 0.0 || pdistance > 100.0)
				goto format_error_pdistance_default;

			API_ACTION(api->admin_view_set_prop("DEFAULT", "default_interdomain", p4p_token_cast<std::string>(pdistance)))
			{
				fprintf(stderr, "Failed to set default interdomain cost (%lf): %s\n", pdistance, API_ACTION_ERROR.c_str());
				return -1;
			}
		}
		else if (token == "exclude" && (is >> token) && token == "intradomain")
		{
			API_ACTION(api->admin_view_set_prop("DEFAULT", "interdomain_includes_intradomain", "0"))
			{
				fprintf(stderr, "Failed to set default interdomain exclude intradomain: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}
		}
		else if (token == "include" && (is >> token) && token == "intradomain")
		{
			API_ACTION(api->admin_view_set_prop("DEFAULT", "interdomain_includes_intradomain", "1"))
			{
				fprintf(stderr, "Failed to set default interdomain include intradomain: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}
		}
		else
			goto format_error_pdistance_default;
	}
	else if (token == "update")
	{
		if (is >> token)
		{
			if (token == "interval")
			{
				unsigned int interval;

				//<interval>
				if (!(is >> interval))
					goto format_error_pdistance_update;

				API_ACTION(api->admin_view_set_prop("DEFAULT", "update_interval", p4p_token_cast<std::string>(interval)))
				{
					fprintf(stderr, "Failed to set pdistance update interval: %s\n", API_ACTION_ERROR.c_str());
					return -1;
				}
			}
			else
				goto format_error_pdistance_update;
		}
		else
		{
			API_ACTION(api->admin_view_update_pdistance("DEFAULT"))
			{
				fprintf(stderr, "Failed to update virtual costs: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}
		}
	}
	else if (token == "ttl")
	{
		return config_pdistance_ttl(is);
	}
	else
		goto format_error_pdistance_all;

	return 0;

format_error_pdistance_default:
	fprintf(stderr, "Format: " FORMAT_PDISTANCE_DEFAULT "\n");
	return -1;

format_error_pdistance_link:
	fprintf(stderr, "Format: " FORMAT_PDISTANCE_LINK "\n");
	return -1;

format_error_pdistance_update:
	fprintf(stderr, "Format: " FORMAT_PDISTANCE_UPDATE "\n");
	return -1;

format_error_pdistance_all:
	fprintf(stderr, "Format: " FORMAT_PDISTANCE_DEFAULT "\n");
	fprintf(stderr, "        " FORMAT_PDISTANCE_LINK "\n");
	fprintf(stderr, "        " FORMAT_PDISTANCE_UPDATE "\n");
	fprintf(stderr, "        " FORMAT_PDISTANCE_TTL "\n");
	return -1;
}


/*
 * Txn: Configuration transactions
 */
int config_txn(std::istream& is)
{
	std::string token;

	//subkeyword
	if (!(is >> token))
		goto format_error_txn_all;

	if (token == "auto")
	{
		if (!(is >> token))
			goto format_error_txn_auto;

		if (token == "on")
		{
			txn_auto = 1;
		}
		else if (token == "off")
		{
			txn_auto = 0;
		}
		else
			goto format_error_txn_auto;
	}
	else if (token == "begin")
	{
		return txn_begin();
	}
	else if (token == "commit")
	{
		return txn_commit();
	}
	else if (token == "rollback")
	{
		return txn_rollback();
	}
	else
		goto format_error_txn_all;

	return 0;

format_error_txn_auto:
	fprintf(stderr, "Format: " FORMAT_TXN_AUTO "\n");
	return -1;

format_error_txn_all:
	fprintf(stderr, "Format: " FORMAT_TXN_AUTO "\n");
	fprintf(stderr, "Format: " FORMAT_TXN_BEGIN "\n");
	fprintf(stderr, "Format: " FORMAT_TXN_COMMIT "\n");
	fprintf(stderr, "Format: " FORMAT_TXN_CANCEL "\n");
	return -1;
}

/*
 * Verbose output setting
 */
int config_verbose(std::istream& is)
{
	std::string token;

	//subkeyword
	if (!(is >> token))
		goto format_error;

	if (token == "on")
	{
		verbose_output = 1;
	}
	else if (token == "off")
	{
		verbose_output = 0;
	}
	else
		goto format_error;
	
	return 0;

format_error:
	fprintf(stderr, "Format: " FORMAT_VERBOSE "\n");
	return -1;
}


/*
 * Show: Configuration Dump
 */
int config_dump(std::istream& is)
{
	std::string token;

	//subkeyword
	if (!(is >> token))
		goto format_error_dump_all;

	if (token == "isp")
	{
		std::vector<NamedPID> pids;
		API_ACTION(api->admin_view_list_pids("DEFAULT", std::back_inserter(pids)))
		{
			fprintf(stderr, "Failed to retrieve ISP identifier: %s\n", API_ACTION_ERROR.c_str());
			return -1;
		}

		if (pids.size() < 1) 
		{
			fprintf(stderr, "ISP identifier is not configured and used\n");
			return -1;
		}

		const p4p::PID&  src = pids.begin()->pid;
		std::cout << src.get_isp() << std::endl;
	}
	else if (token == "topology")
	{
		if (!(is >> token))
			goto format_error_dump_topology;

		if (token == "nodes")
		{
			std::vector<NetNode> nodes;
			API_ACTION(api->admin_net_list_nodes(std::back_inserter(nodes)))
			{
				fprintf(stderr, "Failed to list nodes in topology: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}

			for (std::vector<NetNode>::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
				std::cout << itr->name << '\t' << (itr->external ? "external" : "internal") << std::endl;
		}
		else if (token == "links")
		{
			std::vector<NamedNetLink> links;
			API_ACTION(api->admin_net_list_links(std::back_inserter(links)))
			{
				fprintf(stderr, "Failed to list links in topology: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}

			for (std::vector<NamedNetLink>::const_iterator itr = links.begin(); itr != links.end(); ++itr)
				std::cout << itr->name << '\t' << itr->link.src << '\t' << itr->link.dst << std::endl;
		}
		else
			goto format_error_dump_topology;
	}
	else if (token == "pid")
	{
		if (!(is >> token))
			goto format_error_dump_pid;

		if (token == "nodes")
		{
			std::vector<NamedPID> pids;
			API_ACTION(api->admin_view_list_pids("DEFAULT", std::back_inserter(pids)))
			{
				fprintf(stderr, "Failed to list PID nodes: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}

			for (std::vector<NamedPID>::const_iterator itr = pids.begin(); itr != pids.end(); ++itr)
				std::cout << itr->name << '\t' << itr->pid << std::endl;
		}
		else if (token == "links")
		{
			std::vector<NamedPIDLink> links;
			API_ACTION(api->admin_view_list_pidlinks("DEFAULT", std::back_inserter(links)))
			{
				fprintf(stderr, "Failed to list PID links: %s\n", API_ACTION_ERROR.c_str());
				return -1;
			}

			for (std::vector<NamedPIDLink>::const_iterator itr = links.begin(); itr != links.end(); ++itr)
				std::cout << itr->name << '\t' << itr->link.src << '\t' << itr->link.dst << std::endl;
		}
		else
			goto format_error_dump_pid;
	}
	else if (token == "pdistance")
	{
		typedef std::pair<PID, std::vector<PID> > PIDPair;
		std::vector<PIDPair> pids;

		PID src;
		bool reverse = false;

		if (is >> src)
		{
			std::vector<PID> dsts;
			PID dst;
			while (is >> dst)
			{
				dsts.push_back(dst);
			}
			pids.push_back(std::make_pair(src, dsts));
		}
		if (!is.eof())
			goto format_error_dump_pdistance;

		PDistanceMatrix result;
		P4PPortalProtocolMetaInfo meta;
		API_ACTION(api->get_pdistance("DEFAULT", pids.begin(), pids.end(), reverse, result, &meta))
		{
			fprintf(stderr, "Failed to get pDistances: %s\n", API_ACTION_ERROR.c_str());
			return -1;
		}

		std::set<PID> srcs;
		result.get_srcs(std::inserter(srcs, srcs.end()));
		for (std::set<PID>::const_iterator srcs_itr = srcs.begin(); srcs_itr != srcs.end(); ++srcs_itr)
		{
			const PDistanceMatrix::Row& row = result.get_row(*srcs_itr);
			for (PDistanceMatrix::Row::const_iterator e_itr = row.begin(); e_itr != row.end(); ++e_itr)
				std::cout << *srcs_itr << '\t' << e_itr->first << '\t' << e_itr->second << std::endl;
		}
		if (verbose_output)
		{
			std::cout << "\tTTL = " << meta.get_ttl() << " seconds" << std::endl;
			std::cout << "\tVersion = " << meta.get_version() << std::endl;
		}
	}
	else if (token == "pidmap")
	{
		PID pid;
		std::vector<PID> pids;
		while (is >> pid)
			pids.push_back(pid);

		std::vector<PIDPrefixes> map;
		P4PPortalProtocolMetaInfo meta;
		API_ACTION(api->get_pidmap("DEFAULT", pids.begin(), pids.end(), std::back_inserter(map), &meta))
		{
			fprintf(stderr, "Failed to get PID map: %s\n", API_ACTION_ERROR.c_str());
			return -1;
		}

		for (std::vector<PIDPrefixes>::const_iterator e_itr = map.begin(); e_itr != map.end(); ++e_itr)
		{
			std::cout << e_itr->get_pid();
			for (std::set<p4p::IPPrefix>::const_iterator p_itr = e_itr->get_prefixes().begin(); p_itr != e_itr->get_prefixes().end(); ++p_itr)
				std::cout << '\t' << *p_itr;
			std::cout << std::endl;
		}
		if (verbose_output)
		{
			std::cout << "\tTTL = " << meta.get_ttl() << " seconds" << std::endl;
			std::cout << "\tVersion = " << meta.get_version() << std::endl;
		}
	}
	else if (token == "getpid")
	{
		typedef std::pair<std::string, PID> IPPID;

		std::string ip;
		std::vector<std::string> ips;
		while (is >> ip)
			ips.push_back(ip);

		
		std::vector<IPPID> result;
		API_ACTION(api->get_pids("DEFAULT", ips.begin(), ips.end(), std::back_inserter(result)))
		{
			fprintf(stderr, "Failed to get PIDs: %s\n", API_ACTION_ERROR.c_str());
			return -1;
		}

		for (std::vector<IPPID>::const_iterator itr = result.begin(); itr != result.end(); ++itr)
			std::cout << itr->first << '\t' << itr->second << std::endl;
	}
	else
		goto format_error_dump_all;

	return 0;

format_error_dump_topology:
	fprintf(stderr, "Format: " FORMAT_SHOW_TOPOLOGY "\n");
	return -1;

format_error_dump_pid:
	fprintf(stderr, "Format: " FORMAT_SHOW_PID "\n");
	return -1;

format_error_dump_pdistance:
	fprintf(stderr, "Format: " FORMAT_SHOW_PDISTANCE "\n");
	return -1;

format_error_dump_all:
	fprintf(stderr, "Format: " FORMAT_SHOW_ISP "\n");
	fprintf(stderr, "Format: " FORMAT_SHOW_TOPOLOGY "\n");
	fprintf(stderr, "Format: " FORMAT_SHOW_PID "\n");
	fprintf(stderr, "Format: " FORMAT_SHOW_PDISTANCE "\n");
	fprintf(stderr, "Format: " FORMAT_SHOW_PIDMAP "\n");
	fprintf(stderr, "Format: " FORMAT_SHOW_GETPID "\n");
	return -1;
}

int config_load(std::istream& is)
{
	std::string filename;
	if (!(is >> filename)) 
	{
		fprintf(stderr, "Format: " FORMAT_LOAD "\n");
		return -1;
	}

	if (load_config(filename.c_str()) < 0)
	{
		fprintf(stderr, "Failed to load configuration file\n");
		return -1;
	}

	return 0;
}

/*
 * Parse Command 
 */
int parse(const std::string& command)
{
	std::istringstream line(command);

	// Read command; silently ignore empty commands and comments
	std::string keyword;
	if (!(line >> keyword))			return 0;
	if (keyword[0] == '#')			return 0;

	// Handle all other commands
	if (keyword == "exit")			return CMD_EXIT;
	if (keyword == "server")		return config_server(line);
	if (keyword == "load")			return config_load(line);
	if (keyword == "help")			return help();
	if (keyword == "version")		return config_version();
	if (keyword == "txn")			return config_txn(line);
	if (keyword == "verbose")		return config_verbose(line);
	if (keyword == "isp")			return config_isp(line);
	if (keyword == "pid")			return config_pid(line);
	if (keyword == "dynamic-update-rule")	return config_rule(line);
	if (keyword == "pdistance")		return config_pdistance(line);
	if (keyword == "show")			return config_dump(line);

	return CMD_INVALID;
}


/*
 * Load configuration file
 */
int load_config(const std::string& config_file)
{
	std::string command;
	int lineno = 0, linecnt = 0, linestep = 0;

	std::ifstream fp(config_file.c_str());
	if (!fp)
	{
		fprintf(stderr, "Configuration file not found\n");
		return 0;
	}

	// figure out how many lines there are in the file
	while (std::getline(fp, command))
		linecnt++;
	linestep = linecnt / 25;

	// re-open to go back to the beginning. For some reason,
	// fstream::seekg() isn't working here.
	fp.close();
	fp.open(config_file.c_str());
	if (!fp)
	{
		fprintf(stderr, "Error seeking to beginning of configuration file\n");
		//return -1;
	}

	if (txn_begin() < 0)
		return -1;

	while (std::getline(fp, command))
	{
		lineno++;

		int cmdresult = parse(command);
		if (cmdresult == CMD_EXIT)
			break;

		if (cmdresult < 0)
		{
			fprintf(stderr, "Error in line %d of %s\n", lineno, config_file.c_str());
			txn_rollback();
			return cmdresult;
		}

		if( (linestep > 0) && (lineno % linestep == 0) )
			fprintf(stderr, "Loading.. %3.1lf percent complete\n", ((double)lineno / linecnt) * 100.0);
	}

	if (txn_commit() < 0)
	{
		txn_rollback();
		return -1;
	}

	fprintf(stderr, "  %d lines of configuration loaded\n", linecnt);

	return 0;
}

bool read_command(std::string& out)
{
	std::cout << PROMPT;
	return std::getline(std::cin, out);
}

int main(int argc, char *argv[])
{
	int status = 0;

	if( argc > 1 )
	{
		std::istringstream line(argv[1]);
		config_server(line);
	}
	else
	{
		api = new AdminPortalProtocol(DEFAULT_SERVER, DEFAULT_PORT);
	}

	if( argc > 2 )
	{
		if( load_config(argv[2]) < 0 )
		{
			fprintf(stderr, "Failed to load configuration file\n");
			status = 1;
		}
	}
	else
	{
		printf("\nP4P Portal shell for P4P Portal Service configuration\n");
		printf("Copyright 2008, Laboratory of Networked Systems, Yale University\n");
		printf("  Version 2.1\n");
		/* TODO: Version information */
		/*
		   printf("  Built %s\n", VERINFO_TIMESTAMP);
		   printf("  Code Revision %s\n", VERINFO_ROOT_REV);
		   printf("  Protocol Revision %s\n", VERINFO_WSDL_REV);
		   */
		printf("Usage: %s [<P4P portal server>:<port>] [<configuration file>]\n\n", argv[0]);

		std::string command;
		while (read_command(command))
		{
			// Keep local copy of 'txn_auto' since it can change while
			// processing a command
			int txn_auto_local = txn_auto;
			if (!is_txn_cmd(command) && txn_auto_local && txn_begin() < 0)
				break;

			int result = parse(command);
			if (result == CMD_EXIT)
				break;

			if (result == CMD_INVALID)
				fprintf(stderr, "Command not found\n");

			if(!is_txn_cmd(command) && txn_auto_local && txn_commit() < 0 )
				txn_rollback();
		}
	}

	delete api;

	return status;
}

/* WARNING: 
 *   codes may change due to interface evolution
 */
