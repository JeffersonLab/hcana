/** \class THcTimeSyncEvtHandler
    \ingroup base

  Event handler for to check syncronization of 4ns clock times
  recorded by the TIs and the FADCs.

  To use as a plugin, add to the analysis script:

  THcTimeSyncEvtHandler *timesync = new THcTimeSyncEvtHandler("timesync","ADC/TI Time synchrnoziation"));
  timesync->AddEvtType(1);
  timesync->AddEvtType(2);
  ...
  gHaEvtHandlers->Add(timesync);

  Assumes that all FADCs are in banks with the tag 250 and all
  TIblob banks are tag 4.  And that these banks are contained
  without further structure in ROC banks.  Also asssumes that the
  only banks containing banks are ROC banks.

\author Stephen Wood (saw@jlab.org)
*/

#include "THcTimeSyncEvtHandler.h"
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

THcTimeSyncEvtHandler::THcTimeSyncEvtHandler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description)
{
  ExpectedOffsetMap.clear();
}

THcTimeSyncEvtHandler::~THcTimeSyncEvtHandler()
{
}

//Float_t THcTimeSyncEvtHandler::GetData(const std::string& tag)
//{
//  // A public method which other classes may use
//  if (theDataMap.find(tag) == theDataMap.end())
//    return 0;
// return theDataMap[tag];
//}


Int_t THcTimeSyncEvtHandler::Analyze(THaEvData *evdata)
{

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebug) cout << "------------------\n  Time Syncronization Checker"<<endl;

  Int_t evlen = evdata->GetEvLength();
  UInt_t *rdata = (UInt_t*) evdata->GetRawDataBuffer();

  UInt_t *p = (UInt_t*) rdata;
  UInt_t *plast = p+*p;		// Index to last word in the bank
  Int_t roc = -1;

  CrateTimeMap.clear();

  while(p<plast) {
    Int_t banklen = *p;
    p++;
    if (fDebugFile) {
      *fDebugFile << "Bank: " << hex << *p << dec << " len: " << banklen << endl;
    }
    if((*p & 0xff00) == 0x1000) {	// Bank Containing banks
      if(evlen-*(p-1) > 1) { // Don't use overall event header
        roc = (*p>>16) & 0xf;
	if(fDebug) cout << "ROC: " << roc << " " << evlen << " " << *(p-1) << hex << " " << *p << dec << endl;
        CrateTimeMap[roc] = new RocTimes_t;
      }
      p++;				// Now pointing to a bank in the bank
    } else if (((*p & 0xff00) == 0x100) && (*p != 0xC0000100)) {
      // Bank containing integers.
      // This is either ROC bank containing integers or
      // a bank within a ROC containing data from modules of a single type
      // Look for TI and FADC banks.
      UInt_t tag = (*p>>16) & 0xffff;
      UInt_t *pnext = p+banklen;	// Next bank
      p++;			// First data word
      if(tag==4) { // This is a TI blob banks
        // Actually second word is usually a filler
	// This could be done in a safer way, but it works for now
        Int_t ifill = ((((*p)>>27)&0x1F) == 0x1F) ? 1 : 0;
        if(ifill) {
          p++; banklen--;  // Skip filler word
        }
        if(banklen>=5) {   // Need bank header, at least 2 TI  headers, the  trailer and 2 data words
          UInt_t titime = p[4];
	  if(fDebug) cout << roc << ": TItime " << titime << endl;
          CrateTimeMap[roc]->has_ti_ttime = kTRUE;
          CrateTimeMap[roc]->ti_ttime = titime;
        }
      } else if (tag==250) { // This is an FADC banks
	if(fDebug) cout << roc << ": FADC" << endl;
        // Walk through this bank looking for FADC headers.
        Int_t slot=-1;
        while(p<pnext) {
          UInt_t code = (*p >> 27) & 0x1F;
          switch(code) {
	  case 0x10: // block header
	    slot = (*p >> 22) & 0x1F;
	    p++;
	    break;
	  case 0x13:  // trigger time word
	    {
	      UInt_t fadctime = ((*p)&0xFFFFFF) + (((*(p+1))&0xFF)<<24);
	      if(fDebug) cout << "    " << slot << ": " << fadctime << endl;
        CrateTimeMap[roc]->fadcTimesMap[slot] = fadctime;
	      p += 2;
	      break;
	    }
	  default: // event headers, trailers, fillers, data nwords
	    p++;
	    break;
	  }
	}
      }
      p=pnext;    // Skip to next bank
    } else {
      p = p+*(p-1);
    }
  }

  if(fFirstTime) {
    InitStats();
    fFirstTime = kFALSE;
  }
  AccumulateStats();

  return 1;
}

