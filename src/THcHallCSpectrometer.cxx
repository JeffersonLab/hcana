//*-- Author :    Stephen Wood 20-Apr-2012

//////////////////////////////////////////////////////////////////////////
//
// THcHallCSpectrometer
//
// A standard Hall C spectrometer.
// Contains no standard detectors,
//  May add hodoscope
//
// The usual name of this object is either "H", "S", or "P"
// for HMS, SOS, or suPerHMS respectively
//
// Defines the functions FindVertices() and TrackCalc(), which are common
// to both the LeftHRS and the RightHRS.
//
// Special configurations of the HRS (e.g. more detectors, different 
// detectors) can be supported in on e of three ways:
//
//   1. Use the AddDetector() method to include a new detector
//      in this apparatus.  The detector will be decoded properly,
//      and its variables will be available for cuts and histograms.
//      Its processing methods will also be called by the generic Reconstruct()
//      algorithm implemented in THaSpectrometer::Reconstruct() and should
//      be correctly handled if the detector class follows the standard 
//      interface design.
//
//   2. Write a derived class that creates the detector in the
//      constructor.  Write a new Reconstruct() method or extend the existing
//      one if necessary.
//
//   3. Write a new class inheriting from THaSpectrometer, using this
//      class as an example.  This is appropriate if your HRS 
//      configuration has fewer or different detectors than the 
//      standard HRS. (It might not be sensible to provide a RemoveDetector() 
//      method since Reconstruct() relies on the presence of the 
//      standard detectors to some extent.)
//
//  For timing calculations, S1 is treated as the scintillator at the
//  'reference distance', corresponding to the pathlength correction
//  matrix.
//
//
//  Golden track using scin. Zafar Ahmed. August 19 2014
//      Goldent track is moved to THcHallCSpectrometer::TrackCalc()
//      if  fSelUsingScin == 0 then golden track is calculated just 
//      like podd. i.e. it is the first track with minimum chi2/ndf 
//      with sorting ON
//
//      if fSelUsingScin == 1 then golden track is calculetd just like
//      engine/HTRACKING/h_select_best_track_using_scin.h. This method 
//      gives the best track with minimum value of chi2/ndf but with 
//      additional cuts on the tracks. These cuts are on dedx, beta 
//      and on energy.
//
//  Golden track using prune. Zafar Ahmed. September 23 2014
//      Selection of golden track using prune method is added.
//      A bug is also fixed in THcHodoscope class
//      Number of pmts hits, focal plane time, good time for plane 4 
//      and good time for plane 3 are set to the tracks in 
//      THcHodoscope class.
//
//////////////////////////////////////////////////////////////////////////

#include "THcHallCSpectrometer.h"
#include "THaTrackingDetector.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THaTrack.h"
#include "THaTrackProj.h"
#include "THaTriggerTime.h"
#include "TMath.h"
#include "TList.h"

#include "THcRawShowerHit.h"
#include "THcSignalHit.h"
#include "THcShower.h"
#include "THcHitList.h"
#include "THcHodoscope.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using std::vector;
using namespace std;

//_____________________________________________________________________________
THcHallCSpectrometer::THcHallCSpectrometer( const char* name, const char* description ) :
  THaSpectrometer( name, description )
{
  // Constructor. Defines the standard detectors for the HRS.
  //  AddDetector( new THaTriggerTime("trg","Trigger-based time offset"));

  //sc_ref = static_cast<THaScintillator*>(GetDetector("s1"));

  SetTrSorting(kTRUE);
}

