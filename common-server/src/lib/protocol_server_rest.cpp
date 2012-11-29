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


#include "p4pserver/protocol_server_rest.h"

#include <iostream>

#include <boost/thread/mutex.hpp>
extern "C" {
#include <microhttpd.h>
}

int print_out_key (void *cls, enum MHD_ValueKind kind, 
                   const char *key, const char *value)
{
	RESTRequestState* state = (RESTRequestState*)cls;
	state->add_header(key, value);
	return MHD_YES;
}


#if MHD_VERSION >= 0x00040001
static int REST_AccessHandlerCallback(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* upload_data, size_t* upload_data_size, void** con_cls)
#else
static int REST_AccessHandlerCallback(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* upload_data, unsigned int* upload_data_size, void** con_cls)
#endif
{
	ProtocolServerRESTBase* server = (ProtocolServerRESTBase*)cls;
	RESTRequestState* state = (RESTRequestState*)*con_cls;

	if (!state)
	{
		/* Parse the header and construct request state */
		state = server->get_request_state();
		state->clear();
		state->set_conn(connection);

		//MHD_get_connection_values (connection, MHD_HEADER_KIND, &print_out_key, state);

		/* Parse the version (the 'clear()' operation above sets version to default already) */
		server->get_server_logger()->debug("Parsing version");
		const char* s = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "version");
		if (s && s[0])
		{
			char* errptr = NULL;
			state->set_version(strtoul(s, &errptr, 10));
			if (*errptr)
				goto invalid_argument;
		}

		/* Parse the HTTP Method */
		server->get_server_logger()->debug("Parsing method");
		if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
			state->set_method(ProtocolServerRESTBase::HTTP_METHOD_GET);
		else if (strcmp(method, MHD_HTTP_METHOD_POST) == 0)
			state->set_method(ProtocolServerRESTBase::HTTP_METHOD_POST);
		else if (strcmp(method, MHD_HTTP_METHOD_PUT) == 0)
			state->set_method(ProtocolServerRESTBase::HTTP_METHOD_PUT);
		else if (strcmp(method, MHD_HTTP_METHOD_DELETE) == 0)
			state->set_method(ProtocolServerRESTBase::HTTP_METHOD_DELETE);
		else
			goto invalid_argument;
	
		/* Parse the path components */
		server->get_server_logger()->debug("Parsing path");
		if (url[0] != '/')
			goto invalid_argument;
		state->set_path(url);

		server->get_server_logger()->debug("Processing request: %s %s (%u body bytes)", method, url, (unsigned int)*upload_data_size);

		*con_cls = state;

		/* Install callbacks for handling the request */
		server->get_server_logger()->debug("Installing callbacks");
		if ((!server->handle_request(state) || !state->get_finish_callback()) && !state->get_response())
		{
			state->set_empty_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE);
			return MHD_YES;
		}
	}
	else
	{
		/* We already parsed the headers and we've seen a 0-length chunk
		 * the second time.  Thus, this is the final chunk of the request. */
		server->get_server_logger()->debug("Marking request as finished");
		if (*upload_data_size == 0)
			state->set_request_finished(true);
	}

	server->get_server_logger()->debug("received chunk of size %u", (unsigned int)*upload_data_size);

	if (!state->get_response())
	{
		/* We haven't yet queued a response. Process next chunk. */
		RESTRequestState::Buffer& buffer = state->get_buffer();

		/* Append data buffer. libmicrohttpd requires us to consume at least
		 * some data, but we aren't guaranteed that our callback processes
		 * any data.  Thus, we need an intermediate buffer. */

		buffer.set_capacity(std::max(buffer.capacity(), buffer.size() + *upload_data_size));
		buffer.insert(buffer.end(), upload_data, upload_data + *upload_data_size);
		server->get_server_logger()->debug("added %d bytes to buffer; buffer has %u bytes total", *upload_data_size, buffer.size());

		/* Process available data */
		buffer.linearize();
		int bytes_read = server->process_request_data(state, buffer.array_one().first, buffer.array_one().second);
		if (bytes_read < 0)
			return MHD_NO;
		server->get_server_logger()->debug("read %d bytes", bytes_read);

		/* Remove processed data from the buffer */
		buffer.rresize(buffer.size() - bytes_read);
		server->get_server_logger()->debug("%u bytes left unread", (unsigned int)buffer.size());
	}

	/* We've processed all data */
	*upload_data_size = 0;

	/* Handle end of request */
	if (state->get_request_finished())
	{
		/* Call the last callback if we haven't already generated a response */
		if (!state->get_response())
		{
			server->get_server_logger()->debug("request finished; performing callback");
			state->get_finish_callback()(server->get_server_obj(), state, state->get_callback_data());
		}

		/* If still no response generated, try again later */
		if (!state->get_response())
		{
			server->get_server_logger()->debug("no response queued; trying again later");
			return MHD_YES;
		}

		/* Send the response */
		server->get_server_logger()->debug("Returning status code: %u", state->get_status());
		int rc = MHD_queue_response(state->get_conn(), state->get_status(), state->get_response());
		MHD_destroy_response(state->get_response());
		return rc;
	}

	server->get_server_logger()->debug("continue receiving request");
	return MHD_YES;

