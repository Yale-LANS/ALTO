#!/usr/bin/perl -w
#########################################################
# Simulator for Streaming AOE
#########################################################

use strict;
use warnings;
use Carp;
use Scalar::Util;
use Math::Trig;

(@ARGV >= 3) || die("Usage: $0 <P4P portal> <#locations> <#runs> [ISP]");

my $aoe = "p4p-8.cs.yale.edu";
my $p4p = shift(@ARGV);
my $nl = shift(@ARGV);
my $nr = shift(@ARGV);
my $isp = "America";
if (@ARGV > 0)
{
	$isp = shift(@ARGV);
}

# get network cost map
my %pdistance;

`rm -f pdistance`;
`wget -q http://$p4p/pdistance`;
open (PDF, "<pdistance") || die("pdistance not found");
while(<PDF>)
{
	# 1.i.America	no-reverse	10	1.i.America	0	2.i.America	15	3.i.America	10	4.i.America	21	5.i.America	6	6.i.America	5	7.i.America	30	100.e.America	70	200.e.America	80	1000.e.America	100	
	chomp;

	my @info = split(/\s+/);

	my $self = shift(@info);
	shift(@info);
	shift(@info);

	while(@info > 1)
	{	
		my $remote = shift(@info);
		my $distance = shift(@info);
		$pdistance{$self}->{$remote} = $distance;

		#print $self."\t".$remote."\t".$distance."\n";
	}
}
close(PDF);

# data structures
my @map_i_pid = ();
my %guidance_cdn = ();
my %guidance_p2p = ();
my $ni = 0;
my $generic_demand = 10;

# run multiple experiments
my @supply;
my @demand;
my @cdn;
my $rate = 48;
my $relax;

for (my $t = 0; $t < $nr; $t++)
{
	#print STDERR "Exp $t generating...\n";
	# generate swarm state
	#for (my $i = 1; $i <= $nl; $i++)
	for (my $i = 0; $i < $nl; $i++)
	{
		my $n = int(rand(30)); #error -100% +200%
		if (rand() > (18.0 / $nl))
		{
			$n = 0;
		}
		$demand[$i] = $n;

		# Avg supply * ETA = 1.2+ demand
		my $cap = int( (rand(1.7) + 0.85) * $rate * $n );
		#if (rand() < 0.1)
		#{
		#	$cap = int($cap/3);
		#	if ($cap < 1) 
		#	{
		#		$cap = 1;
		#	}
		#}
		$supply[$i] = $cap;	

		# 30% locations have CDN source
		if ($i == 1 || rand() < 0.3)
		{
			$cdn[$i] = 1;
		}
		else
		{
			$cdn[$i] = 0;
		}
	}

	my $cdn_min = aoe_mincdn(0);
	my $cdn_min_generic = aoe_mincdn(1);

	print STDERR "Exp $t simulating...\n";
	my $filefix = $t;
	open SIMF, ">sim/aoe.sim.".$filefix || die("Failed to create simulation result");
	# relaxation factors
	for($relax = 1.0; $relax >= 0.3; $relax -= 0.05)
	{
		#print STDERR "Exp $t simulating with relax $relax...\n";

		#open SF, ">sim/aoe.sim.ssm.".$filefix || die("Failed to create streaming channel state");
		#open SGF, ">sim/aoe.sim.generic.".$filefix || die("Failed to create generic streaming channel state");
		#print SF "$rate\t$relax\n";
		#print SGF "$rate\t$relax\n";
		#for (my $i = 1; $i <= $nl; $i++)
		#for (my $i = 0; $i < $nl; $i++)
		#{
		#	next if ($demand[$i] < 1);

			# 2.i.America	48	64	0	6	0	0	6
			#	print SF $i.".i.".$isp."\t".$supply[$i]."\t".$rate."\t0\t".$demand[$i]."\t0\t0\t".$cdn[$i]."\n";
			#print SGF $i.".i.".$isp."\t".(int($rate * 1.6))."\t".$rate."\t0\t1\t0\t0\t".$cdn[$i]."\n";
		#}
		#close SF;
		#close SGF;
		#`cp sim/aoe.sim.ssm.$filefix .ssm`;
		#`cp sim/aoe.sim.generic.$filefix .generic`;
		
		# dynamic optimal
		my ($cost_min, undef, undef) = aoe_mincost(0);

		# generic guidance
		my ($cost_generic, $guidance_cdn_ref, $guidance_p2p_ref) = aoe_mincost(1);
		my ($cost, $cdn_generic) = sim_generic($cdn_min_generic, $guidance_cdn_ref, $guidance_p2p_ref);  #sim_generic($cdn_min, $guidance_cdn_ref, $guidance_p2p_ref);

		print SIMF $relax."\t".$cost_min."\t".$cost."\t".($cdn_generic/$cdn_min)."\n";
	}
	close SIMF;

	#print STDERR "Exp $t outputing...\n";
	# plot results
	open GPF, ">sim/aoe.sim.".$filefix.".gp" || die("Failed to create simulation figure");
	#print GPF "set terminal postscript landscape 'Times-Roman' 18\n";
	#print GPF "set size 1.0, 0.8\n";
	print GPF "set terminal png\n";
	print GPF "set title 'Streaming AOE: Min CDN Load vs Min Network Cost ($t)'\n";
	print GPF "set xlabel 'CDN Load'\n";
	print GPF "set ylabel 'Min Network Cost under CDN Load'\n";
	#print GPF "set output 'sim/aoe.sim.$filefix.eps'\n";
	print GPF "set output 'sim/aoe.sim.$filefix.png'\n";
	print GPF "plot 'sim/aoe.sim.$filefix' using (1.0/\$1):2 t 'Dynamic Streaming Matching' with linespoints lw 3, 'sim/aoe.sim.$filefix' using (1.0/\$1):3 t 'Generic Streaming Matching' with linespoints lw 3\n";
	close GPF;
	`gnuplot sim/aoe.sim.$filefix.gp`;
}


