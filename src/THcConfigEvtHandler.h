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
#include <array>

class THcConfigEvtHandler : public THaEvtTypeHandler {

public:

  THcConfigEvtHandler(const char* name, const char* description);
  THcConfigEvtHandler(const THcConfigEvtHandler& fh) = delete;
  THcConfigEvtHandler& operator=(const THcConfigEvtHandler& fh) = delete;
  virtual ~THcConfigEvtHandler();

  virtual Int_t Analyze( THaEvData* evdata );
  virtual void PrintConfig();
  Bool_t IsPresent( UInt_t crate );
  UInt_t GetNSA( UInt_t crate );
  UInt_t GetNSB( UInt_t crate );
  UInt_t GetNPED( UInt_t crate );
  virtual EStatus Init( const TDatime& run_time );
  //  Float_t GetData(const std::string& tag);
  virtual void MakeParms( UInt_t roc );

private:

  // Number of FADC250 thresholds 
  static constexpr UInt_t NTHR = 16;
  static constexpr UInt_t NPS = 6;
  
  typedef struct crateinfo {
    struct FADC250 {
      FADC250();
      Bool_t present;
      UInt_t blocklevel;
      UInt_t dac_level;
      UInt_t threshold;
      UInt_t mode;
      UInt_t window_lat;
      UInt_t window_width;
      UInt_t nsb;
      UInt_t nsa;
      UInt_t np;
      UInt_t nped;
      UInt_t maxped;
      UInt_t nsat;
      UInt_t nmodules;
      std::map<UInt_t, std::array<UInt_t, NTHR>> thresholds; // slot -> thresholds[16]
   } FADC250;
   struct CAEN1190 {
     CAEN1190();
      Bool_t present;
      UInt_t resolution;
      UInt_t timewindow_offset;
      UInt_t timewindow_width;
   } CAEN1190;
    struct TI {
      TI();
      Bool_t present;
      UInt_t nped;
      UInt_t scaler_period;
      UInt_t sync_count;
      std::array<Int_t, NPS> prescales;
      std::array<Int_t, NPS> ps_factors;
    } TI;
   //CrateInfo : FADC250.nmodules(0),CAEN1190.present(0) {}
  } CrateInfo_t;

  std::map<UInt_t, CrateInfo_t> CrateInfoMap;  // roc -> crate info
  std::vector<std::string> fParms;  // names of parameters we've defined

  ClassDef(THcConfigEvtHandler,0)  // Hall C event type 125

};

#endif
