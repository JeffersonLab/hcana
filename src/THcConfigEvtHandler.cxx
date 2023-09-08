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
#include "CodaDecoder.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THaGlobals.h"
#include "THaRunBase.h"
#include "THaRunParameters.h"
#include "Helper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

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
  static const char* const here = "THcConfigEvtHandler::Analyze";

  if ( !IsMyEvent(evdata->GetEvType()) )
    return -1;
  if (fDebug)
    cout << "------------------\n  Event type 125"<<endl;
  const char* whereami = Here(here);

  UInt_t evlen = evdata->GetEvLength();
  UInt_t ip = 1; // index of next word to be processed (for CODA 2)

  if( evdata->GetDataVersion() > 2 ) {
    // Unpack CODA 3 bank structure
    auto bankinfo =
      Decoder::CodaDecoder::GetBank(evdata->GetRawDataBuffer(), 0, evlen);
    ip = bankinfo.pos_;

    // Check for errors
    if( bankinfo.status_ != Decoder::CodaDecoder::BankInfo::kOK ) {
      ostringstream ostr;
      ostr << whereami << ": CODA3 bank decoding error \""
           << bankinfo.Errtxt() << "\"";
      throw Decoder::CodaDecoder::coda_format_error(ostr.str() );
    }
    if( bankinfo.GetDataSize() != Decoder::CodaDecoder::BankInfo::k32bit ||
        bankinfo.GetFloat() != Decoder::CodaDecoder::BankInfo::kInteger ) {
      ostringstream ostr;
      ostr << whereami << ": CODA3 bank does not contain 32-bit integer "
                            "data, found " << bankinfo.Typtxt();
      throw Decoder::CodaDecoder::coda_format_error(ostr.str() );
    }
    if( ip + bankinfo.len_ > evlen ) {
      ostringstream ostr;
      ostr << whereami << ": CODA3 bank size too large, pos+len = "
           << ip+bankinfo.len_ << ", evlen = " << evlen;
      throw Decoder::CodaDecoder::coda_format_error( ostr.str() );
    }
  }

  UInt_t header = evdata->GetRawData(ip++);
  UInt_t roc = header & 0xff;
  if (fDebug)
    cout << whereami << ": Found config for ROC " << roc << endl;
  // Using a map as opposed to a vector ensures that none of the iterators into
  // it are invalidated as more elements are added. This is important since we
  // define parameters on _references_ to the map elements (see MakeParms()).
  auto ins = fCrateInfoMap.emplace(roc, CrateConfig(roc));
  auto& cinfo = ins.first->second;

  // Possible blocks of config data
  // 0xdafadcNN - FADC information for the crate (NN = blocklevel), optionally
  //              followed by set of thresholds by slot/channel
  // 0xdedc1190 - 1190 TDC information for the crate
  // 0xd0000000 - TI configuration (for TI master crate only)
  while( ip < evlen ) {
    UInt_t thisword = evdata->GetRawData(ip);

    if( (thisword & 0xffffff00) == 0xdafadc00 ) {
      // FADC250 information
      ip = DecodeFADC250Config(evdata, ip, cinfo.FADC250);

    } else if( thisword == 0xdedc1190 ) {
      // CAEN 1190 information
      ip = DecodeCAEN1190Config(evdata, ip, cinfo.CAEN1190);

    } else if( thisword == 0xd0000000 ) {
      // TI setup data
      ip = DecodeTIConfig(evdata, ip, cinfo.TI);

    } else {
      cerr << whereami << ": Expected header missing"
           << "pos " << ip << ", data " << hex << thisword << dec << endl;
      break;
    }
  }

  if( fDebug > 1 )
    cout << whereami << ": Making Parms for ROC " << roc << "  Event type " << evdata->GetEvType() << endl;
  cinfo.MakeParms(*this);

  // Copy prescales to run parameters stored in the run object.
  // The run object will be saved in the output ROOT file, so
  // users will have easy access to these numbers.
  const auto& TI = cinfo.TI;
  if( gHaRun && TI.present ) {
    auto& prescales = gHaRun->GetParameters()->Prescales();
    for( Int_t i = 0; i < Podd::SSIZE(TI.ps_factors); ++i ) {
      prescales[i] = TI.ps_factors[i];
    }
  }

  if( fDebug > 1 )
    PrintConfig();
  return 1;
}

