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


#ifndef P4P_UTIL_H
#define P4P_UTIL_H

#include <string>
#include <sstream>
#include <stdexcept>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace detail {

template<typename Target, typename Source>
Target p4p_token_cast(const Source& arg) throw (std::invalid_argument)
{
	/* Convert to string */
	std::ostringstream os;
	os << arg;

	/* Convert to target type */
	Target result;
	std::istringstream is(os.str());
	if (!(is >> result))
	{
		char* a = NULL;
		*a = 'a';
		throw std::invalid_argument("cannot cast argument to target type");
	}

	return result;
}

p4p_common_cpp_EXPORT std::string trim(const std::string& s);

template <class T>
p4p_common_cpp_ex_EXPORT T clip(T val, const T& min_val, const T& max_val)
{
	if (val < min_val)
		val = min_val;
	else if (val > max_val)
		val = max_val;
	return val;
}

#define round_int(x)	((unsigned int)((x) + 0.5))

template <class T>
int compare(const T& a, const T& b)
{
	if (a < b)
		return -1;

	if (a > b)
		return 1;

	return 0;
}

/**
 * Return string error message for a system error.
 *
 * @param e Error code (retrieved from 'errno' variable)
 */
std::string get_errno_str(int e);

}; // namespace detail
}; // namespace p4p

#endif

