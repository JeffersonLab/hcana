#ifndef ROOT_THcShowerPlane
#define ROOT_THcShowerPlane

//////////////////////////////////////////////////////////////////////////////
//                         
// THcShowerPlane
//
// A Hall C scintillator plane
//
// May want to later inherit from a THcPlane class if there are similarities
// in what a plane is shared with other detector types (shower, etc.)
// 
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "TClonesArray.h"

class THaEvData;
class THaSignalHit;

class THcShowerPlane : public THaSubDetector {
  
 public:
  THcShowerPlane( const char* name, const char* description,
			Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcShowerPlane();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);

  Double_t fSpacing;

  TClonesArray* fParentHitList;

 protected:

  TClonesArray* fPosADCHits;
  TClonesArray* fNegADCHits;

  TClass* fPosADCHitsClass;
  TClass* fNegADCHitsClass;

  Int_t fLayerNum;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  ClassDef(THcShowerPlane,0)
};
#endif


 
