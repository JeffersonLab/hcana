#!/usr/bin/perl

# Read a Hall C style MAP file and output a
# Hall A style crate map DB file.
#
# 22.03.2012 (saw)
# 11.04.2014 (saw) Perl cleanup

%crates=();

$crate = 0;
$nsubadd = 0;
$bsub = 0;
$modtype = 0;
$slot = 0;
while(<>) {
    chomp($line=$_);
    if($line=~/^\s*ROC=\s*(\d*)/i) {
	$i++;
	$crate = $1;
	if(not $crates{$crate}) {
	    $slotlist={};
	    $crates{$crate} = $slotlist;
	}
	$modtype = 0;
	$slot = 0;
    } elsif ($line=~/^\s*nsubadd=\s*(\d*)/i) {
	$nsubadd = $1;
	$modtype = 0;
    } elsif ($line=~/^\s*bsub=\s*(\d*)/i) {
	$bsub = $1;
	$modtype = 0;
    } elsif ($line=~/^\s*slot=\s*(\d*)/i) {
	$slot = $1;
	$modtype = 0;
    } elsif ($line=~/^\s*(\d*)\s*,\s*(\d*)\s*,\s*(\d*)/) {
	if($modtype == 0) {	# Slot not yet registered
	    if($nsubadd == 96) {
		$modtype = 1877;
	    } elsif($nsubadd == 64) {
		if($bsub == 16) {
		    $modtype = 1875;
		} elsif($bsub == 17) {
		    $modtype = 1881;
		}
	    }
	    if($modtype == 0) {
		print "Unknown module Crate $crate, Slot $slot\n";
	    }
	    $crates{$crate}{$slot} = $modtype;
	    # print "$crate $slot $modtype\n";
	}
    }
}
print "# Hall C Crate map\n";
foreach $crate (sort {$a <=> $b} keys %crates) {
    print "==== Crate $crate type fastbus\n";
    print "# slot  model   clear   header  mask    nchan   ndata\n";
    foreach $slot (sort {$a <=> $b} keys %{ $crates{$crate}}) {
	$modtype = $crates{$crate}{$slot};
	if($modtype == 1877) {
	    $ndata = 256;
	} else {
	    $ndata = 64;
	}
	printf " %2d     %d    1       0x0     0x0    %3d      %d\n"
	    ,$slot,$modtype,$nsubadd, $ndata;
    }
}


