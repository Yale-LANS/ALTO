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


#ifndef P4P_ISP_H
#define P4P_ISP_H

#include <vector>
#include <p4p/isp_pidmap.h>
#include <p4p/isp_pdistancemap.h>
#include <p4p/detail/compiler.h>

namespace p4p {

//! Maintain P4P information for an Internet Service Provider. 
/**
 * One ISP object should be maintained for each P4P-capable ISP. The ISP
 * object maintains two sets of P4P information:
 * - the PID Map (handled by ISPPIDMap), which maps between IP
 *   prefixes and PIDs
 * - the pDistance Map (handled by ISPPDistanceMap), which specifies
 *   the pDistance amongst PIDs.
 *
 * This API uses <b>PID Indexes</b> to refer to PIDs.  When a PID Map is fetched
 * from an ISP, the ISP object will store the PIDs in an array.  The PID Index
 * is the index into this array.
 *
 * PIDs are divided into two sets: intra-ISP PIDs (which contain addresses
 * within the ISP), and external PIDs (which contain addresses outside
 * the ISP). The getNumPIDs() method returns both the number of intra-ISP
 * PIDs (denoted here by NumIntraISPPIDs) and the total number PIDs (denoted
 * here by NumTotalPIDs). PID indexes are organized as follows:
 * - PID indexes range from 0 ... NumTotalPIDs - 1
 * - PID indexes 0 ... NumIntraISPPIDs - 1 are intra-ISP PIDs
 * - PID indexes NumIntraISPPIDs ... NumTotalPIDs - 1 are external PIDs
 *
 * You may query for more information about a particular PID using
 * the ISP::getPIDInfo() method by specifying a PID Index.
 *
 * The following example program shows how to use the ISP object to fetch
 * P4P information, map IP addressed into PIDs, and query the pDistance
 * between a pair of PIDs:
 * \include isp_object_hello_world.cpp
 */
class p4p_common_cpp_EXPORT ISP
{
public:
	/**
	 * Default constructor: constructs PIDMap and pDistance Map without any
	 * initial data source.  Caller must use either setDataSourceServer()
	 * or setDataSourceFile() before calling loadP4PInfo() to load information.
	 */
	ISP();

	/**
	 * Constructor: constructs PIDMap and pDistanceMap. loadP4PInfo() must be called
	 * to query the Portal Server and load the information.
	 *
	 * @param locationPortalAddr Hostname or IP address for Location Portal Server
	 * @param locationPortalPort Port number for Location Portal Server
	 * @param pdistancePortalAddr Hostname or IP address for pDistance Portal Server
	 * @param pdistancePortalPort Port number for pDistance Portal Server
	 */
	ISP(const std::string& locationPortalAddr, unsigned short locationPortalPort,
	    const std::string& pdistancePortalAddr, unsigned short pdistancePortalPort);

	/**
	 * Constructor: construct PIDMap and pDistance map using the same address for
	 * location portal server and pDistance portal server. loadP4PInfo() must be
	 * called to query the Portal Server and load the information.
	 *
	 * @param portalAddr Hostname or IP address for Portal Server (used for both
	 *   Location Portal and pDistance Portal)
	 * @param portalPort Port number for Portal Server (used for both Location Portal
	 *   and pDistance Portal)
	 */
	ISP(const std::string& portalAddr, unsigned short portalPort);

	/**
	 * Constructor: Use local files for both the PID Map and the pDistance Map.
	 * loadP4PInfo() must be called to load the data from the file.
	 *
	 * @param pidmapFile Filename from which PID Map should be loaded
	 * @param pdistFile Filename from which pDistance Map should be loaded
	 */
	ISP(const std::string& pidmapFile, const std::string& pdistFile);

	/**
	 * Update the data source to be a Portal Server, which provides both the
	 * PID Map and pDistance maps. loadP4PInfo() must
	 * be called to query the Portal Server and load the information. This
	 * method does not clear any existing information currently loaded.
	 *
	 * @param portalAddr Portal Server IP address or hostname
	 * @param portalPort Portal Server port number
	 */
	void setDataSourceServer(const std::string& portalAddr, unsigned short portalPort);

