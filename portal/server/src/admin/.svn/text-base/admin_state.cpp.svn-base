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


#include "admin_state.h"

#include <assert.h>
#include <boost/static_assert.hpp>
#include "state.h"
#include "options.h"

const AdminState::Token AdminState::INVALID_TOKEN = 0;

class TxnMonitor
{
public:
	TxnMonitor(AdminState* admin, const AdminState::Token& token, const boost::posix_time::time_duration& expire_time)
		: admin_(admin),
		  token_(token),
		  expire_time_(expire_time)
	{}

	void operator()()
	{
		while (true)
		{
			try
			{
				boost::this_thread::sleep(expire_time_);
				admin_->txn_rollback(token_);
				return;
			}
			catch (boost::thread_interrupted& e)
			{
				/* Quit if the transaction is over */
				if (admin_->get_token() != token_)
					break;

				/* Reset the expire timer */
				continue;
			}
			catch (admin_error& e)
			{
				admin_->get_logger().error("Automatic transaction rollback failed: %s", e.what());
			}
		}
	}

private:
	AdminState* admin_;
	AdminState::Token token_;
	boost::posix_time::time_duration expire_time_;
};

AdminState::ReadLockRecord AdminAction::get_net_read() throw (admin_error)
{
	AdminState::WriteLockRecord global_state_rec = get_admin_state()->root_ref();
	return get_admin_state()->read_object_ref(global_state_rec, boost::dynamic_pointer_cast<GlobalState>(global_state_rec.first)->get_net(*global_state_rec.second));
}

AdminState::WriteLockRecord AdminAction::get_net_write() throw (admin_error)
{
	AdminState::WriteLockRecord global_state_rec = get_admin_state()->root_ref();
	return get_admin_state()->write_object_ref(global_state_rec, boost::dynamic_pointer_cast<GlobalState>(global_state_rec.first)->get_net(*global_state_rec.second));
}

AdminState::ReadLockRecord AdminAction::get_views_read() throw (admin_error)
{
	AdminState::WriteLockRecord global_state_rec = get_admin_state()->root_ref();
	return get_admin_state()->read_object_ref(global_state_rec, boost::dynamic_pointer_cast<GlobalState>(global_state_rec.first)->get_views(*global_state_rec.second));
}

AdminState::WriteLockRecord AdminAction::get_views_write() throw (admin_error)
{
	AdminState::WriteLockRecord global_state_rec = get_admin_state()->root_ref();
	return get_admin_state()->write_object_ref(global_state_rec, boost::dynamic_pointer_cast<GlobalState>(global_state_rec.first)->get_views(*global_state_rec.second));
}

bool AdminAction::commit(AdminState* admin) throw (admin_error)
{
	admin_state_ = admin;
	return true;
}

bool AdminAction::rollback(AdminState* admin)
{
	return true;
}

bool AdminState::string_to_token(const std::string& s, Token& token)
{
	if (s.empty())
		return false;

	BOOST_STATIC_ASSERT(sizeof(unsigned long long int) == sizeof(Token));

	char* errptr;
	token = strtoull(s.c_str(), &errptr, 16);
	return *errptr == '\0';
}

std::string AdminState::token_to_string(const Token& token)
{
	/* Hex has 2 characters per byte */
	static const unsigned int STR_LEN = sizeof(Token) * 2;
	BOOST_STATIC_ASSERT(16 == STR_LEN);

	char buf[STR_LEN + 1];  /* Leave room for null-terminator */
	buf[STR_LEN] = '\0';

	BOOST_STATIC_ASSERT(sizeof(unsigned long long int) == sizeof(Token));
	int rc = snprintf(buf, STR_LEN + 1, "%016llx", (unsigned long long int)token);
	assert(rc == (int)STR_LEN);
	rc = 0; /* Causes compiler not to warn about unused variable when asserts not compiled in */
	return std::string(buf);
}

AdminState::AdminState()
	: logger_(log4cpp::Category::getInstance("Admin")),
	  token_(INVALID_TOKEN),
	  monitor_thread_(NULL)
{
}

AdminState::~AdminState()
{
	/* Clear any pending transaction */
	clear_txn_state();
}

void AdminState::generate_token()
{
#ifdef DEBUG
	token_ = 1;
#else
	try
	{
		for (unsigned int i = 0; i < MAX_TOKEN_GENERATE_ATTEMPTS; ++i)
		{
			try
			{
				/* Generate 64-bit integer token */
				assert(sizeof(RNG::result_type) * 2 == sizeof(Token));
				union
				{
					RNG::result_type rng[2];
					Token token;
				} rng;
				rng.rng[0] = rng_();
				rng.rng[1] = rng_();
				token_ = rng.token;

				/* Return if we got a valid token */
				if (token_ != INVALID_TOKEN)
					return;
			}
			catch (std::ios_base::failure& e)
			{
				throw admin_token_generation_error(e.what());
			}
		}

		throw admin_token_generation_error("Generated invalid token too many times");
	}
	catch (...)
	{
		/* If anything goes wrong, be sure that we don't leave
		 * some unknown token around */
		token_ = INVALID_TOKEN;
		throw;
	}
#endif
}

