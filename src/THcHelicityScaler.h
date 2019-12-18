#ifndef THcHelicityScaler_
#define THcHelicityScaler_

/////////////////////////////////////////////////////////////////////
//
//   THcHelicityScaler
//   class to handle Helicity scaler events
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include "Decoder.h"
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "TTree.h"
#include "TString.h"
#include <cstring>

class THcHelicity;

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
  Double_t *fCharge;
  Double_t *fChargeAsymmetry;
  Double_t fTime;
  Double_t fTimeAsymmetry;
  Double_t fTriggerAsymmetry;

  std::vector<UInt_t*> fDelayedEvents;
  Int_t fROC;
  Int_t fNScalerChannels;	// Number of scaler channels/event

  Int_t fNumBCMs;
  Double_t *fBCM_Gain;
  Double_t *fBCM_Offset;
  std::vector <std::string> fBCM_Name;

  THcHelicityScaler(const THcHelicityScaler& fh);
  THcHelicityScaler& operator=(const THcHelicityScaler& fh);

  UInt_t evNumber;

  ClassDef(THcHelicityScaler,0)  // Scaler Event handler

};

#endif
