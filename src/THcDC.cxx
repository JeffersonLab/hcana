/** \class THcDC
    \ingroup Detectors

\brief Analyze a package of horizontal drift chambers.

Uses the
first letter of the apparatus name as a prefix to parameter names.  The
paramters, read in the Setup method, determine the number of chambers and
the number of parameters per plane.

\author S. A. Wood, based on Fortran ENGINE

*/

#include "THcDC.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "THcDCTrack.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TVectorD.h"
#include "THaApparatus.h"
#include "THcHallCSpectrometer.h"

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

  fNPlanes = 0;			// No planes until we make them

  fXCenter = NULL;
  fYCenter = NULL;
  fMinHits = NULL;
  fMaxHits = NULL;
  fMinCombos = NULL;
  fSpace_Point_Criterion = NULL;

  fTdcWinMin = NULL;
  fTdcWinMax = NULL;
  fCentralTime = NULL;
  fNWires = NULL;
  fNChamber = NULL;
  fWireOrder = NULL;
  fDriftTimeSign = NULL;
  fReadoutTB = NULL;
  fReadoutLR = NULL;

  fXPos = NULL;
  fYPos = NULL;
  fZPos = NULL;
  fAlphaAngle = NULL;
  fBetaAngle = NULL;
  fGammaAngle = NULL;
  fPitch = NULL;
  fCentralWire = NULL;
  fPlaneTimeZero = NULL;
  fSigma = NULL;

  // These should be set to zero (in a parameter file) in order to
  // replicate historical ENGINE behavior
  fFixLR = 1;
  fFixPropagationCorrection = 1;
  fProjectToChamber = 0;  // Use 1 for SOS chambers

  fDCTracks = new TClonesArray( "THcDCTrack", 20 );

  fNChamHits = 0;
  fPlaneEvents = 0;

  //The version defaults to 0 (old HMS style). 1 is new HMS style and 2 is SHMS style.
  fVersion = 0;
}

