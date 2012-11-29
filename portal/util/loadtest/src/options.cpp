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


#include "options.h"

#include <boost/program_options.hpp>
#include <iostream>

namespace bpo = boost::program_options;

bpo::variables_map OPTIONS;

const bpo::options_description AVAILABLE_OPTIONS = bpo::options_description("Options");

void handle_options(int argc, char** argv)
{
	const_cast<bpo::options_description*>(&AVAILABLE_OPTIONS)->add_options()
	("help",		"print this help message")
	("server",		bpo::value<std::string>()->default_value("localhost"),
				"server address")
	("port",		bpo::value<unsigned short>()->default_value(6671),
				"server port")
	("persistent",		"use persistent connections")
	("clients",		bpo::value<unsigned int>()->default_value(1),
				"number of clients to simulate (one thread per client)")
	("getpids-rate",	bpo::value<double>()->default_value(5.0),
				"requests per second for GetPIDs (per client)")
	("getpids-prefix",	bpo::value<std::string>()->default_value("127.0.0.0/24"),
				"prefix for which requests will be generated")
	("getpids-batch",	bpo::value<unsigned int>()->default_value(5),
				"number of addresses to send per query")
	("getpdistance-pids",	bpo::value<double>()->default_value(1.0),
				"percentage of PIDs to include in GetPDistance request")
	("getpdistance-rate",	bpo::value<double>()->default_value(0.1),
				"requests per second for GetPDistance (per client)")
	;


	try
	{
		bpo::store(bpo::parse_command_line(argc, argv, AVAILABLE_OPTIONS), OPTIONS);
	}
	catch (bpo::error& e)
	{
		std::cerr << "Error: " << e.what() << std::endl
			  << AVAILABLE_OPTIONS << std::endl;
		exit(1);
	}

	bpo::notify(OPTIONS);

	/*
	 *	Handle 'help' option
	 */
	if (OPTIONS.count("help") > 0)
	{
		std::cerr << AVAILABLE_OPTIONS << std::endl;
		exit(0);
	}
}
