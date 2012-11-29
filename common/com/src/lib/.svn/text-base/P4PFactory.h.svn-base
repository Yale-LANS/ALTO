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

#ifndef P4P_COM_P4PFactory
#define P4P_COM_P4PFactory

#include <p4p/p4p.h>
#include <p4pcom/p4pcom.h>

class CP4PFactory : public IP4PFactory
{
public:
	CP4PFactory();
	~CP4PFactory();

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObj);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	virtual HRESULT __stdcall CreateISP(IISP **ppISP);
	//virtual HRESULT __stdcall CreateISPPIDMap(IISPPIDMap **ppISPPIDMap);
	//virtual HRESULT __stdcall CreateISPPDistanceMap(IISPPDistanceMap **ppISPPDistanceMap);
	virtual HRESULT __stdcall CreateISPManager(IISPManager **ppISPManager);
	virtual HRESULT __stdcall CreateP4PPeer(IP4PPeer **ppP4PPeer);
	virtual HRESULT __stdcall CreatePGMSelectionManager(IPGMSelectionManager **ppPGMSelectionManager);		
	virtual HRESULT __stdcall CreatePGMOptions(IPGMOptions **ppPGMOptions);	
	virtual HRESULT __stdcall CreateP4PUpdateManager(IP4PUpdateManager **ppP4PUpdateManager);
	virtual HRESULT __stdcall SetLogLevel(const LOGLEVEL level);

private:
	long m_nRefCount;
};

static CP4PFactory *pP4P = NULL;

#endif
