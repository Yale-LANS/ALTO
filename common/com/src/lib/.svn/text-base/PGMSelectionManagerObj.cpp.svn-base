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

#include "PGMSelectionManagerObj.h"
#include <objbase.h>
#include "stdafx.h"

#include "ISPObj.h"
#include "ISPManagerObj.h"
#include "P4PPeerObj.h"
#include "P4PPeerListObj.h"
#include "PGMOptionsObj.h"
#include "PGMObj.h"
#include "Utils.h"

CPGMSelectionManagerObj::CPGMSelectionManagerObj()
{
	m_aoeaddr = "localhost";
	m_aoeport = 6673;
	m_ispmgr = NULL;
	m_nRefCount = 1;
}

CPGMSelectionManagerObj::~CPGMSelectionManagerObj()
{
	if (m_ispmgr) m_ispmgr->Release();
}

HRESULT __stdcall CPGMSelectionManagerObj::QueryInterface(const IID &riid, void **ppObj)
{
	if (riid == IID_IUnknown)
	{
		*ppObj = static_cast<IUnknown*>(this) ;
		AddRef();
		return S_OK;
	}
	if (riid == IID_IPGMSelectionManager)
	{
		*ppObj = static_cast<IPGMSelectionManager*>(this);
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

ULONG __stdcall CPGMSelectionManagerObj::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG __stdcall CPGMSelectionManagerObj::Release()
{
	long nRefCount = 0;
	nRefCount = InterlockedDecrement(&m_nRefCount);
	if (nRefCount == 0) delete this;
	return nRefCount;
}

HRESULT __stdcall CPGMSelectionManagerObj::SetISPManager(const IISPManager *pISPManager)
{
	if (!setISPManager(sibling_cast<p4p::ISPManager*, CISPManagerObj*>(const_cast<IISPManager*>(pISPManager)))) return E_ABORT;

	const_cast<IISPManager*>(pISPManager)->AddRef();	
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::SetAOE(const unsigned char *addr, unsigned short port)
{
	//TODO: check valid aoe addr:port
	m_aoeaddr = std::string((const char*)addr);
	m_aoeport = port;
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::SetDefaultPGMOptions(const IPGMOptions *pPGMOptions)
{
	const p4p::app::PeeringGuidanceMatrixOptions *options = sibling_cast<const p4p::app::PeeringGuidanceMatrixOptions*, const CPGMOptionsObj*>(pPGMOptions);
	setDefaultOptions(*options);
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::SetISPPGMOptions(const IISP *pISP, const IPGMOptions *pPGMOptions)
{
	const p4p::app::PeeringGuidanceMatrixOptions *options = sibling_cast<const p4p::app::PeeringGuidanceMatrixOptions*, const CPGMOptionsObj*>(pPGMOptions);
	setISPOptions(sibling_cast<const p4p::ISP*, const CISPObj*>(pISP), *options);
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::InitGuidance()
{
	if (!initGuidance(m_aoeaddr, m_aoeport)) return E_ABORT;

	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::GetISPPGM(const IISP *pISP, IPGM **ppPGM)
{
	return E_NOTIMPL;

	CPGMObj *cpgm = new CPGMObj();
	if (cpgm == NULL) return E_OUTOFMEMORY;

	const p4p::app::PeeringGuidanceMatrix *pgm = getGuidanceMatrix(sibling_cast<const p4p::ISP*, const CISPObj*>(pISP));
	if (pgm == NULL) return E_FAIL;
	
	if (!cpgm->setPGM(dynamic_cast<IPGMSelectionManager*>(this), pgm)) return E_ABORT;

	*ppPGM = dynamic_cast<IPGM*>(cpgm);
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::AddPeer(const IP4PPeer *pPeer)
{
	if (!addPeer(sibling_cast<p4p::app::P4PPeerBase*, CP4PPeerObj*>(const_cast<IP4PPeer*>(pPeer)))) return E_FAIL;

	const_cast<IP4PPeer*>(pPeer)->AddRef();
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::SelectPeers(const IP4PPeer *pPeer, const int count, IP4PPeerList **ppPeerList)
{
	std::vector<const p4p::app::P4PPeerBase*> peers;
	if (selectPeers(sibling_cast<const p4p::app::P4PPeerBase*, const CP4PPeerObj*>(pPeer), count, peers) != 0) return E_FAIL;

	CP4PPeerListObj *list = new CP4PPeerListObj();	
	if (list == NULL) return E_OUTOFMEMORY;

	list->reserve(peers.size());
	for (unsigned int i = 0; i < peers.size(); i++)
	{	
		CP4PPeerObj *peer = dynamic_cast<CP4PPeerObj*>(const_cast<p4p::app::P4PPeerBase*>(peers[i]));
		list->addPeer(peer);
		dynamic_cast<IP4PPeer*>(peer)->AddRef();
	}
	
	*ppPeerList = dynamic_cast<IP4PPeerList*>(list);
	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::UpdatePeerInfo(const IP4PPeer *pPeer)
{
	if (!updatePeerStats(sibling_cast<const p4p::app::P4PPeerBase*, const CP4PPeerObj*>(pPeer))) return E_FAIL;

	return S_OK;
}

HRESULT __stdcall CPGMSelectionManagerObj::RemovePeer(const IP4PPeer *pPeer)
{
	if (!removePeer(sibling_cast<const p4p::app::P4PPeerBase*, const CP4PPeerObj*>(pPeer))) return E_FAIL;
	
	const_cast<IP4PPeer*>(pPeer)->Release();
	return S_OK;
}
