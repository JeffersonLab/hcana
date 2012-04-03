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

#include "ThcDetectorBase.h"
#include "THcDetMap.h"

using namespace std;

THcDetectorBase::THcDetectorBase( const char* name,
				  const char* description ) :
  THaAnalysisObject(name, description), fNelem(0)
{
  // Normal constructor. Creates an empty detector map.
  // Later can use THaDetectorBase if signal and plane info
  // is added to the THaDetMap in podd.

  fSize[0] = fSize[1] = fSize[2] = 0.0;
  fDetMap = new THcDetMap;
  fRawHitList = NULL;

}

THcDetectorBase::THcDetectorBase() : THaDetectorBase() {
}

THcDetectorBase::~THcDetectorBase() : ~THaDetectorBase() {
}

THcDetectorBase::InitHitlist(const char *hitclass, Int_T maxhits) {
  // Probably called by ReadDatabase
  fRawHitList = new TClonesArray(hitclass, maxhits);
}
   

THcDetectorBase::Decode( THaEvData& evdata )
{
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
      Int_t nHits = evData.GetNumHits(d->crate, d->slot, chan);
      // Get the data. Scintillators are assumed to have only single hit (hit=0)

      

      for (Int_t hit = 0; hit < nHits; hit++) {
	Int_t data = evdata.GetData( d->crate, d->slot, chan, hit);


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
