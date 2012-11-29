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


#ifndef LOCAL_OBJ_H
#define LOCAL_OBJ_H

#include <map>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <p4pserver/locking.h>
#include <p4pserver/compiler.h>

class LocalObject;
typedef boost::shared_ptr<LocalObject> LocalObjectPtr;
typedef boost::shared_ptr<const LocalObject> LocalObjectConstPtr;

typedef NoLockBase<LocalObject> NoLock;
typedef BlockReadLockBase<LocalObject> BlockReadLock;
typedef TryReadLockBase<LocalObject> TryReadLock;
typedef UpgradableReadLockBase<LocalObject> UpgradableReadLock;
typedef BlockWriteLockBase<LocalObject> BlockWriteLock;
typedef UpgradedWriteLockBase<LocalObject> UpgradedWriteLock;

class p4p_common_server_EXPORT LocalObject : private boost::noncopyable
{
public:
	virtual ~LocalObject();

	boost::shared_mutex& get_local_mutex() const { return mutex_; }

	/* Called automatically when object is locked for writing */
	void begin_write(const WritableLock& lock) const;
	void end_write(const WritableLock& lock) const;

	unsigned int get_version(const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		return version_;
	}

protected:
	LocalObject();
	virtual LocalObjectPtr create_empty_instance() const = 0;

	/* Notification (from descendent class) that the object has been modified */
	void changed(const WritableLock& lock);

	/* Event handler fired the first time the object is modified since it has been locked */
	virtual void on_changed(const WritableLock& lock);

private:
	mutable boost::shared_mutex mutex_;

	mutable bool writing_;
	mutable bool changed_fired_;

	unsigned int version_;
};

#endif
