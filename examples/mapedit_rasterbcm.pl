#!/usr/bin/perl

# Rewrite a Hall C style MAP file so that the Raster and BPM
# are separate detector types instead of being part of the
# MISC detector
#
# 08.04.2014 (saw) Modify just raster entries for now

use POSIX qw(strftime);

$RASTERID=18;
$BCMID=19;
$SWAPRASTERXY=1;

$thedate = strftime( '%b %e, %Y', localtime );

$ifile = $ARGV[0];
$ofile = $ARGV[1];

open(IFILE,"<$ifile");
open(OFILE,">$ofile");

print OFILE "! $ofile  Automatically generated $thedate\n";
print OFILE "! from $ifile by $0\n";

# Insert comments defining the raster and BCM detector IDS
$ininiddefs=0;
while(<IFILE>) {
    chomp;
    $line = $_;
    if($line=~/_ID/) {
	$iniddefs=1;
    } elsif ($iniddefs) {
	if(not $line=~/_ID/ and not $line=~/:/) {
	    print OFILE "! RASTER_ID=$RASTERID       ADC\n";
	    print OFILE "! BCM_ID=$RASTERID          ADC\n";
	    print OFILE "$line\n";
	    last;
	}
    }
    print OFILE "$line\n";
}

# Copy the rest of the file, looking for the specific channels to
# edit
$detector=0;
while(<IFILE>) {
    chomp;
    $line = $_;

    if($line=~/^\s*DETECTOR=\s*(\d*)/i) {
	$detector=$1;
	print "$detector\n";
	print OFILE "$line\n";
    } elsif ($line=~/^\s*(\d*)\s*,\s*(\d*)\s*,\s*(\d*),\s*(\d*)\s*(.*)/) {
	$comment = $5;
	$channel = $1;
	$plane = $2;
	$element = $3;
	$signal = $4;
	if($comment=~"Fast Raster") {
	    $signal = -1;
	    if($comment=~"X-sync") {
		$signal = 0;
	    } elsif ($comment=~"X-signal") {
		$signal = 1;
	    } elsif ($comment=~"Y-sync") {
		$signal = 2;
	    } elsif ($comment=~"Y-signal") {
		$signal = 3;
	    }
	    if($signal>=0) {
		if ($SWAPRASTERXY) {
		    $signal = ($signal+2) % 4;
		}
		print OFILE "detector=$RASTERID ! RASTER\n";
		print OFILE "!$line\n";
		print OFILE "$channel, 1, 1, $signal $comment\n";
		print OFILE "detector=$detector ! RASTER\n";
	    } else {
	    print OFILE "$line\n";
	    }
	} else {
	    print OFILE "$line\n";
	}
    } else {
	print OFILE "$line\n";
    }
}
exit;

%crates=();

$crate = 0;
$nsubadd = 0;
$bsub = 0;
$modtype = 0;
$slot = 0;
while(<>) {
    $line=chomp;
    if($line=/^\s*ROC=\s*(\d*)/i) {
	$i++;
	$crate = $1;
	if(not $crates{$crate}) {
	    $slotlist={};
	    $crates{$crate} = $slotlist;
	}
	$modtype = 0;
	$slot = 0;
    } elsif ($line=/^\s*nsubadd=\s*(\d*)/i) {
	$nsubadd = $1;
	$modtype = 0;
    } elsif ($line=/^\s*bsub=\s*(\d*)/i) {
	$bsub = $1;
	$modtype = 0;
    } elsif ($line=/^\s*slot=\s*(\d*)/i) {
	$slot = $1;
	$modtype = 0;
    } elsif ($line=/^\s*(\d*)\s*,\s*(\d*)\s*,\s*(\d*)/) {
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


