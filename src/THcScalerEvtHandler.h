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

class HCScalerLoc { // Utility class used by THaScalerEvtHandler
 public:
 HCScalerLoc(TString nm, TString desc, Int_t isc, Int_t ich, Int_t iki) :
   name(nm), description(desc), iscaler(isc), ichan(ich), ikind(iki) { };
  ~HCScalerLoc();
  TString name, description;
  UInt_t iscaler, ichan, ivar, ikind;
};

class THcScalerEvtHandler : public THaEvtTypeHandler {

public:

   THcScalerEvtHandler(const char*, const char*);
   virtual ~THcScalerEvtHandler();

   Int_t Analyze(THaEvData *evdata);
   Int_t AnalyzeBuffer(UInt_t *rdata);
   virtual EStatus Init( const TDatime& run_time);
   virtual Int_t End( THaRunBase* r=0 );
   virtual void SetUseFirstEvent(Bool_t b = kFALSE) {fUseFirstEvent = b;}
   virtual void SetDelayedType(int evtype);
   virtual void SetOnlyBanks(Bool_t b = kFALSE) {fOnlyBanks = b;fRocSet.clear();}

private:

   void AddVars(TString name, TString desc, Int_t iscal, Int_t ichan, Int_t ikind);
   void DefVars();
   static size_t FindNoCase(const std::string& sdata, const std::string& skey);

   std::vector<Decoder::GenScaler*> scalers;
   std::vector<HCScalerLoc*> scalerloc;
   Double_t evcount;
   Int_t Nvars, ifound, fNormIdx, nscalers;
   Double_t *dvars;
   Double_t *dvarsFirst;
   TTree *fScalerTree;
   Bool_t fUseFirstEvent;
   Int_t fDelayedType;
   Bool_t fOnlyBanks;
   std::vector<UInt_t*> fDelayedEvents;
   std::set<UInt_t> fRocSet;
   std::set<UInt_t> fModuleSet;

   THcScalerEvtHandler(const THcScalerEvtHandler& fh);
   THcScalerEvtHandler& operator=(const THcScalerEvtHandler& fh);

   ClassDef(THcScalerEvtHandler,0)  // Scaler Event handler

};

#endif
