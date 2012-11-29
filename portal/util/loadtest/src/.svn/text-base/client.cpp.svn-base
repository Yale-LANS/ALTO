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


#include "client.h"

#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include <time.h>
#include "options.h"
#include "main.h"
#ifdef WIN32
	#include <winsock2.h>
#else
	#include <arpa/inet.h>
#endif

boost::mutex OUTPUT_MUTEX;

enum RequestType
{
	REQ_GETPIDS,
	REQ_GETPDISTANCE,
};

struct Request
{
	Request(boost::system_time _deadline, RequestType _type)
		: deadline(_deadline),
		  type(_type)
	{}

	boost::system_time deadline;
	RequestType type;
};

bool operator<(const Request& r1, const Request& r2)
{
	return r1.deadline > r2.deadline;
}

ClientThread::ClientThread()
	: loc_api_(NULL),
	  pdist_api_(NULL),
	  getpids_calls_(0),
	  getpdistance_calls_(0)
{
	/*
	 * Option parsing
	 */
	getpids_rate_ = OPTIONS["getpids-rate"].as<double>();
	if (getpids_rate_ < 0)
		throw std::runtime_error("Invalid value for getpids-rate");
	getpids_batch_ = OPTIONS["getpids-batch"].as<unsigned int>();

	std::string prefix_str = OPTIONS["getpids-prefix"].as<std::string>();
	std::string::size_type prefix_slash_pos = prefix_str.rfind('/');
	if (prefix_slash_pos == std::string::npos)
		throw std::runtime_error("Invalid value for getpids-prefix");
	getpids_prefix_length_ = boost::lexical_cast<unsigned int>(prefix_str.substr(prefix_slash_pos + 1));
	getpids_prefix_mask_ = (~0) >> getpids_prefix_length_;
	prefix_str = prefix_str.substr(0, prefix_slash_pos);
	struct in_addr addrv4;
	if (inet_pton(AF_INET, prefix_str.c_str(), &addrv4) <= 0)
		throw std::runtime_error("Invalid value for getpids-prefix");
	getpids_prefix_ = ntohl(addrv4.s_addr) & ((~0) << (sizeof(addrv4.s_addr) * 8 - getpids_prefix_length_));

	getpdistance_pids_ = OPTIONS["getpdistance-pids"].as<double>();
	if (getpdistance_pids_ < 0 || getpdistance_pids_ > 1.0)
		throw std::runtime_error("Invalid value for getpdistance-pids");

	getpdistance_rate_ = OPTIONS["getpdistance-rate"].as<double>();
	if (getpdistance_rate_ < 0)
		throw std::runtime_error("Invalid value for getpdistance-rate");
}

ClientThread::~ClientThread()
{
	delete pdist_api_;
	delete loc_api_;
}

bool ClientThread::operator()()
{
	loc_api_ = new LocationPortalProtocol(OPTIONS["server"].as<std::string>(), OPTIONS["port"].as<unsigned short>(), "DEFAULT", OPTIONS.count("persistent") > 0);
	pdist_api_ = new PDistancePortalProtocol(OPTIONS["server"].as<std::string>(), OPTIONS["port"].as<unsigned short>(), "DEFAULT", OPTIONS.count("persistent") > 0);

	boost::posix_time::microseconds getpids_interval(0);
	boost::posix_time::microseconds getpdistance_interval(0);

	std::priority_queue<Request> request_queue;

	if (getpids_rate_ > 0)
	{
		getpids_interval = boost::posix_time::microseconds((unsigned int)((1.0 / getpids_rate_) * 1000000.0));
		request_queue.push(Request(boost::get_system_time() + getpids_interval, REQ_GETPIDS));
	}

	if (getpdistance_rate_ > 0)
	{
		getpdistance_interval = boost::posix_time::microseconds((unsigned int)((1.0 / getpdistance_rate_) * 1000000.0));
		request_queue.push(Request(boost::get_system_time() + getpdistance_interval, REQ_GETPDISTANCE));

		/* Get the list of intradomain PIDs */
		std::vector<PID> empty_pids;
		std::vector<PIDPrefixes> pid_map;
		loc_api_->get_pidmap(empty_pids.begin(), empty_pids.end(), std::back_inserter(pid_map));
		BOOST_FOREACH(const PIDPrefixes& p, pid_map)
		{
			all_pids_.push_back(p.get_pid());
		}
	}

	time_t start_time = time(NULL);

	while (!TERMINATED && !request_queue.empty())
	{
		Request r = request_queue.top();
		request_queue.pop();

		try
		{
			boost::thread::sleep(r.deadline);
		}
		catch (boost::thread_interrupted& e)
		{
			break;
		}

		boost::system_time cur_time = boost::get_system_time();

		switch (r.type)
		{
		case REQ_GETPIDS:
			if (handle_getpids())
				++getpids_calls_;
			request_queue.push(Request(cur_time + getpids_interval, REQ_GETPIDS));
			break;
		case REQ_GETPDISTANCE:
			if (handle_getpdistance())
				++getpdistance_calls_;
			request_queue.push(Request(cur_time + getpdistance_interval, REQ_GETPDISTANCE));
			break;
		}
	}

	time_t end_time = time(NULL);

	print_stats(end_time - start_time);

	return true;
}

