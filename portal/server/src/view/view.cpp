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


#include "view.h"

#include <boost/foreach.hpp>
#include "view_registry.h"
#include "plugin_base.h"
#include "plugin_registry.h"
#include "options.h"
#include "state.h"
#include "view_update_job.h"

/* Default weight unless one is explicitly assigned */
const double DEFAULT_WEIGHT = 1.0;

const unsigned int View::MIN_PDISTANCE = 0;
const unsigned int View::MAX_PDISTANCE = (2 << 16) - 1;

View::View(ViewRegistry* view_reg, const std::string& name, const bfs::path& dist_file)
	: DistributedObject(dist_file),
	  view_reg_(view_reg),
	  name_(name),
	  update_interval_(5 * 60),	/* 5 minutes */
	  extra_node_action_(ENA_REMOVE),
	  default_intrapid_pdistance_(0),
	  default_interpid_pdistance_(INFINITY),
	  default_interdomain_pdistance_(60),
	  default_pidlink_pdistance_(1),
	  interdomain_includes_intradomain_(true),
	  pid_ttl_(7 * 24 * 60 * 60),	/* 1 week */
	  pdistance_ttl_(2 * 60 * 60)	/* 2 hours */
{
	{
		BlockWriteLock lock(*this);
		set_child(CHILD_IDX_AGGREGATION, PIDAggregationPtr(new PIDAggregation()), lock);
		set_child(CHILD_IDX_PREFIXES, PIDMapPtr(new PIDMap()), lock);
		set_child(CHILD_IDX_INTRA_ROUTING, PIDRoutingPtr(new PIDRouting(DEFAULT_WEIGHT)), lock);
		set_child(CHILD_IDX_LINK_PDISTANCES, SparsePIDMatrixPtr(new SparsePIDMatrix()), lock);
		set_child(CHILD_IDX_INTRA_PDISTANCES, PIDMatrixPtr(new PIDMatrix()), lock);
		set_child(CHILD_IDX_INTER_PDISTANCES, SparsePIDMatrixPtr(new SparsePIDMatrix()), lock);
	}

	after_construct();
}

View::~View()
{
	before_destruct();
}

#ifdef P4P_CLUSTER
void View::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	stream << get_plugin_name(lock);

	unsigned int i;
	double d;
	ExtraNodeAction a;
	bool b;

	i = get_update_interval(lock);			stream << i;
	a = get_extra_node_action(lock);		stream << a;
	d = get_default_intrapid_pdistance(lock);	stream << d;
	d = get_default_interpid_pdistance(lock);	stream << d;
	d = get_default_interdomain_pdistance(lock);	stream << d;
	d = get_default_pidlink_pdistance(lock);	stream << d;
	b = get_interdomain_includes_intradomain(lock);	stream << b;
	i = get_pid_ttl(lock);				stream << i;
	i = get_pdistance_ttl(lock);			stream << i;
}

void View::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	std::string plugin_name;
	stream >> plugin_name;
	set_plugin(plugin_name, lock);

	unsigned int i;
	double d;
	ExtraNodeAction a;
	bool b;

	stream >> i;	set_update_interval(i, lock);
	stream >> a;	set_extra_node_action(a, lock);
	stream >> d;	set_default_intrapid_pdistance(d, lock);
	stream >> d;	set_default_interpid_pdistance(d, lock);
	stream >> d;	set_default_interdomain_pdistance(d, lock);
	stream >> d;	set_default_pidlink_pdistance(d, lock);
	stream >> b;	set_interdomain_includes_intradomain(b, lock);
	stream >> i;	set_pid_ttl(i, lock);
	stream >> i;	set_pdistance_ttl(i, lock);

	changed(lock);
}
#endif

DistributedObjectPtr View::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());

	ViewPtr new_obj = boost::dynamic_pointer_cast<View>(View::create_empty_instance());
	BlockWriteLock new_obj_lock(*new_obj);

	new_obj->set_plugin(plugin_name_, new_obj_lock);
	new_obj->set_update_interval			(get_update_interval(lock), new_obj_lock);
	new_obj->set_extra_node_action			(get_extra_node_action(lock), new_obj_lock);
	new_obj->set_default_intrapid_pdistance		(get_default_intrapid_pdistance(lock), new_obj_lock);
	new_obj->set_default_interpid_pdistance		(get_default_interpid_pdistance(lock), new_obj_lock);
	new_obj->set_default_interdomain_pdistance	(get_default_interdomain_pdistance(lock), new_obj_lock);
	new_obj->set_default_pidlink_pdistance		(get_default_pidlink_pdistance(lock), new_obj_lock);
	new_obj->set_interdomain_includes_intradomain	(get_interdomain_includes_intradomain(lock), new_obj_lock);
	new_obj->set_pid_ttl				(get_pid_ttl(lock), new_obj_lock);
	new_obj->set_pdistance_ttl			(get_pdistance_ttl(lock), new_obj_lock);
	
	return new_obj;
}

const std::string& View::get_name(const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return name_;
}

bool View::set_plugin(const std::string& name, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/*  If the plugin isn't changing, we don't have to do anything. */
	if (plugin_name_ == name)
		return true;

	/* Instantiate the plugin (if a new one is being set) */
	OptPluginBasePtr p;
	if (!name.empty() && !(p = OptPluginRegistry::Instance().get_instance(name)))
		return false;

	/* Replace the old plugin (old one will be freed properly) */
	plugin_inst_ = p;
	plugin_name_ = name;

	changed(lock);

	return true;
}

void View::on_update(const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Schedule the background job for the view */
	if (update_interval_ > 0)
		JOB_QUEUE->enqueue(JobPtr(new ViewUpdateJob(JOB_QUEUE, boost::get_system_time() + boost::posix_time::seconds(update_interval_), name_)));
}

