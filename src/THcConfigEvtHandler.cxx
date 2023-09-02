/** \class THcConfigEvtHandler
    \ingroup Base

    \brief Analyze Hall C Configuration events.  (Event type 125).

    Hall C Configuration events contain information such as FADC250
    thresholds, mode settings, window settings, pulse mode settings, etc.
    or CAEN1190 window and resolution settings.

    To analyze these events, add

      gHaEvtHandlers->Add (new THcConfigEvtHandler("hallcpre","for evtype 125"));

    to the analysis script.  The first argument ("hallcpre") is an arbitrary
    name that will be used to construct THcParmList type parameters with names
    of the form ghallcpre_*.  These parameters hold the configuration information
    that is found in the events.

    All the configuation data can also be printed out with the PrintConfig method

    \author Stephen Wood (saw@jlab.org)
*/

#include "THcConfigEvtHandler.h"
#include "THaEvData.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "TError.h"
#include <iostream>
#include <iomanip>

using namespace std;

THcConfigEvtHandler::THcConfigEvtHandler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description)
{
}

THcConfigEvtHandler::~THcConfigEvtHandler()
{
  // Remove the parameters we've added to gHcParms. Otherwise they would
  // reference deallocated memory once this object goes away
  for( const auto& name : fParms )
    gHcParms->RemoveName( name.c_str() );

}

//Float_t THcConfigEvtHandler::GetData(const std::string& tag)
//{
//  // A public method which other classes may use
//  if (theDataMap.find(tag) == theDataMap.end())
//    return 0;
// return theDataMap[tag];
//}

