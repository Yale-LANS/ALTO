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


#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <queue>
#include <signal.h>
#include <p4pserver/logging.h>
#include "prog_info.h"
#include "options.h"
#include "constants.h"
#include "view_registry.h"
#include "state.h"
#include "rest_request_handlers.h"
#include "build_info.h"

/*
 * Signal handlers
 */
sig_atomic_t Terminated;
void term_handler(int sig, siginfo_t* info, void* arg);
void pipe_handler(int sig, siginfo_t* info, void* arg);

/*
 * Version information
 */
void print_version();

std::string ALTO_Server;

int main(int argc, char** argv)
{
	/*
	 * Handle command-line args
	 */
	handle_options(argc, argv);

	if (OPTIONS.count("version"))
	{
		print_version();
		return 0;
	}


	if (OPTIONS.count("cluster"))
	{
		std::cerr << "Clustering is currently not available" << std::endl;
		return 1;
	}

	init_logger(OPTIONS["log-level"].as<unsigned int>(), OPTIONS["log-ident"].as<std::string>());

	/*
	 * Logger used for the main loop and request handling
	 */
	log4cpp::Category& logger = log4cpp::Category::getInstance("Main");

	/*
	 * Install signal handlers
	 */
	logger.info("installing signal handlers");
	struct sigaction sig_actn;
	sig_actn.sa_sigaction = term_handler;
	sigemptyset(&sig_actn.sa_mask);
	sig_actn.sa_flags = 0;
	sigaction(SIGINT, &sig_actn, NULL);
	sigaction(SIGTERM, &sig_actn, NULL);

	sig_actn.sa_sigaction = pipe_handler;
	sigemptyset(&sig_actn.sa_mask);
	sig_actn.sa_flags = 0;
	sigaction(SIGPIPE, &sig_actn, NULL);

	/* Initialize global state */
	logger.info("initializing global state");
	init_state();

	/* Move to the background if configured to do so */
	if (OPTIONS.count("daemon"))
	{
		logger.info("forking to background");
		daemon(0, 0);
	}

	{
		JOB_QUEUE->start();

		std::vector<ProtocolServerBaseAbstract*> servers;
		BOOST_FOREACH(const std::string& intf, INTERFACE_LIST)
		{
			std::string type = INTERFACE_OPTIONS[intf + ".type"].as<std::string>();
			bool allow_admin = INTERFACE_OPTIONS[intf + ".allow-admin"].as<bool>();
			if (type == "REST")
			{
				RESTHandler::PortalRESTServer* server = new RESTHandler::PortalRESTServer(
						INTERFACE_OPTIONS[intf + ".address"].as<std::string>(),
						INTERFACE_OPTIONS[intf + ".port"].as<unsigned short>(),
						INTERFACE_OPTIONS[intf + ".threads"].as<unsigned int>(),
						INTERFACE_OPTIONS[intf + ".ssl-cert-file"].as<std::string>(),
						INTERFACE_OPTIONS[intf + ".ssl-key-file"].as<std::string>(),
						INTERFACE_OPTIONS[intf + ".connection-timeout"].as<unsigned int>(),
						INTERFACE_OPTIONS[intf + ".connection-limit"].as<unsigned int>(),
						INTERFACE_OPTIONS[intf + ".ip-connection-limit"].as<unsigned int>()
						);
				server->enable_all();
				if (allow_admin)
					server->enable(RESTHandler::PortalRESTServer::MessageTypeSet().set(PORTAL_MSG_ADMIN_REQ));
				else {
					server->disable(RESTHandler::PortalRESTServer::MessageTypeSet().set(PORTAL_MSG_ADMIN_REQ));

					ALTO_Server += "http://";
					ALTO_Server += boost::lexical_cast<std::string>(INTERFACE_OPTIONS[intf + ".address"].as<std::string>());
					ALTO_Server += ":";
					ALTO_Server += boost::lexical_cast<std::string>(INTERFACE_OPTIONS[intf + ".port"].as<unsigned short>());
				}

				servers.push_back(server);
			}
			else
				throw std::runtime_error("Invalid type '" + type + "' for interface '" + intf + "'");
		}

		/* Move from just behind init_state() to use information from configuration file */
		/* Initilize InfoRes */
		RESTHandler::InitInfoRes();

		BOOST_FOREACH(ProtocolServerBaseAbstract* s, servers)
		{
			s->start();
		}

		logger.info("handling requests");
		while (!Terminated)
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	
		logger.info("signalling handlers to terminate");
		BOOST_FOREACH(ProtocolServerBaseAbstract* h, servers)
		{
			h->stop();
		}
	
		logger.info("waiting for handlers to terminate");
		BOOST_FOREACH(ProtocolServerBaseAbstract* h, servers)
		{
			h->join();
			delete h;
		}
	
		/* Cleanup: notify each thread to stop */
		logger.info("signalling background threads to terminate");
		JOB_QUEUE->stop();

		/* Cleanup: join threads and cleanup soap environments */
		logger.info("waiting for background threads to terminate");
		JOB_QUEUE->join();
	}

	return 0;
}

void term_handler(int sig, siginfo_t* info, void* arg)
{
	Terminated = 1;
}

void pipe_handler(int sig, siginfo_t* info, void* arg) {}

void print_version()
{
	std::cout << PROGINFO_NAME << std::endl;
	std::cout << "Built " << BUILD_TIMESTAMP << std::endl;
	std::cout << "Version " << BUILD_VERSION_MAJOR << '.'
				<< BUILD_VERSION_MINOR << '.'
				<< BUILD_VERSION_RELEASE << std::endl;
	std::cout << PROGINFO_COPYRIGHT << std::endl;
}
