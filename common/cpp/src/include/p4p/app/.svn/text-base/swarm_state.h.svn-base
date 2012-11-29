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


#ifndef P4P_AOE_SWARM_STATE_H
#define P4P_AOE_SWARM_STATE_H

#include <set>
#include <map>
#include <p4p/pid.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

/**
 * Class encapsulating the current state of a single swarm. The swarm
 * state specifies, for each grouping of peers, the following properties:
 * - Number of seeders
 * - Number of leechers
 * - Aggregate upload capacity (in Kbps)
 * - Aggregate download capacity (in Kbps)
 *
 * In the typical case, each grouping of peers can be a PID.
 *
 * NOTE: This class is NOT thread-safe.
 */
class p4p_common_cpp_EXPORT SwarmState
{
public:
	/**
	 * Default constructor. Constructs SwarmState without any PIDs.
	 */
	SwarmState();

	/**
	 * Constructor
	 *
	 * @param num_pids Number of PIDs
	 */
	SwarmState(unsigned int num_pids);

	/**
	 * Get the number of PIDs
	 *
	 * @returns Number of PIDs.
	 */
	unsigned int getNumPIDs() const;

	/**
	 * Get number of seeds within a PID
	 *
	 * @param pid Index of PID
	 * @returns Number of seeds within the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	int getNumSeeds(int pid) const;

	/**
	 * Set number of seeds within a PID
	 *
	 * @param pid Index of PID
	 * @param value Number of seeds
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setNumSeeds(int pid, int value);

	/**
	 * Get number of leechers within a PID
	 *
	 * @param pid Index of PID
	 * @returns Number of leechers within the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	int getNumLeechers(int pid) const;

	/**
	 * Set number of leechers within a PID
	 *
	 * @param pid Index of PID
	 * @param value Number of leechers
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setNumLeechers(int pid, int value);

	/**
	 * Get aggregate upload capacity within a PID (in Kbps).
	 *
	 * @param pid Index of PID
	 * @returns Aggregate upload capacity of the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	float getUploadCapacity(int pid) const;

	/**
	 * Set aggregate upload capacity of a PID.
	 *
	 * @param pid Index of PID
	 * @param value Upload capacity (in Kbps)
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setUploadCapacity(int pid, float value);

	/**
	 * Get aggregate download capacity within a PID (in Kbps).
	 *
	 * @param pid Index of PID
	 * @returns Aggregate download capacity of the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	float getDownloadCapacity(int pid) const;

	/**
	 * Set aggregate download capacity of a PID.
	 *
	 * @param pid Index of PID
	 * @param value Download capacity (in Kbps)
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setDownloadCapacity(int pid, float value);

	/**
	 * Get aggregate upload rate within a PID (in Kbps).
	 *
	 * @param pid Index of PID
	 * @returns Aggregate upload rate of the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	float getUploadRate(int pid) const;

	/**
	 * Set aggregate upload rate of a PID.
	 *
	 * @param pid Index of PID
	 * @param value Upload rate (in Kbps)
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setUploadRate(int pid, float value);

	/**
	 * Get aggregate download rate within a PID (in Kbps).
	 *
	 * @param pid Index of PID
	 * @returns Aggregate download rate of the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	float getDownloadRate(int pid) const;

	/**
	 * Set aggregate download rate of a PID.
	 *
	 * @param pid Index of PID
	 * @param value Download rate (in Kbps)
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setDownloadRate(int pid, float value);

	/**
	 * Get aggregate number of active connected peers across all
	 * peers within a PID. Note that if two peers (say, clients <tt>A</tt>
	 * and <tt>B</tt>) are connected with the same peer (say, client <tt>C</tt>),
	 * then this will count as 2 active connected peers.
	 *
	 * Thus, the average number of active peers peer client within a PID may
	 * be computed as (total number of active connected peers) / (total number of peers).
	 *
	 * @param pid Index of PID
	 * @returns Number of active connected peers within the PID, or ERR_UNKNOWN_PID if PID is invalid.
	 */
	int getNumActivePeers(int pid) const;

	/**
	 * Set number of active connected peers across all peers within
	 * a PID. See getNumActivePeers() for more information.
	 *
	 * @param pid Index of PID
	 * @param value Total number of active connected peers
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int setNumActivePeers(int pid, int value);

	/**
	 * Update statistics for a single PID.
	 *
	 * @param pid Index of PID
	 * @param change_seeds Number of seeds to add (or subtract)
	 * @param change_leechers Number of leechers to add (or subtract)
	 * @param change_uploadcap Upload capacity to add (or subtract)
	 * @param change_downloadcap Upload capacity to add (or subtract)
	 * @param change_uploadrate Upload rate to add (or subtract)
	 * @param change_downloadrate Upload rate to add (or subtract)
	 * @param change_active_peers Number of active peers to add (or subtract)
	 * @returns 0 on success, and ERR_UNKNOWN_PID on an invalid PID.
	 */
	int update(int pid,
		   int change_seeds, int change_leechers,
		   float change_uploadcap, float change_downloadcap,
		   float change_uploadrate, float change_downloadrate,
		   int change_active_peers);

private:
	/**
	 * Struct containing stats for an individual PID
	 */
	struct PIDState
	{
		PIDState() : num_seeds(0), num_leechers(0), upload_cap(0), download_cap(0), upload_rate(0), download_rate(0), num_active_peers(0) {}
		unsigned int num_seeds;
		unsigned int num_leechers;
		float upload_cap;
		float download_cap;
		float upload_rate;
		float download_rate;
		unsigned int num_active_peers;
	};

	/**
	 * Return true if PID is in range
	 */
	bool isValid(int pid) const			{ return 0 <= pid && (unsigned int)pid < m_pids.size(); }

	std::vector<PIDState> m_pids;			/**< Per-PID state */
};

std::ostream& operator<<(std::ostream& os, const SwarmState& rhs);

};
};

#endif

