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


#ifndef P4P_ISP_PDISTANCE_H
#define P4P_ISP_PDISTANCE_H

#include <vector>
#include <p4p/detail/mutex.h>
#include <p4p/protocol-portal/pdistance_matrix.h>
#include <p4p/detail/compiler.h>
#include <time.h>

namespace p4p {

/* Forward declaration */
class ISP;
namespace protocol {
namespace portal {
class PDistancePortalProtocol;
};
};

//! Maintain a PDistanceMap of an ISP
/**
 * The PDistances may come directly from a Portal Server, or a local file.
 */
class p4p_common_cpp_EXPORT ISPPDistanceMap
{
public:
	/**
	 * Default Constructor: initialize without a data source. Caller must
	 * use either setDataSourceServer() or setDataSourceFile()
	 * before calling loadP4PInfo() to load the PID Map.
	 *
	 * @param isp Pointer to parent ISP object
	 */
	ISPPDistanceMap(const ISP* isp);

	/**
	 * Constructor: use Portal Server as the data source.
	 * loadP4PInfo() must be called to query the Portal Server and
	 * load the pDistances.
	 *
	 * @param isp Pointer to parent ISP object
	 * @param addr Portal Server IP address or hostname
	 * @param port Portal Server port number
	 */
	ISPPDistanceMap(const ISP* isp, const std::string& addr, unsigned short port);

	/**
	 * Constructor: use a local file as the data source.
	 * loadP4PInfo() must be called to load data from the file.
	 *
	 * @param isp Pointer to parent ISP object
	 * @param filename Filename from which to load the pDistances
	 */
	ISPPDistanceMap(const ISP* isp, const std::string& filename);

	/**
	 * Destructor
	 */
	~ISPPDistanceMap();

	/**
	 * Update the data source to be a Portal Server. loadP4PInfo() must
	 * be called to query the Portal Server and load the pDistances. This
	 * method does not clear any existing pDistance information currently
	 * loaded.
	 *
	 * @param addr Portal Server IP address or hostname
	 * @param port Portal Server port number
	 */
	void setDataSourceServer(const std::string& addr, unsigned short port);

	/**
	 * Update the data source to be a local file. loadP4PInfo() must be
	 * called to load the data from the file. This method does not clear
	 * any existing pDistance information currently loaded.
	 *
	 * @param filename Filename from which to load the PID Map.
	 */
	void setDataSourceFile(const std::string& filename);

	/**
	 * Updates the pDistances from the data source and updates
	 * internal data structures.
	 *
	 * NOTE: This call is blocking and may not complete immediately.
	 */
	int loadP4PInfo();

	/**
	 * Get the pDistance between a pair of PIDs.
	 *
	 * @param src_pid Index of Source PID
	 * @param dst_pid Index of Destination PID
	 * @returns Returns ERR_UNKNOWN_PID if a PID index was invalid.
	 */
	int getPDistance(int src_pid, int dst_pid) const;

	/**
	 * Get the pDistances from a source PID to all other PIDs.
	 *
	 * @param src_pid Source PID; identifies the PID from which pDistances are to be returned.
	 * @param out_pdists Output parameter; vector will be populated with the row of the pDistance
	 *   matrix and resized accordingly.
	 * @returns Returns ERR_UNKNOWN_PID if PID index was invalid.
	 */
	int getPDistances(int src_pid, std::vector<int>& out_pdists) const;

public:
	/**
	 * Additional methods for advanced usage of ISPPDistanceMap class.
	 */

	/**
	 * Get Portal server address. Returns empty string if Portal Server is not being
	 * used as the data source.
	 */
	std::string getPortalAddr() const;

	/**
	 * Get Portal server port. Returns 0 if Portal Server is not being used as the
	 * data source.
	 */
	unsigned short getPortalPort() const;

	/**
	 * Return time-to-live (in seconds) for the last pDistances retrieved.
	 */
	time_t getTTL() const;

	/**
	 * Time stamp of last update
	 */
	time_t getLastUpdate() const;

	/**
	 * Get the Version identifier for the current PDistance map. The Version
	 * identifer is an opaque tag indicating the PID Map for which the pDistances
	 * are valid.
	 */
	std::string getVersion() const;

public:
	/** Row of pDistance matrix */
	typedef std::vector<int> PDistanceRow;

	/** Matrix of pDistances */
	typedef std::vector<PDistanceRow> PDistanceMatrix;

private:
	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	ISPPDistanceMap(const ISPPDistanceMap& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	ISPPDistanceMap& operator=(const ISPPDistanceMap& rhs) { return *this; }

	const ISP* m_isp;					/**< Parent ISP object */
	protocol::portal::PDistancePortalProtocol* m_proto;	/**< interface for PDistance service */
	std::string m_filename;					/**< Local file containing pDistance matrix */

	detail::SharedMutex m_mutex;				/**< Mutex protecting internal state */
	PDistanceMatrix m_pdists;				/**< Matrix of pDistances */
	time_t m_lastUpdate;					/**< Time the PDistance map was fetched */
	time_t m_ttl;						/**< Time-to-live of the PDistance map */
	std::string m_version;					/**< Version of the pDistance map */
};

std::ostream& operator<<(std::ostream& os, const ISPPDistanceMap::PDistanceMatrix& rhs);

};

#endif
