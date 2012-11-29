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

#include "PGMOptionsObj.h"
#include <objbase.h>
#include "stdafx.h"

CPGMOptionsObj::CPGMOptionsObj()
{	
	m_nRefCount = 1;
}

CPGMOptionsObj::~CPGMOptionsObj()
{
}

HRESULT __stdcall CPGMOptionsObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IPGMOptions)
	{
		*ppObj = static_cast<IPGMOptions*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CPGMOptionsObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CPGMOptionsObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CPGMOptionsObj::SetAlgorithm(const enum PSALGORITHM algorithm)
{
	switch(algorithm)
	{
	case PSA_LOCATION:
		p4p::app::PeeringGuidanceMatrixOptions::operator=(p4p::app::PeeringGuidanceMatrixOptions::LocationOnly());
		break;
	case PSA_FILESHARING_GENERIC:
		p4p::app::PeeringGuidanceMatrixOptions::operator=(p4p::app::PeeringGuidanceMatrixOptions::FilesharingGeneric());
		break;
	case PSA_FILESHARING_MATCHING:
		p4p::app::PeeringGuidanceMatrixOptions::operator=(p4p::app::PeeringGuidanceMatrixOptions::FilesharingSwarmDependent());
		break;
	case PSA_STREAMING_SSM:
		p4p::app::PeeringGuidanceMatrixOptions::operator=(p4p::app::PeeringGuidanceMatrixOptions::StreamingSwarmDependent());
		break;
	default:
		return E_NOINTERFACE;
	}

	return S_OK;
}

HRESULT __stdcall CPGMOptionsObj::SetLocationSelectionPercent(const double intrapid, const double intraisp)
{
	setLocationSelectionPercentage(intrapid, intraisp);
	return S_OK;
}
