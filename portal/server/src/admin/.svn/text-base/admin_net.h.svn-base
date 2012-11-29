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


#ifndef ADMIN_NET_H
#define ADMIN_NET_H

#include "admin_state.h"

class AdminNetBase : public AdminAction
{
public:
	AdminNetBase() {}
};
typedef boost::shared_ptr<AdminNetBase> AdminNetBasePtr;

class AdminNetNodeBase : public AdminNetBase
{
public:
	AdminNetNodeBase(const std::string& node)
	: AdminNetBase(),
	  node_(node)
	{}

	const std::string& get_node() const { return node_; }

private:
	std::string node_;
};

class AdminNetNodeAdd : public AdminNetNodeBase
{
public:
	AdminNetNodeAdd(const std::string& node, bool external)
	: AdminNetNodeBase(node), external_(external)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);

	bool get_external() const { return external_; }

private:
	bool external_;
};

class AdminNetNodeDelete : public AdminNetNodeBase
{
public:
	AdminNetNodeDelete(const std::string& node)
	: AdminNetNodeBase(node)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

class AdminNetLinkBase : public AdminNetBase
{
public:
	AdminNetLinkBase(const std::string& link)
	: AdminNetBase(),
	  link_(link)
	{}

	const std::string& get_link() const { return link_; }

private:
	std::string link_;
};

class AdminNetLinkAdd : public AdminNetLinkBase
{
public:
	AdminNetLinkAdd(const std::string& node, const std::string& src, const std::string& dst)
	: AdminNetLinkBase(node),
	  src_(src),
	  dst_(dst)
	{}

	const std::string& get_src() const { return src_; }
	const std::string& get_dst() const { return dst_; }

	virtual bool commit(AdminState* admin) throw (admin_error);

private:
	std::string src_;
	std::string dst_;
};

class AdminNetLinkDelete : public AdminNetLinkBase
{
public:
	AdminNetLinkDelete(const std::string& link)
	: AdminNetLinkBase(link)
	{}

	virtual bool commit(AdminState* admin) throw (admin_error);
};

#endif
