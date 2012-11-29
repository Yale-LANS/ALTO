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


#ifndef P4P_API_ISP_MANAGER_H
#define P4P_API_ISP_MANAGER_H

#include <map>
#include <p4p/isp.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {

//! Maintain a collection of ISP objects.
/**
 * An application may need to maintain P4P information from multiple ISPs. The
 * information for a single ISP is maintained by the ISP class, and ISPManager
 * maintains a collection of ISPs.
 *
 * NOTE: All operations provided by this class are thread-safe.
 */
class p4p_common_cpp_EXPORT ISPManager
{
public:
	/**
	 * Constructor
	 */
	ISPManager();

	/**
	 * Destructor
	 */
	~ISPManager();

	/**
	 * Create an ISP object and add to the collection.  Location and pDistance Portal
	 * server addresses are specified individually.
	 *
	 * @param name User-defined name to refer to the ISP
	 * @param locationPortalAddr hostname or IP address of the location portal service
	 * @param locationPortalPort port of the location portal service
	 * @param pdistancePortalAddr hostname or IP address of the pDistance portal service
	 * @param pdistancePortalPort port of the pDistance portal service
	 * @returns Returns newly-added ISP object if it was successfully added. Returns NULL
	 *   if an ISP with the desired name already existed.
	 */
	ISP* addISP(const std::string& name,
		    const std::string& locationPortalAddr, unsigned short locationPortalPort,
		    const std::string& pdistancePortalAddr, unsigned short pdistancePortalPort);

	/**
	 * Create an ISP object and add to the collection. Location and pDistance Portal
	 * servers are the same.
	 *
	 * @param name User-defined name to refer to the ISP
	 * @param portalAddr hostname or IP address of the Location Portal Service and pDistance Portal Service
	 * @param portalPort port of the Location Portal Service and pDistance Portal Service
	 * @returns Returns newly-added ISP object if it was successfully added. Returns NULL
	 *   if an ISP with the desired name already existed.
	 */
	ISP* addISP(const std::string& name,
		    const std::string& portalAddr, unsigned short portalPort);

	/**
	 * Add an ISP object to the collection.  The ISP object will now be managed by
	 * the ISPManager and can be freed by removeISP().
	 *
	 * @param name User-defined name to refer to the ISP
	 * @param isp Pointer to existing ISP object.
	 * @returns Returns ISP object if it was successfully added. Returns NULL if an
	 *   ISP with the desired name already existed.
	 */
	ISP* addISP(const std::string& name, ISP* isp);

	/**
	 * Get desired ISP object.
	 *
	 * @param name Name of ISP to return.
	 * @returns Returns desired ISP object, or NULL if an ISP with the desired name does not exist.
	 */
	ISP* getISP(const std::string& name) const;

	/**
	 * Remove ISP and free memory.
	 *
	 * @param name Name of the ISP to remove.
	 * @returns Returns true if ISP was successfully removed. Returns false if ISP with the
	 *   desired name did not exist.
	 */
	bool removeISP(const std::string& name);

	/**
	 * List the available ISP objects
	 *
	 * @param out_isps Output parameter; vector will be filled with pointers to all currently-maintained ISP objects.
	 */
	void listISPs(std::vector<ISP*>& out_isps) const;

private:
	/**
	 * Mapping from ISP name to the ISP object.
	 */
	typedef std::map<std::string, ISP*> ISPMap;

	/**
	 * Disallow copy constructor (we maintain pointers below)
	 */
	ISPManager(const ISPManager& rhs) {}

	/**
	 * Disallow assignment (we maintain pointers below)
	 */
	ISPManager& operator=(const ISPManager& rhs) { return *this; }

	detail::SharedMutex m_isps_mutex;	/**< Mutex protecting ISP collection */
	ISPMap m_isps;				/**< Map ISP names to ISP objects */
};

}; // namespace p4p

#endif

