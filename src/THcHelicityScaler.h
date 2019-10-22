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
   Int_t AnalyzeBuffer(UInt_t *rdata, Bool_t onlysync);
   Int_t AnalyzeHelicityScaler(UInt_t *p);
	
   virtual EStatus Init( const TDatime& run_time);
   virtual Int_t   ReadDatabase(const TDatime& date );
   virtual Int_t End( THaRunBase* r=0 );

   virtual void SetUseFirstEvent(Bool_t b = kFALSE) {fUseFirstEvent = b;}
   virtual void SetDelayedType(int evtype);
   virtual void SetROC(Int_t roc) {fROC=roc;}
   virtual void SetBankID(Int_t bankid) {fBankID=bankid;}
   virtual void SetHelicityDetector(THcHelicity *f) {fglHelicityDetector = f;}
   virtual Int_t GetNevents() { return fNevents;}
   virtual Int_t GetNcycles() { return fNcycles;}
   virtual Int_t GetEvNum() { return evNumber;}
   virtual Int_t* GetHelicityHistoryP() {return fHelicityHistory;}

private:

   static size_t FindNoCase(const std::string& sdata, const std::string& skey);

   Int_t fNumBCMs;
   Double_t *fBCM_Gain;
   Double_t *fBCM_Offset;
   Double_t *fBCM_delta_charge;
   
   Int_t fROC;
   UInt_t fBankID;
   // Helicity Scaler variables
   Int_t fNevents;		/* # of helicity scaler reads in last event */
   Int_t fNcycles;
   Int_t fHelicityHistory[200];
   //
   Bool_t fUseFirstEvent;
   Bool_t fOnlySyncEvents;
   Bool_t fOnlyBanks;
   Int_t fDelayedType;
   Int_t fClockChan;
   UInt_t fLastClock;
   Int_t fClockOverflows;

   std::vector<UInt_t*> fDelayedEvents;
   std::set<UInt_t> fRocSet;

   THcHelicityScaler(const THcHelicityScaler& fh);
   THcHelicityScaler& operator=(const THcHelicityScaler& fh);

   UInt_t evcount;
   Double_t evcountR;
   UInt_t evNumber;
   Int_t ifound;
   THcHelicity *fglHelicityDetector;

   ClassDef(THcHelicityScaler,0)  // Scaler Event handler

};

#endif
