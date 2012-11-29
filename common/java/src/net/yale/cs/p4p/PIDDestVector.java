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

import java.util.Collection;
import java.util.Iterator;

public class PIDDestVector implements Comparable<PIDDestVector> {

	private PID src_;
	private Collection<PID> dsts_;
	private boolean reverse_;

	public PID getSrc() { return src_; }
	public Collection<PID> getDsts() { return dsts_; }
	public boolean getReverse() { return reverse_; }
	
	public PIDDestVector(PID src, Collection<PID> dsts) {
		src_ = src;
		dsts_ = dsts;
		reverse_ = false;
	}

	public PIDDestVector(PID src, Collection<PID> dsts, boolean reverse) {
		src_ = src;
		dsts_ = dsts;
		reverse_ = reverse;
	}

	public boolean equals(Object o) {
		if (o == this)
			return true;

		if (o == null || !(o instanceof PIDDestVector))
			return false;

		PIDDestVector other = (PIDDestVector)o;

		if (!this.src_.equals(other.src_))
			return false;

		if (this.dsts_.size() != other.dsts_.size())
			return false;

		Iterator<PID> thisDst = this.dsts_.iterator();
		Iterator<PID> otherDst = other.dsts_.iterator();
		while (thisDst.hasNext() && otherDst.hasNext())
			if (!thisDst.next().equals(otherDst.next()))
				return false;

		return true;
	}

	public int compareTo(PIDDestVector o) {
		if (o == this)
			return 0;

		if (o == null)
			return -1;

		int c = this.src_.compareTo(o.src_);
		if (c != 0)
			return c;

		if (this.dsts_.size() < o.dsts_.size())
			return -1;
		else if (this.dsts_.size() > o.dsts_.size())
			return 1;

		Iterator<PID> thisDst = this.dsts_.iterator();
		Iterator<PID> otherDst = o.dsts_.iterator();
		while (thisDst.hasNext() && otherDst.hasNext()) {
			c = thisDst.next().compareTo(otherDst.next());
			if (c != 0)
				return c;
		}

		return 0;
	}

	public int hashCode() {
		return src_.hashCode();
	}

}
