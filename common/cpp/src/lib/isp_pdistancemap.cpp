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


#include "p4p/isp_pdistancemap.h"

#include <fstream>
#include <limits.h>
#include <iterator>
#include <p4p/detail/util.h>
#include <p4p/isp.h>
#include <p4p/logging.h>
#include <p4p/protocol-portal/pdistance.h>
#include <p4p/errcode.h>

namespace p4p {

static const time_t DEFAULT_TTL = 2 * 60 * 60;

ISPPDistanceMap::ISPPDistanceMap(const ISP* isp)
	: m_isp(isp),
	  m_proto(NULL),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)	/* Initial value; reset upon retrieving data */
{
}

ISPPDistanceMap::ISPPDistanceMap(const ISP* isp, const std::string& addr, unsigned short port)
	: m_isp(isp),
	  m_proto(NULL),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)	/* Initial value; reset upon retrieving data */
{
	setDataSourceServer(addr, port);
}

ISPPDistanceMap::ISPPDistanceMap(const ISP* isp, const std::string& filename)
	: m_isp(isp),
	  m_proto(NULL),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)
{
	setDataSourceFile(filename);
}

ISPPDistanceMap::~ISPPDistanceMap()
{
	delete m_proto;
}

void ISPPDistanceMap::setDataSourceServer(const std::string& addr, unsigned short port)
{
	/* Replace reference to existing portal server (if it exists) */
	protocol::portal::PDistancePortalProtocol* old_proto = m_proto;
	m_proto = new protocol::portal::PDistancePortalProtocol(addr, port);
	delete old_proto;

	/* Clear other data sources */
	m_filename.clear();
}

void ISPPDistanceMap::setDataSourceFile(const std::string& filename)
{
	/* Replace filename */
	m_filename = filename;

	/* Clear other data sources */
	delete m_proto;
	m_proto = NULL;
}


std::string ISPPDistanceMap::getPortalAddr() const
{
	return m_proto ? m_proto->get_host() : "";
}

unsigned short ISPPDistanceMap::getPortalPort() const
{
	return m_proto ? m_proto->get_port() : 0;
}


int ISPPDistanceMap::getPDistance(int src_pid, int dst_pid) const
{
	detail::ScopedSharedLock lock(m_mutex);

	if (src_pid < 0 || m_pdists.size() <= (unsigned int)src_pid)
		return ERR_UNKNOWN_PID;
	if (dst_pid < 0 || m_pdists[src_pid].size() <= (unsigned int)dst_pid)
		return ERR_UNKNOWN_PID;

	return m_pdists[src_pid][dst_pid];
}

int ISPPDistanceMap::getPDistances(int src_pid, std::vector<int>& out_pdists) const
{
	detail::ScopedSharedLock lock(m_mutex);

	if (src_pid < 0 || m_pdists.size() <= (unsigned int)src_pid)
		return ERR_UNKNOWN_PID;

	out_pdists.clear();
	std::copy(m_pdists[src_pid].begin(), m_pdists[src_pid].end(), std::back_inserter(out_pdists));
	return 0;
}

time_t ISPPDistanceMap::getTTL() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_ttl;
}

time_t ISPPDistanceMap::getLastUpdate() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_lastUpdate;
}

std::string ISPPDistanceMap::getVersion() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_version;
}

int ISPPDistanceMap::loadP4PInfo()
{
	P4P_LOG_DEBUG("event:get_pdistancemap,portal:\"" << getPortalAddr() << ':' << getPortalPort() << "\"");

	std::map<PID, std::set<PID> > dummy_pids;

	/* Get the full PDistance matrix */
	protocol::portal::PDistanceMatrix dists;

	std::string pidmap_version;
	time_t pdist_ttl = DEFAULT_TTL;
	if (m_proto)
	{
		/* Load from Portal Server */

		protocol::portal::P4PPortalProtocolMetaInfo meta;
		try
		{
			m_proto->get_pdistance(dummy_pids.begin(), dummy_pids.end(), false, dists, &meta);
		}
		catch (protocol::P4PProtocolConnectionError& e)
		{
			P4P_LOG_ERROR("Failed to load pDistance Map from " << getPortalAddr() << ':' << getPortalPort() << " : " << e.what());
			return ERR_CONNECTION_FAILURE;
		}
		catch (protocol::P4PProtocolError& e)
		{
			P4P_LOG_ERROR("Failed to load pDistance Map from " << getPortalAddr() << ':' << getPortalPort() << " : " << e.what());
			return ERR_PROTOCOL_FAILURE;
		}

		pdist_ttl = (time_t)meta.get_ttl();
		pidmap_version = detail::p4p_token_cast<std::string>(meta.get_version());
	}
	else
	{
		/* Load from Local File */

		std::ifstream file(m_filename.c_str());
		if (!file)
			return ERR_FILE_OPEN_FAILED;

		protocol::portal::detail::ResponsePDistanceReader reader(dists);
		try
		{
			protocol::detail::ReadResponseStream(file, reader);
		}
		catch (protocol::P4PProtocolError& e)
		{
			file.close();
			return ERR_FILE_READ_FAILED;
		}

		file.close();

		/* TODO: Allow file to define TTL and PIDMap version */
	}

	/* Get current mapping between PIDs and indexes */
	std::vector<PID> index_to_pid;
	m_isp->listPIDs(index_to_pid);

	/* Create new pDistance matrix and fill with pDistances. Invalid
	 * entries (those not contained in the pDistance map returned by
	 * the server are left as INT_MAX. */
	PDistanceMatrix new_pdists(index_to_pid.size());
	for (std::vector<PID>::size_type i = 0; i < index_to_pid.size(); ++i)
	{
		new_pdists[i].resize(index_to_pid.size());
		for (std::vector<PID>::size_type j = 0; j < index_to_pid.size(); ++j)
		{
			try
			{
				new_pdists[i][j] = dists.get(index_to_pid[i], index_to_pid[j]);
			}
			catch (std::range_error& e)
			{
				new_pdists[i][j] = INT_MAX;
			}
		}
	}

	/* Replace the existing pdistances */
	detail::ScopedExclusiveLock lock(m_mutex);
	m_pdists = new_pdists;

	m_lastUpdate = time(NULL);
	m_ttl = pdist_ttl;
	m_version = pidmap_version;

	P4P_LOG_DEBUG("event:receive_pdistancemap,isp:" << m_isp << ",ttl:" << m_ttl << ",version:" << m_version << ",matrix:\"" << m_pdists << "\"");

	return 0;
}

std::ostream& operator<<(std::ostream& os, const ISPPDistanceMap::PDistanceMatrix& rhs)
{
	for (unsigned int s = 0; s < rhs.size(); ++s)
		for (unsigned int d = 0; d < rhs[s].size(); ++d)
			os << '{' << s << ',' << d << '=' << rhs[s][d] << '}';

	return os;
}

};

