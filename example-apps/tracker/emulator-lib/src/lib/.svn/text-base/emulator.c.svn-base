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

#include "p4p-tracker-emulator/emulator.h"

#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

static const unsigned int MAX_LINE_LENGTH = 1024;

/**
 * Data structure maintaining state for single emulator instance.
 */
struct emulator
{
	FILE*	to_emu;			/**< Wrapper stream to emulator */
	FILE*	from_emu;		/**< Wrapper stream from emulator */
};

static int read_result(const struct emulator* emu, char* result_line)
{
	char* line = NULL;
	size_t line_len = 0;
	if (getline(&line, &line_len, emu->from_emu) < 0)
		return -1;

	/* Read the results from the string */
	char event_name[128];
	memset(event_name, 0, 128);
	char success_code[128];
	memset(success_code, 0, 128);
	if (sscanf(line, "%127s %127s", event_name, success_code) != 2)
	{
		free(line);
		return -1;
	}

	/* Copy remainder of line to result if desired */
	if (result_line)
	{
		unsigned int start = strlen(event_name) + 1 + strlen(success_code) + 1;
		unsigned int extra = strlen(line) - start;
		if (extra > MAX_LINE_LENGTH)
			extra = MAX_LINE_LENGTH;
		memcpy(result_line, line + start, extra);
	}

	free(line);

	return strcasecmp(success_code, "success") == 0;
}

void* open_tracker(const char* emulator_in, const char* emulator_out)
{
	struct emulator* emu = malloc(sizeof(*emu));

	emu->to_emu = fopen(emulator_in, "w");
	if (!emu->to_emu)
		return NULL;

	emu->from_emu = fopen(emulator_out, "r");
	if (!emu->from_emu)
	{
		fclose(emu->to_emu);
		return NULL;
	}

	return emu;
}

int add_channel(void* h_tracker, const char* channel_id, const char* alg, unsigned int num_params, ...){
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "add_channel %s %s ", channel_id, alg) < 0)
		return -1;
	va_list ap;
	va_start(ap, num_params);
	unsigned int i;
	for (i = 0; i < num_params; ++i)
	{
		const char* param = va_arg(ap, const char*);
		if (fprintf(emu->to_emu, "%s ", param) < 0)
		{
			va_end(ap);
			return -1;
		}
	}
	va_end(ap);
	if (fprintf(emu->to_emu, "\n") < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	return read_result(emu, NULL);
}

int delete_channel(void* h_tracker, const char* channel_id)
{
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "delete_channel %s\n", channel_id) < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	return read_result(emu, NULL);
}

int peer_join(void* h_tracker, const char* channel_id, const struct tracker_peer* peer)
{
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "peer_join %s %s %s %hu\n", channel_id, peer->id, peer->ipaddr, peer->port) < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	return read_result(emu, NULL);
}

int select_peers(void* h_tracker, const char* channel_id, const char* peer_id, unsigned int max_peers, struct tracker_peer* selected_peers)
{
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "select_peers %s %s %u\n", channel_id, peer_id, max_peers) < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	char line[MAX_LINE_LENGTH];
	int rc = 0;
	if ((rc = read_result(emu, line)) != 0)
		return rc;

	/* Read number of peers selected */
	char returned_peer_id[MAX_UUID_LENGTH];
	unsigned int num_peers = 0;
	if (sscanf(line, "%s %u", returned_peer_id, &num_peers) != 2)
		return -1;
	
	/* Error checking */
	if (num_peers > max_peers)
	{
		fprintf(stderr, "Emulator returned %u peers when we specified max of %u; aborting\n", num_peers, max_peers);
		abort();
	}

	/* Read each peer from the response */
	unsigned int i;
	for (i = 0; i < num_peers; ++i)
	{
		char* peer_line = NULL;
		size_t peer_line_len = 0;
		if (getline(&peer_line, &peer_line_len, emu->from_emu) < 0)
			return -1;

		/* Read the results from the string */
		memset(selected_peers[i].id, 0, MAX_UUID_LENGTH);
		memset(selected_peers[i].ipaddr, 0, MAX_IPADDR_LENGTH);
		if (sscanf(peer_line, "%s %s %hu", selected_peers[i].id, selected_peers[i].ipaddr, &selected_peers[i].port) != 3)
		{
			free(peer_line);
			return -1;
		}

		free(peer_line);
	}

	return num_peers;
}

int client_peer_selection(void* h_tracker, const char* channel_id, const char* peer_id, unsigned int max_peers, unsigned int max_candidates, struct tracker_peer* candidates, struct tracker_peer* selected_peers)
{
	unsigned int i;
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "client_peer_selection %s %s %u %u", channel_id, peer_id, max_peers, max_candidates) < 0)
		return -1;

	for(i = 0; i < max_candidates; i++)
	{
		if (fprintf(emu->to_emu, " %s", candidates[i].id) < 0)
			return -1;
	}
	if (fprintf(emu->to_emu, "\n") < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	char line[MAX_LINE_LENGTH];
	int rc = 0;
	if ((rc = read_result(emu, line)) != 0)
		return rc;

	/* Read number of peers selected */
	char returned_peer_id[MAX_UUID_LENGTH];
	unsigned int num_peers = 0;
	if (sscanf(line, "%s %u", returned_peer_id, &num_peers) != 2)
		return -1;
	
	/* Error checking */
	if (num_peers > max_peers)
	{
		fprintf(stderr, "Emulator returned %u peers when we specified max of %u; aborting\n", num_peers, max_peers);
		abort();
	}

	/* Read each peer from the response */
	for (i = 0; i < num_peers; ++i)
	{
		char* peer_line = NULL;
		size_t peer_line_len = 0;
		if (getline(&peer_line, &peer_line_len, emu->from_emu) < 0)
			return -1;

		/* Read the results from the string */
		memset(selected_peers[i].id, 0, MAX_UUID_LENGTH);
		memset(selected_peers[i].ipaddr, 0, MAX_IPADDR_LENGTH);
		if (sscanf(peer_line, "%s %s %hu", selected_peers[i].id, selected_peers[i].ipaddr, &selected_peers[i].port) != 3)
		{
			free(peer_line);
			return -1;
		}

		free(peer_line);
	}

	return num_peers;
}

int peer_leave(void* h_tracker, const char* channel_id, const char* peer_id)
{
	const struct emulator* emu = (const struct emulator*)h_tracker;

	/* Write command to emulator */
	if (fprintf(emu->to_emu, "peer_leave %s %s\n", channel_id, peer_id) < 0)
		return -1;
	if (fflush(emu->to_emu) == EOF)
		return -1;

	/* Read result */
	return read_result(emu, NULL);

	return 0;
}

int close_tracker(void* h_tracker)
{
	struct emulator* emu = (struct emulator*)h_tracker;

	/* Close input/output */
	fclose(emu->to_emu);
	fclose(emu->from_emu);

	free(emu);
	return 0;
}