bool ClientThread::handle_getpids()
{
	std::vector<std::string> addrs;
	fill_prefix_list(addrs);

	std::vector< std::pair<std::string, PID> > pids;
	pids.reserve(addrs.size());

	try
	{
		loc_api_->get_pids(addrs.begin(), addrs.end(), std::back_inserter(pids));
		return true;
	}
	catch (P4PProtocolError& e)
	{
		std::cerr << "WARNING: Portal API Error: " << e.what() << std::endl;
		return false;
	}
}

bool ClientThread::handle_getpdistance()
{
	std::vector< std::pair<PID, std::vector<PID> > > intra_input;
	intra_input.reserve(all_pids_.size());
	BOOST_FOREACH(const PID& p, all_pids_)
	{
		/* Choose a random destination */
		const PID dst = all_pids_[rand() % all_pids_.size()];
		if (rand() / (double)RAND_MAX < getpdistance_pids_)
		{
			std::vector<PID> dsts;
			dsts.push_back(dst);
			intra_input.push_back(std::make_pair(p, dsts));
		}
	}

	PDistanceMatrix result;
	try
	{
		pdist_api_->get_pdistance(intra_input.begin(), intra_input.end(), false, result);
		return true;
	}
	catch (P4PProtocolError& e)
	{
		std::cerr << "WARNING: Portal API Error: " << e.what() << std::endl;
		return false;
	}
}

void ClientThread::fill_prefix_list(std::vector<std::string>& result)
{
	char buf[128];
	struct in_addr addr;

	result.resize(getpids_batch_);
	for (unsigned int i = 0; i < getpids_batch_; ++i)
	{
		addr.s_addr = htonl(getpids_prefix_ | (rand() & getpids_prefix_mask_));
		result[i] = std::string(inet_ntop(AF_INET, &addr, buf, 128));
	}
}

void ClientThread::print_stats(double elapsed)
{
	boost::mutex::scoped_lock lock(OUTPUT_MUTEX);

	std::cout << "Client " << boost::lexical_cast<std::string>(boost::this_thread::get_id()) << std::endl;
	std::cout << "\tTotal Time (sec) : " << elapsed << std::endl;
	std::cout << "\tGetPIDs: " << std::endl;
	std::cout << "\t\tTotal Requests : " << getpids_calls_ << std::endl;
	std::cout << "\t\tTarget Rate    : " << getpids_rate_ << std::endl;
	std::cout << "\t\tAchieved Rate  : " << (getpids_calls_ / elapsed) << std::endl;
	std::cout << "\tGetPDistance: " << std::endl;
	std::cout << "\t\tTotal Requests : " << getpdistance_calls_ << std::endl;
	std::cout << "\t\tTarget Rate    : " << getpdistance_rate_ << std::endl;
	std::cout << "\t\tAchieved Rate  : " << (getpdistance_calls_ / elapsed) << std::endl;
}
