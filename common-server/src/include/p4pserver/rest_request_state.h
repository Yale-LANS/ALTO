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


#ifndef REST_REQUEST_STATE_H
#define REST_REQUEST_STATE_H

#include <boost/iostreams/stream.hpp>
#include <boost/circular_buffer.hpp>
#include <string>
#include <vector>
#include <map>
#include <p4pserver/protocol_server_base.h>
#include <p4pserver/compiler.h>
#include <p4p/ip_addr.h>

struct MHD_Connection;
struct MHD_Response;
typedef int (*RESTContentReaderCallback) (void *cls, uint64_t pos, char *buf, int max);

class RESTRequestState;
typedef bool (*RESTRequestProcess)(void* server, RESTRequestState*, void*, std::istream&);
typedef void (*RESTRequestFinish)(void* server, RESTRequestState*, void*);
typedef void (*RESTRequestFree)(void*);

typedef std::map<std::string, const char*> HTTPHeaders;

class p4p_common_server_EXPORT RESTRequestState
{
public:
	typedef boost::circular_buffer<char> Buffer;

	static const unsigned int MAX_PATH_LENGTH = 255;
	static const unsigned int RESPONSE_BLOCK_SIZE = 4096;

	RESTRequestState();

	void clear();

	void set_conn(MHD_Connection* value) { conn_ = value; }
	MHD_Connection* get_conn() const { return conn_; }

	void set_version(unsigned int value) { version_ = value; }
	unsigned int get_version() const { return version_; }

	void set_method(unsigned int value) { method_ = value; }
	unsigned int get_method() const { return method_; }

	Buffer& get_buffer() { return buffer_; }

	void set_path(const char* value);
	unsigned int get_argc() const { return args_.size(); }
	const char* get_argv(unsigned int i) const
	{
		assert(i < args_.size());
		return args_[i];
	}

	const char* get_qsargv(const char* key) const;

	bool get_client_addr(p4p::IPPrefix& addr) const;

	void set_callbacks(RESTRequestFinish c_finish, RESTRequestFree c_free = NULL, void* data = NULL, RESTRequestProcess c_process = NULL)
	{
		set_finish_callback(c_finish);
		set_process_callback(c_process);
		set_free_callback(c_free);
		set_callback_data(data);
	}

	void set_process_callback(RESTRequestProcess value) { process_callback_ = value; }
	RESTRequestProcess get_process_callback() const { return process_callback_; }

	void set_finish_callback(RESTRequestFinish value) { finish_callback_ = value; }
	RESTRequestFinish get_finish_callback() const { return finish_callback_; }

	void set_free_callback(RESTRequestFree value) { free_callback_ = value; }
	RESTRequestFree get_free_callback() const { return free_callback_; }

	void set_callback_data(void* value) { callback_data_ = value; }
	void* get_callback_data() const { return callback_data_; }

	void set_request_finished(bool value) { request_finished_ = value; }
	bool get_request_finished() const { return request_finished_; }

	void set_empty_response(unsigned int status);
	void set_text_response(unsigned int status, const std::string& text, const char* content_type = "text/plain");
	void set_callback_response(RESTContentReaderCallback rsp_writer, const char* content_type = "text/plain");

	void set_status(unsigned int value) { status_ = value; }
	unsigned int get_status() const { return status_; }

	void set_response(MHD_Response* value) { response_ = value; }
	MHD_Response* get_response() const { return response_; }

	void add_response_header(const std::string& name, const std::string& value);

	const HTTPHeaders& get_headers() const { return headers_; }
	void add_header(const char* key, const char* value) { headers_[std::string(key)] = value; }

private:
	MHD_Connection* conn_;

	Buffer buffer_;
	bool request_finished_;

	char rawpath_[MAX_PATH_LENGTH+1];
	std::vector<char*> args_;

	unsigned int version_;
	unsigned int method_;

	RESTRequestProcess process_callback_;
	RESTRequestFinish finish_callback_;
	RESTRequestFree free_callback_;
	void* callback_data_;

	unsigned int status_;
	MHD_Response* response_;
	
	HTTPHeaders headers_;
};

#endif

