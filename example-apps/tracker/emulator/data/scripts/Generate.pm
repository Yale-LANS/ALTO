use strict;
use warnings;

use File::Basename;
use lib dirname($0);

use P4PPortalAPI;

sub getPIDMaps {
	my (@servers) = @_;

	my %result;

	foreach my $server (@servers) {
		my %pidmap = getPIDMap($server);

		# Remove PIDs without any prefixes
		my $numprefixes = 0;
		my $numpids = 0;
		for my $pid (keys(%pidmap)) {

			my @prefixes = @{$pidmap{$pid}};
			if (@prefixes == 0) {
				print STDERR "Ignoring empty PID $pid from server $server\n";
				delete $pidmap{$pid};
				next;
			}

			$numprefixes = $numprefixes + @prefixes;
			++$numpids;
		}

		# Ignore if it has no PIDs
		next if ($numpids == 0);

		print STDERR "Using Portal Server: $server with $numpids PIDs and $numprefixes prefixes\n";

		$result{$server} = \%pidmap;
	}

	return %result;
}


sub getRandP4PClientIP {
	my (%pidmaps) = @_;

	# Choose an ISP at random
	my @isps = keys(%pidmaps);
	return undef if (@isps == 0);
	my $isp = $isps[int(rand(@isps))];
	my %pidmap = %{$pidmaps{$isp}};

	# Choose a PID at random
	my @pids = keys(%pidmap);
	my $pid = $pids[int(rand(@pids))];

	# Choose a prefix at random
	my @prefixes = @{$pidmap{$pid}};
	my $prefix = $prefixes[int(rand(@prefixes))];

	# Determine low address for the range
	my ($addr, $length) = split(/\//, $prefix);

	# If no prefix length (e.g., 32-bit length), return address itself
	return $addr if (!defined($length) || $length < 0 || $length >= 32);

	# Generate random offset within subnet
	my $randOffset = int(2**rand(32 - $length));

	# Convert address to integer
	my $addrInt = unpack('N', pack('C4', split('\.', $addr)));

	# Extract necessary part of base address
	$addrInt = $addrInt & ((2**32 - 1) << (32 - $length));

	# Add random offset to base address
	$addrInt = $addrInt + $randOffset;

	return join('.', unpack('C4', pack('N', $addrInt)));
}

1;
