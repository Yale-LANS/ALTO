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


#ifndef P4P_PEER_SELECTION_MANAGER_GUIDANCE_MATRIX_H
#define P4P_PEER_SELECTION_MANAGER_GUIDANCE_MATRIX_H

#include <vector>
#include <p4p/app/peer.h>
#include <p4p/app/p4p_selection_manager.h>
#include <p4p/app/peering_guidance_matrix.h>
#include <p4p/app/peer_distribution_manager.h>
#include <p4p/detail/mutex.h>
#include <p4p/detail/compiler.h>

namespace p4p {
namespace app {

/* Forward declaration */
class PeeringGuidanceMatrixManager;

//! Peer selection implementation using a peering guidance matrix.
/**
 * Peer selection implementation using a peering guidance matrix. This class
 * provides two services:
 * - Peer selection functionality (see the addPeer(), removePeer(), and
 *   selectPeers() methods)
 * - Configuration and management of per-ISP guidance (e.g., setDefaultOptions(),
 *   initGuidance(), computeGuidance() and updatePeerStats() methods)
 *
 * After construction, you may configure any desired options using setDefaultOptions()
 * and setISPOptions(), then call initGuidance() to initialize all peering guidance information.
 * For example:
 * \code
 * PGMSelectionManager* pgm_selection = new PGMSelectionManager();
 * pgm_selection->setDefaultOptions(PeeringGuidanceMatrixOptions::FilesharingSwarmDependent());
 * pgm_selection->initGuidance();
 * \endcode
 *
 * Until initGuidance() is called, peer selection (the selectPeers() method)
 * will return an error code.
 *
 * Within a swarm, it is possible to configure different guidance options
 * for a specific ISP.  For example, one may with to use generic guidance for
 * most ISPs (as generic guidance is shared across swarms), and swarm-dependent
 * only for certain ISPs.  The setISPOptions() method configures guidance options
 * for a single ISP.  If ISP-specific options are not present, the default options
 * (configured by the setDefaultOptions() method) take effect for the ISP.  You
 * may revert an ISP the default options using the clearISPOptions() method.
 *
 * NOTE:  Currently the only supported data source is an Optimization
 * Engine server. Using initGuidance() without specifying an Optimization
 * Engine server will result in an exception being thrown.
 */
class p4p_common_cpp_EXPORT PGMSelectionManager : public P4PSelectionManager
{
public:

	/**
	 * Default constructor; construct PGMSelectionManager without an
	 * ISPManager. Before peers can be added, an ISPManager must be
	 * assigned using setISPManager(). initGuidance() should be called
	 * after setISPManager() to ensure guidance is available for
	 * peer selection.
	 */
	PGMSelectionManager();

	/**
	 * Constructor. Note that initGuidance() must be called before guidance
	 * is available.
	 *
	 * NOTE: Currently, the API only supports the case when all ISPs are
	 *   pre-configured. That is, we do not support adding ISPs to the ISPManager
	 *   after a PGMSelectionManager instance is already constructed.
	 *
	 * @param isp_mgr ISPManager which manages global collection of ISPs.
	 */
	PGMSelectionManager(const ISPManager* isp_mgr);

	/**
	 * Destructor
	 */
	virtual ~PGMSelectionManager();

	/**
	 * \copydoc P4PSelectionManager::setISPManager
	 */
	virtual bool setISPManager(const ISPManager* isp_mgr);

	/**
	 * Set the default guidance matrix options.  These options apply to the guidance
	 * matrix for each ISP that has not been explicitly-overridden by the setISPOptions()
	 * method.
	 *
	 * @param options Peering guidance matrix options
	 *
	 * NOTE: initGuidance() or computeGuidance() must be called for options to take effect.
	 */
	void setDefaultOptions(const PeeringGuidanceMatrixOptions& options);

	/**
	 * Get the default guidance matrix options. See setDefaultOptions() for more information.
	 *
	 * @returns Default guidance matrix options.
	 */
	PeeringGuidanceMatrixOptions getDefaultOptions() const;

	/**
	 * Assign ISP-specific guidance matrix options.  These options apply only to the specified
	 * ISP and override any default options that have been configured by setDefaultOptions().
	 *
	 * @param isp Pointer to ISP
	 * @param options Guidance matrix options to apply to the specified ISP
	 * @returns Returns 0 on success, or ERR_INVALID_ISP if isp is not currently maintained
	 *   by this selection manager.
	 *
	 * NOTE: initGuidance() or computeGuidance() must be called for options to take effect.
	 */
	int setISPOptions(const ISP* isp, const PeeringGuidanceMatrixOptions& options);

	/**
	 * Clear ISP-specific guidance matrix options.  The guidance matrix will be configured
	 * with the default options (as assigned by the setDefaultOptions() method).
	 *
	 * If no ISP-specific guidance matrix options existed for the ISP, this method does
	 * nothing and returns 0.
	 *
	 * @param isp Pointer to ISP
	 * @returns Returns 0 on success, or ERR_INVALID_ISP if isp is not currently maintained
	 *   by this selection manager.
	 *
	 * NOTE: initGuidance() or computeGuidance() must be called for options to take effect.
	 */
	int clearISPOptions(const ISP* isp);

	/**
	 * Get the guidance options in effect for an ISP.  If no ISP-specific options
	 * are configured, this returns the default guidance matrix options.  If ISP-specific
	 * options have been configured (see the setISPOptions() method), this returns
	 * those options instead.
	 *
	 * @param isp Pointer to ISP
	 * @param out_options Output parameter; Filled with guidance matrix options in effect for an ISP.
	 * @returns Returns 0 on success, or ERR_INVALID_ISP if isp is not currently maintained
	 *   by this selection manager.
	 */
	int getEffectiveISPOptions(const ISP* isp, PeeringGuidanceMatrixOptions& out_options) const;

