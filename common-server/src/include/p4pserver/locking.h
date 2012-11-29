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


#ifndef LOCKING_H
#define LOCKING_H

#include <boost/thread/shared_mutex.hpp>
#include <boost/static_assert.hpp>
#include <stdexcept>
#include <p4pserver/compiler.h>

#ifdef DEBUG
	#define LOCK_VIRTUAL virtual
#else
	#define LOCK_VIRTUAL
#endif

template <class T>
class UpgradedWriteLockBase;

class p4p_common_server_ex_EXPORT TryLockFailed : public std::exception {};

class p4p_common_server_EXPORT EmptyLock
{
public:
	LOCK_VIRTUAL ~EmptyLock() {}
};

class p4p_common_server_EXPORT ReadableLock : public EmptyLock
{
public:
	LOCK_VIRTUAL ~ReadableLock() {}
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const = 0;
#else
	void check_read(const boost::shared_mutex& true_mutex) const {}
#endif
};

class p4p_common_server_EXPORT WritableLock : public ReadableLock
{
public:
	LOCK_VIRTUAL ~WritableLock() {}
#ifdef DEBUG
	virtual void check_write(const boost::shared_mutex& true_mutex) const = 0;
#else
	void check_write(const boost::shared_mutex& true_mutex) const {}
#endif
};

template <class T>
class p4p_common_server_ex_EXPORT LockBase
{
public:
	const T* get_obj() const { return obj_; }
protected:
	LockBase(const T& obj) : obj_(&obj) {}
	LockBase() : obj_(NULL) {}
	LOCK_VIRTUAL ~LockBase() {}

	
	/* Debugging methods */
	void check_mutex_common(const boost::shared_mutex& true_mutex, const boost::shared_mutex* my_mutex) const
	{
		if (!my_mutex)
			throw std::runtime_error("Lock not attached to any mutex");

		if (my_mutex != &true_mutex)
			throw std::runtime_error("Lock not attached to expected mutex");
	}

	template <class LockType>
	void check_lock_common(const LockType& lock) const
	{
		if (!lock.owns_lock())
			throw std::runtime_error("Lock not aquired");
	}

	/* shared_lock implementation */
	void check_lock(const boost::shared_mutex& true_mutex, const boost::shared_lock<boost::shared_mutex>& lock) const
	{
		/* FIXME: It looks like boost 1.35.0 has an inconsistency in their documentation
		 *        since they claim that shared_lock has a mutex() method, when it actually
		 *        doesn't.  Since the class layout is the exact same as unique_lock, we'll
		 *        typecast it and access the mutex that way.  This is a very ugly hack and
		 *        it should be removed as soon as the boost code is updated.
		 */
		boost::shared_mutex* my_mutex = static_cast<const boost::unique_lock<boost::shared_mutex>*>(static_cast<const void*>(&lock))->mutex();
		check_mutex_common(true_mutex, my_mutex);
		check_lock_common(lock);
	}

	/* upgrade_lock implementation */
	void check_lock(const boost::shared_mutex& true_mutex, const boost::upgrade_lock<boost::shared_mutex>& lock) const
	{
		/* FIXME: See note above about shared_lock; same applies here */
		boost::shared_mutex* my_mutex = static_cast<const boost::unique_lock<boost::shared_mutex>*>(static_cast<const void*>(&lock))->mutex();
		check_mutex_common(true_mutex, my_mutex);
		check_lock_common(lock);
	}

	/* upgrade_lock implementation */
	void check_lock(const boost::shared_mutex& true_mutex, const boost::upgrade_to_unique_lock<boost::shared_mutex>& lock) const
	{
		/* FIXME: There doesn't appear to be away to get the Mutex object out of this type of lock; Ignore these checks for now */
		/* check_mutex_common(true_mutex, my_mutex); */
		check_lock_common(lock);
	}

	/* unique_lock implementation */
	void check_lock(const boost::shared_mutex& true_mutex, const boost::unique_lock<boost::shared_mutex>& lock) const
	{
		check_mutex_common(true_mutex, lock.mutex());
		check_lock_common(lock);
	}

private:
	const T* obj_;
};

