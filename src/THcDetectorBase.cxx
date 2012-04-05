//*-- Author :    Stephen Wood 30-March-2012

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorBase
//
// Add hitlist to the Hall A detector base
// May not need to inherit from THaDetectorBase since we may end up
// replacing most of the methods
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetectorBase.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "TClonesArray.h"

using namespace std;

THcDetectorBase::THcDetectorBase( const char* name,
				  const char* description ) :
  THaDetectorBase(name, description)
{
  // Normal constructor.

  fRawHitList = NULL;

}

THcDetectorBase::THcDetectorBase() : THaDetectorBase() {
}

THcDetectorBase::~THcDetectorBase() {
  // Destructor
}

void THcDetectorBase::InitHitlist(const char *hitclass, Int_t maxhits) {
  // Probably called by ReadDatabase
  fRawHitList = new TClonesArray(hitclass, maxhits);
  fRawHitClass = fRawHitList->GetClass();
  fNMaxRawHits = maxhits;
  fNRawHits = 0;
  for(Int_t i=0;i<maxhits;i++) {
    fRawHitList->New(i);
  }
}
   

Int_t THcDetectorBase::Decode( const THaEvData& evdata ) {
  THcRawHit* rawhit;
  fRawHitList->Clear("C");
  fNRawHits = 0;

  for ( Int_t i=0; i < fDetMap->GetSize(); i++ ) {
    THaDetMap::Module* d = fDetMap->GetModule(i);

    // Loop over all channels that have a hit.
    for ( Int_t j=0; j < evdata.GetNumChan( d->crate, d->slot); j++) {
      
      Int_t chan = evdata.GetNextChan( d->crate, d->slot, j );
      if( chan < d->lo || chan > d->hi ) continue;     // Not one of my channels
      
      // Need to convert crate, slot, chan into plane, counter, signal
      // Search hitlist for this plane,counter,signal
      Int_t plane = d->plane;
      Int_t signal = d->signal;
      Int_t counter = d->reverse ? d->first + d->hi - chan : d->first + chan - d->lo;
	
      // Search hit list for plane and counter
      // We could do sorting 
      Int_t thishit = 0;
      while(thishit < fNRawHits) {
	rawhit = (THcRawHit*) (*fRawHitList)[thishit];
	if (plane == rawhit->fPlane
	    && counter == rawhit->fCounter) {
	  break;
	}
	thishit++;
      }
      if(thishit == fNRawHits) {
	fNRawHits++;
	rawhit = (THcRawHit*) (*fRawHitList)[thishit];
	rawhit->fPlane = plane;
	rawhit->fCounter = counter;
      }
	
      // Get the data from this channel
      // Allow for multiple hits
      Int_t nMHits = evdata.GetNumHits(d->crate, d->slot, chan);
      for (Int_t mhit = 0; mhit < nMHits; mhit++) {
	Int_t data = evdata.GetData( d->crate, d->slot, chan, mhit);
	rawhit->SetData(signal,data);
      }
    }
  }
  fRawHitList->Sort(fNRawHits);

  return fNRawHits;		// Does anything care what is returned
}

ClassImp(THcDetectorBase)
