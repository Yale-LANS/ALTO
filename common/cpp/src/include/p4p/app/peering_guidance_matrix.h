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


#ifndef P4P_PEERING_GUIDANCE_MATRIX_H
#define P4P_PEERING_GUIDANCE_MATRIX_H

#include <vector>
#include <time.h>
#include <p4p/p4p.h>
#include <p4p/detail/mutex.h>
#include <p4p/app/swarm_state.h>
#include <p4p/app/peering_guidance_matrix_opts.h>
#include <p4p/detail/compiler.h>

namespace p4p {

/* Forward declaration */
namespace protocol {
namespace aoe {
class AOEProtocol;
};
};

namespace app {

/**
 * Manage a peering guidance weight matrix.  The matrix is computed from ISP
 * pDistances and (optionally) current swarm state.  The matrix directly specifies
 * the portion of peers to select from each PID.
 *
 * The matrix may either be computed locally, or by an Optimization Engine server.  This class
 * currently only implements the case where the matrix is computed by an Optimization
 * Engine server.
 */
class p4p_common_cpp_EXPORT PeeringGuidanceMatrix
{
public:
	/**
	 * Constructor specifying an Optimization Engine server as the data source.
	 *
	 * @param isp ISP for which peering guidance is provided.
	 * @param optEngineAddr Hostname or IP address of Optimization Engine server
	 * @param optEnginePort Port number of Optimization Engine server
	 */
	PeeringGuidanceMatrix(const ISP* isp,
			const std::string& optEngineAddr, unsigned short optEnginePort);

	
	/**
	 * Constructor specifying that weight matrix is computed locally.
	 *
	 * @param isp ISP for which peering guidance is provided.
	 */
	PeeringGuidanceMatrix(const ISP* isp) { throw std::runtime_error("Not currently supported"); }

	/**
	 * Destructor
	 */
	~PeeringGuidanceMatrix();

	/**
	 * Get the ISP for which guidance is provided.
	 *
	 * @returns Returns pointer to ISP.
	 */
	const ISP* getISP() const					{ return m_isp; }

	/**
	 * Get the the full set of peering guidance matrix options.
	 */
	const PeeringGuidanceMatrixOptions& getOptions() const		{ return m_options; }

	/**
	 * Configure all peering guidance matrix options.
	 *
	 * @param value PeeringGuidanceMatrxOptions object containing all
	 *   desired options.
	 */
	void setOptions(const PeeringGuidanceMatrixOptions& value)	{ m_options = value; }

	/**
	 * Return weights from the matrix for a particular Source PID. 
	 *
	 * @param src_pid Index of Source PID for which weights are to be returned.
	 * @param out_intrapid_pct Output parameter; Maximum percentage of peers to select from same PID
	 * @param out_intraisp_pct Output parameter; Maximum percentage of peers to select from intra-ISP PIDs
	 * @param out_weights Output parameter; Vector will be populated with both weights for intra-ISP PIDs and
	 *   external PIDs as follows (where NumIntraISPPIDs and NumTotalPIDs can be determined by ISP::getNumPIDs()):
	 *   - Elements 0 ... NumIntraISPPIDs - 1 are populated with weights to intra-ISP PIDs. These elements sum to 1.
	 *   - Elements NumIntraISPPIDs ... NumTotalPIDs - 1 are populated with weights to external PIDs. These elements also sum to 1.
	 * @returns Returns 0 on success, or ERR_UNKNOWN_PID if PID Index is invalid.
	 *
	 * The following example program shows how weights to intra-ISP PIDs
	 * and external PIDs can be extracted:
	 * \include pgm_object_hello_world.cpp
	 */
	int getWeights(int src_pid,
		       double& out_intrapid_pct, double& out_intraisp_pct,
		       std::vector<double>& out_weights) const;

