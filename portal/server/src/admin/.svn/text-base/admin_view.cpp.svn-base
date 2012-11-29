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


#include "admin_view.h"

#include <boost/lexical_cast.hpp>
#include "view_update.h"

AdminState::ReadLockRecord AdminViewBase::get_view_read() throw (admin_error)
{
	AdminState::ReadLockRecord views_rec = get_views_read();
	ViewPtr view = boost::dynamic_pointer_cast<ViewRegistry>(views_rec.first)->get(get_view(), *views_rec.second);
	if (!view)
		throw admin_invalid_view();
	return get_admin_state()->read_object_ref(views_rec, view);
}

AdminState::WriteLockRecord AdminViewBase::get_view_write() throw (admin_error)
{
	AdminState::WriteLockRecord views_rec = get_views_write();
	ViewPtr view = boost::dynamic_pointer_cast<ViewRegistry>(views_rec.first)->get(get_view(), *views_rec.second);
	if (!view)
		throw admin_invalid_view();
	return get_admin_state()->write_object_ref(views_rec, view);
}

AdminState::ReadLockRecord AdminViewBase::get_aggregation_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_aggregation(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_aggregation_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_aggregation(*view_rec.second));
}

AdminState::ReadLockRecord AdminViewBase::get_prefixes_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_prefixes(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_prefixes_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_prefixes(*view_rec.second));
}

AdminState::ReadLockRecord AdminViewBase::get_intradomain_routing_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_intradomain_routing(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_intradomain_routing_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_intradomain_routing(*view_rec.second));
}

AdminState::ReadLockRecord AdminViewBase::get_link_pdistances_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_link_pdistances(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_link_pdistances_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_link_pdistances(*view_rec.second));
}

AdminState::ReadLockRecord AdminViewBase::get_intradomain_pdistances_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_intradomain_pdistances(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_intradomain_pdistances_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_intradomain_pdistances(*view_rec.second));
}

AdminState::ReadLockRecord AdminViewBase::get_interdomain_pdistances_read() throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	return get_admin_state()->read_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_interdomain_pdistances(*view_rec.second));
}

AdminState::WriteLockRecord AdminViewBase::get_interdomain_pdistances_write() throw (admin_error)
{
	AdminState::WriteLockRecord view_rec = get_view_write();
	return get_admin_state()->write_object_ref(view_rec, boost::dynamic_pointer_cast<View>(view_rec.first)->get_interdomain_pdistances(*view_rec.second));
}

bool AdminViewAdd::commit(AdminState* admin) throw (admin_error)
{
	AdminViewBase::commit(admin);

	AdminState::WriteLockRecord views_rec = get_views_write();
	ViewRegistryPtr views = boost::dynamic_pointer_cast<ViewRegistry>(views_rec.first);
	return views->add(get_view(), *views_rec.second);
}

bool AdminViewDelete::commit(AdminState* admin) throw (admin_error)
{
	AdminViewBase::commit(admin);

	AdminState::WriteLockRecord views_rec = get_views_write();
	ViewRegistryPtr views = boost::dynamic_pointer_cast<ViewRegistry>(views_rec.first);
	return views->remove(get_view(), *views_rec.second);
}

bool AdminViewPDistanceUpdate::commit(AdminState* admin) throw (admin_error)
{
	AdminViewBase::commit(admin);

	AdminState::WriteLockRecord global_state_rec = get_admin_state()->root_ref();
	AdminState::ReadLockRecord net_state_rec = get_net_read();
	AdminState::WriteLockRecord views_rec = get_views_write();
	AdminState::WriteLockRecord view_rec = get_view_write();
	AdminState::ReadLockRecord aggregation_rec = get_aggregation_read();
	AdminState::ReadLockRecord prefixes_rec = get_aggregation_read();
	AdminState::ReadLockRecord intradomain_routing_rec = get_intradomain_routing_read();
	AdminState::ReadLockRecord link_pdistances_rec = get_link_pdistances_read();

	ViewUpdateStateDirect view_state(boost::dynamic_pointer_cast<View>(view_rec.first),
					 view_rec.second,
					 aggregation_rec.second,
					 prefixes_rec.second,
					 intradomain_routing_rec.second,
					 link_pdistances_rec.second);

	return ViewUpdateDirect(boost::dynamic_pointer_cast<NetState>(net_state_rec.first), *net_state_rec.second, view_state).do_update() != 0;
}

