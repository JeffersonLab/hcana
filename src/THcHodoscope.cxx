///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
// Class for a generic hodoscope consisting of multiple                      //
// planes with multiple paddles with phototubes on both ends.                //
// This differs from Hall A scintillator class in that it is the whole       //
// hodoscope array, not just one plane.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoscope.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcHodoscope::THcHodoscope( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor

  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
}

//_____________________________________________________________________________
THcHodoscope::THcHodoscope( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoscope::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  if( THaNonTrackingDetector::Init( date ) )
    return fStatus;

  // Replace with what we need for Hall C
  //  const DataDest tmp[NDEST] = {
  //    { &fRTNhit, &fRANhit, fRT, fRT_c, fRA, fRA_p, fRA_c, fROff, fRPed, fRGain },
  //    { &fLTNhit, &fLANhit, fLT, fLT_c, fLA, fLA_p, fLA_c, fLOff, fLPed, fLGain }
  //  };
  //  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  THcHitList::InitHitList(fDetMap, "THcHodoscopeHit", 100);

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HSCIN") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HSCIN");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcHodoscope::ReadDatabase( const TDatime& date )
{
  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  //  DBRequest list[] = {
  //    { "TDC_offsetsL", fLOff, kDouble, fNelem },
  //    { "TDC_offsetsR", fROff, kDouble, fNelem },
  //    { "ADC_pedsL", fLPed, kDouble, fNelem },
  //    { "ADC_pedsR", fRPed, kDouble, fNelem },
  //    { "ADC_coefL", fLGain, kDouble, fNelem },
  //    { "ADC_coefR", fRGain, kDouble, fNelem },
  //    { "TDC_res",   &fTdc2T },
  //    { "TransSpd",  &fCn },
  //    { "AdcMIP",    &fAdcMIP },
  //    { "NTWalk",    &fNTWalkPar, kInt },
  //    { "Timewalk",  fTWalkPar, kDouble, 2*fNelem },
  //    { "ReTimeOff", fTrigOff, kDouble, fNelem },
  //    { "AvgRes",    &fResolution },
  //    { "Atten",     &fAttenuation },
  //    { 0 }
  //  };

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  fNPlanes = 4;			// Hardwire for now

  fNPaddle = new Int_t [4];

  fNPaddle[0] = *(Int_t *)gHcParms->Find("hscin_1x_nr")->GetValuePointer();
  fNPaddle[1] = *(Int_t *)gHcParms->Find("hscin_1y_nr")->GetValuePointer();
  fNPaddle[2] = *(Int_t *)gHcParms->Find("hscin_2x_nr")->GetValuePointer();
  fNPaddle[3] = *(Int_t *)gHcParms->Find("hscin_2y_nr")->GetValuePointer();

  fSpacing = new Double_t [4];
  fSpacing[0] = gHcParms->Find("hscin_1x_spacing")->GetValue(0);
  fSpacing[1] = gHcParms->Find("hscin_1y_spacing")->GetValue(0);
  fSpacing[2] = gHcParms->Find("hscin_2x_spacing")->GetValue(0);
  fSpacing[3] = gHcParms->Find("hscin_2y_spacing")->GetValue(0);

  fCenter = new Double_t* [4];
  Double_t* p;
  Int_t iplane;

  iplane = 0;
  p = (Double_t *)gHcParms->Find("hscin_1x_center")->GetValuePointer();
  fCenter[iplane] = new Double_t [fNPaddle[iplane]];
  // Print out some parameters just to demonstrate that it works
  cout << iplane;
  for(Int_t i=0;i<fNPaddle[iplane];i++) {
    fCenter[iplane][i] = p[i];
    cout << " " << fCenter[iplane][i];
  }
  cout << endl;

  iplane = 1;
  p = (Double_t *)gHcParms->Find("hscin_1y_center")->GetValuePointer();
  fCenter[iplane] = new Double_t [fNPaddle[iplane]];
  cout << iplane;
  for(Int_t i=0;i<fNPaddle[iplane];i++) {
    fCenter[iplane][i] = p[i];
    cout << " " << fCenter[iplane][i];
  }
  cout << endl;

  iplane = 2;
  p = (Double_t *)gHcParms->Find("hscin_2x_center")->GetValuePointer();
  fCenter[iplane] = new Double_t [fNPaddle[iplane]];
  cout << iplane;
  for(Int_t i=0;i<fNPaddle[iplane];i++) {
    fCenter[iplane][i] = p[i];
    cout << " " << fCenter[iplane][i];
  }
  cout << endl;

  iplane = 3;
  p = (Double_t *)gHcParms->Find("hscin_2y_center")->GetValuePointer();
  fCenter[iplane] = new Double_t [fNPaddle[iplane]];
  cout << iplane;
  for(Int_t i=0;i<fNPaddle[iplane];i++) {
    fCenter[iplane][i] = p[i];
    cout << " " << fCenter[iplane][i];
  }
  cout << endl;

  fIsInit = true;

  return kOK;
}

