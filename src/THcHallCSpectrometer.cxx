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
#include "THcShower.h"

#include <iostream>
#include <fstream>

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

  // Get the matrix element filename from the variable store
  // Read in the matrix

  InitializeReconstruction();

  char prefix[2];

  cout << " GetName() " << GetName() << endl;

  prefix[0]=tolower(GetName()[0]);
  prefix[1]='\0';

  string reconCoeffFilename;
  DBRequest list[]={
    {"_recon_coeff_filename",&reconCoeffFilename,kString},
    {"theta_offset",&fThetaOffset,kDouble},
    {"phi_offset",&fPhiOffset,kDouble},
    {"delta_offset",&fDeltaOffset,kDouble},
    {"thetacentral_offset",&fThetaCentralOffset,kDouble},
    {"_oopcentral_offset",&fOopCentralOffset,kDouble},
    {"pcentral_offset",&fPCentralOffset,kDouble},
    {"pcentral",&fPCentral,kDouble},
    {"theta_lab",&fTheta_lab,kDouble},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  //
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
    cout << line << endl;
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
  cout << line << endl;
  fNReconTerms = 0;
  cout << "Reading matrix elements" << endl;
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
  if(!good) {
    Error(here, "error processing reconstruction coefficient file %s",reconCoeffFilename.c_str());
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

  return 0;
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::TrackCalc()
{
  // Additioal track calculations. At present, we only calculate beta here.

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