template <class T>
class p4p_common_server_ex_EXPORT NoLockBase : public LockBase<T>, public EmptyLock
{
public:
	NoLockBase(const T& obj) : LockBase<T>(obj) {}
	LOCK_VIRTUAL ~NoLockBase() {}
};

template <class T>
class p4p_common_server_ex_EXPORT ReadLockBase : public LockBase<T>, public ReadableLock
{
public:
	ReadLockBase(const T& obj) : LockBase<T>(obj) {}
	LOCK_VIRTUAL ~ReadLockBase() {}
};

template <class T>
class p4p_common_server_ex_EXPORT BlockReadLockBase : public ReadLockBase<T>
{
public:
	BlockReadLockBase(const T& obj) : ReadLockBase<T>(obj), lock_(obj.get_local_mutex()) {}
	LOCK_VIRTUAL ~BlockReadLockBase() {}
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
#endif
private:
	boost::shared_lock<boost::shared_mutex> lock_;
};

template <class T>
class p4p_common_server_ex_EXPORT TryReadLockBase : public ReadLockBase<T>
{
public:
	TryReadLockBase(const T& obj) : ReadLockBase<T>(obj), lock_(obj.get_local_mutex(), boost::try_to_lock_t())
	{
		if (!lock_.owns_lock())
			throw TryLockFailed();
	}
	LOCK_VIRTUAL ~TryReadLockBase() {}
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
#endif
private:
	boost::shared_lock<boost::shared_mutex> lock_;
};

template <class T>
class p4p_common_server_ex_EXPORT UpgradableReadLockBase : public ReadLockBase<T>
{
public:
	UpgradableReadLockBase(const T& obj) : ReadLockBase<T>(obj), lock_(obj.get_local_mutex()) {}
	LOCK_VIRTUAL ~UpgradableReadLockBase() {}
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
#endif
private:
	friend class UpgradedWriteLockBase<T>;
	mutable boost::upgrade_lock<boost::shared_mutex> lock_;
};

template <class T> 
class p4p_common_server_ex_EXPORT WriteLockBase : public LockBase<T>, public WritableLock
{
public:
	WriteLockBase(const T& obj) : LockBase<T>(obj) {}
	LOCK_VIRTUAL ~WriteLockBase() {}
protected:
	void on_constructed() { this->get_obj()->begin_write(*this); }
	void on_destructed() { this->get_obj()->end_write(*this); }
};

template <class T>
class p4p_common_server_ex_EXPORT UpgradedWriteLockBase : public WriteLockBase<T>
{
public:
	/*
	 * Don't allow compiler to automatically construct a WriteLock given the upgrade one (such as
	 * for passing an UpgradableReadLock as a parameter to something that requires a WriteLock). The
	 * code must explicitly construct it.
	 */
	explicit UpgradedWriteLockBase(const UpgradableReadLockBase<T>& lock) : WriteLockBase<T>(*lock.get_obj()), lock_(lock.lock_) { this->on_constructed(); }
	LOCK_VIRTUAL ~UpgradedWriteLockBase() { this->on_destructed(); }
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
	virtual void check_write(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
#endif
private:
	boost::upgrade_to_unique_lock<boost::shared_mutex> lock_;
};

template <class T>
class p4p_common_server_ex_EXPORT BlockWriteLockBase : public WriteLockBase<T>
{
public:
	BlockWriteLockBase(const T& obj) : WriteLockBase<T>(obj), lock_(obj.get_local_mutex()) { this->on_constructed(); }
	LOCK_VIRTUAL ~BlockWriteLockBase() { this->on_destructed(); }
#ifdef DEBUG
	virtual void check_read(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
	virtual void check_write(const boost::shared_mutex& true_mutex) const { this->check_lock(true_mutex, lock_); }
#endif
private:
	boost::shared_lock<boost::shared_mutex> lock_;
};

#endif
