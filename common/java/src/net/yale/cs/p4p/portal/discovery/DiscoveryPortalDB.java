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


package net.yale.cs.p4p.portal.discovery;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.InetAddress;

import net.yale.cs.p4p.InetService;
import net.yale.cs.p4p.protocol.ProtoBase;

public class DiscoveryPortalDB extends ProtoBase implements DiscoveryInterface {

	private int connectTimeoutMs_ ;
	private int readTimeoutMs_;
	
	public DiscoveryPortalDB(InetService portaldb, int connectTimeoutMs, int readTimeoutMs) throws IOException {
		super(portaldb);
	}
	
	public DiscoveryPortalDB(InetService portaldb) throws IOException {
		this(portaldb, 0, 0);
	}

	public InetService discover() throws IOException {
		return discover(null);
	}

	public InetService discover(InetAddress addr) throws IOException {

		/* Get a connection to the server */
		HttpURLConnection conn = this.getConnection("portal" + (addr != null ? ("/" + addr.getHostAddress()) : ""));

		conn.setConnectTimeout(connectTimeoutMs_);
		conn.setReadTimeout(readTimeoutMs_);

		try {
			checkResponseCode(conn);

			/* Read the response */
			InputStream inStream = conn.getInputStream();
			try {
				return readInetService(inStream, true);
			} finally {
				inStream.close();
			}

		} finally {
			conn.disconnect();
		}
	}

}
