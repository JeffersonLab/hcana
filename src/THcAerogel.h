#ifndef ROOT_THcAerogel
#define ROOT_THcAerogel

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcAerogel                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcAerogelHit.h"
class THcHodoscope;

class THcAerogel : public THaNonTrackingDetector, public THcHitList {

 public:
  THcAerogel(const char* name, const char* description = "", THaApparatus* a = NULL);
  virtual ~THcAerogel();

  virtual void 	  Clear(Option_t* opt="");
  virtual void    Print(const Option_t* opt) const;
  virtual void    AccumulatePedestals(TClonesArray* rawhits);
  virtual void    CalculatePedestals();
  virtual Int_t   Decode(const THaEvData&);
  virtual Int_t   ReadDatabase(const TDatime& date);
  virtual Int_t   DefineVariables(EMode mode = kDefine);
  virtual Int_t   CoarseProcess(TClonesArray& tracks);
  virtual Int_t   FineProcess(TClonesArray& tracks);
  virtual Int_t   ApplyCorrections(void);
  virtual EStatus Init(const TDatime& run_time);
  Int_t           End(THaRunBase* run=0);

  Int_t GetIndex(Int_t nRegion, Int_t nValue);

  THcAerogel();  // for ROOT I/O

 protected:

  // Event information
  Int_t fNhits;
  Bool_t* fPresentP;

  Int_t fADC_RefTimeCut;

  // 12 GeV variables
  // Vector/TClonesArray length parameters
  static const Int_t MaxNumAdcPulse   = 4;
  // Tracking variables
  Int_t     fNRegions;
  Int_t     fRegionsValueMax;
  Int_t  fOutputSampWaveform;
  Int_t  fUseSampWaveform;
  Double_t  fSampThreshold;
  Int_t  fSampNSA;
  Int_t  fSampNSAT;
  Int_t  fSampNSB;
  Int_t     fDebugAdc;
  Double_t  fRedChi2Min;
  Double_t  fRedChi2Max;
  Double_t  fBetaMin;
  Double_t  fBetaMax;
  Double_t  fENormMin;
  Double_t  fENormMax;
  Double_t  fDpMin;
  Double_t  fDpMax;
  Double_t  fDiffBoxZPos;
  Double_t  fXAtAero;
  Double_t  fYAtAero;
  Double_t  fNpeThresh;

  ////  Double_t  fAdcTimeWindowMin;
  ////  Double_t  fAdcTimeWindowMax;
  Double_t  *fAdcPosTimeWindowMin;
  Double_t  *fAdcPosTimeWindowMax;
  Double_t  *fAdcNegTimeWindowMin;
  Double_t  *fAdcNegTimeWindowMax;
  Int_t*  fPedNegDefault;
  Int_t*  fPedPosDefault;