void AdminState::check_txn_token(const Token& token)
{
	if (token_ == INVALID_TOKEN)
		throw admin_no_txn_error();

	if (token_ != token)
		throw admin_wrong_token_error();
}

void AdminState::clear_txn_state()
{
	/* Clear write locks */
	clear_locks(write_locks_);

	/* Clear original read locks */
	clear_locks(old_state_locks_);

	/* Don't hold any more global state */
	new_state_ = GlobalStatePtr();

	/* Clear changelogs */
	unapplied_changes_.clear();
	applied_changes_.clear();

	/* Reset the token */
	token_ = INVALID_TOKEN;

	/* Send the monitor thread an interrupt to tell it to stop. The thread
	 * looks for the current token being different than the current token
	 * it has, so it is important that we reset the token before calling
	 * the interrupt. */
	if (monitor_thread_)
	{
		monitor_thread_->interrupt();
		monitor_thread_->detach();
		delete monitor_thread_;
		monitor_thread_ = NULL;
	}
}

AdminState::Token AdminState::txn_begin(const TxnType& type)
{
	boost::mutex::scoped_lock l(mutex_);

	get_logger().info("beginning transaction");

	if (token_ != INVALID_TOKEN)
		throw admin_duplicate_txn_error();

	generate_token();
	txn_type_ = type;

	/* Acquire upgradable read locks for each item; in global state */
	{
		get_logger().debug("acquiring read locks for current state");

		std::vector<DistributedObjectPtr> old_state_objs;

		/* Start off with the root object */
		old_state_objs.push_back(GLOBAL_STATE);

		/* Recurse into the structure and get upgradable read locks for each object */
		unsigned int idx = 0;
		while (idx < old_state_objs.size())
		{
			/* Get the next unprocessed object */
			DistributedObjectPtr obj = old_state_objs[idx];

			/* Acquire lock for the object */
			boost::shared_ptr<const UpgradableReadLock> lock = old_state_locks_[obj] = boost::shared_ptr<const UpgradableReadLock>(new UpgradableReadLock(*obj));

			/* Append the children of the object to our vector */
			std::vector<DistributedObjectPtr> children;
			obj->get_children(children, *lock);
			std::copy(children.begin(), children.end(), std::back_inserter(old_state_objs));

			/* Move to the next object */
			++idx;
		}
	}

	/* Create a shallow copy of the global state */
	get_logger().debug("creating shallow copy of current state");
	boost::shared_ptr<const UpgradableReadLock> old_root_lock = find_lock(old_state_locks_, GLOBAL_STATE);
	new_state_ = boost::dynamic_pointer_cast<GlobalState>(GLOBAL_STATE->copy(*old_root_lock));

	/* Allocate a write lock for it */
	write_locks_[new_state_] = boost::shared_ptr<const WritableLock>(new BlockWriteLock(*new_state_));

	/* Create the new monitor thread to rollback incomplete transactions */
	get_logger().debug("creating transaction monitor thread");
	assert(monitor_thread_ == NULL);
	monitor_thread_ = new boost::thread(TxnMonitor(this, token_, boost::posix_time::seconds(OPTIONS["admin-txn-timeout"].as<unsigned int>())));

	if (get_logger().isInfoEnabled())
		get_logger().info("created transaction with token %s", token_to_string(token_).c_str());

	return token_;
}

AdminActionPtr AdminState::txn_apply(const Token& token, AdminActionPtr action)
{
	boost::mutex::scoped_lock l(mutex_);
	check_txn_token(token);

	/* Notify the monitor thread that the transaction is still alive */
	monitor_thread_->interrupt();

	get_logger().debug("adding action to changelog");
	unapplied_changes_.push_back(action);

	/* If we're making copies as we go, then perform
	 * the changes immediately.  Otherwise, everything
	 * gets applied all at once at the end */
	if (txn_type_ == TXN_COPY_ON_WRITE)
	{
		get_logger().debug("applying actions in changelog");
		if (!process_changes())
		{
			/* If an error occurred, quit the transaction immediately
			 * and return an empty pointer */
			get_logger().error("failed to apply action; cleaning up transaction");
			clear_txn_state();
			return AdminActionPtr();
		}

		/* Clear the log of applied changes (rolling back in this mode
		 * doesn't require them. */
		applied_changes_.clear();
	}

	/* Notify the monitor thread that the transaction is still alive.
	 * This is done a second time to handle actions that take longer
	 * to perform, so we can give the client a bit more time to submit
	 * its next request. */
	monitor_thread_->interrupt();

	get_logger().debug("successfully processed action");

	return action;
}

