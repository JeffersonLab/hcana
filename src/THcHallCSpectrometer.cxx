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
#include "THaTrack.h"
#include "THaTrackProj.h"
#include "THaTriggerTime.h"
#include "TMath.h"
#include "TList.h"

#include "TList.h"
#include "TMath.h"

#ifdef WITH_DEBUG
#include <iostream>
#endif

using namespace std;

//_____________________________________________________________________________
THcHallCSpectrometer::THcHallCSpectrometer( const char* name, const char* description ) :
  THaSpectrometer( name, description )
{
  // Constructor. Defines the standard detectors for the HRS.
  //  AddDetector( new THaTriggerTime("trg","Trigger-based time offset"));

  //sc_ref = static_cast<THaScintillator*>(GetDetector("s1"));

  SetTrSorting(kFALSE);
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
Int_t THcHallCSpectrometer::FindVertices( TClonesArray& tracks )
{
  // Reconstruct target coordinates for all tracks found in the focal plane.

  TIter nextTrack( fTrackingDetectors );

  nextTrack.Reset();
  while( THaTrackingDetector* theTrackDetector =
	 static_cast<THaTrackingDetector*>( nextTrack() )) {
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "Call FineTrack() for " 
			<< theTrackDetector->GetName() << "... ";
#endif
    theTrackDetector->FindVertices( tracks );
#ifdef WITH_DEBUG
    if( fDebug>1 ) cout << "done.\n";
#endif
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
  
  if ( !Tracks ) return -1;
  
  THaTrack *track=0;
  Int_t ntrack = GetNTracks();

  // linear regression to:  t = t0 + pathl/(beta*c)
  //   where t0 is the time of the track at the reference plane (sc_ref).
  //   t0 and beta are solved for.
  //
#if 0
  for ( Int_t i=0; i < ntrack; i++ ) {
    track = static_cast<THaTrack*>(Tracks->At(i));
    THaTrackProj* tr_ref = static_cast<THaTrackProj*>
      (sc_ref->GetTrackHits()->At(i));
    
    Double_t pathlref = tr_ref->GetPathLen();
    
    Double_t wgt_sum=0.,wx2=0.,wx=0.,wxy=0.,wy=0.;
    Int_t ncnt=0;
    
    // linear regression to get beta and time at ref.
    TIter nextSc( fNonTrackingDetectors );
    THaNonTrackingDetector *det;
    while ( ( det = static_cast<THaNonTrackingDetector*>(nextSc()) ) ) {
      THaScintillator *sc = dynamic_cast<THaScintillator*>(det);
      if ( !sc ) continue;

      const THaTrackProj *trh = static_cast<THaTrackProj*>(sc->GetTrackHits()->At(i));
      
      Int_t pad = trh->GetChannel();
      if (pad<0) continue;
      Double_t pathl = (trh->GetPathLen()-pathlref);
      Double_t time = (sc->GetTimes())[pad];
      Double_t wgt = (sc->GetTuncer())[pad];
      
      if (pathl>.5*kBig || time>.5*kBig) continue;
      if (wgt>0) wgt = 1./(wgt*wgt);
      else continue;
      
      wgt_sum += wgt;
      wx2 += wgt*pathl*pathl;
      wx  += wgt*pathl;
      wxy += wgt*pathl*time;
      wy  += wgt*time;
      ncnt++;
    }

    Double_t beta = kBig;
    Double_t dbeta = kBig;
    Double_t time = kBig;
    Double_t dt = kBig;
    
    Double_t delta = wgt_sum*wx2-wx*wx;
    
    if (delta != 0.) {
      time = (wx2*wy-wx*wxy)/delta;
      dt = TMath::Sqrt(wx2/delta);
      Double_t invbeta = (wgt_sum*wxy-wx*wy)/delta;
      if (invbeta != 0.) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	Double_t c = TMath::C();
#else
	Double_t c = 2.99792458e8;
#endif
	beta = 1./(c*invbeta);
	dbeta = TMath::Sqrt(wgt_sum/delta)/(c*invbeta*invbeta);
      }
    } 

    track->SetBeta(beta);
    track->SetdBeta(dbeta);
    track->SetTime(time);
    track->SetdTime(dt);
  }
#endif  
  return 0;
}

//_____________________________________________________________________________
ClassImp(THcHallCSpectrometer)
