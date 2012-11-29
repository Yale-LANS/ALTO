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


#include "plugin_registry.h"

#include "plugin_base.h"

/*
 * Default plugins
 */
#include "mlu.h"

OptPluginRegistry* OptPluginRegistry::INSTANCE = NULL;
boost::mutex* OptPluginRegistry::INSTANCE_MUTEX = new boost::mutex();

OptPluginRegistry::OptPluginRegistry()
{
	register_descriptor(OptPluginMLUDescriptor);
}

void OptPluginRegistry::register_descriptor(OptPluginDescriptor* desc)
{
	BlockWriteLock lock(*this);

	if (descriptors_.find(desc) != descriptors_.end())
		return;

	descriptors_[desc] = 0;

	changed(lock);
}

void OptPluginRegistry::unregister_descriptor(OptPluginDescriptor* desc)
{
	BlockWriteLock lock(*this);

	DescriptorMap::const_iterator itr = descriptors_.find(desc);

	if (itr == descriptors_.end())
		return;

	if (itr->second > 0)
	{
		/*
		 * FIXME: Disallow unloading if instances are currently lying around.
		 */
	}

	descriptors_.erase(desc);

	changed(lock);
}

OptPluginDescriptor* OptPluginRegistry::find_descriptor(const std::string& name)
{
	for (DescriptorMap::const_iterator itr = descriptors_.begin(); itr != descriptors_.end(); ++itr)
		if (itr->first->get_name() == name)
			return itr->first;
	return NULL;
}

OptPluginBasePtr OptPluginRegistry::get_instance(const std::string& name)
{
	BlockWriteLock lock(*this);

	OptPluginDescriptor* desc = find_descriptor(name);
	if (desc == NULL)
		return OptPluginBasePtr();

	OptPluginBasePtr inst = desc->create_instance();
	if (!inst)
		return OptPluginBasePtr();

	++descriptors_[desc];
	plugins_[inst.get()] = desc;

	changed(lock);

	return inst;
}

std::vector<std::string> OptPluginRegistry::get_list() const
{
	BlockReadLock lock(*this);

	std::vector<std::string> result;
	for (DescriptorMap::const_iterator i = descriptors_.begin(); i != descriptors_.end(); ++i)
		result.push_back(i->first->get_name());
	return result;
}

void OptPluginRegistry::plugin_instance_destroyed(OptPluginBase* instance)
{
	BlockWriteLock lock(*this);

	PluginInstanceMap::const_iterator itr = plugins_.find(instance);
	if (itr == plugins_.end())
		return;

	--descriptors_[itr->second];
	plugins_.erase(instance);

	changed(lock);
}
