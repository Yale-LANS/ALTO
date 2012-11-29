#!/usr/bin/perl -w

use strict;
use warnings;

use File::Basename;
use lib dirname($0);

use Generate;

# Usage:
#   <scriptname> DisplayIncrementPeers NumPeersTotal NumSelectPeers ChannelOptions
#
# Generate a script that creates a single channel, and then adds a large number
# of peers. As soon as each peer is added, peer selection is done for the newly-joined
# peer.

# Read options
my $DISPLAY_INCREMENT_PEERS = shift(@ARGV);
defined($DISPLAY_INCREMENT_PEERS) || die("Invalid argument");

my $NUM_PEERS_TOTAL = shift(@ARGV);
defined($NUM_PEERS_TOTAL) || die("invalid argument");

my $NUM_SELECT_PEERS = shift(@ARGV);
defined($NUM_SELECT_PEERS) || die("invalid argument");

my $CHANNEL_OPTIONS = "";
while (my $arg = shift(@ARGV)) {
	$CHANNEL_OPTIONS .= " " . $arg;
}

# Read P4P Portal servers from environment variable
my @portals = split(/\s+/, $ENV{"PORTAL_SERVERS"});
my %pidmaps = getPIDMaps(@portals);

print "sleep 1000\n";

# Create the channel
print "add_channel C $CHANNEL_OPTIONS\n";

# Produce initial snapshots
print "mem_snapshot\n";
print "time_snapshot\n";
print "tracker_snapshot\n";

# Add peers
for (my $p = 0; $p < $NUM_PEERS_TOTAL; ++$p) {
	# Generate random IP address
	my $ip = getRandP4PClientIP(%pidmaps);
	defined($ip) || die("Failed to get random IP address");

	# Add peer and select peers for it
	print "peer_join C P$p $ip 1234\n";
	print "select_peers C P$p $NUM_SELECT_PEERS\n";

	# Display stats on even increments
	if ($p % $DISPLAY_INCREMENT_PEERS == 0) {
		# Print change in resource consumption
		print "show_diff\n";
		print "mem_snapshot\n";
		print "time_snapshot\n";
		print "tracker_snapshot\n";
	}
}

