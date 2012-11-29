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


#ifndef ADMIN_STATE_H
#define ADMIN_STATE_H

#include <stdexcept>
#include <queue>
#include <map>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <p4pserver/boost_random.h>
#include <p4pserver/logging.h>
#include "global_state.h"

class admin_error : public std::runtime_error
{
public:
	admin_error(const std::string& err) : std::runtime_error(err) {}
};

class admin_no_txn_error : public admin_error
{
public:
	admin_no_txn_error() : admin_error("No transaction in progress") {}
};

class admin_duplicate_txn_error : public admin_error
{
public:
	admin_duplicate_txn_error() : admin_error("Transaction already in progress") {}
};

class admin_wrong_token_error : public admin_error
{
public:
	admin_wrong_token_error() : admin_error("Incorrect token") {}
};

class admin_token_generation_error : public admin_error
{
public:
	admin_token_generation_error(const std::string& err) : admin_error("Failed to generate token: " + err) {}
};

class AdminState;
typedef boost::shared_ptr<AdminState> AdminStatePtr;

class AdminAction;
typedef boost::shared_ptr<AdminAction> AdminActionPtr;

class AdminState
{
public:
	typedef boost::random_device RNG;
	typedef uint64_t Token;

	enum TxnType
	{
		TXN_DIRECT_WRITE,
		TXN_COPY_ON_WRITE,
	};

	static const Token INVALID_TOKEN;

	static bool string_to_token(const std::string& s, Token& token);
	static std::string token_to_string(const Token& token);

	AdminState();
	~AdminState();

	Token get_token() const { return token_; }
	TxnType get_txn_type() const { return txn_type_; }

	Token txn_begin(const TxnType& type);

	AdminActionPtr txn_apply(const Token& token, AdminActionPtr action);

	bool txn_commit(const Token& token);
	void txn_rollback(const Token& token);

	/* Methods for use by classes derived from AdminAction */

	typedef std::pair<DistributedObjectPtr, boost::shared_ptr<const ReadableLock> > ReadLockRecord;
	typedef std::pair<DistributedObjectPtr, boost::shared_ptr<const WritableLock> > WriteLockRecord;

	/* Get the current state being modified */
	WriteLockRecord root_ref();
	ReadLockRecord read_object_ref(const ReadLockRecord& parent, DistributedObjectPtr obj);
	WriteLockRecord write_object_ref(const WriteLockRecord& parent, DistributedObjectPtr obj);

	log4cpp::Category& get_logger() { return logger_; }

private:
	typedef std::list<AdminActionPtr> ActionLog;

	template <class LockType>
	void clear_locks(std::map<DistributedObjectPtr, LockType>& ring)
	{
		ring.clear();
	}

	template <class LockType>
	LockType find_lock(const std::map<DistributedObjectPtr, LockType>& ring, DistributedObjectPtr obj)
	{
		return (ring.find(obj) != ring.end()) ? ring.find(obj)->second : LockType();
	}

	static const unsigned int MAX_TOKEN_GENERATE_ATTEMPTS = 3;

	void generate_token();
	void check_txn_token(const Token& token);

	void rollback_changes();
	bool process_changes();

	DistributedObjectPtr get_target_object(DistributedObjectPtr obj);
	boost::shared_ptr<const ReadableLock> get_read_lock(DistributedObjectPtr obj);
	boost::shared_ptr<const WritableLock> get_write_lock(DistributedObjectPtr obj);

	void clear_txn_state();

	//mutable 
	log4cpp::Category& logger_;

	RNG rng_;
	boost::mutex mutex_;

	Token token_;
	TxnType txn_type_;

	GlobalStatePtr new_state_;

	std::map<DistributedObjectPtr, boost::shared_ptr<const UpgradableReadLock> > old_state_locks_;
	std::map<DistributedObjectPtr, boost::shared_ptr<const WritableLock> > write_locks_;

	ActionLog unapplied_changes_;
	ActionLog applied_changes_;

	boost::thread* monitor_thread_;
};

class AdminAction
{
public:
	AdminAction() : admin_state_(NULL) {}
	virtual ~AdminAction() {}

	virtual bool commit(AdminState* admin) throw (admin_error);
	virtual bool rollback(AdminState* admin);

	AdminState::ReadLockRecord get_net_read() throw (admin_error);
	AdminState::WriteLockRecord get_net_write() throw (admin_error);

	AdminState::ReadLockRecord get_views_read() throw (admin_error);
	AdminState::WriteLockRecord get_views_write() throw (admin_error);

	AdminState* get_admin_state() { return admin_state_; }

private:
	AdminState* admin_state_;
};

#endif