//_____________________________________________________________________________
void THcDC::Setup(const char* name, const char* description)
{

	Bool_t optional = true;

  // Create the chamber and plane objects using parameters.
  static const char* const here = "Setup";

  THaApparatus *app = GetApparatus();
  if(app) {
    // cout << app->GetName() << endl;
    fPrefix[0]=tolower(app->GetName()[0]);
    fPrefix[1]='\0';
  } else {
    cout << "No apparatus found" << endl;
    fPrefix[0]='\0';
  }

  // For now, decide chamber style from the spectrometer name.
  // Should override with a paramter
  //cout<<"HMS Style??\t"<<fHMSStyleChambers<<endl;
  string planenamelist;
  DBRequest list[]={
    {"dc_num_planes",&fNPlanes, kInt},
    {"dc_num_chambers",&fNChambers, kInt},
    {"dc_tdc_time_per_channel",&fNSperChan, kDouble},
    {"dc_wire_velocity",&fWireVelocity,kDouble},
    {"dc_plane_names",&planenamelist, kString},
    {"dc_version", &fVersion, kInt, 0, optional},
    {"dc_tdcrefcut", &fTDC_RefTimeCut, kInt, 0, 1},
    {0}
  };

  fTDC_RefTimeCut = 0;		// Minimum allowed reference times
  gHcParms->LoadParmValues((DBRequest*)&list,fPrefix);

  if(fVersion==0) {
    fHMSStyleChambers = 1;
  } else {
    fHMSStyleChambers = 0;
  }


  cout << "Plane Name List: " << planenamelist << endl;
  cout << "Drift Chambers: " <<  fNPlanes << " planes in " << fNChambers << " chambers" << endl;

  vector<string> plane_names = vsplit(planenamelist);

  if(plane_names.size() != (UInt_t) fNPlanes) {
    cout << "ERROR: Number of planes " << fNPlanes << " doesn't agree with number of plane names " << plane_names.size() << endl;
    // Should quit.  Is there an official way to quit?
  }
  fPlaneNames = new char* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {
    fPlaneNames[i] = new char[plane_names[i].length()+1];
    strcpy(fPlaneNames[i], plane_names[i].c_str());
  }

  char *desc = new char[strlen(description)+100];
  char *desc1= new char[strlen(description)+100];
  fPlanes.clear();

  for(Int_t i=0;i<fNPlanes;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);

    THcDriftChamberPlane* newplane = new THcDriftChamberPlane(fPlaneNames[i], desc, i+1, this);
    if( !newplane or newplane->IsZombie() ) {
      Error( Here(here), "Error creating Drift Chamber plane %s. Call expert.", name);
      MakeZombie();
      delete [] desc;
      delete [] desc1;
      return;
    }
    fPlanes.push_back(newplane);
    newplane->SetDebug(fDebug);
    // cout << "Created Drift Chamber Plane " << fPlaneNames[i] << ", " << desc << endl;

  }

  fChambers.clear();
  for(UInt_t i=0;i<fNChambers;i++) {
    sprintf(desc1,"Ch%d",i+1);

    // Should construct a better chamber name
    THcDriftChamber* newchamber = new THcDriftChamber(desc1, desc, i+1, this);
    fChambers.push_back(newchamber);
    cout << "Created Drift Chamber " << i+1 << ", " << desc1 << endl;
    newchamber->SetHMSStyleFlag(fHMSStyleChambers); // Tell the chamber its style
  }
  delete [] desc;
  delete [] desc1;
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
  // Register the plane objects with the appropriate chambers.
  // Trigger ReadDatabase to load the remaining parameters
  Setup(GetName(), GetTitle());	// Create the subdetectors here
  EffInit();

  char EngineDID[] = "xDC";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID );
    return kInitError;
  }

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  cout << " DC tdc ref time cut = " << fTDC_RefTimeCut  << endl;
  InitHitList(fDetMap, "THcRawDCHit", fDetMap->GetTotNumChan()+1,
	      fTDC_RefTimeCut, 0);

  CreateMissReportParms(Form("%sdc",fPrefix));

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
  for(UInt_t ic=0;ic<fNChambers;ic++) {
    if((status = fChambers[ic]->Init ( date ))) {
      return fStatus=status;
    }
  }
  // Retrieve the fiting coefficients
  fPlaneCoeffs = new Double_t* [fNPlanes];
  for(Int_t ip=0; ip<fNPlanes;ip++) {
    fPlaneCoeffs[ip] = fPlanes[ip]->GetPlaneCoef();
  }

  fResiduals = new Double_t [fNPlanes];
  fResidualsExclPlane = new Double_t [fNPlanes];
  fWire_hit_did = new Double_t [fNPlanes];
  fWire_hit_should = new Double_t [fNPlanes];


  // Replace with what we need for Hall C
  //  const DataDest tmp[NDEST] = {
  //    { &fRTNhit, &fRANhit, fRT, fRT_c, fRA, fRA_p, fRA_c, fROff, fRPed, fRGain },
  //    { &fLTNhit, &fLANhit, fLT, fLT_c, fLA, fLA_p, fLA_c, fLOff, fLPed, fLGain }
  //  };
  //  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );
  
  fPresentP = 0;
  THaVar* vpresent = gHaVars->Find(Form("%s.present",GetApparatus()->GetName()));
  if(vpresent) {
    fPresentP = (Bool_t *) vpresent->GetValuePointer();
  }
  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcDC::ReadDatabase( const TDatime& date )
{
  /**
  Read this detector's parameters from the ThcParmList
  This function is called by THaDetectorBase::Init() once at the
  beginning of the analysis.

  */
  //  static const char* const here = "ReadDatabase()";

  delete [] fXCenter;  fXCenter = new Double_t [fNChambers];
  delete [] fYCenter;  fYCenter = new Double_t [fNChambers];
  delete [] fMinHits;  fMinHits = new Int_t [fNChambers];
  delete [] fMaxHits;  fMaxHits = new Int_t [fNChambers];
  delete [] fMinCombos;  fMinCombos = new Int_t [fNChambers];
  delete [] fSpace_Point_Criterion;  fSpace_Point_Criterion = new Double_t [fNChambers];

  delete [] fTdcWinMin;  fTdcWinMin = new Int_t [fNPlanes];
  delete [] fTdcWinMax;  fTdcWinMax = new Int_t [fNPlanes];
  delete [] fCentralTime;  fCentralTime = new Double_t [fNPlanes];
  delete [] fNWires;  fNWires = new Int_t [fNPlanes];
  delete [] fNChamber;  fNChamber = new Int_t [fNPlanes]; // Which chamber is this plane
  delete [] fWireOrder;  fWireOrder = new Int_t [fNPlanes]; // Wire readout order
  delete [] fDriftTimeSign;  fDriftTimeSign = new Int_t [fNPlanes];
  delete [] fReadoutLR;  fReadoutLR = new Int_t [fNPlanes];
  delete [] fReadoutTB;  fReadoutTB = new Int_t [fNPlanes];

  delete [] fXPos;  fXPos = new Double_t [fNPlanes];
  delete [] fYPos;  fYPos = new Double_t [fNPlanes];
  delete [] fZPos;  fZPos = new Double_t [fNPlanes];
  delete [] fAlphaAngle;  fAlphaAngle = new Double_t [fNPlanes];
  delete [] fBetaAngle;  fBetaAngle = new Double_t [fNPlanes];
  delete [] fGammaAngle;  fGammaAngle = new Double_t [fNPlanes];
  delete [] fPitch;  fPitch = new Double_t [fNPlanes];
  delete [] fCentralWire;  fCentralWire = new Double_t [fNPlanes];
  delete [] fPlaneTimeZero;  fPlaneTimeZero = new Double_t [fNPlanes];
  delete [] fSigma;  fSigma = new Double_t [fNPlanes];

  Bool_t optional = true;

  DBRequest list[]={
    {"dc_tdc_time_per_channel",&fNSperChan, kDouble},
    {"dc_wire_velocity",&fWireVelocity,kDouble},

    {"dc_xcenter", fXCenter, kDouble, fNChambers},
    {"dc_ycenter", fYCenter, kDouble, fNChambers},
    {"min_hit", fMinHits, kInt, fNChambers},
    {"max_pr_hits", fMaxHits, kInt, fNChambers},
    {"min_combos", fMinCombos, kInt, fNChambers},
    {"space_point_criterion", fSpace_Point_Criterion, kDouble, fNChambers},

    {"dc_tdc_min_win", fTdcWinMin, kInt, (UInt_t)fNPlanes},
    {"dc_tdc_max_win", fTdcWinMax, kInt, (UInt_t)fNPlanes},
    {"dc_central_time", fCentralTime, kDouble, (UInt_t)fNPlanes},
    {"dc_nrwire", fNWires, kInt, (UInt_t)fNPlanes},
    {"dc_chamber_planes", fNChamber, kInt, (UInt_t)fNPlanes},
    {"dc_wire_counting", fWireOrder, kInt, (UInt_t)fNPlanes},
    {"dc_drifttime_sign", fDriftTimeSign, kInt, (UInt_t)fNPlanes},
    {"dc_readoutLR", fReadoutLR, kInt, (UInt_t)fNPlanes, optional},
    {"dc_readoutTB", fReadoutTB, kInt, (UInt_t)fNPlanes, optional},

    {"dc_zpos", fZPos, kDouble, (UInt_t)fNPlanes},
    {"dc_alpha_angle", fAlphaAngle, kDouble, (UInt_t)fNPlanes},
    {"dc_beta_angle", fBetaAngle, kDouble, (UInt_t)fNPlanes},
    {"dc_gamma_angle", fGammaAngle, kDouble, (UInt_t)fNPlanes},
    {"dc_pitch", fPitch, kDouble, (UInt_t)fNPlanes},
    {"dc_central_wire", fCentralWire, kDouble, (UInt_t)fNPlanes},
    {"dc_plane_time_zero", fPlaneTimeZero, kDouble, (UInt_t)fNPlanes},
    {"dc_sigma", fSigma, kDouble, (UInt_t)fNPlanes},
    {"single_stub",&fSingleStub, kInt,0,1},
    {"ntracks_max_fp", &fNTracksMaxFP, kInt},
    {"xt_track_criterion", &fXtTrCriterion, kDouble},
    {"yt_track_criterion", &fYtTrCriterion, kDouble},
    {"xpt_track_criterion", &fXptTrCriterion, kDouble},
    {"ypt_track_criterion", &fYptTrCriterion, kDouble},
    {"dc_fix_lr", &fFixLR, kInt},
    {"dc_fix_propcorr", &fFixPropagationCorrection, kInt},
    {"debuglinkstubs", &fdebuglinkstubs, kInt},
    {"debugprintrawdc", &fdebugprintrawdc, kInt},
    {"debugprintdecodeddc", &fdebugprintdecodeddc, kInt},
    {"debugflagpr", &fdebugflagpr, kInt},
    {"debugflagstubs", &fdebugflagstubs, kInt},
    {"debugtrackprint", &fdebugtrackprint , kInt},
    {0}
  };
  fSingleStub=0;
   for(Int_t ip=0; ip<fNPlanes;ip++) {
    fReadoutLR[ip] = 0.0;
    fReadoutTB[ip] = 0.0;
   }


  gHcParms->LoadParmValues((DBRequest*)&list,fPrefix);

  //Set the default plane x,y positions to those of the chamber
   for(Int_t ip=0; ip<fNPlanes;ip++) {
    fXPos[ip] = fXCenter[GetNChamber(ip+1)-1];
    fYPos[ip] = fYCenter[GetNChamber(ip+1)-1];
   }

   //Load the x,y positions of the planes if they exist (overwrites defaults)
   DBRequest listOpt[]={
     {"dc_xpos", fXPos, kDouble, (UInt_t)fNPlanes, optional},
     {"dc_ypos", fYPos, kDouble, (UInt_t)fNPlanes, optional},
     {0}
   };
   gHcParms->LoadParmValues((DBRequest*)&listOpt,fPrefix);
  if(fNTracksMaxFP <= 0) fNTracksMaxFP = 10;
  // if(fNTracksMaxFP > HNRACKS_MAX) fNTracksMaxFP = NHTRACKS_MAX;
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
  /**
    Initialize global variables for histograms and Root tree
  */
  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "stubtest", "stub test",  "fStubTest" },
    { "nhit", "Number of DC hits",  "fNhits" },
    { "tnhit", "Number of good DC hits",  "fNthits" },
    { "trawhit", "Number of true raw DC hits", "fN_True_RawHits" },
    { "ntrack", "Number of Tracks", "fNDCTracks" },
    { "nsp", "Number of Space Points", "fNSp" },
    { "track_nsp", "Number of spacepoints in track", "fDCTracks.THcDCTrack.GetNSpacePoints()"},
    { "x", "X at focal plane", "fDCTracks.THcDCTrack.GetX()"},
    { "y", "Y at focal plane", "fDCTracks.THcDCTrack.GetY()"},
    { "xp", "XP at focal plane", "fDCTracks.THcDCTrack.GetXP()"},
    { "yp", "YP at focal plane", "fDCTracks.THcDCTrack.GetYP()"},
    { "x_fp", "X at focal plane (golden track)", "fX_fp_best"},
    { "y_fp", "Y at focal plane( golden track)", "fY_fp_best"},
    { "xp_fp", "XP at focal plane (golden track)", "fXp_fp_best"},
    { "yp_fp", "YP at focal plane(golden track) ", "fYp_fp_best"},
    { "chisq", "chisq/dof (golden track) ", "fChisq_best"},
    { "sp1_id", " (golden track) ", "fSp1_ID_best"},
    { "sp2_id", " (golden track) ", "fSp2_ID_best"},
    { "InsideDipoleExit", " ","fInSideDipoleExit_best"},
    { "gtrack_nsp", " Number of space points in golden track ", "fNsp_best"},
    { "residual", "Residuals", "fResiduals"},
    { "residualExclPlane", "Residuals", "fResidualsExclPlane"},
    { "wireHitDid","Wire did have  matched track hit", "fWire_hit_did"},
    { "wireHitShould", "Wire should have matched track hit", "fWire_hit_should"},
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

  // Delete the plane objects
  for (vector<THcDriftChamberPlane*>::iterator ip = fPlanes.begin();
       ip != fPlanes.end(); ++ip) delete *ip;
  // Delete the chamber objects
  for (vector<THcDriftChamber*>::iterator ip = fChambers.begin();
       ip != fChambers.end(); ++ip) delete *ip;

  delete fDCTracks;
}

