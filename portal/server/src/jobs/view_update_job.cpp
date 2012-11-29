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


#include "view_update_job.h"

#include <stdexcept>
#include <boost/foreach.hpp>
#include <p4pserver/locking.h>
#include "state.h"
#include "plugin_base.h"
#include "view_update.h"

ViewUpdateJob::ViewUpdateJob(JobQueuePtr queue, const boost::system_time& deadline, const std::string& name)
	: Job(queue, deadline),
	  name_(name),
	  interval_(0)
{
}

void ViewUpdateJob::run()
{
	get_logger()->info("starting update");

	get_logger()->debug("acquiring locks");
	GlobalStatePtr global_state = GLOBAL_STATE;
	UpgradableReadLock global_state_lock(*global_state);

	NetStatePtr net_state = global_state->get_net(global_state_lock);
	BlockReadLock net_state_lock(*net_state);

	ViewRegistryPtr views = global_state->get_views(global_state_lock);
	UpgradableReadLock views_lock(*views);

	ViewPtr view = views->get(name_, views_lock);
	if (!view)
	{
		get_logger()->warn("Could not find view: %s; cancelling job", name_.c_str());
		return;
	}

	ViewUpdateStateUpgrade view_state(view);

	interval_ = ViewUpdateUpgrade(global_state, global_state_lock,
				      net_state, net_state_lock,
				      views, views_lock,
				      view_state).do_update();

	if (interval_ > 0)
	{
		get_logger()->info("update succeeded; rescheduling");
		reschedule();
	}
	else
		get_logger()->info("update failed; NOT rescheduling");
}

bool ViewUpdateJob::equals(JobPtr job)
{
	boost::shared_ptr<ViewUpdateJob> view_job = boost::dynamic_pointer_cast<ViewUpdateJob>(job);
	if (!view_job)
		return false;

	return name_ == view_job->name_;
}

JobPtr ViewUpdateJob::make_next()
{
	if (interval_ == 0)
		return JobPtr();

	boost::posix_time::time_duration interval = boost::posix_time::seconds(interval_);
	return JobPtr(new ViewUpdateJob(get_queue(), boost::get_system_time() + interval, name_));
}
