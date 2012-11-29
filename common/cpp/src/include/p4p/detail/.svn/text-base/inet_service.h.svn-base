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


#ifndef INET_SERVICE_H
#define INET_SERVICE_H

#ifdef P4P_CLUSTER
	#include <boost/serialization/access.hpp>
#endif

#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <p4p/detail/util.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace detail {

class p4p_common_cpp_EXPORT InetService
{
public:

	static const InetService INVALID;

	/**
	 * Default Constructor: construct instance equivalent to INVALID
	 */
	InetService();

	/**
	 * Constructor: construct from string using 'host:port' representation.
	 * Resulting instance is equivalent to INVALID if string is not of
	 * correct format.
	 */
	InetService(const std::string& service);

	/*
	 * Constructor: construct using explict address and port number
	 */
	InetService(const std::string& address, unsigned short port);

	const std::string& get_address() const		{ return address_; }
	unsigned short get_port() const			{ return port_; }

	bool operator<(const InetService& rhs) const	{ return compare_dst(rhs) < 0; }
	bool operator>(const InetService& rhs) const	{ return compare_dst(rhs) > 0; }
	bool operator==(const InetService& rhs) const	{ return compare_dst(rhs) == 0; }
	bool operator!=(const InetService& rhs) const	{ return compare_dst(rhs) != 0; }

private:
#ifdef P4P_CLUSTER
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & address_;
		ar & port_;
	}
#endif

	int compare_dst(const InetService& rhs) const;

	std::string address_;
	unsigned short port_;
};

p4p_common_cpp_EXPORT std::ostream& operator<<(std::ostream& os, const InetService& rhs);
p4p_common_cpp_EXPORT std::istream& operator>>(std::istream& is, InetService& rhs);

}; // namespace detail
}; // namespace p4p

#endif
