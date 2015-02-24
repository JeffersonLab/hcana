#!/bin/bash

MYDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export ANALYZER=${MYDIR}/podd
export HCANALYZER=${MYDIR}
# Check if defined
if [ ! ${LD_LIBRARY_PATH} ]; then
  export LD_LIBRARY_PATH=""
fi
if [ ! ${DYLD_LIBRARY_PATH} ]; then
  export DYLD_LIBRARY_PATH=""
fi
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYZER:$HCANALYZER
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ANALYZER:$HCANALYZER
