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


#include "p4p/ip_addr.h"

#include "p4p/detail/util.h"

#ifdef WIN32
#else
	#include <netdb.h>
#endif
#include <limits.h>
#include <algorithm>

namespace p4p {

const IPPrefix IPPrefix::INVALID = IPPrefix();

static void mask_bits(unsigned char* buf, unsigned int len, unsigned int bits)
{
	unsigned int i = bits / 8;

	/* Skip if there is nothing to mask out */
	if (i >= len)
		return;

	/* Handle the partial byte */
	buf[i] &= (0xFF << (8 - (bits % 8)));

	/* Everything afterwards just gets set to 0 */
	++i;
	memset(buf + i, 0, len - i);
}

static unsigned short get_addr_size(int family)
{
	switch (family)
	{
	case AF_UNSPEC:	return 0;
	case AF_INET:	return sizeof(in_addr);
	case AF_INET6:	return sizeof(in6_addr);
	default:	return USHRT_MAX;
	}
}

IPPrefix::IPPrefix()
	: family_(AF_UNSPEC),
	  length_(0)
{
	memset(&address_, 0, sizeof(address_));
}

IPPrefix::IPPrefix(int family, const void* address, unsigned short length) throw (std::runtime_error)
	: family_(family)
{
	/* Also validates family argument */
	unsigned short addr_size = get_addr_size(family_);
	if (addr_size == USHRT_MAX)
		throw std::runtime_error("Invalid address family");

	/* Set appropriate prefix length */
	length_ = std::min(length, (unsigned short)(addr_size * 8));

	/* Initialize full address to zero-address */
	memset(&address_, 0, sizeof(address_));

	/* Default address is all zero's */
	if (!address)
		return;

	memcpy(&address_, address, addr_size);

	normalize();
}

IPPrefix::IPPrefix(const std::string& address, unsigned short length)
{
	/* If string has a prefix length, extract it */
	std::string::size_type slash = address.rfind('/');
	if (slash != std::string::npos)
	{
		if (slash < address.size() - 1)
		{
			/* Extract the prefix length */
			char* errptr;
			length = strtoul(&address[slash+1], &errptr, 10);
			if (*errptr != '\0')
			{
				*this = INVALID;
				return;
			}
		}
		else
		{
			/* If slash is the last character, treat as if
			 * no prefix was specified. */
			length = USHRT_MAX;
		}
	}
	else
	{
		/* No slash - address is the entire string */
		slash = address.size();
	}
	std::string addrpart = address.substr(0, slash);

	struct addrinfo HINTS = {
		AI_NUMERICHOST,
		AF_UNSPEC,
		0,
		0
	};
	int rc;

	/* Convert address from text */
	struct addrinfo* ai = NULL;
	if ((rc = getaddrinfo(addrpart.c_str(), NULL, &HINTS, &ai)) != 0)
	{
		*this = INVALID;
		return;
	}

	/* Extract address family */
	family_ = ai->ai_family;

	/* Extract address bytes */
	if (family_ == AF_INET)
		memcpy(&address_, &((struct sockaddr_in*)ai->ai_addr)->sin_addr, sizeof(address_.addr4));
	else if (family_ == AF_INET6)
		memcpy(&address_, &((struct sockaddr_in6*)ai->ai_addr)->sin6_addr, sizeof(address_.addr6));
	else
	{
		freeaddrinfo(ai);
		*this = INVALID;
		return;
	}
	freeaddrinfo(ai);

	/* Normalize the length */
	length_ = std::min(length, (unsigned short)(get_addr_size(family_) * 8));

	normalize();
}

IPPrefix::IPPrefix(const char* address, unsigned short length)
{
	*this = IPPrefix(std::string(address), length);
}

void IPPrefix::normalize()
{
	/*
	 * Zero-out the bits that were outside the length so we can store
	 * in a 'normalized' way, which makes comparisons easier.
	 */
	switch (family_)
	{
	case AF_INET:
		mask_bits((unsigned char*)&address_, sizeof(address_.addr4), length_);
		break;
	case AF_INET6:
		mask_bits((unsigned char*)&address_, sizeof(address_.addr6), length_);
		break;
	}
}

std::string IPPrefix::get_address_text() const throw (std::invalid_argument)
{
	char buf[MAX_STR_LEN] = { 0 };
	int rc = 0;

	switch (family_)
	{
	case AF_INET:
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &address_.addr4, sizeof(address_.addr4));
		rc = getnameinfo((struct sockaddr*)&addr, sizeof(addr),
			buf, MAX_STR_LEN,
			NULL, 0,
			NI_NUMERICHOST);
		break;
	}
	case AF_INET6:
	{
		struct sockaddr_in6 addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin6_family = AF_INET6;
		memcpy(&addr.sin6_addr, &address_.addr6, sizeof(address_.addr6));
		rc = getnameinfo((struct sockaddr*)&addr, sizeof(addr),
			buf, MAX_STR_LEN,
			NULL, 0,
			NI_NUMERICHOST);
		break;
	}
	default:
		throw std::invalid_argument("attempted to convert invalid address to string");
	}

	if (rc != 0)
		throw std::runtime_error("illegal state: failed to convert address to text: " + std::string(gai_strerror(rc)));

	return buf;
}

int IPPrefix::compare_dst(const IPPrefix& rhs) const
{
	/* Handle items of different address types */
	if (family_ != rhs.family_)
		return family_ - rhs.family_;

	/* Handle prefixes of different lengths */
	if (length_ != rhs.length_)
		return (int)length_ - (int)rhs.length_;

	/* Finally compare the raw addresses. Note that it
	 * is assumed here that addresses are stored 'normalized'
	 * such that all masked-out bits are stored as 0. */
	return memcmp(&address_, &rhs.address_, sizeof(address_));
}

std::ostream& operator<<(std::ostream& os, const IPPrefix& rhs)
{
	os << rhs.get_address_text();
	if (!os)
		return os;

	if (rhs.get_length() < get_addr_size(rhs.get_family()) * 8)
		os << '/' << rhs.get_length();

	return os;
}

std::istream& operator>>(std::istream& is, IPPrefix& rhs)
{
	/* Get the token */
	std::string s;
	if (!(is >> s))
		return is;

	/* Parse the address */
	rhs = IPPrefix(s);
	if (rhs == IPPrefix::INVALID)
		is.clear(std::ios::failbit);

	return is;
}

};