UInt_t THcConfigEvtHandler::DecodeFADC250Config( THaEvData* evdata, UInt_t ip,
                                                 CrateConfig::FADC250_t& cfg )
{
  static const char* here = "THcConfigEvtHandler::DecodeFADC250Config";
  const char* whereami = Here(here);

  UInt_t thisword = evdata->GetRawData(ip);
  if( fDebug )
    cout << whereami << ": FADC250 config: Block level " << (thisword & 0xff) << endl;
  UInt_t versionword = evdata->GetRawData(ip + 1);
  if( (versionword & 0xffff0000) == 0xabcd0000 ) {
    //UInt_t version = versionword & 0xffff;
    cfg.present = true;
    cfg.blocklevel = thisword & 0xff;
    cfg.dac_level = evdata->GetRawData(ip + 2);
    cfg.threshold = evdata->GetRawData(ip + 3);
    cfg.mode = evdata->GetRawData(ip + 4);
    cfg.window_lat = evdata->GetRawData(ip + 5);
    cfg.window_width = evdata->GetRawData(ip + 6);
    cfg.nsb = evdata->GetRawData(ip + 7);
    cfg.nsa = evdata->GetRawData(ip + 8);
    cfg.np = evdata->GetRawData(ip + 9);
    cfg.nped = evdata->GetRawData(ip + 10);
    cfg.maxped = evdata->GetRawData(ip + 11);
    cfg.nsat = evdata->GetRawData(ip + 12);
    UInt_t lastword = evdata->GetRawData(ip + 13);
    if( lastword != 0xdafadcff ) {
      cerr << whereami << ": Unexpected FADC250 configuration block trailer"
           << ", expected " << hex << 0xdafadcff << ", got " << lastword << dec << endl;
    }
  } else {
    cerr << whereami << ": Unexpected FADC250 version word "
         << hex << thisword << dec << ", expected 0xabcdNNNN" << endl;
  }
  ip += 14;

  // A set of per-slot ADC thresholds may follow
  thisword = evdata->GetRawData(ip);
  if( (thisword & 0xffffff00) == 0xfadcf000 ) {
    if( fDebug )
      cout << whereami << ": FADC250 thresholds for slot";
    while( (thisword & 0xffffff00) == 0xfadcf000 ) {
      UInt_t slot = thisword & 0x1f;
      if( fDebug )
        cout << " " << slot;
      // Should check if this slot has already been SDC_WIRE_CENTER
      auto ithr =
        cfg.thresholds.emplace(slot, std::array<UInt_t, NTHR>{});
      if( ithr.second ) // if insertion successful, then we've added a module
        cfg.nmodules++;
      ip++;
      for( auto& threshold: ithr.first->second )  // assigns NTHR (=16) values
        threshold = evdata->GetRawData(ip++);
      UInt_t lastword = evdata->GetRawData(ip++);
      if( lastword != (0xfadcff00 | slot) ) {
        if( fDebug )
          cout << endl;
        cerr << whereami << ": Unexpected FADC250 threshold block trailer"
             << ", expected " << hex << (0xfadcff00 | slot)
             << ", got " << lastword << dec
             << ". Thresholds for this slot may be incorrect." << endl;
      }
      thisword = evdata->GetRawData(ip);
    }
    if( fDebug )
      cout << endl;
  }

  return ip;
}

