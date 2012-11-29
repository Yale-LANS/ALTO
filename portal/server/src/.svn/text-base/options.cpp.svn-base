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

#include <errno.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include "constants.h"

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

bpo::variables_map OPTIONS;
std::set<std::string> INTERFACE_LIST;
bpo::variables_map INTERFACE_OPTIONS;

const bpo::options_description AVAILABLE_OPTIONS_GENERIC = bpo::options_description("Generic Options");
const bpo::options_description AVAILABLE_OPTIONS_CONFIG = bpo::options_description("Configuration");
const bpo::options_description AVAILABLE_OPTIONS_INTERFACE = bpo::options_description("Interface Configuration");

void options_init()
{
	const_cast<bpo::options_description*>(&AVAILABLE_OPTIONS_GENERIC)->add_options()
	("help",		"print this help message")
	("interface-help",	"print help about available interface options")
	("version",		"print version information and exit")
	("config-file",		bpo::value<std::string>(),
				"extra file for specifying options")
	("interfaces-file",	bpo::value<std::string>(),
				"configuration file defining enabled interfaces")
	;
	const_cast<bpo::options_description*>(&AVAILABLE_OPTIONS_CONFIG)->add_options()
	("log-level",		bpo::value<unsigned int>()->default_value(2),
				"log level: 0=critical, 1=errors, 2=warnings, 3=info, 4=debug")
	("log-ident",		bpo::value<std::string>()->default_value("p4p-portal"),
				"identity for syslog messages")
	("cluster",		"run in clustered mode")
	("daemon",		"run in background (daemon mode)")
	("state-dir",		bpo::value<std::string>()->default_value(DEFAULT_STATE_DIR.string()),
				"directory used to store runtime and persistent state (used when clustering enabled)")
	("job-threads",		bpo::value<unsigned int>()->default_value(4),
				"size of thread pool for handling background jobs")
	("admin-txn-timeout",	bpo::value<unsigned int>()->default_value(3),
				"number of seconds before cancelling an idle admin transaction")
	;

	const_cast<bpo::options_description*>(&AVAILABLE_OPTIONS_INTERFACE)->add_options()
	("type",		bpo::value<std::string>()->default_value("REST"),
				"type of interface: only 'REST' is currently supported")
	("address",		bpo::value<std::string>()->default_value(""),
				"binding address (leave blank to bind to all) [NOTE: CURRENTLY UNSUPPORTED]")
	("port",		bpo::value<unsigned short>()->default_value(6671),
				"listening port")
	("threads",		bpo::value<unsigned int>()->default_value(4),
				"maximum number of threads in pool")
	("ssl-cert-file",	bpo::value<std::string>()->default_value(""),
				"path to SSL certificate (leave blank to disable SSL) [NOTE: CURRENTLY UNSUPPORTED]")
	("ssl-key-file",	bpo::value<std::string>()->default_value(""),
				"path to SSL key (required if ssl-cert-file specified) [NOTE: CURRENTLY UNSUPPORTED]")
	("connection-timeout",	bpo::value<unsigned int>()->default_value(0),
				"seconds of inactivity before a connection will be timed out (default is no timeout)")
	("connection-limit",	bpo::value<unsigned int>()->default_value(100),
				"maximum number of connections (default is 100)")
	("ip-connection-limit",	bpo::value<unsigned int>()->default_value(5),
				"maximum number of connections per IP (default is 5)")
	("allow-admin",		bpo::value<bool>()->default_value(false),
				"allow administration")
	;
}

bfs::path find_config_file(int argc, char** argv, const bfs::path& name)
{
	bfs::path result;

	/*
	 * Look in same directory as executable
	 */
	result = bfs::path(argv[0]).branch_path();
	result /= name;
	if (bfs::exists(result) && bfs::is_regular(result))
		return result;

	/*
	 * Look in the sibling configuration directory (relative to the location as the executable)
	 */
	result = bfs::path(argv[0]).branch_path();
	result /= DEFAULT_SIBLING_CONFIG_DIR;
	result /= name;
	if (bfs::exists(result) && bfs::is_regular(result))
		return result;

	/*
	 * Look in the system's configuration directory
	 */
	result = DEFAULT_SYSTEM_CONFIG_DIR;
	result /= name;
	if (bfs::exists(result) && bfs::is_regular(result))
		return result;

	return bfs::path();
}

