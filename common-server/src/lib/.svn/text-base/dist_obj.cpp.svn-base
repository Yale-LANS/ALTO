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


#include "p4pserver/dist_obj.h"

#include <boost/foreach.hpp>
#include <boost/system/error_code.hpp>
#include <fstream>
#include <fcntl.h>

namespace bs = boost::system;

DistributedObject::DistributedObject(const bfs::path& file)
	: persistence_enabled_(false),
	  path_(bfs::path()),
	  file_(!path_.string().empty() && !file.string().empty() ? (path_ / file) : bfs::path()),
	  timestamp_(0)
{
}

DistributedObject::~DistributedObject()
{
}

void DistributedObject::after_construct()
{
#ifdef P4P_CLUSTER
	if (persistence_enabled_ && !file_.string().empty())
	{
		/*
		 * Initialize from file if one was specified
		 */
		try
		{
			DistributedReadLock dist_lock(*this);
			UpgradableReadLock local_lock(*this);
			if (!update(dist_lock, local_lock))
				init_default();
		}
		catch (distributed_object_error& e)
		{
			init_default();
		}

		/* FIXME: Temporarily disabled */
		/* DistributedObjectWatcher::Instance().add_object(this); */
	}
	else
#endif
	{
		init_default();
	}
}

void DistributedObject::before_destruct()
{
	/* FIXME: Temporarily disabled */
	/*
	if (persistence_enabled_ && !file_.string().empty())
		DistributedObjectWatcher::Instance().remove_object(this);
	*/
}

#ifdef P4P_CLUSTER
void DistributedObject::commit(const DistributedWriteLock& dist_lock, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());

	if (!persistence_enabled_)
	{
		const_cast<DistributedObject*>(this)->timestamp_ = time(NULL);
		return;
	}

	if (&dist_lock.get_object() != this)
		throw std::runtime_error("Distributed lock has incorrect object");
	if (file_.string().empty())
		throw std::runtime_error("Distributed object has no associated file");

	try
	{
		std::ofstream file_stream(get_file().string().c_str());
		if (!file_stream)
			throw bfs::filesystem_error("open failed", get_file(), bs::error_code(errno, boost::system::posix_category));

		OutputArchive file_arch(file_stream);
		do_save(file_arch, lock);
		file_stream.flush();
		const_cast<DistributedObject*>(this)->timestamp_ = bfs::last_write_time(file_);
	}
	catch (bfs::filesystem_error& e)
	{
		throw distributed_object_error("file error: " + std::string(e.what()));
	}
	catch (boost::archive::archive_exception& e)
	{
		throw distributed_object_error("serialization error: " + std::string(e.what()));
	}
}

bool DistributedObject::update(const DistributedLock& dist_lock, UpgradableReadLock& lock)
{
	lock.check_read(get_local_mutex());

	if (!persistence_enabled_)
		return false;

	throw std::runtime_error("Persistence currently not implemented");
#if 0
	if (&dist_lock.get_object() != this)
		throw std::runtime_error("Distributed lock has incorrect object");
	if (file_.string().empty())
		throw std::runtime_error("Distributed object has no associated file");

	try
	{
		{
			flush_cache();

			/* load from the file */
			std::ifstream file_stream(get_file().string().c_str());
			if (!file_stream)
				throw bfs::filesystem_error("open failed", get_file(), bs::error_code(errno, boost::system::posix_category));

			/* see if the file has changed, do nothing if it hasn't */
			time_t write_time = bfs::last_write_time(file_);
			if (write_time == timestamp_)
				return false;

			/* create a temporary instance to which the file will be loaded */
			DistributedObjectPtr temp_obj = boost::dynamic_pointer_cast<DistributedObject>(create_empty_instance());
			if (!temp_obj)
				throw std::runtime_error("Distributed object failed to create temporary instance");

			/* need an exclusive lock on the temporary object */
			BlockWriteLock temp_obj_lock(*temp_obj);

			InputArchive file_arch(file_stream);
			temp_obj->do_load(file_arch, temp_obj_lock);

			/* temporarily upgrade to a write lock for our local object and then apply the updates */
			BlockWriteLock write_lock(lock);
			copy_from(temp_obj, temp_obj_lock, write_lock);
		}

		/* update the timestamp */
		timestamp_ = bfs::last_write_time(file_);
	}
	catch (bfs::filesystem_error& e)
	{
		throw distributed_object_error("file error: " + std::string(e.what()));
	}
	catch (boost::archive::archive_exception& e)
	{
		throw distributed_object_error("deserialization error: " + std::string(e.what()));
	}
#endif
	return true;
}
#endif

