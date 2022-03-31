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
#include "THcCherenkov.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class THaEvData;
class THaSignalHit;
class THcHodoscope;

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
  virtual Int_t CoarseProcessHits();
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
    return fGoodPosAdcPulseInt[i];
  };

  Double_t GetAnegP(Int_t i) {
    return fGoodNegAdcPulseInt[i];
  };

  Double_t GetApos(Int_t i) {
    return fGoodPosAdcPulseIntRaw[i];
  };

  Double_t GetAneg(Int_t i) {
    return fGoodNegAdcPulseIntRaw[i];
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

  Int_t AccumulateStat(TClonesArray& tracks);

protected:

  THaDetectorBase* fParent;
  THcCherenkov* fCherenkov;

  // Flash ADC parameters
  Int_t fUsingFADC;		// != 0 if using FADC in sample mode
   //  1 == Use the pulse int - pulse ped
    //  2 == Use the sample integral - known ped
    //  3 == Use the sample integral - sample ped
  static const Int_t kADCStandard=0;
  static const Int_t kADCDynamicPedestal=1;
  static const Int_t kADCSampleIntegral=2;
  static const Int_t kADCSampIntDynPed=3;

  Int_t fDebugAdc;              // fADC debug flag
  Int_t fPedSampLow;		// Sample range for
  Int_t fPedSampHigh;		// dynamic pedestal
  Int_t fDataSampLow;		// Sample range for
  Int_t fDataSampHigh;		// sample integration
  Double_t fAdcNegThreshold;		//
  Double_t fAdcPosThreshold;		//
  Double_t fAdcTdcOffset;

  Int_t  fOutputSampWaveform;
  Int_t  fUseSampWaveform;
  Double_t  fSampThreshold;
  Int_t  fSampNSA;
  Int_t  fSampNSAT;
  Int_t  fSampNSB;

  //counting variables
  Int_t     fTotNumPosAdcHits;
  Int_t     fTotNumNegAdcHits;
  Int_t     fTotNumAdcHits;

  Int_t     fTotNumGoodPosAdcHits;
  Int_t     fTotNumGoodNegAdcHits;
  Int_t     fTotNumGoodAdcHits;

   //individual pmt data objects
  vector<Int_t>    fNumGoodPosAdcHits;
  vector<Int_t>    fNumGoodNegAdcHits;

  vector<Double_t>      fGoodPosAdcPed;
  vector<Double_t>      fGoodPosAdcPulseInt;
  vector<Double_t>      fGoodPosAdcPulseAmp;
  vector<Double_t>      fGoodPosAdcPulseTime;
  vector<Double_t>      fGoodPosAdcTdcDiffTime;

  vector<Double_t>      fGoodNegAdcPed;
  vector<Double_t>      fGoodNegAdcPulseInt;
  vector<Double_t>      fGoodNegAdcPulseAmp;
  vector<Double_t>      fGoodNegAdcPulseTime;
 vector<Double_t>       fGoodNegAdcTdcDiffTime;

  vector<Double_t>      fGoodPosAdcPulseIntRaw;
  vector<Double_t>      fGoodNegAdcPulseIntRaw;

  vector<Double_t> fNegAdcSampWaveform;
  vector<Double_t> fPosAdcSampWaveform;
  
  vector<Double_t> fGoodPosAdcMult;
  vector<Double_t> fGoodNegAdcMult;


  vector<Double_t>      fEpos;        // [fNelem] energy depositions seen by positive PMTs
  vector<Double_t>      fEneg;        // [fNelem] energy depositions seen by negative PMTs
  vector<Double_t>      fEmean;        // [fNelem] mean energy depositions (pos + neg)


  Double_t  fEplane_pos;   // Energy deposition in the plane from positive PMTs
  Double_t  fEplane_neg;   // Energy deposition in the plane from negative PMTs
  Double_t  fEplane;

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

  TClonesArray* frPosAdcSampPedRaw;
  TClonesArray* frPosAdcSampPulseIntRaw;
  TClonesArray* frPosAdcSampPulseAmpRaw;
  TClonesArray* frPosAdcSampPulseTimeRaw;
  TClonesArray* frPosAdcSampPed;
  TClonesArray* frPosAdcSampPulseInt;
  TClonesArray* frPosAdcSampPulseAmp;
  TClonesArray* frPosAdcSampPulseTime;
  TClonesArray* frNegAdcSampPedRaw;
  TClonesArray* frNegAdcSampPulseIntRaw;
  TClonesArray* frNegAdcSampPulseAmpRaw;
  TClonesArray* frNegAdcSampPulseTimeRaw;
  TClonesArray* frNegAdcSampPed;
  TClonesArray* frNegAdcSampPulseInt;
  TClonesArray* frNegAdcSampPulseAmp;
  TClonesArray* frNegAdcSampPulseTime;

  TClonesArray* frPosAdcErrorFlag;
  TClonesArray* frPosAdcPedRaw;
  TClonesArray* frPosAdcThreshold;
  TClonesArray* frPosAdcPulseIntRaw;
  TClonesArray* frPosAdcPulseAmpRaw;
  TClonesArray* frPosAdcPulseTimeRaw;

  TClonesArray* frPosAdcPed;
  TClonesArray* frPosAdcPulseInt;
  TClonesArray* frPosAdcPulseAmp;
  TClonesArray* frPosAdcPulseTime;

  TClonesArray* frNegAdcErrorFlag;
  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcThreshold;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;

  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;
  TClonesArray* frNegAdcPulseTime;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );
  virtual void  FillADC_DynamicPedestal( );
  virtual void  FillADC_SampleIntegral( );
  virtual void  FillADC_SampIntDynPed( );
  virtual void  FillADC_Standard( );

  //Quatitites for efficiency calculations.

  Double_t fStatCerMin;
  Double_t fStatSlop;
  Double_t fStatMaxChi2;
  vector<Int_t> fStatNumTrk;
  vector<Int_t> fStatNumHit;
  Int_t fTotStatNumTrk;
  Int_t fTotStatNumHit;

 THcHodoscope* fglHod;		// Hodoscope to get start time
  
  ClassDef(THcShowerPlane,0); // Calorimeter bars in a plane
};
#endif
