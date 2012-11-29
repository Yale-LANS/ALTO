#!/usr/bin/perl -w
#########################################################
# Simulator for Application together with AOE
#########################################################

use strict;
use warnings;
use Carp;
use Scalar::Util;
use Math::Trig;

(@ARGV >= 5) || die("Usage: $0 <Time> <K videos> <L locations> <location distribution file> <guidance file> [algorithm]");

my $debug = 0;
my $T = shift(@ARGV); 
my $K = shift(@ARGV);
my $L = shift(@ARGV);
my $channel = shift(@ARGV);
my $guidance = shift(@ARGV);
my $algorithm = 0;
if (@ARGV > 0)
{
	$algorithm = shift(@ARGV);
}

# get client distribution
my @supply = ();
my @demand = ();
my $total_demand = 0;
open (DF, "<$channel") || die("application distribution $channel not found");
while(<DF>)
{
	# i S_i D_i
	chomp;
	my ($loc, $si, $di) = split(/\s+/, $_);
	$supply[$loc] = $si;
	$demand[$loc] = $di;
	$total_demand += $di;
	#print STDERR "Distribution\t$loc\t$si\t$di\n";

	#WARNING: should match L
}
close(DF);
#print STDERR "Total-demand\t$total_demand\n";

# get network cost map and guidance
my %pdistance = ();
my %cdn_guidance = ();
my %p2p_guidance = ();

#`rm -f pdistance`;
#`wget -q http://$p4p/pdistance`;
open (PDF, "<$guidance") || die("guidance $guidance not found");
while(<PDF>)
{
	# i j p_ij x^p_ij x^c_ij
	chomp;
	my ($self, $remote, $distance, $xpij, $xcij) = split(/\s+/, $_);
	$pdistance{"$self-$remote"} = $distance;
	$p2p_guidance{"$self-$remote"} = $xpij;
	$cdn_guidance{"$self-$remote"} = $xcij;
	print STDERR "Guidance\t$self\t$remote\t$distance\t$xpij\t$xcij\n";

	#WARNING: should match L
}
close(PDF);

# data structures
my %p2p_traffic = ();
my %cdn_traffic = (); 
my %p2p_ratio = ();
my %cdn_prob_bin = ();
my @cdn_ratio = ();
my @arrival_rate = ();
my @video_rate = ();
my @video_length = ();
my @k_v = ();
my %v_k = ();
my %v_loc = ();
my %v_cdn_loc = ();
my %v_ttl = ();
my $V = 0;
my ($k, $i, $j, $v, $t);

# randomness configuration
my $power_law = 3; 
my $arr_high = 1 ** ($power_law + 1); # 0.3 / min
my $arr_low = 0.01 ** ($power_law + 1); 

# genrate video arrival rate following power law distribution, video rate, video length
#FIXME: client arrival rate distribution different across locations
for ($k = 0; $k < $K; $k++)
{
	#x = [(x1^(n+1) - x0^(n+1))*y + x0^(n+1)]^(1/(n+1))
	$arrival_rate[$k] = (($arr_high - $arr_low) * rand() + $arr_low) ** (1 / ($power_law + 1));
	$arrival_rate[$k] = 1 - $arrival_rate[$k];

	my $coin = rand();
	if ($coin < 0.7)
	{
		$video_rate[$k] = 205;
		$video_length[$k] = 210 + rand(900); #3.5+ min
	}
	elsif ($coin < 0.9)
	{
		$video_rate[$k] = 400;
		$video_length[$k] = 420 + rand(1800); #7+ min
	}
	else
	{
		$video_rate[$k] = 1000;
		$video_length[$k] = 900 + rand(4500); #15+ min
	}

	print "Video\t$k\t".($arrival_rate[$k])."\t".($video_rate[$k])."\t".($video_length[$k])."\n";
}

# compute hash bins to map to locations
my @location_hash_bin = ();
my $bh = 0;
for ($i = 0; $i < $L; $i++)
{
	$location_hash_bin[$i] = $bh;
	$bh += $demand[$i]; #FIXME: if client arrival dist is diff across locations, bins need to be recomputed
}

# compute P_ij
for ($i = 0; $i < $L; $i++)
{
	my $sum_ppji = 0;
	my $sum_xcji = 0;
	my $sum_xpji = 0;
	for ($j = 0; $j < $L; $j++)
	{
		# P2P ratio
		my $tw = 0;
		for ($k = 0; $k < $K; $k++)
		{
			$tw += $video_rate[$k] * ($video_length[$k] * $video_length[$k]) * ($arrival_rate[$k] * $demand[$i]) * ($arrival_rate[$k] * $demand[$j]) / ($total_demand * $total_demand); #FIXME: eta_j
		}
		#print STDERR "tw=$tw\n";
		if ($tw < 0.001)
		{# should not happen
			$tw = 0.001;
		}

		$p2p_ratio{"$j-$i"} = $p2p_guidance{"$j-$i"} / $tw;
		$sum_ppji += $p2p_ratio{"$j-$i"};

		# CDN probability and ratio
		$sum_xcji += $cdn_guidance{"$j-$i"};
		$sum_xpji += $p2p_guidance{"$j-$i"};
	}

	my $cdn_prob_bh = 0;
	for ($j = 0; $j < $L; $j++)
	{
		# P2P ratio
		$p2p_ratio{"$j-$i"} /= $sum_ppji;
		print STDERR "PP\t$j\t$i\t".($p2p_ratio{"$j-$i"})."\n";

		# CDN probability
		$cdn_prob_bin{"$j-$i"} = $cdn_prob_bh;
		$cdn_prob_bh += ($cdn_guidance{"$j-$i"} / $sum_xcji);
	}
	# CDN ratio
	$cdn_ratio[$i] = $sum_xcji / ($sum_xcji + $sum_xpji);
}

