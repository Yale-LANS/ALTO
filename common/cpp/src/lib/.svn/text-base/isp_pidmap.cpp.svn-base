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


#include "p4p/isp_pidmap.h"

#include <fstream>
#include <limits.h>
#include <iterator>
#include <p4p/isp.h>
#include <p4p/errcode.h>
#include <p4p/logging.h>
#include <p4p/detail/util.h>
#include <p4p/protocol-portal/location.h>


namespace p4p {

static const time_t DEFAULT_TTL = 2 * 24 * 60 * 60;

ISPPIDMap::ISPPIDMap(const ISP* isp)
	: m_isp(isp),
	  m_proto(NULL),
	  m_trie(new PIDLookup()),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)
{
}

ISPPIDMap::ISPPIDMap(const ISP* isp, const std::string& addr, unsigned short port)
	: m_isp(isp),
	  m_proto(NULL),
	  m_trie(new PIDLookup()),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)	/* Initial value; reset upon retrieving data */
{
	setDataSourceServer(addr, port);
}

ISPPIDMap::ISPPIDMap(const ISP* isp, const std::string& filename)
	: m_isp(isp),
	  m_proto(NULL),
	  m_trie(new PIDLookup()),
	  m_lastUpdate(0),
	  m_ttl(DEFAULT_TTL)
{
	setDataSourceFile(filename);
}

ISPPIDMap::~ISPPIDMap()
{
	delete m_trie;
	delete m_proto;
}

void ISPPIDMap::setDataSourceServer(const std::string& addr, unsigned short port)
{
	/* Replace reference to existing portal server (if it exists) */
	protocol::portal::LocationPortalProtocol* old_proto = m_proto;
	m_proto = new protocol::portal::LocationPortalProtocol(addr, port);
	delete old_proto;

	/* Clear other data sources */
	m_filename.clear();
}

void ISPPIDMap::setDataSourceFile(const std::string& filename)
{
	/* Replace filename */
	m_filename = filename;

	/* Clear other data sources */
	delete m_proto;
	m_proto = NULL;
}

std::string ISPPIDMap::getPortalAddr() const
{
	return m_proto ? m_proto->get_host() : "";
}

unsigned short ISPPIDMap::getPortalPort() const
{
	return m_proto ? m_proto->get_port() : 0;
}

int ISPPIDMap::lookup(const IPAddress& addr, IPPrefix* matching_prefix) const
{
	detail::ScopedSharedLock lock(m_mutex);
	const int* pid = m_trie->lookup(addr, matching_prefix);
	return pid ? *pid : ERR_UNKNOWN_PID;
}

PID ISPPIDMap::getPIDInfo(int pid) const
{
	detail::ScopedSharedLock lock(m_mutex);
	if (pid < 0 || m_pids.size() <= (std::vector<PID>::size_type)pid)
		return PID::INVALID;
	return m_pids[pid];
}

int ISPPIDMap::getPIDIndex(const PID& pid) const
{
	/* PID Maps are typically small enough that we currently
	 * iterate through instead of storing a separate data
	 * structure. */
	detail::ScopedSharedLock lock(m_mutex);

	for (unsigned int i = 0; i < m_pids.size(); ++i)
		if (m_pids[i] == pid)
			return i;

	return ERR_UNKNOWN_PID;
}

void ISPPIDMap::listPIDs(std::vector<PID>& out_pids) const
{
	detail::ScopedSharedLock lock(m_mutex);
	out_pids.clear();
	std::copy(m_pids.begin(), m_pids.end(), std::back_inserter(out_pids));
}

void ISPPIDMap::getNumPIDs(unsigned int* out_intraisp_pids, unsigned int* out_total_pids) const
{
	detail::ScopedSharedLock lock(m_mutex);
	if (out_intraisp_pids)
		*out_intraisp_pids = m_intraisp_pids;
	if (out_total_pids)
		*out_total_pids = m_pids.size();
}

time_t ISPPIDMap::getTTL() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_ttl;
}

time_t ISPPIDMap::getLastUpdate() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_lastUpdate;
}

std::string ISPPIDMap::getVersion() const
{
	detail::ScopedSharedLock lock(m_mutex);
	return m_version;
}

