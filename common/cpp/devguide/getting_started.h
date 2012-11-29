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

/*! \page GettingStarted Getting Started with Fetching P4P Info

A P2P application uses the network location and network distance
information from an ISP P4P Portal to achieve effective peer selection.
In the P4P library, a p4p::ISP object queries an ISP's P4P portal server
and provides your application with two basic capabilities:
- Map IP addresses (of peers, servers, etc) into their corresponding PIDs. This
  service is called the <em>Location Portal Service</em>.
- Retrieve the pDistance between pairs of PIDs. This service is called the
  <em>pDistance Portal Service</em>

The following simple code example shows how to create a p4p::ISP object and
how to use the class (line numbers are added so we can easily refer to
specific pieces of the code):
\includelineno isp_object_hello_world.cpp

The program works as follows:
- At Line 7, we first create an p4p::ISP object to query a P4P Portal server.
  The two parameters in the constructor indicate the IP address and port
  number of the P4P Portal services offered by the ISP.
- In Line 8, we ask the p4p::ISP object to load its P4P information (the constructor
  does not automatically query the Portal Server).
- Lines 10 and 11 query the p4p::ISP object to obtain the PIDs of two IP addresses.
- Finally, at Line 12, we query the pDistance between the two PIDs.

Note that an p4p::ISP object contains both an p4p::ISPPIDMap object (for the Location Portal
Service) and an p4p::ISPPDistanceMap object (for the pDistance Portal Service).  This example
illustrates how P4P information can be fetched and maintained by an application.

*/

