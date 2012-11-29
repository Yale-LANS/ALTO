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


#ifndef P4P_PORTALAPI_ADMIN_H
#define P4P_PORTALAPI_ADMIN_H

#include <string>
#include <p4p/protocol/protobase.h>
#include <p4p/protocol-portal/metainfo.h>
#include <p4p/protocol-portal/admin_exceptions.h>
#include <p4p/protocol-portal/admin_types.h>
#include <p4p/protocol-portal/pdistance_matrix.h>
#include <p4p/protocol-portal/detail/parsing.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {

class p4p_common_cpp_EXPORT AdminPortalProtocol : public P4PProtocol
{
public:

	AdminPortalProtocol(const std::string& host, unsigned short port = 6672, bool peristent = true) throw (std::runtime_error);
	virtual ~AdminPortalProtocol();

	/*** Client calls ***/
	template <class InputIterator, class OutputIterator>
	void get_pids(const std::string& view, InputIterator address_first, InputIterator address_last, OutputIterator result, P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::portal::detail::ResponseAddressPIDReader<OutputIterator> reader(result);
		p4p::protocol::detail::RequestCollectionWriter<InputIterator> writer(address_first, address_last);
		make_request("POST", "pid?view=" + url_escape(view) + "&admin=" + admin_token_, reader, writer);
		if (meta)
			meta->assign(reader);
	}

	template <class InputIterator, class OutputIterator>
	void get_pidmap(const std::string& view, InputIterator pid_first, InputIterator pid_last, OutputIterator result, P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::portal::detail::ResponsePIDMapReader<OutputIterator> reader(result);
		p4p::protocol::detail::RequestCollectionWriter<InputIterator> writer(pid_first, pid_last);
		make_request("GET", "pid/map?view=" + url_escape(view) + "&admin=" + admin_token_, reader, writer);
		if (meta)
			meta->assign(reader);
	}

	template <class InputIterator>
	void get_pdistance(const std::string& view,
			   InputIterator pids_first, InputIterator pids_last,
			   bool reverse,
			   PDistanceMatrix& result,
			   P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::portal::detail::ResponsePDistanceReader reader(result);
		p4p::protocol::portal::detail::RequestPDistanceWriter<InputIterator> writer(pids_first, pids_last, reverse);
		make_request("POST", "pdistance?view=" + url_escape(view) + "&admin=" + admin_token_, reader, writer);
		if (meta)
			meta->assign(reader);
	}

	/*** Admin transaction management ***/
	void admin_begin_txn() throw (P4PProtocolError);
	void admin_commit_txn() throw (P4PProtocolError);
	void admin_cancel_txn() throw (P4PProtocolError);
	bool admin_in_txn()
	{
		return !admin_token_.empty();
	}

	/*** Admin topology management ***/
	void admin_net_add_node(const std::string& name, bool external) throw (P4PProtocolError);
	void admin_net_del_node(const std::string& name) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_net_list_nodes(OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::portal::detail::ResponseNetNodeReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + "/net/node", reader);
	}

	void admin_net_add_link(const NamedNetLink& link) throw (P4PProtocolError);
	void admin_net_del_link(const std::string& name) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_net_list_links(OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::portal::detail::ResponseNamedNetLinkReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + "/net/link", reader);
	}

	/*** Admin url_escape(view) +management ***/
	void admin_view_add(const std::string& name) throw (P4PProtocolError);
	void admin_view_del(const std::string& name) throw (P4PProtocolError);

	void admin_view_set_prop(const std::string& view, const std::string& name, const std::string& value) throw (P4PProtocolError);
	std::string admin_view_get_prop(const std::string& view, const std::string& name) throw (P4PProtocolError);

	void admin_view_add_pid(const std::string& view, const NamedPID& pid) throw (P4PProtocolError);
	void admin_view_del_pid(const std::string& view, const std::string& name) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_view_list_pids(const std::string& view, OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::portal::detail::ResponseNamedPIDReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid", reader);
	}

	void admin_view_add_pidlink(const std::string& view, const NamedPIDLink& link) throw (P4PProtocolError);
	void admin_view_del_pidlink(const std::string& view, const std::string& name) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_view_list_pidlinks(const std::string& view, OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::portal::detail::ResponseNamedPIDLinkReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/link", reader);
	}

	void admin_view_add_pid_node(const std::string& view, const std::string& pid, const std::string& node) throw (P4PProtocolError);
	void admin_view_del_pid_node(const std::string& view, const std::string& pid, const std::string& node) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_view_list_pid_nodes(const std::string& view, const std::string& pid, OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::detail::ResponseTokenReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(pid) + "/nodes", reader);
	}

	void admin_view_set_pidlink_weight(const std::string& view, const std::string& link, double value) throw (P4PProtocolError);
	double admin_view_get_pidlink_weight(const std::string& view, const std::string& link) throw (P4PProtocolError);

	void admin_view_set_pidlink_pdistance(const std::string& view, const std::string& link, const PDistanceBase& value) throw (P4PProtocolError);
	PDistanceBase* admin_view_get_pidlink_pdistance(const std::string& view, const std::string& link) throw (P4PProtocolError);

	void admin_view_add_pid_prefix(const std::string& view, const std::string& name, const std::string& address, unsigned short len) throw (P4PProtocolError);
	void admin_view_del_pid_prefix(const std::string& view, const std::string& name, const std::string& address, unsigned short len) throw (P4PProtocolError);
	void admin_view_clear_pid_prefixes(const std::string& view, const std::string& name) throw (P4PProtocolError);
	template <class OutputIterator>
	void admin_view_list_pid_prefixes(const std::string& view, const std::string& name, OutputIterator result) throw (P4PProtocolError)
	{
		check_txn();
		p4p::protocol::detail::ResponseTokenReader<OutputIterator> reader(result);
		make_request("GET", "admin/" + admin_token_ + '/' + url_escape(view) + "/pid/" + url_escape(name) + "/prefixes", reader);
	}

	void admin_view_update_pdistance(const std::string& view) throw (P4PProtocolError);

private:
	/* Check that a transaction is in progress */
	void check_txn() throw (P4PProtocolError);

	/* Current token if an admin transaction is in progress */
	std::string admin_token_;
};

}; // namespace portal
}; // namespace protocol
}; // namespace p4p

#endif

