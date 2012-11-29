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


#include "admin_net.h"

bool AdminNetNodeAdd::commit(AdminState* admin) throw (admin_error)
{
	AdminNetNodeBase::commit(admin);

	AdminState::WriteLockRecord net_rec = get_net_write();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);

	NetVertex v;
	bool ret = net->add_node(get_node(), v, *net_rec.second);
	if (ret)
		net->set_external(v, get_external(), *net_rec.second);
	return ret;
}

bool AdminNetNodeDelete::commit(AdminState* admin) throw (admin_error)
{
	AdminNetNodeBase::commit(admin);

	AdminState::WriteLockRecord net_rec = get_net_write();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);
	return net->remove_node(get_node(), *net_rec.second);
}

bool AdminNetLinkAdd::commit(AdminState* admin) throw (admin_error)
{
	AdminNetLinkBase::commit(admin);

	AdminState::WriteLockRecord net_rec = get_net_write();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);

	NetVertex v_src;
	if (!net->get_node(get_src(), v_src, *net_rec.second))
		return false;

	NetVertex v_dst;
	if (!net->get_node(get_dst(), v_dst, *net_rec.second))
		return false;

	NetEdge e;
	if (!net->add_edge(v_src, v_dst, e, *net_rec.second))
		return false;

	net->set_name(e, get_link(), *net_rec.second);
	return true;
}

bool AdminNetLinkDelete::commit(AdminState* admin) throw (admin_error)
{
	AdminNetLinkBase::commit(admin);

	AdminState::WriteLockRecord net_rec = get_net_write();
	NetStatePtr net = boost::dynamic_pointer_cast<NetState>(net_rec.first);

	NetEdge e;
	if (!net->get_edge(get_link(), e, *net_rec.second))
		return false;

	net->remove_edge(e, *net_rec.second);
	return true;
}
