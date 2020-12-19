#ifndef THcHelicityScaler_
#define THcHelicityScaler_

/////////////////////////////////////////////////////////////////////
//
//   THcHelicityScaler
//   class to handle Helicity scaler events
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include "THcScalerEvtHandler.h"
#include "Decoder.h"
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "TTree.h"
#include "TString.h"
#include <cstring>


class THcHelicity;
class HCScalerLoc;

class THcHelicityScaler : public THaEvtTypeHandler {

public:

  THcHelicityScaler(const char*, const char*);
  virtual ~THcHelicityScaler();

  Int_t Analyze(THaEvData *evdata);
  Int_t AnalyzeBuffer(UInt_t *rdata);
  Int_t AnalyzeHelicityScaler(UInt_t *p);
	
  virtual EStatus Init( const TDatime& run_time);
  virtual Int_t   ReadDatabase(const TDatime& date );
  virtual Int_t End( THaRunBase* r=0 );

  virtual void SetUseFirstEvent(Bool_t b = kFALSE) {fUseFirstEvent = b;}
  virtual void SetDelayedType(int evtype);
  virtual void SetROC(Int_t roc) {fROC=roc;}
  virtual void SetBankID(Int_t bankid) {fBankID=bankid;}
  virtual void SetNScalerChannels(Int_t n) {fNScalerChannels = n;}
  virtual Int_t GetNevents() { return fNTrigsInBuf;}
  virtual Int_t GetNcycles() { return fNTriggers;}
  virtual Int_t GetEvNum() { return evNumber;}
  virtual Int_t* GetHelicityHistoryP() {return fHelicityHistory;}
  virtual Int_t GetReportedSeed() {return fRingSeed_reported;}
  virtual Int_t GetReportedActual() {return fRingSeed_actual;}
  virtual Bool_t IsSeedGood() {return fNBits>=30;}

private:

  //------------C.Y. Sep 20, 2020 :: Added Utility Function Prototypes----------------
  void AddVars(TString name, TString desc, UInt_t iscal, UInt_t ichan, UInt_t ikind);
  void DefVars();
  static size_t FindNoCase(const std::string& sdata, const std::string& skey);
  
  std::vector<Decoder::GenScaler*> scalers;
  std::vector<HCScalerLoc*> scalerloc;
  //----------------------------------------------------------------------------------
  
  Int_t RanBit30(Int_t ranseed);
  void MakeParms();

  UInt_t fBankID;
  // Helicity Scaler variables
  Int_t fNTrigsInBuf;		/* # of helicity scaler reads in last event */
  Int_t fNTriggers;
  Int_t fFirstCycle;
  Int_t fHelicityHistory[200];
  //
  Bool_t fUseFirstEvent;
  Int_t fDelayedType;

  Int_t fRingSeed_reported;
  Int_t fRingSeed_actual;
  Int_t fNBits;

  Int_t fNTriggersPlus;
  Int_t fNTriggersMinus;
  Double_t* fHScalers[2];
  Int_t fGateCount[2];
  Double_t *fScalerSums;
  Double_t *fAsymmetry;
  Double_t *fAsymmetryError;
  //Double_t *fCharge;

  Double_t fTimePlus;     
  Double_t fTimeMinus;       
  //Double_t fTime;
  //Double_t fTimeAsymmetry;
  Double_t fTriggerAsymmetry;

  //---- C.Y.: 12/14/2020  Variables for quartet-by-quartet asymmetry/error calculations ----
  Bool_t   fHaveCycle[4];

  Int_t fQuartetCount;      //keep track of number of quartets

  //quartet-by-quartet time asymmetry variables
  Double_t fTimeCycle[4];
  Double_t fTimeSum;
  Double_t fTimeAsymmetry;
  Double_t fTimeAsymmetryError;
  Double_t fTimeAsymSum;
  Double_t fTimeAsymSum2;
  
  //quartet-by-quartet scaler counts asymmetry variables
  Double_t *fScalCycle[4];  
  Double_t *fScalSum;     //reminder: need to initialize
  Double_t *fScalAsymmetry;
  Double_t *fScalAsymmetryError;
  Double_t *fScalAsymSum;
  Double_t *fScalAsymSum2;
  
  //quartet-by-quartet charge asymmetry variables
  Double_t *fChargeCycle[4];
  Double_t *fChargeSum;
  Double_t *fChargeAsymmetry;
  Double_t *fChargeAsymmetryError;
  Double_t *fChargeAsymSum;
  Double_t *fChargeAsymSum2;



  
  //----------------------

  
  
  //----C.Y. Nov 26, 2020----
  Double_t *fScalerChan;

  std::vector<UInt_t*> fDelayedEvents;
  Int_t fROC;
  Int_t fNScalerChannels;	// Number of scaler channels/event

  Int_t fNumBCMs;
  Double_t *fBCM_Gain;
  Double_t *fBCM_Offset;
  //---C.Y. Sep 2020 : Added additional BCM-related variables--
  Double_t *fBCM_SatOffset;
  Double_t *fBCM_SatQuadratic;
  Double_t *fBCM_delta_charge;
  Double_t fTotalTime;
  Double_t fDeltaTime;
  Double_t fPrevTotalTime;
  Double_t fbcm_Current_Threshold;
  Double_t fClockFreq;
  Int_t fbcm_Current_Threshold_Index;
  std::vector <std::string> fBCM_Name;

  //----C.Y. Sep 20, 2020 : Added additional variables-----
  // (required by utility functions and scaler tree output)
  UInt_t evcount;
  Double_t evcountR;
  UInt_t evNumber;
  Double_t evNumberR;
  Double_t actualHelicityR;
  Double_t quartetphaseR;
  Int_t Nvars, ifound, fNormIdx, fNormSlot, nscalers;
  Double_t *dvars;
  Double_t *dvarsFirst;
  TTree *fScalerTree;
  Bool_t fOnlySyncEvents;
  Bool_t fOnlyBanks;
  Int_t fClockChan;
  UInt_t fLastClock;
  std::set<UInt_t> fRocSet;
  std::set<UInt_t> fModuleSet;
  //--------------------------------------------------------
  
  THcHelicityScaler(const THcHelicityScaler& fh);
  THcHelicityScaler& operator=(const THcHelicityScaler& fh);


  ClassDef(THcHelicityScaler,0)  // Scaler Event handler

};

#endif
