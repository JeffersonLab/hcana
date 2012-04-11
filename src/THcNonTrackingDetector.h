#ifndef ROOT_THcNonTrackingDetector
#define ROOT_THcNonTrackingDetector

//////////////////////////////////////////////////////////////////////////
//
// THcNonTrackingDetector.h
//
// Abstract base class for a generic non-tracking spectrometer detector. 
//
// This is a special THaSpectrometerDetector -- any detector that
// is not a tracking detector.  This includes PID detectors.
//
//////////////////////////////////////////////////////////////////////////

#include "THcSpectrometerDetector.h"

class TClonesArray;

class THcNonTrackingDetector : public THcSpectrometerDetector {
  
public:
  THcNonTrackingDetector(); // only for ROOT I/O

  virtual ~THcNonTrackingDetector();
  
  virtual Int_t    CoarseProcess( TClonesArray& tracks ) = 0;
  virtual Int_t    FineProcess( TClonesArray& tracks )  = 0;
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

protected:

  //Only derived classes may construct me for real

  THcNonTrackingDetector( const char* name, const char* description,
			  THaApparatus* a = NULL);

  ClassDef(THcNonTrackingDetector,0)  //ABC for a non-tracking spectrometer detector
};

#endif
