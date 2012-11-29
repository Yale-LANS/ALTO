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


#include "p4p/pid.h"

#include <string.h>

namespace p4p {

const PID PID::INVALID = PID("invalid.p4p", 0, true);
const PID PID::DEFAULT = PID("pid.p4p", 0);

std::size_t hash_value(const PID& pid)
{
	return pid.get_num();
}

std::ostream& operator<<(std::ostream& os, const PID& rhs)
{
	os << rhs.get_num() << (rhs.get_external() ? ".e." : ".i.") << rhs.get_isp();
	return os;
}

std::istream& operator>>(std::istream& is, PID& rhs)
{
	/* First is the PID number */
	unsigned int num;
	if (!(is >> num))
		return is;

	if (is.get() != '.')
	{
		is.clear(std::ios::failbit);
		return is;
	}

	/* Next is the 'external' indicator */
	char ext_buf[2];
	ext_buf[0] = '\0';
	if (!is.getline(ext_buf, 2, '.'))
		return is;

	bool external;
	if (strcmp("i", ext_buf) == 0)
		external = false;
	else if (strcmp("e", ext_buf) == 0)
		external = true;
	else
	{
		is.clear(std::ios::failbit);
		return is;
	}

	/* The rest is the ISP identifier */
	ISPID isp;
	if (!(is >> isp))
		return is;

	rhs = PID(isp, num, external);
	return is;
}

};

