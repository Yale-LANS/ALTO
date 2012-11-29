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

/*! \mainpage P4P Guide for Application Developers

\section LibraryOverview Library Overview

Before diving into the code, we will cover the architecture and organization
of the P4P library.

There are two primary functionalities provided in the P4P library: core P4P
concepts and functionality, and reusable classes to ease application integration.

\subsection CoreClassesOverview Core Classes

The core classes encode basic P4P concepts such as PIDs and pDistances, and
provide services to fetch and manage P4P from ISPs.  These classes are
provided in the p4p namespace.

<table border=1>
	<tr>
		<th align=center colspan=2>Core Classes</th>
	</tr>
	<tr>
		<td width=150>\link p4p::ISP ISP \endlink</td>
		<td width=450>Encapsulates the loading and management of P4P information for a single ISP</td>
	</tr>
	<tr>
		<td width=150>\link p4p::ISPManager ISPManager \endlink</td>
		<td width=450>Container class for maintaining P4P information from multiple ISPs</td>
	</tr>
	<tr>
		<td width=150>\link p4p::PID PID \endlink</td>
		<td width=450>Encapsulates concept of a PID, and allows it to largely operate as a primitive data type</td>
	</tr>
</table>

\subsection AppIntegrationClassesOverview Application Integration Classes

In our experience with P4P, we have developed reusable services that may
be useful to applications integrating with P4P.  Our primary focus thus
far has been P2P applications (namely, peer selection), but this functionality
may be useful in other contexts as well.  The application integration classes
are provided in the p4p::app namespace.

<table border=1>
	<tr>
		<th align=center colspan=2>Application Integration Classes</th>
	</tr>
	<tr>
		<td width=150>\link p4p::app::PeeringGuidanceMatrix PeeringGuidanceMatrix \endlink</td>
		<td width=450>Provides an interface to peering guidance provided by an Application
			Optimization Engine (see \ref UsingOptEngine for further details)
		</td>
	</tr>
	<tr>
		<td width=150>\link p4p::app::P4PPeerBase P4PPeerBase \endlink</td>
		<td width=450>Defines a common interface to be implemented by peer objects, allowing
			them to be managed by the P4P application integration classes.
		</td>
	</tr>
	<tr>
		<td width=150>\link p4p::app::PGMSelectionManager PGMSelectionManager \endlink</td>
		<td width=450>Provides an easy-to-use implementation of peer selection based on P4P
			guidance (namely, \link p4p::app::PeeringGuidanceMatrix PeeringGuidanceMatrix \endlink)
			provided by an Application Optimization Engine service
		</td>
	</tr>
	<tr>
		<td width=150>\link p4p::app::P4PUpdateManager P4PUpdateManager \endlink</td>
		<td width=450>Provides services for automatically updating P4P information from ISPs,
			as well as peering guidance information from an Application Optimization Engine
		</td>
	</tr>
</table>


\section FirstStepsGettingStarted Getting Started with Fetching P4P Info

A great place to get started is by walking through a short "Hello World" program that:
- explains the P4P information provided by an ISP, and
- shows how to retrieve this P4P information from an ISP.

You can find this at \ref GettingStarted.

\section FirstStepsUsingAOE Using an Application Optimization Engine

The example gives a P2P application developer P4P
information. We are sure that P2P application developers will find
innovative, effective techniques to use the P4P information to optimize
both network and application performance. During our development
process, we proposed a concept called <b>Application Optimization
Engine</b> (AOE). An Application Optimization Engine converts "raw" P4P
information to a format that might be easier to use by a P2P application.
See \ref UsingOptEngine for details.

\section P2P Tracker Integration using Peering Guidance Matrix

The P4P Library provides classes to simplify P4P integration into existing P2P
trackers.

For an example integration using peering guidance matrix explained
step-by-step, see \ref TrackerIntegrationWalkthrough.

*/

