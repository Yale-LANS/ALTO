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


#include "p4pserver/pid_map.h"

#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <limits.h>

PIDMap::PIDMap()
{
	after_construct();
}

PIDMap::~PIDMap()
{
	before_destruct();
}

#ifdef P4P_CLUSTER
void PIDMap::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	clear(lock);

	/* Read the entries */
	unsigned int num_entries;
	p4p::PID* pid;
	p4p::IPPrefix prefix;
	stream >> num_entries;
	for (unsigned int i = 0; i < num_entries; ++i)
	{
		stream >> pid;
		stream >> prefix;
		add(prefix, pid, lock);
	}

	changed(lock);
}

void PIDMap::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	typedef std::pair<PID, std::vector<p4p::IPPrefix> > Entry;

	lock.check_read(get_local_mutex());

	std::vector<Entry> entries;
	tree_.enumerate(entries);

	/* Count the number of entries */
	unsigned int num_entries = 0;
	BOOST_FOREACH(const Entry& entry, entries)
	{
		num_entries += entry.second.size();
	}
	stream << num_entries;
	
	/* Write out the entries */
	BOOST_FOREACH(const Entry& entry, entries)
	{
		BOOST_FOREACH(const p4p::IPPrefix& prefix, entry.second)
		{
			stream << entry.first;
			stream << prefix;
		}
	}
}
#endif

bool PIDMap::add(const std::string& address, unsigned int prefix_length, const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	p4p::IPPrefix prefix(address, prefix_length);
	if (prefix == p4p::IPPrefix::INVALID)
		return false;

	return add(prefix, pid, lock);
}

bool PIDMap::add(const p4p::IPPrefix& address, const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	bool res = tree_.add(address, pid);
	
	if (res)
		changed(lock);
	
	return res;
}

bool PIDMap::remove(const p4p::IPPrefix& address, const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	bool res = tree_.remove(address, pid);

	if (res)
		changed(lock);

	return res;
}

bool PIDMap::remove(const std::string& address, unsigned int prefix_length, const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	p4p::IPPrefix prefix(address, prefix_length);
	if (prefix == p4p::IPPrefix::INVALID)
		return false;

	return remove(prefix, pid, lock);
}

void PIDMap::clear(const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	tree_.clear();
	changed(lock);
}

bool PIDMap::remove(const p4p::PID&  pid, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	
	bool res = tree_.remove(pid);
	
	if (res)
		changed(lock);

	return res;
}

const p4p::PID* PIDMap::lookup(const char* address, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	p4p::IPPrefix prefix(address);
	if (prefix == p4p::IPPrefix::INVALID)
		return NULL;

	return tree_.lookup(prefix);
}

const p4p::PID* PIDMap::lookup(const p4p::IPPrefix& address, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return tree_.lookup(address);
}

const p4p::PID* PIDMap::lookup(int addr_family, const void* addr_struct, unsigned int prefix_length, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	return tree_.lookup(p4p::IPPrefix(addr_family, addr_struct, prefix_length));
}

void PIDMap::get_prefixes(const p4p::PID&  pid, p4p::IPPrefixVector& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	tree_.get_prefixes(pid, result);
}

void PIDMap::enumerate(std::vector<std::pair<p4p::PID, std::vector<p4p::IPPrefix> > >& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	tree_.enumerate(result);
}

DistributedObjectPtr PIDMap::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());

	PIDMapPtr new_obj = boost::dynamic_pointer_cast<PIDMap>(PIDMap::create_empty_instance());
	new_obj->tree_.copy_from(tree_);
	return new_obj;
}
