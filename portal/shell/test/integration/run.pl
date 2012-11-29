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



use strict;
use warnings;

use File::Basename;
use File::Path;
use POSIX;

my $TESTROOT = dirname($0);
my $ADM_PORT = 16672;

@ARGV == 3
	|| die("Usage: <Test Dir> <Portal Server EXE> <Portal Shell EXE>");

my $TEST = shift(@ARGV);
my $SERVER_EXE = shift(@ARGV);
my $SHELL_EXE = shift(@ARGV);

my $input = "$TESTROOT/$TEST/input.txt";
my $output = "$TESTROOT/$TEST/output.txt";

my $actOutput = tmpnam();
my $errOutput = tmpnam();

print "Configuration:\n";
print "\tServer EXE      : $SERVER_EXE\n";
print "\tShell EXE       : $SHELL_EXE\n";
print "\tTest            : $TEST\n";
print "\tInput           : $input\n";
print "\tExpected Output : $output\n";
print "\tActual Output   : $actOutput\n";
print "\tError Output    : $errOutput\n";
print "\n";

# Start the server in the background
print "Starting server in background\n";
my $serverPid = fork();
$serverPid >= 0
	|| die("Failed to fork server");
if ($serverPid == 0) {
	exec($SERVER_EXE, "--log-level=4", "--config-file=$TESTROOT/p4p-portal.conf", "--interfaces-file=$TESTROOT/p4p-portal-intf.conf")
		|| die("Failed to execute server: $!");
}

# Let the server start up
sleep(1);

# Run the test
print "Running shell\n";
system("$SHELL_EXE localhost:$ADM_PORT \"$input\" > \"$actOutput\" 2> \"$errOutput\"");

# Kill the server and wait for it to complete
print "Sending termination signal to server\n";
kill(15, $serverPid); # 15 = SIGTERM
print "Waiting for server to exit\n";
waitpid($serverPid, 0);

print "\n";

# Print Error Output
print "ERRORS\n";
print "*******\n";
system("cat \"$errOutput\"");
print "*******\n";

# Diff against the expected output
print "DIFF\n";
print "\tLEFT : $output\n";
print "\tRIGHT: $actOutput\n";
print "*******\n";
my $diffRC = system("diff -u \"$output\" \"$actOutput\"");
print "*******\n";
$diffRC == 0
	|| warn("diff command failed with return code: $diffRC");

# Cleanup
unlink($actOutput)
	|| warn("Failed to cleanup temp file $actOutput: $!");

print "DIFF RETURNED: $diffRC\n";

exit($diffRC != 0 ? 1 : 0);

