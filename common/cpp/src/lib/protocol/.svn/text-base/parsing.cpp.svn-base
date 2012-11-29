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


#include "p4p/protocol/detail/parsing.h"

#include <curl/curl.h>
#include <p4p/detail/util.h>
#include "p4p/protocol/detail/dataconv.h"
#include <iostream>

namespace p4p {
namespace protocol {
namespace detail {

const std::string ResponseReader::EMPTY = std::string();

ResponseReader::ResponseReader()
	: has_error_(false)
{
}

size_t ResponseReader::process(void* buf, size_t len)
{
	/* Don't bother consuming anything if
	 * we've already raised an error. Silently
	 * read and ignore rest of response. */
	if (has_error_)
		return len;

	/* Add data to the buffer */
	size_t prev_len = buffer_.size();
	buffer_.resize(prev_len + len);
	if (len > 0)
		memcpy(&buffer_[prev_len], buf, len);

	/* Consume whatever we can */
	size_t consumed;
	try
	{
		consumed = std::min(consume(false), buffer_.size());
	}
	catch (P4PProtocolError& e)
	{
		/* Store the exception and clear the buffer */
		error_ = e;
		has_error_ = true;
		buffer_.clear();
		return len;
	}

	/* Remove the data from the buffer */
	size_t remaining = buffer_.size() - consumed;
	if (remaining > 0)
		memmove(&buffer_[0], &buffer_[consumed], remaining);
	buffer_.resize(remaining);

	return len;
}

bool ResponseReader::read_token(std::string::size_type& pos, bool finished, std::string::size_type& result_start, std::string::size_type& result_end, bool require) throw (P4PProtocolError)
{
	std::string::size_type size = buffer_.size();

	/* Read leading whitespace */
	for ( ; pos < size && isspace(buffer_[pos]); ++pos) {}

	/* Fail if nothing there */
	if (pos >= size)
		goto failed;

	/* Mark current position as start of token */
	result_start = pos;

	/* Read until we reach non-whitespace */
	for ( ; pos < size && !isspace(buffer_[pos]); ++pos) {}

	/* If we reach the end of the buffer and this wasn't the end of the response, then fail */
	if (pos >= size && !finished)
	{
		pos = result_start; /* Reset to first non-whitespace character */
		goto failed;
	}

	/* Mark current position as the end of the token */
	result_end = pos;
	return true;

failed:
	/* Request is finished and we required a token */
	if (require && finished)
		throw P4PProtocolParseError();

	return false;
}

bool ResponseReader::read_token(std::string::size_type& start_pos, bool finished, std::string& result, bool require) throw (P4PProtocolError)
{
	std::string::size_type result_start, result_end;
	bool res = read_token(start_pos, finished, result_start, result_end, require);
	if (res)
		result = get_buffer().substr(result_start, result_end - result_start);
	return res;
}

bool ResponseReader::read_token(std::string::size_type& start_pos, bool finished, unsigned int& result, bool require) throw (P4PProtocolError)
{
	std::string s;
	bool res = read_token(start_pos, finished, s, require);
	if (res)
		result = p4p::detail::p4p_token_cast<unsigned int>(s);
	return res;
}

bool ResponseReader::read_token(std::string::size_type& start_pos, bool finished, double& result, bool require) throw (P4PProtocolError)
{
	std::string s;
	bool res = read_token(start_pos, finished, s, require);
	if (res)
		result = to_double(s);
	return res;
}

bool ResponseReader::read_token(std::string::size_type& start_pos, bool finished, PID& result, bool require) throw (P4PProtocolError)
{
	std::string s;
	bool res = read_token(start_pos, finished, s, require);
	if (!res)
		return false;

	try
	{
		result = p4p::detail::p4p_token_cast<PID>(s);
	}
	catch (std::invalid_argument& e)
	{
		throw P4PProtocolParseError(e.what());
	}

	return true;
}

bool ResponseReader::read_token(std::string::size_type& start_pos, bool finished, IPPrefix& result, bool require) throw (P4PProtocolError)
{
	std::string s;
	bool res = read_token(start_pos, finished, s, require);
	if (!res)
		return false;

	try
	{
		result = p4p::detail::p4p_token_cast<IPPrefix>(s);
	}
	catch (std::invalid_argument& e)
	{
		throw P4PProtocolParseError(e.what());
	}

	return true;
}

const std::string& ResponseReader::get_header(const std::string& name) const
{
	Headers::const_iterator itr = hdrs_.find(name);
	return itr != hdrs_.end() ? itr->second : EMPTY;
}

void ResponseReader::add_header(const std::string& name, const std::string& value)
{
	hdrs_.insert(std::make_pair(name, value));
}

void ReadResponseStream(std::istream& stream, ResponseReader& reader) throw (P4PProtocolError)
{
	static const unsigned int BUFFER_SIZE = 4096;

	char buf[BUFFER_SIZE];
	while (true)
	{
		/* Read next chunk of data */
		stream.read(buf, BUFFER_SIZE);
		size_t bytes_read = stream.gcount();

		if (bytes_read == 0 && stream.eof())
			break;

		if (bytes_read == 0 && stream.fail())
			throw P4PProtocolFileError("Failed to read from file");

		/* Process chunk of data; all data should be processed */
		size_t bytes_processed = reader.process(buf, bytes_read);
		if (bytes_read != bytes_processed)
			throw P4PProtocolParseError("All data not read by reader");
	}

	/* Process any remaining buffered data */
	reader.finish();

	if (reader.has_error())
		throw reader.get_error();
}

RequestWriter::RequestWriter()
	: has_error_(false),
	  buffer_(NULL),
	  buffer_pos_(0),
	  buffer_mark_(0),
	  buffer_len_(0),
	  finished_(false)
{
}

int RequestWriter::process(void* buf, size_t len)
{
	if (has_error_)
		return CURL_READFUNC_ABORT;

	if (finished_)
		return 0;

	/* Initialize the buffer for this chunk */
	buffer_ = (char*)buf;
	buffer_pos_ = 0;
	buffer_mark_ = 0;
	buffer_len_ = len;

	/* Produce as much as possible for this chunk */
	try
	{
		finished_ = produce();
	}
	catch (P4PProtocolError& e)
	{
		/* Store the exception and return error */
		error_ = e;
		has_error_ = true;
		return CURL_READFUNC_ABORT;
	}

	/* Return number of bytes produced */
	return buffer_mark_;
}

bool RequestWriter::write(const std::string& s) throw (P4PProtocolError)
{
	/* Ensure there is space */
	if (buffer_pos_ + s.size() > buffer_len_)
		return false;

	/* Copy to the output buffer */
	memcpy(buffer_ + buffer_pos_, s.c_str(), s.size());
	buffer_pos_ += s.size();
	return true;
}

bool RequestWriter::write(char c) throw (P4PProtocolError)
{
	/* Ensure there is space */
	if (buffer_pos_ + 1 > buffer_len_)
		return false;

	/* Copy to the output buffer */
	*(buffer_ + buffer_pos_++) = c;
	return true;
}

bool RequestWriter::write(unsigned long i) throw (P4PProtocolError)
{
	return write(p4p::detail::p4p_token_cast<std::string>(i));
}

bool RequestWriter::write(unsigned int i) throw (P4PProtocolError)
{
	return write(p4p::detail::p4p_token_cast<std::string>(i));
}

bool RequestWriter::write(int i) throw (P4PProtocolError)
{
	return write(p4p::detail::p4p_token_cast<std::string>(i));
}

bool RequestWriter::write(double d) throw (P4PProtocolError)
{
	return write(p4p::detail::p4p_token_cast<std::string>(d));
}

bool RequestWriter::write(const PID& p) throw (P4PProtocolError)
{
	return write(p4p::detail::p4p_token_cast<std::string>(p));
}

void RequestWriter::write_checkpoint()
{
	/* Mark the current position */
	buffer_mark_ = buffer_pos_;
}

};
};
};
