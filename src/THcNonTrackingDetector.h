#ifndef ROOT_THaNonTrackingDetector
#define ROOT_THaNonTrackingDetector

//////////////////////////////////////////////////////////////////////////
//
// THaNonTrackingDetector.h
//
// Abstract base class for a generic non-tracking spectrometer detector. 
//
// This is a special THaSpectrometerDetector -- any detector that
// is not a tracking detector.  This includes PID detectors.
//
//////////////////////////////////////////////////////////////////////////

#include "THaSpectrometerDetector.h"

class TClonesArray;

class THaNonTrackingDetector : public THaSpectrometerDetector {
  
public:
  THaNonTrackingDetector(); // only for ROOT I/O

  virtual ~THaNonTrackingDetector();
  
  virtual Int_t    CoarseProcess( TClonesArray& tracks ) = 0;
  virtual Int_t    FineProcess( TClonesArray& tracks )  = 0;
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

protected:

  //Only derived classes may construct me for real

  THaNonTrackingDetector( const char* name, const char* description,
			  THaApparatus* a = NULL);

  ClassDef(THaNonTrackingDetector,1)  //ABC for a non-tracking spectrometer detector
};

#endif
