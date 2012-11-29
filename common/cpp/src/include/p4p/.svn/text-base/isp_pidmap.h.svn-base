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


#ifndef P4P_PORTAL_PIDMAP_H
#define P4P_PORTAL_PIDMAP_H

#include <set>
#include <p4p/pid.h>
#include <p4p/ip_addr.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/patricia_trie.h>
#include <p4p/detail/compiler.h>
#include <time.h>

namespace p4p {

/* Forward declarations */
class ISP;
namespace protocol {
namespace portal {
class LocationPortalProtocol;
};
};

//! Maintain a PIDMap of an ISP
/**
 * The PIDMap may either come directly from a Portal Server, or a local file.
 */
class p4p_common_cpp_EXPORT ISPPIDMap
{
public:
	/**
	 * Default Constructor: initialize without a data source. Caller must
	 * use either setDataSourceServer() or setDataSourceFile()
	 * before calling loadP4PInfo() to load the PID Map.
	 *
	 * @param isp Pointer to parent ISP object
	 */
	ISPPIDMap(const ISP* isp);

	/**
	 * Constuctor: use a Portal Server as the data source.
	 * loadP4PInfo() must be called to query the Portal Server
	 * and load the PID Map.
	 *
	 * @param isp Pointer to parent ISP object
	 * @param addr Portal Server IP address or hostname
	 * @param port Portal Server port number
	 */
	ISPPIDMap(const ISP* isp, const std::string& addr, unsigned short port);

	/**
	 * Constructor: use a local file as the data source.
	 * loadP4PInfo() must be called to load data from the file.
	 *
	 * @param isp Pointer to parent ISP object
	 * @param filename Filename from which to load the PID Map.
	 */
	ISPPIDMap(const ISP* isp, const std::string& filename);

	/**
	 * Destructor
	 */
	~ISPPIDMap();

	/**
	 * Update the data source to be a Portal Server. loadP4PInfo() must
	 * be called to query the Portal Server and load the PID Map. This
	 * method does not clear any existing PID Map information currently
	 * loaded.
	 *
	 * @param addr Portal Server IP address or hostname
	 * @param port Portal Server port number
	 */
	void setDataSourceServer(const std::string& addr, unsigned short port);

	/**
	 * Update the data source to be a local file. loadP4PInfo() must be
	 * called to load the data from the file. This method does not clear
	 * any existing PID Map information currently loaded.
	 *
	 * @param filename Filename from which to load the PID Map.
	 */
	void setDataSourceFile(const std::string& filename);

	/**
	 * Updates the PID map from the data source and updates
	 * internal data structures.
	 *
	 * NOTE: This call is blocking and may not complete immediately.
	 */
	int loadP4PInfo(); 

	/**
	 * Lookup an IP address in the PID map.  Returns PID index.
	 *
	 * @param addr IP address to lookup. May be a string (C++ std::string or
	 *   C-style string) and it should automatically be converted to an IPAddress.
	 * @param matching_prefix If non-NULL, will be assigned the value of the matching
	 *   IP prefix.  If NULL (the default if not supplied), then this parameter
	 *   is ignored.
	 * @returns Index of PID.  Returns ERR_UNKNOWN_PID if address was not found
	 *   in the PID Map.
	 */
	int lookup(const IPAddress& addr, IPPrefix* matching_prefix = NULL) const;

	/**
	 * Get information about a particular PID
	 *
	 * @param pid Index of the PID
	 * @returns PID object with information about the PID.  Returns PID::INVALID if
	 *   the PID index is invalid (i.e., less than 0 or larger than getNumTotalPIDs() - 1).
	 */
	PID getPIDInfo(int pid) const;

	/**
	 * Get index for a PID
	 *
	 * @param pid PID object
	 * @returns PID index corresponding to the PID. Returns ERR_UNKNOWN_PID if PID
	 *   is not contained in the PID Map.
	 */
	int getPIDIndex(const PID& pid) const;

	/**
	 * List all PIDs in the current PID Map.
	 *
	 * @param out_pids Output parameter; Vector will be populated with the PIDs. The position
	 *   in the vector is the PID's index.
	 */
	void listPIDs(std::vector<PID>& out_pids) const;

	/**
	 * Get the numbers of intra-ISP and total PIDs.  This method provides
	 * the counts as of a single point of time, which is important when
	 * when P4P information may be updated asynchronously.
	 *
	 * @param out_intraisp_pids Output parameter; If not NULL, will be set to the number of intra-ISP
	 *   PIDs in the current PID Map.
	 * @param out_total_pids Output parameter; If not NULL, will be set to the total number of PIDs
	 *   in the current PID Map.
	 */
	void getNumPIDs(unsigned int* out_intraisp_pids, unsigned int* out_total_pids) const;


public:
	/**
	 * Additional methods for advanced usage of ISPPIDMap class.
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
	 * Get the time-to-live (in seconds) for the last PID map retrieved
	 */
	time_t getTTL() const;

	/**
	 * Time stamp of last update.
	 */
	time_t getLastUpdate() const;

	/**
	 * Get Version identifier of current PID Map.  When a PID Map
	 * is changed, it will get a different, unique identifier.
	 */
	std::string getVersion() const;

public:
	/**
	 * Lookup data structure.  We lookup PID indexes.
	 */
	typedef detail::PatriciaTrie<int> PIDLookup;

	/**
	 * Mapping from index to full PIDs
	 */
	typedef std::vector<PID> PIDInfos;

private:
	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	ISPPIDMap(const ISPPIDMap& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	ISPPIDMap& operator=(const ISPPIDMap& rhs) { return *this; }

	const ISP* m_isp;					/**< Parent ISP object */
	protocol::portal::LocationPortalProtocol* m_proto;	/**< Interface for Location service */
	std::string m_filename;					/**< Local file containing PID Map */

	detail::SharedMutex m_mutex;				/**< Mutex protecting internal data structures */
	PIDLookup* m_trie;					/**< Lookup data structure */
	PIDInfos m_pids;					/**< Collection of raw PIDs */
	PIDInfos::size_type m_intraisp_pids;			/**< Count of intra-ISP PIDs */
	time_t m_lastUpdate;					/**< Time the PIDMap was fetched */
	time_t m_ttl;						/**< Time-to-live of the PIDMap */
	std::string m_version;					/**< Version of the current PID Map */
};

std::ostream& operator<<(std::ostream& os, const ISPPIDMap::PIDLookup& rhs);
std::ostream& operator<<(std::ostream& os, const ISPPIDMap::PIDInfos& rhs);

}; // namespace p4p

#endif
