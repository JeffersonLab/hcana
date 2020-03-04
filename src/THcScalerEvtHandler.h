#ifndef THcScalerEvtHandler_
#define THcScalerEvtHandler_

/////////////////////////////////////////////////////////////////////
//
//   THcScalerEvtHandler
//   Class to handle Hall C scaler events (type 0)
//   author  Edward Brash (brash@jlab.org)
//   based on THaScalerEvtHandler by Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include "Decoder.h"
#include <string>
#include <vector>
#include <set>
#include "TTree.h"
#include "TString.h"
#include <cstring>


class HCScalerLoc { // Utility class used by THcScalerEvtHandler
 public:
  HCScalerLoc(TString nm, TString desc, UInt_t idx, Int_t s1, UInt_t ich,
	      UInt_t iki, Int_t iv) :
   name(nm), description(desc), index(idx), islot(s1), ichan(ich),
   ikind(iki), ivar(iv) { };
  ~HCScalerLoc() {}
  TString name, description;
  UInt_t index, islot, ichan, ikind, ivar;
};

class THcScalerEvtHandler : public THaEvtTypeHandler {

public:

   THcScalerEvtHandler(const char*, const char*);
   virtual ~THcScalerEvtHandler();

   Int_t Analyze(THaEvData *evdata);
   Int_t AnalyzeBuffer(UInt_t *rdata, Bool_t onlysync);
   virtual EStatus Init( const TDatime& run_time);
   virtual Int_t   ReadDatabase(const TDatime& date );
   virtual Int_t End( THaRunBase* r=0 );
   virtual void SetUseFirstEvent(Bool_t b = kFALSE) {fUseFirstEvent = b;}
   virtual void SetDelayedType(int evtype);
   virtual void SetOnlyBanks(Bool_t b = kFALSE) {fOnlyBanks = b;fRocSet.clear();}
   virtual void SetOnlyUseSyncEvents(Bool_t b=kFALSE) {fOnlySyncEvents = b;}

private:

   void AddVars(TString name, TString desc, UInt_t iscal, UInt_t ichan, UInt_t ikind);
   void DefVars();
   static size_t FindNoCase(const std::string& sdata, const std::string& skey);

   std::vector<Decoder::GenScaler*> scalers;
   std::vector<HCScalerLoc*> scalerloc;
   Int_t fNumBCMs;
   Double_t *fBCM_Gain;
   Double_t *fBCM_Offset;
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
   UInt_t evcount;
   Double_t evcountR;
   UInt_t evNumber;
   Double_t evNumberR;
   Int_t Nvars, ifound, fNormIdx, fNormSlot, nscalers;
   Double_t *dvars;
   UInt_t *dvars_prev_read;
   std::vector<UInt_t> scal_prev_read;
   std::vector<UInt_t> scal_present_read;
   std::vector<UInt_t> scal_overflows;
   Double_t *dvarsFirst;
   TTree *fScalerTree;
   Bool_t fUseFirstEvent;
   Bool_t fOnlySyncEvents;
   Bool_t fOnlyBanks;
   Int_t fDelayedType;
   Int_t fClockChan;
   UInt_t fLastClock;
   Int_t fClockOverflows;
   std::vector<UInt_t*> fDelayedEvents;
   std::set<UInt_t> fRocSet;
   std::set<UInt_t> fModuleSet;

   THcScalerEvtHandler(const THcScalerEvtHandler& fh);
   THcScalerEvtHandler& operator=(const THcScalerEvtHandler& fh);

   ClassDef(THcScalerEvtHandler,0)  // Scaler Event handler

};

#endif
