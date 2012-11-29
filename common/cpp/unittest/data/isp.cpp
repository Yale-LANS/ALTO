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

#include "p4p/isp.h"
#include "p4p/detail/util.h"
#include "p4p/detail/temp_file_stream.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace p4p;
using namespace p4p::detail;

BOOST_AUTO_TEST_CASE ( isp_pidmap_file )
{
	/* Load PID Map with Yale IP addresses */
	TempFileStream pidmap_file;
	pidmap_file << "0.i.isp.net 1 128.36.0.0/16" << std::endl;
	pidmap_file << "1.i.isp.net 1 130.132.0.0/16" << std::endl;
	pidmap_file << "100.e.isp.net 1 0.0.0.0/0" << std::endl;
	pidmap_file.flush();

	/* Use empty pDistance map */
	TempFileStream pdist_file;
	pdist_file.flush();

	/* Construct ISP object, reading from files */
	ISP isp(pidmap_file.getFilename(), pdist_file.getFilename());
	BOOST_CHECK_EQUAL(0, isp.loadP4PInfo());

	unsigned int num_intraisp_pids, num_total_pids;
	isp.getNumPIDs(&num_intraisp_pids, &num_total_pids);

	BOOST_CHECK_EQUAL(2, num_intraisp_pids);
	BOOST_CHECK_EQUAL(3, num_total_pids);

	BOOST_CHECK_EQUAL("0.i.isp.net", p4p::detail::p4p_token_cast<std::string>(isp.getPIDInfo(0)));
	BOOST_CHECK_EQUAL("1.i.isp.net", p4p::detail::p4p_token_cast<std::string>(isp.getPIDInfo(1)));
	BOOST_CHECK_EQUAL("100.e.isp.net", p4p::detail::p4p_token_cast<std::string>(isp.getPIDInfo(2)));

	BOOST_CHECK_EQUAL(0, isp.lookup("128.36.1.1"));
	BOOST_CHECK_EQUAL(1, isp.lookup("130.132.1.1"));
	BOOST_CHECK_EQUAL(2, isp.lookup("129.23.23.23"));
}

static void isp_update(ISP* isp)
{
	isp->loadP4PInfo();
}

BOOST_AUTO_TEST_CASE ( isp_update_concurrent )
{
	const std::string	PORTAL_A_SERVER = "p4p-4.cs.yale.edu";
	const unsigned short	PORTAL_A_PORT = 6679;
	const std::string	PORTAL_B_SERVER = "p4p-3.cs.yale.edu";
	const unsigned short	PORTAL_B_PORT = 6685;

	/* Comment out the 'return' statement to enable this test. This
	 * test is disabled by default since it refers to hard-coded
	 * server names. */
	return;

	ISP isp_a(PORTAL_A_SERVER, PORTAL_A_PORT);
	ISP isp_b(PORTAL_B_SERVER, PORTAL_B_PORT);

	boost::thread isp_a_update(boost::bind(&isp_update, &isp_a));
	boost::thread isp_b_update(boost::bind(&isp_update, &isp_b));
	isp_a_update.join();
	isp_b_update.join();
}
