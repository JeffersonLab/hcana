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
  fNMaxRawHits = maxhits;
  fNRawHits = 0;
}
   

Int_t THcDetectorBase::Decode( const THaEvData& evdata ) {
  THcRawHit* rawhit;
  fRawHitList->Clear("C");
  
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
	
      // Allow for multiple hits

      // Search hit list for plane and counter
      for (Int_t hit = 0; hit < n 

      // Get the data. Scintillators are assumed to have only single hit (hit=0)
      Int_t nMHits = evData.GetNumHits(d->crate, d->slot, chan);
      for (Int_t mhit = 0; mhit < nMHits; mhit++) {
	Int_t data = evdata.GetData( d->crate, d->slot, chan, mhit);




 THcRawHit*       GetHit(Int_t i) const
  { assert(i >=0 && i<GetNHits() );
    return (THcRawHit*)fHits->UncheckedAt(i);}


  while( raw data) {
    // Get a data word
    // Get plane, counter, signal
    // Search list for that plane counter
    // if(plane,counter hit doesn't exist) {
    //   rawhit = fRawHitList[i];
    // } else {
    //   rawhit = static_cast(THcRawHit*>( fRawHitClass->New() );
    // }
    // rawhit->AddSignal(isignal) = datavalue;
  }
}


ClassImp(THcDetectorBase)
