/** \class THcConfigEvtHandler
    \ingroup base

  Event handler for Hall C prestart config event.  Type 125

  Example of an Event Type Handler for event type 125,
  the hall C prestart event.

  It was tested on some hms data.  However, note that I don't know what
  these data mean yet and I presume the data structure is under development;
  someone will need to modify this class (and the comments).

  To use as a plugin with your own modifications, you can do this in
  your analysis script

  gHaEvtHandlers->Add (new THcConfigEvtHandler("hallcpre","for evtype 125"));

  Global variables are defined in Init.  You can see them in Podd, as
    analyzer [2] gHaVars->Print()

     OBJ: THaVar	HCvar1	Hall C event type 125 variable 1
     OBJ: THaVar	HCvar2	Hall C event type 125 variable 2
     OBJ: THaVar	HCvar3	Hall C event type 125 variable 3
     OBJ: THaVar	HCvar4	Hall C event type 125 variable 4

  The data can be added to the ROOT Tree "T" using the output
  definition file.  Then you can see them, for example as follows

     T->Scan("HCvar1:HCvar2:HCvar3:HCvar4")

\author Robert Michaels (rom@jlab.org), updated by Stephen Wood (saw@jlab.org)
*/

#include "THcConfigEvtHandler.h"
#include "THaEvData.h"
#include "THaVarList.h"
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
  CrateInfoMap.insert(std::make_pair(roc, cinfo));
  ip++;
  // Three possible blocks of config data
  // 0xdafadc01 - FADC information for the crate
  // 0xdafadcff - Set of threshold by slot/channel
  // 0xdedc1190 - 1190 TDC information for the crate
  thisword = evdata->GetRawData(ip);
  while(ip<evlen) {
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
      cinfo->FADC250.daq_level = evdata->GetRawData(ip+2);
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
      thisword = evdata->GetRawData(ip);
    } else if (thisword == 0xdedc1190) { // CAEN 1190 information
      cout << "TDC information" << endl;
      cinfo->CAEN1190.present = 1;
      cinfo->CAEN1190.resolution = evdata->GetRawData(ip+2);
      cinfo->CAEN1190.timewindow_offset = evdata->GetRawData(ip+3);
      cinfo->CAEN1190.timewindow_width = evdata->GetRawData(ip+4);
      ip += 6;
      thisword = evdata->GetRawData(ip);
    } else {
      cout << "Expected header missing" << endl;
      cout << ip << " " << hex << thisword << dec << endl;
      ip = evlen;
    }
  }

  return 1;
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
    } else if (cinfo->FADC250.present) {
      cout << "    FADC250 Configuration" << endl;
      cout << "       Mode:   " << cinfo->FADC250.mode << endl;
      cout << "       Latency: " << cinfo->FADC250.window_lat << "  Width: "<< cinfo->FADC250.window_width << endl;
      cout << "       DAQ Level: " << cinfo->FADC250.daq_level << "  Threshold: " << cinfo->FADC250.threshold << endl;
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

// dvars is a member of this class.
// The index of the dvars array track the array of global variables created.
// This is just a fake example with 4 variables.
// Please change these comments when you modify this class.

  NVars = 4;
  dvars = new Double_t[NVars];
  memset(dvars, 0, NVars*sizeof(Double_t));
  if (gHaVars) {
      cout << "EvtHandler:: Have gHaVars.  Good thing. "<<gHaVars<<endl;
  } else {
      cout << "EvtHandler:: No gHaVars ?!  Well, that is a problem !!"<<endl;
      return kInitError;
  }
  const Int_t* count = 0;
  char cname[80];
  char cdescription[80];
  for (UInt_t i = 0; i < NVars; i++) {
    sprintf(cname,"HCvar%d",i+1);
    sprintf(cdescription,"Hall C event type 125 variable %d",i+1);
    gHaVars->DefineByType(cname, cdescription, &dvars[i], kDouble, count);
  }


  fStatus = kOK;
  return kOK;
}

ClassImp(THcConfigEvtHandler)
