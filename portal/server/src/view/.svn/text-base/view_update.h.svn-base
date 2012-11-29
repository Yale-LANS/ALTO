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


#ifndef VIEW_UPDATE_H
#define VIEW_UPDATE_H

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <boost/thread/mutex.hpp>
#include <map>
#include <string>
#include <p4pserver/locking.h>
#include <p4pserver/logging.h>
#include "view.h"
#include "global_state.h"
#include "plugin_base.h"

typedef ViewWrapper<
		const ReadableLock, const UpgradableReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock
	> ViewUpdateStateUpgrade;

typedef ViewWrapper<
		const WritableLock, const BlockWriteLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock
	> ViewUpdateStateDirect;

template <class ViewUpdateState>
class ViewUpdateBase
{
public:
	virtual unsigned int do_update() = 0;

protected:
	ViewUpdateBase(NetStatePtr net_state, const ReadableLock& net_state_lock, ViewUpdateState& view_state)
		: logger_(log4cpp::Category::getInstance("ViewUpdate(" + view_state.get()->get_name(view_state.get_view_lock()) + ")")),
		  net_state_orig_(net_state),
		  net_state_orig_lock_(net_state_lock),
		  view_state_(view_state),
		  last_temp_pid_(0),
		  result_intradomain_pdistances_(PIDMatrixPtr(new PIDMatrix())),
		  result_intradomain_pdistances_lock_(*result_intradomain_pdistances_),
		  result_interdomain_pdistances_(SparsePIDMatrixPtr(new SparsePIDMatrix())),
		  result_interdomain_pdistances_lock_(*result_interdomain_pdistances_),
		  result_update_interval_(0)
	{}

	virtual ~ViewUpdateBase() {}

	bool compute_result()
	{
		result_update_interval_ = view_state_.get()->get_update_interval(view_state_.get_view_lock());

		/* Copy network state; we'll make some local modifications here prior to
		 * performing the optimizations */
		{
			get_logger().info("copying current network state");
			net_state_ = boost::dynamic_pointer_cast<NetState>(net_state_orig_->copy(net_state_orig_lock_));
		}
		net_state_lock_ = boost::shared_ptr<WritableLock>(new BlockWriteLock(*net_state_));
	
		p4p::PIDSet pids_internal;
		p4p::PIDSet pids_external;
		p4p::PIDSet pids_all;
		PinnedPIDSet pids;
		{
			get_logger().debug("parsing pids");
			PinnedPIDSet unagg_pids;
			find_pids(pids_internal, pids_external, pids_all, unagg_pids);
	
			get_logger().debug("total pids: %u ", unagg_pids.size());
			get_logger().debug("internal pids: %u", pids_internal.size());
			get_logger().debug("external pids: %u", pids_external.size());
	
			get_logger().info("aggregating topology");
			if (!aggregate_topology(unagg_pids, pids))
			{
				get_logger().error("aggregation failed; cancelling job");
				return false;
			}
	
			get_logger().debug("total pids after aggregation: %u", pids.size());
		}
	
		get_logger().info("handling unaggregated nodes");
		handle_extra_nodes(pids);
	
		get_logger().debug("total pids after handling unaggregated nodes: %u", pids.size());
	
		get_logger().info("verifying static routing (if applicable)");
		if (!verify_static_routing(pids))
		{
			get_logger().error("static route verification failed; cancelling job");
			return false;
		}
	
		/* Kick off the optimization */
		int rc = 0;
		OptPluginBasePtr plugin = view_state_.get()->get_plugin(view_state_.get_view_lock());
		if (plugin)
		{
			get_logger().info("running optimization");
			rc = plugin->compute_pdistances(*net_state_, *net_state_lock_, view_state_, pids);
		}

		/* Fill in the intradomain pdistances */
		get_logger().info("initializing intradomain pdistance matrix");
		result_intradomain_pdistances_->set_pids(pids_internal, result_intradomain_pdistances_lock_);
	
		/* Fill in the interdomain pdistances */
		get_logger().info("initializing interdomain pdistance matrix");
		result_interdomain_pdistances_->set_pids(pids_all, result_interdomain_pdistances_lock_);
	
		get_logger().info("updating peering pdistances");
		if (!compute_pdistances(plugin, pids, pids_external,
				*result_intradomain_pdistances_, result_intradomain_pdistances_lock_,
				*result_interdomain_pdistances_, result_interdomain_pdistances_lock_))
		{
			get_logger().error("peering pdistance computation failed; cancelling job");
			return false;
		}
	
		/* Update the pdistances matrix for the view if it was successful */
		if (rc == 0)
			get_logger().info("view optimization completed successfully");
		else
			get_logger().warn("view optimization returned error '%d'", rc);

		/* FIXME: Allow optimization plugins to override TTL */

		return true;
	}