//_____________________________________________________________________________
THcHallCSpectrometer::~THcHallCSpectrometer()
{
  // Destructor

  DefineVariables( kDelete );
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::DefineVariables( EMode mode )
{
  // Define/delete standard variables for a spectrometer (tracks etc.)
  // Can be overridden or extended by derived (actual) apparatuses
  if( mode == kDefine && fIsSetup ) return kOK;
  THaSpectrometer::DefineVariables( mode );
  fIsSetup = ( mode == kDefine );
  RVarDef vars[] = {
    { "tr.betachisq", "Chi2 of beta", "fTracks.THaTrack.GetBetaChi2()"},
    { 0 }
  };

  
  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
Bool_t THcHallCSpectrometer::SetTrSorting( Bool_t set )
{
  if( set )
    fProperties |= kSortTracks;
  else
    fProperties &= ~kSortTracks;

  return set;
}

//_____________________________________________________________________________
Bool_t THcHallCSpectrometer::GetTrSorting() const
{
  return ((fProperties & kSortTracks) != 0);
}
 
//_____________________________________________________________________________
void THcHallCSpectrometer::InitializeReconstruction()
{
  fNReconTerms = 0;
  fReconTerms.clear();
  fAngSlope_x = 0.0;
  fAngSlope_y = 0.0;
  fAngOffset_x = 0.0;
  fAngOffset_y = 0.0;
  fDetOffset_x = 0.0;
  fDetOffset_y = 0.0;
  fZTrueFocus = 0.0;
}
//_____________________________________________________________________________
Int_t THcHallCSpectrometer::ReadDatabase( const TDatime& date )
{

  static const char* const here = "THcHallCSpectrometer::ReadDatabase";

#ifdef WITH_DEBUG
  cout << "In THcHallCSpectrometer::ReadDatabase()" << endl;
#endif

  // --------------- To get energy from THcShower ----------------------

  const char* detector_name = "hod";  
  //THaApparatus* app = GetDetector();
  THaDetector* det = GetDetector("hod");
  // THaDetector* det = app->GetDetector( shower_detector_name );
  
  if( dynamic_cast<THcHodoscope*>(det) ) {
    fHodo = static_cast<THcHodoscope*>(det);     // fHodo is a membervariable
  } else {
    Error("THcHallCSpectrometer", "Cannot find hodoscope detector %s",
    	  detector_name );
    fHodo = NULL;
  }
  

  // fShower = static_cast<THcShower*>(det);     // fShower is a membervariable
  
  // --------------- To get energy from THcShower ----------------------


  // Get the matrix element filename from the variable store
  // Read in the matrix

  InitializeReconstruction();

  char prefix[2];

  cout << " GetName() " << GetName() << endl;

  prefix[0]=tolower(GetName()[0]);
  prefix[1]='\0';

  string reconCoeffFilename;
  DBRequest list[]={
    {"_recon_coeff_filename", &reconCoeffFilename,     kString               },
    {"theta_offset",          &fThetaOffset,           kDouble               },
    {"phi_offset",            &fPhiOffset,             kDouble               },
    {"delta_offset",          &fDeltaOffset,           kDouble               },
    {"thetacentral_offset",   &fThetaCentralOffset,    kDouble               },
    {"_oopcentral_offset",    &fOopCentralOffset,      kDouble               },
    {"pcentral_offset",       &fPCentralOffset,        kDouble               },
    {"pcentral",              &fPcentral,              kDouble               },
    {"theta_lab",             &fTheta_lab,             kDouble               },
    {"partmass",              &fPartMass,              kDouble               },
    {"sel_using_scin",        &fSelUsingScin,          kInt,            0,  1},
    {"sel_using_prune",       &fSelUsingPrune,         kInt,            0,  1},
    {"sel_ndegreesmin",       &fSelNDegreesMin,        kDouble,         0,  1},
    {"sel_dedx1min",          &fSeldEdX1Min,           kDouble,         0,  1},
    {"sel_dedx1max",          &fSeldEdX1Max,           kDouble,         0,  1},
    {"sel_betamin",           &fSelBetaMin,            kDouble,         0,  1},
    {"sel_betamax",           &fSelBetaMax,            kDouble,         0,  1},
    {"sel_etmin",             &fSelEtMin,              kDouble,         0,  1},
    {"sel_etmax",             &fSelEtMax,              kDouble,         0,  1},
    {"hodo_num_planes",       &fNPlanes,               kInt                  },
    {"scin_2x_zpos",          &fScin2XZpos,            kDouble,         0,  1},
    {"scin_2x_dzpos",         &fScin2XdZpos,           kDouble,         0,  1},
    {"scin_2y_zpos",          &fScin2YZpos,            kDouble,         0,  1},
    {"scin_2y_dzpos",         &fScin2YdZpos,           kDouble,         0,  1},
    {"prune_xp",              &fPruneXp,               kDouble,         0,  1},
    {"prune_yp",              &fPruneYp,               kDouble,         0,  1},
    {"prune_ytar",            &fPruneYtar,             kDouble,         0,  1},
    {"prune_delta",           &fPruneDelta,            kDouble,         0,  1},
    {"prune_beta",            &fPruneBeta,             kDouble,         0,  1},
    {"prune_df",              &fPruneDf,               kDouble,         0,  1},
    {"prune_chibeta",         &fPruneChiBeta,          kDouble,         0,  1},
    {"prune_npmt",            &fPruneNPMT,           kDouble,         0,  1},
    {"prune_fptime",          &fPruneFpTime,             kDouble,         0,  1},
    {0}
  };

  // Default values
  fSelUsingScin = 0;
  fSelUsingPrune = 0;

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  EnforcePruneLimits();
  
  cout <<  "\n\n\nhodo planes = " << fNPlanes << endl;
  cout <<  "sel using scin = "    << fSelUsingScin << endl;
  cout <<  "fPruneXp = "          <<  fPruneXp << endl; 
  cout <<  "fPruneYp = "          <<  fPruneYp << endl; 
  cout <<  "fPruneYtar = "        <<  fPruneYtar << endl; 
  cout <<  "fPruneDelta = "       <<  fPruneDelta << endl; 
  cout <<  "fPruneBeta = "        <<  fPruneBeta << endl; 
  cout <<  "fPruneDf = "          <<  fPruneDf << endl; 
  cout <<  "fPruneChiBeta = "     <<  fPruneChiBeta << endl; 
  cout <<  "fPruneFpTime = "      <<  fPruneFpTime << endl; 
  cout <<  "fPruneNPMT = "        <<  fPruneNPMT << endl; 
  cout <<  "sel using prune = "   <<  fSelUsingPrune << endl;
  cout <<  "fPartMass = "         <<  fPartMass << endl;
  cout <<  "fPcentral = "         <<  fPcentral << " " <<fPCentralOffset << endl; 
  cout <<  "fThate_lab = "        <<  fTheta_lab << " " <<fThetaCentralOffset << endl; 
  fPcentral= fPcentral*(1.+fPCentralOffset/100.);
  // Check that these offsets are in radians
  fTheta_lab=fTheta_lab + fThetaCentralOffset*TMath::RadToDeg();
  Double_t ph = 0.0+fPhiOffset*TMath::RadToDeg();

  SetCentralAngles(fTheta_lab, ph, false);
  Double_t off_x = 0.0, off_y = 0.0, off_z = 0.0;
  fPointingOffset.SetXYZ( off_x, off_y, off_z );
  
  //
  ifstream ifile;
  ifile.open(reconCoeffFilename.c_str());
  if(!ifile.is_open()) {
    Error(here, "error opening reconstruction coefficient file %s",reconCoeffFilename.c_str());
    //    return kInitError; // Is this the right return code?
    return kOK;
  }
  
  string line="!";
  int good=1;
  while(good && line[0]=='!') {
    good = getline(ifile,line).good();
    //    cout << line << endl;
  }
  // Read in focal plane rotation coefficients
  // Probably not used, so for now, just paste in fortran code as a comment
  while(good && line.compare(0,4," ---")!=0) {
    //  if(line(1:13).eq.'h_ang_slope_x')read(line,1201,err=94)h_ang_slope_x
    //  if(line(1:13).eq.'h_ang_slope_y')read(line,1201,err=94)h_ang_slope_y
    //  if(line(1:14).eq.'h_ang_offset_x')read(line,1201,err=94)h_ang_offset_x
    //  if(line(1:14).eq.'h_ang_offset_y')read(line,1201,err=94)h_ang_offset_y
    //  if(line(1:14).eq.'h_det_offset_x')read(line,1201,err=94)h_det_offset_x
    //  if(line(1:14).eq.'h_det_offset_y')read(line,1201,err=94)h_det_offset_y
    //  if(line(1:14).eq.'h_z_true_focus')read(line,1201,err=94)h_z_true_focus
    good = getline(ifile,line).good();
  }
  // Read in reconstruction coefficients and exponents
  line=" ";
  good = getline(ifile,line).good();
  //  cout << line << endl;
  fNReconTerms = 0;
  fReconTerms.clear();
  fReconTerms.reserve(500);
  //cout << "Reading matrix elements" << endl;
  while(good && line.compare(0,4," ---")!=0) {
    fReconTerms.push_back(reconTerm());
    sscanf(line.c_str()," %le %le %le %le %1d%1d%1d%1d%1d"
	   ,&fReconTerms[fNReconTerms].Coeff[0],&fReconTerms[fNReconTerms].Coeff[1]
	   ,&fReconTerms[fNReconTerms].Coeff[2],&fReconTerms[fNReconTerms].Coeff[3]
	   ,&fReconTerms[fNReconTerms].Exp[0]
	   ,&fReconTerms[fNReconTerms].Exp[1]
	   ,&fReconTerms[fNReconTerms].Exp[2]
	   ,&fReconTerms[fNReconTerms].Exp[3]
	   ,&fReconTerms[fNReconTerms].Exp[4]);
    fNReconTerms++;
    good = getline(ifile,line).good();    
  }
  cout << "Read " << fNReconTerms << " matrix element terms"  << endl;
  if(!good) {
    Error(here, "Error processing reconstruction coefficient file %s",reconCoeffFilename.c_str());
    return kInitError; // Is this the right return code?
  }
  return kOK;
}

//_____________________________________________________________________________
void THcHallCSpectrometer::EnforcePruneLimits()
{
  // Enforce minimum values for the prune cuts
  
  fPruneXp      = TMath::Max( 0.08, fPruneXp); 
  fPruneYp      = TMath::Max( 0.04, fPruneYp); 
  fPruneYtar    = TMath::Max( 4.0,  fPruneYtar); 
  fPruneDelta   = TMath::Max( 13.0, fPruneDelta); 
  fPruneBeta    = TMath::Max( 0.1,  fPruneBeta); 
  fPruneDf      = TMath::Max( 1.0,  fPruneDf); 
  fPruneChiBeta = TMath::Max( 2.0,  fPruneChiBeta); 
  fPruneFpTime  = TMath::Max( 5.0,  fPruneFpTime); 
  fPruneNPMT    = TMath::Max( 6.0,  fPruneNPMT);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::FindVertices( TClonesArray& tracks )
{
  // Reconstruct target coordinates for all tracks found in the focal plane.

  // In Hall A, this is passed off to the tracking detectors.
  // In Hall C, we do the target traceback here since the traceback should
  // not depend on which tracking detectors are used.

  fNtracks = tracks.GetLast()+1;

  for (Int_t it=0;it<tracks.GetLast()+1;it++) {
    THaTrack* track = static_cast<THaTrack*>( tracks[it] );

    Double_t hut[5];
    Double_t hut_rot[5];
    
    hut[0] = track->GetX()/100.0 + fZTrueFocus*track->GetTheta() + fDetOffset_x;//m
    hut[1] = track->GetTheta() + fAngOffset_x;//radians
    hut[2] = track->GetY()/100.0 + fZTrueFocus*track->GetPhi() + fDetOffset_y;//m
    hut[3] = track->GetPhi() + fAngOffset_y;//radians

    Double_t gbeam_y = 0.0;// This will be the y position from the fast raster

    hut[4] = -gbeam_y/100.0;

    // Retrieve the focal plane coordnates
    // Do the transpormation
    // Stuff results into track
    hut_rot[0] = hut[0];
    hut_rot[1] = hut[1] + hut[0]*fAngSlope_x;
    hut_rot[2] = hut[2];
    hut_rot[3] = hut[3] + hut[2]*fAngSlope_y;
    hut_rot[4] = hut[4];

    // Compute COSY sums
    Double_t sum[4];
    for(Int_t k=0;k<4;k++) {
      sum[k] = 0.0;
    }
    for(Int_t iterm=0;iterm<fNReconTerms;iterm++) {
      Double_t term=1.0;
      for(Int_t j=0;j<5;j++) {
	if(fReconTerms[iterm].Exp[j]!=0) {
	  term *= pow(hut_rot[j],fReconTerms[iterm].Exp[j]);
	}
      }
      for(Int_t k=0;k<4;k++) {
	sum[k] += term*fReconTerms[iterm].Coeff[k];
      }
    }
    // Transfer results to track
    // No beam raster yet
    //; In transport coordinates phi = hyptar = dy/dz and theta = hxptar = dx/dz 
    //;    but for unknown reasons the yp offset is named  htheta_offset
    //;    and  the xp offset is named  hphi_offset

    track->SetTarget(0.0, sum[1]*100.0, sum[0]+fPhiOffset, sum[2]+fThetaOffset);
    track->SetDp(sum[3]*100.0+fDeltaOffset);	// Percent.  (Don't think podd cares if it is % or fraction)
    // There is an hpcentral_offset that needs to be applied somewhere.
    // (happly_offs)
    track->SetMomentum(fPcentral*(1+track->GetDp()/100.0));

  }


  // ------------------ Moving it to TrackCalc --------------------

  /*
  // If enabled, sort the tracks by chi2/ndof
  if( GetTrSorting() )
    fTracks->Sort();
  
  // Find the "Golden Track". 
  if( GetNTracks() > 0 ) {
    // Select first track in the array. If there is more than one track
    // and track sorting is enabled, then this is the best fit track
    // (smallest chi2/ndof).  Otherwise, it is the track with the best
    // geometrical match (smallest residuals) between the U/V clusters
    // in the upper and lower VDCs (old behavior).
    // 
    // Chi2/dof is a well-defined quantity, and the track selected in this
    // way is immediately physically meaningful. The geometrical match
    // criterion is mathematically less well defined and not usually used
    // in track reconstruction. Hence, chi2 sortiing is preferable, albeit
    // obviously slower.

    fGoldenTrack = static_cast<THaTrack*>( fTracks->At(0) );
    fTrkIfo      = *fGoldenTrack;
    fTrk         = fGoldenTrack;
  } else
    fGoldenTrack = NULL;

  */
  // ------------------ Moving it to TrackCalc --------------------

  return 0;
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::TrackCalc()
{

  if ( ( fSelUsingScin == 0 ) && ( fSelUsingPrune == 0 ) ) {
    BestTrackSimple();
  } else if (fSelUsingPrune !=0) {
    BestTrackUsingPrune();
  } else {
    BestTrackUsingScin();
  }

  return TrackTimes( fTracks );
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackSimple()
{
  
  if( GetTrSorting() )
    fTracks->Sort();
  
  // Find the "Golden Track". 
  //  if( GetNTracks() > 0 ) {
  if( fNtracks > 0 ) {
    // Select first track in the array. If there is more than one track
    // and track sorting is enabled, then this is the best fit track
    // (smallest chi2/ndof).  Otherwise, it is the track with the best
    // geometrical match (smallest residuals) between the U/V clusters
    // in the upper and lower VDCs (old behavior).
    // 
    // Chi2/dof is a well-defined quantity, and the track selected in this
    // way is immediately physically meaningful. The geometrical match
    // criterion is mathematically less well defined and not usually used
    // in track reconstruction. Hence, chi2 sortiing is preferable, albeit
    // obviously slower.
      
    fGoldenTrack = static_cast<THaTrack*>( fTracks->At(0) );
    fTrkIfo      = *fGoldenTrack;
    fTrk         = fGoldenTrack;
  } else
    fGoldenTrack = NULL;

  return(0);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackUsingScin()
{
  Double_t chi2Min;

  if( fNtracks > 0 ) {
    fGoodTrack = -1;
    chi2Min = 10000000000.0;
    Int_t y2Dmin = 100;
    Int_t x2Dmin = 100;

    Bool_t* x2Hits        = new Bool_t [fHodo->GetNPaddles(2)];
    Bool_t* y2Hits        = new Bool_t [fHodo->GetNPaddles(3)];
    for (UInt_t j = 0; j < fHodo->GetNPaddles(2); j++ ){ 
      x2Hits[j] = kFALSE;
    }
    for (UInt_t j = 0; j < fHodo->GetNPaddles(3); j++ ){ 
      y2Hits[j] = kFALSE;
    }
      
    for (Int_t rawindex=0; rawindex<fHodo->GetTotHits(); rawindex++) {
      Int_t ip = fHodo->GetGoodRawPlane(rawindex);
      if(ip==2) {	// X2
	Int_t goodRawPad = fHodo->GetGoodRawPad(rawindex);
	x2Hits[goodRawPad] = kTRUE;
      } else if (ip==3) { // Y2
	Int_t goodRawPad = fHodo->GetGoodRawPad(rawindex);
	y2Hits[goodRawPad] = kTRUE;
      }
    }

    for (Int_t itrack = 0; itrack < fNtracks; itrack++ ){
      Double_t chi2PerDeg;

      THaTrack* aTrack = static_cast<THaTrack*>( fTracks->At(itrack) );      
      if (!aTrack) return -1;	

      if ( aTrack->GetNDoF() > fSelNDegreesMin ){
	chi2PerDeg =  aTrack->GetChi2() / aTrack->GetNDoF();

	if( ( aTrack->GetDedx()    > fSeldEdX1Min )   && 
	    ( aTrack->GetDedx()    < fSeldEdX1Max )   && 
	    ( aTrack->GetBeta()    > fSelBetaMin  )   &&
	    ( aTrack->GetBeta()    < fSelBetaMax  )   &&
	    ( aTrack->GetEnergy()  > fSelEtMin    )   && 
	    ( aTrack->GetEnergy()  < fSelEtMax    ) )  	    	    
	  {
	    Int_t x2D, y2D;
	      	      
	    if ( fNtracks > 1 ){
	      Double_t hitpos3  = aTrack->GetX() + aTrack->GetTheta() * ( fScin2XZpos + 0.5 * fScin2XdZpos );
	      Int_t icounter3  = TMath::Nint( ( hitpos3 - fHodo->GetPlaneCenter(2) ) / fHodo->GetPlaneSpacing(2) ) + 1;
	      Int_t hitCnt3  = TMath::Max( TMath::Min(icounter3, (Int_t) fHodo->GetNPaddles(2) ) , 1); // scin_2x_nr = 16
	      //	      fHitDist3 = fHitPos3 - ( fHodo->GetPlaneSpacing(2) * ( hitCnt3 - 1 ) + fHodo->GetPlaneCenter(2) );
	      Double_t hitpos4 = aTrack->GetY() + aTrack->GetPhi() * ( fScin2YZpos + 0.5 * fScin2YdZpos );
	      Int_t icounter4  = TMath::Nint( ( fHodo->GetPlaneCenter(3) - hitpos4 ) / fHodo->GetPlaneSpacing(3) ) + 1;
	      Int_t hitCnt4  = TMath::Max( TMath::Min(icounter4, (Int_t) fHodo->GetNPaddles(3) ) , 1); // scin_2y_nr = 10
	      //	      fHitDist4 = fHitPos4 - ( fHodo->GetPlaneCenter(3) - fHodo->GetPlaneSpacing(3) * ( hitCnt4 - 1 ) );
	      // Plane 3
	      Int_t mindiff=1000;
	      for (UInt_t i = 0; i <  fHodo->GetNPaddles(2); i++ ){
		if ( x2Hits[i] ) {
		  Int_t diff = TMath::Abs((Int_t)hitCnt3-(Int_t)i-1);
		  if (diff < mindiff) mindiff = diff;
		}
	      }
	      if(mindiff < 1000) {
		x2D = mindiff;
	      } else {
		x2D = 0;	// Is this what we really want if there were no hits on this plane?
	      }

	      // Plane 4
	      mindiff = 1000;
	      for (UInt_t i = 0; i < fHodo->GetNPaddles(3); i++ ){
		if ( y2Hits[i] ) {		    
		  Int_t diff = TMath::Abs((Int_t)hitCnt4-(Int_t)i-1);
		  if (diff < mindiff) mindiff = diff;
		}
	      }
	      if(mindiff < 1000) {
		y2D = mindiff;
	      } else {
		y2D = 0;	// Is this what we really want if there were no hits on this plane?
	      }
	    } else { // Only a single track
	      x2D = 0.;
	      y2D = 0.;
	    }

	    if ( y2D <= y2Dmin ) {
	      if ( y2D < y2Dmin ) {
		x2Dmin = 100;
		chi2Min = 10000000000.;
	      } // y2D min

	      if ( x2D <= x2Dmin ){
		if ( x2D < x2Dmin ){
		  chi2Min = 10000000000.0;
		} // condition x2D
		if ( chi2PerDeg < chi2Min ){

		  fGoodTrack = itrack; // fGoodTrack = itrack
		  y2Dmin = y2D;
		  x2Dmin = x2D;
		  chi2Min = chi2PerDeg;

		  fGoldenTrack = static_cast<THaTrack*>( fTracks->At( fGoodTrack ) );
		  fTrkIfo      = *fGoldenTrack;
		  fTrk         = fGoldenTrack;
		}		  
	      } // condition x2D
	    } // condition y2D
	  } // conditions for dedx, beta and trac energy		  
      } // confition for fNFreeFP greater than fSelNDegreesMin
    } // loop over tracks      

    // Fall back to using track with minimum chi2
    if ( fGoodTrack == -1 ){
      
      chi2Min = 10000000000.0;
      for (Int_t iitrack = 0; iitrack < fNtracks; iitrack++ ){
	Double_t chi2PerDeg;
	THaTrack* aTrack = dynamic_cast<THaTrack*>( fTracks->At(iitrack) );
	if (!aTrack) return -1;
	  
	if ( aTrack->GetNDoF() > fSelNDegreesMin ){
	  chi2PerDeg =  aTrack->GetChi2() / aTrack->GetNDoF();

	  if ( chi2PerDeg < chi2Min ){
	      
	    fGoodTrack = iitrack;
	    chi2Min = chi2PerDeg;

	    fGoldenTrack = aTrack;
	    fTrkIfo      = *fGoldenTrack;
	    fTrk         = fGoldenTrack;

	  }
	}
      } // loop over trakcs

    } 

  } else // Condition for fNtrack > 0
    fGoldenTrack = NULL;

  return(0);
}
    
//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackUsingPrune()
{  
  Int_t nGood;
  Double_t chi2Min;

  if ( fNtracks > 0 ) {
    chi2Min   = 10000000000.0;
    fGoodTrack = 0;    
    Bool_t* keep      = new Bool_t [fNtracks];  
    Int_t* reject    = new Int_t  [fNtracks];  

    THaTrack *testTracks[fNtracks];

    // ! Initialize all tracks to be good
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      keep[ptrack] = kTRUE;
      reject[ptrack] = 0;
      testTracks[ptrack] = static_cast<THaTrack*>( fTracks->At(ptrack) );      
      if (!testTracks[ptrack]) return -1;	
    }      
      
    // ! Prune on xptar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetTTheta() ) < fPruneXp ) && ( keep[ptrack] ) ){	  
	nGood ++;
      }	
    }
    if ( nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTTheta() ) >= fPruneXp ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 1;
	}
      }
    }
      
    // ! Prune on yptar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetTPhi() ) < fPruneYp ) && ( keep[ptrack] ) ){
	nGood ++; 
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTPhi() ) >= fPruneYp ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 2;
  
	}		
      }
    }
      
    // !     Prune on ytar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	
      if ( ( TMath::Abs( testTracks[ptrack]->GetTY() ) < fPruneYtar ) && ( keep[ptrack] ) ){
	nGood ++; 
      }	
    }      
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTY() ) >= fPruneYtar ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 10;
	}
      }
    }
  
    // !     Prune on delta
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	
      if ( ( TMath::Abs( testTracks[ptrack]->GetDp() ) < fPruneDelta ) && ( keep[ptrack] ) ){
	nGood ++; 
      }	
    }            
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetDp() ) >= fPruneDelta ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 20;
	}
      }
    }      
            
    // !     Prune on beta
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
      Double_t p = testTracks[ptrack]->GetP();
      Double_t betaP = p / TMath::Sqrt( p * p + fPartMass * fPartMass );       
      if ( ( TMath::Abs( testTracks[ptrack]->GetBeta() - betaP ) < fPruneBeta ) && ( keep[ptrack] ) ){
	nGood ++; 
      }	
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	Double_t p = testTracks[ptrack]->GetP();
	Double_t betaP = p / TMath::Sqrt( p * p + fPartMass * fPartMass );       
	if ( TMath::Abs( testTracks[ptrack]->GetBeta() - betaP ) >= fPruneBeta ) {	    
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 100;	          
	}	  
      }	
    }
  
    // !     Prune on deg. freedom for track chisq
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( testTracks[ptrack]->GetNDoF() >= fPruneDf ) && ( keep[ptrack] ) ){
	nGood ++; 	  
      }		
    }          
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( testTracks[ptrack]->GetNDoF() < fPruneDf ){	  
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 200;
	}
      }
    }

    //!     Prune on num pmt hits
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( testTracks[ptrack]->GetNPMT() >= fPruneNPMT ) && ( keep[ptrack] ) ){
	nGood ++; 	  
      }	        		
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( testTracks[ptrack]->GetNPMT() < fPruneNPMT ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 100000;
	}
      }
    }

    // !     Prune on beta chisqr
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( testTracks[ptrack]->GetBetaChi2() < fPruneChiBeta ) && 
	   ( testTracks[ptrack]->GetBetaChi2() > 0.01 )          &&   ( keep[ptrack] ) ){
	nGood ++; 	  
      }	        			
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( ( testTracks[ptrack]->GetBetaChi2() >= fPruneChiBeta ) || 
	     ( testTracks[ptrack]->GetBetaChi2() <= 0.01          ) ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 1000;
	}	  	  
      }
    }

    // !     Prune on fptime
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( TMath::Abs( testTracks[ptrack]->GetFPTime() - fHodo->GetStartTimeCenter() ) < fPruneFpTime )  &&   
	   ( keep[ptrack] ) ){
	nGood ++; 	  
      }	        			
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( TMath::Abs( testTracks[ptrack]->GetFPTime() - fHodo->GetStartTimeCenter() ) >= fPruneFpTime ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 2000;	    	    
	}
      }
    }
      
    // !     Prune on Y2 being hit
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( testTracks[ptrack]->GetGoodPlane4() == 1 )  &&  keep[ptrack] ) {
	nGood ++; 	  	  
      }	        			
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( testTracks[ptrack]->GetGoodPlane4() != 1 ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 10000;	    	    
	}
      }
    }      
  
    // !     Prune on X2 being hit
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	
      if ( ( testTracks[ptrack]->GetGoodPlane3() == 1 )  &&  keep[ptrack] ) {
	nGood ++; 	  	  
      }	        			
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      
	if ( testTracks[ptrack]->GetGoodPlane3() != 1 ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] = reject[ptrack] + 20000;	    	    	    	    
	}
      }
    }      
      
    // !     Pick track with best chisq if more than one track passed prune tests
    Double_t chi2PerDeg = 0.;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){	      	

      chi2PerDeg =  testTracks[ptrack]->GetChi2() / testTracks[ptrack]->GetNDoF();

      if ( ( chi2PerDeg < chi2Min ) && ( keep[ptrack] ) ){
	fGoodTrack = ptrack;
	chi2Min = chi2PerDeg;
      }	
    }      

    fGoldenTrack = static_cast<THaTrack*>( fTracks->At(fGoodTrack) );
    fTrkIfo      = *fGoldenTrack;
    fTrk         = fGoldenTrack;
      
  } else // Condition for fNtrack > 0
    fGoldenTrack = NULL;

  return(0);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::TrackTimes( TClonesArray* Tracks ) {
  // Do the actual track-timing (beta) calculation.
  // Use multiple scintillators to average together and get "best" time at S1.
  //
  // To be useful, a meaningful timing resolution should be assigned
  // to each Scintillator object (part of the database).

  
  return 0;
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::ReadRunDatabase( const TDatime& date )
{
  // Override THaSpectrometer with nop method.  All needed kinamatics
  // read in ReadDatabase.
  
  return kOK;
}

//_____________________________________________________________________________
ClassImp(THcHallCSpectrometer)
