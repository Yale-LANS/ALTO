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

public class PID implements Comparable<PID> {

	private String isp_;
	private Integer num_;
	private Boolean ext_;

	public String getISP() { return isp_; }
	public int getNum() { return num_; }
	public boolean getExternal() { return ext_; }

	public PID(String isp, int num, boolean ext) {
		isp_ = isp;
		num_ = num;
		ext_ = ext;
	}

	public boolean equals(Object o) {
		if (o == this)
			return true;

		if (o == null || !(o instanceof PID))
			return false;

		PID other = (PID)o;

		return this.num_.equals(other.num_)
			&& this.ext_.equals(other.ext_)
			&& this.isp_.equals(other.isp_);
	}

	public int compareTo(PID o) {
		if (o == this)
			return 0;

		if (o == null)
			return -1;

		int c = this.num_.compareTo(o.num_);
		if (c != 0)
			return c;

		c = this.ext_.compareTo(o.ext_);
		if (c != 0)
			return c;

		return isp_.compareTo(o.isp_);
	}

	public int hashCode() {
		return ext_.hashCode() + num_.hashCode();
	}

	public String toString() {
		StringBuilder sb = new StringBuilder(32);
		sb.append(num_).append(ext_ ? ".e." : ".i.").append(isp_);
		return sb.toString();
	}

	public static PID parse(String s) throws NumberFormatException {
		int sep1 = s.indexOf('.');
		if (sep1 < 0)
			throw new NumberFormatException();
		int num = Integer.parseInt(s.substring(0, sep1));

		int sep2 = s.indexOf('.', sep1 + 1);
		if (sep2 < 0)
			throw new NumberFormatException();
		String extStr = s.substring(sep1 + 1, sep2);
		boolean ext;
		if (extStr.equals("i"))
			ext = false;
		else if (extStr.equals("e"))
			ext = true;
		else
			throw new NumberFormatException();

		String isp = s.substring(sep2 + 1);
		return new PID(isp, num, ext);
	}

}
