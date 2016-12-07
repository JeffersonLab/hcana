/** \class THcHitList
    \ingroup Base

 Class to build a Hall C ENGINE style list of raw hits from the raw data.
 Detectors that use hit lists need to inherit from this class
 as well as THaTrackingDetector or THaNonTrackingDetector

*/
#include "THcHitList.h"
#include "TError.h"
#include "TClass.h"

using namespace std;

THcHitList::THcHitList()
{
  // Normal constructor.

  fRawHitList = NULL;

}

THcHitList::~THcHitList() {
  // Destructor
  delete fSignalTypes;
}

void THcHitList::InitHitList(THaDetMap* detmap,
				  const char *hitclass, Int_t maxhits) {
  // Save the electronics to detector mapping
  // Initialize a hit array of hits of class hitclass

  fRawHitList = new TClonesArray(hitclass, maxhits);
  fRawHitClass = fRawHitList->GetClass();
  fNMaxRawHits = maxhits;
  fNRawHits = 0;
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
    if(d->plane < 1000 && refindex >= 0) {
      if(!fRefIndexMaps[refindex].defined) {
	cout << "Refindex " << refindex << " not defined for " <<
	  " (" << d->crate << ", " << d->slot <<
	  ", " << d->lo << ")" << endl;
      }
    }
    if(d->signal >= fNSignals) {
      cout << "Invalid signal " << d->signal << " for " <<
	    " (" << d->crate << ", " << d->slot <<
	    ", " << d->lo << ")" << endl;
    }
  }

}

Int_t THcHitList::DecodeToHitList( const THaEvData& evdata ) {
  // Clear the hit list
  // Find all populated channels belonging to the detector and add
  // the data to the hitlist.  A given counter in the detector can have
  // at most one entry in the hit list.  However, the raw "hit" can contain
  // multiple signal types (e.g. ADC+, ADC-, TDC+, TDC-), or multiple
  // hits for multihit tdcs.
  // The hit list is sorted (by plane, counter) after filling.

  // cout << " Clearing TClonesArray " << endl;
  fRawHitList->Clear( );
  fNRawHits = 0;

  // Get the indexed reference times for this event
  for(Int_t i=0;i<fNRefIndex;i++) {
    if(fRefIndexMaps[i].defined) {
      if(evdata.GetNumHits(fRefIndexMaps[i].crate,
			   fRefIndexMaps[i].slot,
			   fRefIndexMaps[i].channel) > 0) {
	// Only take first hit in this reference channel
	fRefIndexMaps[i].reftime =
	  evdata.GetData(fRefIndexMaps[i].crate,fRefIndexMaps[i].slot,
			 fRefIndexMaps[i].channel,0);
	fRefIndexMaps[i].hashit = kTRUE;
      } else {
	fRefIndexMaps[i].hashit = kFALSE;
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
	  if( evdata.GetNumHits(d->crate,d->slot,d->refchan) > 0) {
	    Int_t reftime = evdata.GetData(d->crate, d->slot, d->refchan, 0);
	    rawhit->SetReference(signal, reftime);
	  } else {
	    cout << "HitList: refchan " << d->refindex <<
	      " missing for (" << d->crate << ", " << d->slot <<
	      ", " << chan << ")" << endl;
	  }
	} else {
	  if(d->refindex >=0 && d->refindex < fNRefIndex) {
	    if(fRefIndexMaps[d->refindex].hashit) {
	      rawhit->SetReference(signal, fRefIndexMaps[d->refindex].reftime);
	    } else {
	      cout << "HitList: refindex " << d->refindex <<
		" missing for (" << d->crate << ", " << d->slot <<
		", " << chan << ")" << endl;
	    }
	  }
	}
      } else {			// This is a Flash ADC
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
      }
    }
  }
  fRawHitList->Sort(fNRawHits);

  return fNRawHits;		// Does anything care what is returned
}

ClassImp(THcHitList)