//_____________________________________________________________________________
void THcDC::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  delete [] fXCenter;   fXCenter = NULL;
  delete [] fYCenter;   fYCenter = NULL;
  delete [] fMinHits;   fMinHits = NULL;
  delete [] fMaxHits;   fMaxHits = NULL;
  delete [] fMinCombos;   fMinCombos = NULL;
  delete [] fSpace_Point_Criterion;   fSpace_Point_Criterion = NULL;

  delete [] fTdcWinMin;   fTdcWinMin = NULL;
  delete [] fTdcWinMax;   fTdcWinMax = NULL;
  delete [] fCentralTime;   fCentralTime = NULL;
  delete [] fNWires;   fNWires = NULL;
  delete [] fNChamber;   fNChamber = NULL;
  delete [] fWireOrder;   fWireOrder = NULL;
  delete [] fDriftTimeSign;   fDriftTimeSign = NULL;
  delete [] fReadoutLR;   fReadoutLR = NULL;
  delete [] fReadoutTB;   fReadoutTB = NULL;

  delete [] fXPos;   fXPos = NULL;
  delete [] fYPos;   fYPos = NULL;
  delete [] fZPos;   fZPos = NULL;
  delete [] fAlphaAngle;   fAlphaAngle = NULL;
  delete [] fBetaAngle;   fBetaAngle = NULL;
  delete [] fGammaAngle;   fGammaAngle = NULL;
  delete [] fPitch;   fPitch = NULL;
  delete [] fCentralWire;   fCentralWire = NULL;
  delete [] fPlaneTimeZero;   fPlaneTimeZero = NULL;
  delete [] fSigma;   fSigma = NULL;

  // Efficiency arrays
  delete [] fNChamHits; fNChamHits = NULL;
  delete [] fPlaneEvents; fPlaneEvents = NULL;

  for( Int_t i = 0; i<fNPlanes; ++i )
    delete [] fPlaneNames[i];
  delete [] fPlaneNames;

  delete [] fPlaneCoeffs; fPlaneCoeffs = 0;
  delete [] fResiduals; fResiduals = 0;
  delete [] fResidualsExclPlane; fResidualsExclPlane = 0;
  delete [] fWire_hit_did; fWire_hit_did = 0;
  delete [] fWire_hit_should; fWire_hit_should = 0;
}

//_____________________________________________________________________________
inline
void THcDC::ClearEvent()
{
  // Reset per-event data.
  fStubTest = 0;
  fNhits = 0;
  fNthits = 0;
  fN_True_RawHits=0;
  fX_fp_best=-10000.;
  fY_fp_best=-10000.;
  fXp_fp_best=-10000.;
  fYp_fp_best=-10000.;
  fChisq_best=kBig;
  fNsp_best=0;
  fInSideDipoleExit_best = kTRUE;
  for(UInt_t i=0;i<fNChambers;i++) {
    fChambers[i]->Clear();
  }

  for(Int_t i=0;i<fNPlanes;i++) {
    fResiduals[i] = 1000.0;
    fResidualsExclPlane[i] = 1000.0;
    fWire_hit_did[i] = 1000.0;
    fWire_hit_should[i] = 1000.0;
  }

  //  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcDC::Decode( const THaEvData& evdata )
{
  /**
    Decode event into hit list.
    Pass hit list to the planes.
    Load hits from planes into chamber objects
  */
  ClearEvent();
  Int_t num_event = evdata.GetEvNum();
  if (fdebugprintrawdc ||fdebugprintdecodeddc || fdebuglinkstubs || fdebugtrackprint) cout << " event num = " << num_event << endl;
  // Get the Hall C style hitlist (fRawHitList) for this event

  Bool_t present = kTRUE;	// Suppress reference time warnings
  if(fPresentP) {		// if this spectrometer not part of trigger
    present = *fPresentP;
  }
  fNhits = DecodeToHitList(evdata, !present);



  if(!gHaCuts->Result("Pedestal_event")) {
    // Let each plane get its hits
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
      fN_True_RawHits += fPlanes[ip]->GetNRawhits();

    }

    // fRawHitList is TClones array of THcRawDCHit objects
    Int_t counter=0;
    if (fdebugprintrawdc) {
      cout << " RAW_TOT_HITS = " <<  fNRawHits << endl;
      cout << " Hit #  " << "Plane  " << " Wire " <<  " Raw TDC " << endl;
      for(UInt_t ihit = 0; ihit < fNRawHits ; ihit++) {
	THcRawDCHit* hit = (THcRawDCHit *) fRawHitList->At(ihit);
	for(UInt_t imhit = 0; imhit < hit->GetRawTdcHit().GetNHits(); imhit++) {
	  counter++;
	  cout << counter << "      " << hit->fPlane << "     " << hit->fCounter << "     " << hit->GetRawTdcHit().GetTimeRaw(imhit)	   << endl;
	}
      }
      cout << endl;
    }
    Eff();			// Accumlate statistics
  }
  return fNhits;
}

