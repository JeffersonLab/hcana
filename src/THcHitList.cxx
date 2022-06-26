// Remove this line to restore missing ADC reference time messages
/** \class THcHitList
    \ingroup Base

\brief Builds a Hall C ENGx1INE style list of raw hits from raw data

 Detectors that use hit lists need to inherit from this class
 as well as THaTrackingDetector or THaNonTrackingDetector

*/
#include "THcHitList.h"
#include "TError.h"
#include "TClass.h"

#include "THcConfigEvtHandler.h"
#include "THcRawAdcHit.h"
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "TList.h"

using namespace std;

#define SUPPRESSMISSINGADCREFTIMEMESSAGES 1
THcHitList::THcHitList() : fMap(0), fTISlot(0), fDisableSlipCorrection(kFALSE)
{
  /// Normal constructor.

  fRawHitList = NULL;
  fPSE125 = NULL;
  fFADCSlotMap.clear();

}

THcHitList::~THcHitList() {
  /// Destructor
  delete fRawHitList;
  delete [] fSignalTypes;
}
/**

\brief Save the electronics module to detector mapping and
initialize a hit array of hits of class hitclass

\param[in] detmap Electronics mapping made by THcDetectorMap::FillMap
\param[in] hitclass Name of hit class used by this detector
\param[in] maxhits Maximum number of hits for this detector

*/


/* InitHitList should make a list of ROCs that have FADCs in them.  (Probably
just one.)  It then needs to figure out where the TI is and make sure
the decoder is setup for pulling out the event time.

Do we need to somehow configure this in the Hall C style map file.  Is there
a method to ask the OO decoder what kind of module is in a given slot?

*/

void THcHitList::InitHitList(THaDetMap* detmap,
			     const char *hitclass, Int_t maxhits,
			     Int_t tdcref_cut, Int_t adcref_cut) {
  cout << "InitHitList: " << hitclass << " RefTimeCuts: " << tdcref_cut << " " << adcref_cut << endl;
  fRawHitList = new TClonesArray(hitclass, maxhits);
  fRawHitClass = fRawHitList->GetClass();
  fNMaxRawHits = maxhits;
  fNRawHits = 0;

  if(tdcref_cut >= 0) {
    fTDC_RefTimeBest = kFALSE;
    fTDC_RefTimeCut = tdcref_cut;
  } else {
    fTDC_RefTimeBest = kTRUE;
    fTDC_RefTimeCut = -tdcref_cut;
  }
  if(adcref_cut >= 0) {
    fADC_RefTimeBest = kFALSE;
    fADC_RefTimeCut = adcref_cut;
  } else {
    fADC_RefTimeBest = kTRUE;
    fADC_RefTimeCut = -adcref_cut;
  }

  for(Int_t i=0;i<maxhits;i++) {
    fRawHitList->ConstructedAt(i);
  }
  // Query a raw hit object to see what kind of data to deliver
  THcRawHit* rawhit = (THcRawHit*) (*fRawHitList)[0];
  fNSignals = rawhit->GetNSignals();
  fSignalTypes = new THcRawHit::ESignalType[fNSignals];
  for(UInt_t isig=0;isig<fNSignals;isig++) {
    fSignalTypes[isig] = rawhit->GetSignalType(isig);
  }

  fdMap = detmap;

  /* Pull out all the reference channels */
  fNRefIndex = 0;
  fRefIndexMaps.clear();
  /* Find the biggest refindex */
  for (UInt_t i=0; i < fdMap->GetSize(); i++) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    if(d->plane >= 1000) {
      Int_t refindex = d->signal;
      if(refindex>=fNRefIndex) {
	fNRefIndex = refindex+1;
      }
    }
  }
  // Create the vector.  Could roll this into last loop
  for(Int_t i=0;i<fNRefIndex;i++) {
    RefIndexMap map;
    map.defined = kFALSE;
    map.hashit = kFALSE;
    fRefIndexMaps.push_back(map);
  }
  // Put the refindex mapping information in the vector
  for (UInt_t i=0; i < fdMap->GetSize(); i++) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    if(d->plane >= 1000) {	// This is a reference time definition
      Int_t refindex = d->signal;
      if(refindex >= 0) {
	fRefIndexMaps[refindex].crate = d->crate;
	fRefIndexMaps[refindex].slot = d->slot;
	fRefIndexMaps[refindex].channel = d->lo;
	fRefIndexMaps[refindex].defined = kTRUE;
      } else {
	cout << "Hitlist: Invalid refindex mapping" << endl;
      }
    }
  }
  // Loop to check that requested refindex's are defined
  // and that signal #'s are in range
  for (UInt_t i=0; i < fdMap->GetSize(); i++) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    Int_t refindex = d->refindex;
    if(d->plane < 1000) {
      if(d->signal >= fNSignals) {
	cout << "Invalid signal " << d->signal << " for " <<
	  " (" << d->crate << ", " << d->slot <<
	  ", " << d->lo << ")" << endl;
      }
      if(refindex >= 0) {
	if(!fRefIndexMaps[refindex].defined) {
	  cout << "Refindex " << refindex << " not defined for " <<
	    " (" << d->crate << ", " << d->slot <<
	    ", " << d->lo << ")" << endl;
	}
      }
    }
  }

  // Find the Event 125 handler
  TObjLink *lnk = gHaEvtHandlers->FirstLink();
  while (lnk) {
    if(strcmp(lnk->GetObject()->ClassName(),"THcConfigEvtHandler")==0) {
      break;
    }
    lnk = lnk->Next();
  }
  if(lnk) {
    fPSE125 = static_cast<THcConfigEvtHandler*>(lnk->GetObject());
  } else {
    cout << "THcHitList::InitHitList : Prestart event 125 not found." << endl;
    fPSE125 = 0;
  }
  fHaveFADCInfo = kFALSE;

  fNTDCRef_miss = 0;
  fNADCRef_miss = 0;

  //  DisableSlipCorrection();
}

