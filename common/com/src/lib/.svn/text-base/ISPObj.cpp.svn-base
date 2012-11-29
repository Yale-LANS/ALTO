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

#include "ISPObj.h"
#include <objbase.h>
#include "stdafx.h"

CISPObj::CISPObj()
{	
	m_nRefCount = 1;
}

CISPObj::~CISPObj()
{
}

HRESULT __stdcall CISPObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IISP)
	{
		*ppObj = static_cast<IISP*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CISPObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CISPObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CISPObj::SetPortal(const unsigned char *addr, unsigned short port)
{
	setDataSourceServer(std::string((const char*)addr), port);
	return S_OK;
}

HRESULT __stdcall CISPObj::SetFile(const unsigned char *pidFile, const unsigned char *pdistanceFile)
{
	setDataSourceFile(std::string((const char*)pidFile), std::string((const char*)pdistanceFile));
	return S_OK;
}

HRESULT __stdcall CISPObj::LoadP4PInfo()
{
	if (loadP4PInfo() != 0) return E_FAIL;

	return S_OK;
}

HRESULT __stdcall CISPObj::Lookup(const unsigned char *addr, int *PID_index)
{
	int pid = lookup(p4p::IPAddress(std::string((const char*)addr)), NULL);
	if (pid == p4p::ERR_UNKNOWN_PID) return E_FAIL;

	*PID_index = pid;
	return S_OK;
}

HRESULT __stdcall CISPObj::GetNumPIDs(unsigned int *nIntraISPPIDs, unsigned int *nTotalPIDs)
{
	getNumPIDs(nIntraISPPIDs, nTotalPIDs);
	return S_OK;
}

HRESULT __stdcall CISPObj::GetPDistance(const int PID1, const int PID2, int *pDistance)
{
	int d = getPDistance(PID1, PID2);
	if (d == p4p::ERR_UNKNOWN_PID) return E_FAIL;

	*pDistance = d;
	return S_OK;
}
