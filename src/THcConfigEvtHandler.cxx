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
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;

THcConfigEvtHandler::THcConfigEvtHandler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description)
{
}

THcConfigEvtHandler::~THcConfigEvtHandler()
{
  // TODO: remove the parameters we've added to gHcParms

  DeleteCrateInfoMap();
}

//Float_t THcConfigEvtHandler::GetData(const std::string& tag)
//{
//  // A public method which other classes may use
//  if (theDataMap.find(tag) == theDataMap.end())
//    return 0;
// return theDataMap[tag];
//}

void THcConfigEvtHandler::DeleteCrateInfoMap()
{
  // Clear the CrateInfoMap, deallocating its elements' dynamic memory

  //TODO: don't manage memory manually, contain the object, use STL vectors
  typedef std::map<Int_t, CrateInfo_t *> cmap_t;
  typedef std::map<Int_t, Int_t *> imap_t;
  for( cmap_t::iterator it = CrateInfoMap.begin();
       it != CrateInfoMap.end(); ++it ) {
    CrateInfo_t* cinfo = it->second;
    if( cinfo ) {
      for( imap_t::iterator jt = cinfo->FADC250.thresholds.begin();
	   jt != cinfo->FADC250.thresholds.end(); ++jt ) {
	delete [] jt->second;
      }
      delete cinfo;
    }
  }
  CrateInfoMap.clear();
}

Int_t THcConfigEvtHandler::Analyze(THaEvData *evdata)
{

  Bool_t ldebug = true;  // FIXME: use fDebug

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (ldebug) cout << "------------------\n  Event type 125"<<endl;

  Int_t evlen = evdata->GetEvLength();
  Int_t ip = 0;
  ip++;
  UInt_t thisword = evdata->GetRawData(ip);
  Int_t roc = thisword & 0xff;
  cout << "THcConfigEvtHandler: " << roc << endl;
  // Should check if this roc has already been seen
  CrateInfo_t *cinfo = new CrateInfo_t;
  cinfo->FADC250.nmodules=0;
  cinfo->FADC250.present=0;
  cinfo->CAEN1190.present=0;
  cinfo->TI.present=0;  
  // FIXME: check if entry for this roc already present in the map
  CrateInfoMap.insert(std::make_pair(roc, cinfo));
  ip++;
  // Three possible blocks of config data
  // 0xdafadc01 - FADC information for the crate
  // 0xdafadcff - Set of threshold by slot/channel
  // 0xdedc1190 - 1190 TDC information for the crate
  while(ip<evlen) {
    thisword = evdata->GetRawData(ip);
    if (thisword == 0xdafadcff) {
      ip++;
      thisword = evdata->GetRawData(ip);
      cout << "ADC thresholds for slots ";
      while((thisword & 0xfffff000)==0xfadcf000) {
        Int_t slot = thisword&0x1f;
        // Should check if this slot has already been SDC_WIRE_CENTER
        cinfo->FADC250.nmodules++;
        cout << " " << slot;
        Int_t *thresholds = new Int_t [16];
        cinfo->FADC250.thresholds.insert(std::make_pair(slot, thresholds));
        for(Int_t i=0;i<16;i++) {
          thresholds[i] = evdata->GetRawData(ip+1+i);
        }
        ip +=18;
        if(ip>=evlen) {
          if(ip>evlen) {
            cout << endl << "Info event truncated" << endl;
          }
          break;
        }
        thisword = evdata->GetRawData(ip);
      }
      cout << endl;
    } else if((thisword&0xffffff00) == 0xdafadc00) { // FADC250 information
      cout << "ADC information: Block level " << (thisword&0xff) << endl;
      cinfo->FADC250.present = 1;
      cinfo->FADC250.blocklevel = thisword&0xff;
      cinfo->FADC250.dac_level = evdata->GetRawData(ip+2);
      cinfo->FADC250.threshold = evdata->GetRawData(ip+3);
      cinfo->FADC250.mode = evdata->GetRawData(ip+4);
      cinfo->FADC250.window_lat = evdata->GetRawData(ip+5);
      cinfo->FADC250.window_width = evdata->GetRawData(ip+6);
      cinfo->FADC250.nsb = evdata->GetRawData(ip+7);
      cinfo->FADC250.nsa = evdata->GetRawData(ip+8);
      cinfo->FADC250.np = evdata->GetRawData(ip+9);
      cinfo->FADC250.nped = evdata->GetRawData(ip+10);
      cinfo->FADC250.maxped = evdata->GetRawData(ip+11);
      cinfo->FADC250.nsat = evdata->GetRawData(ip+12);
      ip += 13;
    } else if (thisword == 0xdedc1190) { // CAEN 1190 information
      cout << "TDC information" << endl;
      cinfo->CAEN1190.present = 1;
      cinfo->CAEN1190.resolution = evdata->GetRawData(ip+2);
      cinfo->CAEN1190.timewindow_offset = evdata->GetRawData(ip+3);
      cinfo->CAEN1190.timewindow_width = evdata->GetRawData(ip+4);
      ip += 6;
    } else if (thisword == 0xd0000000) { // TI setup data
      cinfo->TI.present = 1;
      ip += 1;
      UInt_t versionword = evdata->GetRawData(ip++);
      if((versionword & 0xffff0000) != 0xabcd0000) {
	cout << "Unexpected TI info word " << hex << thisword << dec << endl;
	cout << "  Expected 0xabcdNNNN" << endl;
	ip += 1;
      } else {
	Int_t version = versionword & 0xffff;
	cinfo->TI.num_prescales = 6;
	cinfo->TI.nped = evdata->GetRawData(ip++);
	if(version >= 2) {
	  cinfo->TI.scaler_period = evdata->GetRawData(ip++);
	  cinfo->TI.sync_count = evdata->GetRawData(ip++);
	} else {
	  cinfo->TI.scaler_period = 2;
	  cinfo->TI.sync_count = -1;
	}
	for(Int_t i = 0; i<cinfo->TI.num_prescales; i++) {
	  cinfo->TI.prescales[i] = evdata->GetRawData(ip++);
	}
	UInt_t lastword = evdata->GetRawData(ip++);
	if(lastword != 0xd000000f) {
	  cout << "Unexpected last word of TI information block "
	       << hex << lastword << dec << endl;
	}
      }
    } else {
      cout << "Expected header missing" << endl;
      cout << ip << " " << hex << thisword << dec << endl;
      ip = evlen;
    }
  }

  cout << "Making Parms for ROC " << roc << "  Event type " << evdata->GetEvType() << endl;
  MakeParms(roc);

  return 1;
}

