#ifndef THcConfigEvtHandler_
#define THcConfigEvtHandler_

/////////////////////////////////////////////////////////////////////
//
//   THcConfigEvtHandler
//   For more details see the implementation file.
//   This handles hall C's event type 125.
//   author  Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include <string>
#include <vector>
#include <map>

class THcConfigEvtHandler : public THaEvtTypeHandler {

public:

  THcConfigEvtHandler(const char* name, const char* description);
  virtual ~THcConfigEvtHandler();

  virtual Int_t Analyze(THaEvData *evdata);
  virtual void AddEventType(Int_t evtype);
  virtual void PrintConfig();
  virtual Int_t IsPresent(Int_t crate);
  virtual Int_t GetNSA(Int_t crate);
  virtual Int_t GetNSB(Int_t crate);
  virtual Int_t GetNPED(Int_t crate);
  virtual EStatus Init( const TDatime& run_time);
 //  Float_t GetData(const std::string& tag);
  virtual void MakeParms(Int_t roc);

private:

  typedef struct {
    struct FADC250 {
      Int_t present;
      Int_t dac_level;
      Int_t threshold;
      Int_t mode;
      Int_t window_lat;
      Int_t window_width;
      Int_t nsb;
      Int_t nsa;
      Int_t np;
      Int_t nped;
      Int_t maxped;
      Int_t nsat;
      Int_t nmodules;
      Int_t blocklevel;
      std::map<Int_t, Int_t *> thresholds;
   } FADC250;
   struct CAEN1190 {
      Int_t present;
      Int_t resolution;
      Int_t timewindow_offset;
      Int_t timewindow_width;
   } CAEN1190;
    struct TI {
      Int_t present;
      Int_t nped;
      Int_t scaler_period;
      Int_t sync_count;
      Int_t num_prescales;
      Int_t prescales[6];
    } TI;
   //CrateInfo : FADC250.nmodules(0),CAEN1190.present(0) {}
  } CrateInfo_t;

  std::map<Int_t, CrateInfo_t *> CrateInfoMap;

  void DeleteCrateInfoMap();

  THcConfigEvtHandler(const THcConfigEvtHandler& fh);
  THcConfigEvtHandler& operator=(const THcConfigEvtHandler& fh);

  ClassDef(THcConfigEvtHandler,0)  // Hall C event type 125

};

#endif
