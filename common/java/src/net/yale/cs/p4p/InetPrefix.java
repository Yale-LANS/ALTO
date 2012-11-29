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

public class InetPrefix implements Comparable<InetPrefix> {
	
	private static enum Kind {
		IPV4,
		IPV6,
	};

	private InetAddress ip_;
	private Integer length_;

	public InetAddress getIP() { return ip_; }
	public int getLength() { return length_; }

	public InetPrefix(InetAddress ip, int length) {
		if (ip == null)
			throw new RuntimeException("IP must be non-null");

		ip_ = ip;
		length_ = Integer.valueOf(length);
	}

	public boolean equals(Object o) {
		if (o == this)
			return true;

		if (o == null || !(o instanceof InetPrefix))
			return false;

		InetPrefix other = (InetPrefix)o;

		/* FIXME: Only compare the unmasked part of the IP! */
		return this.ip_.equals(other.ip_) && this.length_.equals(other.length_);
	}

	public int compareTo(InetPrefix o) {
		if (o == this)
			return 0;

		if (o == null)
			return -1;

		/*
		 * Sort by IP first
		 * FIXME: Only compare the unmasked part of the IP!
		 */
		int c = this.ip_.getHostAddress().compareTo(o.ip_.getHostAddress());
		if (c != 0)
			return c;

		/*
		 * Sort by length only if IPs equal
		 */
		return this.length_.compareTo(o.length_);
	}

	public int hashCode() {
		return ip_.getHostAddress().hashCode() + length_.hashCode();
	}

	public String toString() {
		return ip_.getHostAddress() + '/' + length_.toString();
	}

	public static InetPrefix parse(String s) {
		Kind kind;
		if (s.indexOf('.') >= 0)
			kind = Kind.IPV4;
		else if (s.indexOf(':') >= 0)
			kind = Kind.IPV6;
		else
			throw new NumberFormatException();

		int length = 0;
		int sep = s.indexOf('/');
		String addrString;
		if (sep < 0) {
			switch (kind) {
			case IPV4:
				length = 32;
				break;
			case IPV6:
				length = 128;
				break;
			}
			addrString = s;
		} else {
			length = Integer.parseInt(s.substring(sep + 1));
			addrString = s.substring(0, sep);
		}

		InetAddress addr;
		try {
			addr = InetAddress.getByName(addrString);
		} catch (UnknownHostException e) {
			throw new NumberFormatException();
		}
		
		return new InetPrefix(addr, length);
	}

}