sub aoe_mincdn
{
	my ($generic) = @_; 

	@map_i_pid = ();

	# (I) estimate min CDN
	open DF, ">.data" || die("Failed to create AOE optimization data file");
	print DF "data;\n\n";
	print DF "param : TotalPID : UPLOAD :=\n";
	my $i = 0;
	for (my $pid = 0; $pid < $nl; $pid++)
	{
		next if ($demand[$pid] == 0);

		if ($generic == 1)
		{
			print DF "$i, ".(78 * $generic_demand)."\n";
		}
		else
		{
			print DF "$i, ".$supply[$pid]."\n";
		}
		$map_i_pid[$i] = $pid;

		$i++;
	}
	$ni = $i;
	print DF ";\n\n";

	print DF "param : SOURCE :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$cdn[ $map_i_pid[$i] ]."\n";
	}
	print DF ";\n\n";

	print DF "param : CLIENTNUM :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		if ($generic == 1)
		{
			print DF "$i, ".$generic_demand."\n";
		}
		else
		{	
			print DF "$i, ".$demand[ $map_i_pid[$i] ]."\n";
		}
	}
	print DF ";\n\n";

	print DF "param RATE := 48;\n\n";
	print DF "param ETA := 0.75;\n\n";

	print DF "end;\n";
	close DF;

	`glpsol --output .mincdn --model model-mincdn --data .data`;
	my $line = `grep "obj =" .mincdn`;
	my (undef, undef, undef, $cdn_min, undef) = split(/\s+/, $line);
	#print STDERR "MINCDN ".$cdn_min."\n";

	# prepare for AOE min cost computation
	my $wl = `wc -l .data | cut -d" " -f1`;
	$wl--;
	`head -$wl .data > .data.$generic`;
	open DF, ">>.data.$generic" || die("Failed to create AOE optimization data file");
	print DF "param : PRICE :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			print DF "$i, $j, ".($pdistance{$map_i_pid[$i].".i.".$isp}->{$map_i_pid[$j].".i.".$isp})."\n";
		}
	}
	print DF ";\n\n";
	print DF "param MINCDN := $cdn_min;\n\n";
	close DF;

	return ($cdn_min);
}

