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


#ifndef P4PPORTALAPI_H
#define P4PPORTALAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Address types
 */
const uint8_t PORTAL_ADDRTYPE_SENDER = 0;				/**< Use the address of the sender. */
const uint8_t PORTAL_ADDRTYPE_IPV4 = 4;					/**< IPv4 address (4-byte length). */
const uint8_t PORTAL_ADDRTYPE_IPV6 = 6;					/**< IPv6 address (16-byte length). */

/**
 * Message types
 */
const uint8_t PORTAL_MSG_ERROR_RSP		= 0x00;			/**< Error response */
const uint8_t PORTAL_MSG_GET_PID_REQ		= 0x01;			/**< GetPID request */
const uint8_t PORTAL_MSG_GET_PID_RSP		= 0x02;			/**< GetPID response */
const uint8_t PORTAL_MSG_GET_PIDS_REQ		= 0x03;			/**< GetPIDs request */
const uint8_t PORTAL_MSG_GET_PIDS_RSP		= 0x04;			/**< GetPIDs response */
const uint8_t PORTAL_MSG_GET_PDISTANCES_REQ	= 0x05;			/**< GetPDistances request */
const uint8_t PORTAL_MSG_GET_PDISTANCES_RSP	= 0x06;			/**< GetPDistances response */
const uint8_t PORTAL_MSG_GET_VIEWS_REQ		= 0x07;			/**< GetViews request */
const uint8_t PORTAL_MSG_GET_VIEWS_RSP		= 0x08;			/**< GetViews response */
const uint8_t PORTAL_MSG_GET_PIDMAP_REQ		= 0x09;			/**< GetPIDMap request */
const uint8_t PORTAL_MSG_GET_PIDMAP_RSP		= 0x10;			/**< GetPIDMap response */
const uint8_t PORTAL_MSG_GET_RECMD_REQ		= 0x11;			/**< GetRecommendation request */
const uint8_t PORTAL_MSG_GET_RECMD_RSP		= 0x12;			/**< GetRecommendation response */
const uint8_t PORTAL_MSG_GET_CAPS_REQ		= 0x13;			/**< GetCapabilities request */
const uint8_t PORTAL_MSG_GET_CAPS_RSP		= 0x14;			/**< GetCapabilities response */

const uint8_t PORTAL_MSG_GET_DIR_REQ		= 0x15;			/**< GetDirectory request */
const uint8_t PORTAL_MSG_GET_DIR_RSP		= 0x16;			/**< GetDirectory response */

const uint8_t PORTAL_MSG_ADMIN_REQ		= 0x17;			/**< Admin request */
const uint8_t PORTAL_MSG_ADMIN_RSP		= 0x18;			/**< Admin response */
const uint8_t PORTAL_MSG_MAX			= PORTAL_MSG_ADMIN_RSP;	/**< Maximum message type */
const uint8_t PORTAL_MSG_COUNT			= PORTAL_MSG_MAX + 1;	/**< Number of message types */

#ifdef __cplusplus
}
#endif

#endif
