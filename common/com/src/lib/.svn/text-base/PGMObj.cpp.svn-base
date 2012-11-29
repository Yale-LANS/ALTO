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

#include "PGMObj.h"
#include <objbase.h>
#include "stdafx.h"

#include "PGMSelectionManagerObj.h"

CPGMObj::CPGMObj()
{
	m_psm = NULL;
	m_pgm = NULL;
	m_nRefCount = 1;
}

CPGMObj::~CPGMObj()
{
	if (m_psm)
	{
		if (m_pgm) dynamic_cast<CPGMSelectionManagerObj*>(m_psm)->releaseGuidanceMatrix(m_pgm);
		dynamic_cast<IPGMSelectionManager*>(m_psm)->Release();
	}
}

HRESULT __stdcall CPGMObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IPGM)
	{
		*ppObj = static_cast<IPGM*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CPGMObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CPGMObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CPGMObj::GetLocationSelectionPercent(double *intrapid, double *intraisp)
{
	if (!m_pgm) return E_ACCESSDENIED;

	//TODO: more efficient implementation
	std::vector<double> weights;
	m_pgm->getWeights(0, *intrapid, *intraisp, weights);
	return S_OK;
}

HRESULT __stdcall CPGMObj::GetWeight(const int src, const int dst, double *weight)
{
	if (!m_pgm) return E_ACCESSDENIED;

	//TODO: more efficient implementation
	double intrapid, intraisp;
	std::vector<double> weights;	

	if (m_pgm->getWeights(src, intrapid, intraisp, weights) != 0) return E_FAIL;

	if ((unsigned int)dst > weights.size()) return E_FAIL;

	*weight = weights[dst];
	return S_OK;
}

bool CPGMObj::setPGM(IPGMSelectionManager *psm, const p4p::app::PeeringGuidanceMatrix *pgm)
{
	if (m_pgm) return false;

	m_psm = psm;
	psm->AddRef();
	m_pgm = pgm;
	return true;
}
