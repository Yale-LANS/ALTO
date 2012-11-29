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

#include <p4p-tracker-emulator/emulator.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void die(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int main(int argc, char** argv)
{
	/* Initialize the tracker */
	void* h_tracker = open_tracker("/tmp/emulator_in", "/tmp/emulator_out");
	if (!h_tracker)
		die("open_tracker() failed");

	/* Add a channel */
	fprintf(stdout, "Adding channel\n");
	if (add_channel(h_tracker, "Channel1", "filesharing-generic", 0) < 0)
		die("add_channel() failed");

	/* Add 2 peers */
	fprintf(stdout, "Adding peer 1\n");
	struct tracker_peer p1;
	strcpy(p1.id, "Peer1");
	strcpy(p1.ipaddr, "128.36.1.33");
	p1.port = 1234;
	if (peer_join(h_tracker, "Channel1", &p1) != 0)
		die("peer_join() failed");

	fprintf(stdout, "Adding peer 2\n");
	struct tracker_peer p2;
	strcpy(p2.id, "Peer2");
	strcpy(p2.ipaddr, "130.132.1.33");
	p2.port = 4321;
	if (peer_join(h_tracker, "Channel1", &p2) != 0)
		die("peer_join() failed");

	/* Do peer selection for first peer */
	fprintf(stdout, "Doing peer selection\n");
	struct tracker_peer selected_peers[5];
	int num_selected;
	if ((num_selected = select_peers(h_tracker, "Channel1", "Peer1", 5, selected_peers)) < 0)
		die("select_peers() failed");

	int i;
	for (i = 0; i < num_selected; ++i)
		fprintf(stdout, "\tSelected: %s (%s:%hu)\n", selected_peers[i].id, selected_peers[i].ipaddr, selected_peers[i].port);

	/* Client-side peer selection for first peer */
	fprintf(stdout, "Doing client-side peer selection\n");
	struct tracker_peer candidates[2];
	struct tracker_peer client_selected_peers[1];
	candidates[0] = p1;
	candidates[1] = p2;
	if ((num_selected = client_peer_selection(h_tracker, "Channel1", "Peer1", 1, 2, candidates, client_selected_peers)) < 0)
		die("client_peer_selection failed");
	for (i = 0; i < num_selected; ++i)
		fprintf(stdout, "\tSelected: %s (%s:%hu)\n", client_selected_peers[i].id, client_selected_peers[i].ipaddr, client_selected_peers[i].port);

	/* The 2 peers leave */
	fprintf(stdout, "Removing peer 1\n");
	if (peer_leave(h_tracker, "Channel1", "Peer1") < 0)
		die("peer_leave() failed");
	fprintf(stdout, "Removing peer 2\n");
	if (peer_leave(h_tracker, "Channel1", "Peer1") < 0)
		die("peer_leave() failed");

	/* Delete the channel */
	fprintf(stdout, "Removing channel\n");
	if (delete_channel(h_tracker, "Channel1") < 0)
		die("delete_channel() failed");

	/* Close the tracker */
	fprintf(stdout, "Closing tracker\n");
	if (close_tracker(h_tracker) < 0)
		die("close_tracker() failed");

	return 0;
}

