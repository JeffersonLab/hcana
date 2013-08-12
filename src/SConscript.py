###### Hall C Software Source SConscript Build File #####
###### Author:  Edward Brash (brash@jlab.org) June 2013

import os
import re
import SCons.Util
Import('baseenv')

list = Split("""
THcInterface.cxx THcParmList.cxx THcAnalyzer.cxx \
THcHallCSpectrometer.cxx \
THcDetectorMap.cxx \
THcRawHit.cxx THcHitList.cxx \
THcSignalHit.cxx \
THcHodoscope.cxx THcScintillatorPlane.cxx \
THcHodoscopeHit.cxx \
THcDC.cxx THcDriftChamberPlane.cxx \
THcDriftChamber.cxx \
THcRawDCHit.cxx THcDCHit.cxx \
THcDCWire.cxx \
THcSpacePoint.cxx \
THcDCLookupTTDConv.cxx THcDCTimeToDistConv.cxx \
THcShower.cxx THcShowerPlane.cxx \
THcShowerHit.cxx \
THcAerogel.cxx THcAerogelHit.cxx
""")

baseenv.Object('main.C')

sotarget = 'HallC'

#srclib = baseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],SHLIBVERSION=['$VERSION'],LIBS=[''])
srclib = baseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],LIBS=[''])
print ('Source shared library = %s\n' % srclib)
baseenv.Install('../',srclib)
baseenv.Alias('install',['../'])

