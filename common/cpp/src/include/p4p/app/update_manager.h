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


#ifndef P4P_UPDATE_MANAGER_H
#define P4P_UPDATE_MANAGER_H

#include <queue>
#include <set>
#include <p4p/isp_manager.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/heap_with_delete.h>
#include <p4p/app/pgm_selection_manager.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Manages updating of P4P ISP information and peering guidance
/**
 * This class manages the updating of P4P ISP information and
 * peering guidance.  ISPs indicate the time-to-live of P4P
 * information they provide, and this class handles updating
 * it when necessary.  This class also handles refreshing
 * peering guidance information for each channel or swarm
 * (instances of PGMSelectionManager).
 *
 * It is suggested that the run() method be executed in its
 * own thread.
 */
class p4p_common_cpp_EXPORT P4PUpdateManager
{
public:
	/**
	 * Default constructor.  setISPManager() must be called before
	 * the update manageer is executed using run().
	 */
	P4PUpdateManager();

	/**
	 * Constructor
	 *
	 * @param isp_mgr ISP Manager which manages the P4P-capable ISPs.
	 */
	P4PUpdateManager(const ISPManager* isp_mgr);

	/**
	 * Destructor
	 */
	~P4PUpdateManager();

	/**
	 * Assign the ISPManager that is used to manage the set of P4P-capable ISPs.
	 * Only a single ISPManager may be assigned during the lifetime of an Update
	 * Manager object.  This method returns false if an ISP Manager has already
	 * been configured, either by a constructor or a previous call to setISPManager().
	 *
	 * @param isp_mgr ISP Manager which manages the P4P-capable ISPs.
	 * @returns Returns true if ISP Manager was assigned successfully, and false
	 *   otherwise (e.g., an ISP Manager has already been assigned).
	 */
	bool setISPManager(const ISPManager* isp_mgr);

	/**
	 * Add a PGMSelectionManager to be managed by the update manager. This
	 * should be called when a new channel/swarm is created. The ISPManager
	 * used by this Update Manager must have already been configured either
	 * by the Update Manager constructor or a previous call to setISPManager().
	 *
	 * @param selection_mgr PGMSelectionManager to be added.
	 * @returns Returns true if selection manager was added successfully, and false if
	 *   it was already being managed by the update manager. This method
	 *   also returns false if an ISPManager has not yet been configured.
	 */
	bool addSelectionManager(PGMSelectionManager* selection_mgr);

	/**
	 * Remove a PGMSelectionManager from being managed by the update manager. This
	 * should be called when a channel/swarm is removed.
	 *
	 * @param selection_mgr PGMSelectionManager to be removed.
	 * @returns Returns true if seleciton manager was removed successfully, and false
	 *   if it was not currently being managed.
	 */
	bool removeSelectionManager(PGMSelectionManager* selection_mgr);

	/**
	 * Run the update manager.  This method should be executed in its own
	 * thread.
	 *
	 * @returns Returns true if run was successful, and false otherwise (e.g., if
	 *   an ISPManager has not been configured).
	 */
	bool run();

	/**
	 * Stop the update manager. This causes the run() method to return
	 * as soon as it is done with the current operation.
	 */
	void stop();

private:

	/* Type of task */
	enum TaskType
	{
		TYPE_PIDMAP,		/**< Type indicating that PID Map should be updated */
		TYPE_PDISTANCE,		/**< Type indicating that pDistance Map should be updated */
		TYPE_GUIDANCE,		/**< Type indicating that guidance matrix should be updated */
	};

	/* Encapsulation of a single task to execute */
	struct Task
	{
		/* Constructor */
		Task(time_t exectime_, TaskType type_, ISP* isp_, PGMSelectionManager* sel_mgr_)
			: exectime(exectime_), type(type_), isp(isp_), sel_mgr(sel_mgr_)
		{}

		/* For checking for duplicate tasks; does NOT compare 'exectime' */
		bool operator<(const Task& rhs) const;

		time_t exectime;			/**< Time to execute the task */
		TaskType type;				/**< Type of task */
		ISP* isp;				/**< ISP to update */
		PGMSelectionManager* sel_mgr;		/**< Selection manager to update */
	};

	/* Less-than comparator for tasks (ignores priority) */
	struct TaskPtrLessThan
	{
		bool operator()(const Task* t1, const Task* t2) const	{ return *t1 < *t2; }
	};

	/* Comparator for priority queue */
	struct TaskPtrPriorityComparator
	{
		bool operator()(const Task* t1, const Task* t2) const	{ return t1->exectime < t2->exectime; }
	};

	/** Priority queue of tasks to execute */
	typedef p4p::detail::MaxHeapWithDelete<Task*, TaskPtrLessThan, TaskPtrPriorityComparator> TaskQueue;

	/** Set of tasks */
	typedef std::set<Task*> TaskSet;

	/** Set of tasks for each Selection Manager */
	typedef std::map<PGMSelectionManager*, TaskSet> TasksBySelMgr;

	/** Collection of PGMSelectionManager objects being managed */
	typedef std::set<PGMSelectionManager*> SelectionManagerSet;

	/**
	 * Method for processing the task at the front of the queue.
	 *
	 * @param task Task to execute
	 */
	void executeTask(Task& task);

	/**
	 * Execute a PID Map update task
	 */
	void updatePIDMap(ISP* isp);

	/**
	 * Execute a pDistance Map update task
	 */
	void updatePDistanceMap(ISP* isp);

	/**
	 * Execute a guidance update task
	 */
	void updateGuidance(ISP* isp, PGMSelectionManager* sel_mgr);

	/**
	 * Enqueue a new task.  If a duplicate task is inserted, the one
	 * with the earliest execution time is used.
	 */
	void enqueueTask(time_t exectime, TaskType type, ISP* isp, PGMSelectionManager* sel_mgr = NULL);

	void addTask(Task* task);
	void removeTask(Task* task, bool remove_from_queue = true);

	volatile bool m_stopped;		/**< Indicate if manager has been requested to stop */

	p4p::detail::SharedMutex m_mutex;	/**< Mutex protecting internal data structures */
	const ISPManager* m_isp_mgr;		/**< ISP Manager for P4P-capable ISPs */
	SelectionManagerSet m_selection_mgrs;	/**< Collection of selection managers */
	TaskQueue m_tasks;			/**< Priority Queue of tasks to execute */
	TasksBySelMgr m_selmgr_tasks;		/**< Organize tasks by selection manager */
};

}; // namespace app
}; // namespace p4p

#endif

