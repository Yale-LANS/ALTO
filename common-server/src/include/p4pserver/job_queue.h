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


#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <set>
#include <p4pserver/logging.h>
#include <p4pserver/compiler.h>

class JobQueue;
typedef boost::shared_ptr<JobQueue> JobQueuePtr;

class Job;
typedef boost::shared_ptr<Job> JobPtr;

class p4p_common_server_EXPORT Job
{
public:
	Job(JobQueuePtr queue, const boost::system_time& deadline)
		: queue_(queue),
		  deadline_(deadline),
		  logger_(NULL)
	{}
	virtual ~Job() {}

	const boost::system_time& get_deadline() const { return deadline_; }

	virtual void run() = 0;

	virtual bool equals(JobPtr rhs) { return false; }

protected:
	JobQueuePtr get_queue() { return queue_; }

	virtual std::string get_logger_name() const = 0;

	virtual JobPtr make_next() = 0;
	bool reschedule();

	log4cpp::Category* get_logger() const
	{
		if (logger_)
			return logger_;

		logger_ = &log4cpp::Category::getInstance(get_logger_name());
		return logger_;
	}

private:
	/* The job queue that we are in */
	JobQueuePtr queue_;

	/* The time at which we are supposed to run */
	boost::system_time deadline_;

	/* Logger for this job */
	mutable log4cpp::Category* logger_;
};

class p4p_common_server_EXPORT JobQueueWorker
{
public:
	JobQueueWorker(JobQueue& queue);

	bool operator()();
private:
	JobQueue& queue_;

	log4cpp::Category* logger_;
};

class p4p_common_server_EXPORT JobQueue
{
public:
	JobQueue(unsigned int pool_size);
	~JobQueue();

	void enqueue(JobPtr task);
	int length();

	void start();
	void stop();
	void join();

private:
	friend class JobQueueWorker;

	class JobPriorityComparator
	{
	public:
		bool operator()(const JobPtr j1, const JobPtr j2) const
		{
			if (!j1.get() || !j2.get())
				throw std::runtime_error("Illegal state: JobQueue contains NULL jobs");

			/* Earlier deadlines come first.  Must reverse notion of 'less than'
			 * to work with the STL priority_queue
			 */
			return j1->get_deadline() < j2->get_deadline();
		}
	};

	typedef std::set<JobPtr, JobPriorityComparator> Queue;
	
	JobPtr dequeue();

	unsigned int pool_size_;
	boost::thread_group threads_;

	boost::condition_variable queue_cond_;
	boost::mutex queue_mutex_;
	Queue queue_;

	bool is_stopping_;

	log4cpp::Category* logger_;

};

#endif
