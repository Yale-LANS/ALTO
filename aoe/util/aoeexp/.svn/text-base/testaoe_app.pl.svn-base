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

#`rm -f pdistance`;
#`wget -q http://$p4p/pdistance`;
open (PDF, "<pdistance.$p4p") || die("pdistance not found");
while(<PDF>)
{
	# 1.i.America	no-reverse	10	1.i.America	0	2.i.America	15	3.i.America	10	4.i.America	21	5.i.America	6	6.i.America	5	7.i.America	30	100.e.America	70	200.e.America	80	1000.e.America	100	
	chomp;
	my ($self, $remote, $distance) = split(/\s+/, $_);
	$pdistance{$self}->{$remote} = $distance;

	#my @info = split(/\s+/);
	#my $self = shift(@info);
	#shift(@info);
	#shift(@info);
	#while(@info > 1)
	#{	
	#	my $remote = shift(@info);
	#	my $distance = shift(@info);
	#	$pdistance{$self}->{$remote} = $distance;
		#print $self."\t".$remote."\t".$distance."\n";
	#}
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
my @cdncost;
my @freebw;
my $rate = 5000 / 10; #assuming peak load VV is 25k, video rate 200k => 5000 Mbps
my $lambda = 0.5;
my $relax;

for (my $t = 0; $t < $nr; $t++)
{
	#print STDERR "Exp $t generating...\n";

	# generate swarm state
	#for (my $i = 1; $i <= $nl; $i++)
	my $nnz = 0;
	my $sum_demand = 0;
	for (my $i = 0; $i < $nl; $i++)
	{
		#my $n = int(rand(30)); #error -100% +200%
		my $n = int(rand(10) + 5); #error 0 +200%
		if (rand() > (11.0 / $nl) || $nnz > 6)
		{
			$n = 0;
		}
		else
		{
			$nnz++;
		}
		$demand[$i] = $n;
		$sum_demand += ($n * $rate);
		$cdncost[$i] = 2 * 1050 + rand(100) - 50; #rand($demand[$i] / 10) + 1;

		# Avg supply * ETA = 1.2+ demand
		#FIXME: p2p capacity is limited
		my $cap = int( (rand(1.8) + 0.9) * $rate * $n );
		if (rand() < 0.1)
		{
			$cap = int($cap/3);
			if ($cap < 1) 
			{
				$cap = 1;
			}
		}
		$supply[$i] = $cap;	
		if ($demand[$i] == 0)
		{
			$supply[$i] = 0;
		}

		# 30% locations have CDN source
		if (($i == 1 || rand() < 0.4) && $demand[$i] > 0) #FIXME: allow CDN without demand?
		{
			$cdn[$i] = 1;
		}
		else
		{
			$cdn[$i] = 0;
		}

		$freebw[$i] = 0;
	}

	# App minimize CDN cost
	#my ($cost_app) = app_mincdn();
	my ($cost_app, $cdn_min) = aoe_mincdn(0);
	my $cost_app_0 = $cost_app;
	my $cdn_min_0 = $cdn_min;
	#my $cdn_min = aoe_mincdn(0);
	#my $cdn_min_generic = aoe_mincdn(1);
	#print STDERR "DEBUG app's cost $cost_app\n";

	# ISP wanted min cost 
	my ($cost_min_min, $traffic_min_min) = aoe_mincost(0, 0);
	#my ($cost_min_min) = isp_mincost();
	my $cost_isp = $cost_min_min;
	#print STDERR "DEBUG isp min cost $cost_isp\n";
	sim_aoe();

	# App random as the initial state
	my ($cost_random, $traffic_random) = sim_random_cdn();

	# App optimize given ISP's free bandwidth control
	unlink("sim/app.sim.$t");
	open SIMF, ">sim/app.sim.$t" || die("Failed to create simulation result");
	print SIMF "0\t".($cost_min_min/$sum_demand)."\t".($cost_app/$sum_demand)."\t".($cost_app/$sum_demand)."\t".($cost_random/$sum_demand)."\t0\n";

	my ($app_cost, $isp_bw);
	my $gamma = $sum_demand / 500;
	#for ($gamma = $sum_demand / 500; $gamma < $sum_demand / 10; $gamma += $sum_demand / 500)
	#for ($gamma = 0.1; $gamma < 2; $gamma += 0.1)
	{
		if (0) 
		{
			# simulation using Lindo, only works for pure CDN case 

			open DF, ">sim/app.sim.$t.dat" || die("Failed to create simulation data file");
			print DF "$gamma\n$cost_min_min\n$cost_app\n\n";
			#FIXME: supply and demand can mismatch
			my ($i, $j);
			for ($i = 0; $i < $nl; $i++)
			{
				next if ($demand[$i] < 1);
				print DF $supply[$i]." ";
			}
			print DF "\n";
			for ($i = 0; $i < $nl; $i++)
			{
				next if ($demand[$i] < 1);
				print DF ($demand[$i] * $rate)." ";
			}
			print DF "\n\n";

			for ($i = 0; $i < $nl; $i++)
			{
				next if ($demand[$i] < 1);
				print DF $cdncost[$i]." ";
			}
			print DF "\n\n";

			for ($i = 0; $i < $nl; $i++)
			{
				for ($j = 0; $j < $nl; $j++)
				{
					next if ($demand[$i] < 1 || $demand[$j] < 1);
					print DF ($pdistance{"$i.i.$isp"}->{"$j.i.$isp"})." ";
				}
				next if ($demand[$i] < 1);
				print DF "\n";
			}
			print DF "\n\n";

			for ($i = 0; $i < $nl; $i++)
			{
				for ($j = 0; $j < $nl; $j++)
				{
					next if ($demand[$i] < 1 || $demand[$j] < 1);
					print DF $traffic_random->{"$i-$j"}." ";
				}
				next if ($demand[$i] < 1);
				print DF "\n";
			}
			print DF "\n\n";

			close DF;

			`./appopt $nnz "sim/app.sim.$t.dat" > .result.$gamma`;

			my $app_cost_result = `grep "cost=" .result.$gamma`;
			chomp($app_cost_result);
			(undef, $app_cost) = split(/=+/, $app_cost_result);
			my $isp_bw_result = `grep "freebw=" .result.$gamma`;
			chomp($isp_bw_result);
			(undef, $isp_bw) = split(/=+/, $isp_bw_result);
		}
		else
		{
			# App iteration based optimization

			$cdn_min = $cdn_min_0;
			$cost_app = $cost_app_0; #FIXME? 
			my $cost_app_previous = $cost_app_0 * 2;
			#print STDERR "DEBUG f^0=$cost_app; g^0=$cdn_min\n";

			my $b = 0;
			my $b_previous = 0;
			for (my $try = 0; $try < 5; $try++)
			{
				# ISP compute freebw
				if ($cost_app - $cost_isp < 0.001) 
				{
					$b = $gamma;	
				}
				else
				{
					$b = $gamma / (1.0 + ($cost_app - $cost_isp) / ($cost_app_0 - $cost_isp));
				}
				#print STDERR "DEBUG gamma=$gamma b=$b f=$cost_app f^*=$cost_isp f^0=$cost_app_0\n";
				for (my $i = 0; $i < $nl; $i++)
				{
					next if ($demand[$i] < 1);

					$freebw[$i] = $b / $nnz; #FIXME? unequal beta_i?
				}

				# App decides rho
				next if ($cost_app == $cost_app_previous);

				my $rho = -(($b - $b_previous) / ($cost_app - $cost_app_previous)) * $sum_demand;
				if ($rho < 0.0001)
				{
					#FIXME
					$rho = 0.0001;
				}
				#print STDERR "DEBUG rho=$rho\n";

				$b_previous = $b;
				$cost_app_previous = $cost_app;

				($cost_app, $cdn_min) = aoe_mincdncost($cdn_min, $rho);
				#print STDERR "DEBUG f=$cost_app with b=$b, g=$cdn_min\n";
			}

			$app_cost = $cost_app;
			$isp_bw = $b;

			sim_aoe();
		}

		#print SIMF ($gamma/$sum_demand)."\t".($cost_min_min/$sum_demand)."\t".($app_cost/$sum_demand)."\t".($cost_app/$sum_demand)."\t".($cost_random/$sum_demand)."\t".($isp_bw * 1050/$sum_demand)."\n";
		print SIMF $gamma."\t".($cost_min_min/$sum_demand)."\t".($app_cost/$sum_demand)."\t".($cost_app_0/$sum_demand)."\t".($cost_random/$sum_demand)."\t".($isp_bw * 1050/$sum_demand)."\n"; #"\t".($cost_app/$sum_demand)."\n";

		#TODO: network cost details: localization, congestion avoid, etc.
	}
	close SIMF;

	#$relax = 0.01;
	#print STDERR "Exp $t simulating...\n";
	#my $filefix = $t;
	#open SIMF, ">sim/aoe.sim.".$filefix || die("Failed to create simulation result");
	# relaxation factors
	#for($relax = 1.0; $relax >= 0.3; $relax -= 0.05)
	#{
		# dynamic optimal
	#	my ($cost_min, $traffic_min) = aoe_mincost(0, 0);

		# comparison: dynamic worst
	#	my ($cost_max, $traffic_max) = aoe_mincost(0, 1);

		# comparison: random
	#	my ($cost_random, $traffic_random, $cdn_random) = sim_random();

		# generic guidance + dynamic adaptation
	#	my ($cost_generic, undef) = aoe_mincost(1, 0);
	#	my ($cost, $traffic, $cdn_generic) = sim_generic($cdn_min_generic, $guidance_cdn_ref, $guidance_p2p_ref);  #sim_generic($cdn_min, $guidance_cdn_ref, $guidance_p2p_ref);

	#	next if ($cdn_min < 1);

	#	print SIMF $relax."\t".($cost_min/$traffic_min)."\t".($cost/$traffic)."\t".($cdn_generic/$cdn_min)."\t".($cost_max/$traffic_max)."\t".($cost_random/$traffic_random)."\t".($cdn_random/$cdn_min)."\t".($cost_min_min/$traffic_min_min)."\n";
		#print SIMF $relax."\t".$cost_min."\t".$cost."\t".($cdn_generic/$cdn_min)."\t".$cost_max."\t".$cost_random."\t".($cdn_random/$cdn_min)."\t".$cost_min_min."\n";
	#}
	#close SIMF;

	# plot results
	open GPF, ">sim/app.sim.$t.gp" || die("Failed to create simulation figure");
	#print GPF "set terminal postscript landscape 'Times-Roman' 18\n";
	#print GPF "set size 1.0, 0.8\n";
	#print GPF "set size 1.0, 1.5\n";
	print GPF "set terminal png\n";
	#print GPF "set output 'sim/app.sim.$t.eps'\n";
	print GPF "set output 'sim/app.sim.$t.png'\n";
	print GPF "set title 'App Network Cost vs Gamma (Case $t)'\n";
	print GPF "set xlabel 'BW Compensation Gamma (Mbps)'\n";
	print GPF "set ylabel 'Per-bit Network Cost'\n";
	print GPF "set yrange [0:1000]\n";
	#print GPF "set logscale x\n";
	#print GPF "plot 'sim/app.sim.$t' using 1:2 t 'Optimal for ISP' with linespoints lw 2, 'sim/app.sim.$t' using 1:3 t 'App w/ ISP bw' with linespoints lw 2, 'sim/app.sim.$t' using 1:4 t 'App w/o ISP bw' with linespoints lw 2, 'sim/app.sim.$t' using 1:5 t 'App w/o optimization' with linespoints lw 2\n";
	print GPF "plot 'sim/app.sim.$t' using 1:(\$1*1050/$sum_demand+\$2) t 'Optimal for ISP' with linespoints lw 2, 'sim/app.sim.$t' using 1:(\$6+\$3) t 'App w/ ISP bw' with linespoints lw 2, 'sim/app.sim.$t' using 1:4 t 'App w/o ISP bw' with linespoints lw 2, 'sim/app.sim.$t' using 1:5 t 'App w/o optimization' with linespoints lw 2\n"; #, 'sim/app.sim.$t' using 1:7 t 'App iteration' with linespoints lw 2\n";
	#TODO: ISP's real optimization on f(x)+\alpha\beta/\beta\betaS^I(x)
	#TODO: plot worst case cost: app does not optimize anything, but still gets 1/(1+phi) free bw
	#print GPF "plot 'sim/aoe.sim.$filefix' using (1.0/\$1):2 t 'Dynamic Optimized Streaming' with linespoints lw 3, 'sim/aoe.sim.$filefix' using (1.0/\$1):3 t 'Generic Optimization with Adaptation' with linespoints lw 3, 'sim/aoe.sim.$filefix' using (1.0/\$1):6 t 'Non-optimized Streaming' with linespoints lw 3\n"; #, 'sim/aoe.sim.$filefix' using (1.0/\$1):5 t 'Network Cost Upper Bound' with linespoints lw 3\n";
	close GPF;

	`gnuplot sim/app.sim.$t.gp`;
}
unlink(".result.*");


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

	print DF "param : BETA :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$cdncost[ $map_i_pid[$i] ]."\n";
	}
	print DF ";\n\n";

	print DF "param : b :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$freebw[ $map_i_pid[$i] ]."\n";
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

	print DF "param RATE := $rate;\n\n";
	print DF "param ETA := 0.75;\n\n";

	print DF "end;\n";
	close DF;

	`glpsol --output .mincdn --model model-mincdn --data .data`;
	my $line = `grep "obj =" .mincdn`;
	my (undef, undef, undef, $cdn_min, undef) = split(/\s+/, $line);

	$line = `grep -n \"x\\[0,0\\]\" .mincdn`;
	my ($li, undef) = split(/:/, $line);
	my $ln = $ni*$ni;
	my $lhead = $li + $ln - 1;
	$line = `head -$lhead .mincdn | tail -$ln`;
	my @lines_cdn = split(/\n/, $line);
	
	my $cost_app = 0;
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
				#$guidance_cdn{"$pidi-$pidj"} = $cdn_ij;
				$cost_app += $cdn_ij * $pdistance{"$pidi.i.$isp"}->{"$pidj.i.$isp"};
			}
		}
	}

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
			my $pdist = $pdistance{$map_i_pid[$i].".i.".$isp}->{$map_i_pid[$j].".i.".$isp};
			#print DF "$i, $j, $pdist\n";
			print DF "$i, $j, $pdist\n";
		}
	}
	print DF ";\n\n";
	print DF "param MINCDN := $cdn_min;\n\n";
	print DF "end;\n\n";
	close DF;

	return ($cost_app, $cdn_min);
}

