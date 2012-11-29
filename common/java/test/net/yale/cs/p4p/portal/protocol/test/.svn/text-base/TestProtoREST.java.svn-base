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


package net.yale.cs.p4p.portal.protocol.test;


import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.Random;

import net.yale.cs.p4p.InetPrefix;
import net.yale.cs.p4p.InetService;
import net.yale.cs.p4p.PID;
import net.yale.cs.p4p.PIDMatrix;
import net.yale.cs.p4p.portal.protocol.ProtoInterface;
import net.yale.cs.p4p.portal.protocol.ProtoREST;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

public class TestProtoREST {
	

	private static final String PORTAL_SERVER_CFG_PATH = "net/yale/cs/p4p/portal/protocol/test/p4p-portal.conf";
	private static final String PORTAL_SERVER_CFG_INTF_PATH = "net/yale/cs/p4p/portal/protocol/test/p4p-portal-intf.conf";
	private static final String PORTAL_SERVER_CFG_SHELL_PATH = "net/yale/cs/p4p/portal/protocol/test/p4p-portal-shell.conf";

	private static final int PORTAL_CLIENT_PORT = 16671;
	private static final int PORTAL_ADMIN_PORT = 16672;

	private static final String PROP_PORTAL_SERVER_EXE = "test.exe.portalserver";
	private static final String PROP_PORTAL_SHELL_EXE = "test.exe.portalshell";
	
	private static final String PORTAL_SERVER_EXE_DEFAULT = "p4p_portal";
	private static final String PORTAL_SHELL_EXE_DEFAULT = "p4p_portal_shell";

	private static File PORTAL_SERVER_CFG;
	private static File PORTAL_SERVER_INTF_CFG;
	private static File PORTAL_SERVER_SHELL_CFG;
	private static Process PORTAL_SERVER;
	private static ProtoInterface QUERY_INTF;
	
	private static Random RNG = new Random();

