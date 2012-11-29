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

/*! \page UsingOptEngine Using an Application Optimization Engine

The preceding example code gives a P2P application developer P4P
information. We are sure that P2P application developers will find
innovative, effective techniques to use the P4P information to optimize
both network and application performance. During our development
process, we proposed a concept called <b>Application Optimization
Engine</b> (AOE). An Application Optimization Engine converts "raw" P4P
information to a format that might be easier to use by a P2P application.
Note that an Application Optimization Engine, as its name implies, can
be application dependent.
	
As an example, our reference implementation provides an example Application
Optimization Engine that converts pDistances (and optionally current swarm
information) into the relative weights to be used for selecting peers from other
PIDs (say PID j) for a client belonging to PID i.  The weights are computed
to optimize swarm bandwidth matching.  The table below is an example guidance matrix:
<table border=1>
	<tr>
		<td width=150></td>
		<td width=150 bgcolor=green><b>Intra-PID Upper Percentage</b></td>
		<td width=150 bgcolor=lightgreen><b>Intra-domain Upper Percentage</b></td>
		<!--td width=150 bgcolor=lightblue><b>Inter-domain Upper Percentage</b></td-->
		<td width=150><b>PID pid1</b></td>
		<td width=150><b>PID pid2</b></td>
		<td width=150><b>PID pid3</b></td>
		<td width=150 bgcolor=yellow><b>PID pid-ext-domestic</b></td>
		<td width=150 bgcolor=yellow><b>PID pid-ext-oversea</b></td>
	</tr>
	<tr>
		<td width=150><b>PID pid1</b></td>
		<td width=150 bgcolor=green align="center">75%</td>
		<td width=150 bgcolor=lightgreen align="center">90%</td>
		<!--td width=150 bgcolor=lightblue align="center">95%</td-->
		<td width=150 align="center">-</td>
		<td width=150 align="center">63%</td>
		<td width=150 align="center">37%</td>
		<td width=150 bgcolor=yellow align="center">89%</td>
		<td width=150 bgcolor=yellow align="center">11%</td>
	</tr>
	<tr>
		<td width=150><b>PID pid2</b></td>
		<td width=150 bgcolor=green align="center">75%</td>
		<td width=150 bgcolor=lightgreen align="center">90%</td>
		<td width=150 align="center">55%</td>
		<td width=150 align="center">-</td>
		<td width=150 align="center">45%</td>
		<td width=150 bgcolor=yellow align="center">89%</td>
		<td width=150 bgcolor=yellow align="center">11%</td>
	</tr>
	<tr>
		<td width=150><b>PID pid3</b></td>
		<td width=150 bgcolor=green align="center">75%</td>
		<td width=150 bgcolor=lightgreen align="center">90%</td>
		<td width=150 align="center">22%</td>
		<td width=150 align="center">78%</td>
		<td width=150 align="center">-</td>
		<td width=150 bgcolor=yellow align="center">89%</td>
		<td width=150 bgcolor=yellow align="center">11%</td>
	</tr>
</table>

Each row of this matrix encodes the peer selection guidance for a single PID.
For example, for a peer in <em>pid1</em>, the first row of the matrix
suggests your application to:
- Select up to 75% peers from local neighbors inside <em>pid1</em>;
- Select up to 90% peers from intra-ISP PIDs (<em>pid1</em>, <em>pid2</em>, and <em>pid3</em>);
- For intra-ISP but non-local PIDs (<em>pid2</em> and <em>pid3</em>), if
  possible, 63% peers should be selected from <em>pid2</em>, and 37% peers
  from <em>pid3</em>;
- After selecting intra-ISP peers, select peers from external PIDs
  (<em>pid-ext-domestic</em> and <em>pid-ext-oversea</em>).  If possible,
  89% peers should be selected from <em>pid-ext-domestic</em>, and 11% peers
  from <em>pid-ext-oversea</em>.

In our reference implementation, guidance from an Application Optimization Engine
is provided by the p4p::app::PeeringGuidanceMatrix class.

We extend the previous example to demonstrate the usage of a p4p::app::PeeringGuidanceMatrix
object. In the example, we assume that the AOE is implemented as an independent server. We
also have version where the AOE is provided as a local library.

\includelineno pgm_object_hello_world.cpp

The example works as follows:
- At line 18, we create a p4p::app::PeeringGuidanceMatrix object to query an AOE server.
  The first parameter is a pointer to the ISP whose network information we want to
  convert (you must have the ISP object available to use the p4p::app::PeeringGuidanceMatrix
  object).  The next two parameters are the hostname (or IP address) and port number of the
  AOE server.
- At line 19, we let the p4p::app::PeeringGuidanceMatrix object compute peering
  guidance information (the constructor does not automatically query the AOE server).
- At line 24, we retrieve three values that are used during peer selection
  -# intrapid_pct, which indicates up to what percentage a <em>pid1</em> peer should select
     inside <em>pid1</em> (intra-PID).
  -# intraisp_pct, which indicates up to what percentage a <em>pid1</em> peer should select
     from PIDs within the ISP (intra-ISP PIDs).
  -# weights, which is vector of the weights with which clients should be chosen
     from each PID. 
- In Lines 31-32, we print out the peer selection weights from <em>pid1</em> to 
  intra-ISP PIDs (PIDs containing clients within the same ISP).
- In Lines 35-36, we print out the peer selection weights from <em>pid1</em> to
  external PIDs (PIDs containing clients outside of the ISP).

*/

