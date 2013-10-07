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
#include "THcDCTrack.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TVectorD.h"

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

  fXCenter = NULL;
  fYCenter = NULL;
  fMinHits = NULL;
  fMaxHits = NULL;
  fMinCombos = NULL;
  fSpace_Point_Criterion2 = NULL;

  fTdcWinMin = NULL;
  fTdcWinMax = NULL;
  fCentralTime = NULL;
  fNWires = NULL;
  fNChamber = NULL;
  fWireOrder = NULL;
  fDriftTimeSign = NULL;

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

  fDCTracks = new TClonesArray( "THcDCTrack", 20 );
}

//_____________________________________________________________________________
void THcDC::Setup(const char* name, const char* description)
{

  static const char* const here = "Setup";
  char prefix[2];

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
  fPlanes.clear();

  for(Int_t i=0;i<fNPlanes;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);

    THcDriftChamberPlane* newplane = new THcDriftChamberPlane(fPlaneNames[i], desc, i+1, this);
    if( !newplane or newplane->IsZombie() ) {
      Error( Here(here), "Error creating Drift Chamber plane %s. Call expert.", name);
      MakeZombie();
      return;
    }
    fPlanes.push_back(newplane);
    newplane->SetDebug(fDebug);
    cout << "Created Drift Chamber Plane " << fPlaneNames[i] << ", " << desc << endl;

  }

  fChambers.clear();
  for(Int_t i=0;i<fNChambers;i++) {
    sprintf(desc,"%s Chamber %d",description, i+1);

    // Should construct a better chamber name
    THcDriftChamber* newchamber = new THcDriftChamber(desc, desc, i+1, this);
    fChambers.push_back(newchamber);
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
  // Retrieve the fiting coefficients
  fPlaneCoeffs = new Double_t* [fNPlanes];
  for(Int_t ip=0; ip<fNPlanes;ip++) {
    fPlaneCoeffs[ip] = fPlanes[ip]->GetPlaneCoef();
  }

  fResiduals = new Double_t [fNPlanes];

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

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  prefix[0]=tolower(GetApparatus()->GetName()[0]);

  prefix[1]='\0';

  delete [] fXCenter;  fXCenter = new Double_t [fNChambers];
  delete [] fYCenter;  fYCenter = new Double_t [fNChambers];
  delete [] fMinHits;  fMinHits = new Int_t [fNChambers];
  delete [] fMaxHits;  fMaxHits = new Int_t [fNChambers];
  delete [] fMinCombos;  fMinCombos = new Int_t [fNChambers];
  delete [] fSpace_Point_Criterion2;  fSpace_Point_Criterion2 = new Double_t [fNChambers];

  delete [] fTdcWinMin;  fTdcWinMin = new Int_t [fNPlanes];
  delete [] fTdcWinMax;  fTdcWinMax = new Int_t [fNPlanes];
  delete [] fCentralTime;  fCentralTime = new Int_t [fNPlanes];
  delete [] fNWires;  fNWires = new Int_t [fNPlanes];
  delete [] fNChamber;  fNChamber = new Int_t [fNPlanes]; // Which chamber is this plane
  delete [] fWireOrder;  fWireOrder = new Int_t [fNPlanes]; // Wire readout order
  delete [] fDriftTimeSign;  fDriftTimeSign = new Int_t [fNPlanes];

  delete [] fZPos;  fZPos = new Double_t [fNPlanes];
  delete [] fAlphaAngle;  fAlphaAngle = new Double_t [fNPlanes];
  delete [] fBetaAngle;  fBetaAngle = new Double_t [fNPlanes];
  delete [] fGammaAngle;  fGammaAngle = new Double_t [fNPlanes];
  delete [] fPitch;  fPitch = new Double_t [fNPlanes];
  delete [] fCentralWire;  fCentralWire = new Double_t [fNPlanes];
  delete [] fPlaneTimeZero;  fPlaneTimeZero = new Double_t [fNPlanes];
  delete [] fSigma;  fSigma = new Double_t [fNPlanes];

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
    {"dc_sigma", fSigma, kDouble, fNPlanes},
    {"single_stub",&fSingleStub, kInt},
    {"ntracks_max_fp", &fNTracksMaxFP, kInt},
    {"xt_track_criterion", &fXtTrCriterion, kDouble},
    {"yt_track_criterion", &fYtTrCriterion, kDouble},
    {"xpt_track_criterion", &fXptTrCriterion, kDouble},
    {"ypt_track_criterion", &fYptTrCriterion, kDouble},
    {"dc_fix_lr", &fFixLR, kInt},
    {"dc_fix_propcorr", &fFixPropagationCorrection, kInt},
    {"debuglinkstubs", &fdebuglinkstubs, kInt},
    {"debugprintdecodeddc", &fdebugprintdecodeddc, kInt},
    {"debugtrackprint", &fdebugtrackprint , kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
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
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "nhit", "Number of DC hits",  "fNhits" },
    { "ntrack", "Number of Tracks", "fNDCTracks" },
    { "x", "X at focal plane", "fDCTracks.THcDCTrack.GetX()"},
    { "y", "Y at focal plane", "fDCTracks.THcDCTrack.GetY()"},
    { "xp", "YP at focal plane", "fDCTracks.THcDCTrack.GetXP()"},
    { "yp", "YP at focal plane", "fDCTracks.THcDCTrack.GetYP()"},
    { "residual", "Residuals", "fResiduals"},
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
       ip != fPlanes.end(); ip++) delete *ip;
  // Delete the chamber objects
  for (vector<THcDriftChamber*>::iterator ip = fChambers.begin();
       ip != fChambers.end(); ip++) delete *ip;

  if (fTrackProj) {
    fTrackProj->Clear();
    delete fTrackProj; fTrackProj = 0;
  }
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
  delete [] fSpace_Point_Criterion2;   fSpace_Point_Criterion2 = NULL;

  delete [] fTdcWinMin;   fTdcWinMin = NULL;
  delete [] fTdcWinMax;   fTdcWinMax = NULL;
  delete [] fCentralTime;   fCentralTime = NULL;
  delete [] fNWires;   fNWires = NULL;
  delete [] fNChamber;   fNChamber = NULL;
  delete [] fWireOrder;   fWireOrder = NULL;
  delete [] fDriftTimeSign;   fDriftTimeSign = NULL;

  delete [] fZPos;   fZPos = NULL;
  delete [] fAlphaAngle;   fAlphaAngle = NULL;
  delete [] fBetaAngle;   fBetaAngle = NULL;
  delete [] fGammaAngle;   fGammaAngle = NULL;
  delete [] fPitch;   fPitch = NULL;
  delete [] fCentralWire;   fCentralWire = NULL;
  delete [] fPlaneTimeZero;   fPlaneTimeZero = NULL;
  delete [] fSigma;   fSigma = NULL;

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

  for(Int_t i=0;i<fNPlanes;i++) {
    fResiduals[i] = 1000.0;
  }
  
  //  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcDC::Decode( const THaEvData& evdata )
{

  ClearEvent();
  Int_t num_event = evdata.GetEvNum();
  if (fdebugprintdecodeddc || fdebuglinkstubs || fdebugtrackprint) cout << " event num = " << num_event << endl;
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
  // fRawHitList is TClones array of THcRawDCHit objects
  if (fdebugprintdecodeddc) {
    cout << " Hit #  " << "Plane  " << " Wire " <<  " Raw TDC " << endl; 
    for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcRawDCHit* hit = (THcRawDCHit *) fRawHitList->At(ihit);
      for(Int_t imhit = 0; imhit < hit->fNHits; imhit++) {
    cout << ihit+imhit+1 << "  " << hit->fPlane << "  " << hit->fCounter << "  " << hit->fTDC[imhit]	   << endl;
      }
  }
    cout << endl;
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
  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  //  static const Double_t sqrt2 = TMath::Sqrt(2.);
  for(Int_t i=0;i<fNChambers;i++) {
    fChambers[i]->FindSpacePoints();
    fChambers[i]->CorrectHitTimes();
    fChambers[i]->LeftRight();
  }
  // Now link the stubs between chambers
  LinkStubs();
  if(fNDCTracks > 0) {
    TrackFit();
    // Copy tracks into podd tracks list
    for(Int_t itrack=0;itrack<fNDCTracks;itrack++) {
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
    }
  }

  // Check for internal TrackFit errors
  // Histogram the focal plane tracks
  // Histograms made in h_fill_dc_fp_hist
  //   The following are one hist per track
  //      x_fp
  //      y_fp
  //      xp_fp
  //      yp_fp
  //      log chi2
  //      reduced chi2
  //      For each plane:
  //         double residual
  //         single residual
  // Will need to make a track class that has all these things.   Need to
  // move the structure out of THcDC into it's own class which should probably
  // inherit from a podd track class

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
void THcDC::LinkStubs()
{
  //     The logic is
  //                  0) Put all space points in a single list
  //                  1) loop over all space points as seeds  isp1
  //                  2) Check if this space point is all ready in a track
  //                  3) loop over all succeeding space pointss   isp2
  //                  4) check if there is a track-criterion match
  //                       either add to existing track
  //                       or if there is another point in same chamber
  //                          make a copy containing isp2 rather than 
  //                            other point in same chamber
  //                  5) If hsingle_stub is set, make a track of all single
  //                     stubs.

  std::vector<THcSpacePoint*> fSp;
  Int_t fNSp=0;
  fSp.clear();
  fSp.reserve(10);
  // Make a vector of pointers to the SpacePoints
  if (fdebuglinkstubs) cout << "Linking stubs Ch 1 has " << fChambers[0]->GetNSpacePoints()
       << " sp pts and ch 2 has " << fChambers[1]->GetNSpacePoints() << " sp pts" << endl;
  for(Int_t ich=0;ich<fNChambers;ich++) {
    Int_t nchamber=fChambers[ich]->GetChamberNum();
    TClonesArray* spacepointarray = fChambers[ich]->GetSpacePointsP();
    for(Int_t isp=0;isp<fChambers[ich]->GetNSpacePoints();isp++) {
      if (fdebuglinkstubs) cout << " Chamber = " << nchamber << " number of space pts = " << fNSp << endl;
      fSp.push_back(static_cast<THcSpacePoint*>(spacepointarray->At(isp)));
      fSp[fNSp]->fNChamber = nchamber;
      fNSp++;
    }
  }
  fNDCTracks=0;		// Number of Focal Plane tracks found
  fDCTracks->Clear("C");
  Double_t stubminx = 999999;
  Double_t stubminy = 999999;
  Double_t stubminxp = 999999;
  Double_t stubminyp = 999999;
  Int_t stub_tracks[MAXTRACKS];
  if (fdebuglinkstubs) cout << "Fsinglestub (no = 0) = " << fSingleStub << endl;
  if (fdebuglinkstubs) cout << "Joined space points = " << fNSp-1 << endl;
  if(!fSingleStub) {
    for(Int_t isp1=0;isp1<fNSp-1;isp1++) { // isp1 is index/id in total list of space points
      THcSpacePoint* sp1 = fSp[isp1];
      if (fdebuglinkstubs) cout << "Loop thru joined space points " << isp1+1<< endl;
      Int_t sptracks=0;
      // Now make sure this sp is not already used in a sp.
      // Could this be done by having a sp point to the track it is in?
      Int_t tryflag=1;
      for(Int_t itrack=0;itrack<fNDCTracks;itrack++) {
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
          if (fdebuglinkstubs) cout << "second Loop space points " << isp2<< endl;
	  if(sp1->fNChamber!=sp2->fNChamber) {
	    Double_t *spstub1=sp1->GetStubP();
	    Double_t *spstub2=sp2->GetStubP();
	    Double_t dposx = spstub1[0] - spstub2[0];
	    Double_t dposy = spstub1[1] - spstub2[1];
	    Double_t dposxp = spstub1[2] - spstub2[2];
	    Double_t dposyp = spstub1[3] - spstub2[3];
	      
	    // What is the point of saving these stubmin values.  They
	    // Don't seem to be used anywhere except that they can be
	    // printed out if hbypass_track_eff_files is zero.
	    if(TMath::Abs(dposx)<TMath::Abs(stubminx)) stubminx = dposx;
	    if(TMath::Abs(dposy)<TMath::Abs(stubminy)) stubminy = dposy;
	    if(TMath::Abs(dposxp)<TMath::Abs(stubminxp)) stubminxp = dposxp;
	    if(TMath::Abs(dposyp)<TMath::Abs(stubminyp)) stubminyp = dposyp;
	    if (fdebuglinkstubs) printf("dposx = %f ,dposy = %f,dposxp = %f,dposyp = %f \n",dposx,dposy,dposxp,dposyp);
	      
	    // if hbypass_track_eff_files == 0 then
	    // Print out each stubminX that is less that its criterion

	    if((TMath::Abs(dposx) < fXtTrCriterion)
	       && (TMath::Abs(dposy) < fYtTrCriterion)
	       && (TMath::Abs(dposxp) < fXptTrCriterion)
	       && (TMath::Abs(dposyp) < fYptTrCriterion)) {
	      if(newtrack) {
		if (fdebuglinkstubs) cout << " new track" << endl;
		assert(sptracks==0);
		//stubtest=1;  Used in h_track_tests.f
		// Make a new track if there are not to many
		if(fNDCTracks < MAXTRACKS) {
		  sptracks=0; // Number of tracks with this seed
		  stub_tracks[sptracks++] = fNDCTracks;
		  THcDCTrack *theDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
		  theDCTrack->AddSpacePoint(sp1);
		  theDCTrack->AddSpacePoint(sp2);
		  if (fdebuglinkstubs) cout << " # sp pts combined = " << theDCTrack->GetNSpacePoints() << endl;
		  if (fdebuglinkstubs) cout << " combine sp = " << isp1 << " and " << isp2 << endl;
		  // Now save the X, Y and XP for the two stubs
		  // in arrays hx_sp1, hy_sp1, hy_sp1, ... hxp_sp2
		  // Why not also YP?
		  // Skip for here.  May be a diagnostic thing
		  newtrack = 0; // Make no more tracks in this loop
		  // (But could replace a SP?)
		} else {
		  if (fdebuglinkstubs) cout << "EPIC FAIL 1:  Too many tracks found in THcDC::LinkStubs" << endl;
		  fNDCTracks=0;
		  // Do something here to fail this event
		  return;
		}
	      } else {
		if (fdebuglinkstubs) cout << " check if another space point in same chamber sptracks= " << sptracks << endl;

		// Check if there is another space point in the same chamber
		for(Int_t itrack=0;itrack<sptracks;itrack++) {
		  Int_t track=stub_tracks[itrack];
		  THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(track));

		  Int_t spoint=-1;
		  Int_t duppoint=0;
		  if (fdebuglinkstubs) cout << "checking abother sp pt in cham track = " << itrack+1 << " with # sp pts = " << theDCTrack->GetNSpacePoints() << endl;
		  for(Int_t isp=0;isp<theDCTrack->GetNSpacePoints();isp++) {
		    // isp is index of space points in theDCTrack
		    if (fdebuglinkstubs) cout << "looping of previous track = " << isp+1 << endl;
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
		    } else {
		      // If there is another point in the same chamber
		      // in this track create a new track with all the
		      // same space points except spoint
		      if(fNDCTracks < MAXTRACKS) {
                        if (fdebuglinkstubs) cout << "found another track, presently # of tracks = " << fNDCTracks << endl;
			stub_tracks[sptracks++] = fNDCTracks;
			THcDCTrack *newDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
                        if (fdebuglinkstubs) cout << "loop over theDCtrack # of sp tps = " << theDCTrack->GetNSpacePoints() << endl;
			for(Int_t isp=0;isp<theDCTrack->GetNSpacePoints();isp++) {
			  if(isp!=spoint) {
			    newDCTrack->AddSpacePoint(theDCTrack->GetSpacePoint(isp));
			  } else {
			    newDCTrack->AddSpacePoint(sp2);
			  } // End check for dup on copy
                        if (fdebuglinkstubs) cout << "newDCtrack # of sp tps = " << newDCTrack->GetNSpacePoints() << endl;
			} // End copy of track
		      } else {
			if (fdebuglinkstubs) cout << "EPIC FAIL 2:  Too many tracks found in THcDC::LinkStubs" << endl;
			fNDCTracks=0;
			// Do something here to fail this event
			return; // Max # of allowed tracks
		      }
		    } // end if on same chamber
		  } // end if on duplicate point
		} // end for over tracks with isp1
	      }
	    }
	  } // end test on same chamber
	} // end isp2 loop over new space points
      } // end test on tryflag
    } // end isp1 outer loop over space points
   } else { // Make track out of each single space point
    for(Int_t isp=0;isp<fNSp;isp++) {
      if(fNDCTracks<MAXTRACKS) {
	// Need some constructed t thingy
	THcDCTrack *newDCTrack = new( (*fDCTracks)[fNDCTracks++]) THcDCTrack(fNPlanes);
	newDCTrack->AddSpacePoint(fSp[isp]);
      } else {
	if (fdebuglinkstubs) cout << "EPIC FAIL 3:  Too many tracks found in THcDC::LinkStubs" << endl;
	fNDCTracks=0;
	// Do something here to fail this event
	return; // Max # of allowed tracks
      }
    }
  }
  ///
  if (fdebuglinkstubs) cout << " End Linkstubs Found " << fNDCTracks << " tracks"<<endl;
}

