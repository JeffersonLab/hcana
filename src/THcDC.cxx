///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDC                                                              //
//                                                                           //
// Class for a generic hodoscope consisting of multiple                      //
// planes with multiple paddles with phototubes on both ends.                //
// This differs from Hall A scintillator class in that it is the whole       //
// hodoscope array, not just one plane.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcDC.h"
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
THcDC::THcDC(
 const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaTrackingDetector(name,description,apparatus)
{
  // Constructor

  //  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
  fNPlanes = 0;			// No planes until we make them

}

//_____________________________________________________________________________
void THcDC::Setup(const char* name, const char* description)
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

  string planenamelist;
  DBRequest list[]={
    {"dc_num_planes",&fNPlanes, kInt},
    {"dc_num_chambers",&fNChambers, kInt},
    {"dc_tdc_time_per_channel",&fNSperChan, kDouble},
    {"dc_wire_velocity",&fWireVelocity,kDouble},
    {"dc_plane_names",&planenamelist, kString},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  cout << planenamelist << endl;
  cout << "Drift Chambers: " <<  fNPlanes << " planes in " << fNChambers << " chambers" << endl;

  vector<string> plane_names = vsplit(planenamelist);

  if(plane_names.size() != (UInt_t) fNPlanes) {
    cout << "ERROR: Number of planes " << fNPlanes << " doesn't agree with number of plane names " << plane_names.size() << endl;
    // Should quit.  Is there an official way to quit?
  }
  fPlaneNames = new char* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {
    fPlaneNames[i] = new char[plane_names[i].length()];
    strcpy(fPlaneNames[i], plane_names[i].c_str());
  }

  char *desc = new char[strlen(description)+100];
  fPlanes = new THcDriftChamberPlane* [fNPlanes];

  for(Int_t i=0;i<fNPlanes;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);

    fPlanes[i] = new THcDriftChamberPlane(fPlaneNames[i], desc, i+1, this);
    cout << "Created Drift Chamber Plane " << fPlaneNames[i] << ", " << desc << endl;

  }
  fChambers = new THcDriftChamber* [fNChambers];
  for(Int_t i=0;i<fNChambers;i++) {
    sprintf(desc,"%s Chamber %d",description, i+1);

    // Should construct a better chamber name
    fChambers[i] = new THcDriftChamber(desc, desc, i+1, this);
    cout << "Created Drift Chamber " << i+1 << ", " << desc << endl;
    
    
  }
}

