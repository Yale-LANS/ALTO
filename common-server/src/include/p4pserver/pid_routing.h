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


#ifndef PID_ROUTING_H
#define PID_ROUTING_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <tr1/unordered_map>
#include <p4p/pid.h>
#include <p4pserver/net_state.h>
#include <p4pserver/pid_matrix.h>
#include <p4pserver/local_obj.h>
#include <p4pserver/compiler.h>

class PIDRouting;
typedef boost::shared_ptr<PIDRouting> PIDRoutingPtr;
typedef boost::shared_ptr<const PIDRouting> PIDRoutingConstPtr;

class p4p_common_server_EXPORT PIDRouting : public DistributedObject
{
public:
	/* Typedefs for common routing structures */
	typedef p4p::PIDVector NamedRoute;
	typedef std::pair<p4p::PID, p4p::PID> RouteEndpoints;

	/* Typedefs for static routing configuration (use standard maps for the ones that will be serialized) */
	typedef std::multimap<RouteEndpoints, NamedRoute> StaticRouteMap;
	typedef std::pair<const RouteEndpoints, NamedRoute> StaticRouteMapEntry;
	typedef StaticRouteMap::const_iterator StaticRouteMapItr;
	typedef std::pair<StaticRouteMapItr, StaticRouteMapItr> StaticRouteMapItrPair;
	typedef StaticRouteMap::iterator StaticRouteMapNonConstItr;
	typedef std::pair<StaticRouteMapNonConstItr, StaticRouteMapNonConstItr> StaticRouteMapNonConstItrPair;

	/* Typedefs for results of route computations */
	typedef std::vector<PinnedPID> PinnedRoute;
	typedef std::tr1::unordered_map<PinnedPID, PinnedRoute, boost::hash<p4p::PID> > PinnedRouteMap;

	/* Mode for computing routes */
	enum RouteMode
	{
		RM_STATIC,
		RM_WEIGHTS,
	};

	class RouteComputationContext
	{
	public:
		const PinnedRouteMap& get_result() const { return result_; }
	private:
		friend class PIDRouting;
		PinnedRouteMap result_;
		NetVertexIndexMap vert_indexes_;
		NetEdgeWeightMap edge_weights_;
		NetVertexPredMap vert_preds_;
		PinnedRoute route_reverse_;
		PinnedRoute route_forward_;
	};

	PIDRouting(double default_weight);
	virtual ~PIDRouting();

	RouteMode get_route_mode(const ReadableLock& lock) const;
	void set_route_mode(RouteMode value, const WritableLock& lock);

	/* Get the routes from a single vertex to all other pids
	 * NOTE: This should not be called more than absolutely needed!
	 * NOTE: It is assumed that the vertices in NetState already have valid PIDs assigned to them.
	 */
	bool get_routes(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const;

	StaticRouteMapItrPair get_static_routes(const ReadableLock& lock) const;
	StaticRouteMapNonConstItrPair get_static_routes(const ReadableLock& lock);
	StaticRouteMapItrPair get_static_routes(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const;
	StaticRouteMapNonConstItrPair get_static_routes(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock);
	const NamedRoute* get_static_route(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const;
	unsigned int get_num_static_routes(const ReadableLock& lock) const;

	bool add_static_route(const NamedRoute& value, const WritableLock& lock);
	bool remove_static_route(const NamedRoute& route, const WritableLock& lock);
	bool clear_static_routes(const WritableLock& lock);

	bool set_weight(const p4p::PID&  src, const p4p::PID&  dst, double value, const WritableLock& lock);
	double get_weight(const p4p::PID&  src, const p4p::PID&  dst, const ReadableLock& lock) const;

	void add_pid(const p4p::PID&  pid, const WritableLock& lock);
	void remove_pid(const p4p::PID&  pid, const WritableLock& lock);

protected:
	virtual LocalObjectPtr create_empty_instance() const { return PIDRoutingPtr(new PIDRouting(1.0)); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
#endif

private:
	/* Helpers for handling static routes */
	struct StaticRouteMapItrLess
	{
		bool operator()(const StaticRouteMapItr& lhs, const StaticRouteMapItr& rhs)
		{
			return ((unsigned long)&lhs->first + (unsigned long)&lhs->second)
					< ((unsigned long)&rhs->first + (unsigned long)&rhs->second);
		}
	};
	typedef std::set<StaticRouteMapItr, StaticRouteMapItrLess> StaticRouteMapItrSet;
	typedef std::multimap<p4p::PID, StaticRouteMapItr> PIDStaticRouteMap;

	bool get_routes_static(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const;
	bool get_routes_weights(const PinnedPID&  src, const NetState& state, const ReadableLock& state_lock, const PinnedPIDSet& pids, RouteComputationContext& result, const ReadableLock& lock) const;

	void append_static_route_ptrs(const p4p::PID&  pid, StaticRouteMapItrSet& route_itrs);
	void add_static_route_ptrs(StaticRouteMapItr r_itr);
	void remove_static_route_ptrs(StaticRouteMapItr r_itr);
	void remove_static_route_ptrs(const StaticRouteMapItrSet& route_itrs);

	double default_weight_;

	RouteMode route_mode_;

	StaticRouteMap static_routes_;
	PIDStaticRouteMap static_route_ptrs_;

	SparsePIDMatrixPtr weights_;
	WritableLock* weights_lock_;
};

#endif
