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


#ifndef P4P_PEERING_GUIDANCE_MATRIX_OPTS_H
#define P4P_PEERING_GUIDANCE_MATRIX_OPTS_H

#include <vector>
#include <p4p/p4p.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Simple container class for peering guidance matrix options.
/**
 * Simple container class for peering guidance matrix options, which control
 * how the weight matrix is computed.  Options include specifying the type
 * of application (e.g., filesharing, streaming, etc), and options controlling
 * if and how current swarm state is incorporated.
 */
class p4p_common_cpp_EXPORT PeeringGuidanceMatrixOptions
{
public:
	/**
	 * Constructor that initializes with default options.
	 */
	PeeringGuidanceMatrixOptions();

	/**
	 * Enumeration for available optimization types
	 */
	enum OptType
	{
		OPT_LOCATION_ONLY,		/**< Guidance using nework location information only */
		OPT_FILESHARING_MATCHING,	/**< Optimization for filesharing applications based on bandwidth matching */
		OPT_STREAMING_SSM,		/**< Optimization for streaming applications using swarm-state matching */
		OPT_STREAMING_SSMCM,		/**< Optimization for streaming applications using swarm-state matching and connection matching */
	};

	/**
	 * Set the optimization type.  The peering guidance matrix will be computed using
	 * the configured optimization method.  Different optimization methods are useful
	 * for different applications, such as filesharing or streaming.  See OptType for
	 * details about the available optimization types.
	 *
	 * Default Value: OPT_FILESHARING_MATCHING
	 */
	void setOptType(OptType value)			{ m_opttype = value; }

	/**
	 * Get the current optimization type.  See setOptType() for details.
	 *
	 * @returns Current optimization type.
	 */
	OptType getOptType() const			{ return m_opttype; }

	/**
	 * Configure whether current swarm statistics will be used to tune
	 * the bandwidth-matching and interdomain selection algorithms
	 * that computes the peering guidance matrix.  This option is disabled
	 * (set to false) by default.
	 *
	 * Default Value: false (disabled)
	 *
	 * Enabling option requires that a separate peering guidance matrix be maintained
	 * for each swarm, instead of for each ISP.
	 *
	 * @param value True if option should be enabled, and false otherwise.
	 */
	void setMatchingUsesSwarmState(bool value)	{ m_matching_uses_swarm_state = value; }

	/**
	 * Return status indicating whether current swarm state is used to tune
	 * the computation of the peering guidance matrix. See setMatchingUsesSwarmState()
	 * for details.
	 *
	 * @returns Returns true if computation uses current swarm state, and false otherwise.
	 */
	bool getMatchingUsesSwarmState() const		{ return m_matching_uses_swarm_state; }

	/**
	 * Configure whether overload protection is enabled.  If overload
	 * protection factor is set to a positive (non-zero) value, then
	 * the computed peering guidance matrix will be post-processed to
	 * such that PIDs with few (or low-capacity peers) are not assigned
	 * high weights.
	 *
	 * Default Value: 0 (disabled)
	 *
	 * Enabling option requires that a separate peering guidance matrix be maintained
	 * for an ISP for each swarm, instead of a sharing a guidance matrix for an ISP
	 * across multiple swarms.
	 *
	 * @param value If 0 (or negative), then overload protection is disabled. If
	 *   set to a number >= 1.0 then the weight assigned to a single PID p will
	 *   be upper bounded by factor * ( number of peers in PID p ) / ( number of total peers ).
	 *   Behavior is undefined if the value is between 0 and 1.0.
	 */
	void setOverloadProtectionFactor(float value)	{ m_overload_protection_factor = value; } 

	/**
	 * Return status indicating the current overload protection factor.  See
	 * setOverloadProtectionFactor() for more information.
	 */
	float getOverloadProtectionFactor() const	{ return m_overload_protection_factor; }

	/**
	 * Configure whether the time-to-live value (if available) provided for a guidance
	 * matrix should be used to determine when the guidance matrix is recomputed.  Note
	 * that this applies to both swarm-dependent and swarm-independent guidance matrices.
	 *
	 * Default Value: true (enabled)
	 *
	 * @param value If true, then the time-to-live value (if available) should be used
	 *   to determine the the interval at which the guidance matrix is recomputed. If false,
	 *   any time-to-live value provided by the Optimization Engine address should be ignored.
	 */
	void setUseOptEngineTTL(bool value)		{ m_use_optengine_ttl = value; }

	/**
	 * Return status indicating whether the engine-provided time-to-live value (if available)
	 * is used to determine when the guidance matrix is recomputed.  See setUseEngineTTL() for
	 * more information.
	 */
	bool getUseOptEngineTTL() const			{ return m_use_optengine_ttl; }

	/**
	 * Configure the interval (in seconds) at which a swarm-dependent guidance matrix
	 * should be recomputed.  Note that this option does not have any effect on guidance
	 * matrices that are not swarm-dependent.
	 *
	 * Default Value: 300 (5 minutes)
	 *
	 * @param value If 0, then the guidance matrix should not be automatically recomputed. If
	 *   non-zero, the swarm state should be updated and the guidance matrix recomputed
	 *   at the desired interval.
	 */
	void setRecomputeInterval(time_t value)		{ m_recompute_interval = value; }

	/**
	 * Return the configured guidance matrix recomputation interval.  See setRecomputeInterval()
	 * for more information.
	 */
	time_t getRecomputeInterval() const		{ return m_recompute_interval; }

	/**
	 * Configure the channel streaming rate (Byte per second).  Used in streaming application context.
	 *
	 * Default Value: 48000 (~380kbps)
	 *
	 * @param value Streaming (live audio, video) channel rate (Byte per second).
	 * 
	 */
	void setStreamingChannelRate(double value)	{ m_streamingdependent_channelrate = value; }