sub aoe_mincost
{
	my ($generic) = @_;

	%guidance_cdn = ();
	%guidance_p2p = ();

	# (II) optimize network cost
	`cp .data.$generic .data`;
	open DF, ">>.data" || die("Failed to create AOE optimization data file");
	print DF "param ALPHA := $relax;\n\n";
	print DF "end;\n\n";
	close DF;

	`glpsol --output .mincost --model model-mincost --data .data`;
	my $line = `grep "obj =" .mincost`;
	my (undef, undef, undef, $cost_min, undef) = split(/\s+/, $line);
	#print STDERR "MINCOST ".$cost_min."\n";

	# retrieve guidance
	$line = `grep -n \"xc\\[0,0\\]\" .mincost`;
	my ($li, undef) = split(/:/, $line);
	my $ln = $ni*$ni;
	my $lhead = $li + $ln - 1;
	$line = `head -$lhead .mincost | tail -$ln`;
	my @lines_cdn = split(/\n/, $line);

	$line = `grep -n \"xp\\[0,0\\]\" .mincost`;
	($li, undef) = split(/:/, $line);
	$lhead = $li + $ln - 1;
	$line = `head -$lhead .mincost | tail -$ln`;
	my @lines_p2p = split(/\n/, $line);

	my ($cdn_ij, $p2p_ij);
	for (my $i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pidi = $map_i_pid[$i];
			my $pidj = $map_i_pid[$j];

			$line = shift(@lines_cdn);
			my (undef, undef, undef, undef, $cdn_ij, undef) = split(/\s+/, $line);
			if ($cdn_ij > 0)
			{
				$guidance_cdn{"$pidi-$pidj"} = $cdn_ij;
			}
			
			$line = shift(@lines_p2p);
			my (undef, undef, undef, undef, $p2p_ij, undef) = split(/\s+/, $line);
			if ($p2p_ij > 0)
			{
				$guidance_p2p{"$pidi-$pidj"} = $p2p_ij;
			}
		}
	}

	return ($cost_min, \%guidance_cdn, \%guidance_p2p);
}