void THcConfigEvtHandler::MakeParms(Int_t roc)
{
  /**
     Add parameters to gHcParms for this roc
  */
  std::map<Int_t, CrateInfo_t *>::iterator it = CrateInfoMap.begin();
  while(it != CrateInfoMap.end()) {
    Int_t thisroc = it->first;
    if(thisroc != roc) {
      it++;
      continue;
    }
    CrateInfo_t *cinfo = it->second;

    // CAEN 1190 TDC information
    if (cinfo->CAEN1190.present) {
      Int_t resolution = cinfo->CAEN1190.resolution;
      gHcParms->Define(Form("g%s_tdc_resolution_%d",fName.Data(),roc),"TDC resolution",resolution);
      Int_t offset = cinfo->CAEN1190.timewindow_offset;
      gHcParms->Define(Form("g%s_tdc_offset_%d",fName.Data(),roc),"TDC Time Window Offset",offset);
      Int_t width = cinfo->CAEN1190.timewindow_width;
      gHcParms->Define(Form("g%s_tdc_width_%d",fName.Data(),roc),"TDC Time Window Width",width);
    }
    // FADC Thresholds
    if (cinfo->FADC250.present) {
      // Loop over FADC slots
      std::map<Int_t, Int_t *>::iterator itt = cinfo->FADC250.thresholds.begin();
      while(itt != cinfo->FADC250.thresholds.end()) {
        Int_t slot = itt->first;

	// this memory would leak
//	Int_t *thresholds = new Int_t[16];
//	memcpy(thresholds,itt->second,16*sizeof(Int_t));
//	gHcParms->Define(Form("g%s_adc_thresholds_%d_%d[16]",fName.Data(),roc,slot),"ADC Thresholds",*thresholds);
	// Define the variable directly on the CrateInfo_t that resides in CrateInfoMap where it will stay
	// until the end of the life of this object
	gHcParms->Define(Form("g%s_adc_thresholds_%d_%d[16]",fName.Data(),roc,slot),"ADC Thresholds",*itt->second);

	Int_t mode = cinfo->FADC250.mode;
	gHcParms->Define(Form("g%s_adc_mode_%d_%d",fName.Data(),roc,slot),"ADC Mode",mode);
	Int_t latency = cinfo->FADC250.window_lat;
	gHcParms->Define(Form("g%s_adc_latency_%d_%d",fName.Data(),roc,slot),"Window Latency",latency);

	Int_t width = cinfo->FADC250.window_width;
	gHcParms->Define(Form("g%s_adc_width_%d_%d",fName.Data(),roc,slot),"Window Width",width);

	Int_t daclevel = cinfo->FADC250.dac_level;
	gHcParms->Define(Form("g%s_adc_daclevely_%d_%d",fName.Data(),roc,slot),"DAC Level",daclevel);
	Int_t nped = cinfo->FADC250.nped;
	gHcParms->Define(Form("g%s_adc_nped_%d_%d",fName.Data(),roc,slot),"NPED",nped);
	Int_t nsa = cinfo->FADC250.nsa;
	gHcParms->Define(Form("g%s_adc_nsa_%d_%d",fName.Data(),roc,slot),"NSA",nsa);
	Int_t maxped = cinfo->FADC250.maxped;
	gHcParms->Define(Form("g%s_adc_maxped_%d_%d",fName.Data(),roc,slot),"MAXPED",maxped);
	Int_t np = cinfo->FADC250.np;
	gHcParms->Define(Form("g%s_adc_np_%d_%d",fName.Data(),roc,slot),"NP",np);

        itt++;
      }
      // TI Configuration
      // We assume that this information is only provided by the master TI crate.
      // If that is not true we will get "Variable XXX already exists." warnings
      if(cinfo->TI.present) {
	Int_t nped = cinfo->TI.nped;
	gHcParms->Define(Form("g%s_ti_nped",fName.Data()),"Number of Pedestal events",nped);
	Int_t scaler_period = cinfo->TI.scaler_period;
	gHcParms->Define(Form("g%s_ti_scaler_period",fName.Data()),"Number of Pedestal events",scaler_period);
	Int_t sync_count = cinfo->TI.sync_count;
	gHcParms->Define(Form("g%s_ti_sync_count",fName.Data()),"Number of Pedestal events",sync_count);

	Int_t *ps_exps = new Int_t[cinfo->TI.num_prescales];
	Int_t *ps_factors = new Int_t[cinfo->TI.num_prescales];
	for(Int_t i=0;i<cinfo->TI.num_prescales;i++) {
	  ps_exps[i] = cinfo->TI.prescales[i];
	  if(ps_exps[i] > 0) {
	    ps_factors[i] = (1<<(ps_exps[i]-1)) + 1;
	  } else if (ps_exps[i] == 0) {
	    ps_factors[i] = 1;
	  } else {
	    ps_factors[i] = -1;
	  }
	}
	gHcParms->Define(Form("g%s_ti_ps[%d]",fName.Data(),cinfo->TI.num_prescales),"TI Event Prescale Internal Value",*ps_exps);
	gHcParms->Define(Form("g%s_ti_ps_factors[%d]",fName.Data(),cinfo->TI.num_prescales),"TI Event Prescale Factor",*ps_factors);
      }
    }
    it++;
  }
}    
    