UInt_t THcConfigEvtHandler::DecodeCAEN1190Config( THaEvData* evdata, UInt_t ip,
                                                  CrateConfig::CAEN1190_t& cfg )
{
  static const char* here = "THcConfigEvtHandler::DecodeCAEN1190Config";
  const char* whereami = Here(here);
  const UInt_t trailer = 0xdedc119f;

  if (fDebug)
    cout << whereami << ": CAEN1190 TDC information" << endl;
  UInt_t versionword = evdata->GetRawData(ip + 1);
  if( (versionword & 0xffff0000) == 0xabcd0000 ) {
    //UInt_t version = versionword & 0xffff;
    cfg.present = true;
    cfg.resolution = evdata->GetRawData(ip + 2);
    cfg.timewindow_offset = evdata->GetRawData(ip + 3);
    cfg.timewindow_width = evdata->GetRawData(ip + 4);
    UInt_t lastword = evdata->GetRawData(ip + 5);
    if( lastword != trailer ) {
      cerr << whereami << ": Unexpected 1190 configuration block trailer"
           << ", expected " << hex << trailer << ", got " << lastword << dec << endl;
    }
  } else {
    UInt_t thisword = evdata->GetRawData(ip);
    cerr << whereami << ": Unexpected 1190 version word "
         << hex << thisword << dec  << ", expected 0xabcdNNNN" << endl;
  }
  return ip + 6;
}

UInt_t THcConfigEvtHandler::DecodeTIConfig( THaEvData* evdata, UInt_t ip,
                                            CrateConfig::TI_t& cfg )
{
  static const char* here = "THcConfigEvtHandler::DecodeTIConfig";
  const char* whereami = Here(here);

  if (fDebug)
    cout << whereami << ": TI setup data" << endl;

  UInt_t versionword = evdata->GetRawData(ip + 1);
  if( (versionword & 0xffff0000) == 0xabcd0000 ) {
    UInt_t version = versionword & 0xffff;
    ip += 2;
    cfg.present = true;
    cfg.nped = evdata->GetRawData(ip++);
    if( version >= 2 ) {
      cfg.scaler_period = evdata->GetRawData(ip++);
      cfg.sync_count = evdata->GetRawData(ip++);
    } else {
      cfg.scaler_period = 2;
      cfg.sync_count = -1;
    }
    // Prescale factors
    for( UInt_t i = 0; i < NPS; i++ ) {
      auto ps = cfg.prescales[i] = static_cast<Int_t>(evdata->GetRawData(ip++));
      if( evdata->GetDataVersion() > 2 ) {
        // CODA3: Calculate actual factor from "exponent" format
        auto& fact = cfg.ps_factors[i];
        if( ps > 0 && ps <= 16 )
          fact = (1 << (ps - 1)) + 1;
        else if( ps == 0 )
          fact = 1;
        else if( ps == -1 )
          fact = -1;
        else {
          cerr << whereami << ": Invalid CODA3 prescale = " << ps << ". "
               << "Must be between -1 and 16 (inclusive)" << endl;
          fact = -1;
        }
      } else
        cfg.ps_factors = cfg.prescales;
    }
    UInt_t lastword = evdata->GetRawData(ip++);
    if( lastword != 0xd000000f ) {
      cerr << whereami << ": Unexpected last word of TI information block "
           << hex << lastword << dec << endl;
    }
  } else {
    UInt_t thisword = evdata->GetRawData(ip);
    cerr << whereami << ": Unexpected TI info word " << hex << thisword
         << dec << endl << "  Expected 0xabcdNNNN" << endl;
    ip += 11; // This may be wrong. Look for trailer? Give up on entire event?
  }
  return ip;
}

void THcConfigEvtHandler::MakeParms( UInt_t roc ) {
  /**
     Add parameters to gHcParms for this roc
  */
  auto it = fCrateInfoMap.find(roc);
  assert(it != fCrateInfoMap.end());  // else bug in Analyze

  const auto& cinfo = it->second;
  cinfo.MakeParms(*this);
}

//  Pretty print the config data
void THcConfigEvtHandler::PrintConfig()
{
  for( const auto& crate_config : fCrateInfoMap ) {
    cout << "================= Configuration Data ROC " << crate_config.first << "==================" << endl;
    crate_config.second.Print();
  }
}

