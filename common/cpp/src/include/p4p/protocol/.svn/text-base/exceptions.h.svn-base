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


#ifndef P4P_APIBASE_EXCEPTIONS_H
#define P4P_APIBASE_EXCEPTIONS_H

#include <string>
#include <stdexcept>
#include <p4p/detail/util.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {

class p4p_common_cpp_ex_EXPORT P4PProtocolError : public std::exception
{
public:
	P4PProtocolError() : what_("P4P API Exception: Unknown") {}
	P4PProtocolError(const std::string& what) : what_("P4P API Exception: " + what) {}
	virtual ~P4PProtocolError() throw () {}

	virtual const char* what() const throw () { return what_.c_str(); }
private:
	std::string what_;
};

class p4p_common_cpp_ex_EXPORT P4PProtocolHTTPError : public P4PProtocolError
{
public:
	P4PProtocolHTTPError(unsigned long status)
		: P4PProtocolError("HTTP Error " + p4p::detail::p4p_token_cast<std::string>(status)), status_(status) {}
	unsigned long get_status() const { return status_; }
private:
	unsigned long status_;
};

class p4p_common_cpp_ex_EXPORT P4PProtocolParseError : public P4PProtocolError
{
public:
	P4PProtocolParseError()
		: P4PProtocolError("Parse error: unknown error")
	{}
	P4PProtocolParseError(const std::string& what)
		: P4PProtocolError("Parse error: " + what)
	{}
};

class p4p_common_cpp_ex_EXPORT P4PProtocolFileError : public P4PProtocolError
{
public:
	P4PProtocolFileError()
		: P4PProtocolError("File error: unknown error")
	{}
	P4PProtocolFileError(const std::string& what)
		: P4PProtocolError("File error: " + what)
	{}
};

class p4p_common_cpp_ex_EXPORT P4PProtocolConnectionError : public P4PProtocolError
{
public:
	P4PProtocolConnectionError(const std::string& host, unsigned short port)
		: P4PProtocolError(host + ":" + p4p::detail::p4p_token_cast<std::string>(port) + " Connection error: unknown error")
	{}
	P4PProtocolConnectionError(const std::string& host, unsigned short port, const std::string& what)
		: P4PProtocolError(host + ":" + p4p::detail::p4p_token_cast<std::string>(port) + " Connection error: " + what)
	{}
};

}; // namespace protocol
}; // namespace p4p

#endif