invalid_argument:
	state->set_empty_response(MHD_HTTP_BAD_REQUEST);
	return MHD_YES;
}

static int REST_RequestCleanupCallback(void* cls, struct MHD_Connection* connection, void** con_cls, enum MHD_RequestTerminationCode toe)
{
	ProtocolServerRESTBase* server = (ProtocolServerRESTBase*)cls;
	RESTRequestState* state = (RESTRequestState*)*con_cls;

	server->get_server_logger()->debug("cleaning up request");
		
	if (!state)
		return MHD_YES;

	if (state->get_free_callback() && state->get_callback_data())
		state->get_free_callback()(state->get_callback_data());

	server->get_server_logger()->debug("clearing request state");
	state->clear();
	server->put_request_state(state);

	return MHD_YES;
}

ProtocolServerRESTBase::ProtocolServerRESTBase(
		const std::string& addr, unsigned short port,
		unsigned int num_threads,
		const std::string& ssl_cert, const std::string& ssl_key,
		unsigned int connection_timeout,
		unsigned int connection_limit,
		unsigned int ip_connection_limit)
	: addr_(addr),
	  port_(port),
	  num_threads_(num_threads),
	  ssl_cert_(ssl_cert),
	  ssl_key_(ssl_key),
	  connection_timeout_(connection_timeout),
	  connection_limit_(connection_limit),
	  ip_connection_limit_(ip_connection_limit),
	  daemon_(NULL),
	  server_logger_(NULL),
	  server_obj_(NULL)
{
}

ProtocolServerRESTBase::~ProtocolServerRESTBase()
{
	base_stop();

	while (!requests_.empty())
	{
		RESTRequestState* s = requests_.front();
		requests_.pop();
		delete s;
	}
}

RESTRequestState* ProtocolServerRESTBase::get_request_state()
{
	boost::mutex::scoped_lock l(requests_mutex_);
	if (requests_.empty())
		return new RESTRequestState();

	RESTRequestState* s = requests_.front();
	requests_.pop();
	return s;
}

void ProtocolServerRESTBase::put_request_state(RESTRequestState* state)
{
	boost::mutex::scoped_lock l(requests_mutex_);
	requests_.push(state);
}

void ProtocolServerRESTBase::base_start(log4cpp::Category* server_logger, void* server_obj)
{
	server_logger_ = server_logger;
	server_obj_ = server_obj;

	int flags = (!ssl_cert_.empty() && !ssl_key_.empty() ? MHD_USE_SSL : 0) | MHD_USE_SELECT_INTERNALLY;
#ifdef DEBUG
	flags |= MHD_USE_DEBUG;
#endif

	daemon_ = MHD_start_daemon(flags,
				   port_,
				   NULL, NULL,
				   &REST_AccessHandlerCallback, this,
#if MHD_VERSION >= 0x00040100
				   MHD_OPTION_THREAD_POOL_SIZE, num_threads_,
#endif
				   MHD_OPTION_NOTIFY_COMPLETED, &REST_RequestCleanupCallback, this,
				   MHD_OPTION_CONNECTION_TIMEOUT, connection_timeout_,
				   MHD_OPTION_CONNECTION_LIMIT, connection_limit_,
				   MHD_OPTION_PER_IP_CONNECTION_LIMIT, ip_connection_limit_,
				   MHD_OPTION_END);

#if MHD_VERSION < 0x00040100
	server_logger_->warn("The version of libmicrohttpd does not include thread pooling support; using default behavior");
#endif

	if (!daemon_)
		throw std::runtime_error("Failed to create HTTP daemon");
}

void ProtocolServerRESTBase::base_stop()
{
	if (daemon_)
		MHD_stop_daemon(daemon_);
	daemon_ = NULL;
}

unsigned int ProtocolServerRESTBase::get_datalen(RESTRequestState* state, const char* data, unsigned int len)
{
	/* Take the whole chunk if this is the last set of data */
	if (state->get_request_finished())
		return len;

	if (len == 0)
		return len;

	/* Find the last whitespace;
	 * NOTE: We ignore stuff after the trailing whitespace */
	const char* n = data + len - 1;
	while (n >= data && !isspace(*n)) --n;
	if (n >= data)
	{
		get_server_logger()->debug("found whitespace character 0x%x at position %d", (int)*n, n - data);
		return n - data + 1;
	}
	else
	{
		get_server_logger()->debug("no whitespace character found");
		return 0;
	}
}

int ProtocolServerRESTBase::process_request_data(RESTRequestState* state, const char* data, unsigned int data_len)
{
	data_len = get_datalen(state, data, data_len);

	/* Process data for this request */
	get_server_logger()->debug("processing chunk of size %d", data_len);
	if (state->get_process_callback())
	{
		RequestStream req(data, data_len);
		bool result = state->get_process_callback()(get_server_obj(), state, state->get_callback_data(), req);
		int consumed = result ? data_len : req.get_mark();

		get_server_logger()->debug("processing returned %s (%d bytes consumed)", (result ? "true" : "false"), consumed);

		return consumed;
	}

	/* No callback was defined; ignore the data in the request */
	return data_len;
}