	private static final File writeTempFile(String resourcePath) throws IOException {
		InputStream resStream = TestProtoREST.class.getClassLoader().getResourceAsStream(resourcePath);
		try {
			File f = File.createTempFile("p4p", null);
			f.deleteOnExit();
			
			OutputStream fStream = new FileOutputStream(f);
			try {
				byte buf[] = new byte[4096];
				int bytesRead;
				while ((bytesRead = resStream.read(buf)) > 0)
					fStream.write(buf, 0, bytesRead);
			} finally {
				fStream.close();
			}
			
			return f;
		} finally {
			resStream.close();
		}
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

		String serverExe = System.getProperty(PROP_PORTAL_SERVER_EXE, PORTAL_SERVER_EXE_DEFAULT);
		String shellExe = System.getProperty(PROP_PORTAL_SHELL_EXE, PORTAL_SHELL_EXE_DEFAULT);

		/* Prepare the configuration files */
		PORTAL_SERVER_CFG = writeTempFile(PORTAL_SERVER_CFG_PATH);
		PORTAL_SERVER_INTF_CFG = writeTempFile(PORTAL_SERVER_CFG_INTF_PATH);
		PORTAL_SERVER_SHELL_CFG = writeTempFile(PORTAL_SERVER_CFG_SHELL_PATH);

		/* Build server and shell command lines */
		StringBuilder serverCmdLine = new StringBuilder();
		serverCmdLine.append(serverExe).append(' ');
		serverCmdLine.append("--log-level=4").append(' ');
		serverCmdLine.append("--config-file=").append(PORTAL_SERVER_CFG.getAbsolutePath()).append(' ');
		serverCmdLine.append("--interfaces-file=").append(PORTAL_SERVER_INTF_CFG.getAbsolutePath()).append(' ');

		StringBuilder shellCmdLine = new StringBuilder();
		shellCmdLine.append(shellExe).append(' ');
		shellCmdLine.append("localhost:").append(PORTAL_ADMIN_PORT).append(' ');
		shellCmdLine.append(PORTAL_SERVER_SHELL_CFG.getAbsolutePath());

		/* Spawn the server. Pause for 1 second to let it start. */
		PORTAL_SERVER = Runtime.getRuntime().exec(serverCmdLine.toString());
		try {
			Thread.sleep(1000);

			/* Spawn portal shell to configure the server */
			Process portalShell = Runtime.getRuntime().exec(shellCmdLine.toString());
			try {
				int rc = portalShell.waitFor();
				if (rc != 0)
					throw new RuntimeException("Portal shell exited with status " + rc);
			} catch (Exception e) {
				portalShell.destroy();
				throw e;
			}

			/* Create interface for querying the server */
			QUERY_INTF = new ProtoREST(new InetService(InetAddress.getLocalHost(), PORTAL_CLIENT_PORT));

		} catch (Exception e) {
			PORTAL_SERVER.destroy();
			throw e;
		}
	}
	
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		/* Clean up the processes we created */
		PORTAL_SERVER.destroy();
	}

	@Test
	public void testGetPIDSingle() throws Exception {
		Map<InetAddress, PID> result = QUERY_INTF.getPIDs(Collections.singleton(InetAddress.getByName("141.161.1.2")));
		assertEquals(1, result.size());
		
		InetAddress addr = InetAddress.getByName("141.161.1.2");
		assertNotNull(result.get(addr));
		assertEquals(new PID("internet2.edu", 2, false), result.get(addr));
	}
	
	@Test
	public void testGetPIDMultiple() throws Exception {
		Collection<InetAddress> query = new ArrayList<InetAddress>();
		query.add(InetAddress.getByName("128.36.1.1"));
		query.add(InetAddress.getByName("198.51.64.56"));
		
		Map<InetAddress, PID> result = QUERY_INTF.getPIDs(query);

		assertEquals(2, result.size());
		
		InetAddress addr1 = InetAddress.getByName("128.36.1.1");
		assertNotNull(result.get(addr1));
		assertEquals(new PID("internet2.edu", 0, false), result.get(addr1));

		InetAddress addr2 = InetAddress.getByName("198.51.64.56");
		assertNotNull(result.get(addr2));
		assertEquals(new PID("internet2.edu", 6, false), result.get(addr2));
	}
	
	@Test
	public void testGetPIDRandom() throws Exception {
		
		final int NUM_TRIALS = 50;
		final int NUM_ADDRS = 100;
		
		for (int i = 0; i < NUM_TRIALS; ++i) {
			Collection<InetAddress> query = new ArrayList<InetAddress>(NUM_ADDRS);
			for (int j = 0; j < NUM_ADDRS; ++j) {
				byte addr[] = new byte[4];
				RNG.nextBytes(addr);
				query.add(InetAddress.getByAddress(addr));
			}

			assertEquals(NUM_ADDRS, QUERY_INTF.getPIDs(query).size());
		}

	}

	private static <T> void assertCollectionsEqual(Collection<T> c1, Collection<T> c2) {
		assertNotNull(c1);
		assertNotNull(c2);
		assertEquals(c1.size(), c2.size());
		for (T c : c1)
			assertTrue(c2.contains(c));
	}
	
	@Test
	public void testGetPIDMapFull() throws Exception {
		
		Map<PID, Collection<InetPrefix>> result = QUERY_INTF.getPIDMap();
		
		assertEquals(13, result.size());
		
		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 0, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("128.36.0.0"), 16),
						new InetPrefix(InetAddress.getByName("210.150.0.0"), 23)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 1, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("12.108.127.0"), 24),
						new InetPrefix(InetAddress.getByName("192.16.104.0"), 23),
						new InetPrefix(InetAddress.getByName("172.16.0.0"), 12)));
		
		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 2, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("141.161.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 3, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("130.207.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 4, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("129.7.235.0"), 24),
						new InetPrefix(InetAddress.getByName("128.83.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 5, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("155.97.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 6, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("192.206.12.0"), 24),
						new InetPrefix(InetAddress.getByName("198.51.64.0"), 24)));
		
		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 7, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("140.142.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 8, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("169.232.0.0"), 16),
						new InetPrefix(InetAddress.getByName("128.195.0.0"), 16),
						new InetPrefix(InetAddress.getByName("128.111.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 7, false)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("140.142.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 100, true)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("10.1.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 105, true)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("10.2.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 120, true)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("10.3.0.0"), 16)));

		assertCollectionsEqual(
				result.get(new PID("internet2.edu", 200, true)),
				Arrays.asList(
						new InetPrefix(InetAddress.getByName("10.0.0.0"), 8)));
	}

	@Test
	public void testGetPDistancesFull() throws Exception {
		PIDMatrix result = QUERY_INTF.getPDistances(null);
		assertEquals(13, result.getSrcs().size());

		assertEquals(19,
				result.get(
						new PID("internet2.edu", 0, false),
						new PID("internet2.edu", 4, false)),
				0.001);

		assertEquals(97,
				result.get(
						new PID("internet2.edu", 8, false),
						new PID("internet2.edu", 200, true)),
				0.001);
	}

}
