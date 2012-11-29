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


#ifndef HEAP_WITH_DELETE_H
#define HEAP_WITH_DELETE_H

#include <map>
#include <vector>
#include <p4p/detail/random_access_set.h>

namespace p4p {
namespace detail {

/**
 * Max-Heap data structure with the ability to remove any element
 * in the queue. Note that most operations are now polylogarithmic
 * instead of logarithmic.
 *
 * Notice that two comparators may be specified. The first comparator
 * is used to avoid inserting duplicate elements.  The second comparator
 * is used to determine the ordering of elements within the heap. This
 * has the advantage of allowing an element (e.g., a task) to be replaced
 * by a similar element with either a higher or lower priority.
 */
template <class T, class Comparator = std::less<T>, class PriorityComparator = std::greater<T> >
class MaxHeapWithDelete
{
public:
	typedef RandomAccessSet<T, Comparator> Heap;
	typedef typename Heap::size_type size_type;

	/**
	 * Add element to the heap.
	 *
	 * @returns Returns true on success, or false otherwise (e.g.,
	 *   element already existed).
	 */
	bool push(const T& value)
	{
		/* Ensure we don't already have the element */
		if (m_heap.find(value) != Heap::npos)
			return false;

		/* Add element to the end of the heap, and
		 * restore the heap invariant. */
		m_heap.push_back(value);
		percolateUp(m_heap.size() - 1);
		return true;
	}

	/**
	 * Remove top element.
	 *
	 * @returns Returns true on success, or false otherwise (e.g.,
	 *   heap was empty).
	 */
	bool pop()
	{
		if (empty())
			return false;

		/* Swap top of heap with the last element. Then we
		 * can remove the last element. */
		m_heap.swap(0, m_heap.size() - 1);
		m_heap.pop_back();

		/* Percolate the new top element down to restore
		 * the heap invariant. */
		percolateDown(0);
		return true;
	}

	/**
	 * Return top element.
	 *
	 * @returns Reference to the element with maximum value
	 */
	const T& top() const			{ return m_heap[0]; }

	/**
	 * Indicate if heap is empty.
	 *
	 * @returns Returns true if heap contains no elements, false otherwise.
	 */
	bool empty() const			{ return m_heap.empty(); }

	/**
	 * Return number of elements in the heap
	 */
	size_type size() const			{ return m_heap.size(); }

	/**
	 * Delete element
	 *
	 * @param value Element to delete
	 * @returns Returns true on success, or false otherwise (e.g.,
	 *   element did not exist.
	 */
	bool erase(const T& value)
	{
		size_type idx = m_heap.find(value);
		if (idx == Heap::npos)
			return false;

		/* Swap element to erase with the last element in the heap. Then,
		 * we can erase the last element. */
		m_heap.swap(idx, m_heap.size() - 1);
		m_heap.pop_back();

		/* We may either need to move the element up or down; try both. */
		if (percolateUp(idx))
			return true;
		else if (percolateDown(idx))
			return true;

		/* No change was needed */
		return true;
	}

	/**
	 * Return existing element stored in the heap. Note that this
	 * isn't as useless as it might seem at first, since the
	 * comparison used for priorities is different than the comparison
	 * used to determine duplicates. This means we can do the following:
	 * - Look for the existing value
	 * - If priority should be adjusted, replace the existing
	 *   value with a different element.
	 * 
	 * @param value Value to search for
	 * @param out_value Output parameter; if not NULL, parameter assigned value of existing element if found
	 * @returns Returns true if successful, false otherwise (e.g., element not found)
	 */
	bool find(const T& value, T* out_value) const
	{
		size_type idx = m_heap.find(value);
		if (idx == Heap::npos)
			return false;

		if (out_value)
			*out_value = m_heap[idx];
		return true;
	}

private:
	/** Return index of parent */
	size_type parent(size_type idx) const			{ return ((idx + 1) / 2) - 1; }

	/** Return index of left-hand child */
	size_type left_child(size_type idx) const		{ return (2 * (idx + 1) - 1); }

	/** Return index of right-hand child */
	size_type right_child(size_type idx) const		{ return ((2 * (idx + 1)) + 1) - 1; }

	/**
	 * Return true if element at idx1 is larger than element at idx2.
	 * Note that comparator is a less-than comparator, so we flip
	 * the arguments.
	 */
	bool is_larger(size_type idx1, size_type idx2) const	{ return PriorityComparator()(m_heap[idx1], m_heap[idx2]); }

	/**
	 * Percolate element down the heap to meet the heap invariant.
	 *
	 * @param idx Start index in the heap
	 */
	bool percolateDown(size_type idx)
	{
		size_type left = left_child(idx);
		size_type right = right_child(idx);

		/* Check if left child is larger */
		size_type largest = (left < size() && is_larger(left, idx))
			? left
			: idx;

		/* Check right child */
		if (right < size() && is_larger(right, largest))
			largest = right;

		/* Adjust if necessary */
		if (largest != idx)
		{
			m_heap.swap(largest, idx);
			percolateDown(largest);

			/* We made progress */
			return true;
		}

		/* No change was made */
		return false;
	}

	/**
	 * Percolate element up the heap to meet the heap invariant.
	 */
	bool percolateUp(size_type idx)
	{
		bool moved = false;

		while (idx > 0 && is_larger(idx, parent(idx)))
		{
			/* Swap values with its parent */
			m_heap.swap(idx, parent(idx));

			/* Consider parent in next step */
			idx = parent(idx);

			/* Indicate that we've made progress */
			moved = true;
		}

		return moved;
	}

	/** Random-access container for storing heap elements */
	Heap m_heap;
};

};
};

#endif

