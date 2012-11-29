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


#ifndef PROTOCOL_SERVER_REST_H
#define PROTOCOL_SERVER_REST_H

#include <queue>
#include <string>
#include <boost/lexical_cast.hpp>
#include <p4pserver/protocol_server_base.h>
#include <p4pserver/marked_stream.h>
#include <p4pserver/rest_request_state.h>
#include <p4pserver/compiler.h>

class RESTRequestState;
struct MHD_Daemon;
struct MHD_Connection;
struct MHD_Response;

static const int MHD_YES			= 1;
static const int MHD_NO				= 0;

static const int MHD_HTTP_OK			= 200;
static const int MHD_HTTP_TEMPORARY_REDIRECT	= 307;
static const int MHD_HTTP_BAD_REQUEST		= 400;
static const int MHD_HTTP_METHOD_NOT_ACCEPTABLE	= 406;
static const int MHD_HTTP_NOT_FOUND		= 404;
static const int MHD_HTTP_INTERNAL_SERVER_ERROR	= 500;
static const int MHD_HTTP_SERVICE_UNAVAILABLE	= 503;

class p4p_common_server_EXPORT ProtocolServerRESTBase
{
public:
	static const unsigned int HTTP_METHOD_GET	= 0;
	static const unsigned int HTTP_METHOD_POST	= 1;
	static const unsigned int HTTP_METHOD_PUT	= 2;
	static const unsigned int HTTP_METHOD_DELETE	= 3;

	ProtocolServerRESTBase(const std::string& addr, unsigned short port,
			       unsigned int num_threads,
			       const std::string& ssl_cert, const std::string& ssl_key,
			       unsigned int connection_timeout,
			       unsigned int connection_limit,
			       unsigned int ip_connection_limit);

	virtual ~ProtocolServerRESTBase();

	RESTRequestState* get_request_state();
	void put_request_state(RESTRequestState* state);
	virtual bool handle_request(RESTRequestState* state) = 0;
	unsigned int get_datalen(RESTRequestState* state, const char* data, unsigned int len);
	int process_request_data(RESTRequestState* state, const char* data, unsigned int data_len);

	log4cpp::Category* get_server_logger()		{ return server_logger_; }
	void* get_server_obj()				{ return server_obj_; }

protected:

	void base_start(log4cpp::Category* server_logger, void* server_obj);
	void base_stop();

private:
	std::string addr_;
	unsigned short port_;
	unsigned int num_threads_;
	std::string ssl_cert_;
	std::string ssl_key_;
	unsigned int connection_timeout_;
	unsigned int connection_limit_;
	unsigned int ip_connection_limit_;

	struct MHD_Daemon* daemon_;

	boost::mutex requests_mutex_;
	std::queue<RESTRequestState*> requests_;

	log4cpp::Category* server_logger_;
	void* server_obj_;
};

template <unsigned int MSG_MAX, class RequestHandler>
class p4p_common_server_ex_EXPORT ProtocolServerREST : public ProtocolServerBase<MSG_MAX>, public ProtocolServerRESTBase
{
public:
	ProtocolServerREST(const std::string& addr, unsigned short port,
			   unsigned int num_threads,
			   const std::string& ssl_cert, const std::string& ssl_key,
			   unsigned int connection_timeout = 0,
			   unsigned int connection_limit = 100,
			   unsigned int ip_connection_limit = 5)
		: ProtocolServerBase<MSG_MAX>(addr, port),
		  ProtocolServerRESTBase(addr, port,
					 num_threads,
					 ssl_cert, ssl_key,
					 connection_timeout,
					 connection_limit,
					 ip_connection_limit)
	{}

	virtual ~ProtocolServerREST() {}

	virtual void* get_server_obj()				{ return this; }
	virtual log4cpp::Category* get_server_logger() const	{ return this->get_logger(); }

protected:

	virtual void server_run()
	{
		base_start(this->get_logger(), this);
		while (!this->is_stopped())
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	virtual void server_stop()
	{
		base_stop();
	}

	virtual std::string get_logger_name() const
	{
		return "RESTHandler(" + boost::lexical_cast<std::string>(this->get_port()) + ")";
	}

	virtual bool handle_request(RESTRequestState* state)
	{
		return handler_(this, state);
	}

private:
	RequestHandler handler_;
};

#define REQUEST_READ_ERR(data_str, state)									\
	do {													\
		/* Ignore end-of-file errors */									\
		if (data_str.eof())										\
		{												\
			if (state->get_request_finished())							\
				break;										\
			else											\
				return false;									\
		}												\
		/* If more data is coming, just return what we've got so far */					\
		if (!state->get_request_finished())								\
			return false;										\
		/* No more data is coming, so request is formatted incorrectly */				\
		goto invalid_argument;										\
	} while (0);

#endif


#define RESPONSE_WRITE_BEGIN											\
	try													\
	{													\
		do {

#define RESPONSE_WRITE_END											\
		} while (0);											\
	}													\
	catch (std::ios_base::failure& e)									\
	{													\
		if (strcmp("write area exhausted", e.what()) == 0)						\
			return rsp.get_mark();									\
		throw e;											\
	}


