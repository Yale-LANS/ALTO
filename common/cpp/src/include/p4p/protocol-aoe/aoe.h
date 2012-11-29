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


#ifndef P4P_AOEProtocol_CPP_H
#define P4P_AOEProtocol_CPP_H

#include <string>
#include <p4p/protocol/protobase.h>
#include <p4p/protocol/metainfo.h>
#include <p4p/protocol-aoe/detail/parsing.h>
#include <p4p/protocol-aoe/weights.h>
#include <p4p/protocol-aoe/swarm_state.h>
#include <p4p/protocol-aoe/pid_class_pcts.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace aoe {

class p4p_common_cpp_EXPORT AOEProtocol : public P4PProtocol
{
public:

	AOEProtocol(const std::string& host, unsigned short port = 6673, bool peristent = true) throw (std::runtime_error);
	virtual ~AOEProtocol();

	template <class InputIterator>
	void get_weights(const std::string& portal, InputIterator pid_begin, InputIterator pid_end, WeightsMap& result_weights, PIDClassPercentages& result_pcts, P4PProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::detail::RequestCollectionWriter<InputIterator> writer(pid_begin, pid_end);
		p4p::protocol::aoe::detail::ResponseWeightsReader reader(result_weights, result_pcts);
		make_request("POST", "filesharing/generic?portal=" + url_escape(portal), reader, writer);
		if (meta)
			meta->assign(reader);
	}

	void get_weights(const std::string& portal, const SwarmState& state, WeightsMap& result_weights, PIDClassPercentages& result_pcts, P4PProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::aoe::detail::RequestStateWriter writer(state);
		p4p::protocol::aoe::detail::ResponseWeightsReader reader(result_weights, result_pcts);
		make_request("POST", "filesharing/custom?portal=" + url_escape(portal), reader, writer);
		if (meta)
			meta->assign(reader);
	}

	// Added for streaming
	void get_weights(const std::string& portal, const SwarmState& state, std::string opt_type, double channel_rate, WeightsMap& result_weights, PIDClassPercentages& result_pcts, P4PProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::aoe::detail::RequestStateWriter writer(state);
		p4p::protocol::aoe::detail::ResponseWeightsReader reader(result_weights, result_pcts);
		std::ostringstream oss;
		oss << channel_rate;
		std::string c = oss.str();
		make_request("POST", "streaming" + url_escape(opt_type) + "?channelrate=" + url_escape(oss.str()) + "&portal=" + url_escape(portal), reader, writer);
		if (meta)
			meta->assign(reader);
	}

	void get_weights(const std::string& portal, const SwarmState& state, std::string opt_type, double channel_rate, double relax, WeightsMap& result_weights, PIDClassPercentages& result_pcts, P4PProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		p4p::protocol::aoe::detail::RequestStateWriter writer(state);
		p4p::protocol::aoe::detail::ResponseWeightsReader reader(result_weights, result_pcts);
		std::ostringstream oss;
		oss << channel_rate;
		std::ostringstream oss2;
		oss2 << relax;
		make_request("POST", "streaming" + url_escape(opt_type) + "?channelrate=" + url_escape(oss.str()) + "&relax=" + url_escape(oss2.str()) + "&portal=" + url_escape(portal), reader, writer);
		if (meta)
			meta->assign(reader);
	}

private:

};

};
};
};

#endif

