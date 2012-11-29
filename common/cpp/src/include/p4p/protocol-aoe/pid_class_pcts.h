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


#ifndef P4P_AOEProtocol_PID_CLASS_PCTS_H
#define P4P_AOEProtocol_PID_CLASS_PCTS_H

#include <map>
#include <p4p/pid.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace aoe {

class p4p_common_cpp_EXPORT PIDClassPercentages
{
public:
	void add(const PID& src, double intrapid, double intradomain, double interdomain)
	{
		intrapid_[src] = intrapid;
		intradomain_[src] = intradomain;
		interdomain_[src] = interdomain;
	}

	double get_intrapid(const PID& src) const { return get_val(intrapid_, src); }
	double get_intradomain(const PID& src) const { return get_val(intradomain_, src); }
	double get_interdomain(const PID& src) const { return get_val(interdomain_, src); }

private:
	typedef std::map<PID, double> PIDPctMap;

	double get_val(const PIDPctMap& pcts, const PID& pid) const
	{
		PIDPctMap::const_iterator itr = pcts.find(pid);
		if (itr == pcts.end())
			return 0;
		return itr->second;
	}

	PIDPctMap intrapid_;
	PIDPctMap intradomain_;
	PIDPctMap interdomain_;
};

};
};
};

#endif

