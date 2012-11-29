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


#include "p4p/protocol-portal/admin.h"

#include <p4p/detail/util.h>
#include <p4p/protocol/detail/dataconv.h>

#ifdef WIN32
	#define strcasecmp	_strcmpi
#endif

namespace p4p {
namespace protocol {
namespace portal {

AdminPortalProtocol::AdminPortalProtocol(const std::string& host, unsigned short port, bool persistent) throw (std::runtime_error)
	: P4PProtocol(host, port, persistent)
{
}

AdminPortalProtocol::~AdminPortalProtocol()
{
}

void AdminPortalProtocol::check_txn() throw (P4PProtocolError)
{
	if (admin_token_.empty())
		throw AdminPortalProtocolNoTxnError();
}

void AdminPortalProtocol::admin_begin_txn() throw (P4PProtocolError)
{
	if (!admin_token_.empty())
		throw AdminPortalProtocolDupTxnError();

	p4p::protocol::detail::ResponseSingleTokenReader reader;
	make_request("GET", "admin", reader);
	admin_token_ = reader.get_token();
}

void AdminPortalProtocol::admin_commit_txn() throw (P4PProtocolError)
{
	check_txn();
	make_request("POST", "admin/" + admin_token_);
	admin_token_.clear();
}

void AdminPortalProtocol::admin_cancel_txn() throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_);
	admin_token_.clear();
}

void AdminPortalProtocol::admin_net_add_node(const std::string& name, bool external) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + "/net/node/" + url_escape(name) + (external ? "?ext=1" : ""));
}

void AdminPortalProtocol::admin_net_del_node(const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + "/net/node/" + url_escape(name));
}

void AdminPortalProtocol::admin_net_add_link(const NamedNetLink& link) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + "/net/link/" + url_escape(link.name) + "?src=" + url_escape(link.link.src) + "&dst=" + url_escape(link.link.dst));
}

void AdminPortalProtocol::admin_net_del_link(const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + "/net/link/" + url_escape(name));
}

void AdminPortalProtocol::admin_view_add(const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(name));
}

void AdminPortalProtocol::admin_view_del(const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(name));
}

void AdminPortalProtocol::admin_view_set_prop(const std::string& view, const std::string& name, const std::string& value) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/prop/" + url_escape(name) + "?v=" + url_escape(value));
}

std::string AdminPortalProtocol::admin_view_get_prop(const std::string& view, const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	p4p::protocol::detail::ResponseSingleTokenReader reader;
	make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/prop/" + url_escape(name), reader);
	return reader.get_token();
}

void AdminPortalProtocol::admin_view_add_pid(const std::string& view, const NamedPID& pid) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(pid.name) + "?v=" + p4p::detail::p4p_token_cast<std::string>(pid.pid));
}

void AdminPortalProtocol::admin_view_del_pid(const std::string& view, const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(name));
}

void AdminPortalProtocol::admin_view_add_pid_node(const std::string& view, const std::string& pid, const std::string& node) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(pid) + "/nodes/" + url_escape(node));
}

void AdminPortalProtocol::admin_view_del_pid_node(const std::string& view, const std::string& pid, const std::string& node) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(pid) + "/nodes/" + url_escape(node));
}

void AdminPortalProtocol::admin_view_add_pidlink(const std::string& view, const NamedPIDLink& link) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(link.name) + "?src=" + url_escape(link.link.src) + "&dst=" + url_escape(link.link.dst));
}

void AdminPortalProtocol::admin_view_del_pidlink(const std::string& view, const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(name));
}


void AdminPortalProtocol::admin_view_set_pidlink_weight(const std::string& view, const std::string& link, double value) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(link) + "/weight?v=" + p4p::detail::p4p_token_cast<std::string>(value));
}

double AdminPortalProtocol::admin_view_get_pidlink_weight(const std::string& view, const std::string& link) throw (P4PProtocolError)
{
	check_txn();
	p4p::protocol::detail::ResponseSingleTokenReader reader;
	make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(link) + "/weight", reader);
	try
	{
		return p4p::protocol::detail::to_double(reader.get_token());
	}
	catch (std::invalid_argument& e)
	{
		throw P4PProtocolParseError(e.what());
	}
}

void AdminPortalProtocol::admin_view_set_pidlink_pdistance(const std::string& view, const std::string& link, const PDistanceBase& value) throw (P4PProtocolError)
{
	check_txn();
	std::string value_str;
	switch (value.get_type())
	{
	case PDT_STATIC:
		value_str = value.get_value();
		break;
	case PDT_DYNAMIC:
		value_str = "dynamic";
		break;
	default:
		throw P4PProtocolError("Invalid PDistance type");
	}

	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(link) + "/pdistance?v=" + value_str);
}

PDistanceBase* AdminPortalProtocol::admin_view_get_pidlink_pdistance(const std::string& view, const std::string& link) throw (P4PProtocolError)
{
	check_txn();
	p4p::protocol::detail::ResponseSingleTokenReader reader;
	make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/link/" + url_escape(link) + "/pdistance", reader);
	if (strcasecmp(reader.get_token().c_str(), "dynamic") == 0)
	{
		return new PDistanceDynamic();
	}
	else
	{
		try
		{
			return new PDistanceStatic(p4p::protocol::detail::to_double(reader.get_token()));
		}
		catch (std::invalid_argument& e)
		{
			throw P4PProtocolParseError(e.what());
		}
	}
}

void AdminPortalProtocol::admin_view_add_pid_prefix(const std::string& view, const std::string& name, const std::string& address, unsigned short len) throw (P4PProtocolError)
{
	check_txn();
	make_request("PUT", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(name) + "/prefixes/" + url_escape(address) + "?len=" + p4p::detail::p4p_token_cast<std::string>(len));
}

void AdminPortalProtocol::admin_view_del_pid_prefix(const std::string& view, const std::string& name, const std::string& address, unsigned short len) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(name) + "/prefixes/" + url_escape(address) + "?len=" + p4p::detail::p4p_token_cast<std::string>(len));
}

void AdminPortalProtocol::admin_view_clear_pid_prefixes(const std::string& view, const std::string& name) throw (P4PProtocolError)
{
	check_txn();
	make_request("DELETE", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(name) + "/prefixes");
}

void AdminPortalProtocol::admin_view_update_pdistance(const std::string& view) throw (P4PProtocolError)
{
	check_txn();
	make_request("POST", "admin/" + admin_token_ + '/' + url_escape(view) + "/pdistance");
}

};
};
};
