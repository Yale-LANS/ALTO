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


#include "p4pserver/pid_routing.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/foreach.hpp>
#include "p4pserver/pid_matrix.h"

PIDRouting::PIDRouting(double default_weight)
	: default_weight_(default_weight),
	  route_mode_(RM_WEIGHTS),
	  weights_(SparsePIDMatrixPtr(new SparsePIDMatrix())),
	  weights_lock_(new BlockWriteLock(*weights_))
{
	after_construct();
}

PIDRouting::~PIDRouting()
{
	before_destruct();
	delete weights_lock_;
}

#ifdef P4P_CLUSTER
void PIDRouting::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_read(get_local_mutex());

	stream >> default_weight_;
	stream >> route_mode_;
	if (route_mode_ == RM_STATIC)
	{
		stream >> static_routes_;
		static_route_ptrs_.clear();
		for (StaticRouteMap::iterator r_itr = static_routes_.begin(); r_itr != static_routes_.end(); ++r_itr)
			add_static_route_ptrs(r_itr);
	}
	weights_->do_load(stream, *weights_lock_);

	changed(lock);
}

void PIDRouting::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	stream << default_weight_;
	stream << route_mode_;
	if (route_mode_ == RM_STATIC)
		stream << static_routes_;
	weights_->do_save(stream, *weights_lock_);
}
#endif

DistributedObjectPtr PIDRouting::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());

	PIDRoutingPtr new_obj = boost::dynamic_pointer_cast<PIDRouting>(PIDRouting::create_empty_instance());
	new_obj->default_weight_ = default_weight_;
	new_obj->route_mode_ = route_mode_;
	if (route_mode_ == RM_STATIC)
	{
		new_obj->static_routes_ = static_routes_;
		for (StaticRouteMap::iterator r_itr = new_obj->static_routes_.begin(); r_itr != new_obj->static_routes_.end(); ++r_itr)
			add_static_route_ptrs(r_itr);
	}
	delete new_obj->weights_lock_;
	new_obj->weights_ = boost::dynamic_pointer_cast<SparsePIDMatrix>(weights_->copy(*weights_lock_));
	new_obj->weights_lock_ = new BlockWriteLock(*new_obj->weights_);
	return new_obj;
}

PIDRouting::StaticRouteMapItrPair PIDRouting::get_static_routes(const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return StaticRouteMapItrPair(static_routes_.begin(), static_routes_.end());
}

PIDRouting::StaticRouteMapNonConstItrPair PIDRouting::get_static_routes(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());
	return StaticRouteMapNonConstItrPair(static_routes_.begin(), static_routes_.end());
}

PIDRouting::StaticRouteMapItrPair PIDRouting::get_static_routes(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return static_routes_.equal_range(RouteEndpoints(src, dst));
}

PIDRouting::StaticRouteMapNonConstItrPair PIDRouting::get_static_routes(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());
	return static_routes_.equal_range(RouteEndpoints(src, dst));
}

const PIDRouting::NamedRoute* PIDRouting::get_static_route(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	StaticRouteMap::const_iterator itr = static_routes_.find(RouteEndpoints(src, dst));
	if (itr == static_routes_.end())
		return NULL;
	return &itr->second;
}

unsigned int PIDRouting::get_num_static_routes(const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return static_routes_.size();
}

bool PIDRouting::set_weight(const p4p::PID&  src, const p4p::PID&  dst, double value, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	bool res = weights_->set_by_pid(src, dst, value, *weights_lock_);

	if (res)
		changed(lock);

	return res;
}

double PIDRouting::get_weight(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return weights_->get_by_pid(src, dst, *weights_lock_, default_weight_);
}

