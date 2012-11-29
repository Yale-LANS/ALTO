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


#ifndef PID_H
#define PID_H

#ifdef P4P_CLUSTER
	#include <boost/serialization/access.hpp>
#endif
#include <string>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <limits.h>
#include <p4p/isp_id.h>
#include <p4p/detail/util.h>
#include <p4p/detail/compiler.h>

namespace p4p {

/**
 * PID data type.  
 * This data type may be used as an opaque datatype for a PID.  
 * It supports common operators.
 */
class p4p_common_cpp_EXPORT PID
{
public:
	static const unsigned int MAX_ISP_LEN = 32;

	/**
	 * Construct PID from three parts: ISP identifier, PID index number, and 'external' indicator
	 */
	PID(const ISPID& isp = "", unsigned int num = 0, bool external = false)
		: isp_(isp),
		  num_(num),
		  external_(external)
	{}

	static const PID DEFAULT;  /*!< Default PID mapping to the whole IP address space */
	static const PID INVALID;  /*!< Invalid PID */

	bool get_external() const		{ return external_; }
	const ISPID& get_isp() const		{ return isp_; }
	unsigned int get_num() const		{ return num_; }

	bool is_default() const			{ return *this == DEFAULT; }
	bool is_invalid() const			{ return *this == INVALID; }

	bool operator<(const PID& rhs) const
	{
		if (external_ != rhs.external_)
			return external_ < rhs.external_;

		return (num_ < rhs.num_)
		    || (num_ == rhs.num_ && isp_ < rhs.isp_);
	}

	bool operator==(const PID& rhs) const
	{
		return external_ == rhs.external_
		    && num_ == rhs.num_
		    && isp_ == rhs.isp_;
	}

	bool operator>(const PID& rhs) const	{ return rhs < *this; }
	bool operator!=(const PID& rhs) const	{ return !(*this == rhs); }

private:
#ifdef P4P_CLUSTER
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & isp_;
		ar & num_;
		ar & external_;
	}
#endif

	ISPID isp_;			/**< ISP Identifier */
	unsigned int num_;		/**< Number defining the PID within the ISP */
	bool external_;			/**< Indicator specifying whether the PID is intradomain or interdomain */

};

p4p_common_cpp_EXPORT std::ostream& operator<<(std::ostream& os, const PID& rhs);
p4p_common_cpp_EXPORT std::istream& operator>>(std::istream& is, PID& rhs);

p4p_common_cpp_EXPORT std::size_t hash_value(const PID& pid);

typedef std::vector<PID> PIDVector;
typedef std::list<PID> PIDList;
typedef std::set<PID> PIDSet;
typedef std::pair<PID, PID> PIDLink;
typedef std::vector<PIDLink> PIDLinkVector;
typedef std::string PIDLinkName;

#define PIDCStr(pid)		( p4p::detail::p4p_token_cast<std::string>(pid).c_str() )

}; // namespace p4p

#endif
