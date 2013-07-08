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

#include <iostream>

#include <fstream>

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
  virtual Int_t AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit);
  virtual void  CalculatePedestals( );

  Double_t fSpacing;

  TClonesArray* fParentHitList;

  Float_t GetEplane() {
    return fEplane;
  };

  Float_t GetEmean(Int_t i) {
    return fEmean[i];
  };

 protected:

  Float_t*   fA_Pos;         // [fNelem] Array of ADC amplitudes of blocks
  Float_t*   fA_Neg;         // [fNelem] Array of ADC amplitudes of blocks
  Float_t*   fA_Pos_p;	     // [fNelem] Array of pedestal subtracted ADC amplitudes
  Float_t*   fA_Neg_p;	     // [fNelem] Array of pedestal subtracted ADC amplitudes

  Float_t* fEpos;     // [fNelem] Array of energy depositions seen by positive PMTs
  Float_t* fEneg;     // [fNelem] Array of energy depositions seen by negative PMTs
  Float_t* fEmean;    // [fNelem] Array of mean energy depositions (pos + neg)
  Float_t  fEplane;   // Energy deposition per plane

  TClonesArray* fPosADCHits;
  TClonesArray* fNegADCHits;

  Int_t fLayerNum;

  Int_t fPlaneNum;		/* Which plane am I 1-4 */
  Int_t fNPedestalEvents;	/* Number of pedestal events */
  Int_t fMinPeds;		/* Only analyze/update if num events > */
  Int_t *fPosPedSum;		/* Accumulators for pedestals */
  Int_t *fPosPedSum2;
  Int_t *fPosPedLimit;
  Int_t *fPosPedCount;
  Int_t *fNegPedSum;
  Int_t *fNegPedSum2;
  Int_t *fNegPedLimit;
  Int_t *fNegPedCount;

  Double_t *fPosPed;
  Double_t *fPosSig;
  Double_t *fPosThresh;
  Double_t *fNegPed;
  Double_t *fNegSig;
  Double_t *fNegThresh;


  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );
  ClassDef(THcShowerPlane,0)
};
#endif
