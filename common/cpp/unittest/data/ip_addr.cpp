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


#include <boost/test/unit_test.hpp>

#include "p4p/ip_addr.h"

using namespace p4p;

BOOST_AUTO_TEST_CASE ( ip_prefix_invalid_address )
{
	BOOST_CHECK_THROW(IPPrefix(-1, NULL), std::exception);
}

BOOST_AUTO_TEST_CASE ( ip_prefix_zero_address )
{
	unsigned char zero_addr[4];
	memset(&zero_addr, 0, 4);

	IPPrefix p;
	BOOST_CHECK_EQUAL(memcmp(p.get_address(), &zero_addr, 4), 0);
}

BOOST_AUTO_TEST_CASE ( ip_prefix_compare )
{
	IPPrefix ip1("127.0.0.1", 8);
	IPPrefix ip2("128.0.0.1", 8);

	BOOST_CHECK(ip1 != IPPrefix::INVALID);
	BOOST_CHECK(ip2 != IPPrefix::INVALID);

	BOOST_CHECK(ip1 < ip2);
	BOOST_CHECK(ip2 > ip1);
	BOOST_CHECK(ip1 != ip2);
	BOOST_CHECK(ip2 != ip1);
	BOOST_CHECK(!(ip1 == ip2));
	BOOST_CHECK(!(ip2 == ip1));
}

BOOST_AUTO_TEST_CASE ( ip_prefix_get_length_default_v4 )
{
	IPPrefix ip("127.0.0.1", USHRT_MAX);

	BOOST_CHECK_EQUAL(ip.get_address_text(), "127.0.0.1");
	BOOST_CHECK_EQUAL(ip.get_length(), 32);
}

BOOST_AUTO_TEST_CASE ( ip_prefix_get_length_default_v6 )
{
	IPPrefix ip("2001::", USHRT_MAX);

	BOOST_CHECK_EQUAL(ip.get_address_text(), "2001::");
	BOOST_CHECK_EQUAL(ip.get_length(), 128);
}

BOOST_AUTO_TEST_CASE ( ip_prefix_zero_bits_v4 )
{
	IPPrefix ip1("127.0.0.1", 8);
	IPPrefix ip2("192.16.104.0", 23);
	
	BOOST_CHECK_EQUAL(ip1.get_address_text(), "127.0.0.0");
	BOOST_CHECK_EQUAL(ip2.get_address_text(), "192.16.104.0");
}

BOOST_AUTO_TEST_CASE ( ip_prefix_zero_bits_v6 )
{
	IPPrefix ip("2001:0db8:0000:0000:0000:0000:1428:57ab", 16);

	BOOST_CHECK_EQUAL(ip.get_address_text(), "2001::");
}
