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


#ifndef OPT_PLUGIN_MLU_H
#define OPT_PLUGIN_MLU_H

#include <stdexcept>

#include "plugin_base.h"

class MLUPlugin : public OptPluginBase
{
	static const double INIT_PDISTANCE = 0.0;
	static const double INIT_SCALING = 1.0;
	static const double EPS = 1e-12;
	static const double ZERO = 0.0;
	static const double INFINITE = 1000.0;

	/* step size */
	double mu_;

	/* capacity feedback control */
	double alpha_;

	/* scaled edge pdistances */
	SparsePIDMatrix scaled_edge_pdistances_;
	BlockWriteLock scaled_edge_pdistances_lock_;
	
	/* edge pdistance scaling factors */
	SparsePIDMatrix scaling_;
	BlockWriteLock scaling_lock_;

public:
	MLUPlugin(const OptPluginDescriptor* descriptor, double mu = 1.0)
		: OptPluginBase(descriptor),
		  scaled_edge_pdistances_lock_(scaled_edge_pdistances_),
		  scaling_lock_(scaling_)
	{
		mu_ = mu;
		alpha_ = 0.0;
	}

	virtual ~MLUPlugin()
	{
		//TODO: what to do?
	}

	double get_mu()
	{
		return mu_;
	}

	void set_mu(double mu)
	{
		mu_ = mu;
	}

	double get_alpha()
	{
		return alpha_;
	}

	double get_edge_pdistance(const NetState& state, const ReadableLock& state_lock, const NetEdge& e)
	{
		return get_scaled_edge_pdistance(state, state_lock, e) / get_edge_scaling(state, state_lock, e);
	}

	double get_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock, const NetEdge& e)
	{
		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);
		return scaled_edge_pdistances_.get_by_pid(state.get_pid(v_src, state_lock), state.get_pid(v_dst, state_lock), scaled_edge_pdistances_lock_, INIT_PDISTANCE);
	}

	void set_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock, const NetEdge& e, double pdistance)
	{
		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);
		scaled_edge_pdistances_.set_by_pid(state.get_pid(v_src, state_lock), state.get_pid(v_dst, state_lock), pdistance, scaled_edge_pdistances_lock_);
	}

	double get_edge_scaling(const NetState& state, const ReadableLock& state_lock, const NetEdge& e)
	{
		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);
		return scaling_.get_by_pid(state.get_pid(v_src, state_lock), state.get_pid(v_dst, state_lock), scaling_lock_, INIT_SCALING);
	}

	void set_edge_scaling(const NetState& state, const ReadableLock& state_lock, const NetEdge& e, double scaling)
	{
		if (scaling < EPS)
			throw std::invalid_argument("scaling factor must be greater than EPS");

		// keep externally observable edge pdistance unchanged
		set_scaled_edge_pdistance(state, state_lock, e, get_edge_pdistance(state, state_lock, e) * scaling);

		NetVertex v_src, v_dst;
		state.get_edge_vert(e, v_src, v_dst, state_lock);
		scaling_.set_by_pid(state.get_pid(v_src, state_lock), state.get_pid(v_dst, state_lock), scaling, scaling_lock_);
	}

	static double compute_mlu(const NetState& state, const ReadableLock& state_lock);

	void update_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock, double mlu);

	virtual int compute_pdistances(const NetState& state, const ReadableLock& state_lock,
				   const ViewState& view,
				   const PinnedPIDSet& pids);

private:
	void dump_scaled_edge_pdistance(const NetState& state, const ReadableLock& state_lock);
	void dump_edge_utilization(const NetState& state, const ReadableLock& state_lock);
};

class MLUDescriptor : public OptPluginDescriptor
{
public:
	virtual std::string get_name() const
	{ return "MLU"; }

	virtual std::string get_description() const
	{ return "Minimize MLU using supergradient"; }

	virtual OptPluginBasePtr create_instance()
	{ return OptPluginBasePtr(new MLUPlugin(this)); }
};

extern OptPluginDescriptor* OptPluginMLUDescriptor;

#endif