//_____________________________________________________________________________
THcDC::THcDC( ) :
  THaTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcDC::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  Setup(GetName(), GetTitle());	// Create the subdetectors here
  
  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  THcHitList::InitHitList(fDetMap, "THcRawDCHit", 1000);

  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaTrackingDetector::Init( date )) )
    return fStatus=status;

  // Initialize planes and add them to chambers
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    } else {
      Int_t chamber=fNChamber[ip];
      fChambers[chamber-1]->AddPlane(fPlanes[ip]);
    }
  }
  // Initialize chambers
  for(Int_t ic=0;ic<fNChambers;ic++) {
    if((status = fChambers[ic]->Init ( date ))) {
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
Int_t THcDC::ReadDatabase( const TDatime& date )
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

  fXCenter = new Double_t [fNChambers];
  fYCenter = new Double_t [fNChambers];
  fMinHits = new Int_t [fNChambers];
  fMaxHits = new Int_t [fNChambers];
  fMinCombos = new Int_t [fNChambers];
  fSpace_Point_Criterion2 = new Double_t [fNChambers];

  fTdcWinMin = new Int_t [fNPlanes];
  fTdcWinMax = new Int_t [fNPlanes];
  fCentralTime = new Int_t [fNPlanes];
  fNWires = new Int_t [fNPlanes];
  fNChamber = new Int_t [fNPlanes]; // Which chamber is this plane
  fWireOrder = new Int_t [fNPlanes]; // Wire readout order
  fDriftTimeSign = new Int_t [fNPlanes];

  fZPos = new Double_t [fNPlanes];
  fAlphaAngle = new Double_t [fNPlanes];
  fBetaAngle = new Double_t [fNPlanes];
  fGammaAngle = new Double_t [fNPlanes];
  fPitch = new Double_t [fNPlanes];
  fCentralWire = new Double_t [fNPlanes];
  fPlaneTimeZero = new Double_t [fNPlanes];


  DBRequest list[]={
    {"dc_tdc_time_per_channel",&fNSperChan, kDouble},
    {"dc_wire_velocity",&fWireVelocity,kDouble},

    {"dc_xcenter", fXCenter, kDouble, fNChambers},
    {"dc_ycenter", fYCenter, kDouble, fNChambers},
    {"min_hit", fMinHits, kInt, fNChambers},
    {"max_pr_hits", fMaxHits, kInt, fNChambers},
    {"min_combos", fMinCombos, kInt, fNChambers},
    {"space_point_criterion", fSpace_Point_Criterion2, kDouble, fNChambers},

    {"dc_tdc_min_win", fTdcWinMin, kInt, fNPlanes},
    {"dc_tdc_max_win", fTdcWinMax, kInt, fNPlanes},
    {"dc_central_time", fCentralTime, kInt, fNPlanes},
    {"dc_nrwire", fNWires, kInt, fNPlanes},
    {"dc_chamber_planes", fNChamber, kInt, fNPlanes},
    {"dc_wire_counting", fWireOrder, kInt, fNPlanes},
    {"dc_drifttime_sign", fDriftTimeSign, kInt, fNPlanes},

    {"dc_zpos", fZPos, kDouble, fNPlanes},
    {"dc_alpha_angle", fAlphaAngle, kDouble, fNPlanes},
    {"dc_beta_angle", fBetaAngle, kDouble, fNPlanes},
    {"dc_gamma_angle", fGammaAngle, kDouble, fNPlanes},
    {"dc_pitch", fPitch, kDouble, fNPlanes},
    {"dc_central_wire", fCentralWire, kDouble, fNPlanes},
    {"dc_plane_time_zero", fPlaneTimeZero, kDouble, fNPlanes},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  cout << "Plane counts:";
  for(Int_t i=0;i<fNPlanes;i++) {
    cout << " " << fNWires[i];
  }
  cout << endl;

  fIsInit = true;

  return kOK;
}

//_____________________________________________________________________________
Int_t THcDC::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "nhit", "Number of DC hits",  "fNhits" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
THcDC::~THcDC()
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
void THcDC::DeleteArrays()
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
void THcDC::ClearEvent()
{
  // Reset per-event data.
  fNhits = 0;

  for(Int_t i=0;i<fNChambers;i++) {
    fChambers[i]->Clear();
  }

  
  //  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcDC::Decode( const THaEvData& evdata )
{

  ClearEvent();

  // Get the Hall C style hitlist (fRawHitList) for this event
  fNhits = THcHitList::DecodeToHitList(evdata);

  // Let each plane get its hits
  Int_t nexthit = 0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
  }

  // Let each chamber get its hits
  for(Int_t ic=0;ic<fNChambers;ic++) {
    fChambers[ic]->ProcessHits();
  }
#if 0
  // fRawHitList is TClones array of THcRawDCHit objects
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcRawDCHit* hit = (THcRawDCHit *) fRawHitList->At(ihit);
    //    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
    //	 << endl;
    for(Int_t imhit = 0; imhit < hit->fNHits; imhit++) {
      //      cout << "                     " << imhit << " " << hit->fTDC[imhit]
      //	   << endl;
    }
  }
  //  cout << endl;
#endif

  return fNhits;
}

//_____________________________________________________________________________
Int_t THcDC::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcDC::CoarseTrack( TClonesArray& /* tracks */ )
{
  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  //  static const Double_t sqrt2 = TMath::Sqrt(2.);
  for(Int_t i=0;i<fNChambers;i++) {

    fChambers[i]->FindSpacePoints();
  }

  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcDC::FineTrack( TClonesArray& tracks )
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

ClassImp(THcDC)
////////////////////////////////////////////////////////////////////////////////