	ViewUpdateState& get_view_state() { return view_state_; }

protected:
	log4cpp::Category& get_logger() const { return logger_; }

private:
	const SparsePIDMatrix EMPTY_EDGE_COSTS;

	bool aggregate_topology(const PinnedPIDSet& unagg_pids, PinnedPIDSet& agg_pids) const
	{
		agg_pids.clear();
	
		/* Iterate over each pid,and create a new vertex for each */
		BOOST_FOREACH(const PinnedPID& pid, unagg_pids)
		{
			/* Add new vertex to the topology */
			NetVertex v;
			std::string v_name = boost::lexical_cast<std::string>(pid);
			if (!net_state_->aggregate_node(pid.get_vertices(), v_name, v, *net_state_lock_))
			{
				get_logger().error("failed to create node '%s' when aggregating topology, "
						   "probably because a node with the same name already exists", v_name.c_str());
				return false;
			}
	
			/* The new node is external as long as the pid is external */
			net_state_->set_external(v, pid.get_external(), *net_state_lock_);
	
			/* Assocate the new node to the appropriate pid */
			net_state_->set_pid(v, pid, *net_state_lock_);
	
			/* Update vertices in the pid map */
			agg_pids.insert(PinnedPID(pid, v));
		}
	
		return true;
	}

	void find_pids(p4p::PIDSet& pids_internal, p4p::PIDSet& pids_external, p4p::PIDSet& pids_all, PinnedPIDSet& pids) const
	{
		/* Clear the output parameters */
		pids_internal.clear();
		pids_external.clear();
		pids_all.clear();
		pids.clear();
	
		/* Iterate through all the pids defined for the view:
		 * - Separate pids into internal/external, and internal only
		 * - Convert vertex names into vertex descriptors to speed up referencing them later
		 */
		BOOST_FOREACH(const PIDAggregation::PIDRecord& view_loc, view_state_.get_pid_map())
		{
			NetVertexSet pid_verts;
	
			/* Determine if this is an internal or external pid.  It is internal
			 * only if all vertices are internal.
			 *
			 * As we're iterating through, we'll also keep track of the vertex 
			 * descriptors we find.
			 */
			bool is_internal = true;
			BOOST_FOREACH(const NetVertexName& v_name, view_loc.vertices)
			{
				NetVertex v;
				if (!net_state_->get_node(v_name, v, *net_state_lock_))
				{
					std::string pid_str = boost::lexical_cast<std::string>(view_loc.pid);
					get_logger().warn("omitting node %s for pid %s since it doesn't exist in topology",
						v_name.c_str(), pid_str.c_str());
					continue;
				}
	
				pid_verts.insert(v);
				if (net_state_->get_external(v, *net_state_lock_))
					is_internal = false;
			}
	
			p4p::PID pid(view_loc.pid.get_isp(), view_loc.pid.get_num(), !is_internal);
	
			/* Add to our lists of internal and external pids */
			if (pid.get_external())
				pids_external.insert(pid);
			else
				pids_internal.insert(pid);
			pids_all.insert(pid);
	
			/* Update the pid assocated with each of the vertices */
			BOOST_FOREACH(const NetVertex& v, pid_verts)
			{
				net_state_->set_pid(v, pid, *net_state_lock_);
			}
	
			/* Store the vertex descriptors found for this pid.  These do NOT
			 * get stored in the above vectors since we don't need the storage
			 * overhead for the vertex sets there.
			 */
			pids.insert(PinnedPID(pid, pid_verts));
		}
	}

