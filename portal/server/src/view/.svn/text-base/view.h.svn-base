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


#ifndef VIEW_H
#define VIEW_H

#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <tr1/unordered_set>
#include <fstream>
#include <p4pserver/locking.h>
#include <p4p/pid.h>
#include <p4pserver/dist_obj.h>
#include <p4pserver/pid_matrix.h>
#include <p4pserver/pid_aggregation.h>
#include <p4pserver/pid_map.h>
#include <p4pserver/pid_routing.h>
#include <p4pserver/net_state.h>

class ViewRegistry;

/*
 * Action used when there are extra nodes left in the physical
 * topology after aggregation into the PID-level topology. This
 * is done before computing routes (if RouteMode == RM_WEIGHTS).
 */
enum ExtraNodeAction
{
	ENA_IGNORE,	/* Leave the node in the topology (it can be an intermediate hop on computed routes) */
	ENA_REMOVE,	/* Remove the node from the topology (it cannot be an intermediate hop on computed routes) */
};

class OptPluginBase;
typedef boost::shared_ptr<OptPluginBase> OptPluginBasePtr;
typedef boost::shared_ptr<const OptPluginBase> OptPluginBaseConstPtr;

class View;
typedef boost::shared_ptr<View> ViewPtr;
typedef boost::shared_ptr<const View> ViewConstPtr;

class View : public DistributedObject
{
public:
	/* Minimum and maximum pDistances according to spec */
	static const unsigned int MIN_PDISTANCE;
	static const unsigned int MAX_PDISTANCE;

	static const unsigned int CHILD_IDX_AGGREGATION		= 0;
	static const unsigned int CHILD_IDX_PREFIXES		= 1;
	static const unsigned int CHILD_IDX_INTRA_ROUTING	= 2;
	static const unsigned int CHILD_IDX_LINK_PDISTANCES	= 3;
	static const unsigned int CHILD_IDX_INTRA_PDISTANCES	= 4;
	static const unsigned int CHILD_IDX_INTER_PDISTANCES	= 5;

	View(ViewRegistry* view_reg = NULL, const std::string& name = "", const bfs::path& dist_file = bfs::path());
	virtual ~View();

	const std::string& get_name(const ReadableLock& lock) const;

	bool set_plugin(const std::string& name, const WritableLock& lock);

	const std::string& get_plugin_name(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return plugin_name_;
	}

	OptPluginBasePtr get_plugin(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return plugin_inst_;
	}

	/* NOTE: set_child() method automatically calls changed(); no need to do it here directly */

	/* IP->PID Mapping */
	PIDAggregationPtr get_aggregation(const ReadableLock& lock)				{ return boost::dynamic_pointer_cast<PIDAggregation>(get_child(CHILD_IDX_AGGREGATION, lock)); }
	void set_aggregation(PIDAggregationPtr value, const WritableLock& lock)		{ set_child(CHILD_IDX_AGGREGATION, value, lock); }

	PIDMapPtr get_prefixes(const ReadableLock& lock)					{ return boost::dynamic_pointer_cast<PIDMap>(get_child(CHILD_IDX_PREFIXES, lock)); }
	void set_prefixes(PIDMapPtr value, const WritableLock& lock)			{ set_child(CHILD_IDX_PREFIXES, value, lock); }

	/* Routing configuration */
	PIDRoutingPtr get_intradomain_routing(const ReadableLock& lock)			{ return boost::dynamic_pointer_cast<PIDRouting>(get_child(CHILD_IDX_INTRA_ROUTING, lock)); }
	void set_intradomain_routing(PIDRoutingPtr value, const WritableLock& lock)	{ set_child(CHILD_IDX_INTRA_ROUTING, value, lock); }

	/* Configured static link pdistances */
	SparsePIDMatrixPtr get_link_pdistances(const ReadableLock& lock)			{ return boost::dynamic_pointer_cast<SparsePIDMatrix>(get_child(CHILD_IDX_LINK_PDISTANCES, lock)); }
	void set_link_pdistances(SparsePIDMatrixPtr value, const WritableLock& lock)	{ set_child(CHILD_IDX_LINK_PDISTANCES, value, lock); }

	/* Computed pdistances */
	PIDMatrixPtr get_intradomain_pdistances(const ReadableLock& lock)			{ return boost::dynamic_pointer_cast<PIDMatrix>(get_child(CHILD_IDX_INTRA_PDISTANCES, lock)); }
	void set_intradomain_pdistances(PIDMatrixPtr value, const WritableLock& lock)	{ set_child(CHILD_IDX_INTRA_PDISTANCES, value, lock); }

