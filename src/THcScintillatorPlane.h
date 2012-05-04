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
  THcScintillatorPlane( const char* name, const char* description,
			  THaApparatus* a = NULL);
  virtual ~THcScintillatorPlane();

  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

 protected:

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  ClassDef(THcScintillatorPlane,0)
};
#endif