void THcConfigEvtHandler::PrintConfig()
{
/**
  Stub of method to pretty print the config data
*/
  std::map<Int_t, CrateInfo_t *>::iterator it = CrateInfoMap.begin();
  while(it != CrateInfoMap.end()) {
    Int_t roc = it->first;
    cout << "================= Configuration Data ROC " << roc << "==================" << endl;
    CrateInfo_t *cinfo = it->second;
    if(cinfo->CAEN1190.present) {
      cout << "    CAEN 1190 Configuration" << endl;
      cout << "        Resolution: " << cinfo->CAEN1190.resolution << " ps" << endl;
      cout << "        T Offset:   " << cinfo->CAEN1190.timewindow_offset << endl;
      cout << "        T Width:    " << cinfo->CAEN1190.timewindow_width << endl;
    }
    if (cinfo->FADC250.present) {
      cout << "    FADC250 Configuration" << endl;
      cout << "       Mode:   " << cinfo->FADC250.mode << endl;
      cout << "       Latency: " << cinfo->FADC250.window_lat << "  Width: "<< cinfo->FADC250.window_width << endl;
      cout << "       DAC Level: " << cinfo->FADC250.dac_level << "  Threshold: " << cinfo->FADC250.threshold << endl;
      cout << "       NPED: " << cinfo->FADC250.nped << "  NSA: " << cinfo->FADC250.nsa << "  NSB: " << cinfo->FADC250.nsb << endl;
      cout << "       MAXPED: " << cinfo->FADC250.maxped << "  NP: " << cinfo->FADC250.np << "  NSAT: " << cinfo->FADC250.nsat << endl;

      // Loop over FADC slots
      cout << "       Thresholds";
      std::map<Int_t, Int_t *>::iterator itt = cinfo->FADC250.thresholds.begin();
      while(itt != cinfo->FADC250.thresholds.end()) {
        Int_t slot = itt->first;
        cout << " " << setw(5) << slot;
        itt++;
      }
      cout << endl;
      for(Int_t ichan=0;ichan<16;ichan++) {
        cout << "           " << setw(2) << ichan << "    ";
        std::map<Int_t, Int_t *>::iterator itt = cinfo->FADC250.thresholds.begin();
        while(itt != cinfo->FADC250.thresholds.end()) {
          Int_t *thresholds = itt->second;
          cout << " " << setw(5) << thresholds[ichan];
          itt++;
        }
        cout << endl;
      }
    }
    if(cinfo->TI.present) {
      cout << "    TI Configuration" << endl;
      cout << "        N Pedestals:   " << cinfo->TI.nped << " events" << endl;
      cout << "        Scaler Period: " << cinfo->TI.scaler_period << " seconds" << endl;
      cout << "        Sync interval: " << cinfo->TI.sync_count << " events" << endl;
      cout << "        Prescales:    ";
      for(Int_t i=0;i<cinfo->TI.num_prescales;i++) {
	cout << " " << cinfo->TI.prescales[i];
      }
      cout << endl;
    }
    it++;
  }
}