	bool verify_static_routing(const PinnedPIDSet& pids) const
	{
		const PIDRouting& routing = *view_state_.get()->get_intradomain_routing(view_state_.get_view_lock());
		const ReadableLock& routing_lock = view_state_.get_intradomain_routing_lock();
	
		/* Ensure that all of the routes for the view are valid by matching against
		 * the physical topology.
		 */
		BOOST_FOREACH(const PIDRouting::StaticRouteMapEntry& route, routing.get_static_routes(routing_lock))
		{
			const NetVertex* prev_hop = NULL;
			BOOST_FOREACH(const p4p::PID&  hop, route.second)
			{
				/* Get the vertex corresponding to the current hop */
				PinnedPIDSet::const_iterator itr = pids.find(PinnedPID(hop, NULL));
				if (itr == pids.end())
					return false;
	
				if (itr->get_vertices().size() != 1)
					throw std::runtime_error("Illegal state: must be exactly one vertex per pid");
	
				const NetVertex* cur_hop = &itr->get_vertex();
	
				NetEdge e;
				if (prev_hop != NULL && !net_state_->get_edge(*cur_hop, *prev_hop, e, *net_state_lock_))
					return false;
	
				prev_hop = cur_hop;
			}
		}
	
		return true;
	}

	void handle_extra_nodes(PinnedPIDSet& pids)
	{
		ExtraNodeAction action = view_state_.get()->get_extra_node_action(view_state_.get_view_lock());

		/* Make a set of all the vertices that we've identified as being part of a pid so far. */
		NetVertexSet agg_verts;
		BOOST_FOREACH(const PinnedPID& pid, pids)
		{
			const NetVertexSet& pid_verts = pid.get_vertices();
			std::copy(pid_verts.begin(), pid_verts.end(), std::inserter(agg_verts, agg_verts.end()));
		}
	
		/* Find the vertices in the network topology that are not part of a PID yet */
		NetVertexVector remove_verts; /* vertices that are to be removed */
		remove_verts.reserve(net_state_->get_num_nodes(*net_state_lock_) - agg_verts.size());
		BOOST_FOREACH(const NetVertex& v, net_state_->get_nodes(*net_state_lock_))
		{
			if (agg_verts.find(v) != agg_verts.end())
				continue;
	
			/* Unaggregated external vertices are automatically removed since they
			 * should not be considered any any routing computations.
			 */
			if (net_state_->get_external(v, *net_state_lock_))
			{
				remove_verts.push_back(v);
				continue;
			}
	
			/* Figure out what to do with the vertex based on the configured action */
			switch (action)
			{
			case ENA_REMOVE:
			{
				remove_verts.push_back(v);
				break;
			}
			case ENA_IGNORE:
			{
				p4p::PID tmp("temp.pid", last_temp_pid_++);
				pids.insert(PinnedPID(tmp, v));
				net_state_->set_pid(v, tmp, *net_state_lock_);
				break;
			}
			default:
				throw std::runtime_error("Illegal state: invalid extra node action");
			}
		}
	
		/* Remove all the vertices we were told to remove. */
		BOOST_FOREACH(const NetVertex& v, remove_verts)
		{
			net_state_->remove_node(v, *net_state_lock_);
		}
	}

	void find_interdomain_links(p4p::PIDLinkVector& links) const
	{
		BOOST_FOREACH(const NetEdge& e, net_state_->get_edges(*net_state_lock_))
		{
			NetVertex src, dst;
			net_state_->get_edge_vert(e, src, dst, *net_state_lock_);
	
			const p4p::PID&  l_src = net_state_->get_pid(src, *net_state_lock_);
			const p4p::PID&  l_dst = net_state_->get_pid(dst, *net_state_lock_);
	
			/* Ignore if both intradomain or both interdomain */
			if (l_src.get_external() == l_dst.get_external())
				continue;

			links.push_back(p4p::PIDLink(l_src, l_dst));
		}
	}

