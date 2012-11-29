/*
 * Copyright (c) 2009,2010 Yale Laboratory of Networked Systems
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

#include "P4PPeerObj.h"
#include <objbase.h>
#include "stdafx.h"

CP4PPeerObj::CP4PPeerObj()
{
	m_info.UploadCapacity = 0;
	m_info.DownloadCapacity = 0;
	m_info.CurrentUploadRate = 0;
	m_info.CurrentDownloadRate = 0;
	m_info.NumActivePeers = 0;
	m_info.PeerType = PT_PEER;

	m_nRefCount = 1;
}

CP4PPeerObj::~CP4PPeerObj()
{
}

HRESULT __stdcall CP4PPeerObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IP4PPeer)
	{
		*ppObj = static_cast<IP4PPeer*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CP4PPeerObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CP4PPeerObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CP4PPeerObj::SetPeerInfo(const unsigned char *IPAddress, const PEERINFO PeerInfo)
{
	m_ipaddress = p4p::IPAddress(std::string((const char*)IPAddress));
	if (m_ipaddress == p4p::IPAddress::INVALID) return E_FAIL;

	//TODO: check peer info is valid	
	m_info = PeerInfo;
	return S_OK;
}

HRESULT __stdcall CP4PPeerObj::GetHomeISP(IISP **ppISP)
{
	if (m_ipaddress == p4p::IPAddress::INVALID) return E_ACCESSDENIED;

	p4p::ISP *isp = const_cast<p4p::ISP*>(getHomeISP());
	if (isp == NULL) return E_FAIL;

	*ppISP = dynamic_cast<IISP*>(isp);
	return S_OK;
}

HRESULT __stdcall CP4PPeerObj::GetHomePID(int *PID)
{
	if (m_ipaddress == p4p::IPAddress::INVALID) return E_ACCESSDENIED;

	int pid = getHomePID();
	if (pid == p4p::ERR_UNKNOWN_PID) return E_FAIL;

	*PID = pid;
	return S_OK;
}

HRESULT __stdcall CP4PPeerObj::GetPeerInfo(PEERINFO *pPeerInfo)
{
	*pPeerInfo = m_info;
	return S_OK;
}
