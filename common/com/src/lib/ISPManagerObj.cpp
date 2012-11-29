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

#include "ISPManagerObj.h"
#include <objbase.h>
#include "stdafx.h"

#include "ISPObj.h"
#include "Utils.h"

CISPManagerObj::CISPManagerObj()
{
	m_nRefCount = 1;
}

CISPManagerObj::~CISPManagerObj()
{
	std::vector<p4p::ISP*> isps;
	listISPs(isps);
	for (unsigned int i = 0; i < isps.size(); i++)
		sibling_cast<IISP*, CISPObj*>(isps[i])->Release();
}

HRESULT __stdcall CISPManagerObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IISPManager)
	{
		*ppObj = static_cast<IISPManager*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CISPManagerObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CISPManagerObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CISPManagerObj::AddISP(const unsigned char *name, const IISP *pISP)
{
	if (addISP(std::string((const char*)name), sibling_cast<p4p::ISP*, CISPObj*>(const_cast<IISP*>(pISP))) == NULL) return E_FAIL;
	
	const_cast<IISP*>(pISP)->AddRef();
	return S_OK;
}

HRESULT __stdcall CISPManagerObj::GetISP(const unsigned char *name, IISP **ppISP)
{
	p4p::ISP *isp = getISP(std::string((const char*)name));
	if (isp == NULL) return E_FAIL;

	*ppISP = sibling_cast<IISP*, CISPObj*>(isp);
	(*ppISP)->AddRef();
	return S_OK;
}

HRESULT __stdcall CISPManagerObj::RemoveISP(const unsigned char *name)
{
	p4p::ISP *isp = getISP(std::string((const char*)name));
	if (isp == NULL) return E_FAIL;

	sibling_cast<IISP*, CISPObj*>(isp)->Release();
	if (!removeISP(std::string((const char*)name))) return E_UNEXPECTED;
	
	return S_OK;
}
