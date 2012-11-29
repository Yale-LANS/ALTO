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


/*
 * Unit Test: PID operations
 */

#include <boost/test/unit_test.hpp>

#include "p4p/detail/util.h"
#include "p4p/pid.h"

using namespace p4p;
using namespace p4p::detail;

BOOST_AUTO_TEST_CASE ( pid_construct_default )
{
	PID pid("myisp", 4);
	BOOST_CHECK_EQUAL(pid.get_isp(), "myisp");
	BOOST_CHECK_EQUAL(pid.get_num(), 4U);
	BOOST_CHECK_EQUAL(pid.get_external(), false);
}

BOOST_AUTO_TEST_CASE ( pid_construct_external )
{
	PID pid("myisp", 6, true);
	BOOST_CHECK_EQUAL(pid.get_isp(), "myisp");
	BOOST_CHECK_EQUAL(pid.get_num(), 6U);
	BOOST_CHECK_EQUAL(pid.get_external(), true);
}

BOOST_AUTO_TEST_CASE ( pid_internal_equal )
{
	BOOST_CHECK_EQUAL(PID("myisp", 4, false), PID("myisp", 4));
}

BOOST_AUTO_TEST_CASE ( pid_external_equal )
{
	BOOST_CHECK_EQUAL(PID("myisp", 4, true), PID("myisp", 4, true));
}

BOOST_AUTO_TEST_CASE ( pid_internal_to_string )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<std::string>(PID("myisp", 4)), "4.i.myisp");
}

BOOST_AUTO_TEST_CASE ( pid_external_to_string )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<std::string>(PID("myisp", 4, true)), "4.e.myisp");
}

BOOST_AUTO_TEST_CASE ( pid_internal_from_string )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<PID>("4.i.myisp"), PID("myisp", 4));
}

BOOST_AUTO_TEST_CASE ( pid_external_from_string )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<PID>("4.e.myisp"), PID("myisp", 4, true));
}

BOOST_AUTO_TEST_CASE ( pid_internal_string_identity )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<std::string>(p4p_token_cast<PID>("4.i.myisp.net")), "4.i.myisp.net");
}

BOOST_AUTO_TEST_CASE ( pid_external_string_identity )
{
	BOOST_CHECK_EQUAL(p4p_token_cast<std::string>(p4p_token_cast<PID>("4.e.myisp.net")), "4.e.myisp.net");
}
