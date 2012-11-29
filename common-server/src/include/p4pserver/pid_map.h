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


#ifndef PID_MAP_H
#define PID_MAP_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <p4p/pid.h>
#include <p4p/ip_addr.h>
#include <p4p/detail/patricia_trie.h>
#include <p4pserver/local_obj.h>
#include <p4pserver/dist_obj.h>
#include <p4pserver/compiler.h>

class PIDMap;
typedef boost::shared_ptr<PIDMap> PIDMapPtr;
typedef boost::shared_ptr<const PIDMap> PIDMapConstPtr;

class p4p_common_server_EXPORT PIDMap : public DistributedObject
{
public:
	PIDMap();
	virtual ~PIDMap();

	bool add(const std::string& address, unsigned int prefix_length, const p4p::PID&  pid, const WritableLock& lock);
	bool add(const p4p::IPPrefix& prefix, const p4p::PID&  pid, const WritableLock& lock);
	bool remove(const std::string& address, unsigned int prefix_length, const p4p::PID&  pid, const WritableLock& lock);
	bool remove(const p4p::IPPrefix& prefix, const p4p::PID&  pid, const WritableLock& lock);
	bool remove(const p4p::PID&  pid, const WritableLock& lock);
	void clear(const WritableLock& lock);

	const p4p::PID* lookup(int addr_family, const void* addr_struct, unsigned int prefix_length, const ReadableLock& lock) const;
	const p4p::PID* lookup(const p4p::IPPrefix& address, const ReadableLock& lock) const;
	const p4p::PID* lookup(const char* address, const ReadableLock& lock) const;
	const p4p::PID* lookup(const std::string& address, const ReadableLock& lock) const { return lookup(address.c_str(), lock); }

	bool has_prefixes(const p4p::PID&  pid, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return tree_.has_prefixes(pid);
	}

	void get_prefixes(const p4p::PID&  pid, p4p::IPPrefixVector& result, const ReadableLock& lock) const;

	void enumerate(std::vector<std::pair<p4p::PID, std::vector<p4p::IPPrefix> > >& result, const ReadableLock& lock) const;

	template <class OutputIterator>
	void enumerate_pids(OutputIterator out, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		tree_.enumerate_values(out);
	}

protected:
	virtual LocalObjectPtr create_empty_instance() const { return PIDMapPtr(new PIDMap()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
#endif

private:
	struct p4p::detail::PatriciaTrie<p4p::PID> tree_;
};

#endif
