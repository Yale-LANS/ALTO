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


#ifndef RANDOM_ACCESS_SET_H
#define RANDOM_ACCESS_SET_H

#include <map>
#include <vector>

namespace p4p {
namespace detail {

/**
 * Random-access container (a vector) with the ability to erase
 * elements in logarithmic time.  Logarithmic time even applies
 * to erasing elements from the end of the vector (i.e., it is
 * no longer constant time).
 *
 * This container does not allow duplicate elements.
 *
 * This container pays a slight overhead in terms of per-element
 * memory (due to mapping elements to their corresponding index).
 */
template <class T, class Comparator = std::less<T> >
class RandomAccessSet
{
public:
	typedef std::vector<T> Vec;
	typedef typename Vec::size_type size_type;
	typedef std::map<T, size_type, Comparator> VecIdxMap;

	/**
	 * Constant indicating element was not found.
	 */
	static const size_type npos = ~((size_type)0);

	/**
	 * Add element to the end of the vector.
	 *
	 * @returns Returns true on success, or false otherwise (e.g.,
	 *   element already existed).
	 */
	bool push_back(const T& value)
	{
		/* Ensure we don't already have the element */
		if (find(value) != npos)
			return false;

		/* add element and record its index */
		m_vec.push_back(value);
		m_vec_index[value] = size() - 1;
		return true;
	}

	/**
	 * Remove element from end of the vector. Method has
	 * no effect if vector is empty.
	 */
	void pop_back()
	{
		/* Nothing to do if there's no elements */
		if (empty())
			return;

		/* Remove index mapping for the element, then the element itself */
		m_vec_index.erase(m_vec.back());
		m_vec.pop_back();
	}

	/**
	 * Indicate if vector is empty.
	 *
	 * @returns Returns true if vector has no elements, false otherwise.
	 */
	bool empty() const				{ return m_vec.empty(); }

	/**
	 * Return number of elements in vector.
	 *
	 * @returns Returns number of elements in vector.
	 */
	size_type size() const				{ return m_vec.size(); }

	/**
	 * Overload subscript operator to provide access to intermediate
	 * elements.  We only allow reading random elements.
	 *
	 * @param idx Index of element to return
	 * @returns Returns element at desired index
	 */
	const T& operator[](size_type idx) const	{ return m_vec[idx]; }

	/**
	 * Swap elements at the specified indices.
	 *
	 * @param idx1 Index of first element to swap
	 * @param idx2 Index of second element to swap
	 */
	void swap(size_type idx1, size_type idx2)
	{
		m_vec_index[m_vec[idx1]] = idx2;
		m_vec_index[m_vec[idx2]] = idx1;
		std::swap(m_vec[idx1], m_vec[idx2]);
	}

	/**
	 * Delete element
	 *
	 * @param value Element to delete
	 * @returns Returns true on success, or false otherwise (e.g.,
	 *   element did not exist.
	 */
	bool erase(const T& value)
	{
		size_type idx = find(value);
		if (idx == npos)
			return false;

		/* Swap element to erase with the last element. Then,
		 * we can just pop the last element. */
		swap(idx, size() - 1);
		pop_back();
		return true;
	}

	/**
	 * Find index of the desired element.
	 * 
	 * @param value Value to search for
	 * @returns Returns index if successful, or npos if element
	 *   was not found.
	 */
	size_type find(const T& value) const
	{
		typename VecIdxMap::const_iterator itr = m_vec_index.find(value);
		if (itr == m_vec_index.end())
			return npos;
		return itr->second;
	}

private:
	/** Map from elements to their indexes in the vector */
	VecIdxMap m_vec_index;

	/** Random-access container for storing elements */
	Vec m_vec;
};

};
};

#endif

