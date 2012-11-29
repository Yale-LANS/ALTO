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
#include <boost/test/floating_point_comparison.hpp>

#include <stdexcept>
#include "interdomain_pair.h"

BOOST_AUTO_TEST_CASE ( interdomain_dest_create_empty )
{
	InterdomainDest d;
	BOOST_CHECK_THROW(d.get_asn(), std::exception);
	BOOST_CHECK_THROW(d.get_ip(), std::exception);
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_create_asn )
{
	InterdomainDest d(100);
	BOOST_CHECK_THROW(d.get_ip(), std::exception);
	BOOST_CHECK_EQUAL(d.get_asn(), 100);
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_create_ip )
{
	p4p::IPPrefix* ip = p4p::IPPrefix::from_text("127.0.0.1", 8);

	InterdomainDest d(*ip);
	BOOST_CHECK_THROW(d.get_asn(), std::exception);
	BOOST_CHECK_EQUAL(d.get_ip().get_address_text(), "127.0.0.0");
	BOOST_CHECK_EQUAL(d.get_ip().get_length(), 8);

	delete ip;
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_empty_less )
{
	InterdomainDest d1;
	InterdomainDest d2(100);
	BOOST_CHECK_THROW(d1 < d2, std::exception);
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_asn_compare )
{
	InterdomainDest d1(100);
	InterdomainDest d2(200);
	BOOST_CHECK(d1 < d2);
	BOOST_CHECK(d2 > d1);
	BOOST_CHECK(d1 != d2);
	BOOST_CHECK(d2 != d1);
	BOOST_CHECK(!(d1 == d2));
	BOOST_CHECK(!(d2 == d1));
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_ip_compare )
{
	p4p::IPPrefix* ip1 = p4p::IPPrefix::from_text("127.0.0.1", 8);
	p4p::IPPrefix* ip2 = p4p::IPPrefix::from_text("128.0.0.1", 8);

	InterdomainDest d1(*ip1);
	InterdomainDest d2(*ip2);
	BOOST_CHECK(d1 < d2);
	BOOST_CHECK(d2 > d1);
	BOOST_CHECK(d1 != d2);
	BOOST_CHECK(d2 != d1);
	BOOST_CHECK(!(d1 == d2));
	BOOST_CHECK(!(d2 == d1));

	delete ip1;
	delete ip2;
}

BOOST_AUTO_TEST_CASE ( interdomain_dest_ip_asn_compare )
{
	p4p::IPPrefix* ip = p4p::IPPrefix::from_text("127.0.0.1", 8);

	InterdomainDest d1(100);
	InterdomainDest d2(*ip);
	BOOST_CHECK(d1 < d2 || d1 > d2);
	BOOST_CHECK(d2 > d1 || d2 > d1);
	BOOST_CHECK(d1 != d2);
	BOOST_CHECK(d2 != d1);
	BOOST_CHECK(!(d1 == d2));
	BOOST_CHECK(!(d2 == d1));

	delete ip;
}
