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


#ifndef P4P_PORTALAPI_ADMIN_PARSING_H
#define P4P_PORTALAPI_ADMIN_PARSING_H

#include <p4p/protocol/detail/parsing.h>
#include <p4p/protocol-portal/admin_types.h>
#include <p4p/protocol-portal/pid_prefixes.h>
#include <p4p/protocol-portal/pdistance_matrix.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace protocol {
namespace portal {
namespace detail {

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseAddressPIDReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseAddressPIDReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			std::string addr;
			if (!read_token(cur_pos, finished, addr, false))
				break;

			PID pid;
			if (!read_token(cur_pos, finished, pid, true))
				break;

			*itr_++ = std::make_pair(addr, pid);

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

class p4p_common_cpp_EXPORT ResponsePDistanceReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponsePDistanceReader(PDistanceMatrix& result)
		: result_(result)
	{
		result_ = PDistanceMatrix();
	}

	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;

		while (true)
		{
			std::string::size_type cur_pos = pos;

			/* Read source PID */
			PID src;
			if (!read_token(cur_pos, finished, src, false))
				break;

			/* Read reverse flag */
			std::string reverse_str;
			if (!read_token(cur_pos, finished, reverse_str, true))
				break;

			bool reverse;
			if (reverse_str == "inc-reverse")
				reverse = true;
			else if (reverse_str == "no-reverse")
				reverse = false;
			else
				throw P4PProtocolParseError();

			/* Read number of destinations */
			unsigned int num_dsts;
			if (!read_token(cur_pos, finished, num_dsts, true))
				break;

			bool failed = false;
			for (unsigned int i = 0; i < num_dsts; ++i)
			{
				/* Read destination PID */
				PID dst;
				if (!read_token(cur_pos, finished, dst, true))
				{
					failed = true;
					break;
				}

				/* Read forward cost */
				unsigned int cost;
				if (!read_token(cur_pos, finished, cost, true))
				{
					failed = true;
					break;
				}

				result_.set(src, dst, cost);

				/* Skip reverse if not present */
				if (!reverse)
					continue;

				if (!read_token(cur_pos, finished, cost, true))
				{
					failed = true;
					break;
				}

				result_.set(dst, src, cost);
			}

			if (failed)
				break;

			pos = cur_pos;
		}

		return pos;
	}
private:
	PDistanceMatrix& result_;
};

template <class InputIterator>
class p4p_common_cpp_ex_EXPORT RequestPDistanceWriter : public p4p::protocol::detail::RequestWriter
{
public:
	RequestPDistanceWriter(InputIterator first, InputIterator last, bool reverse)
		: cur_(first), last_(last), reverse_(reverse ? 1 : 0)
	{}
	virtual bool produce() throw (P4PProtocolError)
	{
		for ( ; cur_ != last_; ++cur_)
		{
			if (!write(cur_->first)) return false;
			if (!write('\t')) return false;
			if (!write(reverse_ ? "inc-reverse" : "no-reverse")) return false;
			if (!write('\t')) return false;
			if (!write(cur_->second.size())) return false;
			if (!write_list(cur_->second.begin(), cur_->second.end(), "\t")) return false;
			if (!write("\r\n")) return false;
			write_checkpoint();
		}

		return true;
	}
private:
	InputIterator cur_;
	InputIterator last_;
	unsigned int reverse_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseNetNodeReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseNetNodeReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			/* Read node name */
			std::string name;
			if (!read_token(cur_pos, finished, name, false))
				break;

			/* Read external indicator */
			std::string external;
			if (!read_token(cur_pos, finished, external, true))
				break;

			*itr_++ = NetNode(name, external == "external");

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseNamedNetLinkReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseNamedNetLinkReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			/* Read link name */
			std::string name;
			if (!read_token(cur_pos, finished, name, false))
				break;

			/* Read link source */
			std::string src;
			if (!read_token(cur_pos, finished, src, true))
				break;

			/* Read link destination */
			std::string dst;
			if (!read_token(cur_pos, finished, dst, true))
				break;

			*itr_++ = NamedNetLink(name, NetLink(src, dst));

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseNamedPIDReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseNamedPIDReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			/* Read pid name */
			std::string name;
			if (!read_token(cur_pos, finished, name, false))
				break;

			/* Read pid asn/pid */
			PID pid;
			if (!read_token(cur_pos, finished, pid, true))
				break;

			*itr_++ = NamedPID(name, pid);

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponsePIDMapReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponsePIDMapReader(OutputIterator itr) : itr_(itr), pid_state_(0), prefix_count_(0) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;

		while (true)
		{
			PID pid;

			std::string::size_type cur_pos = pos;
			switch (pid_state_)
			{
			case 0:
				/* Read source PID */
				if (!read_token(cur_pos, finished, pid, false))
					return pos;
				cur_record_ = PIDPrefixes(pid);
				pos = cur_pos;
				++pid_state_;
			case 1:
				/* Read prefix count */
				if (!read_token(cur_pos, finished, prefix_count_, true))
					return pos;
				pos = cur_pos;
				++pid_state_;
			case 2:
				/* Read prefixes */
				while (cur_record_.num_prefixes() < prefix_count_)
				{
					IPPrefix prefix;
					if (!read_token(cur_pos, finished, prefix, true))
						return pos;
					pos = cur_pos;
					cur_record_.add_prefix(prefix);
				}
				++pid_state_;
			case 3:
				/* Reset state */
				pid_state_ = 0;
				*itr_++ = cur_record_;
			}
		}
	}
private:
	OutputIterator itr_;
	unsigned int pid_state_;
	unsigned int prefix_count_;
	PIDPrefixes cur_record_;
};

template <class OutputIterator>
class p4p_common_cpp_ex_EXPORT ResponseNamedPIDLinkReader : public p4p::protocol::detail::ResponseReader
{
public:
	ResponseNamedPIDLinkReader(OutputIterator itr) : itr_(itr) {}
	virtual size_t consume(bool finished) throw (P4PProtocolError)
	{
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type cur_pos = pos;

			/* Read link name */
			std::string name;
			if (!read_token(cur_pos, finished, name, false))
				break;

			/* Read source */
			std::string src;
			if (!read_token(cur_pos, finished, src, true))
				break;

			/* Read destination */
			std::string dst;
			if (!read_token(cur_pos, finished, dst, true))
				break;

			*itr_++ = NamedPIDLink(name, PIDNameLink(src, dst));

			pos = cur_pos;
		}
		return pos;
	}
private:
	OutputIterator itr_;
};

}; // namespace detail
}; // namespace portal
}; // namespace protocol
}; // namespace p4p

#endif

