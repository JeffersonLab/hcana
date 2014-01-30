#!/bin/csh

set called=($_)
if ("$called" != "") then
  set scriptdir=$called[2]
  echo $scriptdir
#  set MYDIR=`dirname $scriptdir`
#  set MYDIR=`cd $MYDIR && pwd`    # ensure absolute path

  set MYDIR=`pwd`    # ensure absolute path

#  echo $MYDIR
#  echo "Are we here"
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

echo $ANALYZER
echo $HCANALYZER
