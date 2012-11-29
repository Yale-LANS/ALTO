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


#include "view_update.h"

unsigned int ViewUpdateDirect::do_update()
{
	get_logger().debug("Computing update");
	if (!compute_result())
		return 0;

	get_logger().debug("Updating matrices");
	get_view_state().get()->set_intradomain_pdistances(result_intradomain_pdistances_, get_view_state().get_view_lock());
	get_view_state().get()->set_interdomain_pdistances(result_interdomain_pdistances_, get_view_state().get_view_lock());
	get_logger().debug("Finished updating matrices");

	return result_update_interval_;
}

unsigned int ViewUpdateUpgrade::do_update()
{
	get_logger().debug("Computing update");
	if (!compute_result())
		return 0;

	get_logger().debug("Acquiring locks: global");
	UpgradedWriteLock global_state_write_lock(global_state_lock_);
	get_logger().debug("Acquiring locks: views");
	UpgradedWriteLock views_write_lock(views_lock_);
	get_logger().debug("Acquiring locks: view");
	UpgradedWriteLock view_write_lock(*static_cast<const UpgradableReadLock*>(&get_view_state().get_view_lock()));

	get_logger().debug("Updating matrices");
	get_view_state().get()->set_intradomain_pdistances(result_intradomain_pdistances_, view_write_lock);
	get_view_state().get()->set_interdomain_pdistances(result_interdomain_pdistances_, view_write_lock);
	get_logger().debug("Finished updating matrices");

	return result_update_interval_;
}
