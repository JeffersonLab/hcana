###### Hall C Software Main SConscript File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import re
import SCons.Util
Import ('pbaseenv')

######## ROOT Dictionaries #########

roothcdict = pbaseenv.subst('$HC_DIR')+'/HallCDict.C'
roothcobj = pbaseenv.subst('$HC_SRC')+'/HallCDict.so'
hcheaders = Split("""
	src/THcInterface.h src/THcParmList.h src/THcAnalyzer.h src/THcHallCSpectrometer.h 
	src/THcDetectorMap.h src/THcRawHit.h src/THcHitList.h src/THcSignalHit.h src/THcHodoscope.h 
	src/THcScintillatorPlane.h src/THcRawHodoHit.h src/THcHodoHit.h
        src/THcDC.h src/THcDriftChamberPlane.h 
	src/THcDriftChamber.h src/THcRawDCHit.h src/THcDCHit.h src/THcDCWire.h src/THcSpacePoint.h 
	src/THcDCLookupTTDConv.h src/THcDCTimeToDistConv.h src/THcShower.h src/THcShowerPlane.h 
        src/THcShowerArray.h src/THcShowerHit.h
	src/THcRawShowerHit.h src/THcAerogel.h src/THcAerogelHit.h src/THcCherenkov.h src/THcCherenkovHit.h
        src/THcGlobals.h src/THcDCTrack.h src/THcFormula.h
        src/THcRaster.h src/THcRasteredBeam.h src/THcRasterRawHit.h src/THcScalerEvtHandler.h
        src/THcHodoEff.h
	src/HallC_LinkDef.h
	""")
pbaseenv.RootCint(roothcdict,hcheaders)
pbaseenv.SharedObject(target = roothcobj, source = roothcdict)

#######  Start of main SConscript ###########

print ('LIBS = %s\n' % pbaseenv.subst('$LIBS'))

analyzer = pbaseenv.Program(target = 'hcana', source = 'src/main.o')
pbaseenv.Install('./bin',analyzer)
pbaseenv.Alias('install',['./bin'])