// Primary track fitting routine
void THcDC::TrackFit()
{

  // Number of ray parameters in focal plane.
  const Int_t raycoeffmap[]={4,5,2,3};

  // Initialize residuals
  // Need to make these member variables so they can be histogrammed
  // Probably an array of vectors.
  Double_t double_resolution[fNPlanes][fNDCTracks];
  Double_t single_resolution[fNPlanes][fNDCTracks];
  Double_t double_res[fNPlanes]; // For the good track

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    double_res[ip] = 1000.0;
    for(Int_t itrack=0;itrack<fNDCTracks;itrack++) {
      double_resolution[ip][itrack] = 1000.0;
      single_resolution[ip][itrack] = 1000.0;
    }
  }
  
  Double_t dummychi2 = 1.0E4;

  for(Int_t itrack=0;itrack<fNDCTracks;itrack++) {
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
    }

    theDCTrack->SetNFree(theDCTrack->GetNHits() - NUM_FPRAY);
    Double_t chi2 = dummychi2;
    if(theDCTrack->GetNFree() > 0) {
      TVectorD TT(NUM_FPRAY);
      TMatrixD AA(NUM_FPRAY,NUM_FPRAY);
      for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	TT[irayp] = 0.0;
	for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	  TT[irayp] += (coords[ihit]*
			fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]])
	    /pow(fSigma[planes[ihit]],2);
	}
      }
      for(Int_t irayp=0;irayp<NUM_FPRAY;irayp++) {
	for(Int_t jrayp=0;jrayp<NUM_FPRAY;jrayp++) {
	  AA[irayp][jrayp] = 0.0;
	  if(jrayp<irayp) { // Symmetric
	    AA[irayp][jrayp] = AA[jrayp][irayp];
	  } else {
	    for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	      AA[irayp][jrayp] += fPlaneCoeffs[planes[ihit]][raycoeffmap[irayp]]*
		fPlaneCoeffs[planes[ihit]][raycoeffmap[jrayp]]/
		pow(fSigma[planes[ihit]],2);
	    }
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
	}
	theDCTrack->SetCoord(iplane,coord);
      }
      // Compute Chi2 and residuals
      chi2 = 0.0;
      for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	Double_t residual = coords[ihit] - theDCTrack->GetCoord(planes[ihit]);
	theDCTrack->SetResidual(planes[ihit], residual);
	chi2 += pow(residual/fSigma[planes[ihit]],2);
      }
      if (fdebugtrackprint) {
        cout << "Hit     HDC_WIRE_COORD  Fit postiion  Residual " << endl;
	for(Int_t ihit=0;ihit < theDCTrack->GetNHits();ihit++) {
	  THcDCHit* hit=theDCTrack->GetHit(ihit);
	  Int_t plane=hit->GetPlaneNum()-1;
	  cout << ihit+1 << "   " << coords[ihit] << "     " << theDCTrack->GetCoord(plane) << "     " << theDCTrack->GetResidual(plane) << endl;
	}
      }
      theDCTrack->SetVector(dray[0], dray[1], 0.0, dray[2], dray[3]);
    }
    theDCTrack->SetChisq(chi2);
  }

  // Calculate residuals for each chamber if in single stub mode
  // and there was a track found in each chamber
  // Specific for two chambers.  Can/should it be generalized?

  if(fSingleStub != 0) {
    if(fNDCTracks == 2) {
      THcDCTrack *theDCTrack1 = static_cast<THcDCTrack*>( fDCTracks->At(0));
      THcDCTrack *theDCTrack2 = static_cast<THcDCTrack*>( fDCTracks->At(1));
      Int_t itrack=0;
      Int_t ihit=0;
      THcDCHit* hit=theDCTrack1->GetHit(ihit);
      Int_t plane=hit->GetPlaneNum()-1;
      Int_t chamber=fNChamber[plane];
      if(chamber==1) {
	itrack=1;
	hit=theDCTrack2->GetHit(ihit);
	plane=hit->GetPlaneNum()-1;
	chamber=fNChamber[plane];
	if(chamber==2) {
	  Double_t ray1[4];
	  Double_t ray2[4];
	  theDCTrack1->GetRay(ray1);
	  theDCTrack2->GetRay(ray2);
	  itrack = 1;
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
	  itrack=0;
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
  if(fNDCTracks>0) {
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      THcDCTrack *theDCTrack = static_cast<THcDCTrack*>( fDCTracks->At(0));
      fResiduals[ip] = theDCTrack->GetResidual(ip);
    }
  }
  // print tracks if hdebugtrackprint is on
}
Double_t THcDC::DpsiFun(Double_t ray[4], Int_t plane)
{
  /*
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

ClassImp(THcDC)
////////////////////////////////////////////////////////////////////////////////
