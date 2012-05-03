#ifndef ROOT_THcScintillatorPlane
#define ROOT_THcScintillatorPlane

//////////////////////////////////////////////////////////////////////////////
//                         
// THcScintillatorPlane
//
// A Hall C scintillator plane
//
// May want to later inherit from a THcPlane class if there are similarities
// in what a plane is shared with other detector types (shower, etc.)
// 
//////////////////////////////////////////////////////////////////////////////

#include "THaNonTrackingDetector.h"

class THcScintillatorPlane : public THaNonTrackingDetector {
  
 public:
  virtual ~THcScintillatorPlane();

  THcScintillatorPlane( const char* name, const char* description,
			  THaApparatus* a = NULL);

  virtual Int_t CoarseProcess( TClonesArray& tracks ) = 0;
  virtual Int_t    FineProcess( TClonesArray& tracks )  = 0;
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

 protected:

  ClassDef(THcScintillatorPlane,0)
};
#endif


