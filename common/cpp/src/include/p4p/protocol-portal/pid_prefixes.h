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


#ifndef PID_PREFIXES_H
#define PID_PREFIXES_H

#include <p4p/pid.h>
#include <p4p/ip_addr.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {

class p4p_common_cpp_EXPORT PIDPrefixes
{
public:
	PIDPrefixes() {}

	PIDPrefixes(const PID& pid)
		: pid_(pid)
	{}

	const PID& get_pid() const			{ return pid_; }

	void add_prefix(const IPPrefix& prefix)		{ prefixes_.insert(prefix); }
	void remove_prefix(const IPPrefix& prefix)	{ prefixes_.erase(prefix); }
	unsigned int num_prefixes() const		{ return prefixes_.size(); }

	const std::set<IPPrefix>& get_prefixes() const	{ return prefixes_; }

private:
	PID pid_;
	std::set<IPPrefix> prefixes_;
};

}; // namespace portal
}; // namespace protocol
}; // namespace p4p

#endif