	SparsePIDMatrixPtr get_interdomain_pdistances(const ReadableLock& lock)			{ return boost::dynamic_pointer_cast<SparsePIDMatrix>(get_child(CHILD_IDX_INTER_PDISTANCES, lock)); }
	void set_interdomain_pdistances(SparsePIDMatrixPtr value, const WritableLock& lock)	{ set_child(CHILD_IDX_INTER_PDISTANCES, value, lock); }

	/* Properties */
	unsigned int get_update_interval(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return update_interval_;
	}
	void set_update_interval(unsigned int value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		update_interval_ = value;
		changed(lock);
	}

	double get_default_intrapid_pdistance(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return default_intrapid_pdistance_;
	}
	void set_default_intrapid_pdistance(double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		default_intrapid_pdistance_ = value;
		changed(lock);
	}

	double get_default_interpid_pdistance(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return default_interpid_pdistance_;
	}
	void set_default_interpid_pdistance(double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		default_interpid_pdistance_ = value;
		changed(lock);
	}

	double get_default_interdomain_pdistance(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return default_interdomain_pdistance_;
	}
	void set_default_interdomain_pdistance(double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		default_interdomain_pdistance_ = value;
		changed(lock);
	}

	double get_default_pidlink_pdistance(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return default_pidlink_pdistance_;
	}
	void set_default_pidlink_pdistance(double value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		default_pidlink_pdistance_ = value;
		changed(lock);
	}

	bool get_interdomain_includes_intradomain(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return interdomain_includes_intradomain_;
	}
	void set_interdomain_includes_intradomain(bool value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		interdomain_includes_intradomain_ = value;
		changed(lock);
	}

	unsigned int get_pid_ttl(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return pid_ttl_;
	}
	void set_pid_ttl(unsigned int value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		pid_ttl_ = value;
		changed(lock);
	}

	unsigned int get_pdistance_ttl(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return pdistance_ttl_;
	}
	void set_pdistance_ttl(unsigned int value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		pdistance_ttl_ = value;
		changed(lock);
	}

	ExtraNodeAction get_extra_node_action(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return extra_node_action_;
	}
	void set_extra_node_action(ExtraNodeAction value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		extra_node_action_ = value;
		changed(lock);
	}

	bool has_pid(const p4p::PID&  pid, const ReadableLock& aggregation_lock, const ReadableLock& lock)
	{
		lock.check_read(get_local_mutex());
		return get_aggregation(lock)->has(pid, aggregation_lock);
	}

	bool add_pid(const std::string& name, const p4p::PID&  pid,
		     const WritableLock& aggregation_lock,
		     const WritableLock& intradomain_routing_lock,
		     const WritableLock& link_pdistances_lock,
		     const ReadableLock& lock)
	{
		lock.check_read(get_local_mutex());

		if (!get_aggregation(lock)->add(name, pid, NetVertexNameSet(), aggregation_lock))
			return false;

		get_intradomain_routing(lock)->add_pid(pid, intradomain_routing_lock);
		get_link_pdistances(lock)->add_pid(pid, link_pdistances_lock);
		return true;
	}

	bool remove_pid(const p4p::PID&  pid,
			const WritableLock& aggregation_lock,
			const WritableLock& intradomain_routing_lock,
			const WritableLock& link_pdistances_lock,
			const ReadableLock& lock)
	{
		lock.check_read(get_local_mutex());

		if (!has_pid(pid, aggregation_lock, lock))
			return false;

		get_aggregation(lock)->remove(pid, aggregation_lock);
		get_intradomain_routing(lock)->remove_pid(pid, intradomain_routing_lock);
		get_link_pdistances(lock)->remove_pid(pid, link_pdistances_lock);
		return true;
	}

