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


#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include <p4pserver/dist_obj.h>
#include <p4pserver/net_state.h>
#include "view_registry.h"

class GlobalState;
typedef boost::shared_ptr<GlobalState> GlobalStatePtr;
typedef boost::shared_ptr<const GlobalState> GlobalStateConstPtr;

class GlobalState : public DistributedObject
{
public:
	GlobalState();
	virtual ~GlobalState();

	NetStatePtr get_net(const ReadableLock& lock)
	{
		return boost::dynamic_pointer_cast<NetState>(get_child(CHILD_IDX_NET, lock));
	}

	ViewRegistryPtr get_views(const ReadableLock& lock)
	{
		return boost::dynamic_pointer_cast<ViewRegistry>(get_child(CHILD_IDX_VIEWS, lock));
	}

	/* Utility function for getting reference to view only */
	ViewPtr get_view_blocking(const std::string& name, const ReadableLock& lock);

	void apply(GlobalStatePtr new_state, const ReadableLock& new_state_lock, const WritableLock& lock);

protected:
	static const unsigned int CHILD_IDX_NET		= 0;
	static const unsigned int CHILD_IDX_VIEWS	= 1;

	virtual LocalObjectPtr create_empty_instance() const { return GlobalStatePtr(new GlobalState()); }

	virtual DistributedObjectPtr do_copy_properties(const ReadableLock& lock) { return boost::dynamic_pointer_cast<GlobalState>(create_empty_instance()); }

#ifdef P4P_CLUSTER
	virtual void do_save(OutputArchive& stream, const ReadableLock& lock) const;
	virtual void do_load(InputArchive& stream, const WritableLock& lock);
#endif

};

extern GlobalStatePtr GLOBAL_STATE;

template <class LockType>
class GlobalView
{
public:
	GlobalView(const std::string& name)
	{
		GlobalStatePtr global = GLOBAL_STATE;
		LockType global_lock(*global);
		ViewRegistryPtr views = global->get_views(global_lock);
		LockType views_lock(*views);
		view_ = views->get(name, views_lock);
	}
	ViewPtr operator()() { return view_; }
private:
	ViewPtr view_;
};

#endif