	bool compute_pdistances(OptPluginBaseConstPtr plugin,
			    const PinnedPIDSet& pids,
			    const p4p::PIDSet& pids_external,
			    PIDMatrix& intradomain_pdistances, const WritableLock& intradomain_pdistances_lock,
			    SparsePIDMatrix& interdomain_pdistances, const WritableLock& interdomain_pdistances_lock) const
	{
		const SparsePIDMatrix* dynamic_pdistances = plugin ? plugin->get_edge_pdistances() : &EMPTY_EDGE_COSTS;
		BlockReadLock dynamic_pdistances_lock(*dynamic_pdistances);
	
		const PIDRouting& intradomain_routing		= *view_state_.get()->get_intradomain_routing(view_state_.get_view_lock());
		const SparsePIDMatrix& static_pdistances	= *view_state_.get()->get_link_pdistances(view_state_.get_view_lock());
		const ReadableLock& intradomain_routing_lock	= view_state_.get_intradomain_routing_lock();
		const ReadableLock& static_pdistances_lock	= view_state_.get_link_pdistances_lock();
	
		/* Get the pdistance defaults from the view */
		double intrapid_pdistance		= view_state_.get()->get_default_intrapid_pdistance(view_state_.get_view_lock());
		double interpid_pdistance		= view_state_.get()->get_default_interpid_pdistance(view_state_.get_view_lock());
		double interdomain_pdistance		= view_state_.get()->get_default_interdomain_pdistance(view_state_.get_view_lock());
		double pidlink_pdistance		= view_state_.get()->get_default_pidlink_pdistance(view_state_.get_view_lock());
		bool interdomain_includes_intra		= view_state_.get()->get_interdomain_includes_intradomain(view_state_.get_view_lock());

		/* Fill in entries of the interdomain pdistance matrix */
		get_logger().info("computing interdomain pdistances for view");
		p4p::PIDLinkVector interdomain_links;
		find_interdomain_links(interdomain_links);
		get_logger().info("found %u interdomain links", interdomain_links.size());
		BOOST_FOREACH(const p4p::PIDLink& link, interdomain_links)
		{
			double e_p;
	
			if (get_logger().isDebugEnabled())
				get_logger().debug("computing pdistance for %s->%s", PIDCStr(link.first), PIDCStr(link.second));
	
			/* Use static pdistance if one is defined */
			if (!std::isnan((e_p = static_pdistances.get_by_pid(link.first, link.second, static_pdistances_lock))))
			{
				get_logger().debug("using static pdistance: %lf", e_p);
				if (!interdomain_pdistances.set_by_pid(link.first, link.second, e_p, interdomain_pdistances_lock))
					get_logger().error("failed to set static pdistance for %s->%s", PIDCStr(link.first), PIDCStr(link.second));
				continue;
			}
	
			/* Next try the dynamic edge pdistance */
			if (!std::isnan((e_p = dynamic_pdistances->get_by_pid(link.first, link.second, dynamic_pdistances_lock))))
			{
				get_logger().debug("using dynamic pdistance: %lf", e_p);
				if (!interdomain_pdistances.set_by_pid(link.first, link.second, e_p, interdomain_pdistances_lock))
					get_logger().error("failed to set dynamic pdistance for %s->%s", PIDCStr(link.first), PIDCStr(link.second));
				continue;
			}
	
			/* Fall back to the default interdomain pdistance (clear the entry in the matrix) */
			get_logger().debug("using default pdistance: %lf", interdomain_pdistance);
			if (!interdomain_pdistances.set_by_pid(link.first, link.second, interdomain_pdistance, interdomain_pdistances_lock))
				get_logger().error("failed to set default pdistance for %s->%s", PIDCStr(link.first), PIDCStr(link.second));
		}

		/* Fill in pdistances from intradomain PIDs to both intradomain and interdomain PIDs */
		const PIDMatrixPIDsByIdx& intradomain_pdistances_pids = intradomain_pdistances.get_pids_vector(intradomain_pdistances_lock);
		PIDRouting::RouteComputationContext route_context;

		get_logger().info("computing pdistances from intradomain pids");
		BOOST_FOREACH(const IndexedPID& l_src, intradomain_pdistances_pids)
		{
			/* Grab the routes emanating from this source */
			intradomain_routing.get_routes(*pids.find(PinnedPID(l_src, NULL)),
						*net_state_, *net_state_lock_,
						pids,
						route_context,
						intradomain_routing_lock);

			/* Fill in pdistances to all other intradomain PIDs */
			BOOST_FOREACH(const IndexedPID& l_dst, intradomain_pdistances_pids)
			{
				if (get_logger().isDebugEnabled())
					get_logger().debug("computing intradomain pdistance for %s->%s", PIDCStr(l_src), PIDCStr(l_dst));
	
				if (l_src.get_index() == l_dst.get_index())
				{
					get_logger().debug("using intrapid pdistance");
					intradomain_pdistances.set(l_src, l_dst, intrapid_pdistance, intradomain_pdistances_lock);
					continue;
				}
	
				/* Locate the route.  If one doesn't exist, the pair is disconnected
				 * and we assign the default interpid pdistance
				 */
				PIDRouting::PinnedRouteMap::const_iterator route_itr = route_context.get_result().find(PinnedPID(l_dst, NULL));
				if (route_itr == route_context.get_result().end())
				{
					get_logger().debug("no route found");
					intradomain_pdistances.set(l_src, l_dst, interpid_pdistance, intradomain_pdistances_lock);
					continue;
				}
	
				/* Trace along the route and compute the pdistance */
				const PIDRouting::PinnedRoute& route = route_itr->second;
				double p = 0.0;
				for (unsigned int hop = 1; hop < route.size(); ++hop)
				{
					const p4p::PID&  e_src = route.at(hop-1);
					const p4p::PID&  e_dst = route.at(hop);
					double e_p;
	
					if (get_logger().isDebugEnabled())
						get_logger().debug("tracing link %s->%s", PIDCStr(e_src), PIDCStr(e_dst));
	
					/* Use static pdistance if one is defined */
					if (!std::isnan((e_p = static_pdistances.get_by_pid(e_src, e_dst, static_pdistances_lock))))
					{
						p += e_p;
						continue;
					}
	
					/* Next try the dynamic edge pdistance */
					if (!std::isnan((e_p = dynamic_pdistances->get_by_pid(e_src, e_dst, dynamic_pdistances_lock))))
					{
						p += e_p;
						continue;
					}
	
					/* Fall back to the view's default pid-link pdistance */
					p += pidlink_pdistance;
				}
				get_logger().debug("using pdistance: %lf", p);
				intradomain_pdistances.set(l_src, l_dst, p, intradomain_pdistances_lock);
			}

			/* Fill in pdistances from 'l_src' to interdomain PIDs */
			BOOST_FOREACH(const p4p::PID&  l_inter, pids_external)
			{
				if (get_logger().isDebugEnabled())
					get_logger().debug("computing interdomain pdistance for %s->%s", PIDCStr(l_src), PIDCStr(l_inter));

				/* Ignore if there is already an entry from an explicitly-defined link above. */
				if (!std::isnan(interdomain_pdistances.get_by_pid(l_src, l_inter, interdomain_pdistances_lock)))
				{
					get_logger().debug("explicit interdomain pdistance previously computed");
					continue;
				}

				double pdistance = interdomain_pdistance;

				/* Dummy loop allows some cleaner code below by allowing us to use 'break' */
				do
				{
					/* Find route from intradomain PID ('l_src') to interdomain ('l_inter'). */
					PIDRouting::PinnedRouteMap::const_iterator route_itr = route_context.get_result().find(PinnedPID(l_inter, NULL));
					if (route_itr == route_context.get_result().end())
					{
						get_logger().debug("no route; using default interdomain pdistance");
						break;
					}

					/* Find egress PID. This is the next-to-last hop of the path */
					const PIDRouting::PinnedRoute& route = route_itr->second;
					PIDRouting::PinnedRoute::const_reverse_iterator hop_itr = route.rbegin();
					if (++hop_itr == route.rend())
					{
						get_logger().debug("route has length 1; using default interdomain pdistance");
						break;
					}
					const PinnedPID& p_egress = *hop_itr;

					/* Start off with the cost of the egress link */
					pdistance = interdomain_pdistances.get_by_pid(p_egress, l_inter, interdomain_pdistances_lock);

					/* Include the intradomain pdistance if configured to do so */
					if (interdomain_includes_intra)
						pdistance += intradomain_pdistances.get_by_pid(l_src, p_egress, intradomain_pdistances_lock);
				} while (0);

				/* Set the cost */
				get_logger().debug("using pdistance: %lf", pdistance);
				interdomain_pdistances.set_by_pid(l_src, l_inter, pdistance, interdomain_pdistances_lock);
			}

		}

		/* Compute pdistances from interdomain PIDs to intradomain PIDs */
		get_logger().info("computing pdistances from interdomain pids");
		BOOST_FOREACH(const IndexedPID& l_src, interdomain_pdistances.get_pids_vector(interdomain_pdistances_lock))
		{
			/* Skip intradomain PIDs */
			if (!l_src.get_external())
				continue;

			/* Grab the routes emanating from this source */
			intradomain_routing.get_routes(*pids.find(PinnedPID(l_src, NULL)),
						*net_state_, *net_state_lock_,
						pids,
						route_context,
						intradomain_routing_lock);

			/* Fill in pdistances to each intradomain PID */
			BOOST_FOREACH(const IndexedPID& l_intra, intradomain_pdistances_pids)
			{
				if (get_logger().isDebugEnabled())
					get_logger().debug("computing interdomain pdistance for %s->%s", PIDCStr(l_src), PIDCStr(l_intra));

				/* Ignore if there is already an entry from an explicitly-defined link above. */
				if (!std::isnan(interdomain_pdistances.get_by_pid(l_src, l_intra, interdomain_pdistances_lock)))
				{
					get_logger().debug("explicit interdomain pdistance previously computed");
					continue;
				}

				double pdistance = interdomain_pdistance;

				do
				{
					/* Find route from interdomain PID ('l_src') to intradomain ('l_intra'). */
					PIDRouting::PinnedRouteMap::const_iterator route_itr = route_context.get_result().find(PinnedPID(l_intra, NULL));
					if (route_itr == route_context.get_result().end())
					{
						get_logger().debug("no route; using default interdomain pdistance");
						break;
					}

					/* Find egress PID. This is the second hop of the path */
					const PIDRouting::PinnedRoute& route = route_itr->second;
					PIDRouting::PinnedRoute::const_iterator hop_itr = route.begin();
					if (++hop_itr == route.end())
					{
						get_logger().debug("route has length 1; using default interdomain pdistance");
						break;
					}
					const PinnedPID& p_egress = *hop_itr;

					/* Start off with the cost of the egress link */
					pdistance = interdomain_pdistances.get_by_pid(l_src, p_egress, interdomain_pdistances_lock);

					/* Include the intradomain pdistance if configured to do so */
					if (interdomain_includes_intra)
						pdistance += intradomain_pdistances.get_by_pid(p_egress, l_intra, intradomain_pdistances_lock);

				} while (0);

				get_logger().debug("using pdistance: %lf", pdistance);
				interdomain_pdistances.set_by_pid(l_src, l_intra, pdistance, interdomain_pdistances_lock);
			}

		}

		return true;
	}