sub aoe_mincost
{
	my ($generic, $max) = @_;

	# AOE min cost computation
	`cp .data.$generic .data`;
	#open DF, ">>.data" || die("Failed to create AOE optimization data file");
	#print DF "param ALPHA := $relax;\n\n";
	#print DF "end;\n\n";
	#close DF;

	if ($max == 1)
	{
		`glpsol --output .mincost --model model-maxcost --data .data`;
	}
	else
	{
		`glpsol --output .mincost --model model-mincost --data .data`;
	}
	my $line = `grep "obj =" .mincost`;
	my (undef, undef, undef, $cost_min, undef) = split(/\s+/, $line);
	#print STDERR "MINCOST ".$cost_min."\n";

	# retrieve guidance
	%guidance_cdn = ();
	%guidance_p2p = ();

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

	my $traffic = 0;
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
				$traffic += $cdn_ij;
			}
			
			$line = shift(@lines_p2p);
			my (undef, undef, undef, undef, $p2p_ij, undef) = split(/\s+/, $line);
			if ($p2p_ij > 0)
			{
				$guidance_p2p{"$pidi-$pidj"} = $p2p_ij;
				$traffic += $p2p_ij;
			}
		}
	}

	return ($cost_min, $traffic);
}

sub app_mincdn
{
	# estimate app min CDN cost

	# require @map_i_pid built

	open DF, ">.data" || die("Failed to create AOE optimization data file");
	print DF "data;\n\n";
	print DF "param : TotalPID : SUPPLY :=\n";
	my $i = 0;
	for ($i = 0; $i < $ni; $i++)
	{
		#if ($generic == 1)
		#{
		#	print DF "$i, ".(78 * $generic_demand)."\n";
		#}
		#else
		#{
			print DF "$i, ".$supply[ $map_i_pid[$i] ]."\n";
		#}
	}
	print DF ";\n\n";

	print DF "param : DEMAND :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		#if ($generic == 1)
		#{
		#	print DF "$i, ".$generic_demand."\n";
		#}
		#else
		#{	
			print DF "$i, ".$demand[ $map_i_pid[$i] ]."\n";
		#}
	}
	print DF ";\n\n";

	print DF "param : BETA :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$cdncost[ $map_i_pid[$i] ]."\n";
	}
	print DF ";\n\n";
	
	print DF "param RATE := $rate;\n\n";

	print DF "end;\n";
	close DF;

	`glpsol --output .cdnmincdn --model model-cdnmincdn --data .data`;
	my $line = `grep "obj =" .cdnmincdn`;
	my (undef, undef, undef, $cdn_app, undef) = split(/\s+/, $line);
	#print STDERR "MINCDN ".$cdn_app."\n";

	# retrieve guidance
	#%guidance_cdn = ();

	$line = `grep -n \"x\\[0,0\\]\" .cdnmincdn`;
	my ($li, undef) = split(/:/, $line);
	my $ln = $ni*$ni;
	my $lhead = $li + $ln - 1;
	$line = `head -$lhead .cdnmincdn | tail -$ln`;
	my @lines_cdn = split(/\n/, $line);
	
	my $cost_app = 0;
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
				#$guidance_cdn{"$pidi-$pidj"} = $cdn_ij;
				$cost_app += $cdn_ij * $pdistance{"$pidi.i.$isp"}->{"$pidj.i.$isp"};
			}
		}
	}

	return ($cost_app, $cdn_app);
}