/**

\brief Populate the hitlist from the raw event data.

Clears the hit list then, finds all populated channels belonging to the detector and add
sort it into the hitlist.  A given counter in the detector can have
at most one entry in the hit list.  However, the raw "hit" can contain
multiple signal types (e.g. ADC+, ADC-, TDC+, TDC-), or multiplehits for multihit tdcs.
The hit list is sorted (by plane, counter) after filling.

*/
Int_t THcHitList::DecodeToHitList( const THaEvData& evdata, Bool_t suppresswarnings ) {

  if(!fMap) {			// Find the TI slot for ADCs
    // Assumes that all FADCs are in the same crate
    //cout << "Got the Crate map" << endl;
    fMap = evdata.GetCrateMap();
    for (UInt_t i=0; i < fdMap->GetSize(); i++) { // Look for a FADC250
      THaDetMap::Module* d = fdMap->GetModule(i);
      Decoder::Fadc250Module* isfadc = dynamic_cast<Decoder::Fadc250Module*>(evdata.GetModule(d->crate, d->slot));
      if(isfadc) {
	// Scan this crate to find the TI.
	for(UInt_t slot=0;slot<Decoder::MAXSLOT;slot++) {
	  if(fMap->getModel(d->crate, slot) == 4) {
	    fTISlot = slot;
	    fTICrate = d->crate;
	    //cout << "TI Slot = " << fTISlot << endl;
	    break;
	  }
	}
	// Now make a map of all the FADCs in this crate
	if(fTISlot>0) {
	  for(UInt_t slot=0;slot<Decoder::MAXSLOT;slot++) {
	    Decoder::Fadc250Module* fadc = dynamic_cast<Decoder::Fadc250Module*>
	      (evdata.GetModule(d->crate, slot));
	    if(fadc) {
	      fFADCSlotMap[slot] = fadc;
	    }
	  }	    
	}
	break;
      }
    }
  }
  if(fDisableSlipCorrection) fTISlot = -1;
    
  UInt_t titime = 0;
  Bool_t TI_TRIGGER_TIME_FOUND = kFALSE;
  if(fTISlot>0) {
    UInt_t FUDGE=7;
    if (evdata.GetNumHits(fTICrate, fTISlot, 2) > 0) {
    TI_TRIGGER_TIME_FOUND = kTRUE;
    titime = evdata.GetData(fTICrate, fTISlot, 2, 0)-FUDGE;
    // Need to get the FADC time for all modules in this crate
    // that have hits.  Make a map with these times.
    fTrigTimeShiftMap.clear();
    }
  }

  // cout << " Clearing TClonesArray " << endl;
  fRawHitList->Clear( );
  fNRawHits = 0;
  Bool_t tdcref_miss = kFALSE;
  Bool_t adcref_miss = kFALSE;

  // Get the indexed reference times for this event
  for(Int_t i=0;i<fNRefIndex;i++) {
    if(fRefIndexMaps[i].defined) {     
      if(evdata.IsMultifunction(fRefIndexMaps[i].crate,fRefIndexMaps[i].slot)) { // Multifunction module (e.g. FADC)
	// Make sure at least one pulse
  	UInt_t nrefhits = evdata.GetNumEvents(Decoder::kPulseTime,
                                              fRefIndexMaps[i].crate,
                                              fRefIndexMaps[i].slot,
                                              fRefIndexMaps[i].channel);
	Int_t timeshift=0;
	if(fTISlot>0 && TI_TRIGGER_TIME_FOUND) {		// Get the trigger time for this module
	  if(fTrigTimeShiftMap.find(fRefIndexMaps[i].slot)
	     == fTrigTimeShiftMap.end()) { // 
	    if(fFADCSlotMap.find(fRefIndexMaps[i].slot) != fFADCSlotMap.end()) {
	      fTrigTimeShiftMap[fRefIndexMaps[i].slot]
		= fFADCSlotMap[fRefIndexMaps[i].slot]->GetTriggerTime() - titime;
	    }
	    timeshift = fTrigTimeShiftMap[fRefIndexMaps[i].slot];
	  }
	}
	fRefIndexMaps[i].hashit = kFALSE;
	Bool_t goodreftime=kFALSE;
	Int_t reftime = 0;
	Int_t prevtime = 0;
	Int_t difftime = 0;
	for(UInt_t ihit=0; ihit<nrefhits; ihit++) {
	  reftime = evdata.GetData(Decoder::kPulseTime,fRefIndexMaps[i].crate,
				   fRefIndexMaps[i].slot, fRefIndexMaps[i].channel,ihit);
	  reftime += 64*timeshift;
	  if (ihit != 0) difftime=reftime-prevtime;
	  prevtime = reftime;
	  if(reftime >= fADC_RefTimeCut) {
	    goodreftime = kTRUE;
	    break;
	  }
	}
	//
	UInt_t nrefsamples=evdata.GetNumEvents(Decoder::kSampleADC,fRefIndexMaps[i].crate,fRefIndexMaps[i].slot, fRefIndexMaps[i].channel);
	if (nrefhits==0 && nrefsamples>0) {
        THcRawAdcHit* refrawhit = new THcRawAdcHit();
	      Int_t ref_fNSA = 0;
	      Int_t ref_fNSB = 0;
 	      Int_t ref_fNPED = 0;
            if (fPSE125) {
	      ref_fNSA = fPSE125->GetNSA(fRefIndexMaps[i].crate);
	      ref_fNSB = fPSE125->GetNSB(fRefIndexMaps[i].crate);
 	      ref_fNPED = fPSE125->GetNPED(fRefIndexMaps[i].crate);
  	      }
	  // Set F250 parameters.
          refrawhit->SetF250Params(ref_fNSA, ref_fNSB, ref_fNPED);
	    for (UInt_t isamp=0;isamp<nrefsamples;isamp++) {
	      refrawhit->SetSample(evdata.GetData(Decoder::kSampleADC,fRefIndexMaps[i].crate,fRefIndexMaps[i].slot, fRefIndexMaps[i].channel, isamp));
	     }
	    refrawhit->SetSampThreshold(20.);
	    refrawhit->SetSampIntTimePedestalPeak();
	    Int_t NRefSampPulses = refrawhit->GetNSampPulses();
     	    for(Int_t ihit=0; ihit<NRefSampPulses; ihit++) {
	        reftime = refrawhit->GetSampPulseTimeRaw(ihit);
	        reftime += 64*timeshift;
	        if (ihit != 0) difftime=reftime-prevtime;
	        prevtime = reftime;
	        if(reftime >= fADC_RefTimeCut) {
	           goodreftime = kTRUE;
	           break;
	        }
	     }
      }
	//
	if(goodreftime || (nrefhits>0 && fADC_RefTimeBest)|| (nrefsamples>0 && fADC_RefTimeBest)) {
	  fRefIndexMaps[i].reftime = reftime;
	  fRefIndexMaps[i].refdifftime = difftime;
	  fRefIndexMaps[i].hashit = kTRUE;
	}
      } else {			// Assume this is a TDC
	Int_t nrefhits = evdata.GetNumHits(fRefIndexMaps[i].crate,
					   fRefIndexMaps[i].slot,
					   fRefIndexMaps[i].channel);
	fRefIndexMaps[i].hashit = kFALSE;
	// Only take first hit in this reference channel that is bigger
	// then fTDC_RefTimeCut
	Bool_t goodreftime=kFALSE;
	Int_t reftime = 0;
	Int_t prevtime = 0;
	Int_t difftime = 0;
	for(Int_t ihit=0; ihit<nrefhits; ihit++) {
	  reftime = evdata.GetData(fRefIndexMaps[i].crate,fRefIndexMaps[i].slot,
				   fRefIndexMaps[i].channel,ihit);
	  if( ihit != 0) difftime=reftime-prevtime;
	    prevtime=reftime;
	  if(reftime >= fTDC_RefTimeCut) {
	    goodreftime = kTRUE;
	    break;
	  }
	}
	if(goodreftime || (nrefhits>0 && fTDC_RefTimeBest)) {
	    fRefIndexMaps[i].reftime = reftime;
	    fRefIndexMaps[i].refdifftime = difftime;
	    fRefIndexMaps[i].hashit = kTRUE;
	}
      }
    }
  }
  for ( UInt_t i=0; i < fdMap->GetSize(); i++ ) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    
    // Loop over all channels that have a hit.
    //    cout << "Crate/Slot: " << d->crate << "/" << d->slot << endl;
    Int_t plane = d->plane;
    if (plane >= 1000) continue; // Skip reference times
    Int_t signal = d->signal;
    UInt_t signaltype = fSignalTypes[signal];
    Bool_t multifunction = evdata.IsMultifunction(d->crate, d->slot);
    // Should probably get the Decoder::Module object and use it's
    // methods.  Saving a THaEvData::GetModule call every time

    for ( UInt_t j=0; j < evdata.GetNumChan( d->crate, d->slot); j++) {
      THcRawHit* rawhit=0;

      UInt_t chan = evdata.GetNextChan( d->crate, d->slot, j );
      if( chan < d->lo || chan > d->hi ) continue;     // Not one of my channels

      // Need to convert crate, slot, chan into plane, counter, signal
      // Search hitlist for this plane,counter,signal
      Int_t counter = d->reverse ? d->first + d->hi - chan : d->first + chan - d->lo;
      //cout << d->crate << " " << d->slot << " " << chan << " " << plane << " "
      // << counter << " " << signal << endl;
      // Search hit list for plane and counter
      // We could do sorting
      UInt_t thishit = 0;
      while(thishit < fNRawHits) {
	rawhit = (THcRawHit*) (*fRawHitList)[thishit];
	if (plane == rawhit->fPlane
	    && counter == rawhit->fCounter) {
	  //  cout << "Found as " << thishit << "/" << fNRawHits << " pl = " << plane << " c = " << counter << endl;
	  break;
	}
	thishit++;
      }

      if(thishit == fNRawHits) {
	rawhit = (THcRawHit*) fRawHitList->ConstructedAt(thishit,"");
	fNRawHits++;
	rawhit->fPlane = plane;
	rawhit->fCounter = counter;
	//	cout << "Found as " << thishit << " = " << fNRawHits << " pl = " << plane << " c = " << counter << endl;
      }

      // Get the data from this channel
      // Allow for multiple hits
      if(signaltype == THcRawHit::kTDC || !multifunction) {
	UInt_t nMHits = evdata.GetNumHits(d->crate, d->slot, chan);
	for (UInt_t mhit = 0; mhit < nMHits; mhit++) {
          UInt_t data = evdata.GetData( d->crate, d->slot, chan, mhit);
	  // cout << "Signal " << signal << "=" << data << endl;
	  rawhit->SetData(signal,data);
	}
	// Get the reference time.
	if(d->refchan >= 0) {
	  UInt_t nrefhits = evdata.GetNumHits(d->crate,d->slot,d->refchan);
	  Bool_t goodreftime=kFALSE;
	  Int_t reftime=0;
	  Int_t prevtime=0;
	  Int_t difftime=0;
	  for(UInt_t ihit=0; ihit<nrefhits; ihit++) {
	    reftime = evdata.GetData(d->crate, d->slot, d->refchan, ihit);
	    if (ihit != 0 ) difftime=reftime-prevtime;
              prevtime = reftime;
	    if(reftime >= fTDC_RefTimeCut) {
	      goodreftime = kTRUE;
	      break;
	    }
	  }
	  // If RefTimeBest flag set, take the last hit if none of the
	  // hits make the RefTimeCut
	  if(goodreftime || (nrefhits>0 && fTDC_RefTimeBest)) {
	    rawhit->SetReference(signal, reftime);
	    rawhit->SetReferenceDiff(signal, difftime);
	  } else if (!suppresswarnings) {
	    cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
	      " missing for (" << d->crate << ", " << d->slot <<
	      ", " << chan << ")" << endl;
	      tdcref_miss = kTRUE;
	  }
	} else {
	  if(d->refindex >=0 && d->refindex < fNRefIndex) {
	    if(fRefIndexMaps[d->refindex].hashit) {
	      rawhit->SetReference(signal, fRefIndexMaps[d->refindex].reftime);
	      rawhit->SetReferenceDiff(signal, fRefIndexMaps[d->refindex].refdifftime);
	    } else {
	      if(!suppresswarnings) {
		cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
		  " (" << fRefIndexMaps[d->refindex].crate <<
		  ", " << fRefIndexMaps[d->refindex].slot <<
		  ", " << fRefIndexMaps[d->refindex].channel << ")" <<
		  " missing for (" << d->crate << ", " << d->slot <<
		  ", " << chan << ")" << endl;
		tdcref_miss = kTRUE;
	      }
	    }
	  }
	}
      } else {			// This is a Flash ADC

        if (fPSE125) {
	  if(!fHaveFADCInfo) {
	    fNSA = fPSE125->GetNSA(d->crate);
	    fNSB = fPSE125->GetNSB(d->crate);
	    fNPED = fPSE125->GetNPED(d->crate);
	    fHaveFADCInfo = kTRUE;
	  }
	  // Set F250 parameters.
          rawhit->SetF250Params(fNSA, fNSB, fNPED);
        }
	
	// Copy the samples
	UInt_t nsamples=evdata.GetNumEvents(Decoder::kSampleADC, d->crate, d->slot, chan);

	// If nsamples comes back zero, may want to suppress further attempts to
	// get sample data for this or all modules
	for (UInt_t isamp=0;isamp<nsamples;isamp++) {
	  rawhit->SetSample(signal,evdata.GetData(Decoder::kSampleADC, d->crate, d->slot, chan, isamp));
	}
	// Now get the pulse mode data
	// Pulse area will go into regular SetData, others will use special hit methods
	UInt_t npulses=evdata.GetNumEvents(Decoder::kPulseIntegral, d->crate, d->slot, chan);
	// Assume that the # of pulses for kPulseTime, kPulsePeak and kPulsePedestal are same;
	Int_t timeshift=0;
	if(fTISlot>0 && TI_TRIGGER_TIME_FOUND) {		// Get the trigger time for this module
	  if(fTrigTimeShiftMap.find(d->slot)
	     == fTrigTimeShiftMap.end()) { // 
	    if(fFADCSlotMap.find(d->slot) != fFADCSlotMap.end()) {
	      fTrigTimeShiftMap[d->slot]
		= fFADCSlotMap[d->slot]->GetTriggerTime() - titime;
	    }
	  }
	  timeshift = fTrigTimeShiftMap[d->slot];
	}
	for (UInt_t ipulse=0;ipulse<npulses;ipulse++) {
	  rawhit->SetDataTimePedestalPeak(signal,
					  evdata.GetData(Decoder::kPulseIntegral, d->crate, d->slot, chan, ipulse),
					  evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, chan, ipulse)+64*timeshift,
					  evdata.GetData(Decoder::kPulsePedestal, d->crate, d->slot, chan, ipulse),
					  evdata.GetData(Decoder::kPulsePeak, d->crate, d->slot, chan, ipulse));
	}
	if (nsamples>0) {
	  	  rawhit->SetSampIntTimePedestalPeak(signal);
	}
	// Get the reference time for the FADC pulse time
	if(d->refchan >= 0) {	// Reference time for the slot
	  UInt_t nrefhits = evdata.GetNumEvents(Decoder::kPulseIntegral,
                                                d->crate, d->slot, d->refchan);
	  Bool_t goodreftime=kFALSE;
	  Int_t reftime = 0;
	  Int_t prevtime = 0;
	  Int_t difftime = 0;
	  timeshift=0;
	  if(fTISlot>0 && TI_TRIGGER_TIME_FOUND) {		// Get the trigger time for this module
	    if(fTrigTimeShiftMap.find(d->slot)
	       == fTrigTimeShiftMap.end()) { // 
	      if(fFADCSlotMap.find(d->slot) != fFADCSlotMap.end()) {
		fTrigTimeShiftMap[d->slot]
		  = fFADCSlotMap[d->slot]->GetTriggerTime() - titime;
	      }
	    }
	    timeshift = fTrigTimeShiftMap[d->slot];
	  }
	  for(UInt_t ihit=0; ihit<nrefhits; ihit++) {
	    reftime = evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, d->refchan, ihit);
	    reftime += 64*timeshift;
	    if (ihit != 0) difftime=reftime-prevtime;
	    prevtime=reftime;
	    if(reftime >= fADC_RefTimeCut) {
	      goodreftime=kTRUE;
	      break;
	    }
	  }
	//
	UInt_t nrefsamples=evdata.GetNumEvents(Decoder::kSampleADC, d->crate, d->slot, d->refchan);
	if (nrefhits==0 && nrefsamples>0) {
            THcRawAdcHit* refrawhit = new THcRawAdcHit();
	    for (UInt_t isamp=0;isamp<nrefsamples;isamp++) {
	      refrawhit->SetSample(evdata.GetData(Decoder::kSampleADC, d->crate, d->slot, d->refchan, isamp));
	     }
	    refrawhit->SetSampThreshold(20.);
	    refrawhit->SetSampIntTimePedestalPeak();
	    Int_t NRefSampPulses = refrawhit->GetNSampPulses();
     	    for(Int_t ihit=0; ihit<NRefSampPulses; ihit++) {
	        reftime = refrawhit->GetSampPulseTimeRaw(ihit);
	        reftime += 64*timeshift;
	        if (ihit != 0) difftime=reftime-prevtime;
	        prevtime = reftime;
	        if(reftime >= fADC_RefTimeCut) {
	           goodreftime = kTRUE;
	           break;
	      }
	     }
	}
	//
	  // If RefTimeBest flag set, take the last hit if none of the
	  // hits make the RefTimeCut
	  if(goodreftime || (nrefhits>0 && fADC_RefTimeBest)|| (nrefsamples>0 && fADC_RefTimeBest)) {
	    rawhit->SetReference(signal, reftime);
	    rawhit->SetReferenceDiff(signal, difftime);
	  } else if (!suppresswarnings) {
#ifndef SUPPRESSMISSINGADCREFTIMEMESSAGES
	    cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
	      " missing for (" << d->crate << ", " << d->slot <<
	      ", " << chan << ")" << endl;
#endif
	      adcref_miss = kTRUE;
	  }
	} else {
	  if(d->refindex >=0 && d->refindex < fNRefIndex) {
	    if(fRefIndexMaps[d->refindex].hashit) {
	      rawhit->SetReference(signal, fRefIndexMaps[d->refindex].reftime);
	      rawhit->SetReferenceDiff(signal, fRefIndexMaps[d->refindex].refdifftime);
	    } else {
	      if(!suppresswarnings) {
#ifndef SUPPRESSMISSINGADCREFTIMEMESSAGES
		cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
		  " (" << fRefIndexMaps[d->refindex].crate <<
		  ", " << fRefIndexMaps[d->refindex].slot <<
		  ", " << fRefIndexMaps[d->refindex].channel << ")" <<
		  " missing for (" << d->crate << ", " << d->slot <<
		  ", " << chan << ")" << endl;
#endif
		adcref_miss = kTRUE;
	      }
	    }
	  }
	}
      }
    }
  }
