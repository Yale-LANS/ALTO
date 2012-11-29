/*
 * Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
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


#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <p4p/protocol-portal/pdistance.h>
#include <p4p/protocol-portal/location.h>

using namespace p4p;
using namespace p4p::protocol;
using namespace p4p::protocol::portal;

class ClientThread
{
public:
	ClientThread();
	~ClientThread();

	bool operator()();

private:
	bool handle_getpids();
	bool handle_getpdistance();

	void fill_prefix_list(std::vector<std::string>& result);

	void print_stats(double elapsed_time);

	LocationPortalProtocol* loc_api_;
	PDistancePortalProtocol* pdist_api_;

	double getpids_rate_;
	unsigned int getpids_batch_;
	unsigned int getpids_prefix_;
	unsigned int getpids_prefix_mask_;
	unsigned int getpids_prefix_length_;
	double getpdistance_pids_;
	double getpdistance_rate_;

	std::vector<PID> all_pids_;

	unsigned int getpids_calls_;
	unsigned int getpdistance_calls_;
};

#endif
