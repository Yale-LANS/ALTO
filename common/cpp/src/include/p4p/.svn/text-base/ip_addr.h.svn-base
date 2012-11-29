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


#ifndef IP_ADDR_H
#define IP_ADDR_H

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <netinet/ip.h>
#endif

#ifdef P4P_CLUSTER
	#include <boost/serialization/access.hpp>
#endif

#include <string.h>
#include <limits.h>
#include <set>
#include <vector>
#include <stdexcept>
#include <string>
#include <stdexcept>
#include <p4p/detail/util.h>
#include <p4p/detail/compiler.h>

namespace p4p {

//! Class encapsulating a single IPv4 or IPv6 prefix
/**
 * General class for IP address prefixes. See IPAddress for a specialization
 * that handles individual IP addresses.
 */
class p4p_common_cpp_EXPORT IPPrefix
{
public:
	/* Accommodates both IPv4 and IPv6 addresses */
	static const unsigned int MAX_STR_LEN = 80;

	/* Return value when asked to convert an invalid textual
	 * representation to an IPPrefix object */
	static const IPPrefix INVALID;

	/* Default constructor. Default constructed address is equal to INVALID. */
	IPPrefix();

	/* Construct using raw address */
	IPPrefix(int family, const void* address, unsigned short length = USHRT_MAX) throw (std::runtime_error);

	/* Construct from textual representation. If textual representation is invalid, then
	 * the constructed address is equal to INVALID. */
	IPPrefix(const std::string& address, unsigned short length = USHRT_MAX);

	/* Construct from textual representation (C-style string).  If textual representation
	 * is invalid, the constructed address is equal to INVALID. */
	IPPrefix(const char* address, unsigned short length = USHRT_MAX);

	/* Get address family */
	int get_family() const { return family_; }

	/* Get IP address (in network byte order) */
	const void* get_address() const { return &address_; }

	/* Get IP address part as text (no prefix included)
	 * Throws exception if this address is INVALID. */
	std::string get_address_text() const throw (std::invalid_argument);

	/* Get prefix length */
	unsigned short get_length() const { return length_; }

	/* Less-than operator */
	bool operator<(const IPPrefix& rhs) const { return compare_dst(rhs) < 0; }

	/* Greater-than operator */
	bool operator>(const IPPrefix& rhs) const { return compare_dst(rhs) > 0; }
	
	/* Equality operator */
	bool operator==(const IPPrefix& rhs) const { return compare_dst(rhs) == 0; }

	/* Inequality operator */
	bool operator!=(const IPPrefix& rhs) const { return compare_dst(rhs) != 0; }

private:
	union Address
	{
		in_addr addr4;
		in6_addr addr6;
	};

#ifdef P4P_CLUSTER
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & family_;
		ar & *((uint32_t*)(&address_) + 0); /* Serialize 4 bytes at a time */
		ar & *((uint32_t*)(&address_) + 1);
		ar & *((uint32_t*)(&address_) + 2);
		ar & *((uint32_t*)(&address_) + 3);
		ar & length_;
	}
#endif

	int compare_dst(const IPPrefix& rhs) const;

	void normalize();

	int family_;
	Address address_;
	unsigned short length_;
};

//! Class encapsulating a single IPv4 or IPv6 address
/**
 *  Specialized subclass for IP addresses.  An IP address is a special case
 *  of an IPPrefix.
 */
class p4p_common_cpp_EXPORT IPAddress : public IPPrefix
{
public:
	/* Default constructor. Constructed address is equal to INVALID. */
	IPAddress() {}

	/* Construct using raw address */
	IPAddress(int family, const void* address)
		: IPPrefix(family, address)
	{}

	/* Construct from textual representation. If textual representation is invalid, then
	 * the constructed address is equal to INVALID. */
	IPAddress(const std::string& address)
		: IPPrefix(address)
	{}

	/* Construct from textual representation (C-style string).  If textual representation
	 * is invalid, the constructed address is equal to INVALID. */
	IPAddress(const char* address)
		: IPPrefix(address)
	{}
};

p4p_common_cpp_EXPORT std::ostream& operator<<(std::ostream& os, const IPPrefix& rhs);
p4p_common_cpp_EXPORT std::istream& operator>>(std::istream& is, IPPrefix& rhs);

typedef std::set<IPPrefix> IPPrefixSet;
typedef std::vector<IPPrefix> IPPrefixVector;

}; // namespace p4p

#endif
