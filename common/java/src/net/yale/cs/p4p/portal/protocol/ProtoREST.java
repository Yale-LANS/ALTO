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


package net.yale.cs.p4p.portal.protocol;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import net.yale.cs.p4p.InetPrefix;
import net.yale.cs.p4p.InetService;
import net.yale.cs.p4p.PID;
import net.yale.cs.p4p.PIDDestVector;
import net.yale.cs.p4p.PIDMatrix;
import net.yale.cs.p4p.protocol.ProtoBase;

public class ProtoREST extends ProtoBase implements ProtoInterface {

	public ProtoREST(InetService portal) {
		super(portal);
	}

	public Map<InetAddress, PID> getPIDs(Collection<InetAddress> ips) throws IOException {
	
		HttpURLConnection conn = getConnection("pid");
		try {
			/* Write the request */
			conn.setDoOutput(true);
			PrintWriter outWriter = new PrintWriter(conn.getOutputStream());
			try {
				for (InetAddress ip : ips)
					outWriter.println(ip.getHostAddress());
			} finally {
				outWriter.close();
			}
			
			checkResponseCode(conn);

			/* Read the response */
			InputStream inStream = getResponseStream(conn);
			try {
				Map<InetAddress, PID> result = new HashMap<InetAddress, PID>(ips.size());
				while (true) {
					InetPrefix ip = readInetPrefix(inStream, false);
					if (ip == null)
						break;

					result.put(ip.getIP(), readPID(inStream, true));
				}
				return result;
			} finally {
				inStream.close();
			}
		} finally {
			conn.disconnect();
		}

	}

	public PIDMatrix getPDistances(Collection<PIDDestVector> pids)
		throws IOException {

		HttpURLConnection conn = getConnection("pdistance");
		try {
			/* Write the request */
			conn.setDoOutput(true);
			PrintWriter outWriter = new PrintWriter(conn.getOutputStream());
			try {
				if (pids == null)
					pids = Collections.emptyList();
				for (PIDDestVector pair : pids) {
					outWriter.print(pair.getSrc());
					outWriter.print('\t');
					outWriter.print(pair.getReverse() ? "inc-reverse" : "no-reverse");
					outWriter.print('\t');
					outWriter.print(pair.getDsts().size());
					for (PID dst : pair.getDsts()) {
						outWriter.print('\t');
						outWriter.print(dst);
					}
					outWriter.println();
				}
			} finally {
				outWriter.close();
			}

			checkResponseCode(conn);

			/* Read the response */
			InputStream inStream = getResponseStream(conn);
			try {

				try {

					PIDMatrix result = new PIDMatrix();

					while (true) {
						PID src = readPID(inStream, false);
						if (src == null)
							break;

						String reverseToken = readToken(inStream, true);
						boolean reverse;
						if ("inc-reverse".equals(reverseToken))
							reverse = true;
						else if ("no-reverse".equals(reverseToken))
							reverse = false;
						else
							throw new InvalidResponseException();

						long numDsts = readLong(inStream, true);
						
						for (long i = 0; i < numDsts; ++i) {
							PID dst = readPID(inStream, true);
							result.set(src, dst, readDouble(inStream, true));
							if (reverse)
								result.set(dst, src, readDouble(inStream, true));
						}
					}

					return result;

				} catch (NumberFormatException e) {
					throw new InvalidResponseException();
				}

			} finally {
				inStream.close();
			}

		} finally {
			conn.disconnect();
		}

	}

	public Map<PID, Collection<InetPrefix>> getPIDMap()
		throws IOException {

		HttpURLConnection conn = getConnection("pid/map");
		try {

			checkResponseCode(conn);
			
			InputStream inStream = getResponseStream(conn);
			try {
				Map<PID, Collection<InetPrefix>> result = new HashMap<PID, Collection<InetPrefix>>();

				while (true) {
					PID pid = readPID(inStream, false);
					if (pid == null)
						break;

					long prefixCount = readLong(inStream, true);

					Collection<InetPrefix> prefixes = new ArrayList<InetPrefix>();
					for (long i = 0; i < prefixCount; ++i)
						prefixes.add(readInetPrefix(inStream, true));

					result.put(pid, prefixes);
				}

				return result;
			} finally {
				inStream.close();
			}

		} finally {
			conn.disconnect();
		}

	}

}
