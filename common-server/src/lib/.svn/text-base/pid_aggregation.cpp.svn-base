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


#include "p4pserver/pid_aggregation.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>

PIDAggregation::PIDAggregation()
	: pid_records_by_name_(pid_records_.get<0>()),
	  pid_records_by_pid_(pid_records_.get<1>()),
	  link_records_by_name_(link_records_.get<0>()),
	  link_records_by_src_(link_records_.get<1>()),
	  link_records_by_dst_(link_records_.get<2>())
{
	after_construct();
}

PIDAggregation::~PIDAggregation()
{
	before_destruct();
}

#ifdef P4P_CLUSTER
void PIDAggregation::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	stream >> pid_records_;

	changed(lock);
}

void PIDAggregation::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	stream << pid_records_;
}
#endif

DistributedObjectPtr PIDAggregation::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());

	PIDAggregationPtr new_obj = boost::dynamic_pointer_cast<PIDAggregation>(PIDAggregation::create_empty_instance());
	new_obj->pid_records_ = pid_records_;
	return new_obj;
}

const p4p::PID&  PIDAggregation::lookup(const std::string& name, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	PIDRecordsByName::iterator itr = pid_records_by_name_.find(name);
	if (itr == pid_records_by_name_.end())
		return p4p::PID::INVALID;

	return itr->pid;
}

bool PIDAggregation::reverse_lookup(const p4p::PID&  pid, std::string& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	result = itr->name;
	return true;
}

bool PIDAggregation::add(const std::string& name, const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock)
{
	lock.check_read(get_local_mutex());

	if (has(pid, lock))
		return false;

	if (has(name, lock))
		return false;

	pid_records_.insert(PIDRecord(name, pid, vertices));

	changed(lock);
	return true;
}

bool PIDAggregation::remove(const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	pid_records_by_pid_.erase(itr);

	/* Also remove links with this p4p::PID* */
	link_records_by_src_.erase(pid);
	link_records_by_dst_.erase(pid);

	changed(lock);

	return true;
}

bool PIDAggregation::get_vertices(const p4p::PID&  pid, NetVertexNameSet& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	PIDRecordsByPID::const_iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	result = itr->vertices;
	return true;
}

bool PIDAggregation::set_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	PIDRecord r(itr->name, itr->pid, vertices);
	pid_records_by_pid_.erase(itr);
	pid_records_.insert(r);

	changed(lock);
	return true;
}

bool PIDAggregation::add_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	PIDRecord r(itr->name, itr->pid, itr->vertices);
	std::copy(vertices.begin(), vertices.end(), std::inserter(r.vertices, r.vertices.end()));
	pid_records_by_pid_.erase(itr);
	pid_records_.insert(r);

	changed(lock);
	return true;
}

bool PIDAggregation::remove_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	PIDRecord r(itr->name, itr->pid, itr->vertices);
	BOOST_FOREACH(const NetVertexName& v, vertices)
	{
		r.vertices.erase(v);
	}
	pid_records_by_pid_.erase(itr);
	pid_records_.insert(r);

	changed(lock);
	return true;
}

bool PIDAggregation::clear_vertices(const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	PIDRecordsByPID::iterator itr = pid_records_by_pid_.find(pid);
	if (itr == pid_records_by_pid_.end())
		return false;

	PIDRecord r(itr->name, itr->pid, NetVertexNameSet());
	pid_records_by_pid_.erase(itr);
	pid_records_.insert(r);

	changed(lock);
	return true;
}

bool PIDAggregation::define_link(const p4p::PIDLinkName& name, const p4p::PID&  src, const p4p::PID&  dst, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Ensure link name doesn't already exist */
	if (link_records_by_name_.find(name) != link_records_by_name_.end())
		return false;

	link_records_.insert(LinkRecord(name, src, dst));

	changed(lock);
	return true;
}

bool PIDAggregation::undefine_link(const p4p::PIDLinkName& name, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	LinkRecordsByName::iterator itr = link_records_by_name_.find(name);
	if (itr == link_records_by_name_.end())
		return false;

	link_records_by_name_.erase(itr);

	changed(lock);
	return true;
}

bool PIDAggregation::get_link(const p4p::PIDLinkName& name, p4p::PID&  src, p4p::PID&  dst, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	LinkRecordsByName::const_iterator itr = link_records_by_name_.find(name);
	if (itr == link_records_by_name_.end())
		return false;

	src = itr->src;
	dst = itr->dst;
	return true;
}

bool PIDAggregation::get_link(const p4p::PIDLinkName& name, std::string& src, std::string& dst, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	p4p::PID src_pid, dst_pid;
	if (!get_link(name, src_pid, dst_pid, lock))
		return false;

	return (reverse_lookup(src_pid, src, lock) && reverse_lookup(dst_pid, dst, lock));
}

bool PIDAggregation::get_link(const p4p::PID&  src, const p4p::PID& dst, p4p::PIDLinkName& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	std::pair<LinkRecordsBySrc::const_iterator, LinkRecordsBySrc::const_iterator> range = link_records_by_src_.equal_range(src);
	for (LinkRecordsBySrc::const_iterator itr = range.first; itr != range.second; ++itr)
	{
		if (itr->dst != dst)
			continue;

		result = itr->name;
		return true;
	}

	return false;
}