Int_t THcConfigEvtHandler::Analyze(THaEvData *evdata)
{
  static const char* const here = "Analyze";

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebug) cout << "------------------\n  Event type 125"<<endl;

  UInt_t evlen = evdata->GetEvLength();
  UInt_t ip = 1; // index of next word to be processed
  UInt_t thisword = evdata->GetRawData(ip++);
  UInt_t roc = thisword & 0xff;
  cout << "THcConfigEvtHandler: " << roc << endl;
  // Using a map as opposed to a vector ensures that none of the iterators into
  // it are invalidated as more elements are added. This is important since we
  // define parameters on _references_ to the map elements (see MakeParms()).
  auto ins = CrateInfoMap.emplace(roc, CrateInfo_t());
  // Three possible blocks of config data
  // 0xdafadcNN - FADC information for the crate (NN = blocklevel)
  // 0xdafadcff - Set of threshold by slot/channel
  // 0xdedc1190 - 1190 TDC information for the crate
  // 0xd0000000 - TI configuration (for TI master crate only)
  while( ip < evlen ) {
    auto& cinfo = ins.first->second; 
    thisword = evdata->GetRawData(ip++);

    if (thisword == 0xdafadcff) {
      thisword = evdata->GetRawData(ip++);
      cout << "ADC thresholds for slots ";
      auto& cfg = cinfo.FADC250;
      while( (thisword & 0xfffff000) == 0xfadcf000 ) {
        UInt_t slot = thisword & 0x1f;
        // Should check if this slot has already been SDC_WIRE_CENTER
        cout << " " << slot;
        auto ithr =
          cfg.thresholds.emplace(slot, std::array<UInt_t, NTHR>{});
        if( ithr.second ) // if insertion successful, then we've added a module
          cfg.nmodules++;
        for( auto& threshold : ithr.first->second )  // assigns NTHR (=16) values 
          threshold = evdata->GetRawData(ip++);
        if( ip >= evlen ) {
          if( ip > evlen ) {
            cout << endl << "Info event truncated" << endl;
          }
          break;
        }
        thisword = evdata->GetRawData(ip++);
      }
      cout << endl;

    } else if( (thisword & 0xffffff00) == 0xdafadc00 ) { // FADC250 information
      cout << "ADC information: Block level " << (thisword&0xff) << endl;
      auto& cfg = cinfo.FADC250;
      cfg.present = true;
      cfg.blocklevel = thisword & 0xff;
      cfg.dac_level = evdata->GetRawData(ip + 1);
      cfg.threshold = evdata->GetRawData(ip + 2);
      cfg.mode = evdata->GetRawData(ip + 3);
      cfg.window_lat = evdata->GetRawData(ip + 4);
      cfg.window_width = evdata->GetRawData(ip + 5);
      cfg.nsb = evdata->GetRawData(ip + 6);
      cfg.nsa = evdata->GetRawData(ip + 7);
      cfg.np = evdata->GetRawData(ip + 8);
      cfg.nped = evdata->GetRawData(ip + 9);
      cfg.maxped = evdata->GetRawData(ip + 10);
      cfg.nsat = evdata->GetRawData(ip + 11);
      ip += 12;

    } else if (thisword == 0xdedc1190) {  // CAEN 1190 information
      cout << "TDC information" << endl;
      auto& cfg = cinfo.CAEN1190;
      cfg.present = true;
      cfg.resolution = evdata->GetRawData(ip + 1);
      cfg.timewindow_offset = evdata->GetRawData(ip + 2);
      cfg.timewindow_width = evdata->GetRawData(ip + 3);
      ip += 5;

    } else if( thisword == 0xd0000000 ) { // TI setup data
      cout << "TI setup data" << endl;
      auto& cfg = cinfo.TI;
      cfg.present = true;
      UInt_t versionword = evdata->GetRawData(ip++);
      if( (versionword & 0xffff0000) == 0xabcd0000 ) {
        UInt_t version = versionword & 0xffff;
        cfg.nped = evdata->GetRawData(ip++);
        if( version >= 2 ) {
          cfg.scaler_period = evdata->GetRawData(ip++);
          cfg.sync_count = evdata->GetRawData(ip++);
        } else {
          cfg.scaler_period = 2;
          cfg.sync_count = -1;
        }
        for( auto& ps: cfg.prescales ) {
          ps = static_cast<Int_t>(evdata->GetRawData(ip++));
        }
        UInt_t lastword = evdata->GetRawData(ip++);
        if( lastword != 0xd000000f ) {
          cout << Here(here) << "Unexpected last word of TI information block "
               << hex << lastword << dec << endl;
        }
      } else {
        cout << Here(here) << "Unexpected TI info word " << hex << thisword << dec << endl;
        cout << "  Expected 0xabcdNNNN" << endl;
      }
    } else {
      cout << Here(here) << "Expected header missing" << endl;
      cout << "pos " << ip-1 << ", data " << hex << thisword << dec << endl;
      // Let's keep looking. The header words are quite unique.
      //break;
    }
  }

  cout << Here(here) << "Making Parms for ROC " << roc << "  Event type " << evdata->GetEvType() << endl;
  MakeParms(roc);

  return 1;
}