	/**
	 * Compute guidance weight matrix.  No SwarmState is specified.
	 *
	 * @returns Returns 0 on success, and ERR_INVALID_SWARM_STATE if currently-stored swarm state
	 *   is not consistent with current PID Map (e.g., if it was set from a previous call to compute(SwarmState) ).
	 *
	 * NOTE: This is a blocking call and may not complete immediately.
	 */
	int compute();

	/**
	 * Compute guidance weight matrix, specifying a SwarmState.  The SwarmState is cached and will
	 * be used in future computations.
	 *
	 * @param state SwarmState to be used in computation of the weight matrix.
	 * @returns Returns 0 on success, and ERR_INVALID_SWARM_STATE if swarm state is not consistent with current PID Map.
	 *
	 * NOTE: This is a blocking call and may not complete immediately.
	 */
	int compute(const SwarmState& state);

	/**
	 * Get the time the matrix was last computed.
	 *
	 * @returns Returns time at which matrix was last computed.
	 */
	time_t getLastComputed() const					{ return m_last_computed; }

	/**
	 * Get the time-to-live of the current guidance matrix as provided by the Optimization Engine
	 *
	 * @returns Returns time-to-live of the current guidance matrix. If the Optimization Engine
	 *   did not provide a time-to-live, then 0 is returned.
	 */
	time_t getOptEngineTTL() const					{ return m_optengine_ttl; }

	/**
	 * Indicate the time at which guidance matrix should next be computed.
	 *
	 * @returns Returns the time at which the guidance matrix should next be computed (e.g.,
	 *   by calling the compute() method).  If guidance has not yet been computed, returns 0.
	 */
	time_t getNextComputeTime() const;

	/**
	 * Get the address for the Optimization Engine server (if used)
	 *
	 * @returns Address for the Optimization Engine server.  If one is not being used, this method
	 *   returns an empty string.
	 */
	std::string getOptEngineAddr() const;

	/**
	 * Get the port number for the Optimization Engine server (if used)
	 *
	 * @returns Port number for Optimization Engine server. If one is not being used, this method
	 *   returns 0.
	 */
	unsigned short getOptEnginePort() const;

public:
	/**
	 * Vector of peering percentages for a class of peers (e.g., intra-PID or intra-ISP)
	 */
	typedef std::vector<double> ClassPctVector;

	/**
	 * Row of the weight matrix
	 */
	typedef std::vector<double> WeightRow;

	/**
	 * Weight matrix
	 */
	typedef std::vector<WeightRow> WeightMatrix;

private:
	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	PeeringGuidanceMatrix(const PeeringGuidanceMatrix& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	PeeringGuidanceMatrix& operator=(const PeeringGuidanceMatrix& rhs) { return *this; }

	p4p::protocol::aoe::AOEProtocol* m_proto;	/**< Interface to Optimization Engine server (if used) */
	const ISP* m_isp;				/**< ISP for which we are providing guidance */
	p4p::detail::SharedMutex m_swarm_state_mutex;	/**< Mutex protecting swarm state */
	SwarmState m_swarm_state;			/**< Last Swarm state that was set */
	p4p::detail::SharedMutex m_mutex;		/**< Mutex protecting internal state */
	ClassPctVector m_intrapid_pcts;			/**< Intra-PID peering percentages */
	ClassPctVector m_intraisp_pcts;			/**< Intra-ISP peering percentages */
	WeightMatrix m_weights;				/**< Current weight matrix */
	time_t m_last_computed;				/**< Timestamp of last matrix recomputation */
	time_t m_optengine_ttl;				/**< Time-to-live of the current guidance matrix */
	time_t m_last_pdist_time;			/**< Last update time of pDistance matrix. 0 if not yet computed. */

	PeeringGuidanceMatrixOptions m_options;		/**< Current guidance matrix options */
};

std::ostream& operator<<(std::ostream& os, const app::PeeringGuidanceMatrix::WeightMatrix& rhs);
std::ostream& operator<<(std::ostream& os, const app::PeeringGuidanceMatrix::ClassPctVector& rhs);

};
};

#endif