	/**
	 * Initialize peering guidance using the previously-configured options (see the
	 * setOption() method). This version specifies that a remote Optimization Engine
	 * server will be used.
	 *
	 * @param optEngineAddr IP Address or Hostname of Optimization Engine server
	 * @param optEnginePort Port number of the Optimization Engine server
	 * @returns Returns true if guidance was successfully initialized, and false
	 *   otherwise (e.g., if initGuidance() had previously been called).
	 */
	bool initGuidance(const std::string& optEngineAddr, unsigned short optEnginePort);

	/**
	 * Initialize peering guidance using the previously-configured options (see the
	 * setOption() method).  This version specifies that peering guidance will be
	 * computed locally.
	 *
	 * NOTE: This functionality is currently not implemented and will throw an exception
	 * when called.
	 */
	bool initGuidance()
	{ throw std::runtime_error("Locally-computed guidance is not currently supported"); }

	/**
	 * Compute peering guidance matrices for all ISPs.  Guidance matrices will
	 * be computed based on currently-configured options (see setOption() methods).
	 */
	void computeGuidance();

	/**
	 * Compute peering guidance matrix for a single ISP. Guidance matrix will
	 * be computed based on currently-configured options (see setOption() methods).
	 *
	 * @param isp ISP whose guidance should be updated.
	 * @returns Returns 0 on success, or ERR_INVALID_ISP if isp is not currently maintained
	 *   by this selection manager.  If an error occurs while the matrix is being
	 *   recomputed, an error code from PeeringGuidanceMatrix::compute() is returned.
	 */
	int computeGuidance(const ISP* isp);

	/**
	 * Get a reference to the guidance matrix used for a particular ISP.
	 *
	 * When the returned guidance matrix is no longer needed releaseGuidanceMatrix()
	 * should be called.
	 *
	 * @param isp Pointer to ISP
	 * @returns Returns pointer to the guidance matrix for the requested ISP, or NULL
	 *   if the guidance matrix was not available (e.g., an invalid ISP or it has not
	 *   yet been computed).
	 */
	const PeeringGuidanceMatrix* getGuidanceMatrix(const ISP* isp);

	/**
	 * Release a reference to a guidance matrix. This should be called whenever the
	 * application is finished using a guidance matrix returned by getGuidanceMatrix().
	 *
	 * @param matrix Pointer to guidance matrix to be released.
	 * @returns Returns true if guidnace matrix was successfully released, and false
	 *   otherwise (e.g., matrix was not obtained via getGuidanceMatrix() or it had
	 *   previously been released and freed).
	 */
	bool releaseGuidanceMatrix(const PeeringGuidanceMatrix* matrix);

	/**
	 * \copydoc P4PSelectionManager::selectPeers
	 *
	 * This method may also return ERR_GUIDANCE_UNAVAILABLE if P4P guidance is not
	 * currently available. In such a case, you should revert to another type
	 * of peer selection (either your native algorithm, or random).
	 */
	virtual int selectPeers(const P4PPeerBase* peer, unsigned int num_peers, std::vector<const P4PPeerBase*>& out_peers);

public:
	/* Methods for advanced usage such as updating */

	/**
	 * Returns the time at which guidance should next be computed for the ISP.
	 *
	 * @param isp Pointer to ISP
	 * @returns Returns the time at which guidance should next be computed for the ISP. If
	 *   guidance is not yet initialized (see initGuidance() method) or the ISP is not maintained
	 *   by this selection, then TIME_INVALID is returned.
	 */
	time_t getNextComputeTime(const ISP* isp) const;

	/**
	 * Time value returned for error conditions.
	 */
	static const time_t TIME_INVALID;

private:
	/** Map from an ISP to its configured guidance matrix options */
	typedef std::map<const ISP*, PeeringGuidanceMatrixOptions> ISPGuidanceOptionMap;
	
	/** Map from an ISP to its configured guidance matrix */
	typedef std::map<const ISP*, const PeeringGuidanceMatrix*> ISPGuidanceMap;

	/**
	 * Internal method for computing guidance for a single ISP
	 */
	int computeGuidanceInternal(const ISP* isp);

	/**
	 * Get the currently configured options for the desired ISP.
	 */
	PeeringGuidanceMatrixOptions getConfiguredOptions(const ISP* isp) const;

	/**
	 * Select peers using location-only guidance
	 */
	int selectPeersLocationOnly(const P4PPeerBase* peer, const ISP* isp, int pid, unsigned int num_peers, std::vector<const P4PPeerBase*>& out_peers);

	bool m_guidance_initialized;			/**< True if initGuidance() has been called */
	std::string m_optengine_addr;			/**< Optimization Engine server address */
	unsigned short m_optengine_port;		/**< Optimization Engine server port */

	PeeringGuidanceMatrixManager* m_pgm_mgr;	/**< Manager for peering guidance matrices */

	p4p::detail::SharedMutex m_guidance_mutex;	/**< Mutex protecting guidance matrix collection */
	PeeringGuidanceMatrixOptions m_guidance_opts_default;	/**< Default guidance options */
	ISPGuidanceOptionMap m_guidance_opts;		/**< ISP-specific options for guidance matrices */
	ISPGuidanceMap m_guidance;			/**< Guidance matrices */
};

}; // namespace app
}; // namespace p4p

#endif

