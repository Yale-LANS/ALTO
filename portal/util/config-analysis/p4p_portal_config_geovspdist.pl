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
# Analysis of Geographical Distance v.s. pDistance
#########################################################

use strict;
use warnings;
use Switch;

use POSIX;
use Scalar::Util;
use NetAddr::IP;
use Geo::IP;

(@ARGV == 2) || die("Usage: $0 <PID map> <pDistance matrix>");

my $GEOIPCITY = $ENV{'GEOIP_DB_CITY'};
defined($GEOIPCITY)
	|| die("Path to (uncompressed) GeoIP City-level database must be specified in GEOIP_DB_CITY environment variable.\n\n"
		. " Note that latest GeoIP City Database can be retrieved from: \n"
		. "  http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz");

my $pi = atan2(1,1) * 4;
my $gi = Geo::IP->open($GEOIPCITY, GEOIP_STANDARD) || die("Failed to load to GeoIP database");


# get geographical location of a sample IP address in PID 
my $repipfile = shift(@ARGV);
my (%loclat, %loclon);

open(IPF, $repipfile) || die("PID map file not found");
my @ips = <IPF>;
close(IPF);

for( my $i = 0; $i < @ips; ++$i )
{
	chomp($ips[$i]);
	my @info = split(/\s+/, $ips[$i]);
	my $loc = shift(@info) || die("Failed to parse PID (line $i in $repipfile): @info");
	my $repip = int(rand($#info));
	my $ipaddr = new NetAddr::IP($info[$repip]) || die("Failed to parse representative IP address (line $i in $repipfile): @info");

	my $r = $gi->record_by_addr($ipaddr->addr());
	(defined($r) && defined($r->latitude) && defined($r->longitude)) || next;

	$loclat{$loc} = $r->latitude;
	$loclon{$loc} = $r->longitude; 
}


# caculate PID pair geographical distance
my $pdistancefile = shift(@ARGV);

open(PF, $pdistancefile) || die("pDistance matrix file not found");
my @pdistances = <PF>;
close(PF);

for( my $i = 0; $i < @pdistances; ++$i )
{
	my $pdistanceinfo = $pdistances[$i];
	chomp($pdistanceinfo);
	my @info = split(/\s+/, $pdistanceinfo);

	# source geo location
	my $srcloc = shift(@info) || die("Failed to parse source PID (line $i in $pdistancefile): @info");
	my $srclat = $loclat{$srcloc};
	my $srclon = $loclon{$srcloc}; 
	(defined($srclat) && defined($srclon)) || next;

	# detination geo location
	my $dstloc = shift(@info) || die("Failed to parse source PID (line $i in $pdistancefile): @info");
	my $dstlat = $loclat{$dstloc};
	my $dstlon = $loclon{$dstloc}; 
	(defined($dstlat) && defined($dstlon)) || next;

	# geo distance
	my $distance = geodistance($srclat, $srclon, $dstlat, $dstlon, 'K');

	# pdistance
	my $pdistance = shift(@info);

	#print "$srcloc $dstloc $pdistance $distance\n";
	print "$pdistance $distance\n";
}


#::: Interal subroutines                                      :::
#
#:::  
#Definitions:                                                 :::
#:::    South latitudes are negative, east longitudes are 
#positive                                                     :::
#:::                                                                         
#:::
#:::  Passed to 
#function:                                                    :::
#:::    lat1, lon1 = Latitude and Longitude of point 1 (in decimal 
#degrees)  :::
#:::    lat2, lon2 = Latitude and Longitude of point 2 (in decimal 
#degrees)  :::
#:::    unit = the unit you desire for 
#results                                                      :::
#:::           where: 'M' is statute 
#miles                                                        :::
#:::                  'K' is kilometers 
#(default)                                                    :::
#:::                  'N' is nautical 
#miles                                                        :::
sub geodistance {
	my ($lat1, $lon1, $lat2, $lon2, $unit) = @_;
	my $theta = $lon1 - $lon2;
	my $dist = sin(deg2rad($lat1)) * sin(deg2rad($lat2)) + cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * cos(deg2rad($theta));
	$dist  = acos($dist);
	$dist = rad2deg($dist);
	$dist = $dist * 60 * 1.1515;
	if( $unit eq "K" ) 
	{
		$dist = $dist * 1.609344;
	} elsif( $unit eq "N" ) 
	{
		$dist = $dist * 0.8684;
	}

	return ($dist);
}

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#:::  This function get the arccos function using arctan function   :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#sub acos {
#	my ($rad) = @_;
#	my $sqr = 1 - $rad**2;
#	if( $sqr < 0.0 ) 
#	{
#		$sqr = 0.0;
#	}
#	my $ret = atan2(sqrt($sqr), $rad);
#	return $ret;
#}

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#:::  This function converts decimal degrees to radians             :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
sub deg2rad {
	my ($deg) = @_;
	return ($deg * $pi / 180);
}

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#:::  This function converts radians to decimal degrees             :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
sub rad2deg {
	my ($rad) = @_;
	return ($rad * 180 / $pi);
}


