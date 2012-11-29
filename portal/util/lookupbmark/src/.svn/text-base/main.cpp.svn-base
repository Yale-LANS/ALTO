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


#include "main.h"

#include <iostream>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <p4p/pid.h>
#include <p4p/detail/patricia_trie.h>
#include "options.h"

using namespace p4p;
using namespace p4p::detail;

const unsigned int PAGE_SIZE = 4096;

struct MemInfo
{
	long vmpages;
	long rspages;
	long sharedpages;
	long textpages;
	long libpages;
	long datapages;
	long dirtypages;
};

void print_memusage(MemInfo& meminfo)
{
	const char* procfile = "/proc/self/statm";

	std::ifstream f(procfile);
	if (!f)
	{
		std::cerr << "Failed to open memory information file" << std::endl;
		return;
	}

	if (!(f >> meminfo.vmpages >> meminfo.rspages >> meminfo.sharedpages >> meminfo.textpages >> meminfo.libpages >> meminfo.datapages >> meminfo.dirtypages))
	{
		std::cerr << "Failed to read memory information from file" << std::endl;
		f.close();
		return;
	}

	f.close();

}

int main(int argc, char** argv)
{
	handle_options(argc, argv);

	std::cout.setf(std::ios::fixed);

	PatriciaTrie<PID> tree;

	/* Load PID map */
	{
		std::ifstream tree_file(OPTIONS["pidmap-file"].as<std::string>().c_str());
		if (!tree_file)
		{
			std::cerr << "Failed to open pidmap file" << std::endl;
			return 1;
		}

		MemInfo start_mem;
		print_memusage(start_mem);

		std::cout << "begin loading map" << std::endl;

		unsigned int max = OPTIONS["max-prefixes"].as<unsigned int>();
		unsigned int count = 0;
		PID pid;
		p4p::IPPrefix prefix;
	
		time_t start_time = time(NULL);
		while (tree_file >> pid >> prefix)
		{
			tree.add(prefix, pid);
			++count;
			if (max > 0 && count >= max)
				break;
		}

		time_t end_time = time(NULL);
		time_t elapsed = end_time - start_time;

		std::cout << "finished loading map" << std::endl;
		std::cout << "loaded " << count << " prefixes in " << elapsed << " seconds" << std::endl;
		std::cout << "loaded " << ((double)count / elapsed) << " prefixes per second" << std::endl;

		MemInfo end_mem;
		print_memusage(end_mem);

		std::cout << "Memory Changes:"		<< std::endl
			  << "\tVM Size: "		<< (end_mem.vmpages - start_mem.vmpages) * PAGE_SIZE << std::endl
			  << "\tResident Size: "	<< (end_mem.rspages - start_mem.rspages) * PAGE_SIZE << std::endl
			  << "\tShared Size: "		<< (end_mem.sharedpages - start_mem.sharedpages) * PAGE_SIZE << std::endl
			  << "\tText Size: "		<< (end_mem.textpages - start_mem.textpages) * PAGE_SIZE << std::endl
			  << "\tLibrary Size: "		<< (end_mem.libpages - start_mem.libpages) * PAGE_SIZE << std::endl
			  << "\tData Size: "		<< (end_mem.datapages - start_mem.datapages) * PAGE_SIZE << std::endl
			  << "\tDirty Pages: "		<< (end_mem.dirtypages - start_mem.dirtypages) * PAGE_SIZE << std::endl;

		tree_file.close();
	}

	{
		std::cout << "beginning lookups" << std::endl;

		unsigned int num_lookups = OPTIONS["num-lookups"].as<unsigned int>();
		unsigned int success_lookups = 0;

		
		time_t start_time = time(NULL);
		for (unsigned int i = 0; i < num_lookups; ++i)
		{
			/* Generate random prefix */
			p4p::IPPrefix ip(AF_INET, &i);

			if (tree.lookup(ip))
				++success_lookups;
		}

		time_t end_time = time(NULL);
		time_t elapsed = end_time - start_time;

		std::cout << "finished lookups" << std::endl;
		std::cout << success_lookups << " successful lookups out of " << num_lookups << std::endl;
		std::cout << "looked up " << num_lookups << " addresses in " << elapsed << " seconds" << std::endl;
		std::cout << "looked up " << ((double)num_lookups / elapsed) << " addresses per second" << std::endl;
	}

	return 0;
}

