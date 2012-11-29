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


#ifndef P4P_PORTALAPI_ADMIN_TYPES_H
#define P4P_PORTALAPI_ADMIN_TYPES_H

#include <string>
#include <p4p/detail/util.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {

enum PDistanceType
{
	PDT_NONE,
	PDT_STATIC,
	PDT_DYNAMIC,
	PDT_LINK,
};

class p4p_common_cpp_EXPORT PDistanceBase
{
public:
	PDistanceBase(PDistanceType type) : type_(type) {}
	virtual ~PDistanceBase() {}
	PDistanceType get_type() const { return type_; }
	virtual std::string get_value() const = 0;
private:
	PDistanceType type_;
};

class p4p_common_cpp_EXPORT PDistanceDynamic : public PDistanceBase
{
public:
	PDistanceDynamic() : PDistanceBase(PDT_DYNAMIC) {}
	virtual ~PDistanceDynamic() {}
	virtual std::string get_value() const { return ""; };
};

class p4p_common_cpp_EXPORT PDistanceStatic : public PDistanceBase
{
public:
	PDistanceStatic(double cost) : PDistanceBase(PDT_STATIC), cost_(cost) {}
	virtual ~PDistanceStatic() {}
	double get_cost() const { return cost_; }
	virtual std::string get_value() const { return p4p::detail::p4p_token_cast<std::string>(cost_); };
private:
	double cost_;
};

class p4p_common_cpp_EXPORT PDistanceLink : public PDistanceBase
{
public:
	PDistanceLink(const std::string& link) : PDistanceBase(PDT_LINK), link_(link) {}
	virtual ~PDistanceLink() {}
	const std::string& get_link() const { return link_; }
	virtual std::string get_value() const { return link_; };
private:
	std::string link_;
};

struct p4p_common_cpp_EXPORT NetNode
{
	NetNode(const std::string& _name, bool _external) : name(_name), external(_external) {}
	std::string name;
	bool external;
};

struct p4p_common_cpp_EXPORT NetLink
{
	NetLink(const std::string& _src, const std::string& _dst) : src(_src), dst(_dst) {}
	std::string src;
	std::string dst;
};

struct p4p_common_cpp_EXPORT NamedNetLink
{
	NamedNetLink(const std::string& _name, const NetLink& _link) : name(_name), link(_link) {}
	std::string name;
	NetLink link;
};

struct p4p_common_cpp_EXPORT NamedPID
{
	NamedPID(const std::string& _name, const PID& _pid) : name(_name), pid(_pid) {}
	std::string name;
	PID pid;
};

struct p4p_common_cpp_EXPORT PIDNameLink
{
	PIDNameLink(const std::string& _src, const std::string& _dst) : src(_src), dst(_dst) {}
	std::string src;
	std::string dst;
};

struct p4p_common_cpp_EXPORT NamedPIDLink
{
	NamedPIDLink(const std::string& _name, const PIDNameLink& _link) : name(_name), link(_link) {}
	std::string name;
	PIDNameLink link;
};

}; // namespace portal
}; // namespace protocol
}; // namespace p4p

#endif

