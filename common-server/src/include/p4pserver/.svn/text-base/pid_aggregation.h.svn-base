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


#ifndef PID_AGGREGATION_H
#define PID_AGGREGATION_H

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <map>
#include <p4p/pid.h>
#include <p4pserver/local_obj.h>
#include <p4pserver/net_state.h>
#include <p4pserver/compiler.h>

class PIDRouting;
class PIDMap;

class PIDAggregation;
typedef boost::shared_ptr<PIDAggregation> PIDAggregationPtr;
typedef boost::shared_ptr<const PIDAggregation> PIDAggregationConstPtr;

class p4p_common_server_EXPORT PIDAggregation : public DistributedObject
{
public:
	struct PIDRecord
	{
		PIDRecord() {}
		PIDRecord(const std::string& _name, const p4p::PID&  _pid, const NetVertexNameSet& _vertices)
		: name(_name),
		  pid(_pid),
		  vertices(_vertices)
		{}

		std::string		name;
		p4p::PID		pid;
		NetVertexNameSet	vertices;

#ifdef P4P_CLUSTER
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & name;
			ar & pid;
			ar & vertices;
		}
#endif
	};

	typedef boost::multi_index::multi_index_container<
		PIDRecord,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique< boost::multi_index::member<PIDRecord, std::string, &PIDRecord::name> >,
			boost::multi_index::ordered_unique< boost::multi_index::member<PIDRecord, p4p::PID, &PIDRecord::pid> >
		>
	> PIDRecords;
	typedef PIDRecords::nth_index<0>::type PIDRecordsByName;
	typedef PIDRecords::nth_index<1>::type PIDRecordsByPID;

	struct LinkRecord
	{
		LinkRecord() {}
		LinkRecord(const std::string& _name, const p4p::PID&  _src, const p4p::PID&  _dst)
		: name(_name),
		  src(_src),
		  dst(_dst)
		{}

		p4p::PIDLinkName	name;
		p4p::PID		src;
		p4p::PID		dst;

#ifdef P4P_CLUSTER
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & name;
			ar & src;
			ar & dst;
		}
#endif
	};

	typedef boost::multi_index::multi_index_container<
		LinkRecord,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique< boost::multi_index::member<LinkRecord, p4p::PIDLinkName, &LinkRecord::name> >,
			boost::multi_index::ordered_non_unique< boost::multi_index::member<LinkRecord, p4p::PID, &LinkRecord::src> >,
			boost::multi_index::ordered_non_unique< boost::multi_index::member<LinkRecord, p4p::PID, &LinkRecord::dst> >
		>
	> LinkRecords;
	typedef LinkRecords::nth_index<0>::type LinkRecordsByName;
	typedef LinkRecords::nth_index<1>::type LinkRecordsBySrc;
	typedef LinkRecords::nth_index<2>::type LinkRecordsByDst;

	PIDAggregation();
	virtual ~PIDAggregation();

	bool has(const p4p::PID&  pid, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return pid_records_by_pid_.find(pid) != pid_records_by_pid_.end();
	}

	bool has(const std::string& name, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return pid_records_by_name_.find(name) != pid_records_by_name_.end();
	}

	const p4p::PID&  lookup(const std::string& name, const ReadableLock& lock) const;
	bool reverse_lookup(const p4p::PID&  pid, std::string& result, const ReadableLock& lock) const;

	bool add(const std::string& name, const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock);
	bool remove(const p4p::PID&  pid, const WritableLock& lock);
	bool get_vertices(const p4p::PID&  pid, NetVertexNameSet& result, const ReadableLock& lock) const;
	bool set_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock);
	bool add_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock);
	bool remove_vertices(const p4p::PID&  pid, const NetVertexNameSet& vertices, const WritableLock& lock);
	bool clear_vertices(const p4p::PID&  pid, const WritableLock& lock);

	const PIDRecordsByPID&  get_pid_map(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return pid_records_by_pid_;
	}
	const LinkRecordsByName& get_link_map(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return link_records_by_name_;
	}

	bool define_link(const p4p::PIDLinkName& name, const p4p::PID&  src, const p4p::PID&  dst, const WritableLock& lock);
	bool undefine_link(const p4p::PIDLinkName& name, const WritableLock& lock);
	bool get_link(const p4p::PIDLinkName& name, p4p::PID&  src, p4p::PID&  dst, const ReadableLock& lock) const;
	bool get_link(const p4p::PIDLinkName& name, std::string& src, std::string& dst, const ReadableLock& lock) const;
	bool get_link(const p4p::PID&  src, const p4p::PID&  dst, p4p::PIDLinkName& result, const ReadableLock& lock) const;

protected:
	virtual LocalObjectPtr create_empty_instance() const { return PIDAggregationPtr(new PIDAggregation()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
#endif

private:
	PIDRecords pid_records_;
	PIDRecordsByName& pid_records_by_name_;
	PIDRecordsByPID&  pid_records_by_pid_;

	LinkRecords link_records_;
	LinkRecordsByName& link_records_by_name_;
	LinkRecordsBySrc& link_records_by_src_;
	LinkRecordsByDst& link_records_by_dst_;
};

#endif