void read_interfaces_file(const bfs::path& path, std::set<std::string>& interfaces, boost::program_options::variables_map& options)
{
	std::ifstream f(path.string().c_str());
	if (!f)
	{
		std::cerr << "Failed to open interfaces file " << path << std::endl;
		exit(1);
	}

	/*
	 * First step: figure out which option groups are available, and for each
	 * one add the allowed options.
	 */
	bpo::options_description available_options = bpo::options_description("Interface Options");
	try
	{
		bpo::basic_parsed_options<char> unparsed_opts = bpo::parse_config_file(f, bpo::options_description(), true);

		for (unsigned int i = 0; i < unparsed_opts.options.size(); ++i)
		{
			/* Parse out name of the group */
			std::string group = unparsed_opts.options[i].string_key;
			std::string::size_type dot_pos = group.rfind('.');
			if (dot_pos == std::string::npos)
				continue;

			group = group.substr(0, dot_pos);

			/* Skip if we've already found the group */
			if (interfaces.find(group) != interfaces.end())
				continue;
			interfaces.insert(group);

			/* Add the options
			 * NOTE: When updating these, you must also update the
			 *       corresponding options above! */
			available_options.add_options()
				((group + ".type").c_str(),
				bpo::value<std::string>()->default_value(""));
			available_options.add_options()
				((group + ".address").c_str(),
				bpo::value<std::string>()->default_value(""));
			available_options.add_options()
				((group + ".port").c_str(),
				bpo::value<unsigned short>()->default_value(6671));
			available_options.add_options()
				((group + ".threads").c_str(),
				bpo::value<unsigned int>()->default_value(4));
			available_options.add_options()
				((group + ".ssl-cert-file").c_str(),
				bpo::value<std::string>()->default_value(""));
			available_options.add_options()
				((group + ".ssl-key-file").c_str(),
				bpo::value<std::string>()->default_value(""));
			available_options.add_options()
				((group + ".connection-timeout").c_str(),
				bpo::value<unsigned int>()->default_value(0));
			available_options.add_options()
				((group + ".connection-limit").c_str(),
				bpo::value<unsigned int>()->default_value(100));
			available_options.add_options()
				((group + ".ip-connection-limit").c_str(),
				bpo::value<unsigned int>()->default_value(5));
			available_options.add_options()
				((group + ".allow-admin").c_str(),
				bpo::value<bool>()->default_value(false));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Failed to parse interfaces file " << path << ": " << e.what() << std::endl;
		exit(1);
	}

	f.close();
	f.open(path.string().c_str());
	if (!f)
	{
		std::cerr << "Failed to reopen interfaces file " << path << std::endl;
		exit(1);
	}

	/*
	 * Re-parse config file with our configured options
	 */
	try
	{
		bpo::store(bpo::parse_config_file(f, available_options), options);
		bpo::notify(options);
	}
	catch (std::exception& e)
	{
		std::cerr << "Error in interfaces file " << path << ": " << e.what() << std::endl;
		exit(1);
	}

	f.close();
}

void handle_options(int argc, char** argv)
{
	options_init();

	bpo::options_description cmdline_options;
	cmdline_options.add(AVAILABLE_OPTIONS_GENERIC);
	cmdline_options.add(AVAILABLE_OPTIONS_CONFIG);

	bpo::options_description cfgfile_options;
	cfgfile_options.add(AVAILABLE_OPTIONS_CONFIG);

	/*
	 * Load options from command-line
	 */
	try
	{
		bpo::store(bpo::parse_command_line(argc, argv, cmdline_options), OPTIONS);
	}
	catch (bpo::error& e)
	{
		std::cerr << "Error: " << e.what() << std::endl
			  << cmdline_options << std::endl;
		exit(1);
	}

	/*
	 * Open the file and load options (don't overwrite options specified on command-line)
	 */
	bfs::path config_file = OPTIONS.count("config-file")
			      ? bfs::path(OPTIONS["config-file"].as<std::string>())
			      : find_config_file(argc, argv, CONFIG_FILENAME);
	if (!config_file.empty())
	{
		std::ifstream config_stream(config_file.string().c_str());
		if (!config_stream)
		{
			std::cerr << "Failed to open configuration file " << config_file << ": " << strerror(errno) << std::endl;
			exit(1);
		}

		try
		{
			bpo::store(bpo::parse_config_file(config_stream, cfgfile_options), OPTIONS);
			config_stream.close();
		}
		catch (bpo::error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl
				  << cfgfile_options << std::endl;
			exit(1);
		}
		config_stream.close();
	}
	bpo::notify(OPTIONS);

	/*
	 * Handle 'help' option
	 */
	if (OPTIONS.count("help") > 0)
	{
		std::cerr << cmdline_options << std::endl;
		exit(0);
	}
	if (OPTIONS.count("interface-help") > 0)
	{
		std::cerr << AVAILABLE_OPTIONS_INTERFACE << std::endl;
		exit(0);
	}

	/*
	 * Handle the file that defines the interfaces we expose
	 */
	bfs::path interfaces_file = OPTIONS.count("interfaces-file")
				  ? bfs::path(OPTIONS["interfaces-file"].as<std::string>())
				  : find_config_file(argc, argv, INTERFACES_FILENAME);
	if (!interfaces_file.empty())
		read_interfaces_file(interfaces_file, INTERFACE_LIST, INTERFACE_OPTIONS);

}