const p4p::PID&  AdminViewBase::lookup_pid(const std::string& name) throw (admin_error)
{
	AdminState::ReadLockRecord view_rec = get_view_read();
	ViewPtr view = boost::dynamic_pointer_cast<View>(view_rec.first);
	AdminState::ReadLockRecord aggregation_rec = get_aggregation_read();

	const p4p::PID&  result = view->lookup_pid(name, *aggregation_rec.second, *view_rec.second);
	if (result.is_invalid())
		throw admin_invalid_pid();
	return result;
}

bool AdminViewAddPID::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDBase::commit(admin);

	AdminState::WriteLockRecord view_rec = get_view_write();
	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	AdminState::WriteLockRecord intradomain_routing_rec = get_intradomain_routing_write();
	AdminState::WriteLockRecord link_pdistances_rec = get_link_pdistances_write();
	ViewPtr view = boost::dynamic_pointer_cast<View>(view_rec.first);

	return view->add_pid(get_pid(), get_pid_obj(),
			     *aggregation_rec.second,
			     *intradomain_routing_rec.second,
			     *link_pdistances_rec.second,
			     *view_rec.second);
}

bool AdminViewDeletePID::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDBase::commit(admin);

	AdminState::WriteLockRecord view_rec = get_view_write();
	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	AdminState::WriteLockRecord intradomain_routing_rec = get_intradomain_routing_write();
	AdminState::WriteLockRecord link_pdistances_rec = get_link_pdistances_write();
	ViewPtr view = boost::dynamic_pointer_cast<View>(view_rec.first);

	/* The pid returned by lookup_pid() is the actual one
	 * being used in the datastructures, so be sure to remove
	 * using a copy of it. */
	return view->remove_pid(p4p::PID(lookup_pid(get_pid())),
				*aggregation_rec.second,
				*intradomain_routing_rec.second,
				*link_pdistances_rec.second,
				*view_rec.second);
}

bool AdminViewAddPIDPrefixes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDPrefixBase::commit(admin);

	AdminState::WriteLockRecord prefixes_rec = get_prefixes_write();
	PIDMapPtr prefixes = boost::dynamic_pointer_cast<PIDMap>(prefixes_rec.first);

	/* Add the prefixes */
	const p4p::PID&  pid = lookup_pid(get_pid());
	BOOST_FOREACH(const p4p::IPPrefix& prefix, get_prefixes())
	{
		prefixes->add(prefix, pid, *prefixes_rec.second);
	}
	return true;
}

bool AdminViewDeletePIDPrefixes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDPrefixBase::commit(admin);

	AdminState::WriteLockRecord prefixes_rec = get_prefixes_write();
	PIDMapPtr prefixes = boost::dynamic_pointer_cast<PIDMap>(prefixes_rec.first);

	/* Remove the prefixes */
	const p4p::PID&  pid = lookup_pid(get_pid());
	BOOST_FOREACH(const p4p::IPPrefix& prefix, get_prefixes())
	{
		prefixes->remove(prefix, pid, *prefixes_rec.second);
	}
	return true;
}

bool AdminViewClearPIDPrefixes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDPrefixBase::commit(admin);

	AdminState::WriteLockRecord prefixes_rec = get_prefixes_write();
	PIDMapPtr prefixes = boost::dynamic_pointer_cast<PIDMap>(prefixes_rec.first);

	/* Replace existing prefixes */
	return prefixes->remove(lookup_pid(get_pid()), *prefixes_rec.second);
}

bool AdminViewAddPIDNodes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDNodesBase::commit(admin);

	const p4p::PID&  pid = lookup_pid(get_pid());
	if (pid.is_invalid())
		return false;

	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	return aggregation->add_vertices(pid, get_nodes(), *aggregation_rec.second);
}

bool AdminViewDeletePIDNodes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDNodesBase::commit(admin);

	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	return aggregation->remove_vertices(lookup_pid(get_pid()), get_nodes(), *aggregation_rec.second);
}

bool AdminViewClearPIDNodes::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDNodesBase::commit(admin);

	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	return aggregation->clear_vertices(lookup_pid(get_pid()), *aggregation_rec.second);
}

bool AdminViewSetLinkCost::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDLinkBase::commit(admin);

	AdminState::ReadLockRecord aggregation_rec = get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	AdminState::WriteLockRecord link_pdistances_rec = get_link_pdistances_write();
	SparsePIDMatrixPtr link_pdistances = boost::dynamic_pointer_cast<SparsePIDMatrix>(link_pdistances_rec.first);

	p4p::PID src, dst;
	if (!aggregation->get_link(get_link(), src, dst, *aggregation_rec.second))
		return false;

	return link_pdistances->set_by_pid(src, dst, get_cost(), *link_pdistances_rec.second);
}

