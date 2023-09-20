###### Hall C Software Source SConscript Build File #####
###### Author:  Edward Brash (brash@jlab.org) June 2013

import os
# import re
# import SCons.Util
from podd_util import build_library
Import('pbaseenv')

srclist = Glob('*.cxx', exclude=['main.cxx','HallCDict.cxx'])
src = ''.join(str(x)+' ' for x in srclist)

sotarget = 'HallC'
compiledata = 'hc_compiledata.h'
extrahdrs = [compiledata]

srclib = build_library(pbaseenv, sotarget, src, extrahdrs,
                       extradicthdrs=['THcGlobals.h'], useenv=False,
                       versioned=True, install_srcdir='hcana')

Clean(srclib, compiledata)