sub isp_mincost
{
	# estimate ISP min cost
	@map_i_pid = ();

	open DF, ">.data" || die("Failed to create AOE optimization data file");
	print DF "data;\n\n";
	print DF "param : TotalPID : SUPPLY :=\n";
	my $i = 0;
	for (my $pid = 0; $pid < $nl; $pid++)
	{
		next if ($demand[$pid] == 0);

		#if ($generic == 1)
		#{
		#	print DF "$i, ".(78 * $generic_demand)."\n";
		#}
		#else
		#{
			print DF "$i, ".$supply[$pid]."\n";
		#}
		$map_i_pid[$i] = $pid;

		$i++;
	}
	$ni = $i;
	print DF ";\n\n";

	print DF "param : DEMAND :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		#if ($generic == 1)
		#{
		#	print DF "$i, ".$generic_demand."\n";
		#}
		#else
		#{	
			print DF "$i, ".$demand[ $map_i_pid[$i] ]."\n";
		#}
	}
	print DF ";\n\n";

	print DF "param : PRICE :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pdist = $pdistance{$map_i_pid[$i].".i.".$isp}->{$map_i_pid[$j].".i.".$isp};
			print DF "$i, $j, $pdist\n";
		}
	}
	print DF ";\n\n";
	
	print DF "param RATE := $rate;\n\n";

	print DF "end;\n";
	close DF;

	`glpsol --output .cdnmincost --model model-cdnmincost --data .data`;
	my $line = `grep "obj =" .cdnmincost`;
	my (undef, undef, undef, $cost_min, undef) = split(/\s+/, $line);
	#print STDERR "MINCDN ".$cost_min."\n";

	return ($cost_min);
}

