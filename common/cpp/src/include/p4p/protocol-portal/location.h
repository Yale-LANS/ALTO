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


#ifndef P4P_PORTALAPI_LOCATION_H
#define P4P_PORTALAPI_LOCATION_H

#include <string>
#include <p4p/protocol/protobase.h>
#include <p4p/protocol-portal/detail/parsing.h>
#include <p4p/protocol-portal/metainfo.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {

class p4p_common_cpp_EXPORT LocationPortalProtocol : public P4PProtocol
{
public:

	LocationPortalProtocol(const std::string& host, unsigned short port = 6671, const std::string& view = "DEFAULT", bool persistent = false) throw (std::runtime_error);
	virtual ~LocationPortalProtocol();

	/**
	 * Call Location Portal Service's GetPIDs function to map a set of IP address into PIDs. It is
	 * also possible to call this function using an empty address list to obtain the IP address and
	 * PID of the requesting client.
	 *
	 * @param address_first Begin iterator for collection of input IP addresses. Address collection
	 * 	is a collection of std::string, so '*address_first' must be of type std::string.
	 * @param address_last End iterator for collection of input IP addresses. Address collection
	 * 	is a collection of std::string, so '*address_first' must be of type std::string.
	 * @param result Output iterator into which results are placed.  The output iterator
	 * 	must allow assignment from type std::pair<std::string, PID>, where
	 * 	pair.first is the IP address and pair.second is the corresponding PID.
	 * @param meta Output parameter into which the meta information (e.g., TTL) of the returned data
	 * 	is placed. Ignored if NULL.
	 */
	template <class InputIterator, class OutputIterator>
	void get_pids(InputIterator address_first, InputIterator address_last, OutputIterator result, P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		detail::ResponseAddressPIDReader<OutputIterator> reader(result);
		p4p::protocol::detail::RequestCollectionWriter<InputIterator> writer(address_first, address_last);
		make_request("POST", "pid" + (view_ != "DEFAULT" ? "?view=" + url_escape(view_) : ""), reader, writer);
		if (meta)
			meta->assign(reader);
	}

	/**
	 * Call Location Portal Service's GetPIDMap function to retrieve mapping from PIDs to IP
	 * prefixes for the requested set of PIDs.  It is also possible to call this function
	 * using an empty PID list to obtain the full PID Map.
	 *
	 * @param pid_first Begin iterator for collection of PIDs. PID collection is a collection
	 * 	of PID, so '*pid_first' must be of type PID.
	 * @param pid_last End iterator for collection of PIDs. PID collection is a collection
	 * 	of PID, so '*pid_last' must be of type PID.
	 * @param result Output iterator into which results are placed.  The output iterator must
	 * 	allow assignment from type PIDPrefixes, where a PIDPrefixes object specifies
	 * 	a PID object and its corresponding collection of IPPrefix objects.
	 * @param meta Output parameter into which the meta information (e.g., TTL) of the returned data
	 * 	is placed. Ignored if NULL.
	 */
	template <class InputIterator, class OutputIterator>
	void get_pidmap(InputIterator pid_first, InputIterator pid_last, OutputIterator result, P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		detail::ResponsePIDMapReader<OutputIterator> reader(result);
		p4p::protocol::detail::RequestCollectionWriter<InputIterator> writer(pid_first, pid_last);
		make_request("GET", "pid/map" + (view_ != "DEFAULT" ? "?view=" + url_escape(view_) : ""), reader, writer);
		if (meta)
			meta->assign(reader);
	}

private:
	std::string view_;

};

}; // namespace portal
}; // namespace protocol
}; // namespace p4p


#endif

