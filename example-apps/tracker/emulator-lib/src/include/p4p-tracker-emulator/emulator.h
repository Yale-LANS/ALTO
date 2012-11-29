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


#ifndef P4P_TRACKER_EMULATOR_H
#define P4P_TRACKER_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum length in bytes (including null-terminator) of IDs.
 */
#define MAX_UUID_LENGTH 160

/**
 * Length in bytes (including null-terminator) of the textual
 * form of an IP address.
 */
#define MAX_IPADDR_LENGTH 16

/**
 * Data structure maintaining information about a single peer.
 */
struct tracker_peer
{
	char id[MAX_UUID_LENGTH];		/**< Peer's Unique ID (null-terminated) */
	char ipaddr[MAX_IPADDR_LENGTH];		/**< Peer's IPv4 address (textual format, null-terminated) */
	unsigned short port;			/**< Peer's port number */
};

/**
 * Create a tracker instance.  This associates a tracker handle with the
 * input and output FIFO's (see mkfifo(3))for writing commands to, and reading
 * results from the tracker emulator.
 *
 * @param emulator_in Path to FIFO for writing commands to emulator
 * @param emulator_out Path to FIFO for reading results from emulator
 */
void* open_tracker(const char* emulator_in, const char* emulator_out);

/**
 * Add a channel to the tracker.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID for the channel
 * @param alg The peering guidance algorithm to be used. Currently, valid algorithms are:
 *   - filesharing-generic
 *   - filesharing-swarm-dependent
 *   - streaming-swarm-dependent
 * @param num_params The number of additional parameters supplied to the
 *   peering guidance algorithm.
 * @param ... Additional parameters supplied to the peering guidance algorithm. Each
 *   parameter is expected to be a null-terminated string.
 * @returns Returns 0 on success, and non-zero on error.
 */
int add_channel(void* h_tracker, const char* channel_id, const char* alg, unsigned int num_params, ...);

/**
 * Remove a channel from the tracker.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID of channel to remove
 * @returns Returns 0 on success, and non-zero on error.
 */
int delete_channel(void* h_tracker, const char* channel_id);

/**
 * Notify the tracker that a peer has joined a channel.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID for the channel that peer has joined
 * @param peer Data structure describing the peer joining the swarm. All
 *   data members in the data structure are expected to be filled out.
 * @returns Returns 0 on success, and non-zero on error.
 */
int peer_join(void* h_tracker, const char* channel_id, const struct tracker_peer* peer);

/**
 * Perform peer selection for a particular peer.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID for the channel that peer has joined
 * @param peer_id Unique ID for the peer
 * @param max_peers Maximum number of peers to select.
 * @param selected_peers Output parameter; array of at least <tt>max_peers</tt>
 *   peer data structures. The array will be filled with the selected peers.
 * @returns Returns the number of peers selected (i.e., copied to the
 *   <tt>selected_peers</tt> array), or a negative value on error.
 */
int select_peers(void* h_tracker, const char* channel_id, const char* peer_id, unsigned int max_peers, struct tracker_peer* selected_peers);

/**
 * Perform client-side peer selection for a particular peer.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID for the channel that peer has joined
 * @param peer_id Unique ID for the peer
 * @param max_peers Maximum number of peers to select.
 * @param max_candidate Maximum number of candidate peers.
 * @param candidates Array of candidate peers.
 * @param selected_peers Output parameter; array of at least <tt>max_peers</tt>
 *   peer data structures. The array will be filled with the selected peers.
 * @returns Returns the number of peers selected (i.e., copied to the
 *   <tt>selected_peers</tt> array), or a negative value on error.
 */
int client_peer_selection(void* h_tracker, const char* channel_id, const char* peer_id, unsigned int max_peers, unsigned int max_candidates, struct tracker_peer* candidates, struct tracker_peer* selected_peers);

/**
 * Notify the tracker that a peer has left a channel.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @param channel_id Unique ID for the channel that peer has left
 * @param peer_id Unique ID for the peer
 * @returns Returns 0 on success, and non-zero on error.
 */
int peer_leave(void* h_tracker, const char* channel_id, const char* peer_id);

/**
 * Close a tracker instance. Cleans up any resources that were
 * allocated by the open_tracker() method.
 *
 * @param h_tracker Handle to tracker (returned by open_tracker() method).
 * @returns Returns 0 on success, and non-zero on error.
 */
int close_tracker(void* h_tracker);

#ifdef __cplusplus
}
#endif

#endif

