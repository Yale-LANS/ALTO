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


#include "p4p/isp_manager.h"

namespace p4p {

ISPManager::ISPManager()
{
}

ISPManager::~ISPManager()
{
	/* Free memory for all ISPs */
	for (ISPMap::const_iterator itr = m_isps.begin(); itr != m_isps.end(); ++itr)
		delete itr->second;
	m_isps.clear();
}

ISP* ISPManager::addISP(const std::string& name,
			const std::string& locationPortalAddr, unsigned short locationPortalPort,
			const std::string& pdistancePortalAddr, unsigned short pdistancePortalPort)
{
	ISP* isp = new ISP(locationPortalAddr, locationPortalPort, pdistancePortalAddr, pdistancePortalPort);
	if (addISP(name, isp) == NULL)
	{
		/* ISP already existed */
		delete isp;
		return NULL;
	}

	/* Added successfully */
	return isp;
}

ISP* ISPManager::addISP(const std::string& name,
			const std::string& portalAddr, unsigned short portalPort)
{
	ISP* isp = new ISP(portalAddr, portalPort);
	if (addISP(name, isp) == NULL)
	{
		/* ISP already existed */
		delete isp;
		return NULL;
	}

	/* Added successfully */
	return isp;
}

ISP* ISPManager::addISP(const std::string& name, ISP* isp)
{
	detail::ScopedExclusiveLock lock(m_isps_mutex);

	/* See if an ISP already exists  */
	ISPMap::const_iterator itr = m_isps.find(name);
	if (itr != m_isps.end())
		return NULL;

	/* Add the ISP */
	m_isps.insert(std::make_pair(name, isp));

	return isp;
}

ISP* ISPManager::getISP(const std::string& name) const
{
	detail::ScopedSharedLock lock(m_isps_mutex);

	ISPMap::const_iterator itr = m_isps.find(name);
	if (itr == m_isps.end())
		return NULL;

	return itr->second;
}

bool ISPManager::removeISP(const std::string& name)
{
	detail::ScopedExclusiveLock lock(m_isps_mutex);

	ISPMap::iterator itr = m_isps.find(name);
	if (itr == m_isps.end())
		return false;

	/* Free resources for the ISP */
	delete itr->second;
	m_isps.erase(itr);
	return true;
}

void ISPManager::listISPs(std::vector<ISP*>& out_isps) const
{
	detail::ScopedSharedLock lock(m_isps_mutex);

	for (ISPMap::const_iterator itr = m_isps.begin(); itr != m_isps.end(); ++itr)
		out_isps.push_back(itr->second);
}

};
