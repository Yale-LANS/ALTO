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


#include "p4p/detail/util.h"

#include <curl/curl.h>
#ifdef WIN32
	#include <winsock2.h>
#endif

namespace p4p {

static void p4p_init()
{
#ifdef WIN32
	WSADATA wsdata;
	int err = WSAStartup(MAKEWORD(2,2), &wsdata);
	if (err != 0)
		throw std::runtime_error("Failed to initialize Winsock: error code = " + detail::p4p_token_cast<std::string>(err));


	if (LOBYTE(wsdata.wVersion) != 2 || HIBYTE(wsdata.wVersion) != 2)
	{
		WSACleanup();
		throw std::runtime_error("Failed to locate Winsock 2.2");
	}
#endif

	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
		throw std::runtime_error("Failed to initialize libcurl");
}


static void p4p_cleanup()
{
	curl_global_cleanup();

#ifdef WIN32
	WSACleanup();
#endif
}

/**
 * When constructed, this class calls p4p_init().  When destructed,
 * this class calls p4p_cleanup().
 */
class P4PInit
{
public:
	/**
	 * Constructor; calls p4p_init()
	 */
	P4PInit() { p4p_init(); }

	/**
	 * Destructor; calls p4p_cleanup()
	 */
	~P4PInit() { p4p_cleanup(); }
};

/**
 * Create single static instance of P4PInit. This initializes
 * the P4P library when loaded, and cleans up when unloaded.
 */
static P4PInit P4P_INIT;

};

