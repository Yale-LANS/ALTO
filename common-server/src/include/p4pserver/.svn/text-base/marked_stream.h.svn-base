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


#ifndef MARKED_STREAM_H
#define MARKED_STREAM_H

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <p4pserver/compiler.h>

class p4p_common_server_EXPORT StreamMarker
{
public:
	StreamMarker() : mark_(0) {}
	unsigned int get_mark() const { return mark_; }
protected:
	void internal_set_mark(unsigned int value) { mark_ = value; }
private:
	unsigned int mark_;
};

class p4p_common_server_EXPORT RequestStream : public boost::iostreams::stream<boost::iostreams::array_source>, public StreamMarker
{
public:
	RequestStream(const char* data, unsigned int size) : boost::iostreams::stream<boost::iostreams::array_source>(data, size) {}
	void mark() { internal_set_mark(boost::iostreams::seek(*this, 0, std::ios_base::cur)); }
};

class p4p_common_server_EXPORT ResponseStream : public boost::iostreams::stream<boost::iostreams::array_sink>, public StreamMarker
{
public:
	ResponseStream(char* data, unsigned int size) : boost::iostreams::stream<boost::iostreams::array_sink>(data, size) {}
	void mark() { internal_set_mark(boost::iostreams::seek(*this, 0, std::ios_base::cur)); }
};

#endif

