#!/usr/bin/perl 
# Written by Anthony D. Joseph (adj@eecs.berkeley.edu)
# August 25, 2002


require "getopts.pl";
&Getopts("u:n:");  # u for <user login>, n <file> for node file,

($#ARGV == 0) || &usage;
$Command = $ARGV[0];

#
# Initialize
#
$NodeCount = 0;
if ($opt_u) {
    $Login = "$opt_u@";
} else {
    $Login = "";
}

if ($opt_n) { # Read node list
    $NodeCmd = "cat";
    $NodeCmdOpt = "";
    $NodeListHost = "";
    $ListCmd = $opt_n;
} else {
    $NodeCmd = "ssh";
    $NodeCmdOpt = "-q";
    $NodeListHost = "bogle.ext.postel.org";
    $ListCmd = "cat /etc/ROOTSTOCK/tmp/data/nodes.txt";
}

print "Node Command: $NodeCmd $NodeCmdOpt\n";
print "Login: $Login\n";
print "Node List Host: $NodeListHost\n";
print "List Command: $ListCmd\n";
print "Remote Command: $Command\n";

sub usage {
    die("Usage: plcmd.pl [-u <userid>] [-n <nlist>] cmd ");
}

sub getNodesList {
    # Get the list of nodes
    if ($opt_t || $opt_n) {
	open (NODES, '<', "$ListCmd") 
	    or die "Can't open node list, $ListCmd: $!";
    } else {
	open (NODES, '-|', 
	      "$NodeCmd $NodeCmdOpt $Login" . "$NodeListHost $ListCmd") 
	    or die "Can't execute node list fetch cmd, $NodeCmd $ListCmd: $!";
    }
    while (<NODES>) {              # note use of indirection
        $curline = $_;
        chop($curline);				# Discard \n

	if ($curline =~ /Authentication successful./) { # Skip first line
	    next;
	} elsif (length($curline) == 0) { # Skip blank
	    next;
	} elsif ($curline =~ /warning:/) { # Error
	    die "Error retrieving node list: $curline";
	} 
	$Nodes[$NodeCount++] = $curline;
    }
    close NODES;
    die "No nodes found" if ($NodeCount == 0);
}

&getNodesList();
print "Found $NodeCount nodes:\n";
for $i (0 .. $NodeCount - 1) {
    print "$Nodes[$i]\n";
}

$NodeCmd = "ssh";

for $i (0 .. $NodeCount - 1) {
    $RemoteCmd = $Login . $Nodes[$i] . ":" . $RemoteDir;

    @args = ($NodeCmd,  $Login . $Nodes[$i], $Command);

    print "$NodeCmd $NodeCmdOpt $Login" . "$Nodes[$i] $Command\n";
    system(@args);
    $exit_value  = $? >> 8;
    if ($exit_value != 0) {
	$signal_num  = $? & 127;
	$dumped_core = $? & 128;
	print "ERROR for $Nodes[$i]: $exit_value, signal: $signal_num, core: $dumped_core\n";
    }
}





