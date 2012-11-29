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


#ifndef PLUGIN_BASE_H
#define PLUGIN_BASE_H

#include <boost/shared_ptr.hpp>
#include <p4pserver/logging.h>
#include <p4p/pid.h>
#include <p4pserver/net_state.h>
#include <p4pserver/pid_matrix.h>
#include "view.h"

class OptPluginDescriptor;

/**
 * Plugin interface for computing pdistances given current network
 * information and status.  This is the workhorse of the plugin,
 * in that instances of this class will be created to perform
 * the optimizations on behalf of the Portal server.
 */
class OptPluginBase
{
public:
	typedef ViewWrapper<
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock,
		const ReadableLock, const BlockReadLock
	> ViewState;

	/**
	 * Default constructor. Will be called to create an instance
	 * of this plugin.
	 * @param descriptor	Plugin descriptor creating this plugin instance.
	 */
	OptPluginBase(const OptPluginDescriptor* descriptor);

	/**
	 * Destructor. Will be called when deleting an instance
	 * of this plugin.
	 */
	virtual ~OptPluginBase();

	/**
	 * Compute new edge pdistances given the current network status.
	 * @param state			Current network status.
	 * @param state_lock		Pre-initialized read-lock for the network status.
	 * @param view			View which we are optimizing. DO NOT acquire write locks
	 * 				for any data structures contained in the View.
	 * @param pids		Set of PIDs with their associated vertices.
	 * @return Non-zero to indicate error, or 0 for success.
	 */
	virtual int compute_pdistances(const NetState& state, const ReadableLock& state_lock,
				   const ViewState& view,
				   const PinnedPIDSet& pids) = 0;

	/**
	 * Get the plugin descriptor.
	 * @return Plugin descriptor.
	 */
	const OptPluginDescriptor* get_descriptor() const { return descriptor_; }

	/**
	 * Get the data structure for maintaining computed edge pdistances
	 */
	const SparsePIDMatrix* get_edge_pdistances() const { return edge_pdistances_; }

protected:
	/**
	 * Get the logger instance.
	 * @return Logger instance.
	 */
	log4cpp::Category* get_logger() { return logger_; }

	/**
	 * Get the data structure for maintaining computed edge pdistances (non-const version)
	 */
	SparsePIDMatrix* get_edge_pdistances() { return edge_pdistances_; }

private:
	/**
	 * Plugin descriptor that created this plugin instance
	 */
	const OptPluginDescriptor* descriptor_;

	/**
	 * Logger instance for this plugin
	 */
	log4cpp::Category* logger_;

	/**
	 * Computed edge pdistances
	 */
	SparsePIDMatrix* edge_pdistances_;
};

typedef boost::shared_ptr<OptPluginBase> OptPluginBasePtr;
typedef boost::shared_ptr<const OptPluginBase> OptPluginBaseConstPtr;

/**
 * Plugin descriptor used to register a plugin with the Portal server. The
 * descriptor provides some metadata and enables .
 */
class OptPluginDescriptor
{
public:
	/**
	 * Empty destructor
	 */
	virtual ~OptPluginDescriptor() {}

	/**
	 * Get an identifying name for the plugin.
	 * @return Plugin's name.
	 */
	virtual std::string get_name() const = 0;

	/**
	 * Get a description for the plugin.
	 * @return Plugin's description.
	 */
	virtual std::string get_description() const = 0;

	/**
	 * Create an instance of the plugin. This may be called
	 * multiple times. That is, multiple instance of the plugin
	 * may be live at the same time.  This can be useful when the
	 * Portal server is configured to perform different types of
	 * optimizations for different requesting applications.
	 * @return New instance of the plugin.
	 */
	virtual OptPluginBasePtr create_instance() = 0;
};

/**
 * Register new optimization with the Portal server. A good time to call this
 * is when the shared library is loaded.
 * @param plugin Plugin to register.
 */
void register_opt_plugin(OptPluginDescriptor* plugin);

/**
 * Unregister an existing optimization plugin from the Portal server.  A good
 * time to call this is when the shared library is being unloaded.
 * @param plugin Plugin to unregister.
 */
void unregister_opt_plugin(OptPluginDescriptor* plugin);

#endif
