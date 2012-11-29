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

#include <iostream>

std::vector<ISPOptions>			OPTION_ISPS;
std::string				OPTION_OPTENGINE_ADDR;
unsigned short				OPTION_OPTENGINE_PORT = 0;
bool					OPTION_VERBOSE = false;
double					OPTION_INTRA_PID_PCT = 0.7;
double					OPTION_INTRA_ISP_PCT = 0.9;

/**
 * Prototypes for argument-parsing helpers
 */
std::string	next_arg_string(int& argc, const char**& argv);
unsigned short	next_arg_ushort(int& argc, const char**& argv);
double		next_arg_double(int& argc, const char**& argv);
void		print_usage();

/**
 * Main argument-parsing routine
 */
void parse_command_line(int argc, const char** argv)
{
	/* Skip 0-th argument (it is the program name) */
	--argc;
	++argv;

	/* Process arguments one-by-one */
	while (argc > 0)
	{
		std::string arg = next_arg_string(argc, argv);
		if (arg == "-i")
		{
			ISPOptions isp_opts;
			isp_opts.name = next_arg_string(argc, argv);
			isp_opts.host = next_arg_string(argc, argv);
			isp_opts.port = next_arg_ushort(argc, argv);
			OPTION_ISPS.push_back(isp_opts);
		}
		else if (arg == "-o")
		{
			OPTION_OPTENGINE_ADDR = next_arg_string(argc, argv);
			OPTION_OPTENGINE_PORT = next_arg_ushort(argc, argv);
		}
		else if (arg == "-l")
		{
			OPTION_INTRA_PID_PCT = next_arg_double(argc, argv);
			OPTION_INTRA_ISP_PCT = next_arg_double(argc, argv);
		}
		else if (arg == "-v")
		{
			OPTION_VERBOSE = true;
		}
		else
			print_usage();
	}
}

/**
 * Helper function for reading next argument as a string
 */
std::string next_arg_string(int& argc, const char**& argv)
{
	/* Print usage if next argument was not available */
	if (argc == 0)
		print_usage();

	std::string result = argv[0];
	++argv;
	--argc;
	return result;
}

unsigned short next_arg_ushort(int& argc, const char**& argv)
{
	std::string arg = next_arg_string(argc, argv);
	try
	{
		return p4p::detail::p4p_token_cast<unsigned short>(arg);
	}
	catch (std::exception& e)
	{
		print_usage();
		return 0;	/* Avoid compiler warnings */
	}
}

double next_arg_double(int& argc, const char**& argv)
{
	std::string arg = next_arg_string(argc, argv);
	try
	{
		return p4p::detail::p4p_token_cast<double>(arg);
	}
	catch (std::exception& e)
	{
		print_usage();
		return 0;	/* Avoid compiler warnings */
	}
}

/* Usage string is defined in main.cpp  */
extern const char* USAGE;
void print_usage()
{
	std::cout << USAGE << std::endl;
	exit(1);
}

