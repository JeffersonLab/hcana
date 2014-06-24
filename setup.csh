#!/bin/csh

set called=($_)
if ("$called" != "") then
  set scriptdir=$called[2]
  set MYDIR=`dirname $scriptdir`
  set MYDIR=`c\d $MYDIR && pwd`    # ensure absolute path
else
  set scriptdir=$1
  set MYDIR=$scriptdir
endif
setenv ANALYZER $MYDIR/podd
setenv HCANALYZER $MYDIR
# Check if LD_LIBRARY_PATH is defined
if ( ! ($?LD_LIBRARY_PATH) ) then
   setenv LD_LIBRARY_PATH ""
endif
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${ANALYZER}:${HCANALYZER}"
