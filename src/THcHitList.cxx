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

#include <cassert>

using namespace std;

#define SUPPRESSMISSINGADCREFTIMEMESSAGES 1
THcHitList::THcHitList()
  : fNRawHits(0)
  , fNMaxRawHits(0)
  , fTDC_RefTimeCut(0)
  , fADC_RefTimeCut(0)
  , fTDC_RefTimeBest(false)
  , fADC_RefTimeBest(false)
  , fRawHitList(nullptr)
  , fRawHitClass(nullptr)
  , fdMap(nullptr)
  , fNRefIndex(0)
  , fNSignals(0)
  , fSignalTypes(nullptr)
  , fPSE125(nullptr)
  , fHaveFADCInfo(false)
  , fNSA(-1)
  , fNSB(-1)
  , fNPED(-1)
  , fNTDCRef_miss(0)
  , fNADCRef_miss(0)
  , fMap(nullptr)
  , fTISlot(0)
  , fTICrate(0)
  , fDisableSlipCorrection(false)
{
  /// Normal constructor.
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
  auto* rawhit = (THcRawHit*) (*fRawHitList)[0];
  fNSignals = rawhit->GetNSignals();
  fSignalTypes = new THcRawHit::ESignalType[fNSignals];
  for(UInt_t isig=0;isig<fNSignals;isig++) {
    fSignalTypes[isig] = rawhit->GetSignalType(isig);
  }

  fdMap = detmap;

  /* Pull out all the reference channels */
  fNRefIndex = 0;
  fRefIndexMaps.clear();
  fRefIdxDefined.clear();
  /* Find the biggest refindex */
  for (UInt_t i=0; i < fdMap->GetSize(); i++) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    if(d->plane >= 1000) {
      auto refindex = static_cast<Int_t>(d->signal);
      if( refindex >= fNRefIndex) {
        fNRefIndex = refindex + 1;
      }
    }
  }
  // Create the vector
  fRefIndexMaps.resize(fNRefIndex);
  fRefIdxDefined.reserve(fNRefIndex/10);
  // Put the refindex mapping information in the vector
  for( UInt_t i = 0; i < fdMap->GetSize(); i++ ) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    if( d->plane >= 1000 ) {        // This is a reference time definition
      auto refindex = static_cast<Int_t>(d->signal); // Really an Int_t
      if( refindex >= 0 ) {
        auto& theMap = fRefIndexMaps[refindex];
        theMap.crate = d->crate;
        theMap.slot = d->slot;
        theMap.channel = d->lo;
        theMap.defined = kTRUE;
        fRefIdxDefined.push_back(refindex);
      } else {
        cout << "Hitlist: Invalid refindex mapping" << endl;
      }
    }
  }
  // Loop to check that requested refindex's are defined
  // and that signal #'s are in range
  for( UInt_t i = 0; i < fdMap->GetSize(); i++ ) {
    THaDetMap::Module* d = fdMap->GetModule(i);
    Int_t refindex = d->refindex;
    if( d->plane < 1000 ) {
      if( d->signal >= fNSignals ) {
        cout << "Invalid signal " << d->signal << " for " <<
             " (" << d->crate << ", " << d->slot <<
             ", " << d->lo << ")" << endl;
      }
      if( refindex >= 0 ) {
        if( !fRefIndexMaps[refindex].defined ) {
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
  fPSE125 = lnk ? dynamic_cast<THcConfigEvtHandler*>(lnk->GetObject()) : nullptr;
  if( !fPSE125 ) {
    cout << "THcHitList::InitHitList : Prestart event 125 not found." << endl;
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
Int_t THcHitList::DecodeToHitList( const THaEvData& evdata, Bool_t suppresswarnings )
{
  if( fDisableSlipCorrection )
    fTISlot = -1;

  else if( !fMap ) {                        // Find the TI slot for ADCs
    // Assumes that all FADCs are in the same crate
    //cout << "Got the Crate map" << endl;
    fMap = evdata.GetCrateMap();
    for( UInt_t i = 0; i < fdMap->GetSize(); i++ ) { // Look for a FADC250
      THaDetMap::Module* d = fdMap->GetModule(i);
      auto* isfadc = dynamic_cast<Decoder::Fadc250Module*>(evdata.GetModule(d->crate, d->slot));
      if( isfadc ) {
        // Scan this crate to find the TI.
        for( UInt_t slot = 0; slot < Decoder::MAXSLOT; slot++ ) {
          if( fMap->getModel(d->crate, slot) == 4 ) {
            fTISlot = slot;
            fTICrate = d->crate;
            //cout << "TI Slot = " << fTISlot << endl;
            break;
          }
        }
        // Now make a map of all the FADCs in this crate
        if( fTISlot > 0 ) {
          for( UInt_t slot = 0; slot < Decoder::MAXSLOT; slot++ ) {
            auto* fadc = dynamic_cast<Decoder::Fadc250Module*>(evdata.GetModule(d->crate, slot));
            if( fadc ) {
              fFADCSlotMap[slot] = fadc;
            }
          }
        }
        break;
      }
    }
  }

  UInt_t titime = 0;
  Bool_t TI_TRIGGER_TIME_FOUND = kFALSE;
  if( fTISlot > 0 ) {
    const UInt_t FUDGE = 7;
    if( evdata.GetNumHits(fTICrate, fTISlot, 2) > 0 ) {
      TI_TRIGGER_TIME_FOUND = kTRUE;
      titime = evdata.GetData(fTICrate, fTISlot, 2, 0);
      if( titime >= FUDGE )
        titime -= FUDGE;
      else {
        Warning(Here("DecodeToHitList"),
                "Invalid TI time %u - %u < 0 for crate/slot = %d/%d",
                titime, FUDGE, fTICrate, fTISlot);
        titime = 0;
      }
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
  for( auto i: fRefIdxDefined ) {
    assert(fRefIndexMaps[i].defined);  // else bug in InitHitList
    auto& theMap =  fRefIndexMaps[i];

    if( evdata.IsMultifunction(theMap.crate, theMap.slot) ) {
      // Multifunction module (e.g. FADC)
      Int_t timeshift = 0;
      if( fTISlot > 0 && TI_TRIGGER_TIME_FOUND ) {                // Get the trigger time for this module
        auto ishift = fTrigTimeShiftMap.find(theMap.slot);
        if( ishift == fTrigTimeShiftMap.end() ) {
          auto ifadc = fFADCSlotMap.find(theMap.slot);
          if( ifadc != fFADCSlotMap.end() ) {
            timeshift = fTrigTimeShiftMap[theMap.slot] =
              static_cast<Int_t>(ifadc->second->GetTriggerTime() - titime);
          }
        } else {
          timeshift = ishift->second;
        }
      }
      theMap.hashit = kFALSE;
      Bool_t goodreftime = kFALSE;
      Int_t reftime = 0;
      Int_t prevtime = 0;
      Int_t difftime = 0;
      // Make sure there is at least one pulse
      UInt_t nrefhits = evdata.GetNumEvents(Decoder::kPulseTime,
                                            theMap.crate, theMap.slot, theMap.channel);
      for( UInt_t ihit = 0; ihit < nrefhits; ihit++ ) {
        reftime = evdata.GetData(Decoder::kPulseTime, theMap.crate,
                                 theMap.slot, theMap.channel, ihit);
        reftime += 64 * timeshift;
        if( ihit != 0 )
          difftime = reftime - prevtime;
        prevtime = reftime;
        if( reftime >= fADC_RefTimeCut ) {
          goodreftime = kTRUE;
          break;
        }
      }

      UInt_t nrefsamples = evdata.GetNumEvents(Decoder::kSampleADC,
                                               theMap.crate, theMap.slot, theMap.channel);
      if( nrefhits == 0 && nrefsamples > 0 ) {
        Int_t ref_fNSA = 0;
        Int_t ref_fNSB = 0;
        Int_t ref_fNPED = 0;
        if( fPSE125 ) {
          ref_fNSA = fPSE125->GetNSA(theMap.crate);
          ref_fNSB = fPSE125->GetNSB(theMap.crate);
          ref_fNPED = fPSE125->GetNPED(theMap.crate);
          if( ref_fNSA == -1 ) ref_fNSA = 26;
          if( ref_fNSB == -1 ) ref_fNSB = 3;
          if( ref_fNPED == -1 ) ref_fNPED = 4;
        } else {
          ref_fNSA = 26;
          ref_fNSB = 3;
          ref_fNPED = 4;
        }
        // Set F250 parameters.
        auto* refrawhit = new THcRawAdcHit();  // large object, better on the heap
        refrawhit->SetF250Params(ref_fNSA, ref_fNSB, ref_fNPED);
        for( UInt_t isamp = 0; isamp < nrefsamples; isamp++ ) {
          refrawhit->SetSample(
            evdata.GetData(Decoder::kSampleADC,
                           theMap.crate, theMap.slot, theMap.channel, isamp));
        }
        refrawhit->SetSampThreshold(20.);
        refrawhit->SetSampIntTimePedestalPeak();
        UInt_t NRefSampPulses = refrawhit->GetNSampPulses();
        for( UInt_t ihit = 0; ihit < NRefSampPulses; ihit++ ) {
          reftime = refrawhit->GetSampPulseTimeRaw(ihit);
          reftime += 64 * timeshift;
          if( ihit != 0 )
            difftime = reftime - prevtime;
          prevtime = reftime;
          if( reftime >= fADC_RefTimeCut ) {
            goodreftime = kTRUE;
            break;
          }
        }
        delete refrawhit;
      }

      if( goodreftime || (nrefhits > 0 && fADC_RefTimeBest) || (nrefsamples > 0 && fADC_RefTimeBest) ) {
        theMap.reftime = reftime;
        theMap.refdifftime = difftime;
        theMap.hashit = kTRUE;
      }
    }

    else {
      // Assume this is a TDC
      UInt_t nrefhits = evdata.GetNumHits(theMap.crate, theMap.slot, theMap.channel);
      theMap.hashit = kFALSE;
      // Only take first hit in this reference channel that is bigger
      // then fTDC_RefTimeCut
      Bool_t goodreftime = kFALSE;
      Int_t reftime = 0;
      Int_t prevtime = 0;
      Int_t difftime = 0;
      for( UInt_t ihit = 0; ihit < nrefhits; ihit++ ) {
        reftime = evdata.GetData(theMap.crate, theMap.slot, theMap.channel, ihit);
        if( ihit != 0 )
          difftime = reftime - prevtime;
        prevtime = reftime;
        if( reftime >= fTDC_RefTimeCut ) {
          goodreftime = kTRUE;
          break;
        }
      }
      if( goodreftime || (nrefhits > 0 && fTDC_RefTimeBest) ) {
        theMap.reftime = reftime;
        theMap.refdifftime = difftime;
        theMap.hashit = kTRUE;
      }
    }
  }

  map<ULong64_t, THcRawHit*> hitlookup;
  auto lookupkey = []( UInt_t plane, UInt_t counter) -> ULong64_t {
    return (static_cast<ULong64_t>(plane) << 32) + counter;
  };

  THcRawHit* prev_rawhit = nullptr;
  bool sorted = true;
  for ( UInt_t i=0; i < fdMap->GetSize(); i++ ) {
    THaDetMap::Module* d = fdMap->GetModule(i);

    // Loop over all channels that have a hit.
    //    cout << "Crate/Slot: " << d->crate << "/" << d->slot << endl;
    auto plane = static_cast<Int_t>(d->plane);
    if( plane >= 1000 )
      continue; // Skip reference times
    auto signal = static_cast<Int_t>(d->signal);
    UInt_t signaltype = fSignalTypes[signal];
    Bool_t multifunction = evdata.IsMultifunction(d->crate, d->slot);

    for( UInt_t j = 0; j < evdata.GetNumChan(d->crate, d->slot); j++ ) {
      UInt_t chan = evdata.GetNextChan(d->crate, d->slot, j);
      if( chan < d->lo || chan > d->hi )
        continue;     // Not one of my channels

      // Need to convert crate, slot, chan into plane, counter, signal
      // Search hitlist for this plane,counter,signal
      Int_t counter = d->reverse
                      ? static_cast<Int_t>(d->first + d->hi - chan)
                      : static_cast<Int_t>(d->first + chan - d->lo);
      //cout << d->crate << " " << d->slot << " " << chan << " " << plane << " "
      // << counter << " " << signal << endl;
      THcRawHit* rawhit = nullptr;
      // See if we already have a hit for this plane and counter.
      auto found = hitlookup.find(lookupkey(plane, counter));
      if( found != hitlookup.end() )
        rawhit = found->second;
      else {
        rawhit = static_cast<THcRawHit*>(fRawHitList->ConstructedAt(fNRawHits,""));
	rawhit->fPlane = plane;
	rawhit->fCounter = counter;
	//	cout << "Found as " << fNRawHits << " pl = " << plane << " c = " << counter << endl;
        hitlookup.emplace(lookupkey(plane, counter), rawhit);
        fNRawHits++;
        assert(fNRawHits == hitlookup.size());
        if( sorted ) {
          if( prev_rawhit && prev_rawhit->Compare(rawhit) >= 0 )
            sorted = false;
          else
            prev_rawhit = rawhit;
        }
      }

      // Get the data from this channel
      // Allow for multiple hits
      if( signaltype == THcRawHit::kTDC || !multifunction ) {
        UInt_t nMHits = evdata.GetNumHits(d->crate, d->slot, chan);
        for( UInt_t mhit = 0; mhit < nMHits; mhit++ ) {
          Int_t data = evdata.GetData(d->crate, d->slot, chan, mhit);
          // cout << "Signal " << signal << "=" << data << endl;
          rawhit->SetData(signal, data);
        }
        // Get the reference time.
        if( d->refchan >= 0 ) {
          UInt_t nrefhits = evdata.GetNumHits(d->crate, d->slot, d->refchan);
          Bool_t goodreftime = kFALSE;
          Int_t reftime = 0;
          Int_t prevtime = 0;
          Int_t difftime = 0;
          for( UInt_t ihit = 0; ihit < nrefhits; ihit++ ) {
            reftime = evdata.GetData(d->crate, d->slot, d->refchan, ihit);
            if( ihit != 0 ) difftime = reftime - prevtime;
            prevtime = reftime;
            if( reftime >= fTDC_RefTimeCut ) {
              goodreftime = kTRUE;
              break;
            }
          }
          // If RefTimeBest flag set, take the last hit if none of the
          // hits make the RefTimeCut
          if( goodreftime || (nrefhits > 0 && fTDC_RefTimeBest) ) {
            rawhit->SetReference(signal, reftime);
            rawhit->SetReferenceDiff(signal, difftime);
          } else if( !suppresswarnings ) {
            cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
                 " missing for (" << d->crate << ", " << d->slot <<
                 ", " << chan << ")" << endl;
            tdcref_miss = kTRUE;
          }
        } else {
          if( d->refindex >= 0 && d->refindex < static_cast<Int_t>(fNRefIndex) ) {
            const auto& theMap = fRefIndexMaps[d->refindex];
            if( theMap.hashit ) {
              rawhit->SetReference(signal, theMap.reftime);
              rawhit->SetReferenceDiff(signal, theMap.refdifftime);
            } else {
              if( !suppresswarnings ) {
                cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
                     " (" << theMap.crate <<
                     ", " << theMap.slot <<
                     ", " << theMap.channel << ")" <<
                     " missing for (" << d->crate << ", " << d->slot <<
                     ", " << chan << ")" << endl;
                tdcref_miss = kTRUE;
              }
            }
          }
        }
      } else {                        // This is a Flash ADC

        if( fPSE125 ) {
          if( !fHaveFADCInfo ) {
            fNSA = fPSE125->GetNSA(d->crate);
            fNSB = fPSE125->GetNSB(d->crate);
            fNPED = fPSE125->GetNPED(d->crate);
            if( fNSA == -1 ) fNSA = 26;
            if( fNSB == -1 ) fNSB = 3;
            if( fNPED == -1 ) fNPED = 4;
            fHaveFADCInfo = kTRUE;
          }
        } else if( !fHaveFADCInfo ) {
          fNSA = 26;
          fNSB = 3;
          fNPED = 4;
          fHaveFADCInfo = kTRUE;
        }
        rawhit->SetF250Params(fNSA, fNSB, fNPED);

	// Copy the samples
        UInt_t nsamples = evdata.GetNumEvents(Decoder::kSampleADC, d->crate, d->slot, chan);

	// If nsamples comes back zero, may want to suppress further attempts to
	// get sample data for this or all modules
        for( UInt_t isamp = 0; isamp < nsamples; isamp++ ) {
          rawhit->SetSample(signal, evdata.GetData(Decoder::kSampleADC, d->crate, d->slot, chan, isamp));
        }
	// Now get the pulse mode data
	// Pulse area will go into regular SetData, others will use special hit methods
        UInt_t npulses = evdata.GetNumEvents(Decoder::kPulseIntegral, d->crate, d->slot, chan);
        // Assume that the # of pulses for kPulseTime, kPulsePeak and kPulsePedestal are same;
        Int_t timeshift = 0;
        if( fTISlot > 0 && TI_TRIGGER_TIME_FOUND ) {                // Get the trigger time for this module
          auto ishift = fTrigTimeShiftMap.find(d->slot);
          if( ishift == fTrigTimeShiftMap.end() ) {
            auto ifadc = fFADCSlotMap.find(d->slot);
            if( ifadc != fFADCSlotMap.end() ) {
              timeshift = fTrigTimeShiftMap[d->slot] =
                static_cast<Int_t>(ifadc->second->GetTriggerTime() - titime);
            }
          } else {
            timeshift = ishift->second;
          }
        }
        for( UInt_t ipulse = 0; ipulse < npulses; ipulse++ ) {
          rawhit->SetDataTimePedestalPeak(signal,
              evdata.GetData(Decoder::kPulseIntegral, d->crate, d->slot, chan, ipulse),
              evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, chan, ipulse) + 64 * timeshift,
              evdata.GetData(Decoder::kPulsePedestal, d->crate, d->slot, chan, ipulse),
              evdata.GetData(Decoder::kPulsePeak, d->crate, d->slot, chan, ipulse));
	}
        if( nsamples > 0 ) {
          rawhit->SetSampIntTimePedestalPeak(signal);
        }
	// Get the reference time for the FADC pulse time
        if( d->refchan >= 0 ) {        // Reference time for the slot
          UInt_t nrefhits = evdata.GetNumEvents(Decoder::kPulseIntegral,
                                                d->crate, d->slot, d->refchan);
          Int_t reftime = 0;
          Int_t prevtime = 0;
          Int_t difftime = 0;
          timeshift = 0;
          if( fTISlot > 0 && TI_TRIGGER_TIME_FOUND ) {                // Get the trigger time for this module
            auto ishift = fTrigTimeShiftMap.find(d->slot);
            if( ishift == fTrigTimeShiftMap.end() ) {
              auto ifadc = fFADCSlotMap.find(d->slot);
              if( ifadc != fFADCSlotMap.end() ) {
                timeshift = fTrigTimeShiftMap[d->slot] =
                  static_cast<Int_t>(ifadc->second->GetTriggerTime() - titime);
              }
            } else {
              timeshift = ishift->second;
            }
          }
          Bool_t goodreftime = kFALSE;
          for( UInt_t ihit = 0; ihit < nrefhits; ihit++ ) {
            reftime = evdata.GetData(Decoder::kPulseTime, d->crate, d->slot, d->refchan, ihit);
            reftime += 64 * timeshift;
            if( ihit != 0 ) difftime = reftime - prevtime;
            prevtime = reftime;
            if( reftime >= fADC_RefTimeCut ) {
              goodreftime = kTRUE;
              break;
            }
          }

          UInt_t nrefsamples = evdata.GetNumEvents(Decoder::kSampleADC, d->crate, d->slot, d->refchan);
          if( nrefhits == 0 && nrefsamples > 0 ) {
            auto* refrawhit = new THcRawAdcHit();
            for( UInt_t isamp = 0; isamp < nrefsamples; isamp++ ) {
              refrawhit->SetSample(evdata.GetData(Decoder::kSampleADC, d->crate, d->slot, d->refchan, isamp));
            }
            refrawhit->SetSampThreshold(20.);
            refrawhit->SetSampIntTimePedestalPeak();
            UInt_t NRefSampPulses = refrawhit->GetNSampPulses();
            for( UInt_t ihit = 0; ihit < NRefSampPulses; ihit++ ) {
              reftime = refrawhit->GetSampPulseTimeRaw(ihit);
              reftime += 64 * timeshift;
              if( ihit != 0 )
                difftime = reftime - prevtime;
              prevtime = reftime;
              if( reftime >= fADC_RefTimeCut ) {
                goodreftime = kTRUE;
                break;
              }
            }
            delete refrawhit;
          }

          // If RefTimeBest flag set, take the last hit if none of the
          // hits make the RefTimeCut
          if( goodreftime || (nrefhits > 0 && fADC_RefTimeBest) || (nrefsamples > 0 && fADC_RefTimeBest) ) {
            rawhit->SetReference(signal, reftime);
            rawhit->SetReferenceDiff(signal, difftime);
          } else if( !suppresswarnings ) {
#ifndef SUPPRESSMISSINGADCREFTIMEMESSAGES
            cout << "HitList(event=" << evdata.GetEvNum() << "): refchan " << d->refchan <<
              " missing for (" << d->crate << ", " << d->slot <<
              ", " << chan << ")" << endl;
#endif
            adcref_miss = kTRUE;
          }
        } else {
          if( d->refindex >= 0 && d->refindex < fNRefIndex ) {
            const auto& theMap = fRefIndexMaps[d->refindex];
            if( theMap.hashit ) {
              rawhit->SetReference(signal, theMap.reftime);
              rawhit->SetReferenceDiff(signal, theMap.refdifftime);
            } else {
              if( !suppresswarnings ) {
#ifndef SUPPRESSMISSINGADCREFTIMEMESSAGES
                cout << "HitList(event=" << evdata.GetEvNum() << "): refindex " << d->refindex <<
                  " (" << theMap.crate <<
                  ", " << theMap.slot <<
                  ", " << theMap.channel << ")" <<
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
  if( fTISlot > 0 && TI_TRIGGER_TIME_FOUND ) {
    //    cout << "TI ROC: " << fTICrate << "   TI Time: " << titime << endl;
    for( const auto& it: fTrigTimeShiftMap ) {
      if( it.second < -3 || it.second > 3 ) {
        cout << "Big ADC Trigger Time Shift, ROC " << fTICrate << endl;
        cout << it.first << " " << it.second << endl;
      }
    }
  } else if( fTISlot > 0 && !TI_TRIGGER_TIME_FOUND ) {
    //  cout << "TI Trigger Time Not found for event type = " << evdata.GetEvType() << " event num = " << evdata.GetEvNum() << " TI Crate = " <<  fTICrate << " TI Slot = " << fTISlot<< endl;
  }

#endif
  if( !sorted )
    fRawHitList->Sort(fNRawHits);

  fNTDCRef_miss += (tdcref_miss ? 1 : 0);
  fNADCRef_miss += (adcref_miss ? 1 : 0);
  return fNRawHits;		// Does anything care what is returned
}
void THcHitList::CreateMissReportParms(const char *prefix) const
{
  /**

\brief Create parameters to hold missing reference time statistics

Parameters created are ${prefix}_tdcref_miss and ${prefix}_adcref_miss

  */
  cout << "Defining " << Form("%s_tdcref_miss", prefix) << " and " << Form("%s_adcref_miss", prefix) << endl;
  gHcParms->Define(Form("%s_tdcref_miss", prefix), "Missing TDC reference times", fNTDCRef_miss);
  gHcParms->Define(Form("%s_adcref_miss", prefix), "Missing ADC reference times", fNADCRef_miss);
}
void THcHitList::MissReport(const char *name) const
{
  cout << "Missing Ref times:" << setw(20) << name << setw(10) << fNTDCRef_miss << setw(10) << fNADCRef_miss << endl;
}

ClassImp(THcHitList)
