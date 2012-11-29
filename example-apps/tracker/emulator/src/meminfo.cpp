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

#include "meminfo.h"

#include <iostream>
#include <stdexcept>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>

static const unsigned int PAGE_SIZE = 4096;


MemInfo& MemInfo::operator+=(const MemInfo& rhs)
{
	vmpages		+= rhs.vmpages;
	rspages		+= rhs.rspages;
	sharedpages	+= rhs.sharedpages;
	textpages	+= rhs.textpages;
	libpages	+= rhs.libpages;
	datapages	+= rhs.datapages;
	dirtypages	+= rhs.dirtypages;
	return *this;
}

MemInfo& MemInfo::operator-=(const MemInfo& rhs)
{
	vmpages		-= rhs.vmpages;
	rspages		-= rhs.rspages;
	sharedpages	-= rhs.sharedpages;
	textpages	-= rhs.textpages;
	libpages	-= rhs.libpages;
	datapages	-= rhs.datapages;
	dirtypages	-= rhs.dirtypages;
	return *this;
}

MemInfo MemInfo::operator+(const MemInfo& rhs) const
{
	MemInfo result = *this;
	result += rhs;
	return result;
}

MemInfo MemInfo::operator-(const MemInfo& rhs) const
{
	MemInfo result = *this;
	result -= rhs;
	return result;
}

std::ostream& operator<<(std::ostream& os, const MemInfo& rhs)
{
	return os
		<< "\tVMSize: "		<< (rhs.vmpages * PAGE_SIZE)
		<< "\tResidentSize: "	<< (rhs.rspages * PAGE_SIZE)
		<< "\tSharedSize: "	<< (rhs.sharedpages * PAGE_SIZE)
		<< "\tTextSize: "	<< (rhs.textpages * PAGE_SIZE)
		<< "\tLibrarySize: "	<< (rhs.libpages * PAGE_SIZE)
		<< "\tDataSize: "	<< (rhs.datapages * PAGE_SIZE)
		<< "\tDirtyPages: "	<< (rhs.dirtypages * PAGE_SIZE);
}

MemInfo GetCurrentMemInfo()
{
	const char* procfile = "/proc/self/statm";

	std::ifstream f(procfile);
	if (!f)
		throw std::runtime_error("Failed to open memory information file");

	MemInfo result;
	if (!(f >> result.vmpages
		>> result.rspages
		>> result.sharedpages
		>> result.textpages
		>> result.libpages
		>> result.datapages
		>> result.dirtypages))
	{
		f.close();
		throw std::runtime_error("Failed to read memory information from file");
	}

	f.close();

	return result;
}

unsigned long long GetCurrentTimeMicrosec()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
		throw std::runtime_error("Failed to read current time");

	return 1000000ULL * tv.tv_sec + tv.tv_usec;
}

