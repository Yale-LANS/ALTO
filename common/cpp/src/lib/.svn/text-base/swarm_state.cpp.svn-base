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


#include "p4p/app/swarm_state.h"

#include <p4p/errcode.h>

namespace p4p {
namespace app {

SwarmState::SwarmState()
{
}

SwarmState::SwarmState(unsigned int num_pids)
	: m_pids(num_pids)
{
}

unsigned int SwarmState::getNumPIDs() const
{
	return m_pids.size();
}

int SwarmState::getNumSeeds(int pid) const
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].num_seeds;
}

int SwarmState::setNumSeeds(int pid, int value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].num_seeds = value;
	return 0;
}

int SwarmState::getNumLeechers(int pid) const
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].num_leechers;
}

int SwarmState::setNumLeechers(int pid, int value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].num_leechers = value;
	return 0;
}

float SwarmState::getUploadCapacity(int pid) const
{
	if (!isValid(pid))
		return (float)p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].upload_cap;
}

int SwarmState::setUploadCapacity(int pid, float value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].upload_cap = value;
	return 0;
}

float SwarmState::getDownloadCapacity(int pid) const
{
	if (!isValid(pid))
		return (float)p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].download_cap;
}

int SwarmState::setDownloadCapacity(int pid, float value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].download_cap = value;
	return 0;
}

float SwarmState::getUploadRate(int pid) const
{
	if (!isValid(pid))
		return (float)p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].upload_rate;
}

int SwarmState::setUploadRate(int pid, float value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].upload_rate = value;
	return 0;
}

float SwarmState::getDownloadRate(int pid) const
{
	if (!isValid(pid))
		return (float)p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].download_cap;
}

int SwarmState::setDownloadRate(int pid, float value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].download_rate = value;
	return 0;
}

int SwarmState::getNumActivePeers(int pid) const
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	return m_pids[pid].num_active_peers;
}

int SwarmState::setNumActivePeers(int pid, int value)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;
	m_pids[pid].num_active_peers = value;
	return 0;
}

int SwarmState::update(int pid,
		       int change_seeds, int change_leechers,
		       float change_uploadcap, float change_downloadcap,
		       float change_uploadrate, float change_downloadrate,
		       int change_active_peers)
{
	if (!isValid(pid))
		return p4p::ERR_UNKNOWN_PID;

	PIDState& pid_state = m_pids[pid];
	pid_state.upload_cap	+= change_seeds;
	pid_state.num_leechers	+= change_leechers;
	pid_state.upload_cap	+= change_uploadcap;
	pid_state.download_cap	+= change_downloadcap;
	pid_state.upload_rate	+= change_uploadrate;
	pid_state.download_rate	+= change_downloadrate;
	pid_state.num_active_peers += change_active_peers;
	return 0;
}

std::ostream& operator<<(std::ostream& os, const SwarmState& rhs)
{
	for (unsigned int i = 0; i < rhs.getNumPIDs(); ++i)
	{
		os << '{'
			<< i << ','
			<< "seeds:" << rhs.getNumSeeds(i) << ','
			<< "leechers:" << rhs.getNumLeechers(i) << ','
			<< "upload_cap:" << rhs.getUploadCapacity(i) << ','
			<< "download_cap:" << rhs.getDownloadCapacity(i) << ','
			<< "upload_rate:" << rhs.getUploadRate(i) << ','
			<< "download_rate:" << rhs.getDownloadRate(i) << ','
			<< "active_peers:" << rhs.getNumActivePeers(i)
			<< '}';
	}
	return os;
}

};
};