bool PIDRouting::get_routes_static(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	result.result_.clear();

	BOOST_FOREACH(const PinnedPID&  dst, pids)
	{
		/*
		* Currently we silently ignore the case where a route doesn't exist
		*/
		const NamedRoute* route = get_static_route(src, dst, lock);
		if (!route)
			continue;

		result.route_forward_.clear();
		BOOST_FOREACH(const p4p::PID&  hop, *route)
		{
			const PinnedPIDSet::const_iterator itr = pids.find(PinnedPID(hop, NULL));
			if (itr == pids.end())
				return false;

			result.route_forward_.push_back(*itr);
		}

		result.result_[dst] = result.route_forward_;
	}
	return true;
}

bool PIDRouting::get_routes_weights(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	/*
	* If the context is empty, fill it in
	*/
	if (result.vert_indexes_.empty())
	{
		/*
		 * Assign each vertex an index
		 */
		unsigned int v_idx = 0;
		BOOST_FOREACH(const NetVertex& v, state.get_nodes(state_lock))
		{
			result.vert_indexes_[v] = v_idx++;
		}

		/*
		 * Construct a map of edge weights
		 */
		BOOST_FOREACH(const NetEdge& e, state.get_edges(state_lock))
		{
			/*
			 * Find the source and destination pids.  If either doesn't exist,
			 * then assign the edge the default weight.
			 */
			NetVertex v_src, v_dst;
			state.get_edge_vert(e, v_src, v_dst, state_lock);
			result.edge_weights_[e] = get_weight(state.get_pid(v_src, state_lock), state.get_pid(v_dst, state_lock), lock);
		}

		result.route_reverse_.reserve(state.get_num_nodes(state_lock) / 2);
		result.route_forward_.reserve(state.get_num_nodes(state_lock) / 2);
	}

	result.result_.clear();

	/*
	 * Compute the routes
	 */
	state.compute_shortest_paths(src.get_vertex(), result.vert_indexes_, result.vert_preds_, result.edge_weights_, state_lock);

	BOOST_FOREACH(const PinnedPID&  pid, pids)
	{
		const NetVertex& v_dst = pid.get_vertex();

		NetVertex v = result.vert_preds_[v_dst];

		/* Ignore if no path exists, or this is the source node */
		if (v == v_dst) continue;

		result.route_reverse_.clear();
		result.route_forward_.clear();

		/* Handle the destination node */
		result.route_reverse_.push_back(PinnedPID(pid, v_dst));

		/* Predecessor to the destination node */
		result.route_reverse_.push_back(PinnedPID(state.get_pid(v, state_lock), v));

		/* Trace back to the source node */
		while (v != src.get_vertex())
		{
			v = result.vert_preds_[v];
			result.route_reverse_.push_back(PinnedPID(state.get_pid(v, state_lock), v));
		}

		/* Reverse the route so edges appear in the forward direction */
		std::copy(result.route_reverse_.rbegin(), result.route_reverse_.rend(), std::back_inserter(result.route_forward_));

		/* Add route to our data structures */
		result.result_[pid] = result.route_forward_;

		result.route_reverse_.clear();
		result.route_forward_.clear();
	}
	return true;
}

bool PIDRouting::get_routes(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	switch (route_mode_)
	{
		case RM_STATIC:  return get_routes_static(src, state, state_lock, pids, result, lock);
		case RM_WEIGHTS: return get_routes_weights(src, state, state_lock, pids, result, lock);
		default:         throw std::runtime_error("Illegal state: invalid route mode");
	}
}

bool PIDRouting::add_static_route(const NamedRoute& value, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Invalid when routes are automatically computed */
	if (route_mode_ != RM_STATIC)
		return false;

	if (value.size() < 2)
		return false;

	add_static_route_ptrs(static_routes_.insert(std::make_pair(RouteEndpoints(value.front(), value.back()), value)));

	changed(lock);
	return true;
}

bool PIDRouting::remove_static_route(const NamedRoute& route, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	if (route.size() < 2)
		return false;

	/* look at the routes between these endpoints */
	StaticRouteMapNonConstItrPair routes = get_static_routes(route.front(), route.back(), lock);
	for (StaticRouteMapNonConstItr r_itr = routes.first; r_itr != routes.second; ++r_itr)
	{
		/* check if this route matches */
		if (r_itr->second.size() != route.size() || !std::equal(route.begin(), route.end(), r_itr->second.begin()))
			continue;

		remove_static_route_ptrs(r_itr);
		static_routes_.erase(r_itr);

		changed(lock);
		return true;
	}
	return false;
}