	/**
	 * Update the data source to be a local files. loadP4PInfo() must be
	 * called to load the data from the files. This method does not clear
	 * any existing information currently loaded.
	 *
	 * @param pidmapFile Filename from which PID Map should be loaded
	 * @param pdistFile Filename from which pDistance Map should be loaded
	 */
	void setDataSourceFile(const std::string& pidmapFile, const std::string& pdistFile);

	/**
	 * Load new P4P information.
	 *
	 * @returns Returns 0 on success, and an error code on a failure.  The following
	 *   errors may be returned:
	 *   - ERR_CONNECTION_FAILURE: Indicates that an error occurred connecting to
	 *     the portal server.
	 *   - ERR_PROTOCOL_FAILURE: Indicates that an error occurred parsing a response
	 *     from the portal server.
	 *
	 * NOTE: This is a blocking call and may not return immediately.
	 */
	int loadP4PInfo();

	/**
	 * \copydoc ISPPIDMap::lookup
	 */
	int lookup(const IPAddress& addr, IPPrefix* matching_prefix = NULL) const
								{ return m_pidmap.lookup(addr, matching_prefix); }

	/**
	 * \copydoc ISPPIDMap::getPIDInfo
	 */
	PID getPIDInfo(int pid) const				{ return m_pidmap.getPIDInfo(pid); }

	/**
	 * \copydoc ISPPIDMap::getPIDIndex
	 */
	int getPIDIndex(const PID& pid) const			{ return m_pidmap.getPIDIndex(pid); }

	/**
	 * \copydoc ISPPIDMap::listPIDs
	 */
	void listPIDs(std::vector<PID>& out_pids) const		{ m_pidmap.listPIDs(out_pids); }

	/**
	 * \copydoc ISPPIDMap::getNumPIDs
	 */
	void getNumPIDs(unsigned int* out_intraisp_pids, unsigned int* out_total_pids) const
								{ m_pidmap.getNumPIDs(out_intraisp_pids, out_total_pids); }

	/**
	 * \copydoc ISPPDistanceMap::getPDistance
	 */
	int getPDistance(int src_pid, int dst_pid) const	{ return m_pdistmap.getPDistance(src_pid, dst_pid); }

	/**
	 * \copydoc ISPPDistanceMap::getPDistances
	 */
	int getPDistances(int src_pid, std::vector<int>& out_pdists) const
								{ return m_pdistmap.getPDistances(src_pid, out_pdists); }

public:
	/**
	 * Additional methods for advanced usage of ISP class.
	 */

	/**
	 * Get location portal server address
	 */
	std::string getLocationPortalAddr() const		{ return m_pidmap.getPortalAddr(); }

	/**
	 * Get location portal server port
	 */
	unsigned short getLocationPortalPort() const		{ return m_pidmap.getPortalPort(); }

	/**
	 * Get pDistance portal server address
	 */
	std::string getPDistancePortalAddr() const		{ return m_pdistmap.getPortalAddr(); }

	/**
	 * Get pDistance portal server port
	 */
	unsigned short getPDistancePortalPort() const		{ return m_pdistmap.getPortalPort(); }

	/**
	 * Get constant reference to ISPPIDMap
	 */
	const ISPPIDMap& getPIDMap() const			{ return m_pidmap; }

	/**
	 * Get non-constant reference to ISPPIDMap
	 */
	ISPPIDMap& getPIDMap()					{ return m_pidmap; }

	/**
	 * Get constant reference to ISPPDistanceMap
	 */
	const ISPPDistanceMap& getPDistanceMap() const		{ return m_pdistmap; }

	/**
	 * Get non-constant reference to ISPPDistanceMap
	 */
	ISPPDistanceMap& getPDistanceMap()			{ return m_pdistmap; }

protected:
	/**
	 * Dummy method for making ISP a polymorphic type.  This is required
	 * in the COM wrapper.
	 */
	virtual void dummyVirtual() {}

private:
	ISPPIDMap m_pidmap;		/**< PID map */
	ISPPDistanceMap m_pdistmap;	/**< PDistance map */
};

}; // namespace p4p

#endif

