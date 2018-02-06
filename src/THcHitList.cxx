/** \class THcHitList
    \ingroup Base

\brief Builds a Hall C ENGINE style list of raw hits from raw data

 Detectors that use hit lists need to inherit from this class
 as well as THaTrackingDetector or THaNonTrackingDetector

*/
#include "THcHitList.h"
#include "TError.h"
#include "TClass.h"

#include "THcConfigEvtHandler.h"
#include "THaGlobals.h"
#include "TList.h"

using namespace std;

THcHitList::THcHitList()
{
  /// Normal constructor.

  fRawHitList = NULL;
  fPSE125 = NULL;

}

THcHitList::~THcHitList() {
  /// Destructor
  delete fSignalTypes;
}
/**

\brief Save the electronics module to detector mapping and
initialize a hit array of hits of class hitclass

\param[in] detmap Electronics mapping made by THcDetectorMap::FillMap
\param[in] hitclass Name of hit class used by this detector
\param[in] maxhits Maximum number of hits for this detector

*/

void THcHitList::InitHitList(THaDetMap* detmap,
			     const char *hitclass, Int_t maxhits,
			     Int_t tdcref_cut, Int_t adcref_cut) {
  fRawHitList = new TClonesArray(hitclass, maxhits);
  fRawHitClass = fRawHitList->GetClass();
  fNMaxRawHits = maxhits;
  fNRawHits = 0;
  fTDC_RefTimeCut = tdcref_cut;
  fADC_RefTimeCut = adcref_cut;
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
  for (Int_t i=0; i < fdMap->GetSize(); i++) {
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
  for (Int_t i=0; i < fdMap->GetSize(); i++) {
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
  for (Int_t i=0; i < fdMap->GetSize(); i++) {
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

  // cout << " Clearing TClonesArray " << endl;
  fRawHitList->Clear( );
  fNRawHits = 0;

  // Get the indexed reference times for this event
  for(Int_t i=0;i<fNRefIndex;i++) {
    if(fRefIndexMaps[i].defined) {
      if(evdata.IsMultifunction(fRefIndexMaps[i].crate,
				fRefIndexMaps[i].slot)) { // Multifunction module (e.g. FADC)
	// Make sure at least one pulse
	Int_t nrefhits = evdata.GetNumEvents(Decoder::kPulseTime,
					     fRefIndexMaps[i].crate,
					     fRefIndexMaps[i].slot,
					     fRefIndexMaps[i].channel);
	for(Int_t ihit=0; ihit<nrefhits; ihit++) {
	  fRefIndexMaps[i].hashit = kFALSE;
	  Int_t reftime =
	    evdata.GetData(Decoder::kPulseTime,fRefIndexMaps[i].crate,
			   fRefIndexMaps[i].slot, fRefIndexMaps[i].channel,ihit);
	  if(reftime >= fADC_RefTimeCut) {
	    fRefIndexMaps[i].reftime = reftime;
	    fRefIndexMaps[i].hashit = kTRUE;
	    break;
	  }
	}
      } else {			// Assume this is a TDC
	Int_t nrefhits = evdata.GetNumHits(fRefIndexMaps[i].crate,
					   fRefIndexMaps[i].slot,
					   fRefIndexMaps[i].channel);
	fRefIndexMaps[i].hashit = kFALSE;
	// Only take first hit in this reference channel that is bigger
	// then fTDC_RefTimeCut
	for(Int_t ihit=0; ihit<nrefhits; ihit++) {
	  Int_t reftime =
	    evdata.GetData(fRefIndexMaps[i].crate,fRefIndexMaps[i].slot,
			   fRefIndexMaps[i].channel,ihit);
	  if(reftime >= fTDC_RefTimeCut) {
	    fRefIndexMaps[i].reftime = reftime;
	    fRefIndexMaps[i].hashit = kTRUE;
	    break;
	  }
	}
      }
    }
  }
  for ( Int_t i=0; i < fdMap->GetSize(); i++ ) {
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

    for ( Int_t j=0; j < evdata.GetNumChan( d->crate, d->slot); j++) {
      THcRawHit* rawhit=0;

      Int_t chan = evdata.GetNextChan( d->crate, d->slot, j );
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
	  // cout << "Found as " << thishit << "/" << fNRawHits << endl;
	  break;
	}
	thishit++;
      }

      if(thishit == fNRawHits) {
	rawhit = (THcRawHit*) fRawHitList->ConstructedAt(thishit,"");
	fNRawHits++;
	rawhit->fPlane = plane;
	rawhit->fCounter = counter;
      }

      // Get the data from this channel
      // Allow for multiple hits
      if(signaltype == THcRawHit::kTDC || !multifunction) {
	Int_t nMHits = evdata.GetNumHits(d->crate, d->slot, chan);
	for (Int_t mhit = 0; mhit < nMHits; mhit++) {
	  Int_t data = evdata.GetData( d->crate, d->slot, chan, mhit);
	  // cout << "Signal " << signal << "=" << data << endl;
	  rawhit->SetData(signal,data);
	}
	// Get the reference time.  Only take the first hit
	// If a reference channel
	// was specified, it takes precidence of reference index
	if(d->refchan >= 0) {
	  Int_t nrefhits = evdata.GetNumHits(d->crate,d->slot,d->refchan);
	  Bool_t goodreftime=kFALSE;
	  for(Int_t ihit=0; ihit<nrefhits; ihit++) {
	    Int_t reftime = evdata.GetData(d->crate, d->slot, d->refchan, ihit);
	    if(reftime >= fTDC_RefTimeCut) {
	      rawhit->SetReference(signal, reftime);
	      goodreftime=kTRUE;
	      break;
	    }
	  }
	  if(!goodreftime&&!suppresswarnings) {
	    cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
	      " missing for (" << d->crate << ", " << d->slot <<
	      ", " << chan << ")" << endl;
	  }
	} else {
	  if(d->refindex >=0 && d->refindex < fNRefIndex) {
	    if(fRefIndexMaps[d->refindex].hashit) {
	      rawhit->SetReference(signal, fRefIndexMaps[d->refindex].reftime);
	    } else {
	      if(!suppresswarnings) {
		cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
		  " (" << fRefIndexMaps[d->refindex].crate <<
		  ", " << fRefIndexMaps[d->refindex].slot <<
		  ", " << fRefIndexMaps[d->refindex].channel << ")" <<
		  " missing for (" << d->crate << ", " << d->slot <<
		  ", " << chan << ")" << endl;
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
	Int_t nsamples=evdata.GetNumEvents(Decoder::kSampleADC, d->crate, d->slot, chan);

	// If nsamples comes back zero, may want to suppress further attempts to
	// get sample data for this or all modules
	for (Int_t isamp=0;isamp<nsamples;isamp++) {
	  rawhit->SetSample(signal,evdata.GetData(Decoder::kSampleADC, d->crate, d->slot, chan, isamp));
	}
	// Now get the pulse mode data
	// Pulse area will go into regular SetData, others will use special hit methods
	Int_t npulses=evdata.GetNumEvents(Decoder::kPulseIntegral, d->crate, d->slot, chan);
	// Assume that the # of pulses for kPulseTime, kPulsePeak and kPulsePedestal are same;
	for (Int_t ipulse=0;ipulse<npulses;ipulse++) {
	  rawhit->SetDataTimePedestalPeak(signal,
					  evdata.GetData(Decoder::kPulseIntegral, d->crate, d->slot, chan, ipulse),
					  evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, chan, ipulse),
					  evdata.GetData(Decoder::kPulsePedestal, d->crate, d->slot, chan, ipulse),
					  evdata.GetData(Decoder::kPulsePeak, d->crate, d->slot, chan, ipulse));
	}
	// Get the reference time for the FADC pulse time
	if(d->refchan >= 0) {	// Reference time for the slot
	  Int_t nrefhits = evdata.GetNumEvents(Decoder::kPulseIntegral,
					       d->crate, d->slot, d->refchan);
	  Bool_t goodrefhit=kFALSE;
	  for(Int_t ihit=0; ihit<nrefhits; ihit++) {
	    Int_t reftime = evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, d->refchan, ihit);
	    if(reftime >= fADC_RefTimeCut) {
	      rawhit->SetReference(signal, reftime);
	      goodrefhit=kTRUE;
	      break;
	    }
	  }
	  if(!goodrefhit && !suppresswarnings) {
	    cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
	      " missing for (" << d->crate << ", " << d->slot <<
	      ", " << chan << ")" << endl;
	  }
	} else {
	  if(d->refindex >=0 && d->refindex < fNRefIndex) {
	    if(fRefIndexMaps[d->refindex].hashit) {
	      rawhit->SetReference(signal, fRefIndexMaps[d->refindex].reftime);
	    } else {
	      if(!suppresswarnings) {
		cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
		  " (" << fRefIndexMaps[d->refindex].crate <<
		  ", " << fRefIndexMaps[d->refindex].slot <<
		  ", " << fRefIndexMaps[d->refindex].channel << ")" <<
		  " missing for (" << d->crate << ", " << d->slot <<
		  ", " << chan << ")" << endl;
	      }
	    }
	  }
	}
      }
    }
  }
  fRawHitList->Sort(fNRawHits);
  
  return fNRawHits;		// Does anything care what is returned
}

ClassImp(THcHitList)
