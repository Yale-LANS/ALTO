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


#include "p4pserver/rest_request_state.h"

#include <zlib.h>
extern "C" {
#include <microhttpd.h>
}

class ContentReaderState
{
public:
	ContentReaderState(RESTContentReaderCallback callback_write,
			   RESTRequestFree callback_free,
			   void* callback_data,
			   bool gzip = false)
		: callback_write_(callback_write),
		  callback_free_(callback_free),
		  callback_data_(callback_data),
		  gzip_(gzip),
		  callback_finished_(false),
		  z_finished_(false)
	{
		if (gzip_)
		{
			zstrm_.zalloc = Z_NULL;
			zstrm_.zfree = Z_NULL;
			zstrm_.opaque = Z_NULL;

			/* NOTE: adding 16 to 'windowBits' parameter enables gzip compression */
			int rc = deflateInit2(&zstrm_, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + 15, 8, Z_DEFAULT_STRATEGY);
			if (rc != Z_OK)
				throw std::runtime_error("Illegal state: failed to initialize zlib");
		}
	}

	~ContentReaderState()
	{
		if (gzip_)
			deflateEnd(&zstrm_);

		if (callback_free_)
			callback_free_(callback_data_);
	}

	void buffer_read(std::string::size_type bytes)
	{
		if (bytes > buffer_.size())
			throw std::runtime_error("Illegal state: tried to remove too many bytes from buffer");

		std::string::size_type new_size = buffer_.size() - bytes;

		/* Move data down in the buffer and resize */
		if (new_size > 0)
			memmove(&buffer_[0], &buffer_[bytes], new_size);
		buffer_.resize(new_size);
	}

	int process_normal(uint64_t pos, char* buf, int max)
	{
		if (buffer_.empty())
			return -1;

		int rc = std::min((int)buffer_.size(), max);

		/* Copy as much as we can to the target */
		if (rc > 0)
			memcpy(buf, &buffer_[0], rc);

		/* Adjust buffer for data read */
		buffer_read(rc);

		return rc;
	}

	int process_gzip(uint64_t pos, char* buf, int max)
	{
		if (z_finished_)
			return -1;

		int flush_mode = buffer_.empty() ? Z_FINISH : Z_NO_FLUSH;

		zstrm_.avail_in = buffer_.size();
		zstrm_.next_in = (unsigned char*)&buffer_[0];
		zstrm_.avail_out = max;
		zstrm_.next_out = (unsigned char*)buf;

		/* Produce the compressed data */
		z_finished_ = (deflate(&zstrm_, flush_mode) == Z_STREAM_END);

		/* Return number of compressed bytes produced */
		int rc = max - zstrm_.avail_out;

		/* Adjust buffer for data read */
		buffer_read(buffer_.size() - zstrm_.avail_in);

		return rc;
	}

	int process(uint64_t pos, char* buf, int max)
	{
		/* If callback isn't finished yet, ask it for more data
		 * and store it in our buffer */
		if (!callback_finished_)
		{
			std::string::size_type prev_buf_size = buffer_.size();

			/* Expand buffer to fit new data */
			buffer_.resize(prev_buf_size + max);

			int rc = callback_write_(callback_data_, 0, &buffer_[prev_buf_size], max);
			if (rc == -1)
			{
				/* Callback is finished producing data */
				callback_finished_ = true;
				buffer_.resize(prev_buf_size);
			}
			else
			{
				/* Trim buffer to size of data actually produced */
				buffer_.resize(prev_buf_size + rc);
			}
		}

		/* Produce output data. These functions internally remove
		 * written data from the buffer. */
		if (gzip_)
			return process_gzip(pos, buf, max);
		else
			return process_normal(pos, buf, max);
	}

private:
	RESTContentReaderCallback callback_write_;
	RESTRequestFree callback_free_;
	void* callback_data_;
	bool gzip_;


	std::string buffer_;		/* Buffer of unwritten data */
	bool callback_finished_;	/* Set to true when callback is finished producing data */

	z_stream zstrm_;		/* Compressor state */
	int z_finished_;		/* Set to true when compressor all finished (including trailers/checksums) */
};

