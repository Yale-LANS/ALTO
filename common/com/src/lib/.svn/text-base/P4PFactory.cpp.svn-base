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

/*
 * P4PFactory: Implementation of Interface P4PFactory
 */
#include "P4PFactory.h"
#include <objbase.h>
#include "stdafx.h"

#include "ISPObj.h"
#include "ISPManagerObj.h"
#include "P4PPeerObj.h"
#include "PGMOptionsObj.h"
#include "PGMSelectionManagerObj.h"
#include "P4PUpdateManagerObj.h"

P4PCOMDLL_API HRESULT __stdcall GetIP4PFactory(IP4PFactory **ppP4PFactory)
{
	if (pP4P != NULL) return E_ABORT;
	pP4P = new CP4PFactory;
	if (pP4P == NULL) return E_OUTOFMEMORY;
	*ppP4PFactory = (IP4PFactory *) pP4P;

	return S_OK;
}

CP4PFactory::CP4PFactory()
{
	m_nRefCount = 1;
}

CP4PFactory::~CP4PFactory()
{
}

HRESULT __stdcall CP4PFactory::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IP4PFactory)
	{
		*ppObj = static_cast<IP4PFactory*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CP4PFactory::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CP4PFactory::Release()
{
	long nRefCount = 0;	
	nRefCount = InterlockedDecrement(&m_nRefCount);		
	if (nRefCount == 0) delete this;

	return nRefCount;
}

HRESULT __stdcall CP4PFactory::CreateISP(IISP **ppISP)
{	
	*ppISP = dynamic_cast<IISP*>(new CISPObj);
	if (*ppISP == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

/*
HRESULT __stdcall CP4PFactory::CreateISPPIDMap(IISPPIDMap **ppISPPIDMap)
{
	return S_OK;
}

HRESULT __stdcall CP4PFactory::CreateISPPDistanceMap(IISPPDistanceMap **ppISPPDistanceMap)
{
	return S_OK;
}
*/

HRESULT __stdcall CP4PFactory::CreateISPManager(IISPManager **ppISPManager)
{
	*ppISPManager = dynamic_cast<IISPManager*>(new CISPManagerObj);
	if (*ppISPManager == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT __stdcall CP4PFactory::CreateP4PPeer(IP4PPeer **ppP4PPeer)
{
	*ppP4PPeer = dynamic_cast<IP4PPeer*>(new CP4PPeerObj);
	if (*ppP4PPeer == NULL) return E_OUTOFMEMORY;

	return S_OK;
}


HRESULT __stdcall CP4PFactory::CreatePGMOptions(IPGMOptions **ppPGMOptions)
{
	*ppPGMOptions = dynamic_cast<IPGMOptions*>(new CPGMOptionsObj);
	if (*ppPGMOptions == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT __stdcall CP4PFactory::CreatePGMSelectionManager(IPGMSelectionManager **ppPGMSelectionManager)
{
	*ppPGMSelectionManager = dynamic_cast<IPGMSelectionManager*>(new CPGMSelectionManagerObj);
	if (*ppPGMSelectionManager == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT __stdcall CP4PFactory::CreateP4PUpdateManager(IP4PUpdateManager **ppP4PUpdateManager)
{
	*ppP4PUpdateManager = dynamic_cast<IP4PUpdateManager*>(new CP4PUpdateManagerObj);
	if (*ppP4PUpdateManager == NULL) return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT __stdcall CP4PFactory::SetLogLevel(const LOGLEVEL level)
{
	setLogLevel((p4p::LogLevel)level);
	return S_OK;
}
