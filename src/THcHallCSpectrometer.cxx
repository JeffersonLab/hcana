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

  delete [] fX2D;                 fX2D = NULL;               // Ahmed
  delete [] fY2D;                 fY2D = NULL;               // Ahmed
  
  delete [] f2XHits;              f2XHits = NULL;            // Ahmed  
  delete [] f2YHits;              f2YHits = NULL;            // Ahmed  

  //  delete [] fKeep;                fKeep = NULL;           // Ahmed

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
  for(Int_t i=0;i<fNReconTerms;i++) {
    for(Int_t j=0;j<4;j++) {
      fReconCoeff[i][j] = 0.0;
    }
    for(Int_t j=0;j<5;j++) {
      fReconExponents[i][j] = 0;
    }
  }
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

  MAXHODHITS = 53;
  
  fX2D           = new Double_t [MAXHODHITS];
  fY2D           = new Double_t [MAXHODHITS];

  f2XHits        = new Int_t [16];
  f2YHits        = new Int_t [16];

  //  fKeep = new Bool_t [MAXHODHIT];


  // --------------- To get energy from THcShower ----------------------

  const char* detector_name = "hod";  
  //THaApparatus* app = GetDetector();
  THaDetector* det = GetDetector("hod");
  // THaDetector* det = app->GetDetector( shower_detector_name );
  
  if( !dynamic_cast<THcHodoscope*>(det) ) {
    Error("THcHallCSpectrometer", "Cannot find hodoscope detector %s",
    	  detector_name );
     return fStatus = kInitError;
  }
  
  fHodo = static_cast<THcHodoscope*>(det);     // fHodo is a membervariable
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
    {"pcentral",              &fPCentral,              kDouble               },
    {"theta_lab",             &fTheta_lab,             kDouble               },
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
    {"prune_npmt",            &fPruneFpTime,           kDouble,         0,  1},
    {"prune_fptime",          &fPruneNPMT,             kDouble,         0,  1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  //

  cout << "\n\n\nhodo planes = " << fNPlanes << endl;
  cout << "sel using scin = " << fSelUsingScin << endl;
  cout <<  "fPruneXp = "      <<  fPruneXp << endl; 
  cout <<  "fPruneYp = "      <<  fPruneYp << endl; 
  cout <<  "fPruneYtar = "    <<  fPruneYtar << endl; 
  cout <<  "fPruneDelta = "   <<  fPruneDelta << endl; 
  cout <<  "fPruneBeta = "    <<  fPruneBeta << endl; 
  cout <<  "fPruneDf = "      <<  fPruneDf << endl; 
  cout <<  "fPruneChiBeta = " <<  fPruneChiBeta << endl; 
  cout <<  "fPruneFpTime = "  <<  fPruneFpTime << endl; 
  cout <<  "fPruneNPMT = "    <<  fPruneNPMT << endl; 
  cout << "sel using prune = " << fSelUsingPrune << endl;
  cout << " fPcentral = " <<  fPCentral << " " <<fPCentralOffset << endl; 
  cout << " fThate_lab = " <<  fTheta_lab << " " <<fThetaCentralOffset << endl; 
  fPCentral= fPCentral*(1.+fPCentralOffset/100.);
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
    return kInitError; // Is this the right return code?
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
  //cout << "Reading matrix elements" << endl;
  while(good && line.compare(0,4," ---")!=0) {
    if(fNReconTerms >= fMaxReconElements) {
      Error(here, "too much data in reconstruction coefficient file %s",reconCoeffFilename.c_str());
      return kInitError; // Is this the right return code?
    }
    sscanf(line.c_str()," %le %le %le %le %1d%1d%1d%1d%1d"
	   ,&fReconCoeff[fNReconTerms][0],&fReconCoeff[fNReconTerms][1]
	   ,&fReconCoeff[fNReconTerms][2],&fReconCoeff[fNReconTerms][3]
	   ,&fReconExponents[fNReconTerms][0]
	   ,&fReconExponents[fNReconTerms][1]
	   ,&fReconExponents[fNReconTerms][2]
	   ,&fReconExponents[fNReconTerms][3]
	   ,&fReconExponents[fNReconTerms][4]);
    // Parse line into fReconCoeff[fNReconTerms][0 - 3] and
    //                 fReconExponents[fNReconTerms][0 - 5]
    fNReconTerms++;
    good = getline(ifile,line).good();    
  }
  cout << "Read " << fNReconTerms << " matrix element terms" << endl;
  if(!good) {
    Error(here, "Error processing reconstruction coefficient file %s",reconCoeffFilename.c_str());
    return kInitError; // Is this the right return code?
  }

  return kOK;
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
	if(fReconExponents[iterm][j]!=0) {
	  term *= pow(hut_rot[j],fReconExponents[iterm][j]);
	}
      }
      for(Int_t k=0;k<4;k++) {
	sum[k] += term*fReconCoeff[iterm][k];
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
    track->SetMomentum(fPCentral*(1+track->GetDp()/100.0));
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
  }

  if ( fSelUsingScin == 1 ){
    if( fNtracks > 0 ) {
      
      Double_t fY2Dmin, fX2Dmin, fZap, ft, fChi2PerDeg; //, fShowerEnergy;
      Double_t fHitPos4, fHitPos3, fHitDist3, fHitDist4; //, fChi2Min;
      Int_t i, j, itrack, ip, ihit; //, fGoodTimeIndex = -1;
      Int_t  fHitCnt4, fHitCnt3, fRawIndex, fGoodRawPad;

      fChi2Min = 10000000000.0;   fGoodTrack = -1;   fY2Dmin = 100.;
      fX2Dmin = 100.;             fZap = 0.;

      fRawIndex = -1;
      for ( itrack = 0; itrack < fNtracks; itrack++ ){
	
	THaTrack* goodTrack = static_cast<THaTrack*>( fTracks->At(itrack) );      
        if (!goodTrack) return -1;	

	if ( goodTrack->GetNDoF() > fSelNDegreesMin ){
	  fChi2PerDeg =  goodTrack->GetChi2() / goodTrack->GetNDoF();
	  
	  if( ( goodTrack->GetDedx()    > fSeldEdX1Min )   && 
	      ( goodTrack->GetDedx()    < fSeldEdX1Max )   && 
	      ( goodTrack->GetBeta()    > fSelBetaMin  )   &&
	      ( goodTrack->GetBeta()    < fSelBetaMax  )   &&
	      ( goodTrack->GetEnergy()  > fSelEtMin    )   && 
	      ( goodTrack->GetEnergy()  < fSelEtMax    ) )  	    	    
	    {
	      	      
	      for ( j = 0; j < fHodo->GetNPaddles(2); j++ ){ 
		f2XHits[j] = -1; 
		f2YHits[j] = -1; 
	      }
	      
	      for ( ip = 0; ip < fNPlanes; ip++ ){
		for ( ihit = 0; ihit < fHodo->GetNScinHits(ip); ihit++ ){
		  fRawIndex ++;		  

		  //		  fGoodRawPad = fHodo->GetGoodRawPad(fRawIndex)-1;
		  fGoodRawPad = fHodo->GetGoodRawPad(fRawIndex);

		  if ( ip == 2 ){  
		    f2XHits[fGoodRawPad] = 0;   		    
		  }

		  if ( ip == 3 ){  
		    f2YHits[fGoodRawPad] = 0;   
		    
		  } 

		} // loop over hits of a plane
	      } // loop over planes 

	      fHitPos4  = goodTrack->GetY() + goodTrack->GetPhi() * ( fScin2YZpos + 0.5 * fScin2YdZpos );
	      fHitCnt4  = TMath::Nint( ( fHodo->GetPlaneCenter(3) - fHitPos4 ) / fHodo->GetPlaneSpacing(3) ) + 1;
	      fHitCnt4  = TMath::Max( TMath::Min(fHitCnt4, fHodo->GetNPaddles(3) ) , 1); // scin_2y_nr = 10
	      fHitDist4 = fHitPos4 - ( fHodo->GetPlaneCenter(3) - fHodo->GetPlaneSpacing(3) * ( fHitCnt4 - 1 ) );
	      	      
	      //----------------------------------------------------------------

	      if ( fNtracks > 1 ){     // Plane 4		
		fZap = 0.;
		ft = 0;
		
		for ( i = 0; i < fHodo->GetNPaddles(3); i++ ){
		  if ( f2YHits[i] == 0 ) {		    
		    fY2D[itrack] = TMath::Abs(fHitCnt4-i-1);
		    ft ++;
		    		    
		    if   ( ft == 1 )                              fZap = fY2D[itrack];
		    if ( ( ft == 2 ) && ( fY2D[itrack] < fZap ) ) fZap = fY2D[itrack]; 		    
		    if ( ( ft == 3 ) && ( fY2D[itrack] < fZap ) ) fZap = fY2D[itrack]; 
		    if ( ( ft == 4 ) && ( fY2D[itrack] < fZap ) ) fZap = fY2D[itrack]; 
		    if ( ( ft == 5 ) && ( fY2D[itrack] < fZap ) ) fZap = fY2D[itrack]; 
		    if ( ( ft == 6 ) && ( fY2D[itrack] < fZap ) ) fZap = fY2D[itrack]; 

		  } // condition for fHodScinHit[4][i]
		} // loop over 10
		fY2D[itrack] = fZap; 
	      } // condition for track. Plane 4

	      if ( fNtracks == 1 ) fY2D[itrack] = 0.;

	      fHitPos3  = goodTrack->GetX() + goodTrack->GetTheta() * ( fScin2XZpos + 0.5 * fScin2XdZpos );
	      fHitCnt3  = TMath::Nint( ( fHitPos3 - fHodo->GetPlaneCenter(2) ) / fHodo->GetPlaneSpacing(2) ) + 1;
	      fHitCnt3  = TMath::Max( TMath::Min(fHitCnt3, fHodo->GetNPaddles(2) ) , 1); // scin_2x_nr = 16
	      fHitDist3 = fHitPos3 - ( fHodo->GetPlaneSpacing(2) * ( fHitCnt3 - 1 ) + fHodo->GetPlaneCenter(2) );

	      //----------------------------------------------------------------

	      if ( fNtracks > 1 ){     // Plane 3 (2X)
		fZap = 0.;
		ft = 0;
		for ( i = 0; i <  fHodo->GetNPaddles(2); i++ ){
		  if ( f2XHits[i] == 0 ) {
		    fX2D[itrack] = TMath::Abs(fHitCnt3-i-1);
		    
		    ft ++;
		    if   ( ft == 1 )                              fZap = fX2D[itrack];
		    if ( ( ft == 2 ) && ( fX2D[itrack] < fZap ) ) fZap = fX2D[itrack]; 		    
		    if ( ( ft == 3 ) && ( fX2D[itrack] < fZap ) ) fZap = fX2D[itrack]; 
		    if ( ( ft == 4 ) && ( fX2D[itrack] < fZap ) ) fZap = fX2D[itrack]; 
		    if ( ( ft == 5 ) && ( fX2D[itrack] < fZap ) ) fZap = fX2D[itrack]; 
		    if ( ( ft == 6 ) && ( fX2D[itrack] < fZap ) ) fZap = fX2D[itrack]; 
		    
		  } // condition for fHodScinHit[4][i]
		} // loop over 16
		fX2D[itrack] = fZap; 
	      } // condition for track. Plane 3

	      //----------------------------------------------------------------

	      if ( fNtracks == 1 ) 
		fX2D[itrack] = 0.;
	      	      
	      if ( fY2D[itrack] <= fY2Dmin ) {
		if ( fY2D[itrack] < fY2Dmin ) {
		  fX2Dmin = 100.;
		  fChi2Min = 10000000000.;
		} // fY2D min
		
		if ( fX2D[itrack] <= fX2Dmin ){
		  if ( fX2D[itrack] < fX2Dmin ){
		    fChi2Min = 10000000000.0;
		  } // condition fX2D
		  if ( fChi2PerDeg < fChi2Min ){

		    fGoodTrack = itrack; // fGoodTrack = itrack
		    fY2Dmin = fY2D[itrack];
		    fX2Dmin = fX2D[itrack];
		    fChi2Min = fChi2PerDeg;

		    fGoldenTrack = static_cast<THaTrack*>( fTracks->At( fGoodTrack ) );
		    fTrkIfo      = *fGoldenTrack;
		    fTrk         = fGoldenTrack;
		  }		  
		} // condition fX2D
	      } // condition fY2D
	    } // conditions for dedx, beta and trac energy		  
	} // confition for fNFreeFP greater than fSelNDegreesMin
      } // loop over tracks      

      if ( fGoodTrack == -1 ){
	
	fChi2Min = 10000000000.0;
	for (Int_t iitrack = 0; iitrack < fNtracks; iitrack++ ){
	  THaTrack* aTrack = dynamic_cast<THaTrack*>( fTracks->At(iitrack) );
	  if (!aTrack) return -1;
	  
	  if ( aTrack->GetNDoF() > fSelNDegreesMin ){
	    fChi2PerDeg =  aTrack->GetChi2() / aTrack->GetNDoF();
	    
	    if ( fChi2PerDeg < fChi2Min ){
	      
	      fGoodTrack = iitrack;
	      fChi2Min = fChi2PerDeg;
	      
	      fGoldenTrack = static_cast<THaTrack*>( fTracks->At(fGoodTrack) );
	      fTrkIfo      = *fGoldenTrack;
	      fTrk         = fGoldenTrack;

	    }
	  }
	} // loop over trakcs
	
      } // Condition for fNtrack > 0

    } else //  condtion fSelUsingScin == 1
      fGoldenTrack = NULL;
    
  }

  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------

  if ( fSelUsingPrune == 1 ){
    
    fPruneXp      = TMath::Max( 0.08, fPruneXp); 
    fPruneYp      = TMath::Max( 0.04, fPruneYp); 
    fPruneYtar    = TMath::Max( 4.0,  fPruneYtar); 
    fPruneDelta   = TMath::Max( 13.0, fPruneDelta); 
    fPruneBeta    = TMath::Max( 0.1,  fPruneBeta); 
    fPruneDf      = TMath::Max( 1.0,  fPruneDf); 
    fPruneChiBeta = TMath::Max( 2.0,  fPruneChiBeta); 
    fPruneFpTime  = TMath::Max( 5.0,  fPruneFpTime); 
    fPruneNPMT    = TMath::Max( 6.0,  fPruneNPMT); 
    
    Int_t ptrack = 0, fNGood;

    if ( fNtracks > 0 ) {
      fChi2Min   = 10000000000.0;
      fGoodTrack = 0;    
      fKeep      = new Bool_t [fNtracks];  
      fReject    = new Int_t  [fNtracks];  

      THaTrack *testTracks[fNtracks];

      // ! Initialize all tracks to be good
      for ( ptrack = 0; ptrack < fNtracks; ptrack++ ){
	fKeep[ptrack] = kTRUE;
	fReject[ptrack] = 0;
      }      

      // ! Prune on xptar
      fNGood = 0;
      for ( ptrack = 0; ptrack < fNtracks; ptrack++ ){
	THaTrack* xpGoodTrack = static_cast<THaTrack*>( fTracks->At(ptrack) );      
        if (!xpGoodTrack) return -1;	

	testTracks[ptrack] = static_cast<THaTrack*>( fTracks->At(ptrack) );      
        if (!testTracks[ptrack]) return -1;	

	cout << "  Event = " <<  fHodo->GetEvent()
	     << "   TargetX = " << xpGoodTrack->GetTX()
	     << "   test pointer = " << testTracks[ptrack] 
	     << "   TargetX(test) = " << testTracks[ptrack]->GetTX()
	     << endl;

	if ( ( TMath::Abs( xpGoodTrack->GetTX() ) < fPruneXp ) && ( fKeep[ptrack] ) ){
	  fNGood ++;
	}	
      }
      // for ( ptrack = 0; ptrack < fNtracks; ptrack++ ){
     	

      // 	fKeep[ptrack]   = kFALSE;
      // 	fReject[ptrack] = fReject[ptrack] + 1;
	
      // 	cout << "Evnet= "       << fHodo->GetEvent()
      // 	     << "   track = "   << ptrack + 1
      // 	     // << "   fKeep = "   << fKeep[ptrack] 
      // 	     // << "   fReject = " << fReject[ptrack]
      // 	     << endl;
      // }

      delete [] fKeep;             fKeep = NULL;           // Ahmed
      delete [] fReject;           fKeep = NULL;           // Ahmed      
      for ( ptrack = 0; ptrack < fNtracks; ptrack++ ){	
	testTracks[ptrack] = NULL;
	delete 	testTracks[ptrack];

	cout << "test pointer = " << testTracks[ptrack] << endl;
	
      }	
      

    } // if at least one track 
  } // if prune
  
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------

  cout << endl;  
  return TrackTimes( fTracks );
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