//_____________________________________________________________________________
Int_t THcDC::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcDC::CoarseTrack( TClonesArray& tracks )
{
  /**
     Find a set of tracks through the drift chambers and put them
     into the tracks TClonesArray.
     Tracks are in the detector coordinate system.
  */

  // Subtract starttimes from each plane hit
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      fPlanes[ip]->SubtractStartTime();
    }
  //
    // Let each chamber get its hits
    for(UInt_t ic=0;ic<fNChambers;ic++) {
      fChambers[ic]->ProcessHits();
      fNthits += fChambers[ic]->GetNHits();
      if (fdebugprintdecodeddc)fChambers[ic]->PrintDecode();
    }
    //
  for(UInt_t i=0;i<fNChambers;i++) {
    fChambers[i]->FindSpacePoints();
    fChambers[i]->CorrectHitTimes();
    fChambers[i]->LeftRight();
  }
  if (fdebugflagstubs) PrintSpacePoints();
  if (fdebugflagstubs)  PrintStubs();
  // Now link the stubs between chambers
  LinkStubs();
 if(fNDCTracks > 0) {
     TrackFit();
    // Copy tracks into podd tracks list
    for(UInt_t itrack=0;itrack<fNDCTracks;itrack++) {
      THaTrack* theTrack = NULL;
      theTrack = AddTrack(tracks, 0.0, 0.0, 0.0, 0.0); // Leaving off trackID
      // Should we add stubs with AddCluster?  Could we do this
      // by having stubs inherit from cluster

      THcDCTrack *tr = static_cast<THcDCTrack*>( fDCTracks->At(itrack));
      theTrack->Set(tr->GetX(), tr->GetY(), tr->GetXP(), tr->GetYP());
      theTrack->SetFlag((UInt_t) 0);
      // Need to look at how engine does chi2 and track selection.  Reduced?
      theTrack->SetChi2(tr->GetChisq(),tr->GetNFree());
      // CalcFocalPlaneCoords.  Aren't our tracks already in focal plane coords
      // We should have some kind of track ID so that the THaTrack can be
      // associate back with the DC track
      // Assign the track number
      theTrack->SetTrkNum(itrack+1);
    }
 }


  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcDC::FineTrack( TClonesArray& tracks )
{

  return 0;
}
//
void THcDC::SetFocalPlaneBestTrack(Int_t golden_track_index)
{
      THcDCTrack *tr1 = static_cast<THcDCTrack*>( fDCTracks->At(golden_track_index));
      fX_fp_best=tr1->GetX();
      fY_fp_best=tr1->GetY();
      fXp_fp_best=tr1->GetXP();
      fYp_fp_best=tr1->GetYP();
      THcHallCSpectrometer *app = dynamic_cast<THcHallCSpectrometer*>(GetApparatus());
      fInSideDipoleExit_best = app->InsideDipoleExitWindow(fX_fp_best, fXp_fp_best ,fY_fp_best,fYp_fp_best);
      fSp1_ID_best=tr1->GetSp1_ID();
      fSp2_ID_best=tr1->GetSp2_ID();
      fChisq_best=tr1->GetChisq();
      fNsp_best=tr1->GetNSpacePoints();
         for (UInt_t ihit = 0; ihit < UInt_t (tr1->GetNHits()); ihit++) {
	THcDCHit *hit = tr1->GetHit(ihit);
	Int_t plane = hit->GetPlaneNum() - 1;
        fResiduals[plane] = tr1->GetResidual(plane);
        fResidualsExclPlane[plane] = tr1->GetResidualExclPlane(plane);
	 } 
	 EfficiencyPerWire(golden_track_index);
}
//
void THcDC::EfficiencyPerWire(Int_t golden_track_index)
{
  THcDCTrack *tr1 = static_cast<THcDCTrack*>( fDCTracks->At(golden_track_index));
  Double_t track_pos;
  for (UInt_t ihit = 0; ihit < UInt_t (tr1->GetNHits()); ihit++) {
    THcDCHit *hit = tr1->GetHit(ihit);
    Int_t plane = hit->GetPlaneNum() - 1;
    track_pos=tr1->GetCoord(plane);
    Int_t wire_num = hit->GetWireNum();
    Int_t wire_track_num=round(fPlanes[plane]->CalcWireFromPos(track_pos));
    if ( (wire_num-wire_track_num) ==0) fWire_hit_did[plane]=wire_num;
  } 
  for(Int_t ip=0; ip<fNPlanes;ip++) {
    track_pos=tr1->GetCoord(ip);
    Int_t wire_should = round(fPlanes[ip]->CalcWireFromPos(track_pos));
    fWire_hit_should[ip]=wire_should;
  }
}
//
void THcDC::PrintSpacePoints()
{
  for(UInt_t ich=0;ich<fNChambers;ich++) {
    printf("%s %2d %s %3d %s %3d \n"," chamber = ",fChambers[ich]->GetChamberNum()," number of hits = ",fChambers[ich]->GetNHits()," number of spacepoints = ",fChambers[ich]->GetNSpacePoints());
    printf("%6s %-8s %-8s %6s %6s %10s \n","     "," "," ","Number","Number","Plane Wire");
    printf("%6s %-8s %-8s %6s %6s %10s \n","Point","x","y"," hits ","combos"," for each hit");
    TClonesArray* spacepointarray = fChambers[ich]->GetSpacePointsP();
    for(Int_t isp=0;isp<fChambers[ich]->GetNSpacePoints();isp++) {
      THcSpacePoint* sp = (THcSpacePoint*)(spacepointarray->At(isp));
      printf("%5d %8.5f %8.5f %5d  %5d ",isp+1,sp->GetX(),sp->GetY(),sp->GetNHits(),sp->GetCombos()) ;
      for (Int_t ii=0;ii<sp->GetNHits();ii++) {
	THcDCHit* hittemp = (THcDCHit*)(sp->GetHit(ii));
	printf("%3d %3d %3d",hittemp->GetPlaneNum(),hittemp->GetWireNum(),hittemp->GetLR());
      }
      printf("\n");
    }
  }
}
//
//
void THcDC::PrintStubs()
{
  for(UInt_t ich=0;ich<fNChambers;ich++) {
    printf("%s %3d \n"," Stub fit results Chamber = ",ich+1);
    printf("%-5s %-18s %-18s %-18s %-18s\n","point","x_t","y_t","xp_t","yp_t");
    printf("%-5s %-18s %-18s %-18s %-18s\n","     ","[cm]","[cm]","[cm]","[cm]");
    TClonesArray* spacepointarray = fChambers[ich]->GetSpacePointsP();
    for(Int_t isp=0;isp<fChambers[ich]->GetNSpacePoints();isp++) {
      THcSpacePoint* sp = (THcSpacePoint*)(spacepointarray->At(isp));
      Double_t *spstubt=sp->GetStubP();
      printf("%-5d % 15.10e % 15.10e % 15.10e % 15.10e \n",isp+1,spstubt[0],spstubt[1],spstubt[2],spstubt[3]);
    }
  }
}
//
//_____________________________________________________________________________
void THcDC::LinkStubs()
{
  /**
       The logic is
                    0) Put all space points in a single list
                    1) loop over all space points as seeds  isp1
                    2) Check if this space point is all ready in a track
                    3) loop over all succeeding space pointss   isp2
                    4)  check if there is a track-criterion match
                         either add to existing track
                         or if there is another point in same chamber
                            make a copy containing isp2 rather than
                              other point in same chamber
                    5) If hsingle_stub is set, make a track of all single
                       stubs.
  */

  std::vector<THcSpacePoint*> fSp;
  fNSp=0;
  fSp.clear();
  fSp.reserve(100);
  fNDCTracks=0;		// Number of Focal Plane tracks found
  fDCTracks->Delete();
  // Make a vector of pointers to the SpacePoints
  //if (fChambers[0]->GetNSpacePoints()+fChambers[1]->GetNSpacePoints()>10) return;

  for(UInt_t ich=0;ich<fNChambers;ich++) {
    Int_t nchamber=fChambers[ich]->GetChamberNum();
    TClonesArray* spacepointarray = fChambers[ich]->GetSpacePointsP();
    for(Int_t isp=0;isp<fChambers[ich]->GetNSpacePoints();isp++) {
      fSp.push_back(static_cast<THcSpacePoint*>(spacepointarray->At(isp)));
      fSp[fNSp]->fNChamber = nchamber;
      fSp[fNSp]->fNChamber_spnum = isp;
      fNSp++;
      if (ich==0 && fNSp>50) break;
      if (fNSp>100) break;
    }
  }
  Double_t stubminx = 999999;
  Double_t stubminy = 999999;
  Double_t stubminxp = 999999;
  Double_t stubminyp = 999999;
  Int_t stub_tracks[MAXTRACKS];
  if(fSingleStub==0) {
    for(Int_t isp1=0;isp1<fNSp-1;isp1++) { // isp1 is index/id in total list of space points
      THcSpacePoint* sp1 = fSp[isp1];
      Int_t sptracks=0;
      // Now make sure this sp is not already used in a sp.
      // Could this be done by having a sp point to the track it is in?
      Int_t tryflag=1;
      for(UInt_t itrack=0;itrack<fNDCTracks;itrack++) {
	THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(itrack));
	for(Int_t isp=0;isp<theDCTrack->GetNSpacePoints();isp++) {
	  // isp is index into list of space points attached to theDCTrack
	  if(theDCTrack->GetSpacePoint(isp) == sp1) {
	    tryflag=0;
	  }
	}
      }
      if(tryflag) { // SP not already part of a track
	Int_t newtrack=1;
	for(Int_t isp2=isp1+1;isp2<fNSp;isp2++) {
	  THcSpacePoint* sp2=fSp[isp2];
	  if(sp1->fNChamber!=sp2->fNChamber&&sp1->GetSetStubFlag()&&sp2->GetSetStubFlag()) {
	    Double_t *spstub1=sp1->GetStubP();
	    Double_t *spstub2=sp2->GetStubP();
	    Double_t dposx = spstub1[0] - spstub2[0];
	    Double_t dposy;
	    if(fProjectToChamber) { // From SOS s_link_stubs
	      // Since single chamber resolution is ~50mr, and the maximum y`
	      // angle is about 30mr, use differenece between y AT CHAMBERS, rather
	      // than at focal plane.  (Project back to chamber, to take out y' uncertainty)
	      // (Should this be done for SHMS and HMS too?)
	      Double_t y1=spstub1[1]+fChambers[sp1->fNChamber]->GetZPos()*spstub1[3];
	      Double_t y2=spstub2[1]+fChambers[sp2->fNChamber]->GetZPos()*spstub2[3];
	      dposy = y1-y2;
	    } else {
	      dposy = spstub1[1] - spstub2[1];
	    }
	    Double_t dposxp = spstub1[2] - spstub2[2];
	    Double_t dposyp = spstub1[3] - spstub2[3];

	    // What is the point of saving these stubmin values.  They
	    // Don't seem to be used anywhere except that they can be
	    // printed out if hbypass_track_eff_files is zero.
	    if(TMath::Abs(dposx)<TMath::Abs(stubminx)) stubminx = dposx;
	    if(TMath::Abs(dposy)<TMath::Abs(stubminy)) stubminy = dposy;
	    if(TMath::Abs(dposxp)<TMath::Abs(stubminxp)) stubminxp = dposxp;
	    if(TMath::Abs(dposyp)<TMath::Abs(stubminyp)) stubminyp = dposyp;

	    // if hbypass_track_eff_files == 0 then
	    // Print out each stubminX that is less that its criterion

	    if((TMath::Abs(dposx) < fXtTrCriterion)
	       && (TMath::Abs(dposy) < fYtTrCriterion)
	       && (TMath::Abs(dposxp) < fXptTrCriterion)
	       && (TMath::Abs(dposyp) < fYptTrCriterion)) {
	      if(newtrack) {
		assert(sptracks==0);
		fStubTest = 1;
		//stubtest=1;  Used in h_track_tests.f
		// Make a new track if there are not to many
		if(fNDCTracks < fNTracksMaxFP) {
		  sptracks=0; // Number of tracks with this seed
		  stub_tracks[sptracks++] = fNDCTracks;
		  THcDCTrack *theDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
		  theDCTrack->AddSpacePoint(sp1);
		  theDCTrack->AddSpacePoint(sp2);
		  if (sp1->fNChamber==1) theDCTrack->SetSp1_ID(sp1->fNChamber_spnum);
		  if (sp1->fNChamber==2) theDCTrack->SetSp2_ID(sp1->fNChamber_spnum);
		  if (sp2->fNChamber==1) theDCTrack->SetSp1_ID(sp2->fNChamber_spnum);
		  if (sp2->fNChamber==2) theDCTrack->SetSp2_ID(sp2->fNChamber_spnum);
		  newtrack = 0; // Make no more tracks in this loop
		  // (But could replace a SP?)
		} else {
                  if (fHMSStyleChambers) {
		  fNDCTracks=0;
		  return;
                  }
		}
	      } else {
		// Check if there is another space point in the same chamber
		for(Int_t itrack=0;itrack<sptracks;itrack++) {
		  Int_t track=stub_tracks[itrack];
		  THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(track));
		  Int_t spoint=-1;
		  Int_t duppoint=0;
		  for(Int_t isp=0;isp<theDCTrack->GetNSpacePoints();isp++) {
		    // isp is index of space points in theDCTrack
		    if(sp2->fNChamber ==
		       theDCTrack->GetSpacePoint(isp)->fNChamber) {
		      spoint=isp;
		    }
		    if(sp2==theDCTrack->GetSpacePoint(isp)) {
		      duppoint=1;
		    }
		  } // End loop over sp in tracks with isp1
		    // If there is no other space point in this chamber
		    // add this space point to current track(2)
		  if(!duppoint) {
		    if(spoint<0) {
		      theDCTrack->AddSpacePoint(sp2);
		      if (sp2->fNChamber==1) theDCTrack->SetSp1_ID(sp2->fNChamber_spnum);
		      if (sp2->fNChamber==2) theDCTrack->SetSp2_ID(sp2->fNChamber_spnum);
		    } else {
		      		      // If there is another point in the same chamber
		      // in this track create a new track with all the
		      // same space points except spoint
 		      if(fNDCTracks < MAXTRACKS) {
			stub_tracks[sptracks++] = fNDCTracks;
			THcDCTrack *newDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
			for(Int_t isp=0;isp<theDCTrack->GetNSpacePoints();isp++) {
			  if(isp!=spoint) {
			    newDCTrack->AddSpacePoint(theDCTrack->GetSpacePoint(isp));
		            if (theDCTrack->GetSpacePoint(isp)->fNChamber==1) newDCTrack->SetSp1_ID(theDCTrack->GetSpacePoint(isp)->fNChamber_spnum);
		            if (theDCTrack->GetSpacePoint(isp)->fNChamber==2) newDCTrack->SetSp2_ID(theDCTrack->GetSpacePoint(isp)->fNChamber_spnum);
			  } else {
			    newDCTrack->AddSpacePoint(sp2);
		            if (sp2->fNChamber==1) newDCTrack->SetSp1_ID(sp2->fNChamber_spnum);
		            if (sp2->fNChamber==2) newDCTrack->SetSp2_ID(sp2->fNChamber_spnum);
			  } // End check for dup on copy
			} // End copy of track
		      } else {
			if (fHMSStyleChambers) {
			  if (fdebuglinkstubs) cout << "EPIC FAIL 2:  Too many tracks found in THcDC::LinkStubs maxtracks = " << MAXTRACKS << endl;
                          fNDCTracks=0;
                  	  return; // Max # of allowed tracks
                        }
		      }
		    } // end if on same chamber
		  } // end if on duplicate point
		} // end for over tracks with isp1
	      } // else newtrack
	    } // criterion
	  } // end test on same chamber
	} // end isp2 loop over new space points
      } // end test on tryflag
    } // end isp1 outer loop over space points
    //
  //
  } else { // Make track out of each single space point
    for(Int_t isp=0;isp<fNSp;isp++) {
      if(fNDCTracks<MAXTRACKS) {
	// Need some constructed t thingy
        if (fSp[isp]->GetSetStubFlag()) {
	  THcDCTrack *newDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
	  newDCTrack->AddSpacePoint(fSp[isp]);
	}
      } else {
	if (fdebuglinkstubs) cout << "EPIC FAIL 3:  Too many tracks found in THcDC::LinkStubs" << endl;
	fNDCTracks=0;
	// Do something here to fail this event
	return; // Max # of allowed tracks
      }
    }
  }
  ///
  if (fdebuglinkstubs) {
    cout << " Number of tracks from link stubs = " << fNDCTracks << endl;
    printf("%s %s \n","Track","Plane Wire ");
    for (UInt_t itrack=0;itrack<fNDCTracks;itrack++) {
      THcDCTrack *tempTrack = (THcDCTrack*)( fDCTracks->At(itrack));
      printf("%-5d  ",itrack+1);
      for (Int_t ihit=0;ihit<tempTrack->GetNHits();ihit++) {
       	THcDCHit* hit=(THcDCHit*)(tempTrack->GetHit(ihit));
       	printf("%3d %3d",hit->GetPlaneNum(),hit->GetWireNum());
      }
      printf("\n");
    }
  }
}