void THcConfigEvtHandler::MakeParms( UInt_t roc )
{
  /**
     Add parameters to gHcParms for this roc
  */
  auto it = CrateInfoMap.find(roc);
  assert(it != CrateInfoMap.end());  // else bug in Analyze 
  
  auto& cinfo = it->second;

  // CAEN 1190 TDC information
  if( cinfo.CAEN1190.present ) {
    // FIXME: shouldn't this info be per slot?
    const auto& cfg = cinfo.CAEN1190;
    fParms.emplace_back(Form("g%s_tdc_resolution_%d", fName.Data(), roc));
    gHcParms->Define(fParms.back().c_str(), "TDC resolution", cfg.resolution);
    fParms.emplace_back(Form("g%s_tdc_offset_%d", fName.Data(), roc));
    gHcParms->Define(fParms.back().c_str(), "TDC Time Window Offset", cfg.timewindow_offset);
    fParms.emplace_back(Form("g%s_tdc_width_%d", fName.Data(), roc));
    gHcParms->Define(fParms.back().c_str(), "TDC Time Window Width", cfg.timewindow_width);
  }
  // FADC configuration
  if( cinfo.FADC250.present ) {
    const auto& cfg = cinfo.FADC250;
    // Loop over FADC slots
    for( const auto& islot: cfg.thresholds ) {
      auto slot = islot.first;
      const auto& thresholds = islot.second;

      fParms.emplace_back(Form("g%s_adc_thresholds_%u_%u[%u]", fName.Data(), roc, slot, NTHR));
      gHcParms->Define(fParms.back().c_str(), "ADC Thresholds", *thresholds.data());

      // FIXME: The following parameters are labeled "per slot", but their
      //  values are identical for all slots.
      fParms.emplace_back(Form("g%s_adc_mode_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "ADC Mode", cfg.mode);
      fParms.emplace_back(Form("g%s_adc_latency_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "Window Latency", cfg.window_lat);

      fParms.emplace_back(Form("g%s_adc_width_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "Window Width", cfg.window_width);

      fParms.emplace_back(Form("g%s_adc_daclevel_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "DAC Level", cfg.dac_level);
      fParms.emplace_back(Form("g%s_adc_nped_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "NPED", cfg.nped);
      fParms.emplace_back(Form("g%s_adc_nsa_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "NSA", cfg.nsa);
      fParms.emplace_back(Form("g%s_adc_maxped_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "MAXPED", cfg.maxped);
      fParms.emplace_back(Form("g%s_adc_np_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "NP", cfg.np);
      fParms.emplace_back(Form("g%s_adc_blocklevel_%u_%u", fName.Data(), roc, slot));
      gHcParms->Define(fParms.back().c_str(), "Blocklevel", cfg.blocklevel);
    }
  }
  // TI Configuration
  // We assume that this information is only provided by the master TI crate.
  // If that is not true we will get "Variable XXX already exists." warnings
  if( cinfo.TI.present ) {
    auto& cfg = cinfo.TI;
    fParms.emplace_back(Form("g%s_ti_nped", fName.Data()));
    gHcParms->Define(fParms.back().c_str(),
                     "Number of Pedestal events", cfg.nped);
    fParms.emplace_back(Form("g%s_ti_scaler_period", fName.Data()));
    gHcParms->Define(fParms.back().c_str(),
                     "Number of Pedestal events", cfg.scaler_period);
    fParms.emplace_back(Form("g%s_ti_sync_count", fName.Data()));
    gHcParms->Define(fParms.back().c_str(),
                     "Number of Pedestal events", cfg.sync_count);

    // Calculate actual prescale factors from "exponent" format
    for( UInt_t i = 0; i < NPS; i++ ) {
      auto ps = cfg.prescales[i];
      auto& fact = cfg.ps_factors[i];
      if( ps > 0 ) {
        fact = (1 << (ps - 1)) + 1;
      } else if( ps == 0 ) {
        fact = 1;
      } else {
        fact = -1;
      }
    }
    fParms.emplace_back(Form("g%s_ti_ps[%d]", fName.Data(), NPS));
    gHcParms->Define(fParms.back().c_str(), "TI Event Prescale Internal Value", *cfg.prescales.data());
    fParms.emplace_back(Form("g%s_ti_ps_factors[%d]", fName.Data(), NPS));
    gHcParms->Define(fParms.back().c_str(), "TI Event Prescale Factor", *cfg.ps_factors.data());
  }
}
    
void THcConfigEvtHandler::PrintConfig()
{
/**
  Stub of method to pretty print the config data
*/
  for( const auto& item : CrateInfoMap ) {
    UInt_t roc = item.first;
    cout << "================= Configuration Data ROC " << roc << "==================" << endl;
    const auto& cinfo = item.second;
    if( cinfo.CAEN1190.present ) {
      const auto& cfg = cinfo.CAEN1190; 
      cout << "    CAEN 1190 Configuration" << endl;
      cout << "        Resolution: " << cfg.resolution << " ps" << endl;
      cout << "        T Offset:   " << cfg.timewindow_offset << endl;
      cout << "        T Width:    " << cfg.timewindow_width << endl;
    }
    if( cinfo.FADC250.present ) {
      const auto& cfg = cinfo.FADC250;
      cout << "    FADC250 Configuration" << endl;
      cout << "       Mode:   " << cfg.mode << endl;
      cout << "       Latency: " << cfg.window_lat << "  Width: "<< cfg.window_width << endl;
      cout << "       DAC Level: " << cfg.dac_level << "  Threshold: " << cfg.threshold << endl;
      cout << "       NPED: " << cfg.nped << "  NSA: " << cfg.nsa << "  NSB: " << cfg.nsb << endl;
      cout << "       MAXPED: " << cfg.maxped << "  NP: " << cfg.np << "  NSAT: " << cfg.nsat << endl;

      // Loop over FADC slots
      cout << "       Thresholds";
      for( const auto& thr : cfg.thresholds ) {
        auto slot = thr.first;
        cout << " " << setw(5) << slot;
      }
      cout << endl;
      for( UInt_t ichan = 0; ichan < NTHR; ichan++ ) {
        cout << "           " << setw(2) << ichan << "    ";
        auto ithr = cfg.thresholds.begin();
        while( ithr != cfg.thresholds.end() ) {
          const auto& thresholds = ithr->second;
          cout << " " << setw(5) << thresholds[ichan];
          ++ithr;
        }
        cout << endl;
      }
    }
    if( cinfo.TI.present ) {
      const auto& cfg = cinfo.TI;
      cout << "    TI Configuration" << endl;
      cout << "        N Pedestals:   " << cfg.nped << " events" << endl;
      cout << "        Scaler Period: " << cfg.scaler_period << " seconds" << endl;
      cout << "        Sync interval: " << cfg.sync_count << " events" << endl;
      cout << "        Prescales:    ";
      for( UInt_t i = 0; i < NPS; i++ ) {
        cout << " " << cfg.prescales[i];
      }
      cout << endl;
    }
  }
}

Bool_t THcConfigEvtHandler::IsPresent( UInt_t crate)
{
  auto it = CrateInfoMap.find(crate);
  if( it != CrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    return cinfo.FADC250.present;
  }
  return false;
}
UInt_t THcConfigEvtHandler::GetNSA( UInt_t crate)
{
  auto it = CrateInfoMap.find(crate);
  if( it != CrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if(cinfo.FADC250.present > 0) return(cinfo.FADC250.nsa);
  }
  return(-1);
}
UInt_t THcConfigEvtHandler::GetNSB( UInt_t crate)
{
  auto it = CrateInfoMap.find(crate);
  if( it != CrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if( cinfo.FADC250.present > 0 )
      return cinfo.FADC250.nsb;
  }
  return(-1);
}
UInt_t THcConfigEvtHandler::GetNPED( UInt_t crate)
{
  auto it = CrateInfoMap.find(crate);
  if( it != CrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if(cinfo.FADC250.present > 0) return(cinfo.FADC250.nped);
  }
  return(-1);
}
THaAnalysisObject::EStatus THcConfigEvtHandler::Init(const TDatime& date)
{
  if( eventtypes.empty() ) {
    eventtypes.push_back(125);  // what events to look for
  }

  CrateInfoMap.clear();
  
  return THaEvtTypeHandler::Init(date);
}

THcConfigEvtHandler::CrateInfo_t::FADC250::FADC250()
  : present{false}
  , blocklevel{0}
  , dac_level{0}
  , threshold{0}
  , mode{0}
  , window_lat{0}
  , window_width{0}
  , nsb{0}
  , nsa{0}
  , np{0}
  , nped{0}
  , maxped{0}
  , nsat{0}
  , nmodules{0}
{}

THcConfigEvtHandler::CrateInfo_t::CAEN1190::CAEN1190()
  : present{false}
  , resolution{0}
  , timewindow_offset{0}
  , timewindow_width{0}
{}

THcConfigEvtHandler::CrateInfo_t::TI::TI()
  : present{false}
  , nped{0}
  , scaler_period{0}
  , sync_count{0}
  , prescales{}
  , ps_factors{}
{
  for( auto& ps : prescales )
    ps = 0;
}

ClassImp(THcConfigEvtHandler)