/* Use separate templated function so we can let
 * the compiler automatically determine the exact
 * type of the OutputIterator.
 */
template <class OutputIterator>
static int ReadFromFile(const std::string& filename, OutputIterator result)
{
	std::ifstream file(filename.c_str());
	if (!file)
		return ERR_FILE_OPEN_FAILED;

	protocol::portal::detail::ResponsePIDMapReader<OutputIterator> reader(result);
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
	return 0;
}

int ISPPIDMap::loadP4PInfo()
{
	P4P_LOG_DEBUG("event:get_pidmap,portal:\"" << getPortalAddr() << ":" << getPortalPort());

	std::vector<protocol::portal::PIDPrefixes> prefixes;

	std::string pidmap_version;
	time_t pidmap_ttl = DEFAULT_TTL;
	if (m_proto)
	{
		/* Load from Portal Server */
		std::vector<PID> empty_pids;
		protocol::portal::P4PPortalProtocolMetaInfo meta;
		try
		{
			m_proto->get_pidmap(empty_pids.begin(), empty_pids.end(), std::back_inserter(prefixes), &meta);
		}
		catch (protocol::P4PProtocolConnectionError& e)
		{
			P4P_LOG_ERROR("Failed to load PIDMap from " << getPortalAddr() << ':' << getPortalPort() << " : " << e.what());
			return ERR_CONNECTION_FAILURE;
		}
		catch (protocol::P4PProtocolError& e)
		{
			P4P_LOG_ERROR("Failed to load PIDMap from " << getPortalAddr() << ':' << getPortalPort() << " : " << e.what());
			return ERR_PROTOCOL_FAILURE;
		}

		pidmap_ttl = (time_t)meta.get_ttl();
		pidmap_version = detail::p4p_token_cast<std::string>(meta.get_version());
	}
	else
	{
		/* Load from local file */
		int rc = ReadFromFile(m_filename, std::back_inserter(prefixes));
		if (rc != 0)
			return rc;

		/* TODO: Allow file to define TTL and PIDMap version */
	}

	/* Build the new trie and PID list (before acquring a lock) */
	PIDInfos pids(prefixes.size());
	PIDInfos::size_type intraisp_pids = 0;
	PIDLookup* new_trie = new PIDLookup();
	for (unsigned int i = 0; i < prefixes.size(); ++i)
	{
		/* Store mapping from index to full PID object */
		pids[i] = prefixes[i].get_pid();

		/* Update count of intra-ISP PIDs */
		if (!pids[i].get_external())
			++intraisp_pids;

		/* Add mapping from each prefix to the PID index */
		for (std::set<IPPrefix>::const_iterator p_itr = prefixes[i].get_prefixes().begin(); p_itr != prefixes[i].get_prefixes().end(); ++p_itr)
			new_trie->add(*p_itr, i);
	}

	/* Get an exclusive lock and update our data structures */
	detail::ScopedExclusiveLock lock(m_mutex);
	PIDLookup* old_trie = m_trie;
	m_trie = new_trie;
	delete old_trie;

	m_pids = pids;
	m_intraisp_pids = intraisp_pids;

	m_lastUpdate = time(NULL);
	m_ttl = pidmap_ttl;
	m_version = pidmap_version;

	P4P_LOG_DEBUG("event:receive_pidmap,isp:" << m_isp << ",ttl:" << m_ttl << ",version:" << m_version << ",pids:\"" << m_pids << "\",prefixes:\"" << *m_trie << "\"");

	return 0;
}

std::ostream& operator<<(std::ostream& os, const ISPPIDMap::PIDLookup& rhs)
{
	std::vector<std::pair<int, std::vector<IPPrefix> > > entries;
	rhs.enumerate(entries);

	for (unsigned int i = 0; i < entries.size(); ++i)
	{
		const int pid = entries[i].first;
		const std::vector<IPPrefix>& prefixes = entries[i].second;

		os << '{' << pid << ':';
		for (unsigned int p = 0; p < prefixes.size(); ++p)
			os << '{' << prefixes[p] << '}';
		os << '}';
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const ISPPIDMap::PIDInfos& rhs)
{
	for (unsigned int i = 0; i < rhs.size(); ++i)
		os << '{' << i << ':' << rhs[i] << '}';
	return os;
}

};
