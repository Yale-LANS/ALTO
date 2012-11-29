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


#include "p4p/app/update_manager.h"

#include <p4p/errcode.h>
#include <p4p/app/errcode.h>

namespace p4p {
namespace app {

bool P4PUpdateManager::Task::operator<(const Task& rhs) const
{
	if (type < rhs.type)		return true;
	if (type > rhs.type)		return false;
	if (isp < rhs.isp)		return true;
	if (isp > rhs.isp)		return false;
	if (sel_mgr < rhs.sel_mgr)	return true;
	if (sel_mgr > rhs.sel_mgr)	return false;
	return false;
}

P4PUpdateManager::P4PUpdateManager()
	: m_stopped(false),
	  m_isp_mgr(NULL)
{

}
P4PUpdateManager::P4PUpdateManager(const ISPManager* isp_mgr)
	: m_stopped(false),
	  m_isp_mgr(NULL)
{
	setISPManager(isp_mgr);
}

P4PUpdateManager::~P4PUpdateManager()
{
	logTrace("Update manager freeing %u tasks left in queue", (unsigned int)m_tasks.size());

	/* Free all tasks still in the queue */
	while (!m_tasks.empty())
	{
		Task* task = m_tasks.top();
		m_tasks.pop();
		delete task;
	}
}

bool P4PUpdateManager::setISPManager(const ISPManager* isp_mgr)
{
	if (m_isp_mgr)
		return false;

	m_isp_mgr = isp_mgr;

	/* Enqueue initial tasks for updating ISP Info. Once we update
	 * the PID Map, a task will automatically be enqueued to update
	 * the pDistance map. */
	std::vector<ISP*> isps;
	m_isp_mgr->listISPs(isps);

	logTrace("Update manager initialized with %u ISPs", (unsigned int)isps.size());

	for (unsigned int i = 0; i < isps.size(); ++i)
	{
		logTrace("Enqueuing update task for ISP %lx", isps[i]);
		enqueueTask(0, TYPE_PIDMAP, isps[i]);
	}

	return true;
}

bool P4PUpdateManager::addSelectionManager(PGMSelectionManager* selection_mgr)
{
	if (!m_isp_mgr)
		return false;

	p4p::detail::ScopedExclusiveLock lock(m_mutex);

	SelectionManagerSet::const_iterator itr = m_selection_mgrs.find(selection_mgr);
	if (itr != m_selection_mgrs.end())
		return false;

	m_selection_mgrs.insert(selection_mgr);

	/* Enqueue initial tasks to update guidance information */
	std::vector<ISP*> isps;
	m_isp_mgr->listISPs(isps);

	for (unsigned int i = 0; i < isps.size(); ++i)
		enqueueTask(0, TYPE_GUIDANCE, isps[i], selection_mgr);

	return true;
}

bool P4PUpdateManager::removeSelectionManager(PGMSelectionManager* selection_mgr)
{
	p4p::detail::ScopedExclusiveLock lock(m_mutex);

	SelectionManagerSet::iterator itr = m_selection_mgrs.find(selection_mgr);
	if (itr == m_selection_mgrs.end())
		return false;

	/* Free all tasks referring to the selection manager */
	TasksBySelMgr::iterator selmgr_tasks_itr = m_selmgr_tasks.find(selection_mgr);
	if (selmgr_tasks_itr != m_selmgr_tasks.end())
	{
		/* Copy tasks to temporary vectory */
		const TaskSet& selmgr_tasks = selmgr_tasks_itr->second;
		std::vector<Task*> tasks;
		tasks.reserve(selmgr_tasks.size());
		std::copy(selmgr_tasks.begin(), selmgr_tasks.end(), std::back_inserter(tasks));

		/* Remove the tasks and free memory */
		for (unsigned int i = 0; i < tasks.size(); ++i)
			removeTask(tasks[i]);

		m_selmgr_tasks.erase(selmgr_tasks_itr);
	}

	/* Remove entry from collection of selection managers */
	m_selection_mgrs.erase(itr);
	return true;
}

bool P4PUpdateManager::run()
{
	if (!m_isp_mgr)
		return false;

	logTrace("Update manager started");

	while (true)
	{
		/* Execute as many tasks as we can before we pause to sleep */
		while (!m_stopped && !m_tasks.empty())
		{
			p4p::detail::ScopedExclusiveLock lock(m_mutex);

			/* Look at the top task on the queue and check
			 * if it is time to execute it. */
			Task* task = m_tasks.top();

			/* We're done for now if the first task should not
			 * yet be executed. */
			if (task->exectime > time(NULL))
				break;

			/* Remove task from the queue and execute */
			m_tasks.pop();

			logTrace("Executing task (type=%d,time=%lu,isp=%lx)",
				(int)task->type, (unsigned long)task->exectime, task->isp);

			executeTask(*task);

			logTrace("Finished executing task");

			/* Task has already been dequeued */
			removeTask(task, false);
		}

		/* Quit if we're signalled to do so */
		if (m_stopped)
		{
			logTrace("Update manager stopping by signal");
			break;
		}

		/* Sleep for a small amount of time before
		 * checking the next task again. */
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	logTrace("Update manager finished");

	return true;
}

void P4PUpdateManager::stop()
{
	m_stopped = true;
}

void P4PUpdateManager::executeTask(Task& task)
{
	/* TODO: Avoid updating ISP info when PID Map Version doesn't change */

	switch (task.type)
	{
	case TYPE_PIDMAP:
		updatePIDMap(task.isp);
		break;
	case TYPE_PDISTANCE:
		updatePDistanceMap(task.isp);
		break;
	case TYPE_GUIDANCE:
		updateGuidance(task.isp, task.sel_mgr);
		break;
	default:
		/* Ignore tasks with invalid type */
		return;
	}
}

void P4PUpdateManager::updatePIDMap(ISP* isp)
{
	/* Get reference to the ISP's PIDMap */
	ISPPIDMap& pidmap = isp->getPIDMap();

	/* Update P4P information */
	int rc = pidmap.loadP4PInfo();
	if (rc != 0)
	{
		/* If there was an error, then we'll just try again later (currently 15 minutes) */
		enqueueTask(time(NULL) + 15 * 60, TYPE_PIDMAP, isp);
		return;
	}

	/* Enqueue a task for the next time the PIDMap needs to be updated */
	enqueueTask(time(NULL) + pidmap.getTTL(), TYPE_PIDMAP, isp);

	/* Enqueue a task to update the pDistance map immediately */
	enqueueTask(0, TYPE_PDISTANCE, isp);
}

void P4PUpdateManager::updatePDistanceMap(ISP* isp)
{
	/* Get reference to the pDistance map */
	ISPPDistanceMap& pdistmap = isp->getPDistanceMap();

	/* Update P4P information */
	int rc = pdistmap.loadP4PInfo();
	if (rc != 0)
	{
		enqueueTask(time(NULL) + 15 + 60, TYPE_PDISTANCE, isp);
		return;
	}

	/* Enqueue a task for the next time pDistances need to be updated */
	enqueueTask(time(NULL) + pdistmap.getTTL(), TYPE_PDISTANCE, isp);

	/* Enqueue tasks for updating guidance immediately. */
	for (SelectionManagerSet::iterator itr = m_selection_mgrs.begin(); itr != m_selection_mgrs.end(); ++itr)
		enqueueTask(0, TYPE_GUIDANCE, isp, *itr);
}

void P4PUpdateManager::updateGuidance(ISP* isp, PGMSelectionManager* sel_mgr)
{
	/* Update peer distributions. Ignore ISP if not found. */
	int rc = sel_mgr->updatePeerDistribution(isp);
	if (rc == ERR_INVALID_ISP)
		return;

	/* Recompute guidance for the desired ISP. Ignore if ISP is not
	 * maintained by this selection manager. */
	rc = sel_mgr->computeGuidance(isp);
	if (rc == ERR_INVALID_ISP)
		return;

	/* Determine the next time selection manager should be updated. Ignore if
	 * we get an error saying ISP is not maintained by the selection manager
	 * (e.g., it could have been removed between computing the guidance and now).*/
	time_t next_compute_time = sel_mgr->getNextComputeTime(isp);
	if (next_compute_time == PGMSelectionManager::TIME_INVALID)
		return;

	/* TODO: Only update swarm-independent if AOE specifies a TTL */

	/* Enqueue a task for the next time guidance should be computed */
	enqueueTask(next_compute_time, TYPE_GUIDANCE, isp, sel_mgr);
}

void P4PUpdateManager::enqueueTask(time_t exectime, TaskType type, ISP* isp, PGMSelectionManager* sel_mgr)
{
	/* Construct task to be enqueued */
	Task* task = new Task(exectime, type, isp, sel_mgr);

	/* Check if we already have the task */
	Task* existing_task = NULL;
	if (!m_tasks.find(task, &existing_task))
	{
		/* Task did not exist already; simply add it */
		addTask(task);
		return;
	}

	/* There was an existing task. Compare execution times. If the existing
	 * task has an earlier execution time, ignore the new task. */
	if (existing_task->exectime <= task->exectime)
	{
		delete task;
		return;
	}

	/* Remove the existing task and add the new one. */
	removeTask(existing_task);
	addTask(task);
}

void P4PUpdateManager::addTask(Task* task)
{
	if (!m_tasks.push(task))
		throw std::runtime_error("Illegal state: failed to add new task");
	if (task->sel_mgr)
		m_selmgr_tasks[task->sel_mgr].insert(task);
}

void P4PUpdateManager::removeTask(Task* task, bool remove_from_queue)
{
	if (remove_from_queue && !m_tasks.erase(task))
		throw std::runtime_error("Illegal state: failed to remove existing task");
	if (task->sel_mgr)
		m_selmgr_tasks[task->sel_mgr].erase(task);
	delete task;
}

};
};

