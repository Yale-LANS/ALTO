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


#ifndef ADMIN_VIEW_H
#define ADMIN_VIEW_H

#include "admin_state.h"
#include "view.h"

class admin_invalid_view : public admin_error
{
public:
	admin_invalid_view() : admin_error("Invalid view") {}
};

class admin_invalid_pid : public admin_error
{
public:
	admin_invalid_pid() : admin_error("Invalid PID") {}
};

class AdminViewsBase : public AdminAction
{
public:
	AdminViewsBase() {}
};
typedef boost::shared_ptr<AdminViewsBase> AdminViewsBasePtr;

class AdminViewBase : public AdminViewsBase
{
public:
	AdminViewBase(const std::string& view)
	: view_(view)
	{}

	const std::string& get_view() const { return view_; }

	AdminState::ReadLockRecord get_view_read() throw (admin_error);
	AdminState::WriteLockRecord get_view_write() throw (admin_error);

	AdminState::ReadLockRecord get_aggregation_read() throw (admin_error);
	AdminState::WriteLockRecord get_aggregation_write() throw (admin_error);

	AdminState::ReadLockRecord get_prefixes_read() throw (admin_error);
	AdminState::WriteLockRecord get_prefixes_write() throw (admin_error);

	AdminState::ReadLockRecord get_intradomain_routing_read() throw (admin_error);
	AdminState::WriteLockRecord get_intradomain_routing_write() throw (admin_error);

	AdminState::ReadLockRecord get_link_pdistances_read() throw (admin_error);
	AdminState::WriteLockRecord get_link_pdistances_write() throw (admin_error);

	AdminState::ReadLockRecord get_intradomain_pdistances_read() throw (admin_error);
	AdminState::WriteLockRecord get_intradomain_pdistances_write() throw (admin_error);

	AdminState::ReadLockRecord get_interdomain_pdistances_read() throw (admin_error);
	AdminState::WriteLockRecord get_interdomain_pdistances_write() throw (admin_error);

	const p4p::PID&  lookup_pid(const std::string& name) throw (admin_error);

private:
	std::string view_;
};

