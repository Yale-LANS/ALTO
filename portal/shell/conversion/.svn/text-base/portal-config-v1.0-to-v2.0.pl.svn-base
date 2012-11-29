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
# Portal configuration conversion: version 1.0 to 2.0
# Convert preprocessed version 1.0 configuration to version 2.0
#################################################################

use strict;
use warnings;
use Switch;

use POSIX;
use Scalar::Util;
use NetAddr::IP;

(@ARGV == 1) || die("Usage: $0 <pre-processed v1.0 configuration file> > <v2.0 configuration file>");

my $OLD_FILE = shift(@ARGV);



# configuration data
my $DEFAULT_ASN = 0;

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

	# skip empty line
	if( ($command =~ /^#.*/) || ($command =~ /^$/) )
	{
		next;
	}

	# parse line
	my @info = split(/\s+/, $command);

	# check keyword (N, V, P, G, E, R, C, I)
	my $keyword = shift(@info);
	$keyword =~ /^[NVPGERCI]$/ || die("Unknown command (line $i): $command");

	switch ($keyword) 
	{
		# Default ASN
		case "N" 
		{
			my $asn = shift(@info);
			(Scalar::Util::looks_like_number($asn) && ($asn > 0) && ($asn < 65535 )) || die("Error AS Number (1-65534) (line $i): $command");   

			$DEFAULT_ASN = $asn;
		}

		# PID info
		case "V"
		{
			# <NET-LOC>
			my $netloc = shift(@info);
			my $asn = $DEFAULT_ASN;
			my $pidloc = 0;
			my @netlocinfo = split(/:/, $netloc);
			if( @netlocinfo > 1 )
			{
				$asn = shift(@netlocinfo);
				(Scalar::Util::looks_like_number($asn) && ($asn > 0) && ($asn < 65535 )) || die("Error AS Number (1-65534) in NET-LOC (line $i): $command");
				$pidloc = shift(@netlocinfo);
				Scalar::Util::looks_like_number($pidloc) || die("Error PID-INT in NET-LOC (line $i): $command");
			}
			else
			{
				$asn = $DEFAULT_ASN;
				$pidloc = $netloc;
				Scalar::Util::looks_like_number($pidloc) || die("Error PID-INT in NET-LOC (line $i): $command");
			}
			$netloc = "$asn:$pidloc";
			# <PID>
			my $pidid = shift(@info);
			!defined($pid_netloc{$pidid}) || die("Redefined PID (line $i): $command");
			push(@pid, $pidid);
			$pid_netloc{$pidid} = $netloc;
			$netloc_pid{$netloc} = $pidid;
			#my @fordesc = split(/"/, $command);
			#(@fordesc <= 3) || die("Error description (line $i): $command"); 
			#$pid_desc{$pidid} = $fordesc[1];
			# <latitude> <longitude> ignored
			#[<A | C | E>]
			if( @info > 0 )
			{
				my $type = pop(@info);
				$type =~ s/[ \t]//g; 
				switch($type)
				{
					case "A" { $pid_type{$pidid} = "aggregation-node"; }
					case "C" { $pid_type{$pidid} = "core-node"; }
					case "E" { $pid_type{$pidid} = "external-node"; }
					else { $pid_type{$pidid} = "aggregation-node"; }
					#else { die("Error node type (line $i): $command"); }
				}
			}
			else
			{
				$pid_type{$pidid} = "aggregation-node";
			}
		}

		# PID prefixes
		case "P"
		{
			# <NET-LOC>
			my $netloc = shift(@info);
			my @netlocinfo = split(/:/, $netloc);
			if( @netlocinfo <= 1 )
			{
				$netloc = "$DEFAULT_ASN:$netloc"
			}
			my $pidid = shift(@info);
			(defined($pid_netloc{$pidid}) && ($pid_netloc{$pidid} eq $netloc)) || die("Unmatched NET-LOC (line $i): $command");
			shift(@info);
			# <access type> | <upload capacity> <download capacity>: ignored
			my $prefix = new NetAddr::IP($info[0]) || shift(@info);
			(@info > 0) || die("Error prefixes (line $i): $command");
			# <list of prefixes>
			for( my $j = 0; $j < @info; $j++ )
			{
				my $prefix = new NetAddr::IP($info[$j]) || die("Error IP prefix (line $i): $command"); 
				$pid_prefixes{$pidid} .= $prefix->cidr()." ";
			}
		}

		# PID link
		case "E"
		{
			# <Src NET-LOC>
			my $srcloc = shift(@info);
			my @netlocinfo = split(/:/, $srcloc);
			if( @netlocinfo <= 1 )
			{
				$srcloc = "$DEFAULT_ASN:$srcloc"
			}
			# <Dst NET-LOC>
			my $dstloc = shift(@info);
			@netlocinfo = split(/:/, $dstloc);
			if( @netlocinfo <= 1 )
			{
				$dstloc = "$DEFAULT_ASN:$dstloc"
			}
			# <PID-link>
			my $linkid = shift(@info);
			!defined($link_srcloc{$linkid}) || die("Redefined link (line $i): $command");
			(defined($netloc_pid{$srcloc}) && defined($netloc_pid{$dstloc})) || die("Error NET-LOC in edge (line $i): $command");
			push(@link, $linkid);
			$link_srcloc{$linkid} = $srcloc;
			$link_dstloc{$linkid} = $dstloc;
			# <capacity>
			my $capacity = shift(@info);
			#(Scalar::Util::looks_like_number($capacity) && ($capacity >= 0)) || die("Error link capacity (line $i): $command"); 
			$link_capacity{$linkid} = $capacity;
			# <traffic>
			my $traffic = shift(@info);
			if( $traffic eq "dynamic" ) 
			{
				$link_traffic{$linkid} = "dynamic";
			}
			else
			{
				#(Scalar::Util::looks_like_number($traffic) && ($traffic >= 0) && ($traffic <= $capacity)) || die("Error link traffic volume (line $i): $command"); 
				$link_traffic{$linkid} = "static $traffic";
			}
			# <routing-weight>
			my $weight = shift(@info);
			Scalar::Util::looks_like_number($weight) || die("Error link weight (line $i): $command"); 
			$link_weight{$linkid} = $weight;
			# <PID> <PID>: ignored
			shift(@info);
			shift(@info);
			# <cost>
			if( @info > 0 )
			{
				my $cost = shift(@info);
				(Scalar::Util::looks_like_number($cost) && ($cost >= 0) && ($cost <= 100)) || die("Error link cost (0.0-100.0) (line $i): $command"); 
				$link_cost{$linkid} = $cost;
			}
			else
			{
				$link_cost{$linkid} = -1;
			}
		}

		# PID routing
		case "R"
		{
			# <Src PID>
			my $srcpid = shift(@info);
			# <Dst PID>
			my $dstpid = shift(@info);
			(defined($pid_netloc{$srcpid}) && defined($pid_netloc{$dstpid})) || die("Undefined PID (line $i): $command");
			my $rid = "$srcpid $dstpid";
			!defined($srcdst_path{$rid}) || die("Conflict routing (line $i): $command"); 
			push(@srcdst, $rid);
			# <PID-link path>
			$srcdst_path{$rid} = @info;
			# disable "pid routing weight"
			$weightrouting = 0;
		}	

		# Virtual costs
		case "C"
		{
			my $subkeyword = shift(@info);
			switch($subkeyword)
			{
				# default
				case "D" 
				{
					# intra-pid
					my $cost = shift(@info);
					(Scalar::Util::looks_like_number($cost) && ($cost >= 0) && ($cost <= 100)) || die("Error intra-pid cost (0.0-100.0) (line $i): $command"); 
					$intrapid = $cost;
					# inter-pid
					$cost = shift(@info);
					(Scalar::Util::looks_like_number($cost) && ($cost >= 0) && ($cost <= 100)) || die("Error inter-pid cost (0.0-100.0) (line $i): $command"); 
					$interpid = $cost;
					if( $intrapid > $interpid ) 
					{
						print "WARNING: Prefer inter-PID than intra-PID.\n         This case is not supported in current P4P. Please contact Yale team.\n";
					}
					# interdomain
					$cost = shift(@info);
					(Scalar::Util::looks_like_number($cost) && ($cost >= 0) && ($cost <= 100)) || die("Error interdomain cost (0.0-100.0) (line $i): $command"); 
					$interdomain = $cost;
				}
				# dynamic-update-rule
				case "R" 
				{
					# intradomain
					$intradomainrule = shift(@info);
					(($intradomainrule eq "MLU") || ($intradomainrule eq "congestion-volume")) || die("Error intradomain dynamic update rule (line $i): $command");
					# interdomain
					$interdomainrule = shift(@info);
					($interdomainrule eq "multihoming-cost") || die("Error interdomain dynamic update rule (line $i): $command");
				}
				# vcost interdomain exclude intradomain
				case "I"
				{
					my $isexclude = shift(@info);
					if( $isexclude eq "exclude" )
					{
						$interexcludeintra = 1;
					}
					else
					{
						die("Error interdomain exclude intradomain (line $i): $command");
					}
				}
				else
				{
					die("Unknown virtual cost command (line $i): $command");
				}
			}
		}
	
		# interdomain
		case "I"
		{
			my $subkeyword = shift(@info);
			# <source PID>
			my $pidid = pop(@info);
			if( !defined($pid_netloc{$pidid}) )
			{
				push(@info, $pidid);
				$pidid = "";
			}
			switch($subkeyword)
			{
				# static
				case "S" 
				{ 
					# <cost>
					my $cost = shift(@info);
					(Scalar::Util::looks_like_number($cost) && ($cost >= 0) && ($cost <= 100)) || die("Error cost value(0.0-100.0) (line $i): $command");
					# <list of destination ASes>
					for( my $j = 0; $j < @info; ++$j )
					{
						my $asid = $info[$j];
						(($asid > 0) && ($asid < 65535 )) || die("Error AS Number (1-65534) (line $i): $command");   
						if( $pidid =~ /^$/  )
						{
							# source independent
							(!defined($as_cost{$asid}) && !defined($as_link{$asid})) || die("Unmatched interdomain virtual cost (line $i: $command");
							push(@as, $asid);
							$as_cost{$asid} = $cost;
						}
						else
						{
							# source dependent
							my $aspidid = "$pidid $asid";
							(!defined($aspid_cost{$aspidid}) && !defined($aspid_link{$aspidid})) || die("Unmatched interdomain virtual cost (line $i: $command");
							push(@aspid, $aspidid);
							$aspid_cost{$aspidid} = $cost;
						}	
					}
				}
				# use-ext-pid-link
				case "E"
				{
					# <external PID-link>
					my $linkid = shift(@info);
					defined($link_srcloc{$linkid}) || die("Undefined interdomain link (line $i): $command");
					my $netloc = $link_dstloc{$linkid};
					my $pidid = $netloc_pid{$netloc};
					($pid_type{$pidid} eq "external-node") || die("Unmatched interdomain link (line $i): $command");
					# <list of detination ASes>
					for( my $j = 0; $j < @info; ++$j )
					{
						my $asid = $info[$j];
						if( $pidid =~ /^$/ )
						{
							# source independent
							(!defined($as_cost{$asid}) && !defined($as_link{$asid})) || die("Unmatched interdomain virtual cost (line $i: $command");
							push(@as, $asid);
							$as_link{$asid} = $linkid;
						}
						else
						{
							# source dependent
							my $aspidid = "$asid $pidid";
							(!defined($aspid_cost{$aspidid}) && !defined($aspid_link{$aspidid})) || die("Unmatched interdomain virtual cost (line $i: $command");
							push(@aspid, $aspidid);
							$aspid_link{$aspidid} = $linkid;
						}	
					}
				}
				else
				{
					die("Unknown interdomain virtual cost command (line $i): $command");
				}
			}
		}

		else 
		{
			# should not go here
		}
	}
}



# PID topology configuration
print "# PID topology configuration\n";
#if( $DEFAULT_ASN > 0 )
#{
#	print "asn default $DEFAULT_ASN\n\n";
#}

# pid {aggregation-node | core-node | external-node} <NETLOC> <PID> ["<description>"] [prefixes <list of prefixes>]
for( my $i = 0; $i < @pid; ++$i )
{
	my $pidid = $pid[$i];
	my $type = $pid_type{$pidid};
	my $netloc = $pid_netloc{$pidid};
	my $desc = "$netloc $pidid";
	if( defined($pid_desc{$pidid}) ) 
	{
		$desc = $pid_desc{$pidid};
	}
	if( defined( $pid_prefixes{$pidid} ) )
	{
		my $prefixes = $pid_prefixes{$pidid};
		print "pid $type $netloc $pidid \"$desc\" prefixes $prefixes\n"
	}
	else
	{
		print "pid $type $netloc $pidid \"$desc\"\n"
	}
}
print "\n";

# pid link <PID> <PID> <PID-link> capacity <capacity> traffic {static <volume> | dynamic} [routing-weight <weight>] ["<description>"] 
for( my $i = 0; $i < @link; ++$i )
{
	my $linkid = $link[$i];
	my $srcloc = $link_srcloc{$linkid};
	my $srcpid = $netloc_pid{$srcloc};
	my $dstloc = $link_dstloc{$linkid};
	my $dstpid = $netloc_pid{$dstloc};
	my $capacity = $link_capacity{$linkid};
	my $traffic = $link_traffic{$linkid};
	my $weight = $link_weight{$linkid};

	print "pid link $srcpid $dstpid $linkid capacity $capacity traffic $traffic routing-weight $weight\n";
}
print "\n\n";

# PID routing configuration
print "# PID routing configuration\n";
if( $weightrouting == 0 )
{
	# pid path <source PID> <destination PID> : <intermediate PID-link> ... <intermediate PID-link>
	for( my $i = 0; $i < @srcdst; ++$i )
	{
		my $srcdstid = $srcdst[$i];
		my $path = $srcdst_path{$srcdstid};

		print "pid path $srcdstid : $path\n";
	}
}
else
{
	# pid routing weights
	print "pid routing weights\n";
}
print "\n\n";

# virtual costs configuration
print "# virtual costs configuration\n";
#print "dynamic-update-rule intradomain $intradomainrule\n";
#print "dynamic-update-rule interdomain $interdomainrule\n\n";

if( $intrapid != -1 )
{
	# vcost intra-pid default <cost (0.0-100.0)>
	print "vcost intra-pid default $intrapid\n";
}
if( $interpid != -1 )
{
	# vcost inter-pid default <cost (0.0-100.0)>
	print "vcost inter-pid default $interpid\n";
}
print "\n";

# vcost link <PID-link> {static <cost (0.0-100.0)> | dynamic}
for( my $i = 0; $i < @link; ++$i )
{
	my $linkid = $link[$i];
	my $cost = $link_cost{$linkid};

	if( $cost != -1 )
	{
		print "vcost link $linkid static $cost\n";
	}
}
print "\n";

if( $interdomain != -1 )
{
	# vcost interdomian default <cost (0.0-100.0)>
	print "vcost interdomain default $interdomain\n";
}
print "\n";

if( $interexcludeintra == 1 )
{
	# vcost interdomain exclude intradomain
	print "vcost interdomain exclude intradomain\n";
}
print "\n";

# vcost interdomain by-dst-asn <destination AS> {static <cost (0.0-100.0)> | use-pid-link-cost <external PID-link>
for( my $i = 0; $i < @as; ++$i )
{
	my $asid = $as[$i];

	if( defined($as_cost{$asid}) )
	{
		my $cost = $as_cost{$asid};
		print "vcost interdomain by-dst-asn $asid static $cost\n";
	}

	if( defined($as_link{$asid}) )
	{
		my $linkid = $as_link{$asid};
		print "vcost interdomain by-dst-asn $asid use-pid-link-cost $linkid\n";
	}
}
print "\n";

# vcost interdomain by-src-pid-dst-asn <source PID> <destination AS> {static <cost (0.0-100.0)> | use-pid-link-cost <external PID-link>
for( my $i = 0; $i < @aspid; ++$i )
{
	my $aspidid = $aspid[$i];

	if( defined($aspid_cost{$aspidid}) )
	{
		my $cost = $aspid_cost{$aspidid};
		print "vcost interdomain by-src-pid-dst-asn $aspidid static $cost\n";
	}

	if( defined($aspid_link{$aspidid}) )
	{
		my $linkid = $aspid_link{$aspidid};
		print "vcost interdomain by-src-pid-dst-asn $aspidid use-pid-link-cost $linkid\n";
	}
}
print "\n\n# P4P iTracker configuration generated by scripts\n\n"; 

# WARNING: this script may change according to iTracker configuration specification
