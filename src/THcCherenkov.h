#ifndef ROOT_THcCherenkov
#define ROOT_THcCherenkov

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Cherenkov                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcCherenkovHit.h"
class THcHodoscope;

class THcCherenkov : public THaNonTrackingDetector, public THcHitList {

 public:
  THcCherenkov(const char* name, const char* description = "", THaApparatus* a = NULL);
  virtual ~THcCherenkov();

  virtual void 	  Clear(Option_t* opt="");
  virtual void    Print(const Option_t* opt) const;
  virtual void    AccumulatePedestals(TClonesArray* rawhits);
  virtual void    CalculatePedestals();
  virtual Int_t   Decode(const THaEvData&);
  virtual Int_t   ReadDatabase(const TDatime& date);
  virtual Int_t   DefineVariables(EMode mode = kDefine);
  virtual Int_t   CoarseProcess(TClonesArray& tracks);
  virtual Int_t   FineProcess(TClonesArray& tracks);
  virtual Int_t   ApplyCorrections( void );
  virtual EStatus Init(const TDatime& run_time);
  Int_t           End(THaRunBase* run);


  void  InitArrays();
  void  DeleteArrays();
  Int_t GetIndex(Int_t nRegion, Int_t nValue);

  //  Double_t GetCerNPE() { return fNPEsum;}
  Double_t GetCerNPE();

  // Vector/TClonesArray length parameters
  static const Int_t MaxNumCerPmt   = 4;
  static const Int_t MaxNumAdcPulse = 4;

  THcCherenkov();  // for ROOT I/O
 protected:
  Bool_t*   fPresentP;
  Int_t     fAnalyzePedestals;
  Int_t     fDebugAdc;
  Double_t* fWidth;

  Int_t     fADC_RefTimeCut;

  Int_t     fNhits;
  Int_t     fTotNumAdcHits;
  Int_t     fTotNumGoodAdcHits;
  Int_t     fTotNumTracksMatched;
  Int_t     fTotNumTracksFired;
  Double_t  fNpeSum;
  Double_t* fGain;

  vector<Int_t>    fNumAdcHits;
  vector<Int_t>    fNumGoodAdcHits;
  vector<Int_t>    fNumTracksMatched;
  vector<Int_t>    fNumTracksFired;
  vector<Double_t> fGoodAdcPed;
  vector<Double_t> fGoodAdcMult;
  vector<Double_t> fGoodAdcHitUsed;
  vector<Double_t> fGoodAdcPulseInt;
  vector<Double_t> fGoodAdcPulseIntRaw;
  vector<Double_t> fGoodAdcPulseAmp;
  vector<Double_t> fGoodAdcPulseTime;
  vector<Double_t> fGoodAdcTdcDiffTime;
  vector<Double_t> fNpe;

  Int_t     fNRegions;
  Int_t     fRegionsValueMax;
  Double_t  fRedChi2Min;
  Double_t  fRedChi2Max;
  Double_t  fBetaMin;
  Double_t  fBetaMax;
  Double_t  fENormMin;
  Double_t  fENormMax;
  Double_t  fDpMin;
  Double_t  fDpMax;
  Double_t  fMirrorZPos;
  Double_t  fXAtCer;
  Double_t  fYAtCer;
  Double_t  fNpeThresh;
  Double_t*  fAdcTimeWindowMin;
  Double_t*  fAdcTimeWindowMax;
  Double_t  fAdcTdcOffset;
  Double_t* fRegionValue;

  // 6 Gev pedestal variables
  Int_t     fNPedestalEvents;
  Int_t     fMinPeds;
  Int_t*    fPedSum;	  /* Accumulators for pedestals */
  Int_t*    fPedSum2;
  Int_t*    fPedLimit;
  Int_t*    fPedCount;
  Double_t* fPedMean; 	  /* Can be supplied in parameters and then */
  Double_t* fPed;
  Double_t* fThresh;
  Double_t* fAdcPulseAmpTest;
  Int_t*    fAdcGoodElem;

  // 12 Gev FADC variables
  TClonesArray* frAdcPedRaw;
  TClonesArray* frAdcPulseIntRaw;
  TClonesArray* frAdcPulseAmpRaw;
  TClonesArray* frAdcPulseTimeRaw;
  TClonesArray* frAdcPed;
  TClonesArray* frAdcPulseInt;
  TClonesArray* frAdcPulseAmp;
  TClonesArray* frAdcPulseTime;
  TClonesArray* fAdcErrorFlag;

  void Setup(const char* name, const char* description);
  virtual void  InitializePedestals( );
 THcHodoscope* fglHod;		// Hodoscope to get start time

  ClassDef(THcCherenkov,0)        // Generic cherenkov class
};

#endif
