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


#ifndef P4P_PORTALAPI_PDISTANCE_H
#define P4P_PORTALAPI_PDISTANCE_H

#include <string>
#include <p4p/protocol/protobase.h>
#include <p4p/protocol-portal/detail/parsing.h>
#include <p4p/protocol-portal/metainfo.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {

class p4p_common_cpp_EXPORT PDistancePortalProtocol : public P4PProtocol
{
public:

	PDistancePortalProtocol(const std::string& host, unsigned short port = 6671, const std::string& view = "DEFAULT", bool persistent = false) throw (std::runtime_error);
	virtual ~PDistancePortalProtocol();

	/**
	 * Call PDistance Portal Service's GetPDistance function to retrieve PDistances corresponding
	 * to a set of source PIDs.  The resulting PDistanceMatrix contains a row for each source PID
	 * contained in the request.  It is also possible to call this function using an empty list
	 * of source PIDs to obtain the full set of PDistances (between each pair of available PIDs)
	 * available from the PDistance Portal Service.
	 *
	 * @param pids_first Begin iterator for collection of input source PIDs. Source PID collection
	 * 	is a collection of PID objects, so '*pids_first' must be of type PID.
	 * @param pids_last End iterator for collection of input source PIDs. Source PID collection
	 * 	is a collection of PID objects, so '*pids_first' must be of type PID.
	 * @param reverse Specify whether the reverse PID mappings should be included in the resulting
	 * 	PDistanceMatrix as well. If true, the resulting PDistanceMatrix will also included entries
	 * 	for each Source PID for DestinationPID -> SourcePID for each available DestinationPID.
	 * @param result Resulting PDistanceMatrix specifying directional PDistances between PIDs.
	 * @param meta Output parameter into which meta information (e.g., TTL) of the returned data
	 * 	is placed. Ignored if NULL.
	 */
	template <class InputIterator>
	void get_pdistance(InputIterator pids_first, InputIterator pids_last,
			   bool reverse,
			   PDistanceMatrix& result,
			   P4PPortalProtocolMetaInfo* meta = NULL) throw (P4PProtocolError)
	{
		detail::ResponsePDistanceReader reader(result);
		detail::RequestPDistanceWriter<InputIterator> writer(pids_first, pids_last, reverse);
		make_request("POST", "pdistance" + (view_ != "DEFAULT" ? "?view=" + url_escape(view_) : ""), reader, writer);
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

