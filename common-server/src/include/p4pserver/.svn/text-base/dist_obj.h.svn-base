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


#ifndef DIST_OBJ_H
#define DIST_OBJ_H

#include <stdexcept>
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <p4pserver/local_obj.h>
#include <p4pserver/compiler.h>

namespace bfs = boost::filesystem;

class DistributedLock;
class DistributedReadLock;
class DistributedWriteLock;

class p4p_common_server_ex_EXPORT distributed_object_error : public std::runtime_error
{
public:
	distributed_object_error(const std::string& err)
		: std::runtime_error(err)
	{}
};

class DistributedObject;
typedef boost::shared_ptr<DistributedObject> DistributedObjectPtr;
typedef boost::shared_ptr<const DistributedObject> DistributedObjectConstPtr;

class p4p_common_server_EXPORT DistributedObject : public LocalObject, public boost::enable_shared_from_this<DistributedObject>
{
public:
#ifdef P4P_CLUSTER
	typedef boost::archive::binary_iarchive InputArchive;
	typedef boost::archive::binary_oarchive OutputArchive;
#endif

	DistributedObject(const bfs::path& file = bfs::path());
	virtual ~DistributedObject();

#ifdef P4P_CLUSTER
	void commit(const DistributedWriteLock& dist_lock, const ReadableLock& lock) const;
	bool update(const DistributedLock& dist_lock, UpgradableReadLock& lock);
#endif

	const bfs::path& get_file() const { return file_; }
	boost::system_time get_timestamp() const;
	bool get_persistence_enabled() const { return persistence_enabled_; }

	DistributedObjectPtr copy(const ReadableLock& lock);

	unsigned int get_children_count(const ReadableLock& lock) const;

	void get_children(std::vector<DistributedObjectPtr>& result, const ReadableLock& lock) const;

	DistributedObjectPtr get_child(unsigned int i, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		if (i >= children_.size())
			return DistributedObjectPtr();
		return children_[i];
	}

	void set_child(unsigned int i, DistributedObjectPtr value, const WritableLock& lock)
	{
		lock.check_write(get_local_mutex());
		children_.resize(std::max(children_.size(), (size_t)(i + 1)));
		children_[i] = value;
		changed(lock);
	}

	unsigned int find_child(DistributedObjectPtr value, const ReadableLock& lock) const
	{
		lock.check_read(get_local_mutex());
		for (unsigned int i = 0; i < children_.size(); ++i)
			if (children_[i] == value)
				return i;
		return UINT_MAX;
	}

	/* Called automatically after this object has been installed in the global state */
	void updated();

protected:
	/* Must be called by final class at end of constructor */
	void after_construct();

	/* Must be called by final class at start of destructor */
	void before_destruct();

	/* Called automatically by after_construct() if we can't initialize from file */
	virtual void init_default() {};

#ifdef P4P_CLUSTER
	virtual void do_load(InputArchive& stream, const WritableLock& lock) = 0;
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const = 0;
#endif

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock) = 0;

	virtual void on_update(const WritableLock& lock) {}

private:
	/* invalidate NFS (or other filesystem) cache so that new files are noticed */
	void flush_cache();

	/* flag indicating whether persistence is enabled */
	bool persistence_enabled_;

	/* directory containing persistence files */
	bfs::path path_;

	/* file used for persistence */
	bfs::path file_;

	/* timestamp of the loaded file */
	time_t timestamp_;

	/* Child objects */
	std::vector<DistributedObjectPtr> children_;
};

class p4p_common_server_EXPORT DistributedLock
{
public:
	DistributedLock(DistributedObject& object) : object_(object) {}
	virtual ~DistributedLock() {}

	const DistributedObject& get_object() const { return object_; }
	int get_fd() const { return fd_; }

protected:
	virtual int get_open_flags() = 0;
	virtual int get_lock_type() = 0;

	void do_open();
	void do_close();
private:
	DistributedObject& object_;
	int fd_;
};


class p4p_common_server_EXPORT DistributedReadLock : public DistributedLock
{
public:
	DistributedReadLock(DistributedObject& object) : DistributedLock(object) { do_open(); }
	virtual ~DistributedReadLock() { do_close(); }
protected:
	virtual int get_open_flags();
	virtual int get_lock_type();
};

class p4p_common_server_EXPORT DistributedWriteLock : public DistributedLock
{
public:
	DistributedWriteLock(DistributedObject& object) : DistributedLock(object) { do_open(); }
	virtual ~DistributedWriteLock() { do_close(); }
protected:
	virtual int get_open_flags();
	virtual int get_lock_type();
};

#endif
