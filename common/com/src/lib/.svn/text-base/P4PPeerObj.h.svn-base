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

#ifndef P4P_COM_P4PPeer
#define P4P_COM_P4PPeer

#include <p4p/p4p.h>
#include <p4p/app/p4papp.h>
#include <p4pcom/p4pcom.h>

class CP4PPeerObj : public IP4PPeer, public p4p::app::P4PPeerBase
{
public:
	CP4PPeerObj();
	~CP4PPeerObj();

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObj);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	virtual HRESULT __stdcall SetPeerInfo(const unsigned char *IPAddress, const PEERINFO PeerInfo);
	virtual HRESULT __stdcall GetHomeISP(IISP **ppISP);
	virtual HRESULT __stdcall GetHomePID(int *PID);
	virtual HRESULT __stdcall GetPeerInfo(PEERINFO *pPeerInfo);

public:	/* Internal Functions: implement P4PPeerBase */
	virtual p4p::IPAddress getIPAddress() const { return m_ipaddress; };
	virtual float getUploadCapacity() const { return m_info.UploadCapacity; };
	virtual float getDownloadCapacity() const { return m_info.DownloadCapacity; };
	virtual float getCurrentUploadRate() const { return m_info.CurrentUploadRate; };
	virtual float getCurrentDonwloadRate() const { return m_info.CurrentDownloadRate; };
	virtual unsigned int getNumActivePeers() const { return m_info.NumActivePeers; };
	virtual bool isSeeder() const { return (m_info.PeerType == PT_SEED); };

private:
	p4p::IPAddress m_ipaddress;
	PEERINFO m_info;
	long m_nRefCount;
};

#endif