sub aoe_mincdncost
{
	my ($mincdn_previous, $rho) = @_; 

	@map_i_pid = ();

	open DF, ">.data" || die("Failed to create AOE optimization data file");
	print DF "data;\n\n";
	print DF "param : TotalPID : UPLOAD :=\n";
	my $i = 0;
	for (my $pid = 0; $pid < $nl; $pid++)
	{
		next if ($demand[$pid] == 0);

		#if ($generic == 1)
		#{
		#	print DF "$i, ".(78 * $generic_demand)."\n";
		#}
		#else
		#{
			print DF "$i, ".$supply[$pid]."\n";
		#}
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

	print DF "param : BETA :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$cdncost[ $map_i_pid[$i] ]."\n";
	}
	print DF ";\n\n";

	print DF "param : b :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		print DF "$i, ".$freebw[ $map_i_pid[$i] ]."\n";
	}
	print DF ";\n\n";

	print DF "param : CLIENTNUM :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		#if ($generic == 1)
		#{
		#	print DF "$i, ".$generic_demand."\n";
		#}
		#else
		#{	
			print DF "$i, ".$demand[ $map_i_pid[$i] ]."\n";
		#}
	}
	print DF ";\n\n";

	print DF "param : PRICE :=\n";
	for ($i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pdist = $pdistance{$map_i_pid[$i].".i.".$isp}->{$map_i_pid[$j].".i.".$isp};
			#print DF "$i, $j, $pdist\n";
			print DF "$i, $j, $pdist\n";
		}
	}
	print DF ";\n\n";
	
	print DF "param RATE := $rate;\n\n";
	print DF "param MINCDN := $mincdn_previous;\n\n";
	print DF "param ETA := 0.75;\n\n";
	print DF "param RHO := $rho;\n\n";

	print DF "end;\n";
	close DF;

	# retrieve guidance
	%guidance_cdn = ();
	%guidance_p2p = ();

	`glpsol --output .mincdncost --model model-mincdncost --data .data`;
	my $line = `grep -n \"xc\\[0,0\\]\" .mincdncost`;
	my ($li, undef) = split(/:/, $line);
	my $ln = $ni*$ni;
	my $lhead = $li + $ln - 1;
	$line = `head -$lhead .mincdncost | tail -$ln`;
	my @lines_cdn = split(/\n/, $line);
	
	my $cost_app = 0;
	for (my $i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pidi = $map_i_pid[$i];
			my $pidj = $map_i_pid[$j];

			$line = shift(@lines_cdn);
			my (undef, undef, undef, undef, $x_ij, undef) = split(/\s+/, $line);
			$guidance_cdn{"$pidi-$pidj"} = $x_ij;
			$cost_app += $x_ij * $pdistance{"$pidi.i.$isp"}->{"$pidj.i.$isp"};
		}
	}

	$line = `grep -n \"xp\\[0,0\\]\" .mincdncost`;
	($li, undef) = split(/:/, $line);
	$ln = $ni*$ni;
	$lhead = $li + $ln - 1;
	$line = `head -$lhead .mincdncost | tail -$ln`;
	my @lines_p2p = split(/\n/, $line);
	
	for (my $i = 0; $i < $ni; $i++)
	{
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pidi = $map_i_pid[$i];
			my $pidj = $map_i_pid[$j];

			$line = shift(@lines_p2p);
			my (undef, undef, undef, undef, $x_ij, undef) = split(/\s+/, $line);
			$guidance_p2p{"$pidi-$pidj"} = $x_ij;
			$cost_app += $x_ij * $pdistance{"$pidi.i.$isp"}->{"$pidj.i.$isp"};
		}
	}

	# retrieve app cost
	$line = `grep -n \"s\\[0\\]\" .mincdncost`;
	($li, undef) = split(/:/, $line);
	$ln = $ni*$ni;
	$lhead = $li + $ln - 1;
	$line = `head -$lhead .mincdncost | tail -$ln`;
	@lines_cdn = split(/\n/, $line);
	
	my $cdn_app = 0;
	for (my $i = 0; $i < $ni; $i++)
	{
		my $pidi = $map_i_pid[$i];

		$line = shift(@lines_cdn);
		my (undef, undef, undef, undef, $s_i, undef) = split(/\s+/, $line);
		$cdn_app += $s_i * $cdncost[$pidi];
	}

	return ($cost_app, $cdn_app);
}

