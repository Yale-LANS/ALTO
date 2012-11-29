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


package net.yale.cs.p4p;

import java.net.InetAddress;
import java.net.UnknownHostException;

public class InetService implements Comparable<InetService> {

	private InetAddress ip_;
	private Integer port_;

	public InetAddress getIP() { return ip_; }
	public int getPort() { return port_.intValue(); }

	public InetService(InetAddress ip, int port) {
		if (ip == null)
			throw new RuntimeException("IP must be non-null");

		ip_ = ip;
		port_ = Integer.valueOf(port);
	}

	public InetService(String hostport) throws UnknownHostException {
		int split = hostport.indexOf(':');
		if (split < 0)
			throw new IllegalArgumentException("Parameter must contain host and port");

		ip_ = InetAddress.getByName(hostport.substring(0, split));
		port_ = Integer.valueOf(hostport.substring(split + 1));
	}

	public boolean equals(Object o) {
		if (o == this)
			return true;

		if (o == null || !(o instanceof InetService))
			return false;

		InetService other = (InetService)o;

		return this.ip_.equals(other.ip_) && this.port_.equals(other.port_);
	}

	public int compareTo(InetService o) {
		if (o == this)
			return 0;

		if (o == null)
			return -1;

		/*
		 * Sort by IP first
		 */
		int c = this.ip_.getHostAddress().compareTo(o.ip_.getHostAddress());
		if (c != 0)
			return c;

		/*
		 * Sort by port only if IPs equal
		 */
		return this.port_.compareTo(o.port_);
	}

	public int hashCode() {
		return ip_.getHostAddress().hashCode() + port_.hashCode();
	}

	public String toString() {
		return ip_.getHostAddress() + ':' + port_.toString();
	}

	public static InetService parse(String s) {
		int sep = s.indexOf(':');
		if (sep < 0)
			throw new NumberFormatException();
		
		int port = Integer.parseInt(s.substring(sep + 1));
		try {
			return new InetService(InetAddress.getByName(s.substring(0, sep)), port);
		} catch (UnknownHostException e) {
			throw new NumberFormatException();
		}
	}

}
