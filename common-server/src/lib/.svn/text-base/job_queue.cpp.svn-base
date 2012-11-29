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


#include "p4pserver/job_queue.h"

#include <boost/lexical_cast.hpp>

JobQueueWorker::JobQueueWorker(JobQueue& queue)
	: queue_(queue)
{
}

bool JobQueueWorker::operator()()
{
	logger_ = &log4cpp::Category::getInstance("JobQueueWorker(" + boost::lexical_cast<std::string>(boost::this_thread::get_id()) + ")");

	while (true)
	{
		/*
		 * Get the next work item to do
		 */
		JobPtr job = queue_.dequeue();
		if (!job)
			return true;

		logger_->debug("beginning job");
		job->run();
		logger_->debug("finished job");
	}
}

JobQueue::JobQueue(unsigned int pool_size)
	: pool_size_(pool_size),
	  is_stopping_(false),
	  logger_(&log4cpp::Category::getInstance("JobQueue"))
{
}

JobQueue::~JobQueue()
{
	stop();
	join();

	queue_.clear();
}

bool Job::reschedule()
{
	JobPtr next = make_next();
	if (!next)
		return false;

	queue_->enqueue(next);
	return true;
}

void JobQueue::start()
{
	if (threads_.size() > 0)
		throw std::runtime_error("Illegal state: JobQueue already started");

	logger_->debug("spawning worker threads");
	for (unsigned int i = 0; i < pool_size_; ++i)
		threads_.add_thread(new boost::thread(JobQueueWorker(*this)));
}

void JobQueue::stop()
{
	boost::unique_lock<boost::mutex> lock(queue_mutex_);
	logger_->debug("sending stop signal to worker threads");
	is_stopping_ = true;
	queue_cond_.notify_all();
}

void JobQueue::join()
{
	logger_->debug("waiting for worker threads to complete");
	threads_.join_all();
	logger_->debug("all worker threads are finished");
}

void JobQueue::enqueue(JobPtr job)
{
	if (!job)
		throw std::runtime_error("Illegal state: Tried to insert NULL job into JobQueue");

	boost::unique_lock<boost::mutex> lock(queue_mutex_);

	/* Ensure a duplicate job isn't already present.  If one is
	 * already there, don't add the new one.
	 * FIXME: This currently doesn't handle the case when the
	 *        duplicate job is currently being executed. */
	for (Queue::iterator itr = queue_.begin(); itr != queue_.end(); ++itr)
	{
		JobPtr j = *itr;

		if (!job->equals(j))
			continue;

		/* Found a duplicate job */
		if (JobPriorityComparator()(job, j))
		{
			/* New job comes before existing job; replace
			 * the existing job */
			queue_.erase(itr);
			break;
		}
		else
		{
			/* New job comes after existing job; drop
			 * the new job */
			return;
		}
	}

	queue_.insert(job);
	queue_cond_.notify_one();
}

JobPtr JobQueue::dequeue()
{
	logger_->debug("dequeuing job to thread " + boost::lexical_cast<std::string>(boost::this_thread::get_id()));

	boost::unique_lock<boost::mutex> lock(queue_mutex_);

	if (is_stopping_)
		JobPtr();

	while (!is_stopping_ && (queue_.empty() || boost::get_system_time() < (*queue_.begin())->get_deadline()))
	{
		if (queue_.empty())
			queue_cond_.wait(lock);
		else
			queue_cond_.timed_wait(lock, (*queue_.begin())->get_deadline());
	}

	if (is_stopping_)
		return JobPtr();

	JobPtr result = *queue_.begin();
	queue_.erase(queue_.begin());
	return result;
}

int JobQueue::length()
{
	boost::unique_lock<boost::mutex> lock(queue_mutex_);

	return queue_.size();
}