sub aoe_inverseprop
{
	#TODO
}

sub sim_aoe
{
	my ($p2p_guidance_ref) = @_;

	# require @map_i_pid built
	open DF, ">.guidance" || die("Failed to create simulation data file\n");
	for (my $i = 0; $i < $ni; $i++)
	{
		my $pidi = $map_i_pid[$i];
		for (my $j = 0; $j < $ni; $j++)
		{
			my $pidj = $map_i_pid[$j];
			my $p2pij = 0;
			if (exists($guidance_p2p{"$pidi-$pidj"}))
			{
				$p2pij = $guidance_p2p{"$pidi-$pidj"};
			}
			my $cdnij = 0;
			if (exists($guidance_cdn{"$pidi-$pidj"}))
			{
				$cdnij = $guidance_cdn{"$pidi-$pidj"};
			}
			
			print DF "$i\t$j\t".($pdistance{"$pidi.i.$isp"}->{"$pidj.i.$isp"})."\t$p2pij\t$cdnij\n";
		}
	}
	close DF;

	open DF, ">.distribution" || die("Failed to create simulation data file\n");
	for (my $i = 0; $i < $ni; $i++)
	{
		my $pidi = $map_i_pid[$i];
		print DF "$i\t".($supply[$pidi])."\t".($demand[$pidi])."\n";
	}
	close DF;

	print STDERR "perl app_sim.pl 3600 1 $ni .distribution .guidance\n";
	#``;
}

