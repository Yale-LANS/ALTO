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


#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <boost/filesystem.hpp>
#include <string>

static const boost::filesystem::path PARENT_DIR = boost::filesystem::path("..");

#ifdef __linux
static const boost::filesystem::path ROOT = boost::filesystem::path("/");
static const boost::filesystem::path DEFAULT_SIBLING_CONFIG_DIR = PARENT_DIR / "conf";
static const boost::filesystem::path DEFAULT_SYSTEM_CONFIG_DIR = ROOT / "etc" / "p4p";
static const boost::filesystem::path DEFAULT_STATE_DIR = ROOT / "var" / "lib" / "p4p";
static const boost::filesystem::path DEFAULT_TEMP_DIR = ROOT / "tmp";
#else
	#error Unsupported platform!
#endif


static const boost::filesystem::path CONFIG_FILENAME = "p4p-portal.conf";
static const boost::filesystem::path INTERFACES_FILENAME = "p4p-portal-intf.conf";
static const boost::filesystem::path NETSTATE_FILENAME = "net.state";
static const boost::filesystem::path VIEWREG_FILENAME = "views.state";
static const boost::filesystem::path VIEWSTATE_FILENAME_BASE = "view.state.";

static const unsigned int NSEC_PER_USEC = 1000;
static const unsigned int USEC_PER_MSEC = 1000;
static const unsigned int MSEC_PER_SSEC = 1000;

#endif