class AdminViewAdd : public AdminViewBase
{
public:
	AdminViewAdd(const std::string& view)
	: AdminViewBase(view)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewDelete : public AdminViewBase
{
public:
	AdminViewDelete(const std::string& view)
	: AdminViewBase(view)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewPDistanceUpdate : public AdminViewBase
{
public:
	AdminViewPDistanceUpdate(const std::string& view)
	: AdminViewBase(view)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewPIDBase : public AdminViewBase
{
public:
	AdminViewPIDBase(const std::string& view, const std::string& pid)
	: AdminViewBase(view),
	  pid_(pid)
	{}

	const std::string& get_pid() const { return pid_; }

private:
	std::string pid_;
};
typedef boost::shared_ptr<AdminViewBase> AdminViewBasePtr;

class AdminViewAddPID : public AdminViewPIDBase
{
public:
	AdminViewAddPID(const std::string& view, const std::string& pid, const p4p::PID&  pid_obj)
	: AdminViewPIDBase(view, pid),
	  pid_obj_(pid_obj)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);

	const p4p::PID&  get_pid_obj() const { return pid_obj_; }

private:
	p4p::PID pid_obj_;
};

class AdminViewDeletePID : public AdminViewPIDBase
{
public:
	AdminViewDeletePID(const std::string& view, const std::string& pid)
	: AdminViewPIDBase(view, pid)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewPIDLinkBase : public AdminViewBase
{
public:
	AdminViewPIDLinkBase(const std::string& view, const p4p::PIDLinkName& link)
	: AdminViewBase(view),
	  link_(link)
	{}

	const p4p::PIDLinkName& get_link() const { return link_; }

private:
	std::string link_;
};

class AdminViewAddPIDLink : public AdminViewPIDLinkBase
{
public:
	AdminViewAddPIDLink(const std::string& view, const p4p::PIDLinkName& link, const std::string& src, const std::string& dst)
	: AdminViewPIDLinkBase(view, link),
	  src_(src),
	  dst_(dst)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);

	const std::string& get_src() const { return src_; }
	const std::string& get_dst() const { return dst_; }

private:
	std::string src_;
	std::string dst_;
};

class AdminViewDeletePIDLink : public AdminViewPIDLinkBase
{
public:
	AdminViewDeletePIDLink(const std::string& view, const p4p::PIDLinkName& link)
	: AdminViewPIDLinkBase(view, link)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewPIDPrefixBase : public AdminViewPIDBase
{
public:
	AdminViewPIDPrefixBase(const std::string& view, const std::string& pid, const p4p::IPPrefixSet& prefixes)
	: AdminViewPIDBase(view, pid),
	  prefixes_(prefixes)
	{}

	const p4p::IPPrefixSet& get_prefixes() const { return prefixes_; }

private:
	p4p::IPPrefixSet prefixes_;
};
typedef boost::shared_ptr<AdminViewPIDPrefixBase> AdminViewPIDPrefixBasePtr;

class AdminViewAddPIDPrefixes : public AdminViewPIDPrefixBase
{
public:
	AdminViewAddPIDPrefixes(const std::string& view, const std::string& pid, const p4p::IPPrefixSet& prefixes)
	: AdminViewPIDPrefixBase(view, pid, prefixes)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewDeletePIDPrefixes : public AdminViewPIDPrefixBase
{
public:
	AdminViewDeletePIDPrefixes(const std::string& view, const std::string& pid, const p4p::IPPrefixSet& prefixes)
	: AdminViewPIDPrefixBase(view, pid, prefixes)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewClearPIDPrefixes : public AdminViewPIDPrefixBase
{
public:
	AdminViewClearPIDPrefixes(const std::string& view, const std::string& pid)
	: AdminViewPIDPrefixBase(view, pid, p4p::IPPrefixSet())
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewPIDNodesBase : public AdminViewPIDBase
{
public:
	AdminViewPIDNodesBase(const std::string& view, const std::string& pid, const NetVertexNameSet& nodes)
	: AdminViewPIDBase(view, pid),
	  nodes_(nodes)
	{}

	const NetVertexNameSet& get_nodes() const { return nodes_; }

private:
	NetVertexNameSet nodes_;
};
typedef boost::shared_ptr<AdminViewPIDNodesBase> AdminViewPIDNodesBasePtr;

class AdminViewAddPIDNodes : public AdminViewPIDNodesBase
{
public:
	AdminViewAddPIDNodes(const std::string& view, const std::string& pid, const NetVertexNameSet& nodes)
	: AdminViewPIDNodesBase(view, pid, nodes)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewDeletePIDNodes : public AdminViewPIDNodesBase
{
public:
	AdminViewDeletePIDNodes(const std::string& view, const std::string& pid, const NetVertexNameSet& nodes)
	: AdminViewPIDNodesBase(view, pid, nodes)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewClearPIDNodes : public AdminViewPIDNodesBase
{
public:
	AdminViewClearPIDNodes(const std::string& view, const std::string& pid)
	: AdminViewPIDNodesBase(view, pid, NetVertexNameSet())
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminViewSetLinkCost : public AdminViewPIDLinkBase
{
public:
	AdminViewSetLinkCost(const std::string& view, const p4p::PIDLinkName& link, double cost)
	: AdminViewPIDLinkBase(view, link),
	  cost_(cost)
	{}

	double get_cost() const { return cost_; }

	virtual bool commit(AdminState* admin) throw (admin_error);

private:
	double cost_;
};

class AdminViewSetLinkWeight : public AdminViewPIDLinkBase
{
public:
	AdminViewSetLinkWeight(const std::string& view, const p4p::PIDLinkName& link, double weight)
	: AdminViewPIDLinkBase(view, link),
	  weight_(weight)
	{}

	double get_weight() const { return weight_; }

	virtual bool commit(AdminState* admin) throw (admin_error);

private:
	double weight_;
};

class AdminViewPropBase : public AdminViewBase
{
public:
	AdminViewPropBase(const std::string& view, const std::string& prop)
	: AdminViewBase(view),
	  prop_(prop),
	  prop_valid_(false)
	{}

	const std::string& get_prop() const { return prop_; }
	bool get_prop_valid() const { return prop_valid_; }

protected:
	void prop_valid() { prop_valid_ = true; }

private:
	std::string prop_;
	bool prop_valid_;
};

class AdminViewPropGet : public AdminViewPropBase
{
public:
	AdminViewPropGet(const std::string& view, const std::string& prop)
	: AdminViewPropBase(view, prop)
	{}

	const std::string& get_value() const { return value_; }

	virtual bool commit(AdminState* admin) throw (admin_error);

protected:
	void set_value(const std::string& value) { value_ = value; }

private:
	std::string value_;
};
typedef boost::shared_ptr<AdminViewPropGet> AdminViewPropGetPtr;

class AdminViewPropSet : public AdminViewPropBase
{
public:
	AdminViewPropSet(const std::string& view, const std::string& prop, const std::string& value)
	: AdminViewPropBase(view, prop),
	  value_(value)
	{}

	const std::string& get_value() const { return value_; }

	virtual bool commit(AdminState* admin) throw (admin_error);

private:
	std::string value_;
};

#endif
