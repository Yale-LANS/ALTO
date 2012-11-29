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



#################################################################
# Portal configuration conversion: version 2.0 to 2.1
# Convert version 2.0 configuration to version 2.1
#################################################################

use strict;
use warnings;
use Switch;

use POSIX;
use Scalar::Util;
use NetAddr::IP;

(@ARGV == 1) || die("Usage: $0 <pre-processed v2.0 configuration file> > <v2.1 configuration file>");

my $OLD_FILE = shift(@ARGV);



# configuration data
my @pid;
my %pid_netloc;
my %netloc_pid;
my %pid_desc;
my %pid_type;
my %pid_prefixes;

my @link;
my %link_srcloc;
my %link_dstloc;
my %link_capacity;
my %link_traffic;
my %link_weight;
my %link_cost;

my $weightrouting = 1;
my @srcdst;
my %srcdst_path;

my $intrapid = -1;
my $interpid = -1;
my $interdomain = -1;

my $intradomainrule = "MLU";
my $interdomainrule = "multihoming-cost";

my $interexcludeintra = 0;

my @as;
my @aspid;
my %as_cost;
my %as_link;
my %aspid_cost;
my %aspid_link;



# load old format configuration
open(OF, $OLD_FILE) || die("Configuration file not found");
my @commands = <OF>;
close(OF);

# translate line by line
for( my $i = 0; $i < @commands; ++$i )
{
	my $command = $commands[$i];
	chomp($command);

	# empty or comment line
	if( ($command =~ /^#.*/) || ($command =~ /^$/) )
	{
		print "$command\n";
		next;
	}

	# parse line
	my @info = split(/\s+/, $command);

	# check keyword 
	my $keyword = shift(@info);

	switch ($keyword) 
	{
		# Default ASN
		case "version"
		{
			print "version 2.1\n";
		}

		case "asn" 
		{
			my (undef, $asn) = @info;
			print "isp default as$asn.net\n";
		}

		# PID info
		case "pid"
		{
			# "link"
			if ($info[0] eq "link")
			{
				shift(@info);

				# <src> <dst> <link>
				my $src = shift(@info) || die("Error pid link configuration (line $i): $command");
				my $dst = shift(@info) || die("Error pid link configuration (line $i): $command");
				my $link = shift(@info) || die("Error pid link configuration (line $i): $command");

				# capacity
				(shift(@info) eq "capacity") || die("Error pid link configuration (line $i): $command");
				my $capacity = shift(@info);

				# traffic
				(shift(@info) eq "traffic") || die("Error pid link configuration (line $i): $command");
				my $traffic = shift(@info);
				if ($traffic eq "static") 
				{
					$traffic .= " ".shift(@info);
				};

				# routing-weight
				my $weight = 1;
				if ((@info > 0) && ($info[0] eq "routing-weight"))
				{
					shift(@info);
					my $weight = shift(@info);
				}

				# "description"
				my $desc = "";
				if (@info > 0)
				{
					my $desc = "description @info";
				}
			
				print "pid link $src $dst $link capacity $capacity traffic $traffic routing-weight $weight $desc\n";
				last;
			}

			# "routing"
			if ($info[0] eq "routing")
			{
				print "pid routing weights\n";
				last;
			}

			# "path"
			if ($info[0] eq "path")
			{
				#Ignore static path
				print "# $command\n";
				print STDERR "Ignoring static routing configuration (line $i):\n  $command\n";
				last;
			}

			# <type> <NEC-LOC> <PID>
			my $type = shift(@info) || die("Error pid configuration (line $i): $command");
			$type =~ s/aggregation-node/internal/;
			$type =~ s/core-node/internal/;
			$type =~ s/external-node/external/;

		
			my $netloc = shift(@info);
			die("Error pid configuration (line $i): $command") if (!defined($netloc));
			my @netlocinfo = split(/:/, $netloc);
			my $number = pop(@netlocinfo);

			my $pid = shift(@info) || die("Error pid configuration (line $i): $command");

			# "description"
			my $desc = "";
			while ((@info > 0) && ($info[0] !~ /prefixes/))
			{
				$desc .= shift(@info)." ";
			}

			# prefixes
			shift(@info);
			my $prefixes = "";
			for(my $j = 0; $j < @info; $j++)
			{
				my $prefix = new NetAddr::IP($info[$j]) || die("Error IP prefix (line $i): $command"); 
				$prefixes .= $prefix->cidr()." ";
			}

			print "pid $type $number $pid";
			if( $prefixes ne "" ) 
			{
				print " prefixes $prefixes";
			}

			if( $desc ne "" )
			{
				print " description $desc";
			}
			print "\n";
		}

		# dynamic-update-rule
		case "dynamic-update-rule"
		{
			print "$command\n";
		}

		# pDistance
		case "vcost"
		{
			my $subkeyword = shift(@info);
			switch($subkeyword)
			{
				case "intra-pid" 
				{
					print "pdistance intra-pid @info\n";
				}
				case "inter-pid" 
				{
					print "pdistance inter-pid @info\n";
				}
				# vcost interdomain exclude intradomain
				case "pid-link"
				{
					print "pdistance pid-link @info\n";
				}
				case "link"
				{
					print "pdistance link @info\n";
				}
				case "interdomain"
				{
					print "# " if (($info[0] ne "default") && ($info[0] ne "exclude"));
					print "pdistance interdomain @info\n";
					print STDERR "Ignoring incompatible interdomain pDistance configuration (line $i):\n  $command\n";
				}	
				else
				{
					die("Error pdistance configuration (line $i): $command");
				}
			}
		}

		else 
		{
			die("Error configuration command (line $i): $command");
		}
	}
}