//_____________________________________________________________________________
Int_t THcHodoscope::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  //  RVarDef vars[] = {
  //    { "nlthit", "Number of Left paddles TDC times",  "fLTNhit" },
  //    { "nrthit", "Number of Right paddles TDC times", "fRTNhit" },
  //    { "nlahit", "Number of Left paddles ADCs amps",  "fLANhit" },
  //    { "nrahit", "Number of Right paddles ADCs amps", "fRANhit" },
  //    { "lt",     "TDC values left side",              "fLT" },
  //    { "lt_c",   "Corrected times left side",         "fLT_c" },
  //    { "rt",     "TDC values right side",             "fRT" },
  //    { "rt_c",   "Corrected times right side",        "fRT_c" },
  //    { "la",     "ADC values left side",              "fLA" },
  //    { "la_p",   "Corrected ADC values left side",    "fLA_p" },
  //    { "la_c",   "Corrected ADC values left side",    "fLA_c" },
  //    { "ra",     "ADC values right side",             "fRA" },
  //    { "ra_p",   "Corrected ADC values right side",   "fRA_p" },
  //    { "ra_c",   "Corrected ADC values right side",   "fRA_c" },
  //    { "nthit",  "Number of paddles with l&r TDCs",   "fNhit" },
  //    { "t_pads", "Paddles with l&r coincidence TDCs", "fHitPad" },
  //    { "y_t",    "y-position from timing (m)",        "fYt" },
  //    { "y_adc",  "y-position from amplitudes (m)",    "fYa" },
  //    { "time",   "Time of hit at plane (s)",          "fTime" },
  //    { "dtime",  "Est. uncertainty of time (s)",      "fdTime" },
  //    { "dedx",   "dEdX-like deposited in paddle",     "fAmpl" },
  //    { "troff",  "Trigger offset for paddles",        "fTrigOff"},
  //    { "trn",    "Number of tracks for hits",         "GetNTracks()" },
  //    { "trx",    "x-position of track in det plane",  "fTrackProj.THaTrackProj.fX" },
  //    { "try",    "y-position of track in det plane",  "fTrackProj.THaTrackProj.fY" },
  //    { "trpath", "TRCS pathlen of track to det plane","fTrackProj.THaTrackProj.fPathl" },
  //    { "trdx",   "track deviation in x-position (m)", "fTrackProj.THaTrackProj.fdX" },
  //    { "trpad",  "paddle-hit associated with track",  "fTrackProj.THaTrackProj.fChannel" },
  //    { 0 }
  //  };
  //  return DefineVarsFromList( vars, mode );
  return kOK;
}

//_____________________________________________________________________________
THcHodoscope::~THcHodoscope()
{
  // Destructor. Remove variables from global list.

  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();
  if (fTrackProj) {
    fTrackProj->Clear();
    delete fTrackProj; fTrackProj = 0;
  }
}

//_____________________________________________________________________________
void THcHodoscope::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  delete [] fNPaddle;  fNPaddle = NULL;
  delete [] fSpacing;  fSpacing = NULL;
  delete [] fCenter;   fCenter = NULL; // This 2D. What is correct way to delete?

  //  delete [] fRA_c;    fRA_c    = NULL;
  //  delete [] fRA_p;    fRA_p    = NULL;
  //  delete [] fRA;      fRA      = NULL;
  //  delete [] fLA_c;    fLA_c    = NULL;
  //  delete [] fLA_p;    fLA_p    = NULL;
  //  delete [] fLA;      fLA      = NULL;
  //  delete [] fRT_c;    fRT_c    = NULL;
  //  delete [] fRT;      fRT      = NULL;
  //  delete [] fLT_c;    fLT_c    = NULL;
  //  delete [] fLT;      fLT      = NULL;
  
  //  delete [] fRGain;   fRGain   = NULL;
  //  delete [] fLGain;   fLGain   = NULL;
  //  delete [] fRPed;    fRPed    = NULL;
  //  delete [] fLPed;    fLPed    = NULL;
  //  delete [] fROff;    fROff    = NULL;
  //  delete [] fLOff;    fLOff    = NULL;
  //  delete [] fTWalkPar; fTWalkPar = NULL;
  //  delete [] fTrigOff; fTrigOff = NULL;

  //  delete [] fHitPad;  fHitPad  = NULL;
  //  delete [] fTime;    fTime    = NULL;
  //  delete [] fdTime;   fdTime   = NULL;
  //  delete [] fYt;      fYt      = NULL;
  //  delete [] fYa;      fYa      = NULL;
}

//_____________________________________________________________________________
inline 
void THcHodoscope::ClearEvent()
{
  // Reset per-event data.

  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcHodoscope::Decode( const THaEvData& evdata )
{

  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

  // fRawHitList is TClones array of THcHodoscopeHit objects
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_pos << " " << hit->fADC_neg << " "  <<  hit->fTDC_pos
	 << " " <<  hit->fTDC_neg << endl;
  }
  cout << endl;

  return nhits;
}

//_____________________________________________________________________________
Int_t THcHodoscope::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Double_t THcHodoscope::TimeWalkCorrection(const Int_t& paddle,
					     const ESide side)
{
  return(0.0);
}

//_____________________________________________________________________________
Int_t THcHodoscope::CoarseProcess( TClonesArray& /* tracks */ )
{
  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  //  static const Double_t sqrt2 = TMath::Sqrt(2.);
  
  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoscope::FineProcess( TClonesArray& tracks )
{
  // Reconstruct coordinates of particle track cross point with scintillator
  // plane, and copy the data into the following local data structure:
  //
  // Units of measurements are meters.

  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::FineTrack() are used.

  return 0;
}

ClassImp(THcHodoscope)
////////////////////////////////////////////////////////////////////////////////
