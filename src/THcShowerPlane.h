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

  Double_t GetEplane() {
    return fEplane;
  };

  Double_t GetEmean(Int_t i) {
    return fEmean[i];
  };

  Double_t GetAposP(Int_t i) {
    return fA_Pos_p[i];
  };

  Double_t GetAnegP(Int_t i) {
    return fA_Neg_p[i];
  };

  Double_t GetApos(Int_t i) {
    return fA_Pos[i];
  };

  Double_t GetAneg(Int_t i) {
    return fA_Neg[i];
  };

  Double_t GetPosThr(Int_t i) {
    return fPosThresh[i];
  };

  Double_t GetNegThr(Int_t i) {
    return fNegThresh[i];
  };

  Double_t GetPosPed(Int_t i) {
    return fPosPed[i];
  };

  Double_t GetNegPed(Int_t i) {
    return fNegPed[i];
  };

 protected:

  Double_t*   fA_Pos;         // [fNelem] Array of ADC amplitudes of blocks
  Double_t*   fA_Neg;         // [fNelem] Array of ADC amplitudes of blocks
  Double_t*   fA_Pos_p;	     // [fNelem] Array of pedestal subtracted ADC amplitudes
  Double_t*   fA_Neg_p;	     // [fNelem] Array of pedestal subtracted ADC amplitudes

  Double_t* fEpos;     // [fNelem] Array of energy depositions seen by positive PMTs
  Double_t* fEneg;     // [fNelem] Array of energy depositions seen by negative PMTs
  Double_t* fEmean;    // [fNelem] Array of mean energy depositions (pos + neg)
  Double_t  fEplane;   // Energy deposition per plane

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

  //  Double_t *fPosPed;
  //  Double_t *fPosSig;
  //  Double_t *fPosThresh;
  //  Double_t *fNegPed;
  //  Double_t *fNegSig;
  //  Double_t *fNegThresh;
  Float_t *fPosPed;      //To be consistent with Engine
  Float_t *fPosSig;
  Float_t *fPosThresh;
  Float_t *fNegPed;
  Float_t *fNegSig;
  Float_t *fNegThresh;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );
  ClassDef(THcShowerPlane,0)
};
#endif