	//mutable log4cpp::Category&		logger_;
	log4cpp::Category&			logger_;

	NetStatePtr				net_state_orig_;
	const ReadableLock&			net_state_orig_lock_;
	ViewUpdateState&			view_state_;

	NetStatePtr				net_state_;
	boost::shared_ptr<WritableLock>		net_state_lock_;

	unsigned int				last_temp_pid_;

protected:
	/* Computed results */
	PIDMatrixPtr				result_intradomain_pdistances_;
	BlockWriteLock				result_intradomain_pdistances_lock_;

	SparsePIDMatrixPtr			result_interdomain_pdistances_;
	BlockWriteLock				result_interdomain_pdistances_lock_;

	unsigned int				result_update_interval_;

};

class ViewUpdateDirect : public ViewUpdateBase<ViewUpdateStateDirect>
{
public:
	ViewUpdateDirect(NetStatePtr net_state, const ReadableLock& net_state_lock,
			 ViewUpdateStateDirect& view_state)
		: ViewUpdateBase<ViewUpdateStateDirect>(net_state, net_state_lock, view_state)
	{}
	virtual ~ViewUpdateDirect() {}

	virtual unsigned int do_update();
};

class ViewUpdateUpgrade : public ViewUpdateBase<ViewUpdateStateUpgrade>
{
public:
	ViewUpdateUpgrade(GlobalStatePtr global_state, const UpgradableReadLock& global_state_lock,
			  NetStatePtr net_state, const ReadableLock& net_state_lock,
			  ViewRegistryPtr views, const UpgradableReadLock& views_lock,
			  ViewUpdateStateUpgrade& view_state)
		: ViewUpdateBase<ViewUpdateStateUpgrade>(net_state, net_state_lock, view_state),
		  global_state_(global_state),
		  global_state_lock_(global_state_lock),
		  views_(views),
		  views_lock_(views_lock)
	{}
	virtual ~ViewUpdateUpgrade() {}

	virtual unsigned int do_update();

private:
	GlobalStatePtr global_state_;
	const UpgradableReadLock& global_state_lock_;

	ViewRegistryPtr views_;
	const UpgradableReadLock& views_lock_;
};

#endif
