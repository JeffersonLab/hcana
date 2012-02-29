#!/bin/bash

export ANALYZER=/home/saw/ROOT/analyzer-1.5.22
export HCANALYZER=/home/saw/ROOT/HALLC
# Check if defined
if [ ! ${LD_LIBRARY_PATH} ]; then
  export LD_LIBRARY_PATH=""
fi
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYZER:$HCANALYZER
