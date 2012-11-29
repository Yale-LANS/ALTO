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

#include "P4PPeerListObj.h"
#include <objbase.h>
#include "stdafx.h"

#include "P4PPeerObj.h"

CP4PPeerListObj::CP4PPeerListObj()
{
	m_nRefCount = 1;
}

CP4PPeerListObj::~CP4PPeerListObj()
{
	for(unsigned int i = 0; i < m_peerlist.size(); i++)
		dynamic_cast<IP4PPeer*>(m_peerlist[i])->Release();
	clear();
}

HRESULT __stdcall CP4PPeerListObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IP4PPeerList)
	{
		*ppObj = static_cast<IP4PPeerList*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CP4PPeerListObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CP4PPeerListObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CP4PPeerListObj::GetSize(unsigned int *size)
{
	*size = (unsigned int)m_peerlist.size();
	return S_OK;
}

HRESULT __stdcall CP4PPeerListObj::GetPeer(unsigned int index, IP4PPeer **ppPeer)
{
	if (index >= m_peerlist.size()) return E_FAIL;

	*ppPeer = dynamic_cast<IP4PPeer*>(m_peerlist[index]);
	(*ppPeer)->AddRef();
	return S_OK;
}

void CP4PPeerListObj::reserve(unsigned int capacity)
{
	m_peerlist.reserve(capacity);
}

void CP4PPeerListObj::addPeer(CP4PPeerObj* peer)
{
	m_peerlist.push_back(peer);
}

void CP4PPeerListObj::clear()
{
	m_peerlist.clear();
}
