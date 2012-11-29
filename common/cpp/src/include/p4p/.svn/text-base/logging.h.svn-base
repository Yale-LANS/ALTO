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

#ifndef P4P_LOGGING_H
#define P4P_LOGGING_H

#include <p4p/detail/compiler.h>

namespace p4p {

/**
 * Enumerated type indicating the available log levels.
 */
enum LogLevel {
	LOG_TRACE,	/**< Lots of internal information (e.g., internal steps of peer selection) */
	LOG_DEBUG,	/**< Some debug information (e.g, peer events, peer selection results, fetched P4P data) */
	LOG_INFO,	/**< Informational program-level events (e.g., update manager status) */
	LOG_WARN,	/**< Warnings only */
	LOG_ERROR,	/**< Errors only */
	LOG_NONE,	/**< No logs (this is the default) */
};

/**
 * Set the logging level
 *
 * @param value New logging level
 */
p4p_common_cpp_EXPORT void setLogLevel(LogLevel value);

/**
 * Retrieve the current logging level
 *
 * @returns Current logging level
 */
p4p_common_cpp_EXPORT LogLevel getLogLevel();

/**
 * Indicate if desired logging level is enabled
 *
 * @returns True if logging is enabled at the specified level
 */
p4p_common_cpp_EXPORT bool isLogEnabled(LogLevel value);

/**
 * Log a message at the trace level
 *
 * @param msg Message to log
 * @param ... Additional parameters
 */
p4p_common_cpp_EXPORT void logTrace(const char* msg, ...);

/**
 * Log a message at the debug level
 *
 * @param msg Message to log
 * @param ... Additional parameters
 */
p4p_common_cpp_EXPORT void logDebug(const char* msg, ...);

/**
 * Log a message at the info level
 *
 * @param msg Message to log
 * @param ... Additional parameters
 */
p4p_common_cpp_EXPORT void logInfo(const char* msg, ...);

/**
 * Log a message at the warning level
 *
 * @param msg Message to log
 * @param ... Additional parameters
 */
p4p_common_cpp_EXPORT void logWarn(const char* msg, ...);

/**
 * Log a message at the error level
 *
 * @param msg Message to log
 * @param ... Additional parameters
 */
p4p_common_cpp_EXPORT void logError(const char* msg, ...);

/**
 * Typedef for callback function used to output log messages.  The callback
 * function simply takes the LogLevel at which the message is logged,
 * and the message itself.
 */
typedef void (*LogOutputCallback)(LogLevel level, const char* msg);

/**
 * Use a custom callback function used to log messages.  Without calling
 * this function, log messages are written to Standard Error.
 *
 * @param callback The callback function to be used for logging messages
 */
p4p_common_cpp_EXPORT void setLogOutputCallback(LogOutputCallback callback);

/**
 * The default callback function used to log messages. This implementation
 * simply logs messages to standard error.  Each message is prefixed with
 * an indication of the level at which the message was logged.
 * 
 * @param level Level at which the message was logged
 * @param msg Log message itself (null-terminated string)
 */
p4p_common_cpp_EXPORT void logOutputCallbackStdErr(LogLevel level, const char* msg);

};

#define P4P_LOG_TRACE(streamArg)			\
	if (isLogEnabled(LOG_TRACE))			\
	{						\
		std::ostringstream str;			\
		str << streamArg;			\
		p4p::logTrace(str.str().c_str());	\
	}

#define P4P_LOG_DEBUG(streamArg)			\
	if (isLogEnabled(LOG_DEBUG))			\
	{						\
		std::ostringstream str;			\
		str << streamArg;			\
		p4p::logDebug(str.str().c_str());	\
	}

#define P4P_LOG_INFO(streamArg)				\
	if (isLogEnabled(LOG_INFO))			\
	{						\
		std::ostringstream str;			\
		str << streamArg;			\
		p4p::logInfo(str.str().c_str());	\
	}

#define P4P_LOG_ERROR(streamArg)			\
	if (isLogEnabled(LOG_ERROR))			\
	{						\
		std::ostringstream str;			\
		str << streamArg;			\
		p4p::logError(str.str().c_str());	\
	}


#endif