sub sim_generic
{
	my ($cdn_min, $guidance_cdn_ref, $guidance_p2p_ref) = @_;
	my %cdn_traffic = (); # = $guidance_cdn_ref;
	my %p2p_traffic = (); #= $guidance_p2p_ref;
	
	my @p2p_load;
	my @download;
	my @cdn_load;
	my @pids;
	my @p2p_cong;
	my @deficit;

	# demand-drive adaptation
	my $j = 0;
	for($j = 0; $j < $nl; $j++) {
		$p2p_load[$j] = 0.1;
		for(my $i = 0; $i < $nl; $i++)
		{
			if (exists($guidance_p2p_ref->{"$j-$i"}))
			{
				$p2p_traffic{"$j-$i"} = $guidance_p2p_ref->{"$j-$i"} * $demand[$i] / $generic_demand;
			}
			else
			{
				$p2p_traffic{"$j-$i"} = 0;
			}

			if (exists($guidance_cdn_ref->{"$j-$i"}))
			{
				#if ($demand[$j] > $demand_generic[$j])
				#	$cdn_traffic->{"$j-$i"} = $cdn_traffic->{"$j-$i"} * $demand[$i]; 

				$cdn_traffic{"$j-$i"} = $guidance_cdn_ref->{"$j-$i"}; #FIXME: oversupply?

				if ($demand[$i] > $generic_demand)
				{
					$p2p_traffic{"$j-$i"} += $cdn_traffic{"$j-$i"} * (($demand[$i] / $generic_demand) - 1); #FIXME: oversupply?
				}
			}
			else
			{
				$cdn_traffic{"$j-$i"} = 0;
			}

			$p2p_load[$j] += $p2p_traffic{"$j-$i"};
			$cdn_load[$j] += $cdn_traffic{"$j-$i"};

			$download[$i] += ($p2p_traffic{"$j-$i"} + $cdn_traffic{"$j-$i"});
		}

		$p2p_cong[$j] = $supply[$j] * 0.75 / $p2p_load[$j];
		$deficit[$j] = 0;
		$pids[$j] = $j;
	}

	# balance p2p to avoid overload
	#TODO: balance each location's upload capacity according to guidance?
	my @sorted_pid = sort { $p2p_cong[$a] <=> $p2p_cong[$b] } @pids;
	my $upload_remain = 0;
	for($j = 0; $j < $nl; $j++)
	{
		my $pidj = $sorted_pid[$j];

		if ($p2p_cong[$pidj] < 1 )
		{
			for (my $pidi = 0; $pidi < $nl; $pidi++)
			{
				my $p2p_traffic_expected = $p2p_traffic{"$pidj-$pidi"};
				$p2p_traffic{"$pidj-$pidi"} = $p2p_traffic{"$pidj-$pidi"} * $p2p_cong[$pidj];
				$deficit[$pidi] += ($p2p_traffic_expected - $p2p_traffic{"$pidj-$pidi"});
			}
		}
		
		if ($p2p_cong[$pidj] > 1 )
		{
			$upload_remain += ($supply[$pidj] * 0.75 - $p2p_load[$pidj]);
		}
	}	

	for($j = 0; $j < $nl; $j++)
	{
		my $pidj = $sorted_pid[$j];

		if ($p2p_cong[$pidj] > 1 )
		{
			for (my $pidi = 0; $pidi < $nl; $pidi++)
			{
				next if ($download[$pidi] == 0);

				my $p2p_traffic_expected = $p2p_traffic{"$pidj-$pidi"}; 
				my $p2p_ij_max = $p2p_traffic{"$pidj-$pidi"} * $p2p_cong[$pidj];

				my $p2p_traffic_needadd = $deficit[$pidi] * ($p2p_ij_max - $p2p_traffic_expected) / $upload_remain;
				#my $p2p_traffic_needadd = $deficit[$pidi] * $p2p_traffic_expected / $download[$pidi];
				if ($p2p_ij_max > ($p2p_traffic_needadd + $p2p_traffic_expected))
				{
					$p2p_traffic{"$pidj-$pidi"} = $p2p_traffic_needadd + $p2p_traffic_expected;
				}
				else
				{
					$p2p_traffic{"$pidj-$pidi"} = $p2p_ij_max;
				}

				$deficit[$pidi] -= ($p2p_traffic{"$pidj-$pidi"} - $p2p_traffic_expected);
				$upload_remain -= ($p2p_traffic{"$pidj-$pidi"} - $p2p_traffic_expected);
			}
		}
	}

	# TODO: if P2P is insufficient, use CDN
	# use more CDN for less cost?! 
	for(my $i = 0; $i < $nl; $i++)
	{
		next if ($deficit[$i] <= 0);

		# find cheapeast CDN 
		my $j_cdn = $i;
		my $p_ji = 10000;
		for(my $j = 0; $j < $nl; $j++)
		{
			if ($cdn[$j] > 0 && $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji)
			{
				$j_cdn = $j;
				$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
			}
		}

		$cdn_traffic{"$j_cdn-$i"} += $deficit[$i];
		$deficit[$i] = 0;
	}


	# compute cost and CDN
	my $sum_cost = 0;
	my $sum_cdn = 0;
	for(my $i = 0; $i < $nl; $i++)
	{	
		#$sum_cdn += $deficit[$i];
		for(my $j = 0; $j < $nl; $j++)
		{
			my $self = "$i.i.$isp";
			my $remote = "$j.i.$isp";
			my $distance = $pdistance{$remote}->{$self};
			if (exists($p2p_traffic{"$j-$i"}))
			{
				$sum_cost += $p2p_traffic{"$j-$i"} * $distance;
			}
			if (exists($cdn_traffic{"$j-$i"}))
			{
				$sum_cost += $cdn_traffic{"$j-$i"} * $distance;
				$sum_cdn += $cdn_traffic{"$j-$i"};
			}
		}
	}

	return ($sum_cost, $sum_cdn);
}