Bool_t THcConfigEvtHandler::IsPresent( UInt_t crate)
{
  auto it = fCrateInfoMap.find(crate);
  if( it != fCrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    return cinfo.FADC250.present;
  }
  return false;
}
UInt_t THcConfigEvtHandler::GetNSA( UInt_t crate)
{
  auto it = fCrateInfoMap.find(crate);
  if( it != fCrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if( cinfo.FADC250.present )
      return cinfo.FADC250.nsa;
  }
  return -1;
}
UInt_t THcConfigEvtHandler::GetNSB( UInt_t crate)
{
  auto it = fCrateInfoMap.find(crate);
  if( it != fCrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if( cinfo.FADC250.present )
      return cinfo.FADC250.nsb;
  }
  return -1;
}
UInt_t THcConfigEvtHandler::GetNPED( UInt_t crate)
{
  auto it = fCrateInfoMap.find(crate);
  if( it != fCrateInfoMap.end() ) {
    const auto& cinfo = it->second;
    if( cinfo.FADC250.present )
      return cinfo.FADC250.nped;
  }
  return -1;
}
THaAnalysisObject::EStatus THcConfigEvtHandler::Init(const TDatime& date)
{
  if( eventtypes.empty() ) {
    eventtypes.push_back(125);  // what events to look for
  }

  fCrateInfoMap.clear();

  return THaEvtTypeHandler::Init(date);
}

