#ifndef THcConfigEvtHandler_
#define THcConfigEvtHandler_

/////////////////////////////////////////////////////////////////////
//
//   THcConfigEvtHandler
//   For more details see the implementation file.
//   This handles hall C's event type 125.
//   Author Stephen Wood, Feb 2017 - Dec 2018.
//   Partly rewritten by Ole Hansen, Aug/Sep 2023.
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

  struct CrateConfig {
    CrateConfig() : CrateConfig(0) {}
    explicit CrateConfig( UInt_t crate ) : roc{crate} {}
    Int_t MakeParms( THcConfigEvtHandler& h ) const;
    void Print() const;
    UInt_t roc;
    typedef struct FADC250 {
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
    } FADC250_t;
    FADC250_t FADC250;
    typedef struct CAEN1190 {
      CAEN1190();
      Bool_t present;
      UInt_t resolution;
      UInt_t timewindow_offset;
      UInt_t timewindow_width;
    } CAEN1190_t;
    CAEN1190_t CAEN1190;
    typedef struct TI {
      TI();
      Bool_t present;
      UInt_t nped;
      UInt_t scaler_period;
      UInt_t sync_count;
      std::array<Int_t, NPS> prescales;
      std::array<Int_t, NPS> ps_factors;
    } TI_t;
    TI_t TI;
  };

  std::map<UInt_t, CrateConfig> fCrateInfoMap;  // roc -> crate info
  std::vector<std::string> fParms;  // names of parameters we've defined

  UInt_t DecodeFADC250Config( THaEvData* evdata, UInt_t ip,
                              CrateConfig::FADC250_t& cfg );
  UInt_t DecodeCAEN1190Config( THaEvData* evdata, UInt_t ip,
                               CrateConfig::CAEN1190_t& cfg );
  UInt_t DecodeTIConfig( THaEvData* evdata, UInt_t ip,
                         CrateConfig::TI_t& cfg );

  ClassDef(THcConfigEvtHandler,0)  // Hall C event type 125

};

#endif
