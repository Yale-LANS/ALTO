#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find qw(find);
use String::Similarity;
use File::Temp qw( :POSIX);
use File::Copy;
use File::Basename;

sub prepareLicenseCPP {
	my ($license) = @_;
	my $buf = "/*\n";
	foreach my $line (split(/\n/, $license)) { $buf .= " * $line\n"; } 
	$buf .= " */\n";
	return $buf;
}

sub prepareLicenseCMake {
	my ($license) = @_;
	my $buf = "";
	foreach my $line (split(/\n/, $license)) { $buf .= "# $line\n"; } 
	return $buf;
}

my %PROCESSORS = (
	'.*\.cpp'		=> \&prepareLicenseCPP,
	'.*\.h'			=> \&prepareLicenseCPP,
	'.*\.java'		=> \&prepareLicenseCPP,
	'.*\.pl'		=> \&prepareLicenseCMake,
	'CMakeLists\.txt'	=> \&prepareLicenseCMake,
	);

my @BLACKLIST = (
	'add_license\.pl',
	'boost_random_device\.cpp',
	'p4p-common/cpp/src/include/p4p/patricia\.h',
	'p4p-common/cpp/src/lib/patricia\.c',
	'xbtt',
	);

my $MIN_SIMILARITY = 0.80;

sub readFile {
	my ($file) = @_;

	open(F, "<", $file)  || die("Failed to open file '$file': $!");

	my $buf = "";

	# Read contents of file into buffer
	my $rc;
	while ($rc = read(F, $buf, 128, length($buf))) {}
	defined($rc) || die("Failed to read from file '$file': $!");

	close(F);

	return $buf;
}

sub prependLicense {
	my ($file, $license) = @_;

	# Ignore files in the blacklist
	if (grep { $file =~ m/$_/ } @BLACKLIST) {
		print "SKIP: $file blacklisted\n";
		return;
	}

	# Open the existing file
	open(F, "<", $file)
		|| die("Failed to open file '$file': $!");

	my $shebang = "";
	my $file_contents = "";
	my $max_sim = 0.0;

	while (my $line = <F>) {
		if ($line =~ m/^#!/) {
			$shebang = $line;
			next;
		}

		# Add to our string keeping track of file contents
		$file_contents .= $line;

		# Handle similarity matching if we're still searching
		# for a license.
		if (defined($max_sim)) {

			# Ignore this check if we have nowhere near enough data
			next if (length($file_contents) < 0.90 * length($license));

			my $sim = similarity($license, $file_contents);
			if ($sim == 1) {
				# Found the match
				print "SKIP: $file already contains desired license\n";
				close(F);
				return;
			}

			# If we're still increasing similarity, keep going
			next if ($sim >= $max_sim);

			# We've stopped increasing in similarity. If we're "similar enough"
			# then it appears that a license was already present. Don't touch
			# the file in this case.
			if ($max_sim > $MIN_SIMILARITY) {
				print "WARN: $file contains slightly-different license (similarity = $max_sim)\n";
				close(F);
				return;
			}

			# If we've got to this point, we know there is no license
			# already in the file.
			$max_sim = undef;
		}
	}

	close(F);

	# Write new file containing the license
	open(F, ">", $file)
		|| die("Failed to open file '$file': $!");
	print F $shebang;
	print F "$license\n\n";
	print F $file_contents;
	close(F);

	print "ADD : $file prepended with license\n";
}

sub process {
	my ($file, $license) = @_;

	# Find first pattern 
	foreach my $pattern (keys(%PROCESSORS)) {
		next unless (basename($file) =~ m/^$pattern$/);
		prependLicense($file, $PROCESSORS{$pattern}($license));
		last;
	}
}

# Determine the license we'll be adding to files
my $LICENSE_FILE = shift(@ARGV);
defined($LICENSE_FILE)
	|| die("Must specify license file as first argument");
my $LICENSE = readFile($LICENSE_FILE);

foreach my $arg (@ARGV) {
	sub wanted {
		process($File::Find::name, $LICENSE) if (-f $File::Find::name);
	}
	find( { wanted => \&wanted, no_chdir => 1 }, $arg);
}


