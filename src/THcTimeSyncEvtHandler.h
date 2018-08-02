#ifndef THcTimeSyncEvtHandler_
#define THcTimeSyncEvtHandler_

/////////////////////////////////////////////////////////////////////
//
//   THcTimeSyncEvtHandler
//
/////////////////////////////////////////////////////////////////////

#include "THaEvtTypeHandler.h"
#include "Decoder.h"
#include <string>
#include <vector>
#include <map>

//class THaRunBase;
//class THaEvData;

class THcTimeSyncEvtHandler : public THaEvtTypeHandler {

public:

  THcTimeSyncEvtHandler(const char* name, const char* description);
  virtual ~THcTimeSyncEvtHandler();

  virtual Int_t Analyze(THaEvData *evdata);
  virtual EStatus Init( const TDatime& run_time);
  virtual void PrintStats();
  virtual void SetExpectedOffset(Int_t roc, Int_t offset);
  virtual void AddExpectedOffset(Int_t roc, Int_t offset);
  virtual Int_t End( THaRunBase* r=0 );
  virtual Int_t SetRewriteFile(const char *filename);
  virtual void SetBadROC(Int_t roc) {fBadROC = roc;}
  virtual void SetResync(Bool_t b) {fResync = b;}
  virtual void SetBadSyncSizeTrigger(Int_t sizetrigger) {fBadSyncSizeTrigger = sizetrigger;}
  virtual Int_t AllTdcsPresent(UInt_t *bank);
private:

  virtual void InitStats();
  virtual void AccumulateStats(Bool_t sync);

  Bool_t fFirstTime;
  Int_t fMasterRoc; // ROC with the TI master
  Int_t fNEvents;   // Number of events analyzed
  Int_t fSlippage;
  UInt_t fLastEvent[32000];
  UInt_t fSlippingBank[32000];
  Bool_t fWriteDelayed;
  Int_t fDumpNew;
  Int_t fBadROC;		// ROC to check and filter sync problems
  Bool_t fResync;		// If true, stop correcting events on sync
  Int_t fBadSyncSizeTrigger;
  Bool_t fLastEventWasSync;	// True when last event was sync event
  Bool_t fFirstTdcCheck;
  UInt_t fTdcMask;		  // Bit Pattern of TDC in ROC being checked

  Decoder::THaCodaFile* fCodaOut; // The CODA output file
  Int_t handle;

  typedef struct RocTimes {
    Bool_t has_ti_ttime;
    UInt_t ti_ttime;
    UInt_t ti_evcount;
    std::map<Int_t, UInt_t> fadcTimesMap;
    std::map<Int_t, UInt_t> ftdcEvCountMap;
    RocTimes() : has_ti_ttime(kFALSE) {
      fadcTimesMap.clear();
      ftdcEvCountMap.clear();
    }
  } RocTimes_t;

  typedef struct RocStats {
    Int_t ti_ttime_offset;
    Int_t ti_earlyslipcount;
    Int_t ti_lateslipcount;
    Int_t fadc_expected_offset;
    std::map<Int_t, Int_t> fadcOffsetMap;
    std::map<Int_t, Int_t> fadcEarlySlipCountMap;
    std::map<Int_t, Int_t> fadcLateSlipCountMap;
    std::map<Int_t, Int_t> ftdcEvCountWrongMap;
    std::map<Int_t, Int_t> ftdcEvCountOffsetMap;
    RocStats() : ti_ttime_offset(0), ti_earlyslipcount(0), ti_lateslipcount(0),
      fadc_expected_offset(0)
    {
      fadcOffsetMap.clear();
      fadcEarlySlipCountMap.clear();
      fadcLateSlipCountMap.clear();
      ftdcEvCountWrongMap.clear();
      ftdcEvCountOffsetMap.clear();
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