bool AdminState::txn_commit(const Token& token)
{
	boost::mutex::scoped_lock l(mutex_);
	check_txn_token(token);

	get_logger().info("committing transaction");

	/* Apply any pending changes */
	get_logger().debug("applying pending changes");
	if (!process_changes())
	{
		get_logger().error("processing changes failed; cleaning up transaction");
		clear_txn_state();
		return false;
	}

	{
		/* Use our new global state in place of the old one */
		get_logger().debug("acquiring write lock for current state");
		UpgradedWriteLock write_lock(*find_lock(old_state_locks_, GLOBAL_STATE));
		get_logger().debug("updating current state");
		GLOBAL_STATE->apply(new_state_, *find_lock(write_locks_, new_state_), write_lock);
		get_logger().debug("finished applying changes to current state");
	}

	get_logger().debug("finished applying changes; cleaning up transaction");
	clear_txn_state();

	get_logger().debug("signaling current state that it has been updated");
	GLOBAL_STATE->updated();

	get_logger().info("successfully committed transaction");

	return true;
}

void AdminState::txn_rollback(const Token& token)
{
	boost::mutex::scoped_lock l(mutex_);
	check_txn_token(token);

	get_logger().info("rolling back transaction");

	clear_txn_state();

	get_logger().info("successfully rolled back transaction");
}

void AdminState::rollback_changes()
{
	/* Rollback changes in reverse order */
	get_logger().info("rolling back previously-applied changes");
	while (!applied_changes_.empty())
	{
		AdminActionPtr action = applied_changes_.back();
		action->rollback(this);
		applied_changes_.pop_back();
	}
}

bool AdminState::process_changes()
{
	while (!unapplied_changes_.empty())
	{
		AdminActionPtr action = unapplied_changes_.front();

		bool success;
		try
		{
			success = action->commit(this);
			if (!success)
				get_logger().error("failed to apply action: action returned false");
		}
		catch (admin_error& e)
		{
			get_logger().error("failed to apply action: %s", e.what());
			success = false;
		}

		if (!success)
		{
			rollback_changes();
			return false;
		}

		/* Change applied successfully */
		get_logger().debug("successfully applied all actions");
		applied_changes_.push_back(action);
		unapplied_changes_.pop_front();
	}

	/* All changes applied successfully */
	return true;
}

AdminState::WriteLockRecord AdminState::root_ref()
{
	return WriteLockRecord(new_state_, write_locks_[new_state_]);
}

AdminState::ReadLockRecord AdminState::read_object_ref(const ReadLockRecord& parent, DistributedObjectPtr obj)
{
	/* Check the lock for the parent */
	parent.second->check_read(parent.first->get_local_mutex());

	/* See if we've got a write lock for it already */
	boost::shared_ptr<const WritableLock> r_write = find_lock(write_locks_, obj);
	if (r_write)
		return ReadLockRecord(obj, r_write);

	/* Return the existing read lock */
	boost::shared_ptr<const UpgradableReadLock> r_read = find_lock(old_state_locks_, obj);
	if (r_read)
		return ReadLockRecord(obj, r_read);

	/* No existing object; ensure it is actually a child */
	if (parent.first->find_child(obj, *parent.second) == UINT_MAX)
		return ReadLockRecord(); /* Error condition; invalid object passed in */

	/* Allocate new write lock */
	return ReadLockRecord(obj, write_locks_[obj] = boost::shared_ptr<const WritableLock>(new BlockWriteLock(*obj)));
}

AdminState::WriteLockRecord AdminState::write_object_ref(const WriteLockRecord& parent, DistributedObjectPtr obj)
{
	/* Check the lock for the parent */
	parent.second->check_write(parent.first->get_local_mutex());

	/* See if we've got a write lock for it already */
	boost::shared_ptr<const WritableLock> r_write = find_lock(write_locks_, obj);
	if (r_write)
		return WriteLockRecord(obj, r_write);

	/* Locate the existing read lock; we'll need it below */
	boost::shared_ptr<const UpgradableReadLock> read_lock = find_lock(old_state_locks_, obj);
	if (!read_lock)
	{
		/* Couldn't find an existing object; ensure it is actually a child */
		if (parent.first->find_child(obj, *parent.second) == UINT_MAX)
			return WriteLockRecord(); /* Error condition; invalid object passed in */

		/* Allocate a new write lock */
		return WriteLockRecord(obj, write_locks_[obj] = boost::shared_ptr<const WritableLock>(new BlockWriteLock(*obj)));
	}

	/* No write lock exists; our action depends on the transaction mode */
	if (txn_type_ == TXN_COPY_ON_WRITE)
	{
		/* Determine the child index for this object */
		unsigned int child_idx = parent.first->find_child(obj, *parent.second);
		if (child_idx == UINT_MAX)
			return WriteLockRecord(); /* Error condition; no parent/child relationship */

		/* Make the shallow copy of the child object */
		parent.first->set_child(child_idx, obj = obj->copy(*read_lock), *parent.second);

		/* Write lock is a direct lock on the object itself (NOT an
		 * upgrade of the existing lock) */
		return WriteLockRecord(obj, write_locks_[obj] = boost::shared_ptr<const WritableLock>(new BlockWriteLock(*obj)));
	}
	else
	{
		/* Upgrade the existing read lock to a write lock */
		return WriteLockRecord(obj, write_locks_[obj] = boost::shared_ptr<const WritableLock>(new UpgradedWriteLock(*read_lock)));
	}
}
