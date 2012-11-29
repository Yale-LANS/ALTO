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

/*! \page TrackerIntegrationWalkthrough P2P Tracker Integration with Peering Guidance Matrix: Walkthrough

\dontinclude p4ptracker_walkthrough.cpp

The P4P Library provides classes to simplify P4P integration into existing P2P
trackers.

To illustrate how to integrate P4P into a P2P tracker, we will walk through
an example step-by-step.  In this example, we assume the P2P tracker is
already equipped
with a data structure representing peers (called <tt>YourPeerClass</tt>) and
a data structure for managing a swarm (called <tt>YourSwarmClass</tt>) which
handles adding and removing peers, as well as peer selection.

\section WalkthroughOverview Overview

At the end of this walkthrough, we will have a running tracker program that
handles multiple swarms and peer management (peers joining and leaving) and
P4P-guided peer selection.

This example program will use the P4P Portal Servers deployed for two ISPs: Yale
University and Abilene.

The peering guidance will be computed by an Application Optimization Engine server.
See \ref UsingOptEngine for an overview if you aren't sure what this is.

Now, let's begin. 

\section IncludeFiles Step 1: Include P4P Header Files and Namespaces

The first thing to do is include the P4P library header files and namespaces.
The core libraries are included by
<tt>p4p/p4p.h</tt> (in namespace <tt>p4p</tt>)
and the application integration libraries are included by
<tt>p4p/app/p4papp.h</tt> (in namespace <tt>p4p::app</tt>):

\skip include <p4p/p4p.h>
\until namespace p4p::app

\section ExtendPeerClass Extend YourPeerClass for P4P

To implement peer selection, the P4P Application library data structures
must be able to extract certain information from peers.

\subsection InheritP4PPeerBase Step 2a: Inherit YourPeerClass from P4PPeerBase

You should inherit your peer class from p4p::app::P4PPeerBase, which defines
the peer functionality required by the P4P Application library:

\skipline class YourPeerClass
\until {

If your peer class already
inherits from another base class, you may use multiple inheritance.

\subsection ImplementP4PPeerBaseMethods Step 2b: Implement P4PPeerBase methods

The p4p::app::P4PPeerBase class defines methods that will be called by the P4P Application
library. The only method that you must implement is p4p::app::P4PPeerBase::getIPAddress().  This
returns the IP address for the peer. In this example, the <tt>m_addr</tt> data
member stores the IP address as a string, and we can write the following:

\skipline getIPAddress

The P4P Application library includes the option to perform peer selection using
<em>swarm-dependent</em> guidance.  This allows the guidance to be tuned for
swarm by considering the locations of peers, and certain statistics about them.

p4p::app::P4PPeerBase defines additional methods that you may override to provide
the additional statistics required for swarm-dependent peering guidance:
- p4p::app::P4PPeerBase::getUploadCapacity(), which returns an estimate of the
  peer's total upload capacity to other peers in the swarm in Kbps
- p4p::app::P4PPeerBase::getDownloadCapacity(), which returns an estimate of the
  peer's total download capacity from other peers in the swarm in Kbps
- p4p::app::P4PPeerBase::isSeeder(), which returns true if the peer has all
  of the desired content and is no longer downloading, and false otherwise.

In this example, <tt>YourPeerClass</tt> already maintains these statistics in
the <tt>m_uploadcapacity</tt>,
<tt>m_downloadcapacity</tt>, and <tt>m_downloading</tt> data members. Thus,
we can write the following:

\skip getUploadCapacity
\until isSeeder

With that, we're done with our modifications to YourPeerClass:

\skipline }

\section AddSwarmDataMembers Step 3: Add data members to YourSwarmClass

Now, we turn our attention to the data structure that handles a single swarm:

\skipline class YourSwarmClass
\until {

This data structure manages peers within the swarm and handles peer selection. The P4P
Application library provides an easy-to-use class, p4p::app::PGMSelectionManager, for
performing peer selection
using P4P guidance from an Application Optimization Engine.  We will add a data
member to your swarm data structure for PGMSelectionManager, which will
be initialized later:

\until private
\skipline PGMSelectionManager*

As the tracker is running, peering guidance provided by the PGMSelectionManager may need
to be updated.  This example uses p4p::app::P4PUpdateManager to handle
this functionality.  The swarm stores a pointer to the update manager:

\skipline P4PUpdateManager*

Storing a pointer to the update manager allows us to register
the PGMSelectionManager for this swarm when constructed (so that peering guidance may
be updated), and unregister it when the swarm is destructed.

\section ExtendSwarmToInitialize Step 4: Extend YourSwarmClass to Initialize P4P Data Members

\subsection ExtendSwarmConstructor Step 4a: Extend YourSwarmClass Constructor

When YourSwarmClass is constructed to perform P4P-guided peer selection, the data members
that we added in the previous step must be initialized.  In this example, we first add
a constructor which takes additional arguments (we assume the swarm ID is already
specified as part of your tracker):
- <tt>isp_mgr</tt> is a reference to an existing p4p::ISPManager object
  that manages the collection of P4P-capable ISPs (we will see more
  details on this later)
- <tt>update_mgr</tt> is a pointer to an existing p4p::app::P4PUpdateManager object

The <tt>update_mgr</tt> pointer is copied to the <tt>m_update_mgr</tt> data member
in the initializer list.  The selection manager will be constructed, passing it a 
reference to the ISPManager.  We also register the selection manager with the 
update manager:

\skip YourSwarmClass(
\until {
\skipline new PGMSelectionManager
\skipline addSelectionManager
\until }

\subsection SetSwarmGuidanceOptions Step 4b: Allow Guidance Options to be Specified

Next, we must be able to configure peering guidance options. The peer selection
guidance used for a peer is controlled by the options specified for its Home ISP.
In this walkthrough, we show how to specify default options for all ISPs, and
also provide customized options for certain ISPs.

The P4P library uses <tt>PeeringGuidanceMatrixOptions</tt> objects to specify a
set of options.  We will see more details on in \ref CreatingSwarms; for now, it
suffices to say that this controls how guidance is computed, and how frequently
it is updated.

We first extend <tt>YourSwarmClass</tt> to expose a method, <tt>setDefaultGuidanceOptions()</tt>,
that configures the default peering guidance options for the swarm for all ISPs.
In this method, we only have to call the selection manager to set the default
guidance options:

\skip setDefaultGuidanceOptions(const
\until {
\skipline setDefaultOptions
\until }

We also expose a second method, <tt>setSpecificISPGuidanceOptions()</tt>, which
allows guidance options to be overridden for certain ISPs. The argument of this
method is a map from ISP to its specific guidance options.
In this method, we iterate through the ISP-options map and configure guidance
options for each specific ISP:

\skipline setSpecificISPGuidanceOptions
\until {
\until }
\until }

Finally, we expose a method to initialize the peering guidance, telling it that 
its guidance should come from an Optimization Engine server.  The arguments
<tt>optEngineAddr</tt> and <tt>optEnginePort</tt> specify the address
and port number of the Application Optimization Engine server:

\skipline initGuidance
\until {
\until }

Now, we're done with the P4P initialization.

\section SwarmPeerManagement Step 5: Update Swarm Peer Management Methods

The PGMSelectionManager object will handle peer selection, so it must be notified
when peers join and leave the swarm.  If the peering guidance information is swarm-dependent,
then it should also be notified when a particular peer's statistics have changed.
In this walkthrough, we'll assume that <tt>YourSwarmClass</tt> is
already equipped with <tt>peerJoined()</tt>, <tt>peerDeparted()</tt>, and
<tt>peerStatsChanged()</tt> methods to handle these events.

\subsection SwarmPeerJoined Step 5a: New Peer Joined Event

First, let's look at the <tt>peerJoined()</tt> method:

\skipline peerJoined
\until {

Within this method, simply call the selection manager's p4p::app::PGMSelectionManager::addPeer() method
to notify it of the new peer:

\skipline addPeer

That's all we must change for the <tt>peerJoined()</tt> event:

\skipline }

\subsection SwarmPeerStatsChanged Step 5b: Peer Statistics Updated Event

Next, let's look at the <tt>peerStatsChanged()</tt> method.  We assume this method
is called to update statistics for a peer:

\skipline peerStatsChanged
\until {

Within this method, locate the peer and update its statistics as normal (your
code may already do something similar):

\skipline findPeer
\skipline setDownloading
\skipline setCapacityEstimates

Once the statistics are updated, you simply call the selection
manager's p4p::app::PGMSelectionManager::updatePeerStats() method to notify
it that the peer's statistics have changed:

\skipline updatePeerStats

That's it for the <tt>peerStatsChanged()</tt> method:

\skipline }

\subsection SwarmSelectPeers Step 5c: Peer Selection Event

We now turn to peer selection. We assume that <tt>YourSwarmClass</tt> already contains
a method for selecting peers:

\skipline selectPeers
\until {

First, the peer is located as normal (your code may already do something similar):

\skipline findPeer

Next, call the selection manager's p4p::app::PGMSelectionManager::selectPeers() method.
Notice that this produces a vector of <tt>P4PPeerBase</tt> pointers, so you will likely
need to typecast the members back to
<tt>YourPeerClass</tt>.  If the selection manager's <tt>selectPeers()</tt> returns 0, then
it has succeeded in selecting peers, and we are finished.  This logic is implemented
by the following code:

\skip {
\skipline std::vector
\until {
\skipline for (
\until return
\skipline }
\skip }

Notice that it is possible for the selection manager's <tt>selectPeers()</tt> to fail
with an error code. This may happen,
for example, if the peer is not within a P4P-capable ISP, or peering guidance has not
yet been computed.  In such a case, it is important that you fall back to your
native peer selection logic (i.e., random, or some other algorithm) in this case.

\skipline native peer selection logic
\skip Finished with native peer selection

With that, we're done with the modifications required for peer selection.

\skipline }

\subsection SwarmPeerDeparted Step 5d: Peer Departed Event

The last peer management event we must handle is when a peer leaves the swarm. Thus,
we turn to the <tt>peerDeparted()</tt> method:

\skipline peerDeparted
\until {

Within this method, simply call the selection manager's p4p::app::PGMSelectionManager::removePeer()
method to notify it that the peer is leaving:

\skipline removePeer

That's all we must change for the <tt>peerDeparted()</tt> event:

\skipline }

\section SwarmDestructor Step 6: Update YourSwarmClass Destructor

Recall that in the constructor, we've initialized some additional data members for use
with P4P.  We now must clean them up in the destructor:

\skipline ~YourSwarmClass
\until {

We first unregister the selection manager with the update manager. This notifies
the update manager that the selection manager will be destroyed and that its
guidance should no longer be updated:

\skipline removeSelectionManager

Finally, we can cleanup the selection manager:

\skipline delete

With that, we're done with the destructor:

\skip normal destructor
\skipline }

\section UpdateThreadMethod Step 7: Define the Update Manager Thread

Recall that the update manager will update P4P information from ISPs when necessary,
and will recompute peering guidance according to the configured options.  These
operations may not complete immediately, and thus should be done in a separate thread.
We create a simple function, <tt>p4p_update_thread()</tt>, that will be executed in its own thread:

\skipline p4p_update_thread
\until {
\until }

The p4p::app::P4PUpdateManager instance is passed as an argument into the thread as an argument,
and its run() method is called so it can begin its work.

\section ConfiguringOptions Step 8: Configuring System Options

It's almost time to start creating swarms. Before doing so, we need to define
some system-level configuration options used by P4P.
For simplicity in this example, we'll define them as global variables. However,
you may instead choose to read them from another source, such as a configuration
file, database, or command-line options.

\subsection ConfigPortalServers Step 8a: Portal Servers

P4P will make use of Portal Servers run by ISPs. For a P2P tracker, these
are currently configured manually.  In this example, we configure three ISPs:
- China Telecom Zhejiang
- China NetCom
- CERNET

\skipline CTZJ_PORTAL_ADDR
\until CERNET_PORTAL_PORT

\subsection ConfigOptimizationEngine Step 8b: Optimization Engine Server

As we've seen, this walkthrough makes use of an Application Optimization
Engine Server for peering guidance.  We define constants for the server's
address and port:

\skipline OPT_ENGINE_ADDR
\skipline OPT_ENGINE_PORT

\section SystemInitialization Step 9: System Initialization

The prior steps have done all the necessary changes to data structures,
peer management events, and swarm management. Now it's time to look at
what needs to be done when the tracker starts executing.

In this walkthrough, this is done at the beginning of the program's <tt>main()</tt> function:

\skipline int main
\until {

\subsection ISPManagerInit Step 9a: Configure P4P-capable ISPs

Recall that a p4p::ISPManager object will manage P4P information from a collection of P4P-capable
ISPs.  It is now time to create the ISP manager:

\skipline ISPManager

and initialize it with ISPs:

\skipline addISP
\skipline addISP
\skipline addISP
\skipline addISP

The first parameter is a user-defined name for the ISP, and the second two parameters
are the address and port of the ISP's Portal Server.

\subsection UpdateManagerSpawn Step 9b: Spawning the Update Manager Thread

Next, we create a p4p::app::P4PUpdateManager instance, and run the p4p_update_thread()
function that we defined earlier.  Recall that <tt>p4p_update_thread()</tt> required a
pointer to the <tt>P4PUpdateManager</tt> object to be passed as an argument.  This example
uses pthreads to run the thread:

\skipline P4PUpdateManager
\skipline pthread_t
\until pthread_create
\skipline {
\until }

You may of course adapt this to use any thread library you are already using.

\section CreatingSwarms Step 10: Creating Swarms

In this walkthrough, we will be configuring peer selection for peers within China Netcom
and CERNET to use only network location information from P4P. Peer selection
for peers within other ISPs (in this case, China Telecom Zhejiang) will use peering
guidance from an Application Optimization Engine.

\subsection ISPOptions Step 10a: Define Peering Guidance Options for Particular ISPs

For peers in China Netcom and CERNET, we use a simple peer selection algorithm that only
requires network location information.  In particular, for a China Netcom peer, we first try to
select up to 90% of peers from the same PID, and then select up to 95% from China Netcom. For a 
CERNET peer, we first try to select up to 50% of peers from the same PID, and then select up to 
70% from CERNET.  We can specify peering guidance options for these two ISPs:

\skipline isp_options
\skipline cnc 
\skipline cernet 

In the \ref TrackerIntegrationWalkthroughCode "full example program", we also 
specify guidance options for the portal of China Telecom excluding Zhejiang.

\subsection ConstructingSwarms Step10b: Construct Swarms

At this point, all of the global data structures we need have been created, and
we can begin to add swarms.  Recall that we've extended <tt>YourSwarmClass</tt>
to support P4P peer selection.  The following snippet of the example program
shows how you can use the new constructor and methods:

\skipline YourSwarmClass* swarms

\skipline p4p-filesharing-swarm-dependent
\until initGuidance

The \ref TrackerIntegrationWalkthroughCode "full example program", lines 476-494,
constructs 4 different swarms, but we show one here for simplicity.

We've detailed guidance options in \ref SetSwarmGuidanceOptions, but we'll discuss
peering guidance options in more detail at this point.

The p4p::app::PeeringGuidanceMatrixOptions object specifies the configuration
options for the peering guidance used by the swarm.  These options specify:
- The type of optimization that is performed. Particular optimizations may
  be tailored for specific applications, such as filesharing or streaming.
- If and how current swarm information is used to compute peering guidance (i.e.,
  <em>swarm-dependent</em> guidance).
  One possibility is to compute "generic" guidance that is shared across swarms,
  while another is to compute guidance that incorporates knowledge of the
  current peers in the swarm.
- The frequency with which guidance information is updated.

Some pre-defined sets of options have been provided:
- p4p::app::PeeringGuidanceMatrixOptions::LocationOnly(), which provides
  simple guidance that is generated using only network location information
- p4p::app::PeeringGuidanceMatrixOptions::FilesharingGeneric(), which provides
  swarm-independent guidance for filesharing applications
- p4p::app::PeeringGuidanceMatrixOptions::FilesharingSwarmDependent(), which provides
  guidance filesharing applications that is tuned to the current swarm state
- p4p::app::PeeringGuidanceMatrixOptions::StreamingSwarmDependent(), which provides
  guidance tuned for streaming applications that is tuned to the current swarm state

In the example above, p4p::app::PeeringGuidanceMatrixOptions::FilesharingSwarmDependent()
is used to configure the peering guidance for the swarm.

It is also possible to construct your own p4p::app::PeeringGuidanceMatrixOptions object to
further customize the guidance options.  See the class's documentation for the full set
of options that may be configured.

Now, the swarm is ready for peers to join and have peer selection guided by P4P!

\section SystemCleanup Step 11: System Cleanup

When it comes time for the system to terminate, we must be sure to cleanup
the resources we allocated at system initialization.

Before exiting the system, we must signal the update manager to stop, and then wait
for its thread to complete:

\skipline p4p_update_manager.stop
\skipline pthread_join
\skipline {
\until }

With that, we're finished with the example program:

\skipline }

\section FullCode Viewing the Full Example Code

This walkthrough has explained an example integration step-by-step. The example code
is also available in full, as a program simulates peers joining
and leaving each of the swarms.

The example code also includes an additional feature that cause <tt>YourSwarmClass</tt>
to only use P4P-guided peer selection when it is configured to do so. For simplicity,
this was omitted from this step-by-step walkthrough.

To view the full code, see \ref TrackerIntegrationWalkthroughCode.

*/

/*! \page TrackerIntegrationWalkthroughCode P2P Tracker Integration with Peering Guidance Matrix: Code

This is the full example code for integrating P4P into an example P2P tracker. To
view the step-by-step walkthrough, see \ref TrackerIntegrationWalkthrough.

\includelineno p4ptracker_walkthrough.cpp

*/