//_____________________________________________________________________________
void THcDC::TrackFit()
{
  /**
     Primary track fitting routine
  */

  // Number of ray parameters in focal plane.
  const Int_t raycoeffmap[]={4,5,2,3};

  Double_t dummychi2 = 1.0E4;
  for(UInt_t itrack=0;itrack<fNDCTracks;itrack++) {
    //    Double_t chi2 = dummychi2;
    //    Int_t htrack_fit_num = itrack;
    THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(itrack));

    Double_t coords[theDCTrack->GetNHits()];
    Int_t planes[theDCTrack->GetNHits()];
    for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
      THcDCHit* hit=theDCTrack->GetHit(ihit);
      planes[ihit]=hit->GetPlaneNum()-1;
      if(fFixLR) {
	if(fFixPropagationCorrection) {
	  coords[ihit] = hit->GetPos()
	    + theDCTrack->GetHitLR(ihit)*theDCTrack->GetHitDist(ihit);
	} else {
	  coords[ihit] = hit->GetPos()
	    + theDCTrack->GetHitLR(ihit)*hit->GetDist();
	}

      } else {
	if(fFixPropagationCorrection) {
	  coords[ihit] = hit->GetPos()
	    + hit->GetLR()*theDCTrack->GetHitDist(ihit);
	} else {
	  coords[ihit] = hit->GetCoord();
	}
      }


    } //end loop over hits

    theDCTrack->SetNFree(theDCTrack->GetNHits() - NUM_FPRAY);
    Double_t chi2 = dummychi2;
    if(theDCTrack->GetNFree() > 0) {
      TVectorD TT(NUM_FPRAY);
      TMatrixD AA(NUM_FPRAY,NUM_FPRAY);
      for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	TT[irayp] = 0.0;
	for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {	

	  THcDCHit* hit=theDCTrack->GetHit(ihit);
	    
	  TT[irayp] += (coords[ihit]*fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]])/pow(hit->GetWireSigma(),2);
	  //	  if (hit->GetPlaneNum()==5)
	  //	    {
	  //	      //	cout << "Plane: " << hit->GetPlaneNum() << endl;
	  //	      //cout << "Wire: " <<hit->GetWireNum() << endl;
	  //	      //cout << "Sigma: " << hit->GetWireSigma() << endl;
	  //	    }

	} //end hit loop
      }
      for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	for(Int_t jrayp=0;jrayp<NUM_FPRAY;jrayp++) {
	  AA[irayp][jrayp] = 0.0;
	  if(jrayp<irayp) { // Symmetric
	    AA[irayp][jrayp] = AA[jrayp][irayp];
	  } else {
	    for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {

	      THcDCHit* hit=theDCTrack->GetHit(ihit);
        
		      
	      AA[irayp][jrayp] += fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]]*
		fPlaneCoeffs[planes[ihit]][raycoeffmap[jrayp]]/
		pow(hit->GetWireSigma(),2);

	    } //end ihit loop
	  }
	}
      }

      // Solve 4x4 equations
      TVectorD dray(NUM_FPRAY);
      // Should check that it is invertable
      AA.Invert();
      dray = AA*TT;
      //      cout << "DRAY: " << dray[0] << " "<< dray[1] << " "<< dray[2] << " "<< dray[3] << " "  << endl;
      //      if(bad_determinant) {
      //	dray[0] = dray[1] = 10000.; dray[2] = dray[3] = 2.0;
      //      }
      // Calculate hit coordinate for each plane for chi2 and efficiency
      // calculations

      // Make sure fCoords, fResiduals, and fDoubleResiduals are clear
      for(Int_t iplane=0;iplane < fNPlanes; iplane++) {
	Double_t coord=0.0;
	for(Int_t ir=0;ir<NUM_FPRAY;ir++) {
	  coord += fPlaneCoeffs[iplane][raycoeffmap[ir]]*dray[ir];
	  // cout << "ir = " << ir << ", dray[ir] = " << dray[ir] << endl;
	}
	theDCTrack->SetCoord(iplane,coord);
      }
      // Compute Chi2 and residuals
      chi2 = 0.0;
      for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {

	THcDCHit* hit=theDCTrack->GetHit(ihit);


	Double_t residual = coords[ihit] - theDCTrack->GetCoord(planes[ihit]);
	theDCTrack->SetResidual(planes[ihit], residual);

  //  	  double track_coord = theDCTrack->GetCoord(planes[ihit]);
//cout<<planes[ihit]<<"\t"<<track_coord<<"\t"<<coords[ihit]<<"\t"<<residual<<endl;
	chi2 += pow(residual/hit->GetWireSigma(),2);

      }

      theDCTrack->SetVector(dray[0], dray[1], 0.0, dray[2], dray[3]);
    }
    theDCTrack->SetChisq(chi2);

    // calculate ray without a plane in track
    for(Int_t ipl_hit=0;ipl_hit < theDCTrack->GetNHits();ipl_hit++) {    
 

      if(theDCTrack->GetNFree() > 0) {
	TVectorD TT(NUM_FPRAY);
	TMatrixD AA(NUM_FPRAY,NUM_FPRAY);
	for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	  TT[irayp] = 0.0;
	  for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	  

	    THcDCHit* hit=theDCTrack->GetHit(ihit);

	    if (ihit != ipl_hit) {
	      TT[irayp] += (coords[ihit]*
			    fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]])
		/pow(hit->GetWireSigma(),2);

	    }
	  }
	}
	for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	  for(Int_t jrayp=0;jrayp<NUM_FPRAY;jrayp++) {
	    AA[irayp][jrayp] = 0.0;
	    if(jrayp<irayp) { // Symmetric
	      AA[irayp][jrayp] = AA[jrayp][irayp];
	    } else {

	      for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	      
		THcDCHit* hit=theDCTrack->GetHit(ihit);


		if (ihit != ipl_hit) {
		  AA[irayp][jrayp] += fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]]*
		    fPlaneCoeffs[planes[ihit]][raycoeffmap[jrayp]]/
		    pow(hit->GetWireSigma(),2);

		}
	      }
	    }
	  }
	}
	//
	// Solve 4x4 equations
	// Should check that it is invertable
	TVectorD dray(NUM_FPRAY);
	AA.Invert();
	dray = AA*TT;
	Double_t coord=0.0;
	for(Int_t ir=0;ir<NUM_FPRAY;ir++) {
	  coord += fPlaneCoeffs[planes[ipl_hit]][raycoeffmap[ir]]*dray[ir];
	  // cout << "ir = " << ir << ", dray[ir] = " << dray[ir] << endl;
	}
	Double_t residual = coords[ipl_hit] - coord;
	theDCTrack->SetResidualExclPlane(planes[ipl_hit], residual);
      }
    }
  }
  //Calculate residual without plane

  // Calculate residuals for each chamber if in single stub mode
  // and there was a track found in each chamber
  // Specific for two chambers.  Can/should it be generalized?

  if(fSingleStub != 0) {
    if(fNDCTracks == 2) {
      THcDCTrack *theDCTrack1 = static_cast<THcDCTrack*>( fDCTracks->At(0));
      THcDCTrack *theDCTrack2 = static_cast<THcDCTrack*>( fDCTracks->At(1));
      //      Int_t itrack=0;
      Int_t ihit=0;
      THcDCHit* hit=theDCTrack1->GetHit(ihit);
      Int_t plane=hit->GetPlaneNum()-1;
      Int_t chamber=fNChamber[plane];
      if(chamber==1) {
	//	itrack=1;
	hit=theDCTrack2->GetHit(ihit);
	plane=hit->GetPlaneNum()-1;
	chamber=fNChamber[plane];
	if(chamber==2) {
	  Double_t ray1[4];
	  Double_t ray2[4];
	  theDCTrack1->GetRay(ray1);
	  theDCTrack2->GetRay(ray2);
	  //	  itrack = 1;
	  // Loop over hits in second chamber
	  for(Int_t ihit=0;ihit < theDCTrack2->GetNHits();ihit++) {
	    // Calculate residual in second chamber from first chamber track
	    THcDCHit* hit=theDCTrack2->GetHit(ihit);
	    Int_t plane=hit->GetPlaneNum()-1;
	    Double_t pos = DpsiFun(ray1,plane);
	    Double_t coord;
	    if(fFixLR) {
	      if(fFixPropagationCorrection) {
		coord = hit->GetPos()
		  + theDCTrack2->GetHitLR(ihit)*theDCTrack2->GetHitDist(ihit);
	      } else {
		coord = hit->GetPos()
		  + theDCTrack2->GetHitLR(ihit)*hit->GetDist();
	      }
	    } else {
	      if(fFixPropagationCorrection) {
		coord = hit->GetPos()
		  + hit->GetLR()*theDCTrack2->GetHitDist(ihit);
	      } else {
		coord = hit->GetCoord();
	      }
	    }
	    theDCTrack1->SetDoubleResidual(plane,coord - pos);
	    //  hdc_dbl_res(pln) = hdc_double_residual(1,pln)  for hists
	  }
	  //	  itrack=0;
	  // Loop over hits in first chamber
	  for(Int_t ihit=0;ihit < theDCTrack1->GetNHits();ihit++) {
	    // Calculate residual in first chamber from second chamber track
	    THcDCHit* hit=theDCTrack1->GetHit(ihit);
	    Int_t plane=hit->GetPlaneNum()-1;
	    Double_t pos = DpsiFun(ray1,plane);
	    Double_t coord;
	    if(fFixLR) {
	      if(fFixPropagationCorrection) {
		coord = hit->GetPos()
		  + theDCTrack1->GetHitLR(ihit)*theDCTrack1->GetHitDist(ihit);
	      } else {
		coord = hit->GetPos()
		  + theDCTrack1->GetHitLR(ihit)*hit->GetDist();
	      }
	    } else {
	      if(fFixPropagationCorrection) {
		coord = hit->GetPos()
		  + hit->GetLR()*theDCTrack1->GetHitDist(ihit);
	      } else {
		coord = hit->GetCoord();
	      }
	    }
	    theDCTrack2->SetDoubleResidual(plane,coord - pos);
	    //  hdc_dbl_res(pln) = hdc_double_residual(1,pln)  for hists
	  }
	}
      }
    }
  }
  //
  if (fdebugtrackprint) {
    printf("%5s %-14s %-14s %-14s %-14s  %-10s %-10s \n","Track","x_t","y_t","xp_t","yp_t","chi2","DOF");
    printf("%5s %-14s %-14s %-14s %-14s  %-10s %-10s \n","     ","[cm]","[cm]","[rad]","[rad]"," "," ");
    for(UInt_t itr=0;itr < fNDCTracks;itr++) {
      THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(itr));
      printf("%-5d %14.6e %14.6e %14.6e %14.6e %10.3e %3d \n", itr+1,theDCTrack->GetX(),theDCTrack->GetY(),theDCTrack->GetXP(),theDCTrack->GetYP(),theDCTrack->GetChisq(),theDCTrack->GetNFree());
    }
    for(UInt_t itr=0;itr < fNDCTracks;itr++) {
      printf("%s %5d \n","Hit info for track number = ",itr+1);
      printf("%5s %-15s %-15s %-15s \n","Plane","WIRE_COORD","Fit postiion","Residual");
      THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(itr));
      for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	THcDCHit* hit=theDCTrack->GetHit(ihit);
	Int_t plane=hit->GetPlaneNum()-1;
	Double_t coords_temp;
	if(fFixLR) {
	  if(fFixPropagationCorrection) {
	    coords_temp = hit->GetPos()
	      + theDCTrack->GetHitLR(ihit)*theDCTrack->GetHitDist(ihit);
	  } else {
	    coords_temp = hit->GetPos()
	      + theDCTrack->GetHitLR(ihit)*hit->GetDist();
	  }
	} else {
	  if(fFixPropagationCorrection) {
	    coords_temp = hit->GetPos()
	      + hit->GetLR()*theDCTrack->GetHitDist(ihit);
	  } else {
	    coords_temp = hit->GetCoord();
	  }
	}
	printf("%-5d %15.7e %15.7e %15.7e \n",plane+1,coords_temp,theDCTrack->GetCoord(plane),theDCTrack->GetResidual(plane));
      }
    }
  }

  //
}
//
//
Double_t THcDC::DpsiFun(Double_t ray[4], Int_t plane)
{
  /**
    this function calculates the psi coordinate of the intersection
    of a ray (defined by ray) with a hms wire chamber plane. the geometry
    of the plane is contained in the coeff array calculated in the
    array hplane_coeff
    Note it is call by MINUIT via H_FCNCHISQ and so uses double precision
    variables

    the ray is defined by
    x = (z-zt)*tan(xp) + xt
    y = (z-zt)*tan(yp) + yt
     at some fixed value of zt*
    ray(1) = xt
    ray(2) = yt
    ray(3) = tan(xp)
    ray(4) = tan(yp)
  */

  Double_t infinity = 1.0E+20;
  Double_t cinfinity = 1/infinity;
  Double_t DpsiFun =
    ray[2]*ray[1]*fPlaneCoeffs[plane][0] +
    ray[3]*ray[0]*fPlaneCoeffs[plane][1] +
    ray[2]*fPlaneCoeffs[plane][2] +
    ray[3]*fPlaneCoeffs[plane][3] +
    ray[0]*fPlaneCoeffs[plane][4] +
    ray[1]*fPlaneCoeffs[plane][5];
  Double_t denom = ray[2]*fPlaneCoeffs[plane][6]
    + ray[3]*fPlaneCoeffs[plane][7]
    + fPlaneCoeffs[plane][8];
  if(TMath::Abs(denom) < cinfinity) {
    DpsiFun = infinity;
  } else {
    DpsiFun = DpsiFun/denom;
  }
  return(DpsiFun);
}