sub sim_generic
{
	my ($cdn_min, $guidance_cdn_ref, $guidance_p2p_ref) = @_;
	my %cdn_traffic = (); #= $guidance_cdn_ref;
	my %p2p_traffic = (); #= $guidance_p2p_ref;
	
	my @p2p_load;
	#my @download;
	my @cdn_load;
	#my @pids;
	my @p2p_cong;
	my @deficit;

	# demand-drive adaptation: following generic guidance
	for(my $j = 0; $j < $nl; $j++) 
	{
		$p2p_load[$j] = 0;
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

			#$download[$i] += ($p2p_traffic{"$j-$i"} + $cdn_traffic{"$j-$i"});
		}

		if ($supply[$j] < 1)
		{
			$p2p_cong[$j] = 1;
		}
		else
		{
			$p2p_cong[$j] = $p2p_load[$j] / ($supply[$j] * 0.75);
		}
		#$pids[$j] = $j;
		$deficit[$j] = 0;
	}

	# re-balance p2p to avoid overload
	my $overload_pids = 0;
	my $deficit_pids = $nl;
	my $try = 0;
	#my @sorted_pid;
	while($overload_pids < $nl and $deficit_pids > 0 and $try < 200) 
	{
		#@sorted_pid = ();
		#@sorted_pid = sort { $p2p_cong[$b] <=> $p2p_cong[$a] } @pids;

		# count overload and deficit, re-balance
		$overload_pids = 0;
		for(my $j = 0; $j < $nl; $j++)
		{
			my $pidj = $j; #$sorted_pid[$j];

			if ($p2p_cong[$pidj] > 1)
			{
				#print STDERR "OVERLOAD $pidj: ".$p2p_cong[$pidj]."\t".$p2p_load[$pidj]."\t".$supply[$pidj]."\t".$demand[$pidj]."\t".$cdn[$pidj]."\n";
				$overload_pids++;

				for (my $pidi = 0; $pidi < $nl; $pidi++)
				{
					my $p2p_traffic_expected = $p2p_traffic{"$pidj-$pidi"};
					next if ($p2p_traffic_expected < 1);

					$p2p_traffic{"$pidj-$pidi"} = $p2p_traffic_expected / $p2p_cong[$pidj];
					$deficit[$pidi] += ($p2p_traffic_expected - $p2p_traffic{"$pidj-$pidi"});
				}

				$p2p_load[$pidj] = $supply[$pidj] * 0.75;
			}
		 	elsif ($p2p_cong[$pidj] > 0.9)
			{
				$overload_pids++;
			}
		}	

		# put overload on underloaded locations
		$deficit_pids = 0;
		for (my $pidi = 0; $pidi < $nl; $pidi++)
		{
			next if ($deficit[$pidi] < 1);
			$deficit_pids++;

			# sum P2P supply
			my $p2p_supply = 0.01;
			for (my $j = 0; $j < $nl; $j++)
			{
				my $pidj = $j; #$sorted_pid[$j];
				
				if ($p2p_cong[$pidj] < 1)
				{
					$p2p_supply += $p2p_traffic{"$pidj-$pidi"};
				}
			}
			next if ($p2p_supply <= 0);

			# redistribute request for deficit
			my $deficit_reduce = 0;
			for (my $j = 0; $j < $nl; $j++)
			{
				next if ($p2p_cong[$j] >= 1);

				my $pidj = $j; #$sorted_pid[$j];

				my $p2p_traffic_expected = $p2p_traffic{"$pidj-$pidi"}; 
				if (!exists($guidance_p2p_ref->{"$pidj-$pidi"})) 
				{# not guided, use hueristic
					my $small_step = 1;
					if ($pdistance{"$pidj.i.$isp"}->{"$pidi.i.$isp"} == 0)
					{# should rarely happen
						print STDERR "  $pidj.i.$isp -> $pidi.i.$isp: 0\n";
						$small_step = $supply[$pidj] - $p2p_load[$pidj];
					}
					else 
					{
						$small_step = $demand[$pidi] * $rate / 12 / $pdistance{"$pidj.i.$isp"}->{"$pidi.i.$isp"};
					}

					if ($small_step > ($deficit[$pidi] - $deficit_reduce))
					{
						$small_step = $deficit[$pidi] - $deficit_reduce;
					}
					if ($small_step > ($supply[$pidj] - $p2p_load[$pidj]))
					{
						$small_step = $supply[$pidj] - $p2p_load[$pidj];
					}

					$p2p_traffic{"$pidj-$pidi"} += $small_step;
				}
				else
				{# guided
					my $big_step = $deficit[$pidi] * $p2p_traffic_expected / $p2p_supply;
					if ($big_step > ($deficit[$pidi] - $deficit_reduce))
					{
						$big_step = $deficit[$pidi] - $deficit_reduce;
					}

					$p2p_traffic{"$pidj-$pidi"} += $big_step;
				}

				$p2p_load[$pidj] += ($p2p_traffic{"$pidj-$pidi"} - $p2p_traffic_expected);
				$deficit_reduce += ($p2p_traffic{"$pidj-$pidi"} - $p2p_traffic_expected);
			}

			$deficit[$pidi] -= $deficit_reduce;
		}

		# recalculate congestion
		for (my $j = 0; $j < $nl; $j++)
		{
			if ($supply[$j] < 1)
			{
				$p2p_cong[$j] = 1;
			}
			else
			{
				$p2p_cong[$j] = $p2p_load[$j] / ($supply[$j] * 0.75);
			}
		}
		
		$try++;
	}

	# use P2P or CDN for deficit
	#my $upload_remain = 1;
	for(my $i = 0; $i < $nl; $i++)
	{
		#while(0 && $deficit[$i] > 1) # && $upload_remain > 0) 
		#{
		#	$upload_remain = 0;

		#	# first use P2P inversely proportional to network cost
		#	my $j_p2p = $i;
		#	my $j_cdn = $i;
		#	my $p_ji = 10000;
		#	for(my $j = 0; $j < $nl; $j++)
		#	{
		#		if ($p2p_load[$j] < $supply[$j] * 0.75) #&& $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji)
		#		{
		#			$j_p2p = $j;
		#			$p_ji = $p"distance{"$j.i.$isp"}->{"$i.i.$isp"};
		#			if ($p_ji < 1)
		#			{
		#				$p_ji = 1;
		#			}
		#			$upload_remain = 1;

		#			my $j_p2p_add = $supply[$j_p2p] * 0.75 - $p2p_load[$j_p2p];
		#			my $j_cdn_add = 0;
		#			if ($j_p2p_add > $deficit[$i] / $p_ji)
		#			{
		#				$j_p2p_add = $deficit[$i] / $p_ji;
		#			}
		#			elsif ($cdn[$j] > 0) 
		#			{
		#				$j_cdn_add = ($deficit[$i] / $p_ji - $j_p2p_add);
		#				$cdn_traffic{"$j_cdn-$i"} += $j_cdn_add;
		#			}

		#			$p2p_traffic{"$j_p2p-$i"} += $j_p2p_add + $j_cdn_add;
		#			$p2p_load[$j_p2p] += $j_p2p_add + $j_cdn_add;
		#			$deficit[$i] -= $j_p2p_add + $j_cdn_add;
		#		}
		#		elsif ($cdn[$j] > 0) #&& $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji)
		#		{
		#			$j_cdn = $j;
		#			$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
		#		}
		#	}

		#	if ($upload_remain == 1)
		#	{	
		#		my $j_p2p_add = $supply[$j_p2p] * 0.75 - $p2p_load[$j_p2p];
		#		if ($j_p2p_add > $deficit[$i])
		#		{
		#			$j_p2p_add = $deficit[$i];
		#		}
		#		$p2p_traffic{"$j_p2p-$i"} += $j_p2p_add;
		#		$p2p_load[$j_p2p] += $j_p2p_add;
		#		$deficit[$i] -= $j_p2p_add;
		#	}
		#	else
		#	{
		#		$cdn_traffic{"$j_cdn-$i"} += $deficit[$i];
		#		$deficit[$i] = 0;
		#	}
		#}

		next if ($deficit[$i] <= 0);

		# find cheapest CDN
		my $j_cdn = $i;
		my $p_ji = 10000;
		for(my $j = 0; $j < $nl; $j++)
		{
			if ($cdn[$j] > 0 && $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji) #means $supply[$j] > 0
			{
				$j_cdn = $j;
				$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
			}
		}
		$cdn_traffic{"$j_cdn-$i"} += $deficit[$i];
		$deficit[$i] = 0;
	}

	# compute cost and CDN
	my ($cost, $traffic, $cdn) = sim_result(\%p2p_traffic, \%cdn_traffic);

	return ($cost, $traffic, $cdn);
}

