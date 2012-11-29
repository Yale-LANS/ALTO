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


#include "view_registry.h"

#include <boost/foreach.hpp>
#include <stdexcept>
#include "constants.h"
#include "plugin_registry.h"
#include "view.h"
#include "view_update_job.h"

const char* DEFAULT_VIEW_NAME = "DEFAULT";

ViewRegistry::ViewRegistry(const bfs::path& dist_file)
	: DistributedObject(dist_file)
{
	after_construct();
}

ViewRegistry::~ViewRegistry()
{
	before_destruct();
}

void ViewRegistry::on_update(const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Ensure that the default view is installed */
	if (get_children_count(lock) == 0 && !add(DEFAULT_VIEW_NAME, lock))
		throw std::runtime_error("Failed to register default view");
}

ViewPtr ViewRegistry::get(const std::string& name, const ReadableLock& lock)
{
	ViewMap::const_iterator v_itr = views_.find(!name.empty() ? name : DEFAULT_VIEW_NAME);
	if (v_itr == views_.end())
		return ViewPtr();

	return boost::dynamic_pointer_cast<View>(get_child(v_itr->second, lock));
}

bool ViewRegistry::add(const std::string& name, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Find first unfilled slot */
	for (unsigned int i = 0; i < MAX_VIEWS; ++i)
	{
		/* Skip filled slots */
		if (get_child(i, lock))
			continue;

		/* Create the view and add the index to our map */
		set_child(i, ViewPtr(new View(this, name)), lock);
		views_[name] = i;

		changed(lock);

		return true;
	}

	return false;
}

bool ViewRegistry::remove(const std::string& name, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());

	/* Find the existing view */
	ViewMap::iterator v_itr = views_.find(name);
	if (v_itr == views_.end())
		return false;

	/* Remove the view and its index mapping */
	set_child(v_itr->second, ViewPtr(), lock);
	views_.erase(v_itr);

	changed(lock);
	return true;
}

DistributedObjectPtr ViewRegistry::do_copy_properties(const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());
	ViewRegistryPtr new_obj = boost::dynamic_pointer_cast<ViewRegistry>(ViewRegistry::create_empty_instance());
	new_obj->views_ = views_;
	return new_obj;
}

#ifdef P4P_CLUSTER
void ViewRegistry::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
}

void ViewRegistry::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	changed(lock);
}
#endif

