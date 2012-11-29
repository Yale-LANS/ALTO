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


#include "mlu.h"

#include <boost/foreach.hpp>
#include <p4pserver/pid_routing.h>
#include "view.h"

int MLUPlugin::
compute_pdistances(const NetState& state, const ReadableLock& state_lock,
	       const ViewState& view,
	       const PinnedPIDSet& pids)
try {
	//TODO: error check on the value of p, t, c

	// set up scaling factors
	BOOST_FOREACH(const NetEdge& e, state.get_edges(state_lock))
	{
		set_edge_scaling(state, state_lock, e, state.get_capacity(e, state_lock));
	}

	/* update: alpha = max(t_e / c_e) */
	alpha_ = compute_mlu(state, state_lock);
	get_logger()->info("current MLU: %f", alpha_);

	if (get_logger()->isDebugEnabled())
	{
		get_logger()->debug("Before super-gradient adjustment:");
		dump_edge_utilization(state, state_lock);
		dump_scaled_edge_pdistance(state, state_lock);
	}

	// update scaled edge pdistances
	get_logger()->info("adjusting scaled edge pdistances via supergradient");
	update_scaled_edge_pdistance(state, state_lock, alpha_);

	if (get_logger()->isDebugEnabled())
	{
		get_logger()->debug("After super-gradient adjustment:");
		dump_scaled_edge_pdistance(state, state_lock);
	}

	get_logger()->info("finished successfully");
	return 0;
} catch (...) {
	return -1;
}

OptPluginDescriptor* OptPluginMLUDescriptor = new MLUDescriptor();

double MLUPlugin::
compute_mlu(const NetState& state, const ReadableLock& state_lock)
{
	NetEdgeItrPair edges = state.get_edges(state_lock);

	double mlu = 0.0;
	for (NetEdgeItr e_itr = edges.first; e_itr != edges.second; ++e_itr)
	{
		double t_e = state.get_traffic(*e_itr, state_lock);
		double c_e = state.get_capacity(*e_itr, state_lock);
		double LU_e = t_e / c_e;

		if (mlu < LU_e) mlu = LU_e;
	}

	return mlu;
}

void MLUPlugin::
update_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock, double mlu)
{
	NetEdgeItrPair edges = state.get_edges(state_lock);

	/**
	 * Super-gradient: p_e = p_e + mu * (t_e / c_e - mlu).
	 */
	double sum_p_e = 0.0;
	for (NetEdgeItr e_itr = edges.first; e_itr != edges.second; ++e_itr)
	{
		double p_e = get_scaled_edge_pdistance(state, state_lock, *e_itr);
		double t_e = state.get_traffic(*e_itr, state_lock);
		double c_e = state.get_capacity(*e_itr, state_lock);

		p_e += mu_ * (t_e / c_e - mlu);
		if (p_e < 0) {
			p_e = 0;
		}

		set_scaled_edge_pdistance(state, state_lock, *e_itr, p_e);
		sum_p_e += p_e;
	}

	/**
	 * Projection: p_e = p_e / sum(p_e).
	 */
	int num_edges = state.get_num_edges(state_lock);
	for (NetEdgeItr e_itr = edges.first; e_itr != edges.second; ++e_itr)
	{
		double p_e = get_scaled_edge_pdistance(state, state_lock, *e_itr);

		if (sum_p_e > EPS) {
			p_e /= sum_p_e;
		} else {
			/**
			 * NOTE: sum_p_e <= EPS indicates that all edge pdistances
			 * are close to 0, and we must re-calibrate the pdistances.
			 */
			p_e = 1.0 / num_edges;
		}

		set_scaled_edge_pdistance(state, state_lock, *e_itr, p_e);
	}

	/**
	 * Update set of unscaled edge pdistances
	 */
	SparsePIDMatrix* result_pdistances = get_edge_pdistances();
	BlockWriteLock result_pdistances_lock(*result_pdistances);
	BOOST_FOREACH(const NetEdge& e, state.get_edges(state_lock))
	{
		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);
		const p4p::PID&  l_src = state.get_pid(v_src, state_lock);
		const p4p::PID&  l_dst = state.get_pid(v_dst, state_lock);
		result_pdistances->set_by_pid(l_src, l_dst,
					  scaled_edge_pdistances_.get_by_pid(l_src, l_dst, scaled_edge_pdistances_lock_)
					  / scaling_.get_by_pid(l_src, l_dst,scaling_lock_),
					  result_pdistances_lock);
	}
}

void MLUPlugin::
dump_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock)
{
	BOOST_FOREACH(const NetEdge& e, state.get_edges(state_lock))
	{
		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);

		get_logger()->debug("scaled pdistance for edge %s->%s: %.12lf",
				    state.get_name(v_src, state_lock).c_str(),
				    state.get_name(v_dst, state_lock).c_str(),
				    get_scaled_edge_pdistance(state, state_lock, e));
	}
}

void MLUPlugin::
dump_edge_utilization(const NetState& state, const ReadableLock& state_lock)
{
	BOOST_FOREACH(const NetEdge& e, state.get_edges(state_lock))
	{
		NetVertex from, to;
		state.get_edge_vert(e, from, to, state_lock);

		get_logger()->debug("Edge '%s' -> '%s' utilization: %f",
				    state.get_name(from, state_lock).c_str(),
				    state.get_name(to, state_lock).c_str(),
				    state.get_traffic(e, state_lock)
				    / state.get_capacity(e, state_lock));
	}
}
