#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ global gHcParms;
#pragma link C++ global gHcDetectorMap;

//#ifdef MACVERS
//#pragma link C++ global gHaVars;
//#pragma link C++ global gHaCuts;
//#pragma link C++ global gHaApps;
//#pragma link C++ global gHaScalers;
//#pragma link C++ global gHaPhysics;
//#pragma link C++ global gHaRun;
//#pragma link C++ global gHaDB;
//#pragma link C++ global gHaTextvars;
//#pragma link C++ global gHaDecoder;
//#endif

#pragma link C++ class THcInterface+;
#pragma link C++ class THcParmList+;
#pragma link C++ class THcAnalyzer+;
#pragma link C++ class THcHallCSpectrometer+;
#pragma link C++ class THcDetectorMap+;
#pragma link C++ class THcRawHit+;
#pragma link C++ class THcHitList+;
#pragma link C++ class THcSignalHit+;
#pragma link C++ class THcHodoscope+;
#pragma link C++ class THcScintillatorPlane+;
#pragma link C++ class THcRawHodoHit+;
#pragma link C++ class THcHodoHit+;
#pragma link C++ class THcDC+;
#pragma link C++ class THcDriftChamber+;
#pragma link C++ class THcDriftChamberPlane+;
#pragma link C++ class THcRawDCHit+;
#pragma link C++ class THcDCHit+;
#pragma link C++ class THcDCWire+;
#pragma link C++ class THcDCLookupTTDConv+;
#pragma link C++ class THcDCTimeToDistConv+;
#pragma link C++ class THcSpacePoint+;
#pragma link C++ class THcDCTrack+;
#pragma link C++ class THcShower+;
#pragma link C++ class THcShowerPlane+;
#pragma link C++ class THcShowerArray+;

#pragma link C++ class THcShowerHit+;
#pragma link C++ class THcRawShowerHit+;
#pragma link C++ class THcAerogel+;
#pragma link C++ class THcAerogelHit+;
#pragma link C++ class THcCherenkov+;
#pragma link C++ class THcCherenkovHit+;
#pragma link C++ class THcFormula+;
#pragma link C++ class THcRaster+;
#pragma link C++ class THcRasteredBeam+;
#pragma link C++ class THcRasterRawHit+;
#pragma link C++ class THcScalerEvtHandler+;
#pragma link C++ class THcHodoEff+;

#pragma link C++ class THcTrigApp+;
#pragma link C++ class THcTrigDet+;
#pragma link C++ class THcTrigRawHit+;

#endif