sub sim_random
{
	my %cdn_traffic = ();
	my %p2p_traffic = ();

	my @p2p_load = ();
	my @p2p_cong = ();
	my @deficit = ();
	my $deficit_pids = 0;
	my $underload_pids = 0;
	for(my $i = 0; $i < $nl; $i++)
	{
		$deficit[$i] = $demand[$i] * $rate;
		if ($deficit[$i] > 0)
		{
			$deficit_pids++;
		}

		if ($supply[$i] > 0)
		{
			$underload_pids++;
			$p2p_cong[$i] = 0;
		}
		else
		{
			$p2p_cong[$i] = 1;
		}
		$p2p_load[$i] = 0;
	}

	# find CDN source - cheapest
	for(my $i = 0; $i < $nl; $i++)
	{
		my $j_cdn = $i;
		my $p_ji = 10000;
		for(my $j = 0; $j < $nl; $j++)
		{
			$cdn_traffic{"$j-$i"} = 0;
			$p2p_traffic{"$j-$i"} = 0;
			if ($cdn[$j] > 0 && $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji)
			{
				$j_cdn = $j;
				$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
			}
		}

		$cdn_traffic{"$j_cdn-$i"} += $rate;
		$deficit[$i] -= $rate;
	}

	# find P2P supply
	my $try = 0;
	while($deficit_pids > 0 && $underload_pids > 0 && $try < 200)
	{
		# compensate deficit by random p2p
		$deficit_pids = 0;
		for(my $i = 0; $i < $nl; $i++) 
		{
			next if ($deficit[$i] <= 0);
			$deficit_pids++;

			# randomly use P2P
			my $perj = $deficit[$i] / $underload_pids;
			foreach(my $j = 0; $j < $nl; $j++)
			{
				next if ($p2p_cong[$j] >= 1);

				$p2p_traffic{"$j-$i"} += $perj;
				$p2p_load[$j] += $perj;
				$deficit[$i] -= $perj;
			}

			#$deficit[$i] = 0;
		}

		# re-balance
		$underload_pids = 0;
		for(my $j = 0; $j < $nl; $j++)
		{
			if ($supply[$j] < 1)
			{
				$p2p_cong[$j] = 1;
			}
			else
			{
				$p2p_cong[$j] = $p2p_load[$j] / ($supply[$j] * 0.75);
			}

			if ($p2p_cong[$j] > 1)
			{
				#print STDERR "OVERLOAD $pidj: ".$p2p_cong[$pidj]."\t".$p2p_load[$pidj]."\t".$supply[$pidj]."\t".$demand[$pidj]."\t".$cdn[$pidj]."\n";
				#$overload_pids++;

				for (my $i = 0; $i < $nl; $i++)
				{
					my $p2p_traffic_expected = $p2p_traffic{"$j-$i"};
					next if ($p2p_traffic_expected < 1);

					$p2p_traffic{"$j-$i"} = $p2p_traffic{"$j-$i"} / $p2p_cong[$j];
					$deficit[$i] += ($p2p_traffic_expected - $p2p_traffic{"$j-$i"});
				}

				$p2p_load[$j] = $supply[$j] * 0.75;
				$p2p_cong[$j] = 1;
			}
			#elsif ($p2p_cong[$j] > 0.9)
			#{
			#	$overload_pids++;
			#}
			
			if ($p2p_cong[$j] < 1)
			{
				$underload_pids++;
			}
		}	

		$try++;
	}

	# Use CDN for final supply deficit
	for(my $i = 0; $i < $nl; $i++)
	{
		next if ($deficit[$i] <= 0);

		my $j_cdn = $i;
		my $p_ji = 10000;
		for(my $j = 0; $j < $nl; $j++)
		{
			if ($cdn[$j] > 0 && $pdistance{"$j.i.$isp"}->{"$i.i.$isp"} < $p_ji) #means $supply[$j] > 0
			{
				$j_cdn = $j;
				$p_ji = $pdistance{"$j.i.$isp"}->{"$i.i.$isp"};
			}
		}
		$cdn_traffic{"$j_cdn-$i"} += $deficit[$i];
		$deficit[$i] = 0;
	}

	# compute cost and CDN
	my ($cost, $traffic, $cdn) = sim_result(\%p2p_traffic, \%cdn_traffic);

	return ($cost, $traffic, $cdn);
}

