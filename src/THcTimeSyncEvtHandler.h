#ifndef THcTimeSyncEvtHandler_
#define THcTimeSyncEvtHandler_

/////////////////////////////////////////////////////////////////////
//
//   THcTimeSyncEvtHandler
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include <string>
#include <vector>
#include <map>

class THcTimeSyncEvtHandler : public THaEvtTypeHandler {

public:

  THcTimeSyncEvtHandler(const char* name, const char* description);
  virtual ~THcTimeSyncEvtHandler();

  virtual Int_t Analyze(THaEvData *evdata);
  virtual EStatus Init( const TDatime& run_time);
  virtual void PrintStats();
  virtual void SetExpectedOffset(Int_t roc, Int_t offset);
  virtual void AddExpectedOffset(Int_t roc, Int_t offset);

 //  Float_t GetData(const std::string& tag);

private:

  virtual void InitStats();
  virtual void AccumulateStats();

  Bool_t fFirstTime;
  Int_t fMasterRoc; // ROC with the TI master
  Int_t fNEvents;   // Number of events analyzed

  typedef struct RocTimes {
    Bool_t has_ti_ttime;
    UInt_t ti_ttime;
    std::map<Int_t, UInt_t> fadcTimesMap;
    RocTimes() : has_ti_ttime(kFALSE) {fadcTimesMap.clear();}
  } RocTimes_t;

  typedef struct RocStats {
    Int_t ti_ttime_offset;
    Int_t ti_earlyslipcount;
    Int_t ti_lateslipcount;
    Int_t fadc_expected_offset;
    std::map<Int_t, Int_t> fadcOffsetMap;
    std::map<Int_t, Int_t> fadcEarlySlipCountMap;
    std::map<Int_t, Int_t> fadcLateSlipCountMap;
    RocStats() : ti_ttime_offset(0), ti_earlyslipcount(0), ti_lateslipcount(0),
      fadc_expected_offset(0)
    {
      fadcOffsetMap.clear();
      fadcEarlySlipCountMap.clear();
      fadcLateSlipCountMap.clear();
    }
  } RocStats_t;

  std::map<Int_t, RocTimes_t *> CrateTimeMap;
  std::map<Int_t, RocStats_t *> CrateStatsMap;
  std::map<Int_t, Int_t> ExpectedOffsetMap;

  THcTimeSyncEvtHandler(const THcTimeSyncEvtHandler& fh);
  THcTimeSyncEvtHandler& operator=(const THcTimeSyncEvtHandler& fh);

  ClassDef(THcTimeSyncEvtHandler,0)  // Hall C event type 125

};

#endif
