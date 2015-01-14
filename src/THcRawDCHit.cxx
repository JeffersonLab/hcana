///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawDCHit                                                                  //
//                                                                           //
// Class representing for drift chamber wire (or other device with           //
//   a single multihit TDC channel per detector element                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawDCHit.h"

using namespace std;


void THcRawDCHit::SetData(Int_t signal, Int_t data) {
  fTDC[fNHits++] = data;
}

// Return just the first hit
Int_t THcRawDCHit::GetData(Int_t signal) {
  if(fNHits>0) {
    return(fTDC[0]);
  } else {
    return(-1);
  }
}

// Return a requested hit
Int_t THcRawDCHit::GetData(Int_t signal, Int_t ihit) {
  if(ihit >=0 && ihit< fNHits) {
    return(fTDC[ihit]);
  } else {
    return(-1);
  }
}


Int_t THcRawDCHit::Compare(const TObject* obj) const
{
  // Compare to sort by plane and counter
  // Should we be able to move this into THcRawHit

  const THcRawDCHit* hit = dynamic_cast<const THcRawDCHit*>(obj);

  if(!hit) return -1;
  Int_t p1 = fPlane;
  Int_t p2 = hit->fPlane;
  if(p1 < p2) return -1;
  else if(p1 > p2) return 1;
  else {
    Int_t c1 = fCounter;
    Int_t c2 = hit->fCounter;
    if(c1 < c2) return -1;
    else if (c1 == c2) return 0;
    else return 1;
  }
}
//_____________________________________________________________________________
THcRawDCHit& THcRawDCHit::operator=( const THcRawDCHit& rhs )
{
  // Assignment operator.

  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane = rhs.fPlane;
    fCounter = rhs.fCounter;
    fNHits = rhs.fNHits;
    for(Int_t ihit=0;ihit<fNHits;ihit++) {
      fTDC[ihit] = rhs.fTDC[ihit];
    }
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawDCHit)

