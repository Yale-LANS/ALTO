#!/usr/bin/perl -w
#########################################################
# Build a testing P4P portal config based on cost map
#########################################################

use strict;
use warnings;
use Switch;

use POSIX;
use Scalar::Util;

(@ARGV == 2) || die("Usage: $0 <intradomain cost map data> <isp> > <portal config>");

my $costfile = shift(@ARGV);
my $isp = shift(@ARGV);

my %pid;
my %pdist;

# load intra cost
open(OF, $costfile) || die("Configuration file not found");
my $n = 0;
while( <OF> )
{
	chomp;

	my ($pid1, $pid2, $cost) = split(/ +/);

	if (!exists($pid{$pid1}))
	{
		$pid{$pid1} = $n;
		$n++;
	}

	$pdist{$pid1}->{$pid2} = $cost;
}
close(OF);

# build config 
print "version 2.1\n\n";
print "isp default $isp\n\n";

foreach( keys %pdist )
{
	print "pid internal ".$pid{$_}." pid".$pid{$_}." prefixes 1.2.3.".$pid{$_}."/32\n";
}
print "\n";

foreach my $src (keys %pdist) 
{
	foreach my $dst (keys $pdist{$src})
	{
		next if ($pid{$src} == $pid{$dst});
		
		print "pid link pid".$pid{$src}." pid".$pid{$dst}." link-".$pid{$src}."-".$pid{$dst}." capacity 10.0 traffic static 1.0 routing-weight 100\n";	
	}
}
print "\n";
print "pid routing weights\n";

foreach my $src (keys %pdist) 
{
	foreach my $dst (keys $pdist{$src}) 
	{
		next if ($pid{$src} == $pid{$dst});

		print "pdistance link link-".$pid{$src}."-".$pid{$dst}." static ".($pdist{$src}->{$dst})."\n";
	}
}
print "\n";

print "pdistance intra-pid default 0\n";

print "pdistance inter-pid default 10\n";

print "pdistance interdomain exclude intradomain\n";

print "pdistance interdomain default 65\n";

print "pdistance update\n";