Int_t THcConfigEvtHandler::IsPresent(Int_t crate) {
  if(CrateInfoMap.find(crate)!=CrateInfoMap.end()) {
    CrateInfo_t *cinfo = CrateInfoMap[crate];
    return cinfo->FADC250.present;
  }
  return(0);
}
Int_t THcConfigEvtHandler::GetNSA(Int_t crate) {
  if(CrateInfoMap.find(crate)!=CrateInfoMap.end()) {
    CrateInfo_t *cinfo = CrateInfoMap[crate];
    if(cinfo->FADC250.present > 0) return(cinfo->FADC250.nsa);
  }
  return(-1);
}
Int_t THcConfigEvtHandler::GetNSB(Int_t crate) {
  if(CrateInfoMap.find(crate)!=CrateInfoMap.end()) {
    CrateInfo_t *cinfo = CrateInfoMap[crate];
    if(cinfo->FADC250.present > 0) return(cinfo->FADC250.nsb);
  }
  return(-1);
}
Int_t THcConfigEvtHandler::GetNPED(Int_t crate) {
  if(CrateInfoMap.find(crate)!=CrateInfoMap.end()) {
    CrateInfo_t *cinfo = CrateInfoMap[crate];
    if(cinfo->FADC250.present > 0) return(cinfo->FADC250.nped);
  }
  return(-1);
}
void THcConfigEvtHandler::AddEventType(Int_t evtype)
{
  eventtypes.push_back(evtype);
}

THaAnalysisObject::EStatus THcConfigEvtHandler::Init(const TDatime& date)
{

  cout << "Howdy !  We are initializing THcConfigEvtHandler !!   name =   "<<fName<<endl;

  if(eventtypes.size()==0) {
    eventtypes.push_back(125);  // what events to look for
  }

  DeleteCrateInfoMap();

  fStatus = kOK;
  return kOK;
}

ClassImp(THcConfigEvtHandler)
