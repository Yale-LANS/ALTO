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


#ifndef P4P_MUTEX_H
#define P4P_MUTEX_H

#include <string>
#include <sstream>
#include <stdexcept>
#include <p4p/detail/compiler.h>

#ifdef __linux 
	#define USE_PTHREADS
	#include <pthread.h>
#elif defined(WIN32)
	#define USE_WIN32THREADS
	/* Prevent windows.h from including Winsock1. This
	 * conflicts with use of Winsock2 elsewhere in this
	 * library. */
	#include <winsock2.h>
	#include <windows.h>
#endif

namespace p4p {
namespace detail {

/* Forward declaration */
class MutexImpl;

class p4p_common_cpp_EXPORT SharedMutex
{
public:
	SharedMutex();
	~SharedMutex();

	void shared_lock() const;
	void shared_unlock() const;

	void exclusive_lock() const;
	void exclusive_unlock() const;

private:

#ifdef USE_PTHREADS
	int check_rc(int rc, const char* func) const;
	mutable pthread_rwlock_t m_;

#elif defined(USE_WIN32THREADS)
	/* Adapted from implementation of RWLockFavorWriters at:
	 *   http://msdn.microsoft.com/en-us/magazine/cc163405.aspx
	 */

	mutable LONG numWritersWaiting_;
	mutable LONG numReadersWaiting_;

	// HIWORD is writer active flag;
	// LOWORD is readers active count;
	mutable DWORD activeWriterReaders_;

	mutable HANDLE hReadyToRead_;
	mutable HANDLE hReadyToWrite_;
	mutable CRITICAL_SECTION cs_;
	mutable DWORD numReaderWakeups_;
#endif
};

class ScopedSharedLock
{
public:
	ScopedSharedLock(const SharedMutex& m) : m_(m)	{ m_.shared_lock(); }
	~ScopedSharedLock()				{ m_.shared_unlock(); }
private:
	const SharedMutex& m_;
};

class ScopedExclusiveLock
{
public:
	ScopedExclusiveLock(const SharedMutex& m) : m_(m)	{ m_.exclusive_lock(); }
	~ScopedExclusiveLock()					{ m_.exclusive_unlock(); }
private:
	const SharedMutex& m_;
};

}; // namespace detail
}; // namespace p4p

#endif