  Double_t  fAdcTdcOffset;
  Double_t  *fRegionValue;
  // Counting variables
  Int_t     fTotNumAdcHits;
  Int_t     fTotNumGoodAdcHits;
  Int_t     fTotNumPosAdcHits;
  Int_t     fTotNumGoodPosAdcHits;
  Int_t     fTotNumNegAdcHits;
  Int_t     fTotNumGoodNegAdcHits;
  Int_t     fTotNumTracksMatched;
  Int_t     fTotNumTracksFired;
  // NPE variables
  Double_t  fPosNpeSum;
  Double_t  fNegNpeSum;
  Double_t  fNpeSum;
  Double_t  *fPosGain;
  Double_t  *fNegGain;
  // FADC data objects
  TClonesArray* frPosAdcPedRaw;
  TClonesArray* frPosAdcPulseIntRaw;
  TClonesArray* frPosAdcPulseAmpRaw;
  TClonesArray* frPosAdcPulseTimeRaw;
  TClonesArray* frPosAdcPed;
  TClonesArray* frPosAdcPulseInt;
  TClonesArray* frPosAdcPulseAmp;
  TClonesArray* frPosAdcPulseTime;
  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;
  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;
  TClonesArray* frNegAdcPulseTime;
  TClonesArray* fPosAdcErrorFlag;
  TClonesArray* fNegAdcErrorFlag;
  // 12 Gev FADC Sample variables
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
  // Individual PMT data objects
  vector<Int_t>    fNumPosAdcHits;
  vector<Int_t>    fNumNegAdcHits;
  vector<Int_t>    fNumGoodPosAdcHits;
  vector<Int_t>    fNumGoodNegAdcHits;
  vector<Int_t>    fNumTracksMatched;
  vector<Int_t>    fNumTracksFired;
  vector<Double_t> fPosNpe;
  vector<Double_t> fNegNpe;
  vector<Double_t> fGoodPosAdcPed;
  vector<Double_t> fGoodPosAdcMult;
  vector<Double_t> fGoodPosAdcPulseInt;
  vector<Double_t> fGoodPosAdcPulseIntRaw;
  vector<Double_t> fGoodPosAdcPulseAmp;
  vector<Double_t> fGoodPosAdcPulseTime;
  vector<Double_t> fGoodPosAdcTdcDiffTime;
  vector<Double_t> fGoodNegAdcPed;
  vector<Double_t> fGoodNegAdcMult;
  vector<Double_t> fGoodNegAdcPulseInt;
  vector<Double_t> fGoodNegAdcPulseIntRaw;
  vector<Double_t> fGoodNegAdcPulseAmp;
  vector<Double_t> fGoodNegAdcPulseTime;
  vector<Double_t> fGoodNegAdcTdcDiffTime;
  vector<Double_t> fNegAdcSampWaveform;
  vector<Double_t> fPosAdcSampWaveform;

  // 6 GeV era variables
  Int_t     fAnalyzePedestals;
  Int_t     fSixGevData;
  Int_t     fNGoodHits;
  Int_t     fNADCPosHits;
  Int_t     fNADCNegHits;
  Int_t     fNTDCPosHits;
  Int_t     fNTDCNegHits;
  Int_t     fTdcOffset; /* Global TDC offset */
  Int_t     fNPedestalEvents;
  Int_t     fMinPeds;
  Double_t  fPosNpeSumSixGev;
  Double_t  fNegNpeSumSixGev;
  Double_t  fNpeSumSixGev;
  Int_t    *fPosPedSum;		/* Accumulators for pedestals */
  Int_t    *fPosPedSum2;
  Int_t    *fPosPedLimit;
  Int_t    *fPosPedCount;
  Int_t    *fNegPedSum;
  Int_t    *fNegPedSum2;
  Int_t    *fNegPedLimit;
  Int_t    *fNegPedCount;
  Float_t  *fA_Pos;          // [fNelem] Array of ADC amplitudes
  Float_t  *fA_Neg;          // [fNelem] Array of ADC amplitudes
  Float_t  *fA_Pos_p;	     // [fNelem] Array of ped-subtracted ADC amplitudes
  Float_t  *fA_Neg_p;	     // [fNelem] Array of ped-subtracted ADC amplitudes
  Float_t  *fT_Pos;          // [fNelem] Array of TDCs
  Float_t  *fT_Neg;          // [fNelem] Array of TDCs
  Double_t *fPosPed;
  Double_t *fPosSig;
  Double_t *fPosThresh;
  Double_t *fNegPed;
  Double_t *fNegSig;
  Double_t *fNegThresh;
  Double_t *fPosPedMean; 	/* Can be supplied in parameters and then */
  Double_t *fNegPedMean;	/* be overwritten from ped analysis */

  TClonesArray *fPosTDCHits;
  TClonesArray *fNegTDCHits;
  TClonesArray *fPosADCHits;
  TClonesArray *fNegADCHits;

  vector<Double_t> fPosNpeSixGev;
  vector<Double_t> fNegNpeSixGev;

  void Setup(const char* name, const char* description);
  void  DeleteArrays();
  void  DeletePedestalArrays();
  virtual void  InitializePedestals( );
  THcHodoscope* fglHod;		// Hodoscope to get start time

  ClassDef(THcAerogel,0)   // Generic aerogel class
}
;

#endif
