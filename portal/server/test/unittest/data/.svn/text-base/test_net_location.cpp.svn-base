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

#include <map>

#include "net_location.h"

BOOST_AUTO_TEST_CASE ( test_net_location_primitive )
{
	NetLocation i(1, 0);
	NetLocation j(1, 1);
	NetLocation k(1, 0, true);

	BOOST_CHECK_EQUAL(i.get_external(), false);
	BOOST_CHECK_EQUAL(i.get_asn(), 1);
	BOOST_CHECK_EQUAL(i.get_pid(), 0);

	BOOST_CHECK_MESSAGE(i == i, "equality not reflexive");
	BOOST_CHECK_MESSAGE(i != j, "equality holds between distinct NetLocation");
}

BOOST_AUTO_TEST_CASE ( test_net_location_as_map_key )
{
	NetLocationVector pid(3);
	pid[0] = NetLocation(1, 0);
	pid[1] = NetLocation(1, 1);
	pid[2] = NetLocation(1, 2);

	typedef std::map<NetLocation, double> NetLocationMap;
	NetLocationMap m;
	m[ pid[0] ] = 4;
	m[ pid[1] ] = 3;
	m[ pid[2] ] = 8;

	BOOST_CHECK_EQUAL(m[ pid[0] ], 4);
	BOOST_CHECK_EQUAL(m[ pid[1] ], 3);
	BOOST_CHECK_EQUAL(m[ pid[2] ], 8);
}
