###### Hall C Software Main SConscript File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import re
import SCons.Util
Import ('baseenv')

######## ROOT Dictionaries #########

roothcdict = baseenv.subst('$HC_DIR')+'/HallCDict.C'
roothcobj = baseenv.subst('$HC_SRC')+'/HallCDict.so'
hcheaders = Split("""
	src/THcInterface.h src/THcParmList.h src/THcAnalyzer.h src/THcHallCSpectrometer.h 
	src/THcDetectorMap.h src/THcRawHit.h src/THcHitList.h src/THcSignalHit.h src/THcHodoscope.h 
	src/THcScintillatorPlane.h src/THcHodoscopeHit.h src/THcDC.h src/THcDriftChamberPlane.h 
	src/THcDriftChamber.h src/THcRawDCHit.h src/THcDCHit.h src/THcDCWire.h src/THcSpacePoint.h 
	src/THcDCLookupTTDConv.h src/THcDCTimeToDistConv.h src/THcShower.h src/THcShowerPlane.h 
	src/THcRawShowerHit.h src/THcAerogel.h src/THcAerogelHit.h src/THcGlobals.h src/THcDCTrack.h
	src/HallC_LinkDef.h
	""")
baseenv.RootCint(roothcdict,hcheaders)
baseenv.SharedObject(target = roothcobj, source = roothcdict)

baseenv.Append(CPPDEFINES= '-DHALLC_MODS')

#######  Start of main SConscript ###########

analyzer = baseenv.Program(target = 'hcana', source = 'src/main.o')
baseenv.Install('./bin',analyzer)
baseenv.Alias('install',['./bin'])
