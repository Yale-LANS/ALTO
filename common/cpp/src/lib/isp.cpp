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


#include "p4p/isp.h"

namespace p4p {

ISP::ISP()
	: m_pidmap(this),
	  m_pdistmap(this)
{
}

ISP::ISP(const std::string& locationPortalAddr, unsigned short locationPortalPort,
         const std::string& pdistancePortalAddr, unsigned short pdistancePortalPort)
	: m_pidmap(this, locationPortalAddr, locationPortalPort),
	  m_pdistmap(this, pdistancePortalAddr, pdistancePortalPort)
{
}

ISP::ISP(const std::string& portalAddr, unsigned short portalPort)
	: m_pidmap(this, portalAddr, portalPort),
	  m_pdistmap(this, portalAddr, portalPort)
{
}

ISP::ISP(const std::string& pidmapFile, const std::string& pdistFile)
	: m_pidmap(this, pidmapFile),
	  m_pdistmap(this, pdistFile)
{
}

void ISP::setDataSourceServer(const std::string& portalAddr, unsigned short portalPort)
{
	m_pidmap.setDataSourceServer(portalAddr, portalPort);
	m_pdistmap.setDataSourceServer(portalAddr, portalPort);
}

void ISP::setDataSourceFile(const std::string& pidmapFile, const std::string& pdistFile)
{
	m_pidmap.setDataSourceFile(pidmapFile);
	m_pdistmap.setDataSourceFile(pdistFile);
}

int ISP::loadP4PInfo()
{
	int rc = m_pidmap.loadP4PInfo();
	if (rc != 0)
		return rc;
	
	rc = m_pdistmap.loadP4PInfo();
	if (rc != 0)
		return rc;

	return 0;
}

};
