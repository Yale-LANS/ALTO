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


#ifndef PROTOCOL_SERVER_BASE_H
#define PROTOCOL_SERVER_BASE_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <set>
#include <bitset>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <p4pserver/logging.h>
#include <p4pserver/compiler.h>

class p4p_common_server_EXPORT ProtocolServerBaseAbstract
{
public:
	ProtocolServerBaseAbstract() {}
	virtual ~ProtocolServerBaseAbstract() {}

	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void join() = 0;

};

template <unsigned int MSG_MAX>
class p4p_common_server_ex_EXPORT ProtocolServerBase : public ProtocolServerBaseAbstract
{
public:
	typedef std::bitset<MSG_MAX> MessageTypeSet;

	ProtocolServerBase(const std::string& addr, unsigned short port)
		: ProtocolServerBaseAbstract(),
		  addr_(addr),
		  port_(port),
		  stopped_(false),
		  addr_family_(addr.find(':') != std::string::npos ? AF_INET6 : AF_INET),
		  bg_thread_(NULL),
		  logger_(NULL)
	{
		addr4_.s_addr = INADDR_ANY;
		addr6_ = in6addr_any;
			/*We use addr_ to record user specified ALTO server IP address */
			/* 
		if (addr_family_ == AF_INET && !addr.empty() && inet_pton(addr_family_, addr_.c_str(), &addr4_) <= 0)
			throw std::runtime_error("Invalid IPv4 address: " + addr_);
		else if (addr_family_ == AF_INET6 && !addr.empty() && inet_pton(addr_family_, addr_.c_str(), &addr4_) <= 0)
			throw std::runtime_error("Invalid IPv6 address: " + addr_);
			*/
	}

	virtual ~ProtocolServerBase()
	{
		if (bg_thread_)
			bg_thread_->join();
		delete bg_thread_;
	}

	const std::string& get_addr() const		{ return addr_; }
	unsigned short get_port() const			{ return port_; }
	int get_addr_family() const			{ return addr_family_; }
	const struct in_addr& get_addr4() const		{ return addr4_; }
	const struct in6_addr& get_addr6() const	{ return addr6_; }
	bool is_stopped() const				{ return stopped_; }

	/* Enable/disable messages for this particular handler */
	void enable(const MessageTypeSet& msgs)		{ enabled_msgs_ |= msgs; }
	void disable(const MessageTypeSet& msgs)	{ enabled_msgs_ &= ~msgs; }
	void enable_all()				{ enabled_msgs_.set(); }
	void disable_all()				{ enabled_msgs_.reset(); }

	bool is_enabled(unsigned char msg) const	{ return msg <= MSG_MAX && enabled_msgs_[msg]; }

	virtual void start()				{ bg_thread_ = new boost::thread(boost::bind(&ProtocolServerBase<MSG_MAX>::server_run, this)); }
	virtual void stop()				{ if (stopped_) return; stopped_ = true; this->server_stop(); }
	virtual void join()
	{
		if (!bg_thread_)
			throw std::runtime_error("Illegal state: No background thread created yet");
		bg_thread_->join();
	}

	log4cpp::Category* get_logger() const
	{
		if (!logger_)
			logger_ = create_logger();
		return logger_;
	}

protected:

	void run()					{ this->server_run(); }

	virtual void server_run() = 0;
	virtual void server_stop() = 0;

	virtual std::string get_logger_name() const = 0;

private:
	log4cpp::Category* create_logger() const	{ return &log4cpp::Category::getInstance(get_logger_name()); }

	std::string addr_;
	unsigned short port_;
	volatile bool stopped_;

	int		addr_family_;
	struct in_addr	addr4_;
	struct in6_addr	addr6_;


	MessageTypeSet enabled_msgs_;

	boost::thread* bg_thread_;

	mutable log4cpp::Category* logger_;
};

#endif
