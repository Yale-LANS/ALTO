#!/usr/bin/perl -w
# Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the documentation
#       and/or other materials provided with the distribution.
#     * Neither the name of Yale University nor the names of its contributors may
#       be used to endorse or promote products derived from this software without
#       specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#########################################################
# Portal configuration syntax preprocess
# Preprocess version 1.0 configuration file before
# conversion to version 2.0.
#########################################################

use strict;
use warnings;
use Switch;

use POSIX;
use Scalar::Util;
use NetAddr::IP;

(@ARGV == 1) || die("Usage: $0 <v1.0 configuration file> > <pre-processed v1.0 configuration file>");

my $OLD_FILE = shift(@ARGV);

# load old format configuration
open(OF, $OLD_FILE) || die("Configuration file not found");
my @commands = <OF>;
close(OF);

my $asn = 0;

# preprocess line by line
for( my $i = 0; $i < @commands; ++$i )
{
	# read line
	my $command = $commands[$i];
	chomp($command);
	if( ($command =~ /^#.*/) || ($command =~ /^$/) )
	{
		next;
	}

	my @info = split(/[ \t]+/, $command);

	# filtering obsolete keywords (G, N, L, C)
	# other keywords are compatible
	my $keyword = shift(@info);
	if( $keyword !~ /^[GNLC]$/ )
	{
		print "$command\n";
		next;
	}

	# G: (geographical info) ignored 
	if( $keyword eq "G" )
	{
		next;
	}

	# N: default ASN
	if( $keyword eq "N" )
	{
		print "$command\n";
		$asn = shift(@info);
		next;
	}

	# C: (capacity info) ignored
	# C [DRI]: (cost info) 
	if( $keyword eq "C" )
	{
		my $subkeyword = shift(@info);
		if( $subkeyword =~ /^[DRI]$/ )
		{
			print "$command\n";
		}
		next;
	}

	# L: (link info) 
	# <Src NET-LOC>
	my $netloc = shift(@info);
	my $srcloc = "$asn:$netloc";
	# <Dst NET-LOC>
	$netloc = shift(@info);
	my $dstloc = "$asn:$netloc";
	# <PID-link>
	my $linkid = "link-$srcloc-$dstloc";

	# <routing-weight>
	my $weight = shift(@info);
	# <capacity>
	my $capacity = shift(@info);

	# [<Src PID>]: can be any value
	my $srcpid = shift(@info);
	if(!defined($srcpid)) 
	{
		$srcpid = "PID$srcloc";
	}
	# [<Dst PID>]: can be any value
	my $dstpid = shift(@info);
	if(!defined($dstpid)) 
	{
		$dstpid = "PID$dstloc";
	}

	# traffic load
	my $traffic = 0;
	# link vcost
	my $cost = 1;

	# generate new "E" according to "L"
	print "E $srcloc $dstloc $linkid $capacity $traffic $weight $srcpid $dstpid $cost\n";
}


