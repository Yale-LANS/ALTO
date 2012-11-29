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


#ifndef P4P_APIBASE_H
#define P4P_APIBASE_H

#include <string>
#include <p4p/pid.h>
#include <p4p/protocol/exceptions.h>
#include <p4p/protocol/detail/parsing.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {

class p4p_common_cpp_EXPORT P4PProtocol
{
public:
	static const char* HDR_CACHECONTROL;
	static unsigned int extract_cache_maxage(const std::string& hdr_value) throw (P4PProtocolError);

	P4PProtocol(const std::string& host, unsigned short port, bool peristent = true) throw (std::runtime_error, P4PProtocolError);
	virtual ~P4PProtocol();

	const std::string& get_host() const { return host_; }
	unsigned short get_port() const { return port_; }
	bool get_persistent() const { return persistent_; }

protected:

	std::string url_escape(const std::string& s) throw (P4PProtocolError);

	/* Wrapper function to make an HTTP request */
	void make_request(const char* method, const std::string& path, detail::ResponseReader* reader, detail::RequestWriter* writer) throw (P4PProtocolError);
	void make_request(const char* method, const std::string& path, detail::ResponseReader& reader, detail::RequestWriter& writer) throw (P4PProtocolError)
	{
		make_request(method, path, &reader, &writer);
	}
	void make_request(const char* method, const std::string& path) throw (P4PProtocolError)
	{
		make_request(method, path, NULL, NULL);
	}
	void make_request(const char* method, const std::string& path, detail::ResponseReader& reader) throw (P4PProtocolError)
	{
		make_request(method, path, &reader, NULL);
	}
	void make_request(const char* method, const std::string& path, detail::RequestWriter& writer) throw (P4PProtocolError)
	{
		make_request(method, path, NULL, &writer);
	}

private:
	/* Disallow copy constructor and assignment operator */
	P4PProtocol(const P4PProtocol& dummy) {}
	P4PProtocol& operator=(const P4PProtocol& dummy) { return *this; }

	/* Callback function for processing request/response data */
	static size_t header_read(void* buffer, size_t size, size_t nmemb, void* userp);
	static size_t response_read(void* buffer, size_t size, size_t nmemb, void* userp);
	static size_t request_write(void* buffer, size_t size, size_t nmemb, void* userp);

	/* Default reader and writer */
	detail::ResponseReader default_reader_;
	detail::RequestWriter default_writer_;

	/* The host and port we're talking to */
	std::string host_;
	unsigned short port_;

	/* Use persistent connections? */
	bool persistent_;

	void* http_handle_;
};

}; // namespace protocol
}; // namespace p4p

#endif

