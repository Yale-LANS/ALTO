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
#include "p4p/detail/heap_with_delete.h"

using namespace p4p;
using namespace p4p::detail;

BOOST_AUTO_TEST_CASE ( heap_empty )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_single_element )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(!h.empty());
	BOOST_CHECK(h.top() == 1);
	BOOST_CHECK(h.pop());
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_small )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(4));
	for (int i = 5; i > 0; --i)
	{
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_duplicate )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(!h.push(3));
	for (int i = 5; i >= 1; --i)
	{
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_erase_left_middle )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.erase(6));
	for (int i = 7; i >= 1; --i)
	{
		if (i == 6) continue;
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_erase_right_middle )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.erase(3));
	for (int i = 7; i >= 1; --i)
	{
		if (i == 3) continue;
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_erase_top )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.erase(7));
	for (int i = 7; i >= 1; --i)
	{
		if (i == 7) continue;
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_erase_right_bottom )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.erase(2));
	for (int i = 7; i >= 1; --i)
	{
		if (i == 2) continue;
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_erase_left_bottom )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	BOOST_CHECK(h.erase(5));
	for (int i = 7; i >= 1; --i)
	{
		if (i == 5) continue;
		BOOST_CHECK_EQUAL(i, h.top());
		BOOST_CHECK(h.pop());
	}
	BOOST_CHECK(h.empty());
}

BOOST_AUTO_TEST_CASE ( heap_find )
{
	MaxHeapWithDelete<int> h;
	BOOST_CHECK(h.push(7));
	BOOST_CHECK(h.push(6));
	BOOST_CHECK(h.push(3));
	BOOST_CHECK(h.push(5));
	BOOST_CHECK(h.push(4));
	BOOST_CHECK(h.push(2));
	BOOST_CHECK(h.push(1));
	
	int found_value = 0;
	BOOST_CHECK(h.find(3, &found_value));
	BOOST_CHECK_EQUAL(3, found_value);

	BOOST_CHECK(h.erase(3));
	found_value = 0;
	BOOST_CHECK(!h.find(3, NULL));
}