Int_t THcConfigEvtHandler::CrateConfig::MakeParms( THcConfigEvtHandler& h ) const
{
  // Create Hall C parameters referencing these configuration data

  auto& parm_names = h.fParms;
  const auto& modname = h.fName;

  // FADC configuration
  if( FADC250.present ) {
    // Loop over FADC slots
    for( const auto& islot: FADC250.thresholds ) {
      auto slot = islot.first;
      const auto& thresholds = islot.second;

      parm_names.emplace_back(Form("g%s_adc_thresholds_%u_%u[%u]", modname.Data(), roc, slot, NTHR));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "ADC Thresholds", *thresholds.data());

      // FIXME: The following parameters are labeled "per slot", but their
      //  values are identical for all slots.
      parm_names.emplace_back(Form("g%s_adc_mode_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "ADC Mode", FADC250.mode);
      parm_names.emplace_back(Form("g%s_adc_latency_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "Window Latency", FADC250.window_lat);

      parm_names.emplace_back(Form("g%s_adc_width_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "Window Width", FADC250.window_width);

      parm_names.emplace_back(Form("g%s_adc_daclevel_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "DAC Level", FADC250.dac_level);
      parm_names.emplace_back(Form("g%s_adc_nped_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "NPED", FADC250.nped);
      parm_names.emplace_back(Form("g%s_adc_nsa_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "NSA", FADC250.nsa);
      parm_names.emplace_back(Form("g%s_adc_maxped_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "MAXPED", FADC250.maxped);
      parm_names.emplace_back(Form("g%s_adc_np_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "NP", FADC250.np);
      parm_names.emplace_back(Form("g%s_adc_blocklevel_%u_%u", modname.Data(), roc, slot));
      gHcParms->RemoveName(parm_names.back().c_str());
      gHcParms->Define(parm_names.back().c_str(), "Blocklevel", FADC250.blocklevel);
    }
  }

  // CAEN 1190 TDC information
  if( CAEN1190.present ) {
    // FIXME: shouldn't this info be per slot?
    parm_names.emplace_back(Form("g%s_tdc_resolution_%d", modname.Data(), roc));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(), "TDC resolution", CAEN1190.resolution);
    parm_names.emplace_back(Form("g%s_tdc_offset_%d", modname.Data(), roc));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(), "TDC Time Window Offset", CAEN1190.timewindow_offset);
    parm_names.emplace_back(Form("g%s_tdc_width_%d", modname.Data(), roc));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(), "TDC Time Window Width", CAEN1190.timewindow_width);
  }

  // TI Configuration
  // We assume that this information is only provided by the master TI crate.
  // If that is not true we will get "Variable XXX already exists." warnings
  if( TI.present ) {
    parm_names.emplace_back(Form("g%s_ti_nped", modname.Data()));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(),
                     "Number of Pedestal events", TI.nped);
    parm_names.emplace_back(Form("g%s_ti_scaler_period", modname.Data()));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(),
                     "Number of Pedestal events", TI.scaler_period);
    parm_names.emplace_back(Form("g%s_ti_sync_count", modname.Data()));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(),
                     "Number of Pedestal events", TI.sync_count);
    parm_names.emplace_back(Form("g%s_ti_ps[%d]", modname.Data(), NPS));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(), "TI Event Prescale Internal Value", *TI.prescales.data());
    parm_names.emplace_back(Form("g%s_ti_ps_factors[%d]", modname.Data(), NPS));
    gHcParms->RemoveName(parm_names.back().c_str());
    gHcParms->Define(parm_names.back().c_str(), "TI Event Prescale Factor", *TI.ps_factors.data());
  }

  return 0;
}

void THcConfigEvtHandler::CrateConfig::Print() const
{
  // Print configuration info

  if( CAEN1190.present ) {
    cout << "    CAEN 1190 Configuration" << endl;
    cout << "        Resolution: " << CAEN1190.resolution << " ps" << endl;
    cout << "        T Offset:   " << CAEN1190.timewindow_offset << endl;
    cout << "        T Width:    " << CAEN1190.timewindow_width << endl;
  }
  if( FADC250.present ) {
    cout << "    FADC250 Configuration" << endl;
    cout << "       Mode:   " << FADC250.mode << endl;
    cout << "       Blocklevel: " << FADC250.blocklevel << endl;
    cout << "       Latency: " << FADC250.window_lat << "  Width: "<< FADC250.window_width << endl;
    cout << "       DAC Level: " << FADC250.dac_level << "  Threshold: " << FADC250.threshold << endl;
    cout << "       NPED: " << FADC250.nped << "  NSA: " << FADC250.nsa << "  NSB: " << FADC250.nsb << endl;
    cout << "       MAXPED: " << FADC250.maxped << "  NP: " << FADC250.np << "  NSAT: " << FADC250.nsat << endl;

    if( !FADC250.thresholds.empty() ) {
      cout << "       Thresholds";
      for( const auto& thr: FADC250.thresholds ) {
        auto slot = thr.first;
        cout << " " << setw(5) << slot;
      }
      cout << endl;
      for( UInt_t ichan = 0; ichan < NTHR; ichan++ ) {
        cout << "           " << setw(2) << ichan << "    ";
        for( const auto& chan: FADC250.thresholds ) {
          const auto& thresholds = chan.second;
          cout << " " << setw(5) << thresholds[ichan];
        }
        cout << endl;
      }
    }
  }
  if( TI.present ) {
    cout << "    TI Configuration" << endl;
    cout << "        N Pedestals:   " << TI.nped << " events" << endl;
    cout << "        Scaler Period: " << TI.scaler_period << " seconds" << endl;
    cout << "        Sync interval: " << TI.sync_count << " events" << endl;
    cout << "        Prescale exponents: ";
    for( auto ps: TI.prescales )
      cout << " " << ps;
    cout << endl;
    cout << "        Prescale factors:   ";
    for( auto ps: TI.ps_factors )
      cout << " " << ps;
    cout << endl;
  }
}

THcConfigEvtHandler::CrateConfig::FADC250::FADC250()
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

THcConfigEvtHandler::CrateConfig::CAEN1190::CAEN1190()
  : present{false}
  , resolution{0}
  , timewindow_offset{0}
  , timewindow_width{0}
{}

THcConfigEvtHandler::CrateConfig::TI::TI()
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