sub sim_random_cdn
{
	my %traffic = ();
	my %no_traffic = ();

	my @load = ();
	my @cong = ();
	my @deficit = ();
	my $deficit_pids = 0;
	my $underload_pids = 0;
	for(my $i = 0; $i < $nl; $i++)
	{
		$deficit[$i] = $demand[$i] * $rate;
		if ($deficit[$i] > 0)
		{
			$deficit_pids++;
		}

		if ($supply[$i] > 0)
		{
			$underload_pids++;
			$cong[$i] = 0;
		}
		else
		{
			$cong[$i] = 1;
		}
		$load[$i] = 0;
	}

	# find supply
	my $try = 0;
	while($deficit_pids > 0 && $underload_pids > 0 && $try < 200)
	{
		# compensate deficit by random p2p
		$deficit_pids = 0;
		for(my $i = 0; $i < $nl; $i++) 
		{
			next if ($deficit[$i] <= 0);
			$deficit_pids++;

			# randomly download
			my $perj = $deficit[$i] / $underload_pids;
			foreach(my $j = 0; $j < $nl; $j++)
			{
				next if ($cong[$j] >= 1);

				$traffic{"$j-$i"} += $perj;
				$load[$j] += $perj;
				$deficit[$i] -= $perj;
			}

			#$deficit[$i] = 0;
		}

		# re-balance
		$underload_pids = 0;
		for(my $j = 0; $j < $nl; $j++)
		{
			if ($supply[$j] < 1)
			{
				$cong[$j] = 1;
			}
			else
			{
				$cong[$j] = $load[$j] / ($supply[$j] * 0.75);
			}

			if ($cong[$j] > 1)
			{
				#print STDERR "OVERLOAD $pidj: ".$p2p_cong[$pidj]."\t".$p2p_load[$pidj]."\t".$supply[$pidj]."\t".$demand[$pidj]."\t".$cdn[$pidj]."\n";
				#$overload_pids++;

				for (my $i = 0; $i < $nl; $i++)
				{
					if (!exists($traffic{"$j-$i"}))
					{
						$traffic{"$j-$i"} = 0;
					}
					my $traffic_expected = $traffic{"$j-$i"};
					next if ($traffic_expected < 1);

					$traffic{"$j-$i"} = $traffic{"$j-$i"} / $cong[$j];
					$deficit[$i] += ($traffic_expected - $traffic{"$j-$i"});
				}

				$load[$j] = $supply[$j] * 0.75;
				$cong[$j] = 1;
			}
			
			if ($cong[$j] < 1)
			{
				$underload_pids++;
			}
		}	

		$try++;
	}

	# compute cost and CDN
	my ($cost, undef, undef) = sim_result(\%traffic, \%no_traffic);

	return ($cost, \%traffic);
}

sub sim_result
{
	my ($p2p_traffic_ref, $cdn_traffic_ref) = @_;

	my $sum_cost = 0;
	my $sum_cdn = 0;
	my $sum_traffic = 0;
	for(my $i = 0; $i < $nl; $i++)
	{	
		#$sum_cdn += $deficit[$i];
		for(my $j = 0; $j < $nl; $j++)
		{
			my $distance = $pdistance{"$i.i.$isp"}->{"$j.i.$isp"};
			if (exists($p2p_traffic_ref->{"$j-$i"}))
			{
				$sum_cost += $p2p_traffic_ref->{"$j-$i"} * $distance;
				$sum_traffic += $p2p_traffic_ref->{"$j-$i"};
			}
			if (exists($cdn_traffic_ref->{"$j-$i"}))
			{
				$sum_cost += $cdn_traffic_ref->{"$j-$i"} * $distance;
				$sum_cdn += $cdn_traffic_ref->{"$j-$i"};
				$sum_traffic += $cdn_traffic_ref->{"$j-$i"};
			}
		}
	}

	return ($sum_cost, $sum_traffic, $sum_cdn);
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

		if ($download[$i] > $demand[$i] * $rate) # eta?
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
			# how to compensate by CDN and P2P?
			if ($sum_cdn_generic < $sum_cdn)
			{
				# find cheapeast CDN

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
				# find cheapeast location where there is extra supply

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

