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


#include "p4p/app/p4p_selection_manager.h"

#include <p4p/app/peering_guidance_matrix_manager.h>
#include <p4p/app/errcode.h>
#include <p4p/logging.h>

#include <stdexcept>

namespace p4p {
namespace app {

P4PSelectionManager::P4PSelectionManager()
	: m_isp_mgr(NULL)
{
}

P4PSelectionManager::P4PSelectionManager(const ISPManager* isp_mgr)
	: m_isp_mgr(isp_mgr)
{
	if (!m_isp_mgr)
		throw std::invalid_argument("isp_mgr must not be NULL");
	setISPManager(m_isp_mgr);
}

P4PSelectionManager::~P4PSelectionManager()
{
}

bool P4PSelectionManager::setISPManager(const ISPManager* isp_mgr)
{
	/* Fail if ISPManager already configured */
	if (m_isp_mgr)
		return false;

	m_isp_mgr = isp_mgr;

	/* Add each ISP to the PeerDistributionManager so it can manage peers
	 * according to the ISP. */
	std::vector<ISP*> isp_list;
	m_isp_mgr->listISPs(isp_list);

	for (unsigned int i = 0; i < isp_list.size(); ++i)
		m_isp_peers.addISP(isp_list[i]);

	return true;
}

int P4PSelectionManager::updatePeerDistribution(const ISP* isp)
{
	bool result = m_isp_peers.updatePeerDistribution(isp);

	if (result && isLogEnabled(LOG_DEBUG))
		logDebug("event:update_peer_distribution,sel_mgr:%lx,isp:%lx", this, isp);

	return result;
}

bool P4PSelectionManager::addPeer(P4PPeerBase* peer)
{
	/* Add fails if ISPManager not yet configured */
	if (!m_isp_mgr)
		return false;

	bool result = m_isp_peers.addPeer(peer);

	P4P_LOG_DEBUG("event:peer_added,sel_mgr:" << this << ",peer:{" << peer << ',' << peer->getIPAddress() << "},home_isp:" << peer->getHomeISP() << ",home_pid:" << peer->getHomePID());

	return result;
}

bool P4PSelectionManager::removePeer(const P4PPeerBase* peer)
{
	P4P_LOG_DEBUG("event:peer_removed,sel_mgr:" << this << ",peer:{" << peer << ',' << peer->getIPAddress() << "},home_isp:" << peer->getHomeISP() << ",home_pid:" << peer->getHomePID());

	return m_isp_peers.removePeer(peer);
}

bool P4PSelectionManager::updatePeerStats(const P4PPeerBase* peer)
{
	P4P_LOG_DEBUG("event:peer_stats_updated,sel_mgr:" << this << ",peer:{" << peer << ',' << peer->getIPAddress() << "},home_isp:" << peer->getHomeISP() << ",home_pid:" << peer->getHomePID()
			<< ",upload_cap:" << peer->getUploadCapacity() << ",download_cap:" << peer->getDownloadCapacity()
			<< ",cur_upload_rate:" << peer->getCurrentUploadRate() << ",cur_download_rate:" << peer->getCurrentDownloadRate()
			<< ",active_peers:" << peer->getNumActivePeers() << ",seeder:" << (peer->isSeeder() ? 1 : 0));

	return m_isp_peers.updatePeerStats(peer);
}

};
};