unsigned int DistributedObject::get_children_count(const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	unsigned int count = 0;
	BOOST_FOREACH(DistributedObjectPtr p, children_)
	{
		if (p)
			++count;
	}
	return count;
}

void DistributedObject::get_children(std::vector<DistributedObjectPtr>& result, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
	result.clear();
	BOOST_FOREACH(DistributedObjectPtr p, children_)
	{
		if (p)
			result.push_back(p);
	}
}

void DistributedObject::updated()
{
	BlockWriteLock lock(*this);

	/* Handle the update for this object */
	on_update(lock);

	/* Recursively update all children */
	BOOST_FOREACH(DistributedObjectPtr p, children_)
	{
		if (p)
			p->updated();
	}
}

DistributedObjectPtr DistributedObject::copy(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());
	DistributedObjectPtr result = do_copy_properties(lock);		/* Copy the objects properties */
	result->children_ = children_;					/* Shallow copy of child objects */
	return result;
}

boost::system_time DistributedObject::get_timestamp() const
{
	return boost::posix_time::from_time_t(timestamp_);
}

void DistributedObject::flush_cache()
{
	std::string lockdir = path_.string();
	int fd = open(lockdir.c_str(), O_RDONLY, 0600);
	if (fd < 0)
		throw bfs::filesystem_error("flush: open failed", lockdir, bs::error_code(errno, boost::system::posix_category));

	/* acquire the lock */
	struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLKW, &lock) < 0)
		throw bfs::filesystem_error("flush: lock failed", lockdir, bs::error_code(errno, boost::system::posix_category));

	if (close(fd) < 0)
		throw bfs::filesystem_error("flush: close failed", lockdir, bs::error_code(errno, boost::system::posix_category));
}

void DistributedLock::do_open()
{
	/* If there is no persistence, then there is no contention for distributed locks
	   and we can just return immediately. */
	if (!object_.get_persistence_enabled())
		return;


#ifdef P4P_CLUSTER
	std::string lockfile = object_.get_file().string() + ".lock";

	/* open the file */
	fd_ = open(lockfile.c_str(), get_open_flags(), 0600);
	if (fd_ < 0)
		throw bfs::filesystem_error("open failed", lockfile, bs::error_code(errno, boost::system::posix_category));

	/* acquire the lock */
	struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_type = get_lock_type();
	lock.l_whence = SEEK_SET;
	if (fcntl(fd_, F_SETLKW, &lock) < 0)
		throw bfs::filesystem_error("lock failed", lockfile, bs::error_code(errno, boost::system::posix_category));

	/* ensure the local object is up to date */
	try
	{
		UpgradableReadLock local_lock(object_);
		object_.update(*this, local_lock);
	}
	catch (distributed_object_error& e) { /* ignore exceptions */ }
#else
	throw std::runtime_error("Persistence support not enabled");
#endif
}

void DistributedLock::do_close()
{
	if (!object_.get_persistence_enabled())
		return;

#ifdef P4P_CLUSTER
	/* Close the file and release the lock */
	close(fd_);
#else
	throw std::runtime_error("Persistence support not enabled");
#endif
}

int DistributedReadLock::get_open_flags()
{
	return O_RDONLY | O_CREAT;
}

int DistributedReadLock::get_lock_type()
{
	return F_RDLCK;
}

int DistributedWriteLock::get_open_flags()
{
	return O_RDWR | O_CREAT;
}

int DistributedWriteLock::get_lock_type()
{
	return F_WRLCK;
}
