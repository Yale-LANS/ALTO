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

#include "pid_map.h"

BOOST_AUTO_TEST_CASE ( pid_map_lookup_exact )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 0, NetLocation(1, 1), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(result = pid_map.lookup("127.0.0.0", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(1, 1));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_duplicates )
{
	PIDMap pid_map;

	BlockWriteLock lock(pid_map);
	BOOST_CHECK(pid_map.add("127.0.0.0", 32, NetLocation(1, 1), lock));
	BOOST_CHECK(pid_map.add("127.0.0.0", 32, NetLocation(1, 1), lock));
	BOOST_CHECK(!pid_map.add("127.0.0.0", 32, NetLocation(2, 1), lock));
	BOOST_CHECK(pid_map.add("127.0.0.0", 24, NetLocation(1, 1), lock));
}

BOOST_AUTO_TEST_CASE ( pid_map_lookup_child )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 8, NetLocation(1, 1), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(result = pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(1, 1));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_lookup_longest )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 8, NetLocation(1, 1), lock));
		BOOST_CHECK(pid_map.add("127.0.0.0", 9, NetLocation(2, 2), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(result = pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(2, 2));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_lookup_failed_empty_tree )
{
	PIDMap pid_map;

	{
		BlockReadLock lock(pid_map);

		BOOST_CHECK(!pid_map.lookup("127.0.1.0", lock));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_lookup_failed_nonempty_tree )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 8, NetLocation(1, 1), lock));
	}

	{
		BlockReadLock lock(pid_map);

		BOOST_CHECK(!pid_map.lookup("128.0.0.0", lock));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_remove )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 8, NetLocation(1, 1), lock));
		BOOST_CHECK(pid_map.add("127.0.0.0", 9, NetLocation(2, 2), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(result = pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(2, 2));
	}

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.remove("127.0.0.0", 9, NetLocation(2, 2), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(result = pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(1, 1));
	}
}

BOOST_AUTO_TEST_CASE ( pid_map_remove_location )
{
	PIDMap pid_map;

	{
		BlockWriteLock lock(pid_map);
		BOOST_CHECK(pid_map.add("127.0.0.0", 8, NetLocation(1, 1), lock));
		BOOST_CHECK(pid_map.add("127.0.0.0", 10, NetLocation(1, 1), lock));
		BOOST_CHECK(pid_map.add("127.0.0.0", 9, NetLocation(2, 2), lock));
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(1, 1));
	}

	{
		BlockWriteLock lock(pid_map);
		pid_map.remove(NetLocation(1, 1), lock);
	}

	{
		BlockReadLock lock(pid_map);

		const NetLocation* result;
		BOOST_CHECK(pid_map.lookup("127.0.0.1", lock));
		BOOST_CHECK_EQUAL(*result, NetLocation(2, 2));
	}
}
