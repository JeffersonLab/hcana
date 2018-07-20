/** \class THcTimeSyncEvtHandler
    \ingroup Base

    \brief Event handler to check TI and FADC synchronization.

    Check syncronization of 4ns clock times recorded by the TIs and the FADCs.
    Also can be used to detect CAEN1190 TDC syncronization problems and
    write out a new coda file that corrects the synchronization.  (These
    TDC synchronization problems were occuring in the spring of 2018.)

    This event handler is typically not used in normal anlysis scripts.  It is
    used with custom to check suspect runs and write synchronization corrected
    data files.

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
/* This version will deal with bad run coin_all_03302.dat.

That has an event with most of a noisy set of 1190 data in ROC 3.
The next event has the remainder of the noisy data.  It's good 1190 data
is in the next event.
So the 1190 data for the first event with noisy data should be discarded.
(Empty bank).  The next event should be cached, and then written out when
the next event comes.
*/

#include "THcTimeSyncEvtHandler.h"
#include "THaEvData.h"
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THaCodaFile.h"
#include "THaRunBase.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
//#include "evio.h"

using namespace std;
using namespace Decoder;

THcTimeSyncEvtHandler::THcTimeSyncEvtHandler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description)
{
  ExpectedOffsetMap.clear();
  fBadROC = -1;
  fResync = kTRUE;
  fBadSyncSizeTrigger = 450;
  fCodaOut = 0;
  fLastEventWasSync = kFALSE;
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

  //  cout << evdata->GetEvType() << " " << evdata->GetEvLength() << endl;

  // If filtering data file, pass through all events we are not dealing with
  if ( !IsMyEvent(evdata->GetEvType()) ) {
    if(fCodaOut) {
      fCodaOut->codaWrite(evdata->GetRawDataBuffer());
    }
    return -1;
  }


  if (fDebug) cout << "------------------\n  Time Syncronization Checker"<<endl;

  Int_t evlen = evdata->GetEvLength();

  UInt_t *rdata = (UInt_t*) evdata->GetRawDataBuffer();

  UInt_t *pslippedbank=0;

  UInt_t *p = (UInt_t*) rdata;
  UInt_t *plast = p+*p;		// Index to last word in the bank
  Int_t roc = -1;
  Bool_t issyncevent=kFALSE;

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
      UInt_t num = *p & 0xff;
      UInt_t *pnext = p+banklen;	// Next bank
      p++;			// First data word
      if(tag==4) { // This is a TI blob banks
        // Actually second word is usually a filler
	// This could be done in a safer way, but it works for now
        Int_t ifill = ((((*p)>>27)&0x1F) == 0x1F) ? 1 : 0;
        if(ifill) {
          p++; banklen--;  // Skip filler word
        }
	CrateTimeMap[roc]->ti_evcount = p[3];
        if(banklen>=5) {   // Need bank header, at least 2 TI  headers, the  trailer and 2 data words
          UInt_t titime = p[4];
	  if(fDebug) cout << roc << ": TItime " << titime << endl;
          CrateTimeMap[roc]->has_ti_ttime = kTRUE;
          CrateTimeMap[roc]->ti_ttime = titime;
        }
      } else if (tag==3801) {
	if(fResync && num==1) {
	  issyncevent = kTRUE;
	}
      } else if (tag==250) { // This is an FADC bank
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
      } else if (tag==1190) {	// Bank with CAEN 1190 TDCs
	if(roc==fBadROC) {		// Point to slipped bank
	  if(fSlippage) {
	    pslippedbank = p-2;
	    //	    cout << banklen << " " << pslippedbank[0] << endl;
	    if(AllTdcsPresent(pslippedbank) && (banklen > fBadSyncSizeTrigger)) {
	      cout << "Slippage detected at event " << evdata->GetEvNum() << " with size " << banklen << " but not corrected" << endl;
	    }
	  } else {
	    if(AllTdcsPresent(p-2) && (banklen > fBadSyncSizeTrigger)) {
	      cout << "Slippage enabled at event " << evdata->GetEvNum() << " with size " << banklen << endl;
	      fSlippage = 1;
	    }
	  }
	}
	if(fDebug) cout << roc << ": 1190" << endl;
	// Walk through this bank looking for TDC headers
	while(p<pnext) {
	  if((*p & 0xf8000000) == 0x40000000) {
	    Int_t slot= *p & 0x1f;
	    Int_t evcount = (*p >> 5) & 0x3fffff;
	    CrateTimeMap[roc]->ftdcEvCountMap[slot] = evcount;
	  }
	  p++;
	}
      }
      p=pnext;    // Skip to next bank
    } else {
      p = p+*(p-1);
    }
  }

  if(fFirstTime) {
    InitStats();
    fLastEvent[0] = 0;
    fFirstTime = kFALSE;
    fDumpNew=2;
  }
  if(issyncevent) cout << "SYNC event" << endl;
  AccumulateStats(fLastEventWasSync);
  fLastEventWasSync = issyncevent;

  if(!fCodaOut) return(1);
  
  if(fSlippage > 0) {
    if(fLastEvent[0]>0) { // Now slipping.  Output corrected event
      fWriteDelayed=kTRUE;
      //      cout << "Will write corrected event" << endl;
    } else {
      cout << "Skipping event " << evdata->GetEvNum() << endl;
    }
  } else {			// Not slipping yet, just copy event
    if(fCodaOut) {
      fCodaOut->codaWrite(evdata->GetRawDataBuffer());
    }
  }    

  if(fWriteDelayed) {

    // Dump event before fixing
    //    if(fDumpNew > 0) {
    //      cout << "Unmodified event" << endl;
    //      for(UInt_t i=0;i<=fLastEvent[0];i++) {
    //	if(i%5 == 0) cout<<endl<<dec<<i<<": ";
    //	cout << hex << setw(10) << fLastEvent[i];
    //      }
    //    }
    //    cout << dec << endl;

    // Use fLastEvent with current pslipped bank to write out fixed event
    // Find the 1190 bank.  Shift everything beyond that bank forward or
    // or back to make room for the 1190 bank from the current event.
    // Copy the current 1190 bank into fLastEvent.  Write the event.
    p = fLastEvent;
    plast = p+*p;
    roc = -1;
    UInt_t *poverwrite=0;
    Int_t replacementlen;
    Int_t banklen;
    UInt_t *rocbanklenp=0;
    UInt_t *roc3banklenp=0;

    //    cout << "Old Size: " << fLastEvent[0] << " " << pslippedbank[0] << " ";
    while(p<plast) {
      banklen = *p;
      p++;
      if((*p & 0xff00) == 0x1000) {	// Bank Containing banks
	if((fLastEvent[0]+1)-*(p-1) > 1) { // Don't use overall event header
	  roc = (*p>>16) & 0xf;
	  rocbanklenp = p-1;	// Pointer to rocbank length
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
	if(tag==1190 && roc==fBadROC) { // The TDC bank we want to replace
	  replacementlen=pslippedbank[0];
	  roc3banklenp=rocbanklenp; // Save pointer to ROC bank header
	  poverwrite=p-2;	// Where to write the slipped bank
	  break;
	}
	p=pnext;
      } else {
	p=p+*(p-1);
      }
    }
    Int_t icopied=0;
    if(poverwrite) {
      //      cout << banklen << endl;
      if(replacementlen < banklen) { // Shift data after bank down
	p = poverwrite+banklen+1;    // Data to be shifted
	//	cout << "Shifting " << (p-fLastEvent) << " to " << (p+replacementlen-banklen-fLastEvent) << endl;
	while(p<=plast) {
	  //	  cout << (p+replacementlen-banklen-fLastEvent) << ": " << hex << *p << dec << endl;
	  *(p + replacementlen - banklen) = *p;
	  p++;
	  icopied++;
	}
      } else if (replacementlen > banklen) { // Shift data up
	p = plast;
	//	UInt_t *pfirst = poverwrite + replacementlen + 1;
	UInt_t *pfirst = poverwrite + banklen + 1;
	//	cout << "Shifting " << (plast-pfirst+1) << " words " << (pfirst-fLastEvent) << " to " << (pfirst+replacementlen-banklen-fLastEvent) << endl;
	while(p>=pfirst) {
	  //	  cout << (p+replacementlen-banklen-fLastEvent) << ": " << hex << *p << dec << endl;
	  *(p+replacementlen-banklen) = *p;
	  p--;
	  icopied++;
	}
      }
      fLastEvent[0] += replacementlen-banklen; // Correct overall event length
      *roc3banklenp += replacementlen-banklen; // Also ROC3 bank length
      //      cout << "Copying " << replacementlen+1 << " words of new bank to " << poverwrite-fLastEvent << endl;
      for(Int_t i=0;i<=replacementlen;i++) { // Copy slipped bank into last event
	poverwrite[i] = pslippedbank[i];
      }
    }
    //    cout << "New Size: " << fLastEvent[0] << "   copied " << icopied << endl;
    if(fCodaOut) {
      //      if(fDumpNew > 0) {
      //	for(UInt_t i=0;i<=fLastEvent[0];i++) {
      //	  if(i%5 == 0) cout<<endl<<dec<<i<<": ";
      //	  cout << hex << setw(10) << fLastEvent[i];
      //	}
      //	fDumpNew--;
      //      }
      //      cout << dec << endl;
      fCodaOut->codaWrite(fLastEvent);
      if(issyncevent) {		// If this was a sync event, write it out and stop rewriting
	cout << "Run back in sync at event " << evdata->GetEvNum() << endl;
	fCodaOut->codaWrite(evdata->GetRawDataBuffer());
	fSlippage = 0;
	fLastEvent[0] = 0;
	fWriteDelayed = kFALSE;
      }
    }
  }
  if(fSlippage>0) {		// Just handle slippage of 1 now
    for(UInt_t i=0;i<=rdata[0];i++) {
      fLastEvent[i]=rdata[i];
    }
    //    cout << "Cached event " << evdata->GetEvNum() << " " << fLastEvent[0] << endl;
  }

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
    if(roctimes->ftdcEvCountMap.size()>0) {
      if(fDebug) cout << endl << " 1190";
      std::map<Int_t, UInt_t>::iterator itt = roctimes->ftdcEvCountMap.begin();
      while(itt != roctimes->ftdcEvCountMap.end()) {
        Int_t slot = itt->first;
	itt++;
	CrateStatsMap[roc]->ftdcEvCountWrongMap[slot] = 0;
      }
    }
  }
}

