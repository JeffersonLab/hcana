#!/usr/bin/wish -f
#This is a Tcl script that reads the contents of a data file.
#It reads every few seconds (defined by readinterval)
#The file is assumed to consist of two entries per line 
#separated by a colon (no blank lines!). 
#Note also that the wish path is different
#for different machines;  Usually /usr/local/bin/wish
#
#Sep 1995 Derek van Westrum  (vanwestr@colorado.edu)
#

#First check to see if filename and run# given as arguments...

if {$argc == 1} {
#    puts stderr "Wrong number of arguments."
#    puts stderr "Usage:  'runstats filename run#'"
#    puts stderr "Trying something..."
    puts stderr "Num of arguments: $argc"
    puts stderr "Value of argument: $argv"
    if {$argc == 1} {
	set file [lindex $argv 0]
	if ![file exist $file] {
	    after 2000
	}
	if [catch {open $file r} FileId] {
	    puts stderr "Cannot find file: $file"
	    puts stderr "Check file name and path."
	    puts stderr "Trying 'stats.txt'..."
	    set file "scalers/stats$file.txt"
	    if [catch {open $file r} FileId] {
		puts stderr "Nope. 'stats.txt' not there.  Exiting..."
		puts stderr "What file? $file"
		exit
	    } else {
		set FileId [open $file r]
		set fileline [split [read $FileId] \n:] 
		puts stdout "Setting Run Number to 0000..."
		set runnum "0000"
	    }
	} else {
	    set FileId [open $file r]
	    set fileline [split [read $FileId] \n:] 
	    puts stdout "Setting Run Number to 0000..."
	    set runnum "0000"
	}
    } else {
	set file "stats.txt"
	if [catch {open $file r} FileId] {
	    puts stderr "Nope. 'stats.txt' not there.  Exiting..."
	    exit
	} else {
	    set FileId [open $file r]
	    set fileline [split [read $FileId] \n:] 
	    puts stdout "Setting Run Number to 0000..."
	    set runnum "0000"
	}
    }
}
#Now set the file name as the 1st argument in the command line...
if {$argc == 2} {
    set file [lindex $argv 0]
    set runnum [lindex $argv 1]
    if [catch {open $file r} FileId] {
	puts stderr "Cannot find file: $file"
	puts stderr "Check file name and path."
	    puts stderr "Trying 'stats.txt'..."
	    set file "stats.txt"
	    if [catch {open $file r} FileId] {
		puts stderr "Nope.  'stats.txt' not there.  Exiting..."
		exit
	    } else {
		set FileId [open $file r]
		set fileline [split [read $FileId] \n:] 
		puts stdout "Setting Run Number to 0000..."
		set runnum "0000"
	    }
	} else {
	set FileId [open $file r]
	set fileline [split [read $FileId] \n:] 
    }
}

#Then set the read interval in ms
set readinterval 2000

#Now find the number of lines in the file.
#This is done by counting colons.  Blank lines
#will screw this up!
set lines [exec grep -c ":" $file]
global off
set off 0 
#Now setup the main window
wm title . "Stats:Run#$runnum"
wm minsize . 50 4

#Setup the button bar
#The read button will run the process Read.  See below
frame .buttons -borderwidth 4 -relief ridge 
pack .buttons -side top -fill x
button .buttons.quit -text Quit -command exit
button .buttons.read -text Read -command Read
button .buttons.faster -text {Analyze Faster} -command Fast
pack .buttons.read -side left
pack .buttons.quit -side right
pack .buttons.faster -side right

#Now setup two fields for each line in the file
#The variable names will be raised on the left
#The values will be sunken on the right

frame .field 
pack .field -side bottom -expand true -fill both -padx 10 -pady 10

frame .field.var
pack .field.var -side left -fill y -expand true

frame .field.val
pack .field.val -side right -fill y -expand true

for {set i 1} {$i <= $lines } {incr i 1} {
    set temp [expr 2*$i -2]
    set mylabl [lindex $fileline $temp]
    set mylabl [string trimright $mylabl]
    if {[string length $mylabl]==0} {
      label .field.var.var$i -width 13 -anchor e -padx 2
      label .field.val.val$i -width 8 
    } else {
      label .field.var.var$i -width 15 -anchor e -padx 2
      label .field.val.val$i -relief sunken -width 8 -bg DarkGoldenrod3
      .field.var.var$i config -text $mylabl: -padx 10 -anchor e
    }
    pack .field.var.var$i -side top -expand true
    pack .field.val.val$i -side top -expand true
}

#The process Read will go through "fileline" picking out the
# values which are in the "even" positions.  Read will execute
#when it's called by the loop below, or when the read button
#is pressed.

proc Read { } {
    global lines file fileline off
    set FileId [open $file r]
    set fileline [split [read $FileId] \n:] 
    for {set i 1} {$i <=$lines } {incr i 1} {
	set temp [expr 2*$i -1]
	.field.val.val$i config -text [lindex $fileline $temp] \
		-padx 10 -anchor w
	if {[expr 2*$i-1] == 7} {
	    set x [lindex $fileline $temp]
	    set y [lindex $x 0]
	    set z [expr $y+$off]
	    set a [concat $z Hz]
	    .field.val.val$i config -text $a \
 	      -padx 10 -anchor w
	}
}

    #Now change the color of he background quickly to 
    #indicate that the file is being read.
#    for {set i 1} {$i <=$lines } {incr i 1} {
#	.field.val.val$i config -bg Grey
#    }
#    after 2 {
#	for {set i 1} {$i <=$lines } {incr i 1} {
#	    .field.val.val$i config -bg LightGrey
#	}
#    }
    close $FileId
}

proc Fast { } {
    global off
    incr off 100
    }

#Now define a recursive (general) process that runs
#a procedure every "interval" ms
proc loopdeloop { process interval } {
    $process
    after $interval [list loopdeloop $process $interval]
}

#Now keep reading...
loopdeloop Read $readinterval


close $FileId
