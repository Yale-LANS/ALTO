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


#include "p4p/app/peering_guidance_matrix_manager.h"

#include <p4p/app/errcode.h>

namespace p4p {
namespace app {


PeeringGuidanceMatrixManager::GuidanceMatrixKey::GuidanceMatrixKey()
	: m_isp(NULL),
	  m_optengine_port(0)
{
}

PeeringGuidanceMatrixManager::GuidanceMatrixKey::GuidanceMatrixKey(const ISP* isp,
								   const PeeringGuidanceMatrixOptions& options,
								   const std::string& optEngineAddr,
								   unsigned short optEnginePort)
	: m_isp(isp),
	  m_options(options),
	  m_optengine_addr(optEngineAddr),
	  m_optengine_port(optEnginePort)
{
}

bool PeeringGuidanceMatrixManager::GuidanceMatrixKey::operator<(const GuidanceMatrixKey& rhs) const
{
	if (m_isp < rhs.m_isp)				return true;
	if (m_isp > rhs.m_isp)				return false;
	if (m_options < rhs.m_options)			return true;
	if (m_options > rhs.m_options)			return false;
	if (m_optengine_addr < rhs.m_optengine_addr)	return true;
	if (m_optengine_addr > rhs.m_optengine_addr)	return false;
	if (m_optengine_port < rhs.m_optengine_port)	return true;
	if (m_optengine_port > rhs.m_optengine_port)	return false;
	return false;
}

PeeringGuidanceMatrixManager::GuidanceMatrixContainer::GuidanceMatrixContainer(const GuidanceMatrixKey& key)
	: m_refcount(1)	/* We create matrix; reference count is initialized to 1 */
{
	/* If optimization engine specified, use it */
	if (!key.getOptEngineAddr().empty())
		m_matrix = new PeeringGuidanceMatrix(key.getISP(), key.getOptEngineAddr(), key.getOptEnginePort());
	else
		m_matrix = new PeeringGuidanceMatrix(key.getISP());

	/* Configure options */
	m_matrix->setOptions(key.getOptions());
}

PeeringGuidanceMatrixManager::GuidanceMatrixContainer::~GuidanceMatrixContainer()
{
	delete m_matrix;
}

PeeringGuidanceMatrixManager::PeeringGuidanceMatrixManager()
{
}

PeeringGuidanceMatrixManager::~PeeringGuidanceMatrixManager()
{
	/* Free resources for all maintained guidance matrices */

	/* Iterate over all keys */
	for (GuidanceMatrixMap::const_iterator key_itr = m_matrices.begin(); key_itr != m_matrices.end(); ++key_itr)
	{
		const GuidanceMatrixContainers& key_matrices = key_itr->second;

		/* Iterate over all matrices for each key */
		for (GuidanceMatrixContainers::const_iterator itr = key_matrices.begin(); itr != key_matrices.end(); ++itr)
		{
			GuidanceMatrixContainer* matrix = *itr;
			delete matrix;
		}
	}

	m_matrices.clear();
}

const PeeringGuidanceMatrix* PeeringGuidanceMatrixManager::getGuidanceMatrix(const ISP* isp,
									     const PeeringGuidanceMatrixOptions& options)
{
	GuidanceMatrixKey key(isp, options);
	return getGuidanceMatrix(key);
}

const PeeringGuidanceMatrix* PeeringGuidanceMatrixManager::getGuidanceMatrix(const ISP* isp,
									     const PeeringGuidanceMatrixOptions& options,
									     const std::string& optEngineAddr,
									     unsigned short optEnginePort)
{
	GuidanceMatrixKey key(isp, options, optEngineAddr, optEnginePort);
	return getGuidanceMatrix(key);
}

const PeeringGuidanceMatrix* PeeringGuidanceMatrixManager::getGuidanceMatrix(const GuidanceMatrixKey& key)
{
	p4p::detail::ScopedExclusiveLock lock(m_mutex);
	return getGuidanceMatrixNoLock(key);
}

const PeeringGuidanceMatrix* PeeringGuidanceMatrixManager::getGuidanceMatrixNoLock(const GuidanceMatrixKey& key)
{
	/* Look for matrices with existing key; if key not found, add it */
	GuidanceMatrixMap::iterator itr = m_matrices.find(key);
	if (itr == m_matrices.end())
	{
		/* Insert the key with an empty set of matrices */
		itr = m_matrices.insert(std::make_pair(key, GuidanceMatrixContainers())).first;
	}
	GuidanceMatrixContainers& key_matrices = itr->second;

	/* We must create a new matrix in two cases:
	 * - Options indicate a swarm-dependent matrix
	 * - Options indicate a swarm-independent matrix and we don't currently have one
	 */
	if (key.getOptions().isSwarmDependent() || key_matrices.empty())
	{
		/* We are constructing a swarm-dependent matrix. New matrix is constructed,
		 * added to the collection, and returned.  New matrices are constructed
		 * with a reference count of 1.
		 */
		GuidanceMatrixContainer* matrix = new GuidanceMatrixContainer(key);
		key_matrices.insert(matrix);
		return matrix->getMatrix();
	}

	/* If we get here, the options indicate a swarm-independent matrix and one
	 * must already exist.  Thus, we increment its reference count and return it.
	 */
	GuidanceMatrixContainer* matrix = *key_matrices.begin();
	matrix->incRefCount();
	return matrix->getMatrix();
}

bool PeeringGuidanceMatrixManager::releaseGuidanceMatrix(const PeeringGuidanceMatrix* matrix)
{
	p4p::detail::ScopedExclusiveLock lock(m_mutex);
	return releaseGuidanceMatrixNoLock(matrix);
}

bool PeeringGuidanceMatrixManager::releaseGuidanceMatrixNoLock(const PeeringGuidanceMatrix* matrix)
{
	/* Construct key to identify the guidance matrix within our data structure */
	GuidanceMatrixKey key(matrix->getISP(),
			      matrix->getOptions(),
			      matrix->getOptEngineAddr(),
			      matrix->getOptEnginePort());

	/* Locate key for the matrices */
	GuidanceMatrixMap::iterator key_itr = m_matrices.find(key);
	if (key_itr == m_matrices.end())
		return false;
	GuidanceMatrixContainers& key_matrices = key_itr->second;

	/* Look through the matrices matching the key to find the desired one */
	for (GuidanceMatrixContainers::iterator itr = key_matrices.begin(); itr != key_matrices.end(); ++itr)
	{
		GuidanceMatrixContainer* matrix_container = *itr;

		/* Skip if it isn't the desired matrix */
		if (matrix_container->getMatrix() != matrix)
			continue;

		/* Decrement reference count */
		matrix_container->decRefCount();

		/* We're done if the matrix is still referenced */
		if (matrix_container->isReferenced())
			return true;

		/* Matrix is no longer referenced; remove from collection and free it */
		key_matrices.erase(itr);
		delete matrix_container;
		return true;
	}

	/* Desired matrix not found */
	return false;
}

bool PeeringGuidanceMatrixManager::addGuidanceMatrixRef(const PeeringGuidanceMatrix* matrix)
{
	p4p::detail::ScopedExclusiveLock lock(m_mutex);

	/* Construct key to identify the guidance matrix within our data structure */
	GuidanceMatrixKey key(matrix->getISP(),
			      matrix->getOptions(),
			      matrix->getOptEngineAddr(),
			      matrix->getOptEnginePort());

	/* Locate key for the matrices */
	GuidanceMatrixMap::iterator key_itr = m_matrices.find(key);
	if (key_itr == m_matrices.end())
		return false;
	GuidanceMatrixContainers& key_matrices = key_itr->second;

	/* Look through the matrices matching the key to find the desired one */
	for (GuidanceMatrixContainers::iterator itr = key_matrices.begin(); itr != key_matrices.end(); ++itr)
	{
		GuidanceMatrixContainer* matrix_container = *itr;

		/* Skip if it isn't the desired matrix */
		if (matrix_container->getMatrix() != matrix)
			continue;

		/* Decrement reference count and report success */
		matrix_container->incRefCount();
		return true;
	}

	/* Desired matrix not found */
	return false;
}

const PeeringGuidanceMatrix* PeeringGuidanceMatrixManager::setOptions(const PeeringGuidanceMatrix* matrix,
							       const PeeringGuidanceMatrixOptions& options)
{
	/* Nothing to do if the options haven't changed */
	if (options == matrix->getOptions())
		return matrix;

	p4p::detail::ScopedExclusiveLock lock(m_mutex);

	/* Currently we take a simple approach and release the previous
	 * matrix and allocate a new one with the desired options.
	 * TODO: One option is to update the options in-place where possible.
	 */

	GuidanceMatrixKey key(matrix->getISP(),
			      options,
			      matrix->getOptEngineAddr(),
			      matrix->getOptEnginePort());

	/* Release previous reference */
	if (!releaseGuidanceMatrixNoLock(matrix))
		return NULL;

	return getGuidanceMatrixNoLock(key);
}

int PeeringGuidanceMatrixManager::compute(const PeeringGuidanceMatrix* matrix) const
{
	return compute(matrix, NULL);
}

int PeeringGuidanceMatrixManager::compute(const PeeringGuidanceMatrix* matrix, const SwarmState& swarm_state) const
{
	return compute(matrix, &swarm_state);
}

int PeeringGuidanceMatrixManager::compute(const PeeringGuidanceMatrix* matrix, const SwarmState* swarm_state) const
{
	p4p::detail::ScopedSharedLock lock(m_mutex);

	/* Ensure that we actually manage this matrix */

	GuidanceMatrixKey key(matrix->getISP(),
			      matrix->getOptions(),
			      matrix->getOptEngineAddr(),
			      matrix->getOptEnginePort());

	GuidanceMatrixMap::const_iterator key_itr = m_matrices.find(key);
	if (key_itr == m_matrices.end())
		return ERR_INVALID_GUIDANCE_MATRIX;

	const GuidanceMatrixContainers& key_matrices = key_itr->second;

	/* Look through the matrices matching the key to find the desired one */
	for (GuidanceMatrixContainers::const_iterator itr = key_matrices.begin(); itr != key_matrices.end(); ++itr)
	{
		GuidanceMatrixContainer* matrix_container = *itr;

		/* Skip if it isn't the desired matrix */
		if (matrix_container->getMatrix() != matrix)
			continue;

		/* We've found it; now compute */
		if (swarm_state)
			return matrix_container->getMatrix()->compute(*swarm_state);
		else
			return matrix_container->getMatrix()->compute();
	}

	/* Matrix not found */
	return ERR_INVALID_GUIDANCE_MATRIX;
}

};
};