# simulate across time
my $deltat = 60;
for ($t = 0; $t < $T; $t += $deltat)
{
	# generate client for each channel
	for ($k = 0; $k < $K; $k++)
	{
		if( rand() < $arrival_rate[$k] )
		{
			$V++;
			$v = $V;
			$v_k{$v} = $k;
			$k_v[$k]->{$v} = $v;
			$v_loc{$v} = get_loc();
			$v_cdn_loc{$v} = get_cdn_loc($v_loc{$v});
			$v_ttl{$v} = $video_length[$k];
			print "Client $v arrive: video $k, time $t, loc ".($v_loc{$v}).", ttl ".($v_ttl{$v})."\n";
		}
	}

	# measure each client's traffic 
	my @vs = keys(%v_k);
	foreach $v (@vs)
	{
		# clean up departed clients
		$k = $v_k{$v};
		$v_ttl{$v} -= $deltat;
		if ($v_ttl{$v} <= 0)
		{
			print "Client $v depart: video $k, time $t, loc ".($v_loc{$v}).", ttl ".($v_ttl{$v})."\n";
			delete $k_v[$k]->{$v};
			delete $v_ttl{$v};
			delete $v_loc{$v};
			delete $v_cdn_loc{$v};
			delete $v_k{$v};
			next;
		}

		# measure P2P traffic
		$i = $v_loc{$v};
		my $deficit = $video_rate[$k];
		my @us = keys(%{$k_v[$k]});
		my $npeers = @us;
		if (rand() < 0.1)
		{ # sampling P2P concurrency
			print "CONCURRENT\t".($npeers+1)."\n";
		}
		foreach my $u (@us)
		{
			next if ($u == $v);

			$j = $v_loc{$u};
			my $tuv = 0;
			if ($algorithm == 0)
			{ # guided
				$tuv = $p2p_ratio{"$j-$i"} * $video_rate[$k] * (1 - $cdn_ratio[$i]); 
				#FIXME: measure and use eta
			}
			elsif ($algorithm == 1)
			{ # random
				$tuv = $video_rate[$k] / $npeers;
			}

			$p2p_traffic{"$j-$i"} += $tuv * $deltat;
			$deficit -= $tuv;
		}	

		# conservatively measure CDN source
		$j = $v_cdn_loc{$v};
		if ($algorithm == 0)
		{
			$cdn_traffic{"$j-$i"} += $video_rate[$k] * $cdn_ratio[$i] * $deltat;
			$deficit -= $video_rate[$k] * $cdn_ratio[$i];
		}
		if ($deficit > 0)
		{
			$cdn_traffic{"$j-$i"} += $deficit * $deltat;
		}
	}

	next if ($t % ($deltat * 3) > 0);

	my ($netcost, undef, undef, undef, undef) = get_netcost();
	print "TIMENETCOST\t$t\t$netcost\n";
}

# output

my ($netcost, $sum_cost, $sum_p2p, $sum_cdn) = get_netcost();
print "NETCOST\t$sum_cost\t".($sum_p2p+$sum_cdn)."\t$netcost\n";
print "CDN\t$sum_cdn\t".($sum_cdn/($sum_p2p+$sum_cdn))."\n";

# Utils
sub get_loc 
{
	my $coin = rand($total_demand);
	for($i = 0; $i < $L - 1; $i++)
	{
		if ($coin >= $location_hash_bin[$i] and $coin < $location_hash_bin[$i+1])
		{
			return $i;
		}
	}

	return $i;
}

sub get_cdn_loc
{
	my ($client_loc) = @_;

	my $coin = rand();
	for($i = 0; $i < $L - 1; $i++)
	{
		my $inext = $i + 1;
		if ($coin >= $cdn_prob_bin{"$i-$client_loc"} and $coin < $cdn_prob_bin{"$inext-$client_loc"})
		{
			return $i;
		}
	}

	return $i;
}

sub get_netcost
{
	my $sum_cost; 
	my $sum_p2p; 
	my $sum_cdn; 
	for ($i = 0; $i < $L; $i++)
	{
		for ($j = 0; $j < $L; $j++)
		{
			if (!exists($p2p_traffic{"$i-$j"}))
			{
				$p2p_traffic{"$i-$j"} = 0;
			}
			if (!exists($cdn_traffic{"$i-$j"}))
			{
				$cdn_traffic{"$i-$j"} = 0;
			}

			if ($debug == 1)
			{
				print "P2P\t$i\t$j\t".($p2p_traffic{"$i-$j"})."\n";
				print "CDN\t$i\t$j\t".($p2p_traffic{"$i-$j"})."\n";
				print "TRAFFIC\t$i\t$j\t".($p2p_traffic{"$i-$j"} + $cdn_traffic{"$i-$j"})."\n";
			}

			$sum_cost += ($cdn_traffic{"$i-$j"} + $p2p_traffic{"$i-$j"}) * $pdistance{"$i-$j"};
			$sum_p2p += $p2p_traffic{"$i-$j"};

			$sum_cdn += $cdn_traffic{"$i-$j"};
		}
	}

	my $netcost = 0;
	my $sum_traffic = $sum_p2p + $sum_cdn;
	if ($sum_traffic > 0)
	{
		$netcost = $sum_cost / $sum_traffic;
	}

	return ($netcost, $sum_cost, $sum_p2p, $sum_cdn);
}


