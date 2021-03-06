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


#include "global_state.h"

#include "state.h"

GlobalState::GlobalState()
{
	BlockWriteLock lock(*this);
	set_child(CHILD_IDX_NET, NetStatePtr(new NetState()), lock);
	set_child(CHILD_IDX_VIEWS, ViewRegistryPtr(new ViewRegistry()), lock);
}

GlobalState::~GlobalState()
{
}

ViewPtr GlobalState::get_view_blocking(const std::string& name, const ReadableLock& lock)
{
	lock.check_read(get_local_mutex());
	ViewRegistryPtr view_reg = get_views(lock);
	BlockReadLock view_reg_lock(*view_reg);
	return view_reg->get(name, view_reg_lock);
}

void GlobalState::apply(GlobalStatePtr new_state, const ReadableLock& new_state_lock, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	set_child(CHILD_IDX_NET, new_state->get_child(CHILD_IDX_NET, new_state_lock), lock);
	set_child(CHILD_IDX_VIEWS, new_state->get_child(CHILD_IDX_VIEWS, new_state_lock), lock);
	changed(lock);
}

#ifdef P4P_CLUSTER
void GlobalState::do_save(OutputArchive& stream, const ReadableLock& lock) const
{
	lock.check_read(get_local_mutex());
}

void GlobalState::do_load(InputArchive& stream, const WritableLock& lock)
{
	lock.check_write(get_local_mutex());
	changed(lock);
}
#endif

