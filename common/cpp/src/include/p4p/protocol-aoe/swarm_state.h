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


#ifndef P4P_AOEProtocol_SWARM_STATE_H
#define P4P_AOEProtocol_SWARM_STATE_H

#include <set>
#include <map>
#include <p4p/pid.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace aoe {

class p4p_common_cpp_EXPORT SwarmState
{
public:
	typedef std::set<PID> PIDList;

	void add(const PID& pid, unsigned int seeds, unsigned int leechers, double ul_cap, double dl_cap, double ul_rate, double dl_rate, unsigned int active_peers)
	{
		pids_.insert(pid);
		pid_seeds_[pid] = seeds;
		pid_leechers_[pid] = leechers;
		pid_ul_cap_[pid] = ul_cap;
		pid_dl_cap_[pid] = dl_cap;
		pid_ul_rate_[pid] = ul_rate;
		pid_dl_rate_[pid] = dl_rate;
		pid_active_peers_[pid] = active_peers;
	}

	const PIDList& get_pids() { return pids_; }
	unsigned int get_seeds(const PID& pid) { return pid_seeds_[pid]; }
	unsigned int get_leechers(const PID& pid) { return pid_leechers_[pid]; }
	double get_ul_cap(const PID& pid) { return pid_ul_cap_[pid]; }
	double get_dl_cap(const PID& pid) { return pid_dl_cap_[pid]; }
	double get_ul_rate(const PID& pid) { return pid_ul_rate_[pid]; }
	double get_dl_rate(const PID& pid) { return pid_dl_rate_[pid]; }
	unsigned int get_active_peers(const PID& pid) { return pid_active_peers_[pid]; }

private:
	typedef std::map<PID, unsigned int> PIDSizeMap;
	typedef std::map<PID, double> PIDCapacityMap;

	PIDList pids_;
	PIDSizeMap pid_seeds_;
	PIDSizeMap pid_leechers_;
	PIDCapacityMap pid_ul_cap_;
	PIDCapacityMap pid_dl_cap_;
	PIDCapacityMap pid_ul_rate_;
	PIDCapacityMap pid_dl_rate_;
	PIDSizeMap pid_active_peers_;
};

};
};
};

#endif

