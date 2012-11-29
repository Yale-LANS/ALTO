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


#include "p4p/protocol/protobase.h"

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
#endif

#include <sstream>
#include <ctype.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <p4p/detail/util.h>
#include <p4p/detail/inet_service.h>
#include <p4p/protocol/detail/dataconv.h>

namespace p4p {
namespace protocol {

const char* P4PProtocol::HDR_CACHECONTROL = "Cache-Control";

P4PProtocol::P4PProtocol(const std::string& host, unsigned short port, bool persistent) throw (std::runtime_error, P4PProtocolError)
	: host_(host),
	  port_(port),
	  persistent_(persistent)
{
	/* Make some adjustments if 'host' contains a port itself */
	if (host.find(':') != std::string::npos)
	{
		try
		{
			p4p::detail::InetService host_serv = p4p::detail::p4p_token_cast<p4p::detail::InetService>(host);
			host_ = host_serv.get_address();
			port_ = host_serv.get_port();
		}
		catch (std::invalid_argument& e)
		{
			throw P4PProtocolError("Invalid format for host parameter: " + std::string(e.what()));
		}
	}

	http_handle_ = curl_easy_init();
	if (!http_handle_)
		throw std::runtime_error("Failed to initialize libcurl handle");

}

P4PProtocol::~P4PProtocol()
{
	curl_easy_cleanup(http_handle_);
}

size_t P4PProtocol::header_read(void* buffer, size_t size, size_t nmemb, void* userp)
{
	detail::ResponseReader* reader = (detail::ResponseReader*)userp;

	size_t count = size * nmemb;

	std::string line((const char*)buffer, count);
	line = p4p::detail::trim(line);

	/* Ignore blank lines */
	if (line.empty())
		return count;

	/* Ignore status line */
	if (line.substr(0, 5) == "HTTP/")
		return count;

	std::string::size_type colon = line.find(':');
	if (colon == std::string::npos)
		return -1;

	std::string name = line.substr(0, colon);
	std::string value = line.substr(colon + 1);

	name = p4p::detail::trim(name);
	value = p4p::detail::trim(value);

	reader->add_header(name, value);

	return count;
}

size_t P4PProtocol::response_read(void* buffer, size_t size, size_t nmemb, void* userp)
{
	detail::ResponseReader* reader = (detail::ResponseReader*)userp;
	return reader->process(buffer, size * nmemb);
}

size_t P4PProtocol::request_write(void* buffer, size_t size, size_t nmemb, void *userp)
{
	detail::RequestWriter* writer = (detail::RequestWriter*)userp;
	return writer->process(buffer, size * nmemb);
}

std::string P4PProtocol::url_escape(const std::string& s) throw (P4PProtocolError)
{
	char* esc = curl_easy_escape(http_handle_, s.c_str(), s.size());
	if (!esc)
		P4PProtocolError("Failed to escape URL");

	std::string result = esc;
	curl_free(esc);
	return result;
}

unsigned int P4PProtocol::extract_cache_maxage(const std::string& hdr_value) throw (P4PProtocolError)
{
	std::string::size_type sep = hdr_value.find('=');

	/* No equal sign means this can't be a 'max-age' header */
	if (sep == std::string::npos)
		return 0;

	/* Only read 'max-age' directive for now */
	if (hdr_value.substr(0, sep) != "max-age")
		return 0;

	/* Assume uncacheable on parse error */
	try
	{
		return p4p::detail::p4p_token_cast<unsigned int>(hdr_value.substr(sep + 1));
	}
	catch (std::invalid_argument& e)
	{
		return 0;
	}
}

void P4PProtocol::make_request(const char* method, const std::string& path, detail::ResponseReader* reader, detail::RequestWriter* writer) throw (P4PProtocolError)
{
	/* Setup the URL */
	std::ostringstream url;
	url << "http://" << get_host() << ':' << get_port() << '/' << path;
	std::string url_str = url.str();

	/* Assign CURL options */
	curl_easy_setopt(http_handle_, CURLOPT_HEADERFUNCTION, header_read);
	curl_easy_setopt(http_handle_, CURLOPT_HEADERDATA, reader ? reader : &default_reader_);
	curl_easy_setopt(http_handle_, CURLOPT_WRITEFUNCTION, response_read);
	curl_easy_setopt(http_handle_, CURLOPT_WRITEDATA, reader ? reader : &default_reader_);
	curl_easy_setopt(http_handle_, CURLOPT_READFUNCTION, request_write);
	curl_easy_setopt(http_handle_, CURLOPT_READDATA, writer ? writer : &default_writer_);
	curl_easy_setopt(http_handle_, CURLOPT_UPLOAD, writer ? 1 : 0);
	curl_easy_setopt(http_handle_, CURLOPT_CUSTOMREQUEST, method);
	curl_easy_setopt(http_handle_, CURLOPT_URL, url_str.c_str());
	curl_easy_setopt(http_handle_, CURLOPT_FORBID_REUSE, persistent_ ? 0L : 1L);
	curl_easy_setopt(http_handle_, CURLOPT_TCP_NODELAY, 1L);

	/* Make the request and process the response */
	CURLcode rc = curl_easy_perform(http_handle_);
	if (rc != CURLE_OK)
		throw P4PProtocolConnectionError(get_host(), get_port(), curl_easy_strerror(rc));

	/* Re-throw exceptions caused while writing the request */
	if (writer && writer->has_error())
		throw writer->get_error();

	/* Process any remaining response left in the buffer
	 * It's okay if an exception gets raised here. */
	if (reader)
		reader->finish();

	/* Re-throw any exception that was raised during processing of the response */
	if (reader && reader->has_error())
		throw reader->get_error();

	/* Get the HTTP status code */
	long status;
	rc = curl_easy_getinfo(http_handle_, CURLINFO_RESPONSE_CODE, &status);
	if (rc != CURLE_OK)
		throw P4PProtocolParseError("libcurl failed to retrieve HTTP response code");

	/* Throw an exception for anything but the normal '200' response */
	if (status != 200)
		throw P4PProtocolHTTPError(status);
}

};
};

