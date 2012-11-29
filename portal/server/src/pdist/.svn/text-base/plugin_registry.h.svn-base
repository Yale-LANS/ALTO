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


#ifndef PLUGIN_REGISTRY_H
#define PLUGIN_REGISTRY_H

#include <string>
#include <map>
#include <vector>
#include <p4pserver/local_obj.h>
#include "plugin_base.h"

class OptPluginRegistry : public LocalObject
{
public:
	static OptPluginRegistry& Instance()
	{
		if (INSTANCE)
			return *INSTANCE;

		boost::mutex::scoped_lock l(*INSTANCE_MUTEX);
		if (INSTANCE)
			return *INSTANCE;

		return *(INSTANCE = new OptPluginRegistry());
	}

	void register_descriptor(OptPluginDescriptor* desc);
	void unregister_descriptor(OptPluginDescriptor* desc);

	OptPluginBasePtr get_instance(const std::string& name);

	std::vector<std::string> get_list() const;

	virtual LocalObjectPtr copy(const ReadableLock& lock) { throw std::runtime_error("Not implemented"); }

protected:
	OptPluginRegistry();
	static OptPluginRegistry* INSTANCE;
	static boost::mutex* INSTANCE_MUTEX;

	virtual LocalObjectPtr create_empty_instance() const { throw std::runtime_error("Not implemented"); }

private:
	friend OptPluginBase::~OptPluginBase();

	typedef std::map<OptPluginDescriptor*, unsigned int> DescriptorMap;
	typedef std::map<OptPluginBase*, OptPluginDescriptor*> PluginInstanceMap;

	OptPluginDescriptor* find_descriptor(const std::string& name);

	/*
	 * Notification from a plugin instance that it
	 * is being destroyed.
	 */
	void plugin_instance_destroyed(OptPluginBase* instance);

	/*
	 * Keep track of the descriptors we know about as well
	 * as a reference count for how many of each type
	 * are currently constructed.
	 */
	DescriptorMap descriptors_;

	/*
	 * Keep track of which plugins instances are currently
	 * constructed, and their corresponding descriptors.
	 */
	PluginInstanceMap plugins_;
};

#endif
