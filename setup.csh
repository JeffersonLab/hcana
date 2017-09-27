#!/bin/csh

if ( -r "./setup.csh" ) then
  set MYDIR=`pwd`
  if ($?prompt) then
    echo "--> Setting HCANALYZER to '$MYDIR'"
  endif
else
  if ( "$1" == "" ) then
    echo "ERROR:  'setup.csh' should have a path to the hcana location as its argument."
    echo "   Often this is also the location of the setup.csh script itself:"
    echo "     source /my/path/to/setup.csh /my/path/to/setup.csh"
    echo "   Or, if you have already cd'd into the hcana directory, then you can call:"
    echo '     source setup.csh `pwd`'
    exit 1
  else
    set MYDIR=$1
  endif
endif

setenv ANALYZER $MYDIR/podd
setenv HCANALYZER $MYDIR
# Check if LD_LIBRARY_PATH is defined
if ( ! ($?LD_LIBRARY_PATH) ) then
   setenv LD_LIBRARY_PATH ""
endif
# Check if DYLD_LIBRARY_PATH is defined
if ( ! ($?DYLD_LIBRARY_PATH) ) then
   setenv DYLD_LIBRARY_PATH ""
endif
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${ANALYZER}:${HCANALYZER}"
setenv DYLD_LIBRARY_PATH "${DYLD_LIBRARY_PATH}:${ANALYZER}:${HCANALYZER}"