# NOT USED
sub sim_aoe_server
{
	my $sum_cost = 0;
	my $sum_cdn = 0;
	my $sum_cost_generic = 0;
	my $sum_cdn_generic = 0;

	# Dynamic
	my @upload;
	for(my $j = 0; $j < $nl; $j++)
	{
		$upload[$j] = 0;
	}
	`./p4p_aoe_client --server=$aoe --port=6673 --op=ssm --portal=$p4p <.ssm >.aoe`;
	open(AOEF, "<.aoe") || die("AOE result not found");
	while(<AOEF>)
	{
		# 1.i.America	intrapid	0.104167	intradomain	0.104167	interdomain	0.104167	1.i.America	0.104167	2.i.America	0	3.i.America	0	4.i.America	0
		chomp;

		my @info = split(/\s+/);

		my $self = shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);

		while(@info > 1)
		{	
			my $remote = shift(@info);
			my $traffic = shift(@info);
			my $distance = $pdistance{$remote}->{$self};
			$sum_cost += $traffic * $distance;

			my ($remote_pid, undef, undef) = split(/\.+/, $remote);  
			$upload[$remote_pid] += $traffic;

			#print STDERR "TRAFFIC\t".$self."\t".$remote."\t".$traffic."\t".$distance."\n";
		}
	}
	for(my $j = 0; $j < $nl; $j++)
	{
		if ($upload[$j] > $supply[$j])
		{
			$sum_cdn += ($upload[$j] - $supply[$j]);
		}
	}
	close(AOEF);


	# Generic
	`./p4p_aoe_client --server=$aoe --port=6673 --op=ssm --portal=$p4p <.generic >.aoe.generic`;
	my %guidance;
	my @guidance_cdn;
	for (my $j = 0; $j < $nl; $j++)
	{
		$upload[$j] = 0;
	}
	#open(AOEF, "<generic/aoe.sim.generic.$isp.0") || die("AOE generic guidance not found");
	open(AOEF, "<.aoe.generic") || die("AOE generic guidance not found");
	while(<AOEF>)
	{
		# 1.i.America	intrapid	0.104167	intradomain	0.104167	interdomain	0.104167	1.i.America	0.104167	2.i.America	0	3.i.America	0	4.i.America	0
		chomp;

		my @info = split(/\s+/);

		my $self = shift(@info);
		my ($self_pid, undef, undef) = split(/\.+/, $self);  
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);
		shift(@info);

		# compute traffic based on generic guidance
		# Major problem: generic guidance does not directly apply...
		#		 compute extra CDN load instead?
		#                approximate algorithm to fetch
		#                greedy algorithm to fetch CDN?
		#                  v.s.
		#                random algorithm to fetch CDN?

		while(@info > 1)
		{
			my $remote = shift(@info);
			my $traffic_generic = shift(@info);
			my ($remote_pid, undef, undef) = split(/\.+/, $remote);  

			$guidance{"$remote_pid-$self_pid"} = $traffic_generic;
			$upload[$remote_pid] += $traffic_generic;
		}
	}
	close(AOEF);

	#for ecah demanding location, distribute its request according to generic guidance
	for(my $j = 0; $j < $nl; $j++)
	{
		$guidance_cdn[$j] = 0;

		for(my $i = 0; $i < $nl; $i++)
		{
			if (exists($guidance{"$j-$i"}) && $upload[$j] > 0)
			{
				$guidance_cdn[$j] += $guidance{"$j-$i"};

				#$guidance{"$j-$i"} = $guidance{"$j-$i"} * ($upload[$j] / 78) * $supply[$j] / 78; 
				$guidance{"$j-$i"} = $guidance{"$j-$i"} * $demand[$j]; 
			}
		}

		$guidance_cdn[$j] = ($guidance_cdn[$j] - 78) * $upload[$j] / 78; #FIXME: 78 hardcoded
	}

