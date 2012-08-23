///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDriftChamber                                                              //
//                                                                           //
// Class for a generic hodoscope consisting of multiple                      //
// planes with multiple paddles with phototubes on both ends.                //
// This differs from Hall A scintillator class in that it is the whole       //
// hodoscope array, not just one plane.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcDriftChamber.h"
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
THcDriftChamber::THcDriftChamber(
 const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaTrackingDetector(name,description,apparatus)
{
  // Constructor

  //  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
  fNPlanes = 0;			// No planes until we make them

}

//_____________________________________________________________________________
void THcDriftChamber::Setup(const char* name, const char* description)
{

  char prefix[2];
  char parname[100];

  THaApparatus *app = GetApparatus();
  if(app) {
    cout << app->GetName() << endl;
  } else {
    cout << "No apparatus found" << endl;
  }

  prefix[0]=tolower(app->GetName()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,name); 	// Append "dc"
  Int_t plen=strlen(parname);
  cout << "parname=" << parname << endl;

  // Get number of planes and number wires for each plane
  strcat(parname,"_num_planes");
  fNPlanes = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  parname[plen] = '\0';
  strcat(parname,"_num_chambers");
  fNChambers = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  fPlaneNames = new char* [fNPlanes];

  char *desc = new char[strlen(description)+100];
  fPlanes = new THcDriftChamberPlane* [fNPlanes];

  for(Int_t i=0;i<fNPlanes;i++) {
    fPlaneNames[i] = new char[5];
    sprintf(fPlaneNames[i],"%d",i+1);
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);

    fPlanes[i] = new THcDriftChamberPlane(fPlaneNames[i], desc, i+1, this);
    cout << "Created Drift Chamber Plane " << fPlaneNames[i] << ", " << desc << endl;

  }
    
}

//_____________________________________________________________________________
THcDriftChamber::THcDriftChamber( ) :
  THaTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcDriftChamber::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  Setup(GetName(), GetTitle());	// Create the subdetectors here
  
  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  THcHitList::InitHitList(fDetMap, "THcDCHit", 1000);

  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaTrackingDetector::Init( date )) )
    return fStatus=status;

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }

  // Replace with what we need for Hall C
  //  const DataDest tmp[NDEST] = {
  //    { &fRTNhit, &fRANhit, fRT, fRT_c, fRA, fRA_p, fRA_c, fROff, fRPed, fRGain },
  //    { &fLTNhit, &fLANhit, fLT, fLT_c, fLA, fLA_p, fLA_c, fLOff, fLPed, fLGain }
  //  };
  //  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  char EngineDID[4];

  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  EngineDID[1] = 'D';
  EngineDID[2] = 'C';
  EngineDID[3] = '\0';
  
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     EngineDID);
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::ReadDatabase( const TDatime& date )
{
  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  prefix[0]=tolower(GetApparatus()->GetName()[0]);

  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,"dc");
  Int_t plen=strlen(parname);

  // Get number of planes and number wires for each plane
  strcat(parname,"_num_planes");
  fNPlanes = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  parname[plen]='\0';
  strcat(parname,"_nrwire");
  
  fNWires = new Int_t [fNPlanes];
  Int_t* p= (Int_t *)gHcParms->Find(parname)->GetValuePointer();
  for(Int_t i=0;i<fNPlanes;i++) {
    fNWires[i] = p[i];
  }

  fIsInit = true;

  return kOK;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::DefineVariables( EMode mode )
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
THcDriftChamber::~THcDriftChamber()
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
void THcDriftChamber::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  delete [] fNWires;  fNWires = NULL;
  //  delete [] fSpacing;  fSpacing = NULL;
  //  delete [] fCenter;   fCenter = NULL; // This 2D. What is correct way to delete?

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
void THcDriftChamber::ClearEvent()
{
  // Reset per-event data.

  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcDriftChamber::Decode( const THaEvData& evdata )
{

  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

  // Let each plane get its hits
  Int_t nexthit = 0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
  }

#if 0
  // fRawHitList is TClones array of THcDCHit objects
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcDCHit* hit = (THcDCHit *) fRawHitList->At(ihit);
    //    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
    //	 << endl;
    for(Int_t imhit = 0; imhit < hit->fNHits; imhit++) {
      //      cout << "                     " << imhit << " " << hit->fTDC[imhit]
      //	   << endl;
    }
  }
  //  cout << endl;
#endif

  return nhits;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcDriftChamber::CoarseTrack( TClonesArray& /* tracks */ )
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
Int_t THcDriftChamber::FineTrack( TClonesArray& tracks )
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

ClassImp(THcDriftChamber)
////////////////////////////////////////////////////////////////////////////////
