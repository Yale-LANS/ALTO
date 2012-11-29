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


#include "p4p/detail/inet_service.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>

namespace p4p {
namespace detail {

const InetService InetService::INVALID("", 0);

InetService::InetService()
	: address_(""),
	  port_(0)
{
}

InetService::InetService(const std::string& service)
{
	/* Find port separator */
	std::string::size_type sep = service.rfind(':');
	if (sep == std::string::npos)
	{
		*this = INVALID;
		return;
	}

	/* Ensure port number exists */
	if (sep == service.size() - 1)
	{
		*this = INVALID;
		return;
	}
	address_ = service.substr(0, sep);

	/* Extract port number and ensure it is valid */
	char* endptr = NULL;
	unsigned long port = strtoul(&service[sep+1], &endptr, 10);
	if ((*endptr != '\0') || (port > USHRT_MAX))
	{
		*this = INVALID;
		return;
	}
	port_ = (unsigned short)port;
}

InetService::InetService(const std::string& address, unsigned short port)
	: address_(address),
	  port_(port)
{
}

int InetService::compare_dst(const InetService& rhs) const
{
	int rc_addr = compare(address_, rhs.address_);
	if (rc_addr != 0)
		return rc_addr;

	return compare(port_, rhs.port_);
}

std::ostream& operator<<(std::ostream& os, const InetService& rhs)
{
	os << rhs.get_address() << ':' << rhs.get_port();
	return os;
}

std::istream& operator>>(std::istream& is, InetService& rhs)
{
	/* Get the token */
	std::string s;
	if (!(is >> s))
		return is;

	/* Parse the service */
	rhs = InetService(s);
	if (rhs == InetService::INVALID)
		is.clear(std::ios::failbit);

	return is;
}

};
};