goto direct;

	# move extra load to elsewhere
	my @download;
	for(my $i = 0; $i < $nl; $i++)
	{
		for(my $j = 0; $j < $nl; $j++)
		{
			if (exists($guidance{"$j-$i"}))
			{
				$download[$i] += $guidance{"$j-$i"};
			}
		}

		if ($download[$i] > $demand[$i] * $rate) #TODO: eta?
		{
			for(my $j = 0; $j < $nl; $j++)
			{
				if (exists($guidance{"$j-$i"}))
				{
					$guidance{"$j-$i"} = $guidance{"$j-$i"} * ($demand[$i] * $rate) / ($download[$i] * 0.75);
				}
			}
		}
	}

	for(my $j = 0; $j < $nl; $j++)
	{
		$upload[$j] = 0;
		for(my $i = 0; $i < $nl; $i++)
		{
			if (exists($guidance{"$j-$i"}))
			{
				$upload[$j] += $guidance{"$j-$i"};
			}
		}
		#WARNING: CDN not really used
		#if ($upload[$j] > $supply[$j])
		#{
		#	$sum_cdn_generic += ($upload[$j] - $supply[$j]);
		#}
	}
	#print STDERR "CDN\t$sum_cdn\t$sum_cdn_generic\n";

	#for each demanding location, apply greedy/random algorithm to 
	#  (ii) then compensate the demand deficit
	for(my $i = 0; $i < $nl; $i++)
	{
		# (i) compensate the demand deficit
		if ($download[$i] < $demand[$i] * $rate)
		{
			my $deficit = $demand[$i] * $rate - $download[$i] * 0.75; #FIXME: eta?

			# random v.s. greedy compensate algorithm
			# TODO: how to compensate by CDN and P2P?
			if ($sum_cdn_generic < $sum_cdn)
			{
				# TODO: find cheapeast CDN

				my $j_cdn = $i;
				my $p_ji = 10000;
				for( my $j = 0; $j < $nl; $j++ )
				{
					if ($cdn[$j] > 0 && $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji)
					{
						$j_cdn = $j;
						$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
					}
				}

				if ($deficit > ($sum_cdn - $sum_cdn_generic))
				{
					$deficit -= ($sum_cdn - $sum_cdn_generic);
					$guidance{"$j_cdn-$i"} += ($sum_cdn - $sum_cdn_generic);
					$sum_cdn_generic = $sum_cdn;
				}
				else
				{
					$sum_cdn_generic += $deficit;
					$guidance{"$j_cdn-$i"} += $deficit;
					$deficit = 0;
				}
			}

			if ($deficit > 0)
			{
				# TODO: find cheapeast location where there is extra supply

				my $t = 0;
				for(my $j = $i; $deficit > 0 && $t < $nl; $j = int(rand($nl)) && $t++)
				{
					next if ($upload[$j] >= $supply[$j]);
					#print STDERR "P2P compensation: $j-$i $deficit ".$upload[$j]." ".$supply[$j]."\n";

					if ($deficit > ($supply[$j] - $upload[$j]))
					{
						$deficit -= ($supply[$j] - $upload[$j]);
						$guidance{"$j-$i"} += ($supply[$j] - $upload[$j]);
						$upload[$j] = $supply[$j];
					}
					else
					{
						$upload[$j] += $deficit;
						$guidance{"$j-$i"} += $deficit;
						$deficit = 0;
					}
				}
			}
		}
	}

direct:
	# compute network cost
	$sum_cdn_generic = 0;
	for(my $i = 0; $i < $nl; $i++)
	{	
		my $cdn_i = 0;
		for(my $j = 0; $j < $nl; $j++)
		{
			my $self = "$i.i.$isp";
			my $remote = "$j.i.$isp";
			my $distance = $pdistance{$remote}->{$self};
			if (exists($guidance{"$j-$i"}))
			{
				$sum_cost_generic += $guidance{"$j-$i"} * $distance;
			}

			if (exists($guidance{"$i-$j"}))
			{
				my $cdn_i += $guidance{"$i-$j"};
			}
		}

		if ($cdn_i > $supply[$i])
		{
			$sum_cdn_generic += ($cdn_i - $supply[$i]);
		}
	}

	if ($sum_cdn == 0)
	{
		$sum_cdn_generic = 0;
		$sum_cdn = 1;
	}
	return($sum_cost, $sum_cost_generic, ($sum_cdn_generic / $sum_cdn));
}