//_____________________________________________________________________________
Int_t THcDC::End(THaRunBase* run)
{
  //  EffCalc();
  MissReport(Form("%s.%s", GetApparatus()->GetName(), GetName()));
  return 0;
}

//_____________________________________________________________________________
void THcDC::EffInit()
{
  /**
     Create, and initialize counters used to calculate
     efficiencies.  Register the counters in gHcParms so that the
     variables can be used in end of run reports.
  */

  delete [] fNChamHits; fNChamHits = new Int_t [fNChambers];
  delete [] fPlaneEvents; fPlaneEvents = new Int_t [fNPlanes];

  fTotEvents = 0;
  for(UInt_t i=0;i<fNChambers;i++) {
    fNChamHits[i] = 0;
  }
  for(Int_t i=0;i<fNPlanes;i++) {
    fPlaneEvents[i] = 0;
  }
  gHcParms->Define(Form("%sdc_tot_events",fPrefix),"Total DC Events",fTotEvents);
  gHcParms->Define(Form("%sdc_cham_hits[%d]",fPrefix,fNChambers),"N events with hits per chamber",*fNChamHits);
  gHcParms->Define(Form("%sdc_events[%d]",fPrefix,fNPlanes),"N events with hits per plane",*fPlaneEvents);
}

//_____________________________________________________________________________
void THcDC::Eff()
{
  /**
     Accumulate statistics for efficiency calculations
  */

  fTotEvents++;
  for(UInt_t i=0;i<fNChambers;i++) {
    if(fChambers[i]->GetNHits()>0) fNChamHits[i]++;
  }
  for(Int_t i=0;i<fNPlanes;i++) {
    if(fPlanes[i]->GetNHits() > 0) fPlaneEvents[i]++;
  }
  return;
}

ClassImp(THcDC)
////////////////////////////////////////////////////////////////////////////////
