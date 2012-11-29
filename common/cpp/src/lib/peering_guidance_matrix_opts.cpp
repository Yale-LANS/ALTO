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


#include "p4p/app/peering_guidance_matrix_opts.h"

namespace p4p {
namespace app {

PeeringGuidanceMatrixOptions PeeringGuidanceMatrixOptions::LocationOnly(const double intrapid_pct, const double intraisp_pct)
{
	PeeringGuidanceMatrixOptions opts;
	opts.setOptType(OPT_LOCATION_ONLY);
	opts.setUseOptEngineTTL(false);
	opts.setLocationSelectionPercentage(intrapid_pct, intraisp_pct);
	opts.setRecomputeInterval(0);			/* no recompute */	
	return opts;
}

PeeringGuidanceMatrixOptions PeeringGuidanceMatrixOptions::FilesharingGeneric()
{
	PeeringGuidanceMatrixOptions opts;
	opts.setRecomputeInterval(7 * 24 * 60 * 60);	/* One week */
	return opts;
}

PeeringGuidanceMatrixOptions PeeringGuidanceMatrixOptions::FilesharingSwarmDependent()
{
	PeeringGuidanceMatrixOptions opts;
	opts.setMatchingUsesSwarmState(true);
	opts.setOverloadProtectionFactor(1.5);
	return opts;
}

PeeringGuidanceMatrixOptions PeeringGuidanceMatrixOptions::StreamingSwarmDependent(double channel_rate, OptType opt_type)
{
	PeeringGuidanceMatrixOptions opts;
	opts.setOptType(opt_type);
	opts.setMatchingUsesSwarmState(true);
	opts.setOverloadProtectionFactor(1.5);
	opts.setStreamingChannelRate(channel_rate);
	return opts;
}

PeeringGuidanceMatrixOptions::PeeringGuidanceMatrixOptions()
	: m_opttype(OPT_FILESHARING_MATCHING),
	  m_matching_uses_swarm_state(false),
	  m_overload_protection_factor(0.0),
	  m_use_optengine_ttl(true),
	  m_recompute_interval(5 * 60),			/* 5 minutes */
	  m_streamingdependent_channelrate(48000),	/* 375 kbps */
	  m_intrapid_pct(0.75),
	  m_intraisp_pct(0.9)
{
}

bool PeeringGuidanceMatrixOptions::isSwarmDependent() const
{
	if (m_matching_uses_swarm_state)
		return true;

	if (m_overload_protection_factor > 0.0)
		return true;

	return false;
}

void PeeringGuidanceMatrixOptions::setLocationSelectionPercentage(const double intrapid_pct, const double intraisp_pct)
{
	m_intrapid_pct = (intrapid_pct < 0.0 || intrapid_pct > intraisp_pct) ? m_intrapid_pct : intrapid_pct;
	m_intraisp_pct = (intraisp_pct < intrapid_pct || intraisp_pct > 0.99) ? m_intraisp_pct : intraisp_pct;
}

bool PeeringGuidanceMatrixOptions::operator==(const PeeringGuidanceMatrixOptions& rhs) const
{
	return m_opttype == rhs.m_opttype
		&& m_matching_uses_swarm_state == rhs.m_matching_uses_swarm_state
		&& m_overload_protection_factor == rhs.m_overload_protection_factor;
}

bool PeeringGuidanceMatrixOptions::operator!=(const PeeringGuidanceMatrixOptions& rhs) const
{
	return !(*this == rhs);
}

bool PeeringGuidanceMatrixOptions::operator<(const PeeringGuidanceMatrixOptions& rhs) const
{
	if (m_opttype < rhs.m_opttype)							return true;
	if (m_opttype > rhs.m_opttype)							return false;
	if (m_matching_uses_swarm_state < rhs.m_matching_uses_swarm_state)		return true;
	if (m_matching_uses_swarm_state > rhs.m_matching_uses_swarm_state)		return false;
	if (m_overload_protection_factor < rhs.m_overload_protection_factor)		return true;
	if (m_overload_protection_factor > rhs.m_overload_protection_factor)		return false;
	if (m_streamingdependent_channelrate < rhs.m_streamingdependent_channelrate)	return true;
	if (m_streamingdependent_channelrate > rhs.m_streamingdependent_channelrate)	return false;
	if (m_intrapid_pct < rhs.m_intrapid_pct )			return true;
	if (m_intrapid_pct > rhs.m_intrapid_pct )			return false;
	if (m_intraisp_pct < rhs.m_intraisp_pct )			return true;
	if (m_intraisp_pct > rhs.m_intraisp_pct )			return false;
	return false;
}

bool PeeringGuidanceMatrixOptions::operator>(const PeeringGuidanceMatrixOptions& rhs) const
{
	return (rhs < *this);
}

std::ostream& operator<<(std::ostream& os, const PeeringGuidanceMatrixOptions& rhs)
{
	return os
		<< '{'
		<< "opt_type:" << rhs.getOptType() << ','
		<< "matching_uses_swarm_state:" << rhs.getMatchingUsesSwarmState() << ','
		<< "overload_protection_factor:" << rhs.getOverloadProtectionFactor() << ','
		<< "use_opt_engine_ttl:" << rhs.getUseOptEngineTTL() << ','
		<< "recompute_interval:" << rhs.getRecomputeInterval() << ','
		<< "streaming_channel_rate:" << rhs.getStreamingChannelRate() << ','
		<< "default_intrapid_pct:" << rhs.getDefaultIntraPIDPercentage() << ','
		<< "default_intraisp_pct:" << rhs.getDefaultIntraISPPercentage() << ','
		<< "is_swarm_dependent:" << rhs.isSwarmDependent()
		<< '}';
}

};
};