#if 1
  if(fTISlot>0 && TI_TRIGGER_TIME_FOUND) {
    //    cout << "TI ROC: " << fTICrate << "   TI Time: " << titime << endl;
    map<Int_t, Int_t>::iterator it;
    for(it=fTrigTimeShiftMap.begin(); it!=fTrigTimeShiftMap.end(); it++) {
      if(it->second < -3 || it->second > 3) {
	cout << "Big ADC Trigger Time Shift, ROC " << fTICrate << endl;
	cout << it->first << " " << it->second << endl;
      }
    }
  } else if ( fTISlot>0 && !TI_TRIGGER_TIME_FOUND) {
    cout << "TI Trigger Time Not found for event type = " << evdata.GetEvType() << " event num = " << evdata.GetEvNum() << " TI Crate = " <<  fTICrate << " TI Slot = " << fTISlot<< endl;
   } 
    
#endif    
  fRawHitList->Sort(fNRawHits);

  fNTDCRef_miss += (tdcref_miss ? 1 : 0);
  fNADCRef_miss += (adcref_miss ? 1 : 0);
  return fNRawHits;		// Does anything care what is returned
}
void THcHitList::CreateMissReportParms(const char *prefix)
{
  /**

\brief Create parameters to hold missing reference time statistics

Parameters created are ${prefix}_tdcref_miss and ${prefix}_adcref_miss

  */
  cout << "Defining " << Form("%s_tdcref_miss", prefix) << " and " << Form("%s_adcref_miss", prefix) << endl;
  gHcParms->Define(Form("%s_tdcref_miss", prefix), "Missing TDC reference times", fNTDCRef_miss);
  gHcParms->Define(Form("%s_adcref_miss", prefix), "Missing ADC reference times", fNADCRef_miss);
}
void THcHitList::MissReport(const char *name)
{
  cout << "Missing Ref times:" << setw(20) << name << setw(10) << fNTDCRef_miss << setw(10) << fNADCRef_miss << endl;
}

ClassImp(THcHitList)
