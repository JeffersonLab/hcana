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
   virtual EStatus Init( const TDatime& run_time);
   virtual Int_t End( THaRunBase* r=0 );


private:

   void AddVars(TString name, TString desc, Int_t iscal, Int_t ichan, Int_t ikind);
   void DefVars();
   static size_t FindNoCase(const std::string& sdata, const std::string& skey);

   std::vector<Decoder::GenScaler*> scalers;
   std::vector<HCScalerLoc*> scalerloc;
   Double_t evcount;
   UInt_t *rdata;
   std::vector<Int_t> index;
   Int_t Nvars, ifound, fNormIdx, nscalers;
   Double_t *dvars;
   Double_t *dvarsFirst;
   TTree *fScalerTree;

   THcScalerEvtHandler(const THcScalerEvtHandler& fh);
   THcScalerEvtHandler& operator=(const THcScalerEvtHandler& fh);

   ClassDef(THcScalerEvtHandler,0)  // Scaler Event handler

};

#endif
