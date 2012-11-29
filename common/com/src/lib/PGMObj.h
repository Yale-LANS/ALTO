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

#ifndef P4P_COM_PGM
#define P4P_COM_PGM

#include <p4p/p4p.h>
#include <p4p/app/p4papp.h>
#include <p4pcom/p4pcom.h>

class CPGMObj : public IPGM
{
public:
	CPGMObj();
	~CPGMObj();

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObj);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	virtual HRESULT __stdcall GetLocationSelectionPercent(double *intrapid, double *intraisp);
	virtual HRESULT __stdcall GetWeight(const int src, const int dst, double *weight);

public:	/* Internal functions */
	bool setPGM(IPGMSelectionManager *psm, const p4p::app::PeeringGuidanceMatrix *pgm);

private:
	IPGMSelectionManager *m_psm;
	const p4p::app::PeeringGuidanceMatrix *m_pgm;
	long m_nRefCount;
};

#endif
