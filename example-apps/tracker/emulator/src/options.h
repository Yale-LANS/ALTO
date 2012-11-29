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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>
#include <p4p/app/p4papp.h>

using namespace p4p::app;

/* Container for options for a single ISP */
struct ISPOptions
{
	std::string name;	/* Also used for local PIDMap file name */
	std::string host;
	unsigned short port;
};

/* Storage for command-line options */
extern std::vector<ISPOptions>			OPTION_ISPS;				/* Configured ISPs */
extern std::string				OPTION_OPTENGINE_ADDR;			/* Optimization Engine server address and port */
extern unsigned short				OPTION_OPTENGINE_PORT;

extern bool					OPTION_VERBOSE;				/* True if detailed messages should be printed */

extern double					OPTION_INTRA_PID_PCT;
extern double					OPTION_INTRA_ISP_PCT;

/* Function for parsing command-line options to fill in
 * global variables listed above. */
void parse_command_line(int argc, const char** argv);

#endif
