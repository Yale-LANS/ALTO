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


package net.yale.cs.p4p.protocol;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.zip.GZIPInputStream;

import net.yale.cs.p4p.InetPrefix;
import net.yale.cs.p4p.InetService;
import net.yale.cs.p4p.PID;

public class ProtoBase {
	
	public static class HTTPErrorException extends IOException {
		private static final long serialVersionUID = 1L;
		private static int code_;
		public final int getCode() { return code_; }
		private static String reason_;
		public final String getReason() { return reason_; }
		public HTTPErrorException(int code, String reason) {
			code_ = code;
			reason_ = reason;
		}
	}

	public static class InvalidResponseException extends IOException {
		private static final long serialVersionUID = 1L;
	}

	private InetService portal_;
	public InetService getPortal() { return portal_; }

	public ProtoBase(InetService portal) {
		portal_ = portal;
	}

	private URL getURL(String path) {
		try {
			return new URL("http", getPortal().getIP().getHostAddress(), getPortal().getPort(), "/" + path);
		} catch (MalformedURLException e) {
			throw new RuntimeException("Illegal State: failed to construct proper URL");
		}
	}

	protected HttpURLConnection getConnection(String path)
		throws IOException {

		URLConnection conn = getURL(path).openConnection();
		if (!(conn instanceof HttpURLConnection)) {
			conn.getOutputStream().close();
			throw new IOException("Opened connection was not an HTTP connection");
		}
		
		HttpURLConnection httpConn = (HttpURLConnection)conn;
		httpConn.setRequestProperty("Accept-Encoding", "gzip");
		return httpConn;
	}

	protected InputStream getResponseStream(HttpURLConnection conn) throws IOException {
		String encoding = conn.getHeaderField("Content-Encoding");
		if (encoding != null && "gzip".equals(encoding.trim()))
			return new GZIPInputStream(conn.getInputStream());
		else
			return conn.getInputStream();
	}
	
	protected void checkResponseCode(HttpURLConnection conn) throws IOException {
		if (conn.getResponseCode() != HttpURLConnection.HTTP_OK)
			throw new HTTPErrorException(conn.getResponseCode(), conn.getResponseMessage());
	}

	protected String readToken(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {

		int c;

		/* Read until first non-whitespace; quit early on EOF */
		while ((c = stream.read()) >= 0 && Character.isWhitespace(c)) {}
		if (c < 0) {
			if (required)
				throw new InvalidResponseException();
			return null;
		}

		/* Read token */
		StringBuffer sb = new StringBuffer();
		do {
			sb.append((char)c);
		} while ((c = stream.read()) >= 0 && !Character.isWhitespace(c));
		return sb.toString();
	}

	protected PID readPID(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {
		String token = readToken(stream, required);
		if (token == null)
			return null;
		
		try {
			return PID.parse(token);
		} catch (NumberFormatException e) {
			throw new InvalidResponseException();
		}
	}

	protected InetPrefix readInetPrefix(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {
		String token = readToken(stream, required);
		if (token == null)
			return null;
		
		try {
			return InetPrefix.parse(token);
		} catch (NumberFormatException e) {
			throw new InvalidResponseException();
		}
	}

	protected Long readLong(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {
		String token = readToken(stream, required);
		if (token == null)
			return null;

		try {
			return Long.valueOf(token);
		} catch (NumberFormatException e) {
			throw new InvalidResponseException();
		}
	}

	protected Double readDouble(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {
		String token = readToken(stream, required);
		if (token == null)
			return null;
	
		try {
			return token.equalsIgnoreCase("nan") ? Double.NaN : Double.valueOf(token);
		} catch (NumberFormatException e) {
			throw new InvalidResponseException();
		}
	}

	protected InetService readInetService(InputStream stream, boolean required)
		throws IOException, InvalidResponseException {
		String token = readToken(stream, required);
		if (token == null)
			return null;
		
		try {
			return InetService.parse(token);
		} catch (NumberFormatException e) {
			throw new InvalidResponseException();
		}
	}

}
