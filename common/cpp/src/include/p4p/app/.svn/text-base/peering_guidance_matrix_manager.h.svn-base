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


#ifndef P4P_PEERING_GUIDANCE_MATRIX_MANAGER_H
#define P4P_PEERING_GUIDANCE_MATRIX_MANAGER_H

#include <vector>
#include <time.h>
#include <p4p/p4p.h>
#include <p4p/detail/mutex.h>
#include <p4p/app/peering_guidance_matrix.h>
#include <p4p/app/peering_guidance_matrix_opts.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

//! Manage a collection of peering guidance matrices
/**
 * Manage a collection of peering guidance matrices.  This class manages creation
 * and deletion of guidance matrices, and shares matrices across users of the
 * matrices (e.g., multiple swarms sharing a generic matrix for an ISP) where
 * possible.
 *
 * The getGuidanceMatrix() method is used to get a pointer to a guidance matrix. When
 * the guidance matrix is no longer needed, releaseGuidanceMatrix() should be called.
 * This is important since this class maintains reference counts for guidance matrices,
 * facilitating sharing of guidance matrices (e.g., generic matrices) where possible.
 *
 * Note that the guidance matrix returned by getGuidanceMatrix() is a constant pointer,
 * meaning the returned guidance matrix does not allow you to configure options for
 * an individual PeeringGuidanceMatrix outside of this class.
 *
 * To adjust options for a guidance matrix, use the setOptions() method. Note that
 * setOptions() returns a pointer to a PeeringGuidanceMatrix object that may be
 * different than the one passed in.  This is important, because setting certain
 * options (see PeeringGuidanceMatrixOptions for more details) may result in a
 * PeeringGuidanceMatrix no longer being sharable (swarm-independent).  In such
 * a case, the reference count for the original matrix is decremented (possibly
 * causing it to be freed) and a new PeeringGuidanceMatrix is returned (possibly
 * shared with an existing instance).
 * 
 * NOTE: All operations provided by this class are thread-safe.
 */
class p4p_common_cpp_EXPORT PeeringGuidanceMatrixManager
{
public:
	/**
	 * Constructor
	 */
	PeeringGuidanceMatrixManager();

	/**
	 * Destructor
	 */
	~PeeringGuidanceMatrixManager();

	/**
	 * Get a reference to a guidance matrix with the specified options. This version of
	 * the method causes the guidance matrix to use a local Optimization Engine.
	 *
	 * When the returned guidance matrix is no longer needed releaseGuidanceMatrix()
	 * should be called.
	 *
	 * NOTE: Usage of a local Optimization Engine is currently not supported and this
	 * method throws an exception when called.
	 *
	 * @param isp Pointer to ISP
	 * @param options Options to be enabled for the guidance matrix returned.
	 */
	const PeeringGuidanceMatrix* getGuidanceMatrix(const ISP* isp,
						       const PeeringGuidanceMatrixOptions& options);

	/**
	 * Get a reference to a guidance matrix with the specified options. This version of
	 * the method causes the guidance matrix to use a remote Optimization Engine server
	 * as its data source.
	 *
	 * When the returned guidance matrix is no longer needed releaseGuidanceMatrix()
	 * should be called.
	 *
	 * @param isp Pointer to ISP
	 * @param options Options to be enabled for the guidance matrix returned.
	 * @param optEngineAddr Hostname or IP Addresss of Optimization Engine server
	 * @param optEnginePort Port number of Optimization Engine server
	 */
	const PeeringGuidanceMatrix* getGuidanceMatrix(const ISP* isp,
						       const PeeringGuidanceMatrixOptions& options,
						       const std::string& optEngineAddr,
						       unsigned short optEnginePort);

	/**
	 * Release a reference to a guidance matrix. If there are no other references to
	 * the guidance matrix, its resources will be freed.
	 *
	 * @param matrix Pointer to guidance matrix to be released.
	 * @returns Returns true if guidnace matrix was successfully released, and false
	 *   otherwise (e.g., matrix was not obtained via getGuidanceMatrix() or it had
	 *   previously been released and freed).
	 */
	bool releaseGuidanceMatrix(const PeeringGuidanceMatrix* matrix);

	/**
	 * Add a reference to an existing guidance matrix (e.g., that has been returned
	 * by the getGuidanceMatrix() method). It is expected that releaseGuidanceMatrix()
	 * will be called for the additional reference returned here.
	 *
	 * @param matrix Pointer to existing guidance matrix
	 * @returns Returns true if reference count for the guidance matrix was incremented
	 *   successfully, and false otherwise (e.g., matrix was not obtained via getGuidanceMatrix()
	 *   or it had previously been released and freed).
	 */
	bool addGuidanceMatrixRef(const PeeringGuidanceMatrix* matrix);

	/**
	 * Configure options for a guidance matrix instance.  The pointer returned by
	 * this method should be used to refer to the guidance matrix instead of the
	 * previous pointer, since the previous guidance matrix may be freed as a
	 * result of this call. For example:
	 * \code
	 * void setOptionsDemo(PeeringGuidanceMatrixManager* mgr, const ISP* isp) {
	 *   PeeringGuidanceMatrixOptions options;
	 *
	 *   // Configure a swarm-dependent matrix
	 *   options.setMatchingUsesSwarmState(true);
	 *   const PeeringGuidanceMatrix* matrix = mgr->getGuidanceMatrix(isp, options);
	 *
	 *   // ... Use matrix weights for peer selection, etc ...
	 *
	 *   // Reconfigure options to be swarm-independent.  The guidance matrix
	 *   // may now be shared across swarms.  A new reference to the generic matrix is
	 *   // returned, and the previous (swarm-dependent) guidance matrix is freed.
	 *   options.setMatchingUsesSwarmState(false);
	 *   matrix = mgr->setOptions(matrix, options);
	 *
	 *   // ... Use matrix weights for peer selection, etc ...
	 * }
	 * \endcode
	 *
	 *
	 * @param matrix Matrix on which options should be set
	 * @param options New options to set
	 * @returns Returns refrence to matrix with the desired options. This matrix may be
	 *   different than the one supplied in the 'matrix' parameter.  If the supplied
	 *   matrix was not currently maintained (e.g., it had not been obtained via getGuidanceMatrix()
	 *   or had previously been released and freed), then this method returns NULL.
	 */
	const PeeringGuidanceMatrix* setOptions(const PeeringGuidanceMatrix* matrix, const PeeringGuidanceMatrixOptions& options);