	/**
	 * Return configured channel streaming rate (Byte per second).
	 * Only meaningful to streaming application optimization types.
	 */
	double getStreamingChannelRate() const		{ return m_streamingdependent_channelrate; }

	/**
	 * Configure default intra-PID peer selection percentage and intra-ISP percentage.  
	 * Only used by location-only optimization type.
	 * 
	 * @param intrapid_pct Default intra-PID upper bound percentage for peer selection,
	 *   default value is 0.75 (75%)
	 * @param intraisp_pct Default intra-ISP upper bound percentage for peer selection,
	 *   default value is 0.9 (90%)
	 */
	void setLocationSelectionPercentage(const double intrapid_pct, const double intraisp_pct);

	/**
	 * Return configured intra-PID upper bound percentage for peer selection 
	 */
	double getDefaultIntraPIDPercentage() const	{ return m_intrapid_pct; }

	/**
	 * Return configured intra-ISP upper bound percentage for peer selection 
	 */
	double getDefaultIntraISPPercentage() const	{ return m_intraisp_pct; }

	/**
	 * Indicates whether the configured options require the use
	 * of a swarm state when computing the guidance matrix. Using
	 * the swarm state makes the matrix swarm-dependent, and thus
	 * is not shared across multiple swarms.
	 *
	 * @returns Returns true if current options indicate the peering guidance
	 *   matrix is swarm-dependent, and false otherwise.
	 */
	bool isSwarmDependent() const;

	/**
	 * Overload equality operator to determine if a pair of option sets are equivalent
	 *
	 * @param rhs Right-hand side of equality expression.
	 * @returns Returns true if sets of options are equivalent, false otherwise.
	 */
	bool operator==(const PeeringGuidanceMatrixOptions& rhs) const;

	/**
	 * Overload inequality operator to determine of a pair of option sets are not equivalent
	 * 
	 * @param rhs Right-hand side of inequality expression.
	 * @returns Returns true if sets of options are not equivalent, false otherwise.
	 */
	bool operator!=(const PeeringGuidanceMatrixOptions& rhs) const;

public:
	/* Factory methods for common pre-defined sets of options */

	/**
	 * Get pre-defined set of options for location-only (swarm-independent) guidance 
	 * in an application.  Only network location information is used to generate 
	 * this guidance.  Default intra-PID peer selection percentage and intra-ISP 
	 * percentage should be specified.
	 *
	 * @param intrapid_pct Intra-PID upper bound percentage for peer selection,
	 *   default value is 0.75 (75%)
	 * @param intraisp_pct Intra-ISP upper bound percentage for peer selection,
	 *   default value is 0.9 (90%)
	 * @returns Returns set of options
	 */
	static PeeringGuidanceMatrixOptions LocationOnly(const double intrapid_pct = 0.75, const double intraisp_pct = 0.9);

	/**
	 * Get pre-defined set of options for generic (swarm-independent) guidance in a
	 * filesharing application. Swarm-independent bandwidth matching is used.
	 *
	 * @returns Returns set of options
	 */
	static PeeringGuidanceMatrixOptions FilesharingGeneric();

	/**
	 * Get pre-defined set of options for swarm-dependent guidance in a filesharing application.
	 * Both swarm-dependent bandwidth matching and overload protection are enabled.
	 *
	 * @returns Returns set of options
	 */
	static PeeringGuidanceMatrixOptions FilesharingSwarmDependent();

	/**
	 * Get pre-defined set of options for swarm-dependent guidance in a streaming application.
	 * The streaming application can specify the channel(swarm) streaming rate in Byte per second.
	 * It can also specify customized optimization type (e.g., OPT_STREAMING_SSM or OPT_STREAMING_SSMCM).
	 * Both swarm-dependent bandwidth matching and overload protection are enabled.
	 *
	 * @param channel_rate Channel streaming rate, by default 48000 Byte per second.
	 * @param opt_type Optimization type, can be set to OPT_STREAMING_SSMCM or by default OPT_STREAMING_SSM.
	 * @returns Returns set of options
	 */
	static PeeringGuidanceMatrixOptions StreamingSwarmDependent(double channel_rate = 48000, OptType opt_type = OPT_STREAMING_SSM);

public:
	/* Additional methods for advanced usage. */

	/**
	 * Overload less-than operator so option sets can be part of keys for STL collections (e.g., maps). This
	 * facilitates sharing of generic matrices across swarms.
	 */
	bool operator<(const PeeringGuidanceMatrixOptions& rhs) const;

	/**
	 * Overload greater-than operator so option sets can be part of keys for STL collections (e.g., maps). This
	 * facilitates sharing of generic matrices across swarms.
	 */
	bool operator>(const PeeringGuidanceMatrixOptions& rhs) const;

private:
	OptType m_opttype;				/**< Optimization type */
	bool m_matching_uses_swarm_state;		/**< Indicates whether matching operation uses swarm state */
	float m_overload_protection_factor;		/**< Configured overload protection factor */
	bool m_use_optengine_ttl;			/**< Indicates whether Optimization Engine TTL should be used */
	time_t m_recompute_interval;			/**< Interval (in seconds) at which swarm-dependent guidance should be recomputed */
	double m_streamingdependent_channelrate;	/**< Channel video/audio rate (Byte per second) a streaming appliction should specify */
	double m_intrapid_pct;				/**< Intra-PID peer selection percentage for location-only guidance */
	double m_intraisp_pct;				/**< Intra-ISP peer selection percentage for location-only guidance */
};

std::ostream& operator<<(std::ostream& os, const PeeringGuidanceMatrixOptions& rhs);

};
};

#endif

