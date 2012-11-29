#!/usr/bin/perl -w

use strict;
use warnings;

use File::Basename;
use lib dirname($0);

use Generate;

# Usage:
#   <scriptname> DisplayIncrementChannels NumChannelsTotal ChannelOptions
#
# Generate a script that creates channels to test both speed and memory overhead.

# Read options
my $DISPLAY_INCREMENT_CHANNELS = shift(@ARGV);
defined($DISPLAY_INCREMENT_CHANNELS) || die("Invalid argument");

my $NUM_CHANNELS_TOTAL = shift(@ARGV);
defined($NUM_CHANNELS_TOTAL) || die("invalid argument");

my $CHANNEL_OPTIONS = "";
while (my $arg = shift(@ARGV)) {
	$CHANNEL_OPTIONS .= " " . $arg;
}

# Read P4P Portal servers from environment variable
my @portals = split(/\s+/, $ENV{"PORTAL_SERVERS"});
my %pidmaps = getPIDMaps(@portals);

print "sleep 1000\n";

# Produce initial snapshots
print "mem_snapshot\n";
print "time_snapshot\n";
print "tracker_snapshot\n";

# Add channels
for (my $c = 0; $c < $NUM_CHANNELS_TOTAL; ++$c) {
	# Add channel
	print "add_channel C$c $CHANNEL_OPTIONS\n";

	# Display stats on even increments
	if ($c % $DISPLAY_INCREMENT_CHANNELS == 0) {
		# Print change in resource consumption
		print "show_diff\n";
		print "mem_snapshot\n";
		print "time_snapshot\n";
		print "tracker_snapshot\n";
	}
}