	/**
	 * Compute the desired guidance matrix without configuring a new swarm state. See
	 * PeeringGuidanceMatrix::compute() for details.
	 *
	 * @param matrix Peering guidance matrix to compute
	 * @returns Returns 0 on success, ERR_INVALID_GUIDANCE_MATRIX if guidance matrix is
	 *   not currently maintained.  If an error occurred during recomputation, the
	 *   error code from PeeringGuidanceMatrix::compute() is returned.
	 */
	int compute(const PeeringGuidanceMatrix* matrix) const;

	/**
	 * Update swarm state for the peering guidance matrix and compute. See
	 * PeeringGuidanceMatrix::compute(SwarmState) for details.
	 *
	 * @param matrix Peering guidance matrix to compute
	 * @param swarm_state Swarm state to use when computing guidance matrix
	 * @returns Returns 0 on success, ERR_INVALID_GUIDANCE_MATRIX if guidance matrix is
	 *   not currently maintained.  If an error occurred during recomputation, the
	 *   error code from PeeringGuidanceMatrix::compute(SwarmState) is returned.
	 */
	int compute(const PeeringGuidanceMatrix* matrix, const SwarmState& swarm_state) const;

private:

	/**
	 * Key including parameters to identify (and construct) a guidance matrix with
	 * a set of desired options.
	 */
	class GuidanceMatrixKey
	{
	public:
		/* Default constructor; all data members initialized to null/empty values */
		GuidanceMatrixKey();

		/* Constructor provided to initialize data members. */
		GuidanceMatrixKey(const ISP* isp,
				  const PeeringGuidanceMatrixOptions& options,
				  const std::string& optEngineAddr = "",
				  unsigned short optEnginePort = 0);

		/* Accessor methods */
		const ISP* getISP() const				{ return m_isp; }
		const PeeringGuidanceMatrixOptions& getOptions() const	{ return m_options; }
		const std::string& getOptEngineAddr() const		{ return m_optengine_addr; }
		unsigned short getOptEnginePort() const			{ return m_optengine_port; }

		/* Overload less-than operator so class can be part of STL maps */
		bool operator<(const GuidanceMatrixKey& rhs) const;

	private:
		const ISP* m_isp;				/**< ISP for the guidance matrix */
		PeeringGuidanceMatrixOptions m_options;		/**< Guidance matrix options */
		std::string m_optengine_addr;			/**< Optimization Engine Host */
		unsigned short m_optengine_port;		/**< Optimization Engine port number */
	};

	/**
	 * Container for a single guidance matrix.  This container handles constructing
	 * the guidance matrix itself, as well as maintaining its reference count.
	 */
	class GuidanceMatrixContainer
	{
	public:
		/* Constructor specifying the guidance matrix key.  The key defines the properties
		 * necessary to construct the guidance matrix */
		GuidanceMatrixContainer(const GuidanceMatrixKey& key);

		/* Destructor */
		~GuidanceMatrixContainer();

		/* Accessor/mutator methods */
		PeeringGuidanceMatrix* getMatrix()	{ return m_matrix; }
		void incRefCount()			{ ++m_refcount; }
		void decRefCount()			{ --m_refcount; }
		bool isReferenced()			{ return m_refcount > 0; }

	private:
		/* Disallow copy constructor and assignment operator (we maintain pointers) */
		GuidanceMatrixContainer(const GuidanceMatrixContainer& rhs) {}
		GuidanceMatrixContainer& operator=(const GuidanceMatrixContainer& rhs) { return *this; }

		PeeringGuidanceMatrix* m_matrix;	/**< Guidance matrix */
		unsigned int m_refcount;		/**< Reference count for guidance matrix */
	};

	/**
	 * Collection of guidance matrices matching a single key (e.g., multiple swarm-dependent
	 * guidance matrices)
	 */
	typedef std::set<GuidanceMatrixContainer*> GuidanceMatrixContainers;

	/**
	 * Map from a guidance matrix key to the guidance matrices matching that key.
	 */
	typedef std::map<GuidanceMatrixKey, GuidanceMatrixContainers> GuidanceMatrixMap;

	/**
	 * Internal method for getting reference to a guidance matrix.
	 */
	const PeeringGuidanceMatrix* getGuidanceMatrix(const GuidanceMatrixKey& key);

	/**
	 * Internal method for getting reference to a guidance matrix without locking
	 */
	const PeeringGuidanceMatrix* getGuidanceMatrixNoLock(const GuidanceMatrixKey& key);

	/**
	 * Internal method for releasing reference to a guidance matrix without locking
	 */
	bool releaseGuidanceMatrixNoLock(const PeeringGuidanceMatrix* matrix);
	
	/**
	 * Internal method for computing the guidance matrix (optionally with a swarm state)
	 */
	int compute(const PeeringGuidanceMatrix* matrix, const SwarmState* swarm_state) const;

	p4p::detail::SharedMutex m_mutex;	/**< Mutex protecting internal data structures */
	GuidanceMatrixMap m_matrices;		/**< The collection of guidance matrices maintained */

};

};
};

#endif