#if MHD_VERSION >= 0x00040001
static int64_t callback_write(void *cls, uint64_t pos, char *buf, uint64_t max)
#else
static int callback_write(void *cls, size_t pos, char *buf, int max)
#endif
{
	ContentReaderState* state = (ContentReaderState*)cls;
	return state->process(pos, buf, max);
}

static void callback_free(void *cls)
{
	ContentReaderState* state = (ContentReaderState*)cls;
	delete state;
}


RESTRequestState::RESTRequestState()
	: buffer_(4096)
{
	memset(rawpath_, 0, MAX_PATH_LENGTH + 1);
	clear();
}

void RESTRequestState::clear()
{
	conn_ = NULL;
	rawpath_[0] = '\0';
	args_.clear();
	version_ = 0;
	method_ = 0;
	buffer_.clear();
	buffer_.set_capacity(4096);
	request_finished_ = false;
	process_callback_ = NULL;
	finish_callback_ = NULL;
	free_callback_ = NULL;
	callback_data_ = NULL;
	status_ = MHD_HTTP_OK;
	response_ = NULL;
}

void RESTRequestState::set_empty_response(unsigned int status)
{
	set_response(MHD_create_response_from_data(0, (void*)"", MHD_NO, MHD_NO));
	set_status(status);
}

void RESTRequestState::set_text_response(unsigned int status, const std::string& text, const char* content_type)
{
	set_response(MHD_create_response_from_data(text.size(), (void*)text.c_str(), MHD_NO, MHD_YES));
	MHD_add_response_header(get_response(), "Content-Type", content_type);
	set_status(status);
}

void RESTRequestState::set_callback_response(RESTContentReaderCallback rsp_writer, const char* content_type)
{
	const char* accept_enc = MHD_lookup_connection_value(get_conn(), MHD_HEADER_KIND, "Accept-Encoding");
	bool use_gzip = accept_enc && strstr(accept_enc, "gzip");

	set_response(MHD_create_response_from_callback(
			-1, RESPONSE_BLOCK_SIZE,
			callback_write,
			new ContentReaderState(rsp_writer, get_free_callback(), get_callback_data(), use_gzip),
			callback_free));
	MHD_add_response_header(get_response(), "Content-Type", content_type);
	if (use_gzip)
		MHD_add_response_header(get_response(), "Content-Encoding", "gzip");

	/* libmicrohttpd will call the appropriate callback to free the data instead of us */
	set_free_callback(NULL);
}

void RESTRequestState::add_response_header(const std::string& name, const std::string& value)
{
	MHD_add_response_header(get_response(), name.c_str(), value.c_str());
}

void RESTRequestState::set_path(const char* value)
{
	strncpy(rawpath_, value, MAX_PATH_LENGTH);

	args_.clear();

	/* Find components in the raw path */
	char* p = rawpath_;
	while ((p = strchr(p, '/')) != NULL)
	{
		*p = '\0';
		args_.push_back(++p);
	}
}

const char* RESTRequestState::get_qsargv(const char* key) const
{
	return MHD_lookup_connection_value(conn_, MHD_GET_ARGUMENT_KIND, key);
}

bool RESTRequestState::get_client_addr(p4p::IPPrefix& addr) const
{
#if MHD_VERSION >= 0x00040001
	const MHD_ConnectionInfo* cinfo = MHD_get_connection_info(conn_, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	if (!cinfo)
		return false;

	const struct sockaddr_in* saddr = (const struct sockaddr_in*) cinfo->client_addr;
	if (!saddr)
		return false;

	if (saddr->sin_family == AF_INET)
	{
		addr = p4p::IPPrefix(AF_INET, &saddr->sin_addr, sizeof(saddr->sin_addr) * 8);
		return true;
	}

	const struct sockaddr_in6* saddr6 = (const struct sockaddr_in6*)saddr;
	if (saddr6->sin6_family == AF_INET6)
	{
		addr = p4p::IPPrefix(AF_INET6, &saddr6->sin6_addr, sizeof(saddr6->sin6_addr) * 8);
		return true;
	}
#endif

	return false;
}

