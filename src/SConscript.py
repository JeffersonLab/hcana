###### Hall C Software Source SConscript Build File #####
###### Author:  Edward Brash (brash@jlab.org) June 2013

import os
import re
import SCons.Util
Import('pbaseenv')

list = Split("""
THcInterface.cxx THcParmList.cxx THcAnalyzer.cxx
THcHallCSpectrometer.cxx
THcDetectorMap.cxx
THcRawHit.cxx THcHitList.cxx
THcSignalHit.cxx
THcHodoscope.cxx THcScintillatorPlane.cxx
THcRawHodoHit.cxx THcHodoHit.cxx
THcDC.cxx THcDriftChamberPlane.cxx
THcDriftChamber.cxx
THcRawDCHit.cxx THcDCHit.cxx
THcDCWire.cxx
THcSpacePoint.cxx THcDCTrack.cxx
THcDCLookupTTDConv.cxx THcDCTimeToDistConv.cxx
THcShower.cxx THcShowerPlane.cxx THcShowerArray.cxx
THcRawShowerHit.cxx THcShowerHit.cxx
THcAerogel.cxx THcAerogelHit.cxx
THcCherenkov.cxx THcCherenkovHit.cxx
THcFormula.cxx
THcRaster.cxx THcRasteredBeam.cxx THcRasterRawHit.cxx
THcScalerEvtHandler.cxx THcConfigEvtHandler.cxx
THcTrigApp.cxx THcTrigDet.cxx THcTrigRawHit.cxx
THcRawAdcHit.cxx THcRawTdcHit.cxx
THcDummySpectrometer.cxx
THcHodoEff.cxx
""")

pbaseenv.Object('main.C')

sotarget = 'HallC'

#srclib = pbaseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],SHLIBVERSION=['$VERSION'],LIBS=[''])
srclib = pbaseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],SHLIBPREFIX='../lib',LIBS=[''])
print ('Source shared library = %s\n' % srclib)

linkbase =pbaseenv.subst('$SHLIBPREFIX')+sotarget

cleantarget = linkbase+'.so.'+pbaseenv.subst('$VERSION')
localmajorcleantarget = '../'+linkbase+'.so'

print('cleantarget = %s\n' % cleantarget)
print('localmajorcleantarget = %s\n' % localmajorcleantarget)
try:
	os.symlink(cleantarget,localmajorcleantarget)
except:
	print " Continuing ... "

Clean(srclib,cleantarget)
Clean(srclib,localmajorcleantarget)

#baseenv.Install('../',srclib)
#baseenv.Alias('install',['../'])
