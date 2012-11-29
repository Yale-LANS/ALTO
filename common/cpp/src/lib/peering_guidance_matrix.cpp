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


#include "p4p/app/peering_guidance_matrix.h"

#include <p4p/protocol-aoe/aoe.h>
#include <p4p/detail/inet_service.h>
#include <p4p/app/errcode.h>

namespace p4p {
namespace app {

PeeringGuidanceMatrix::PeeringGuidanceMatrix(const ISP* isp, 
				 const std::string& optEngineAddr, unsigned short optEnginePort) 
	: m_proto(new p4p::protocol::aoe::AOEProtocol(optEngineAddr, optEnginePort)),
	  m_isp(isp),
	  m_last_computed(0),
	  m_optengine_ttl(7 * 24 * 60 * 60), /* 1 week */
	  m_last_pdist_time(0)
{
	P4P_LOG_DEBUG("event:create_guidance,pgm:" << this << ",isp:" << m_isp << ",aoe:\"" << optEngineAddr << ":" << optEnginePort << "\"");
}

PeeringGuidanceMatrix::~PeeringGuidanceMatrix()
{
	if (isLogEnabled(LOG_DEBUG))
		logDebug("event:remove_guidance,pgm:%lx,isp:%lx", this, m_isp);

	delete m_proto;
}

int PeeringGuidanceMatrix::getWeights(int src_pid,
				double& out_intrapid_pct, double& out_intraisp_pct,
				std::vector<double>& out_weights) const
{
	detail::ScopedSharedLock lock(m_mutex);

	if (m_options.getOptType() == PeeringGuidanceMatrixOptions::OPT_LOCATION_ONLY)
	{
		out_intrapid_pct = m_options.getDefaultIntraPIDPercentage();
		out_intraisp_pct = m_options.getDefaultIntraISPPercentage();
		return 0;
	}

	if (src_pid < 0 || m_weights.size() <= (unsigned int)src_pid)
		return ERR_UNKNOWN_PID;

	out_intrapid_pct = m_intrapid_pcts[src_pid];
	out_intraisp_pct = m_intraisp_pcts[src_pid];
	
	out_weights.clear();
	std::copy(m_weights[src_pid].begin(), m_weights[src_pid].end(), std::back_inserter(out_weights));
	return 0;
}

int PeeringGuidanceMatrix::compute()
{
	/* NOTE: There is no synchronization until it is time to install the new guidance
	 * matrix. Thus, it is possible for two computations to
	 * take place at once. The last one to complete will take effect, though. */

	/* Location-only guidance does not require compuation */
	if (m_options.getOptType() == PeeringGuidanceMatrixOptions::OPT_LOCATION_ONLY)
	{
		P4P_LOG_DEBUG("event:compute_guidance,pgm:" << this << ",isp:" << m_isp << ",options:" << m_options << ",intrapid_pcts:\"" << m_intrapid_pcts << "\",intraisp_pcts:\"" << m_intraisp_pcts << "\"");
		return 0;
	}

	/* If the pDistances haven't changed and this is swarm-independent, there no need
	 * to recompute. This prevents generic matrices from being computed multiple times
	 * without having the user application know that they need to handle swarm-independent
	 * matrices any different.
	 *
	 * We'll also avoid computing if we don't yet have any pDistances.
	 */
	time_t pdist_time = m_isp->getPDistanceMap().getLastUpdate();
	if (!m_options.isSwarmDependent()		/* Swarm independent */
		&& m_last_pdist_time != 0		/* We have already computed the matrix */
		&& m_last_pdist_time == pdist_time)	/* pDistances haven't changed */
		return 0;

	/* TODO: Do we want to return an error code here? */
	if (pdist_time == 0)
		return 0;

	/* Get the current set of PIDs  */
	std::vector<p4p::PID> pids;
	m_isp->listPIDs(pids);

	/* Get local copy of the swarm state if needed */
	SwarmState swarm_state;
	if (m_options.isSwarmDependent())
	{
		detail::ScopedSharedLock lock(m_swarm_state_mutex);

		/* Ensure that we have the same number of PIDs in the swarm state
		 * as in the PID Map.  Note that this is not a complete sanity check
		 * (e.g., if the PID Map changed but the total number of PIDs may
		 * have remained the same), but it is a basic check for consistent
		 * sizes to let our code go through.
		 */
		if (m_swarm_state.getNumPIDs() != pids.size())
			return ERR_INVALID_SWARM_STATE;
	
		swarm_state = m_swarm_state;
	}

	/* Request guidance for the available PIDs */
	protocol::aoe::WeightsMap weights;
	protocol::aoe::PIDClassPercentages class_pcts;
	protocol::P4PProtocolMetaInfo meta;
        try
        {
                if (m_options.getMatchingUsesSwarmState())
                {
                        /* We're requesting guidance customized for the last swarm state */

                        /* Construct swarm state used by optimization engine */
                        protocol::aoe::SwarmState proto_swarm_state;
                        for (unsigned int i = 0; i < swarm_state.getNumPIDs(); ++i)
                                proto_swarm_state.add(pids[i],
                                        swarm_state.getNumSeeds(i),
                                        swarm_state.getNumLeechers(i),
                                        swarm_state.getUploadCapacity(i),
                                        swarm_state.getDownloadCapacity(i),
                                        swarm_state.getUploadRate(i),
                                        swarm_state.getDownloadRate(i),
                                        swarm_state.getNumActivePeers(i));

                        /* Request the weights using specified Optimization */
                        switch(m_options.getOptType())
                        {
                                case PeeringGuidanceMatrixOptions::OPT_FILESHARING_MATCHING:
                                        m_proto->get_weights(
                                                p4p::detail::p4p_token_cast<std::string>(
                                                        p4p::detail::InetService(m_isp->getPDistancePortalAddr(), m_isp->getPDistancePortalPort())),
                                                        proto_swarm_state,
                                                        weights, class_pcts, &meta);
                                        break;
                                case PeeringGuidanceMatrixOptions::OPT_STREAMING_SSM:
                                        m_proto->get_weights(
                                                p4p::detail::p4p_token_cast<std::string>(
                                                        p4p::detail::InetService(m_isp->getPDistancePortalAddr(), m_isp->getPDistancePortalPort())),
                                                        proto_swarm_state, "ssm", m_options.getStreamingChannelRate(),
                                                        weights, class_pcts, &meta);
                                        break;
                                case PeeringGuidanceMatrixOptions::OPT_STREAMING_SSMCM:
                                        m_proto->get_weights(
                                                p4p::detail::p4p_token_cast<std::string>(
                                                        p4p::detail::InetService(m_isp->getPDistancePortalAddr(), m_isp->getPDistancePortalPort())),
                                                        proto_swarm_state, "ssmcm", m_options.getStreamingChannelRate(),
                                                        weights, class_pcts, &meta);
                                        break;
                                default:
                                        return ERR_INVALID_OPT_TYPE;
                        }
                }
                else
                {
                        /* We're requesting generic guidance */
                        m_proto->get_weights(
                                p4p::detail::p4p_token_cast<std::string>(p4p::detail::InetService(m_isp->getPDistancePortalAddr(), m_isp->getPDistancePortalPort())),
                                pids.begin(), pids.end(),
                                weights, class_pcts, &meta);
                }
        } catch (...) {
                return ERR_GUIDANCE_UNAVAILABLE;
        }
	
	/* Convert result into our weight matrix and PID class percentages */
	WeightMatrix new_weights;
	ClassPctVector new_intrapid_pcts;
	ClassPctVector new_intraisp_pcts;
	for (std::vector<PID>::size_type i = 0; i < pids.size(); ++i)
	{
		/* It is meaningless to specify weights for an external PID as
		 * the Source PID.  Thus, we stop filling in the matrix as soon
		 * as we reach the first external PID (or invalid PID).
		 */
		if (pids[i].get_external() || pids[i] == PID::INVALID)
			break;

		/* Add a new row */
		new_weights.push_back(WeightRow(pids.size()));

		/* Fill in the row. If there is an error filling in a particular
		 * entry, then set it to 0.
		 * TODO: Do we need to renormalize if this happens?
		 */
		for (std::vector<PID>::size_type j = 0; j < pids.size(); ++j)
		{
			try
			{
				new_weights[i][j] = weights.get(pids[i], pids[j]);
			}
			catch (std::range_error& e)
			{
				new_weights[i][j] = 0;
			}
		}

		/* Add intra-PID and intra-ISP percentages */
		new_intrapid_pcts.push_back(class_pcts.get_intrapid(pids[i]));
		new_intraisp_pcts.push_back(class_pcts.get_intradomain(pids[i]));
	}

	if (m_options.getOverloadProtectionFactor() > 0)
	{
		/* Overload protection: 
		 * avoid overloading small-sized PIDs with too large weights 
		 * w_ij = min((n_ij+1)/n * factor, w_ij);
		 *
		
		* Count PID population percentage *
		* TODO: check error swarm state (e.g., getNum < 0) *
		double total_num_peers = 0;
		double pid_population[pids.size()];
		for (std::vector<PID>::size_type i = 0; i < pids.size(); ++i)
		{
			pid_population[i] = m_swarm_state.getNumSeeds(i) + m_swarm_state.getNumLeechers(i) + 1;
			total_num_peers += pid_population[i];
		}
		for (std::vector<PID>::size_type i = 0; i < pids.size(); ++i)
		{
			pid_population[i] /= total_num_peers;
		}

		for (std::vector<PID>::size_type i = 0; i < pids.size(); ++i)
		{
			double total_weights = 0.0;

			* Capture the weight upper bound *
			for (std::vector<PID>::size_type j = 0; j < pids.size(); ++i)
			{
				double weight_upperbound = m_options.getOverloadProtectionFactor() * pid_population[j];
				if (new_weights[i][j] > weight_upperbound) 
				{
					new_weights[i][j] = weight_upperbound; 
				}

				total_weights += new_weights[i][j];
			}

			* TODO: error weights? *
			if (total_weights < 1.0 - 1e-2 || total_weights > 1.0 + 1e-2)
				continue;

			* Renormalize weights *
			for (std::vector<PID>::size_type j = 0; j < pids.size(); ++i)
			{
				new_weights[i][j] /= total_weights;
			}
		}
		*/

	}

	/* Update data structures */
	detail::ScopedExclusiveLock lock(m_mutex);
	m_weights = new_weights;
	m_intrapid_pcts = new_intrapid_pcts;
	m_intraisp_pcts = new_intraisp_pcts;
	m_last_computed = time(NULL);
	m_optengine_ttl = meta.get_ttl();
	m_last_pdist_time = pdist_time;

	P4P_LOG_DEBUG("event:compute_guidance,pgm:" << this << ",isp:" << m_isp << ",options:" << m_options << ",swarm-state:\"" << m_swarm_state << "\",weights:\"" << m_weights << "\"");

	return 0;
}

int PeeringGuidanceMatrix::compute(const SwarmState& state)
{
	{
		detail::ScopedExclusiveLock lock(m_swarm_state_mutex);
		m_swarm_state = state;
	}

	/* Run through normal compute procedure */
	return compute();
}

time_t PeeringGuidanceMatrix::getNextComputeTime() const
{
	detail::ScopedExclusiveLock lock(m_mutex);

	/* If computation hasn't yet been done, return 0 */
	if (m_last_computed == 0)
		return 0;

	/* Otherwise return time at which computation should next be done */
	if (m_options.getUseOptEngineTTL())
		return m_last_computed + m_optengine_ttl;
	else
		return m_last_computed + m_options.getRecomputeInterval();
}

std::string PeeringGuidanceMatrix::getOptEngineAddr() const
{
	if (m_proto)
		return m_proto->get_host();
	else
		return "";
}

unsigned short PeeringGuidanceMatrix::getOptEnginePort() const
{
	if (m_proto)
		return m_proto->get_port();
	else
		return 0;
}

std::ostream& operator<<(std::ostream& os, const app::PeeringGuidanceMatrix::WeightMatrix& rhs)
{
	for (unsigned int s = 0; s < rhs.size(); ++s)
		for (unsigned int d = 0; d < rhs[s].size(); ++d)
			os << '{' << s << ',' << d << '=' << rhs[s][d] << '}';

	return os;
}

std::ostream& operator<<(std::ostream& os, const app::PeeringGuidanceMatrix::ClassPctVector& rhs)
{
	for (unsigned int s = 0; s < rhs.size(); ++s)
		os << '{' << s << '=' << rhs[s] << '}';

	return os;
}

};
};

