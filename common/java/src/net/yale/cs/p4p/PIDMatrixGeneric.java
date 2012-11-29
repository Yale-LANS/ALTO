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
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class PIDMatrixGeneric<E> {
	
	private Map<PID, Map<PID, E>> matrix_ = new HashMap<PID, Map<PID, E>>();

	public void set(PID src, PID dst, E value) {
		Map<PID, E> row = matrix_.get(src);
		if (row == null)
			matrix_.put(src, row = new HashMap<PID, E>());
		row.put(dst, value);
	}

	public E get(PID src, PID dst) {
		Map<PID, E> row = matrix_.get(src);
		if (row == null)
			return null;
		E e = row.get(dst);
		if (e == null)
			return null;
		return e;
	}

	public E get(PID src, PID dst, E dflt) {
		E d = get(src, dst);
		return d != null ? d : dflt;
	}

	public Map<PID, E> get(PID src) {
		return matrix_.get(src);
	}

	public Set<PID> getSrcs() {
		return matrix_.keySet();
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();

		Collection<PID> srcs = getSrcs();
		for (PID src : srcs)
		{
			Map<PID, E> dsts = get(src);
			for (Map.Entry<PID, E> e : dsts.entrySet())
				sb.append(src).append(',').append(e.getKey()).append('=').append(e.getValue()).append('\n');
		}

		return sb.toString();
	}
}