void THcTimeSyncEvtHandler::InitStats() {
  /** Initialize structure to hold statistics
      Record the offset of each TI and FADC time relative to the
      trigger time of the master TI.
  */
  // Assume the smallest ROC # is the TI master
  if(fMasterRoc < 0) {
    fMasterRoc = CrateTimeMap.begin()->first;
  }
  if(fDebug) cout << "fMasterRoc " << fMasterRoc << endl;
  UInt_t master_ttime = CrateTimeMap[fMasterRoc]->ti_ttime;
  if(fDebug) cout << "master_ttime " << master_ttime << endl;

  CrateStatsMap.clear();

  std::map<Int_t, struct RocTimes *>::iterator it = CrateTimeMap.begin();
  while(it != CrateTimeMap.end()) {
    Int_t roc = it->first;
    struct RocTimes *roctimes = it->second;
    it++;
    CrateStatsMap[roc] = new RocStats_t;
    CrateStatsMap[roc]->ti_ttime_offset = roctimes->ti_ttime - master_ttime;
    if(roctimes->fadcTimesMap.size()>0) {
      if(fDebug) cout << endl << " FADC";
      std::map<Int_t, UInt_t>::iterator itt = roctimes->fadcTimesMap.begin();
      Bool_t use_expected_offset = kFALSE;
      Int_t expected_offset = 0;
      if(ExpectedOffsetMap.find(roc) != ExpectedOffsetMap.end()) {
	expected_offset = ExpectedOffsetMap[roc];
	use_expected_offset = kTRUE;
      }
      while(itt != roctimes->fadcTimesMap.end()) {
        Int_t slot = itt->first;
        UInt_t fadctime = itt->second;
        itt++;
	if(use_expected_offset) {
	  CrateStatsMap[roc]->fadcOffsetMap[slot] = expected_offset;
	} else {
	  CrateStatsMap[roc]->fadcOffsetMap[slot] = fadctime - master_ttime;
	}
        CrateStatsMap[roc]->fadcEarlySlipCountMap[slot] = 0;
        CrateStatsMap[roc]->fadcLateSlipCountMap[slot] = 0;
      }
    }
  }
}

void THcTimeSyncEvtHandler::AccumulateStats() {
  fNEvents++;
  // Get trigger time from master CrateInfo
  UInt_t master_ttime = CrateTimeMap[fMasterRoc]->ti_ttime;
  std::map<Int_t, RocStats_t *>::iterator it = CrateStatsMap.begin();
  while(it != CrateStatsMap.end()) {
    Int_t roc = it->first;
    RocStats* rocstats = it->second;
    it++;
    if(CrateTimeMap[roc]->ti_ttime < master_ttime + rocstats->ti_ttime_offset) {
      rocstats->ti_earlyslipcount++;
    } else if(CrateTimeMap[roc]->ti_ttime > master_ttime + rocstats->ti_ttime_offset) {
      rocstats->ti_lateslipcount++;
    }
    if(rocstats->fadcOffsetMap.size()>0) {
      std::map<Int_t, Int_t>::iterator itt = rocstats->fadcOffsetMap.begin();
      while(itt != rocstats->fadcOffsetMap.end()) {
        Int_t slot = itt->first;
        Int_t fadcoffset = itt->second;
        itt++;
        if(CrateTimeMap[roc]->fadcTimesMap[slot] < master_ttime+fadcoffset) {
          rocstats->fadcEarlySlipCountMap[slot]++;
	} else if(CrateTimeMap[roc]->fadcTimesMap[slot] > master_ttime+fadcoffset) {
	  rocstats->fadcLateSlipCountMap[slot]++;
        }
      }
    }
  }
}
void THcTimeSyncEvtHandler::PrintStats()
/* Print time syncrhonization statistics.
   Need to add an optional filename argument
*/
{
  cout << "-------------------------------------------------------------------" << endl;
  cout << "------ TI and FADC250 trigger time synchronization statitics ------" << endl;
  cout << "-------------------------------------------------------------------" << endl;
  cout << "      " << fNEvents << " events analyzed" << endl;
  // Dump the map to see if we did it right.
  std::map<Int_t, RocStats_t *>::iterator it = CrateStatsMap.begin();
  while(it != CrateStatsMap.end()) {
    Int_t roc = it->first;
    struct RocStats *rocstats = it->second;
    it++;
    cout << "ROC " << roc << "  TI Offset " << rocstats->ti_ttime_offset << "  Slips " << rocstats->ti_earlyslipcount << "   " << rocstats->ti_lateslipcount << endl;
    std::map<Int_t, Int_t>::iterator itt = rocstats->fadcEarlySlipCountMap.begin();
    while(itt != rocstats->fadcEarlySlipCountMap.end()) {
      Int_t slot = itt->first;
      Int_t earlyslips = itt->second;
      Int_t lateslips = rocstats->fadcLateSlipCountMap[slot];
      itt++;
      if(earlyslips+lateslips > 0) { // Only print slots with slippage
	cout << "    " << slot << " " << rocstats->fadcOffsetMap[slot] << "    " << earlyslips << "    " << lateslips << endl;
      }
    }
  }
  cout << "-------------------------------------------------------------------" << endl;
}
void THcTimeSyncEvtHandler::SetExpectedOffset(Int_t roc, Int_t offset) {
  ExpectedOffsetMap.clear();
  AddExpectedOffset(roc, offset);
}
void THcTimeSyncEvtHandler::AddExpectedOffset(Int_t roc, Int_t offset) {
  ExpectedOffsetMap[roc] = offset;
}
		      
THaAnalysisObject::EStatus THcTimeSyncEvtHandler::Init(const TDatime& date)
{

  cout << "Howdy !  We are initializing THcTimeSyncEvtHandler !!   name =   "<<fName<<endl;

  if(eventtypes.size()==0) {
    eventtypes.push_back(125);  // what events to look for
  }

  fFirstTime = kTRUE;
  fMasterRoc = -1;
  fNEvents = 0;
  CrateTimeMap.clear();
  //CrateStatsMap.clear();

  fStatus = kOK;
  return kOK;
}

ClassImp(THcTimeSyncEvtHandler)
