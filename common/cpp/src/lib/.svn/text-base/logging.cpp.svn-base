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


#include "p4p/logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

namespace p4p {

/* Currently-configured logging level */
static LogLevel CURRENT_LOG_LEVEL = LOG_NONE;

/* Currently-configured logging output function */
static LogOutputCallback CURRENT_LOG_OUTPUT_CB = logOutputCallbackStdErr;

/* English names for each logging level */
static const char* LOG_LEVEL_NAMES[] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"NONE",
	};

void setLogLevel(LogLevel value)
{
	CURRENT_LOG_LEVEL = value;
}

LogLevel getLogLevel()
{
	return CURRENT_LOG_LEVEL;
}

bool isLogEnabled(LogLevel value)
{
	return CURRENT_LOG_LEVEL <= value;
}

void setLogOutputCallback(LogOutputCallback callback)
{
	CURRENT_LOG_OUTPUT_CB = callback;
}

void logOutputCallbackStdErr(LogLevel level, const char* msg)
{
	/* Ignore out-of-range log levels */
	if (level < 0 || LOG_ERROR < level)
		return;

	/* Include timestamp */
	time_t curtime = time(NULL);

	/* Get name corresponding to the level */
	const char* name = LOG_LEVEL_NAMES[level];

	fprintf(stderr, "time:%lu,type:json,%s:{%s}\n", (unsigned long)curtime, name, msg);
}

static void logMsgLarge(LogLevel level, const char* msg, int total_size, va_list ap)
{
	char* large_buf = (char*)malloc(total_size + 1);
	vsnprintf(large_buf, total_size + 1, msg, ap);
	CURRENT_LOG_OUTPUT_CB(level, large_buf);
	free(large_buf);
}

static int logMsgStatic(LogLevel level, const char* msg, va_list ap)
{
	static const int STATIC_LOG_MSG_LENGTH = 512;

	char buf[STATIC_LOG_MSG_LENGTH];
	int bytes_required = vsnprintf(buf, STATIC_LOG_MSG_LENGTH, msg, ap);
	if (bytes_required >= STATIC_LOG_MSG_LENGTH)
		return bytes_required;

	CURRENT_LOG_OUTPUT_CB(level, buf);
	return -1;
}

#define LOG_MSG(level, msg)							\
	do {									\
		int bytes_required;						\
		do {								\
			va_list ap;						\
			va_start(ap, msg);					\
			bytes_required = logMsgStatic(level, msg, ap);		\
			va_end(ap);						\
										\
			if (bytes_required < 0)					\
				return;						\
		} while (false);						\
										\
		va_list ap;							\
		va_start(ap, msg);						\
		logMsgLarge(level, msg, bytes_required, ap);			\
		va_end(ap);							\
	} while (false); 

void logTrace(const char* msg, ...)
{
	if (!isLogEnabled(LOG_TRACE)) return;
	LOG_MSG(LOG_TRACE, msg);
}

void logDebug(const char* msg, ...)
{
	if (!isLogEnabled(LOG_DEBUG)) return;
	LOG_MSG(LOG_DEBUG, msg);
}

void logInfo(const char* msg, ...)
{
	if (!isLogEnabled(LOG_INFO)) return;
	LOG_MSG(LOG_INFO, msg);
}

void logWarn(const char* msg, ...)
{
	if (!isLogEnabled(LOG_WARN)) return;
	LOG_MSG(LOG_WARN, msg);
}

void logError(const char* msg, ...)
{
	if (!isLogEnabled(LOG_ERROR)) return;
	LOG_MSG(LOG_ERROR, msg);
}

};