void THcTimeSyncEvtHandler::AccumulateStats(Bool_t sync) {
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
    if(rocstats->ftdcEvCountWrongMap.size()>0) {
      std::map<Int_t, Int_t>::iterator itt = rocstats->ftdcEvCountWrongMap.begin();
      while(itt != rocstats->ftdcEvCountWrongMap.end()) {
	Int_t slot = itt->first;
	itt++;
	if(rocstats->ftdcEvCountOffsetMap.find(slot)==rocstats->ftdcEvCountOffsetMap.end()) {
	  rocstats->ftdcEvCountOffsetMap[slot] = 1;
	}
	Int_t cdiff = (CrateTimeMap[roc]->ti_evcount & 0x3fffff) -
	  ((CrateTimeMap[roc]->ftdcEvCountMap[slot]+rocstats->ftdcEvCountOffsetMap[slot])&0x3fffff);
	if(sync) { // Need to do this check on the event after the sync event too
	  if(cdiff>2) {
	    cout << "ROC/Slot " << roc << "/" << slot << " count diff correction " << cdiff << endl;
	    rocstats->ftdcEvCountOffsetMap[slot] += cdiff;
	    cdiff = 0;
	  }
	}
	if(cdiff != 0) {
	  rocstats->ftdcEvCountWrongMap[slot]++;
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
    std::map<Int_t, Int_t>::iterator itw = rocstats->ftdcEvCountWrongMap.begin();
    while(itw != rocstats->ftdcEvCountWrongMap.end()) {
      Int_t slot = itw->first;
      Int_t wrongcount = itw->second;
      itw++;
      if(wrongcount > 0) {
	cout << "    " << slot << " " << wrongcount << endl;
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
    eventtypes.push_back(1);  // If no event types specified, 
    eventtypes.push_back(2);  // check timestamp synchronization on these event types
    eventtypes.push_back(3);
    eventtypes.push_back(4);
    eventtypes.push_back(5);
    eventtypes.push_back(6);
    eventtypes.push_back(7);
  }

  fFirstTime = kTRUE;
  fFirstTdcCheck = kTRUE;
  fMasterRoc = -1;
  fNEvents = 0;
  CrateTimeMap.clear();
  fSlippage=0;
  fWriteDelayed=kFALSE;
  fLastEvent[0]=0;
  //CrateStatsMap.clear();
  if(fCodaOut && fBadROC < 0) {
    Warning(Here("Init"), "Sync filtering requested, but bad ROC not specified");
  }

  fStatus = kOK;
  return kOK;
}

Int_t THcTimeSyncEvtHandler::End( THaRunBase* r) {
  if(!fCodaOut) {
    return 0;
  }
  return fCodaOut->codaClose();
}

Int_t THcTimeSyncEvtHandler::SetRewriteFile(const char *filename) {
  if(filename==0 || strlen(filename)==0) {
    fCodaOut = 0;
    cout << "THcTimeSyncEvtHandler sync filtering disabled" << endl;
  } else {
    TString ts=filename;
    fCodaOut = new THaCodaFile;
    if (!fCodaOut) {
      Error(Here("SetRewriteFile"),"Cannot create CODA output file object. "
	    "Something is very wrong." );
      return -2;
    }
    if ( fCodaOut->codaOpen(ts, "w", 1) ) {
      Error(Here("SetRewriteFile"),"Cannot open CODA file %s for writing.",ts.Data());
      return -3;
    }
    cout << "THcTimeSYncEvtHandler writing sync filtered coda file to " << ts <<endl;
  }
  return(0);
}
Int_t THcTimeSyncEvtHandler::AllTdcsPresent(UInt_t *bank) {
  /**
     Check that all the 1190 TDCs that should be present are there.

     Return codes:
       0: All TDCs present
       1: TDCs missing at high end of crate
       2: TDCs missing at low end of crate
       3: Other arrangement of missing 
  */
  UInt_t headermask=0;
  UInt_t trailermask=0;

  UInt_t *p=bank;
  Int_t banklen = *p;
  p++;			// Header word
  while(p++ < bank+banklen) {
    if((*p & 0xf8000000) == 0x40000000) {
      headermask |= (1<<(*p&0x1f));
    } else if ((*p & 0xf8000000) == 0x80000000) {
      trailermask |= (1<<(*p&0x1f));
    }
  }

  if(fFirstTdcCheck) {
    fFirstTdcCheck=kFALSE;
    fTdcMask = headermask | trailermask;
    return(0);			// All TDC present by definition
  } else {
    if((fTdcMask == headermask) && (fTdcMask == trailermask)) {
      return(0);
    } else {
      cout << hex << "Header mask " << headermask << "  Trailer mask " << trailermask << dec << endl;
      cout << "TDC1190 Bank" << endl;
      for(Int_t i=0;i<=banklen;i++) {
	if(i%5 == 0) cout<<endl<<dec<<i<<": ";
	cout << hex << setw(10) << bank[i];
      }
      cout << dec << endl;
      return(1);		// Just return this for now
    }
  }
}
  
ClassImp(THcTimeSyncEvtHandler)
