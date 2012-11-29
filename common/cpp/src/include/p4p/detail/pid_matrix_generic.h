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


#ifndef PID_MATRIX_GENERIC_H
#define PID_MATRIX_GENERIC_H

#include <set>
#include <map>
#include <iostream>
#include <p4p/pid.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace detail {

template <class T>
class p4p_common_cpp_ex_EXPORT PIDMatrixGeneric
{
public:
	typedef std::map<const PID, T> Row;
	typedef std::pair<const PID, T> RowEntry;
	typedef std::map<const PID, Row> Matrix;

	PIDMatrixGeneric() {}

	template <class OutputIterator>
	void get_srcs(OutputIterator out) const
	{
		for (typename Matrix::const_iterator itr = data_.begin(); itr != data_.end(); ++itr)
			*out++ = itr->first;
	}

	bool has_row(const PID& src) const
	{
		return data_.find(src) != data_.end();
	}

	const Row& get_row(const PID& src) const throw (std::range_error)
	{
		typename Matrix::const_iterator itr = data_.find(src);
		if (itr == data_.end())
			throw std::range_error("invalid source PID");
		return itr->second;
	}

	const RowEntry& get_entry(const Row& row, const PID& dst) const throw (std::range_error)
	{
		typename Row::const_iterator itr = row.find(dst);
		if (itr == row.end())
			throw std::range_error("invalid destination PID");
		return *itr;
	}

	void set(const PID& i, const PID& j, const T& value)
	{
		get_entry_make(get_row_make(i), j).second = value;
	}	

	const T& get(const PID& i, const PID& j) const throw (std::range_error)
	{
		return get_entry(get_row(i), j).second;
	}

	const T& get(const Row& row, const PID& dst) const throw (std::range_error)
	{
		return get_entry(row, dst).second;
	}


private:
	Row& get_row_make(const PID& src)
	{
		typename Matrix::iterator itr = data_.find(src);
		if (itr == data_.end())
			itr = data_.insert(std::make_pair(src, Row())).first;
		return itr->second;
	}

	RowEntry& get_entry_make(Row& row, const PID& dst)
	{
		typename Row::iterator itr = row.find(dst);
		if (itr == row.end())
			itr = row.insert(std::make_pair(dst, T())).first;
		return *itr;
	}

	Matrix data_;
};

}; // namespace detail
}; // namespace p4p

#endif