bool AdminViewSetLinkWeight::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDLinkBase::commit(admin);

	AdminState::ReadLockRecord aggregation_rec = get_aggregation_read();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);
	AdminState::WriteLockRecord intradomain_routing_rec = get_intradomain_routing_write();
	PIDRoutingPtr intradomain_routing = boost::dynamic_pointer_cast<PIDRouting>(intradomain_routing_rec.first);

	p4p::PID src, dst;
	if (!aggregation->get_link(get_link(), src, dst, *aggregation_rec.second))
		return false;

	return intradomain_routing->set_weight(src, dst, get_weight(), *intradomain_routing_rec.second);
}

bool AdminViewPropGet::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPropBase::commit(admin);

	AdminState::ReadLockRecord view_rec = get_view_read();
	ViewPtr view = boost::dynamic_pointer_cast<View>(view_rec.first);

	if (get_prop() == "default_interpid")
		set_value(boost::lexical_cast<std::string>(view->get_default_interpid_pdistance(*view_rec.second)));
	else if (get_prop() == "default_intrapid")
		set_value(boost::lexical_cast<std::string>(view->get_default_intrapid_pdistance(*view_rec.second)));
	else if (get_prop() == "default_pidlink")
		set_value(boost::lexical_cast<std::string>(view->get_default_pidlink_pdistance(*view_rec.second)));
	else if (get_prop() == "default_interdomain")
		set_value(boost::lexical_cast<std::string>(view->get_default_interdomain_pdistance(*view_rec.second)));
	else if (get_prop() == "interdomain_includes_intradomain")
		set_value(boost::lexical_cast<std::string>(view->get_interdomain_includes_intradomain(*view_rec.second)));
	else if (get_prop() == "update_interval")
		set_value(boost::lexical_cast<std::string>(view->get_update_interval(*view_rec.second)));
	else if (get_prop() == "pid_ttl")
		set_value(boost::lexical_cast<std::string>(view->get_pid_ttl(*view_rec.second)));
	else if (get_prop() == "pdistance_ttl")
		set_value(boost::lexical_cast<std::string>(view->get_pdistance_ttl(*view_rec.second)));
	else
		return false;

	prop_valid();
	return true;
}

bool AdminViewPropSet::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPropBase::commit(admin);

	AdminState::WriteLockRecord view_rec = get_view_write();
	ViewPtr view = boost::dynamic_pointer_cast<View>(view_rec.first);

	try
	{
		if (get_prop() == "default_interpid")
			view->set_default_interpid_pdistance(boost::lexical_cast<double>(get_value()), *view_rec.second);
		else if (get_prop() == "default_intrapid")
			view->set_default_intrapid_pdistance(boost::lexical_cast<double>(get_value()), *view_rec.second);
		else if (get_prop() == "default_pidlink")
			view->set_default_pidlink_pdistance(boost::lexical_cast<double>(get_value()), *view_rec.second);
		else if (get_prop() == "default_interdomain")
			view->set_default_interdomain_pdistance(boost::lexical_cast<double>(get_value()), *view_rec.second);
		else if (get_prop() == "interdomain_includes_intradomain")
			view->set_interdomain_includes_intradomain(boost::lexical_cast<bool>(get_value()), *view_rec.second);
		else if (get_prop() == "update_interval")
			view->set_update_interval(boost::lexical_cast<unsigned int>(get_value()), *view_rec.second);
		else if (get_prop() == "pid_ttl")
			view->set_pid_ttl(boost::lexical_cast<unsigned int>(get_value()), *view_rec.second);
		else if (get_prop() == "pdistance_ttl")
			view->set_pdistance_ttl(boost::lexical_cast<unsigned int>(get_value()), *view_rec.second);
		else
			return false;

		prop_valid();
		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		return false;
	}
}

bool AdminViewAddPIDLink::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDLinkBase::commit(admin);

	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	return aggregation->define_link(get_link(), lookup_pid(get_src()), lookup_pid(get_dst()), *aggregation_rec.second);
}

bool AdminViewDeletePIDLink::commit(AdminState* admin) throw (admin_error)
{
	AdminViewPIDLinkBase::commit(admin);

	AdminState::WriteLockRecord aggregation_rec = get_aggregation_write();
	PIDAggregationPtr aggregation = boost::dynamic_pointer_cast<PIDAggregation>(aggregation_rec.first);

	return aggregation->undefine_link(get_link(), *aggregation_rec.second);
}