bool PIDRouting::clear_static_routes(const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Invalid when routes are automatically computed */
	if (route_mode_ != RM_STATIC)
		return false;

	static_routes_.clear();
	static_route_ptrs_.clear();
	changed(lock);
	return true;
}

PIDRouting::RouteMode PIDRouting::get_route_mode(const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return route_mode_;
}

void PIDRouting::set_route_mode(RouteMode value, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Don't update if our mode is already set */
	if (route_mode_ == value)
		return;

	/* Update the mode */
	route_mode_ = value;

	/* initialize routes for the new mode */
	clear_static_routes(lock);
	switch (route_mode_)
	{
		case RM_STATIC:
			/* routes must be manually configured afterwards */
			break;
		case RM_WEIGHTS:
			/* compute routes using the weights */
			break;
		default:
			throw std::runtime_error("Invalid routing mode!");
	}

	changed(lock);
}

void PIDRouting::append_static_route_ptrs(const p4p::PID&  pid, StaticRouteMapItrSet& route_itrs)
{
	if (route_mode_ != RM_STATIC)
		return;

	std::pair<PIDStaticRouteMap::iterator, PIDStaticRouteMap::iterator> itrs = static_route_ptrs_.equal_range(pid);
	for (PIDStaticRouteMap::iterator itr = itrs.first; itr != itrs.second; ++itr)
		route_itrs.insert(itr->second);
}

void PIDRouting::add_static_route_ptrs(StaticRouteMapItr r_itr)
{
	if (route_mode_ != RM_STATIC)
		return;

	BOOST_FOREACH(const p4p::PID&  pid, r_itr->second)
	{
		static_route_ptrs_.insert(std::make_pair(pid, r_itr));
	}
}

void PIDRouting::remove_static_route_ptrs(StaticRouteMapItr r_itr)
{
	if (route_mode_ != RM_STATIC)
		return;

	BOOST_FOREACH(const p4p::PID&  pid, r_itr->second)
	{
		std::vector<PIDStaticRouteMap::iterator> delentries;

		std::pair<PIDStaticRouteMap::iterator, PIDStaticRouteMap::iterator> itrs = static_route_ptrs_.equal_range(pid);
		for (PIDStaticRouteMap::iterator itr = itrs.first; itr != itrs.second; ++itr)
			if (itr->second == r_itr)
				delentries.push_back(itr);

		for (unsigned int i = 0; i < delentries.size(); ++i)
			static_route_ptrs_.erase(delentries[i]);
	}
}

void PIDRouting::remove_static_route_ptrs(const StaticRouteMapItrSet& route_itrs)
{
	if (route_mode_ != RM_STATIC)
		return;

	BOOST_FOREACH(const StaticRouteMapItr& i, route_itrs)
	{
		remove_static_route_ptrs(i);
	}
}

void PIDRouting::add_pid(const p4p::PID&  pid, const WritableLock& lock)
{
	switch (route_mode_)
	{
	case RM_STATIC:
	{
		break;
	}
	case RM_WEIGHTS:
	{
		weights_->add_pid(pid, *weights_lock_);
		break;
	}
	default: throw std::runtime_error("Invalid routing mode!");
	}

	changed(lock);
}

void PIDRouting::remove_pid(const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	switch (route_mode_)
	{
	case RM_STATIC:
	{
		StaticRouteMapItrSet invalidated_static_routes;
		append_static_route_ptrs(pid, invalidated_static_routes);
		remove_static_route_ptrs(invalidated_static_routes);
		break;
	}
	case RM_WEIGHTS:
	{
		weights_->remove_pid(pid, *weights_lock_);
		break;
	}
	default: throw std::runtime_error("Invalid routing mode!");
	}

	changed(lock);
}