	const p4p::PID&  lookup_pid(const std::string& name, const ReadableLock& aggregation_lock, const ReadableLock& lock)
	{
		lock.check_read(get_local_mutex());
		return get_aggregation(lock)->lookup(name, aggregation_lock);
	}

protected:
	virtual LocalObjectPtr create_empty_instance() const { return ViewPtr(new View()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
#endif

	virtual void on_update(const WritableLock& lock);

private:
	friend class ViewRegistry;

	ViewRegistry* view_reg_;
	std::string name_; /* name is not serialized since it is assigned at construction!! */

	std::string plugin_name_;
	OptPluginBasePtr plugin_inst_;

	unsigned int update_interval_;

	ExtraNodeAction extra_node_action_;

	double	default_intrapid_pdistance_;
	double	default_interpid_pdistance_;
	double	default_interdomain_pdistance_;
	double	default_pidlink_pdistance_;
	bool	interdomain_includes_intradomain_;
	unsigned int pid_ttl_;
	unsigned int pdistance_ttl_;
};

template <class ViewLock			= const EmptyLock,
	  class ViewLockImpl			= const NoLock,

	  class AggregationLock			= const EmptyLock,
	  class AggregationLockImpl		= const NoLock,

	  class PrefixesLock			= const EmptyLock,
	  class PrefixesLockImpl		= const NoLock,
	  
	  class IntraRoutingLock		= const EmptyLock,
	  class IntraRoutingLockImpl		= const NoLock,
	  
	  class LinkPDistancesLock		= const EmptyLock,
	  class LinkPDistancesLockImpl		= const NoLock,
	  
	  class IntraPDistancesLock		= const EmptyLock,
	  class IntraPDistancesLockImpl		= const NoLock,
	  
	  class InterPDistancesLock		= const EmptyLock,
	  class InterPDistancesLockImpl		= const NoLock>
class ViewWrapper
{
public:
	typedef ViewWrapper<ViewLock,
			    AggregationLock,
			    PrefixesLock,
			    IntraRoutingLock,
			    LinkPDistancesLock,
			    IntraPDistancesLock,
			    InterPDistancesLock> MyType;

	class PIDUpdater
	{
	public:
		PIDUpdater(MyType& _view) : view(_view), changed(false), saved(false)
		{
			view.update_pids_begin(this);
		}

		~PIDUpdater()
		{
			if (changed && saved)
				view.update_pids_end(this);
		}

		void save() { saved = true; }

	private:
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
		friend class ViewWrapper;

		MyType& view;
	
		bool changed;
		bool saved;
		p4p::PIDSet all_pids;
		p4p::PIDSet added_pids;
		p4p::PIDSet removed_pids;
	};

	ViewWrapper(ViewPtr view)
		: view_				(view),
		  view_lock_			(new ViewLockImpl(*view)),
		  aggregation_lock_		(new AggregationLockImpl(*view->get_aggregation(*view_lock_))),
		  prefixes_lock_		(new PrefixesLockImpl(*view->get_prefixes(*view_lock_))),
		  intradomain_routing_lock_	(new IntraRoutingLockImpl(*view->get_intradomain_routing(*view_lock_))),
		  link_pdistances_lock_		(new LinkPDistancesLockImpl(*view->get_link_pdistances(*view_lock_))),
		  intradomain_pdistances_lock_	(new IntraPDistancesLockImpl(*view->get_intradomain_pdistances(*view_lock_))),
		  interdomain_pdistances_lock_	(new InterPDistancesLockImpl(*view->get_interdomain_pdistances(*view_lock_)))
	{}

	ViewWrapper(ViewPtr view,
		    boost::shared_ptr<ViewLock> view_lock,
		    boost::shared_ptr<AggregationLock> aggregation_lock			= boost::shared_ptr<const NoLock>(),
		    boost::shared_ptr<PrefixesLock> prefixes_lock			= boost::shared_ptr<const NoLock>(),
		    boost::shared_ptr<IntraRoutingLock> intradomain_routing_lock	= boost::shared_ptr<const NoLock>(),
		    boost::shared_ptr<LinkPDistancesLock> link_pdistances_lock			= boost::shared_ptr<const NoLock>(),
		    boost::shared_ptr<IntraPDistancesLock> intradomain_pdistances_lock		= boost::shared_ptr<const NoLock>(),
		    boost::shared_ptr<InterPDistancesLock> interdomain_pdistances_lock		= boost::shared_ptr<const NoLock>())
		: view_				(view),
		  view_lock_			(view_lock),
		  aggregation_lock_		(aggregation_lock),
		  prefixes_lock_		(prefixes_lock),
		  intradomain_routing_lock_	(intradomain_routing_lock),
		  link_pdistances_lock_		(link_pdistances_lock),
		  intradomain_pdistances_lock_	(intradomain_pdistances_lock),
		  interdomain_pdistances_lock_	(interdomain_pdistances_lock)
	{}

	template <class T>
	ViewWrapper(const T& rhs)
		: view_(rhs.get()),
		  view_lock_(rhs.get_view_lock_ptr()),
		  aggregation_lock_(rhs.get_aggregation_lock_ptr()),
		  prefixes_lock_(rhs.get_prefixes_lock_ptr()),
		  intradomain_routing_lock_(rhs.get_intradomain_routing_lock_ptr()),
		  link_pdistances_lock_(rhs.get_link_pdistances_lock_ptr()),
		  intradomain_pdistances_lock_(rhs.get_intradomain_pdistances_lock_ptr()),
		  interdomain_pdistances_lock_(rhs.get_interdomain_pdistances_lock_ptr())
	{}

	ViewPtr get() const									{ return view_; }

	boost::shared_ptr<ViewLock> get_view_lock_ptr() const					{ return view_lock_; }
	boost::shared_ptr<AggregationLock> get_aggregation_lock_ptr() const			{ return aggregation_lock_; }
	boost::shared_ptr<PrefixesLock> get_prefixes_lock_ptr() const				{ return prefixes_lock_; }
	boost::shared_ptr<IntraRoutingLock> get_intradomain_routing_lock_ptr() const		{ return intradomain_routing_lock_; }
	boost::shared_ptr<LinkPDistancesLock> get_link_pdistances_lock_ptr() const		{ return link_pdistances_lock_; }
	boost::shared_ptr<IntraPDistancesLock> get_intradomain_pdistances_lock_ptr() const	{ return intradomain_pdistances_lock_; }
	boost::shared_ptr<InterPDistancesLock> get_interdomain_pdistances_lock_ptr() const	{ return interdomain_pdistances_lock_; }

	const ViewLock& get_view_lock() const					{ return *view_lock_; }
	const AggregationLock& get_aggregation_lock() const			{ return *aggregation_lock_; }
	const PrefixesLock& get_prefixes_lock() const				{ return *prefixes_lock_; }
	const IntraRoutingLock& get_intradomain_routing_lock() const		{ return *intradomain_routing_lock_; }
	const LinkPDistancesLock& get_link_pdistances_lock() const		{ return *link_pdistances_lock_; }
	const IntraPDistancesLock& get_intradomain_pdistances_lock() const	{ return *intradomain_pdistances_lock_; }
	const InterPDistancesLock& get_interdomain_pdistances_lock() const	{ return *interdomain_pdistances_lock_; }

	bool get_pid_vertices(const p4p::PID&  pid, NetVertexNameSet& result) const
	{
		return view_->get_aggregation(get_view_lock())->get_vertices(pid, result, get_aggregation_lock());
	}

	bool set_pid_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices)
	{
		return view_->get_aggregation(get_view_lock())->set_vertices(pid, vertices, get_aggregation_lock());
	}

	const PIDAggregation::PIDRecordsByPID& get_pid_map() const
	{
		return view_->get_aggregation(get_view_lock())->get_pid_map(get_aggregation_lock());
	}

	const PIDAggregation::LinkRecordsByName& get_link_map() const
	{
		return view_->get_aggregation(get_view_lock())->get_link_map(get_aggregation_lock());
	}

	double const get_pdistance(const p4p::PID&  src, const p4p::PID&  dst) const
	{
		/* We don't support costs between two external PIDs */
		if (src.get_external() && dst.get_external())
			return NAN;

		/* Ensure both PIDs have prefixes assigned (i.e., they are externally visible) */
		if (!get()->get_prefixes(get_view_lock())->has_prefixes(src, get_prefixes_lock())
			|| !get()->get_prefixes(get_view_lock())->has_prefixes(dst, get_prefixes_lock()))
			return NAN;

		if (!src.get_external() && !dst.get_external())
			return get_pdistance_intradomain(src, dst);
		else
			return get_pdistance_interdomain(src, dst);
	}

private:

	double const get_pdistance_intradomain(const p4p::PID&  src, const p4p::PID&  dst) const
	{
		return get()->get_intradomain_pdistances(get_view_lock())->get_by_pid(src, dst, get_intradomain_pdistances_lock());
	}

	double const get_pdistance_interdomain(const p4p::PID&  src, const p4p::PID&  dst) const
	{
		/* Internal PID must be in the intradomain pdistance matrix. */
		const PIDMatrixPIDsByLoc& intradomain_pids = get()->get_intradomain_pdistances(get_view_lock())->get_pids_set(get_intradomain_pdistances_lock());
		if (intradomain_pids.find(src.get_external() ? dst : src) == intradomain_pids.end())
			return NAN;

		return get()->get_interdomain_pdistances(get_view_lock())->get_by_pid(src, dst, get_interdomain_pdistances_lock());
	}

	ViewPtr	view_;

	boost::shared_ptr<ViewLock>		view_lock_;
	boost::shared_ptr<AggregationLock>	aggregation_lock_;
	boost::shared_ptr<PrefixesLock>		prefixes_lock_;
	boost::shared_ptr<IntraRoutingLock>	intradomain_routing_lock_;
	boost::shared_ptr<LinkPDistancesLock>	link_pdistances_lock_;
	boost::shared_ptr<IntraPDistancesLock>	intradomain_pdistances_lock_;
	boost::shared_ptr<InterPDistancesLock>	interdomain_pdistances_lock_;
};


#endif
