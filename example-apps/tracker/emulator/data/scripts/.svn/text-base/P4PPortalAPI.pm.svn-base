use strict;
use warnings;

use Net::Patricia;
use LWP::UserAgent;
use HTTP::Message;
use HTTP::Request;

my $PID_REGEX = '(\d+)\.(i|e)\.(\S+)';

my $DEFAULT_PID_NUM = 99999;

sub parsePID {
	my ($pid) = @_;
	$pid =~ m/$PID_REGEX/
		|| die("Invalid PID '$pid'");
	return (int($1), $2 eq "e" ? 1 : 0, $3);
}

sub makePID {
	my ($num, $ext, $isp) = @_;
	return "$num." . ($ext ? "e" : "i") . ".$isp";
}

sub getPortalConnection {
	my ($server) = @_;

	my $ua = LWP::UserAgent->new;
	$ua->default_header('Accept-Encoding' => 'gzip');
	return $ua;
}

sub getPIDMapTrie {
	my ($server) = @_;

	my %pidmap = getPIDMap($server);

	my $result = new Net::Patricia;

	while (my ($pid, $prefixes) = each(%pidmap)) {
		foreach my $prefix (@{$prefixes}) {
			if ($prefix eq "0.0.0.0/0") {
				defined($result->add_string("128.0.0.0/1", $pid))
					|| die("Failed to add default PID '$pid' to Patricia trie");
				defined($result->add_string("0.0.0.0/1", $pid))
					|| die("Failed to add default PID '$pid' to Patricia trie");
			} elsif (index($prefix, "/32") >= 0) {
				$prefix = substr($prefix, 0, -3) . "/29";
				defined($result->add_string($prefix, $pid))
					|| die("Failed to add prefix '$prefix' in PID '$pid' to Patricia trie");
			} else {
				defined($result->add_string($prefix, $pid))
					|| die("Failed to add prefix '$prefix' in PID '$pid' to Patricia trie");
			}
		}
	}

	return $result;
}

sub getPIDMap {
	my ($server) = @_;

	my $response = getPortalConnection($server)->request(HTTP::Request->new(GET => "http://$server/pid/map"));
	$response->is_success
		|| die("Query for PID Map failed: " . $response->status_line);

	my $body = $response->decoded_content;

	my %result;

	my $pid = undef;
	my $count = -1;
	my $read = 0;
	while ($body =~ m/(\S+)/g) {
		if (!defined($pid)) {
			$pid = $1;
			parsePID($pid); # Validates the PID
			$result{$pid} = [];
			next;
		} elsif ($count < 0) {
			$count = $1;
			$read = 0;
		} elsif ($read < $count) {
			my $prefix = $1;
			push(@{$result{$pid}}, $prefix);
			++$read;
		}

		if ($read >= $count) {
			$pid = undef;
			$count = -1;
		}
	}

	return %result;
}

sub splitPDistanceKey {
	my ($key) = @_;

	my @parts = split(/,/, $key);
	@parts == 2
		|| die("Invalid PDistance key '$key'");

	return @parts;
}

sub getPDistance {
	my ($server) = @_;

	my $response = getPortalConnection($server)->request(HTTP::Request->new(GET => "http://$server/pdistance"));
	$response->is_success
		|| die("Query for PDistances failed: " . $response->status_line);

	my $body = $response->decoded_content;

	my %result;

	my $srcpid = undef;
	my $reverse = undef;
	my $count = -1;
	my $read = 0;
	my $dstpid = undef;
	while ($body =~ m/(\S+)/g) {
		if (!defined($srcpid)) {
			$srcpid = $1;
			parsePID($srcpid); # Validates the PID
		} elsif (!defined($reverse)) {
			$reverse = $1;
			$reverse eq "no-reverse"
				|| die("Expected no-reverse flag in PDistance response. Got '$reverse'");
		} elsif ($count < 0) {
			$count = $1;
			$read = 0;
			if ($read >= $count) {
				$srcpid = undef;
				$reverse = undef;
				$count = -1;
			}
		} elsif ($read < $count && !defined($dstpid)) {
			$dstpid = $1;
			parsePID($dstpid); # Validates the PID
		} elsif ($read < $count) {
			my $pdist = $1;
			$result{"$srcpid,$dstpid"} = $1;
			$dstpid = undef;
			++$read;
			if ($read >= $count) {
				$srcpid = undef;
				$reverse = undef;
				$count = -1;
			}
		}
	}

	return %result;
}

sub getISPID {
	my ($server) = @_;

	# Get Portal PDistance information
	my %pdists = getPDistance($server);
	keys(%pdists) > 0
		|| die("No pDistances found from Portal server");

	# Get the ISP identifier from one of the PIDs
	my @dummykeys = keys(%pdists);
	my ($dummysrc, $dummdst) = splitPDistanceKey(shift(@dummykeys));
	my ($dummy1, $dummy2, $result) = parsePID($dummysrc);

	return $result;
}

1;
