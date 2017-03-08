#ifndef ROOT_THcShowerPlane
#define ROOT_THcShowerPlane

//////////////////////////////////////////////////////////////////////////////
//
// THcShowerPlane
//
// A Hall C Shower plane
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

  //  Double_t fSpacing;   not used

  //  TClonesArray* fParentHitList;  not used

  TVector3 GetOrigin() {
    return fOrigin;
  }

  Double_t GetEplane() {
    return fEplane;
  };

  Double_t GetEplane_pos() {
    return fEplane_pos;
  };

  Double_t GetEplane_neg() {
    return fEplane_neg;
  };

  Double_t GetEmean(Int_t i) {
    return fEmean[i];
  };

  Double_t GetEpos(Int_t i) {
    return fEpos[i];
  };

  Double_t GetEneg(Int_t i) {
    return fEneg[i];
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

  // Flash ADC parameters
  Int_t fUsingFADC;		// != 0 if using FADC in sample mode
  Int_t fADCMode;		//    
   //  1 == Use the pulse int - pulse ped
    //  2 == Use the sample integral - known ped
    //  3 == Use the sample integral - sample ped
 static const Int_t kADCStandard=0;
  static const Int_t kADCDynamicPedestal=1;
  static const Int_t kADCSampleIntegral=2;
  static const Int_t kADCSampIntDynPed=3;
   Int_t fPedSampLow;		// Sample range for
  Int_t fPedSampHigh;		// dynamic pedestal
  Int_t fDataSampLow;		// Sample range for
  Int_t fDataSampHigh;		// sample integration

  Double_t*   fA_Pos;         // [fNelem] ADC amplitudes of blocks
  Double_t*   fA_Neg;         // [fNelem] ADC amplitudes of blocks
  Double_t*   fA_Pos_p;	      // [fNelem] pedestal subtracted ADC amplitudes
  Double_t*   fA_Neg_p;	      // [fNelem] pedestal subtracted ADC amplitudes

  Double_t* fEpos;     // [fNelem] energy depositions seen by positive PMTs
  Double_t* fEneg;     // [fNelem] energy depositions seen by negative PMTs
  Double_t* fEmean;    // [fNelem] mean energy depositions (pos + neg)
  Double_t  fEplane;   // Energy deposition in the plane
  Double_t  fEplane_pos;   // Energy deposition in the plane from positive PMTs
  Double_t  fEplane_neg;   // Energy deposition in the plane from negative PMTs

  // These lists are not used actively for now.
  TClonesArray* fPosADCHits;    // List of positive ADC hits
  TClonesArray* fNegADCHits;    // List of negative ADC hits

  Int_t fLayerNum;		// Layer # 1-4

  Int_t fNPedestalEvents;	/* Pedestal event counter */
  Int_t fMinPeds;		/* Only analyze/update if num events > */
  Int_t *fPosPedSum;		/* Accumulators for pedestals */
  Int_t *fPosPedSum2;
  Int_t *fPosPedLimit;          // Analyze pedestal if ADC signal < PedLimit
  Int_t *fPosPedCount;          // [fNelem] counter of pedestal analysis
  Int_t *fNegPedSum;
  Int_t *fNegPedSum2;
  Int_t *fNegPedLimit;          // Analyze pedestal if ADC signal < PedLimit
  Int_t *fNegPedCount;          // [fNelem] counter of pedestal analysis

  Float_t *fPosPed;             // [fNelem] pedestal positions
  Float_t *fPosSig;             // [fNelem] pedestal rms-s
  Float_t *fPosThresh;          // [fNelem] ADC thresholds
  Float_t *fNegPed;
  Float_t *fNegSig;
  Float_t *fNegThresh;

  TClonesArray* frPosAdcPedRaw;
  TClonesArray* frPosAdcPulseIntRaw;
  TClonesArray* frPosAdcPulseAmpRaw;
  TClonesArray* frPosAdcPulseTimeRaw;

  TClonesArray* frPosAdcPed;
  TClonesArray* frPosAdcPulseInt;
  TClonesArray* frPosAdcPulseAmp;

  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;

  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );
  ClassDef(THcShowerPlane,0); // Calorimeter bars in a plane
};
#endif
