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

#include "pid_matrix.h"

BOOST_AUTO_TEST_CASE ( pid_matrix_add_pid )
{
	PIDMatrix pid_matrix;

	{
		NetLocation loc(1, 1);
		BlockWriteLock lock(pid_matrix);
		BOOST_CHECK(pid_matrix.add_pid(loc, lock));
		pid_matrix.set_by_loc(loc, loc, 0.0, lock);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc, loc, lock), 0.0, 0.001);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_rows(lock), 1);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_cols(lock), 1);
	}
}

BOOST_AUTO_TEST_CASE ( pid_matrix_add_pid_duplicate_fail )
{
	PIDMatrix pid_matrix;

	{
		NetLocation loc(1, 1);
		BlockWriteLock lock(pid_matrix);
		BOOST_CHECK(pid_matrix.add_pid(loc, lock));
		BOOST_CHECK(!pid_matrix.add_pid(loc, lock));
		pid_matrix.set_by_loc(loc, loc, 0.0, lock);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc, loc, lock), 0.0, 0.001);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_rows(lock), 1);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_cols(lock), 1);
	}
}

BOOST_AUTO_TEST_CASE ( pid_matrix_set_value )
{
	PIDMatrix pid_matrix;

	{
		NetLocation loc(1, 1);
		BlockWriteLock lock(pid_matrix);
		BOOST_CHECK(pid_matrix.add_pid(loc, lock));
		pid_matrix.set_by_loc(loc, loc, 0.0, lock);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc, loc, lock), 0.0, 0.001);
		pid_matrix.set_by_loc(loc, loc, 1.0, lock);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc, loc, lock), 1.0, 0.001);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_rows(lock), 1);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_cols(lock), 1);
	}
}

BOOST_AUTO_TEST_CASE ( pid_matrix_get_locations )
{
	PIDMatrix pid_matrix;

	{
		NetLocation loc1(1, 1);
		NetLocation loc2(2, 2);
		NetLocationVector loc_vec; loc_vec.push_back(loc1); loc_vec.push_back(loc2);
		BlockWriteLock lock(pid_matrix);
		BOOST_CHECK(pid_matrix.add_pid(loc1, lock));
		BOOST_CHECK(pid_matrix.add_pid(loc2, lock));
		const PIDMatrixNetLocationsByIdx& loc_vec_act = pid_matrix.get_locations_vector(lock);
		BOOST_CHECK_EQUAL_COLLECTIONS(loc_vec_act.begin(), loc_vec_act.end(), loc_vec.begin(), loc_vec.end());
		BOOST_CHECK_EQUAL(pid_matrix.get_num_rows(lock), 2);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_cols(lock), 2);
	}
}

BOOST_AUTO_TEST_CASE ( pid_matrix_get_set_2x2 )
{
	PIDMatrix pid_matrix;

	{
		NetLocation loc1(1, 1);
		NetLocation loc2(2, 2);
		BlockWriteLock lock(pid_matrix);
		BOOST_CHECK(pid_matrix.add_pid(loc1, lock));
		BOOST_CHECK(pid_matrix.add_pid(loc2, lock));
		pid_matrix.set_by_loc(loc1, loc1, 11.0, lock);
		pid_matrix.set_by_loc(loc1, loc2, 12.0, lock);
		pid_matrix.set_by_loc(loc2, loc1, 21.0, lock);
		pid_matrix.set_by_loc(loc2, loc2, 22.0, lock);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc1, loc1, lock), 11.0, 0.001);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc1, loc2, lock), 12.0, 0.001);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc2, loc1, lock), 21.0, 0.001);
		BOOST_CHECK_CLOSE(pid_matrix.get_by_loc(loc2, loc2, lock), 22.0, 0.001);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_rows(lock), 2);
		BOOST_CHECK_EQUAL(pid_matrix.get_num_cols(lock), 2);
	}
}
