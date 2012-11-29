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


#ifndef P4P_APIBASE_PARSING_H
#define P4P_APIBASE_PARSING_H

#include <map>
#include <p4p/pid.h>
#include <p4p/ip_addr.h>
#include <p4p/protocol/exceptions.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {

/* Forward declarations */
class P4PProtocol;

namespace detail {

/* Convenience classes for reading responses */
class p4p_common_cpp_EXPORT ResponseReader
{
public:
	ResponseReader();
	virtual ~ResponseReader() {}
	const std::string& get_header(const std::string& name) const;
	size_t process(void* buf, size_t len);
	void finish() throw (P4PProtocolError) { consume(true); };
	void add_header(const std::string& name, const std::string& value);
	bool has_error() const { return has_error_; }
	const P4PProtocolError& get_error() const { return error_; }
protected:
	virtual size_t consume(bool finished) throw (P4PProtocolError) { return buffer_.size(); }
	bool read_token(std::string::size_type& start_pos, bool finished, std::string::size_type& result_start, std::string::size_type& result_end, bool require = true) throw (P4PProtocolError);
	bool read_token(std::string::size_type& start_pos, bool finished, std::string& result, bool require = true) throw (P4PProtocolError);
	bool read_token(std::string::size_type& start_pos, bool finished, unsigned int& result, bool require = true) throw (P4PProtocolError);
	bool read_token(std::string::size_type& start_pos, bool finished, double& result, bool require = true) throw (P4PProtocolError);
	bool read_token(std::string::size_type& start_pos, bool finished, PID& result, bool require = true) throw (P4PProtocolError);
	bool read_token(std::string::size_type& start_pos, bool finished, IPPrefix& result, bool require = true) throw (P4PProtocolError);
	const std::string& get_buffer() const { return buffer_; }
private:
	const static std::string EMPTY;
	typedef std::map<std::string, std::string> Headers;
	bool has_error_;
	P4PProtocolError error_;
	std::string buffer_;
	Headers hdrs_;
};

void p4p_common_cpp_EXPORT ReadResponseStream(std::istream& stream, ResponseReader& reader) throw (P4PProtocolError);

class p4p_common_cpp_EXPORT RequestWriter
{
public:
	RequestWriter();
	virtual ~RequestWriter() {}
	int process(void* buf, size_t len);
	bool has_error() const { return has_error_; }
	const P4PProtocolError& get_error() const { return error_; }
protected:
	virtual bool produce() throw (P4PProtocolError) { return true; }
	bool write(const std::string& s) throw (P4PProtocolError);
	bool write(unsigned long i) throw (P4PProtocolError);
	bool write(unsigned int i) throw (P4PProtocolError);
	bool write(int i) throw (P4PProtocolError);
	bool write(double d) throw (P4PProtocolError);
	bool write(char c) throw (P4PProtocolError);
	bool write(const PID& p) throw (P4PProtocolError);
	template <class Iterator>
	bool write_list(Iterator first, Iterator last, const char* pre_sep)
	{
		for ( ; first != last; ++first)
		{
			if (!write(pre_sep)) return false;
			if (!write(*first)) return false;
		}
		return true;
	}
	void write_checkpoint();
private:
	bool has_error_;
	P4PProtocolError error_;
	char* buffer_;
	size_t buffer_pos_;
	size_t buffer_mark_;
	size_t buffer_len_;
	bool finished_;
};

template <class InputIterator>
class p4p_common_cpp_ex_EXPORT RequestCollectionWriter : public RequestWriter
{
public:
	RequestCollectionWriter(InputIterator first, InputIterator last) : cur_(first), last_(last) {}
	virtual bool produce() throw (P4PProtocolError)
	{
		for ( ; cur_ != last_; ++cur_)
		{
			if (!write(*cur_)) return false;
			if (!write("\r\n")) return false;
			write_checkpoint();
		}

		if (!write("\r\n")) return false;
		write_checkpoint();

		return true;
	}
private:
	InputIterator cur_, last_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseTokenReader : public ResponseReader
{
public:
	ResponseTokenReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		std::string token;
		while (read_token(pos, finished, token, false))
			*itr_++ = token;
		return pos;
	}
private:
	OutputIterator itr_;
};

class p4p_common_cpp_EXPORT ResponseSingleTokenReader : public ResponseReader
{
public:
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		if (!token_.empty())
			return get_buffer().size();

		std::string::size_type pos = 0;
		read_token(pos, finished, token_);
		return pos;
	}
	const std::string& get_token() const { return token_; }
private:
	std::string token_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponsePIDReader : public ResponseReader
{
public:
	ResponsePIDReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			PID pid;
			if (!read_token(cur_pos, finished, pid, false))
				break;

			*itr_++ = pid;

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

}; // namespace detail
}; // namespace protocol
}; // namespace p4p

#endif

