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


#ifndef VIEW_REGISTRY_H
#define VIEW_REGISTRY_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <p4pserver/locking.h>
#include <p4pserver/dist_obj.h>
#include <p4pserver/job_queue.h>

class View;
typedef boost::shared_ptr<View> ViewPtr;
typedef boost::shared_ptr<const View> ViewConstPtr;

class ViewRegistry;
typedef boost::shared_ptr<ViewRegistry> ViewRegistryPtr;
typedef boost::shared_ptr<const ViewRegistry> ViewRegistryConstPtr;

extern const char* DEFAULT_VIEW_NAME;

class ViewRegistry : public DistributedObject
{
public:
	static const unsigned int MAX_VIEWS = 256;

	ViewRegistry(const bfs::path& dist_file = bfs::path());
	virtual ~ViewRegistry();

	ViewPtr get(const std::string& name, const ReadableLock& lock);

	bool add(const std::string& name, const WritableLock& lock);
	bool remove(const std::string& name, const WritableLock& lock);

	void clear(const WritableLock& lock);

protected:
	virtual LocalObjectPtr create_empty_instance() const { return ViewRegistryPtr(new ViewRegistry()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock);

#ifdef P4P_CLUSTER
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
#endif

	virtual void on_update(const WritableLock& lock);

private:
	typedef std::map<std::string, unsigned int> ViewMap;

	ViewMap views_;
};

#endif
