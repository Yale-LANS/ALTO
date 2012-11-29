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

#include "p4p/detail/mutex.h"

namespace p4p {
namespace detail {

/***** PTHREADS IMPLEMENTATION *****/
#ifdef USE_PTHREADS

SharedMutex::SharedMutex()
{	
	check_rc(pthread_rwlock_init(&m_, NULL), "pthread_rwlock_init");
}

SharedMutex::~SharedMutex()
{
	check_rc(pthread_rwlock_destroy(&m_), "pthread_rwlock_destroy");
}

void SharedMutex::shared_lock() const
{
	check_rc(pthread_rwlock_rdlock(&m_), "pthread_rwlock_rdlock");
}

void SharedMutex::shared_unlock() const
{
	check_rc(pthread_rwlock_unlock(&m_), "pthread_rwlock_unlock");
}

void SharedMutex::exclusive_lock() const
{
	check_rc(pthread_rwlock_wrlock(&m_), "pthread_rwlock_wrlock");
}

void SharedMutex::exclusive_unlock() const
{
	check_rc(pthread_rwlock_unlock(&m_), "pthread_rwlock_unlock");
}

int SharedMutex::check_rc(int rc, const char* func) const
{
	if (rc != 0)
		throw std::runtime_error(func + std::string(" failed"));
	return rc;
}

/***** WIN32 IMPLEMENTATION *****/
#elif defined(USE_WIN32THREADS)

SharedMutex::SharedMutex()
	: numWritersWaiting_(0),
	  numReadersWaiting_(0),
	  activeWriterReaders_(0)
{
	InitializeCriticalSection(&cs_);

	hReadyToRead_ = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hReadyToRead_ == NULL)
		throw std::runtime_error("CreateEvent failed");

	hReadyToWrite_ = CreateSemaphore(NULL, 0, 1, NULL);
	if (hReadyToWrite_ == NULL)
	{
		CloseHandle(hReadyToRead_);
		throw std::runtime_error("CreateSemaphore failed");
	}
}

SharedMutex::~SharedMutex()
{
	if (hReadyToRead_ != NULL)
		CloseHandle(hReadyToRead_);

	if (hReadyToWrite_ != NULL)
		CloseHandle(hReadyToWrite_);

	DeleteCriticalSection(&cs_);
}

void SharedMutex::shared_lock() const
{
	bool fNotifyReaders = false;

	EnterCriticalSection(&cs_);

	// Block readers from acquiring the lock if 
	// there are any writers waiting or if a writer
	// has already acquired the lock.

	if ((numWritersWaiting_ > 0) || (HIWORD(activeWriterReaders_) > 0))
	{
		++numReadersWaiting_;
		while (true)
		{
			ResetEvent(hReadyToRead_);
			LeaveCriticalSection(&cs_);
			WaitForSingleObject(hReadyToRead_, INFINITE);

			EnterCriticalSection(&cs_);

			// The reader is only allowed to read if there aren't
			// any writers waiting and if a writer doesn't own the
			// lock.
			if ((numWritersWaiting_ == 0) && (HIWORD(activeWriterReaders_) == 0))
				break;
		}

		// Reader is done waiting.
		--numReadersWaiting_;

		// Reader can read.
		++activeWriterReaders_;
	}
	else
	{
		// Reader can read.
		if ((++activeWriterReaders_ == 1) && (numReadersWaiting_ != 0))
		{
			// Set flag to notify other waiting readers
			// outside of the critical section
			// so that they don't when the threads
			// are dispatched by the scheduler they
			// don't immediately block on the critical
			// section that this thread is holding.
			fNotifyReaders = true;
		}
	}

	LeaveCriticalSection(&cs_);

	if (fNotifyReaders)
		SetEvent(hReadyToRead_);
}

void SharedMutex::shared_unlock() const
{
	EnterCriticalSection(&cs_);

	// Decrement the number of active readers.
	if (--activeWriterReaders_ == 0)
		ResetEvent(hReadyToRead_);

	// if writers are waiting and this is the last reader
	// hand owneership over to a writer.
	if ((numWritersWaiting_ != 0) && (activeWriterReaders_ == 0))
	{
		// Decrement the number of waiting writers
		--numWritersWaiting_;

		// Pass ownership to a writer thread.
		activeWriterReaders_ = MAKELONG(0, 1);
		ReleaseSemaphore(hReadyToWrite_, 1, NULL);
	}

	LeaveCriticalSection(&cs_);
}

void SharedMutex::exclusive_lock() const
{
	EnterCriticalSection(&cs_);

	// Are there active readers?
	if (activeWriterReaders_ != 0)
	{
		++numWritersWaiting_;

		LeaveCriticalSection(&cs_);

		WaitForSingleObject(hReadyToWrite_, INFINITE);

		// Upon wakeup theirs no need for the writer
		// to acquire the critical section.  It 
		// already has been transfered ownership of the
		// lock by the signaler.
	}
	else
	{
		// Set that the writer owns the lock.
		activeWriterReaders_ = MAKELONG(0, 1);;

		LeaveCriticalSection(&cs_);
	}
}

void SharedMutex::exclusive_unlock() const
{
	bool fNotifyWriter = false;
	bool fNotifyReaders = false;

	EnterCriticalSection(&cs_);

	if (numWritersWaiting_ != 0)
	{
		// Writers waiting, decrement the number of
		// waiting writers and release the semaphore
		// which means ownership is passed to the thread
		// that has been released.
		--numWritersWaiting_;
		fNotifyWriter = true;
	}
	else
	{
		// There aren't any writers waiting
		// Release the exclusive hold on the lock.
		activeWriterReaders_ = 0;

		// if readers are waiting set the flag
		// that will cause the readers to be notified
		// once the critical section is released.  This
		// is done so that an awakened reader won't immediately
		// block on the critical section which is still being
		// held by this thread.
		if (numReadersWaiting_ != 0)
			fNotifyReaders = true;
	}

	LeaveCriticalSection(&cs_);

	if (fNotifyWriter)
		ReleaseSemaphore(hReadyToWrite_, 1, NULL);
	else if (fNotifyReaders)
		SetEvent(hReadyToRead_);
}

#endif

};
};

