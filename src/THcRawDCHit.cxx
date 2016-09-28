/** \class  THcRawDCHit
    \ingroup DetSupport

 Class representing for drift chamber wire (or other device with      
   a single multihit TDC channel per detector element                 

*/

#include "THcRawDCHit.h"

using namespace std;


void THcRawDCHit::SetData(Int_t signal, Int_t data) {
  fTDC[fNHits++] = data;
}

// Return just the first hit
Int_t THcRawDCHit::GetData(Int_t signal) {
  if(fNHits>0) {
    if(fHasRef) {
      return(fTDC[0]-fReferenceTime);
    } else {
      return(fTDC[0]);
    }
  } else {
    return(-1);
  }
}

// Return just the first hit
Int_t THcRawDCHit::GetRawData(Int_t signal) {
  if(fNHits>0) {
    return(fTDC[0]);
  } else {
    return(-1);
  }
}

// Return a requested hit with reference time subtracted
Int_t THcRawDCHit::GetData(Int_t signal, UInt_t ihit) {
  if(ihit >=0 && ihit< fNHits) {
    if(fHasRef) {
      return(fTDC[ihit]-fReferenceTime);
    } else {
      return(fTDC[ihit]);
    }
  } else {
    return(-1);
  }
}

// Return a requested raw hit
Int_t THcRawDCHit::GetRawData(Int_t signal, UInt_t ihit) {
  if(ihit >=0 && ihit< fNHits) {
    return(fTDC[ihit]);
  } else {
    return(-1);
  }
}

// Set the reference time
void THcRawDCHit::SetReference(Int_t signal, Int_t reference) {
  fReferenceTime = reference;
  fHasRef = kTRUE;
}

// Get the reference time
Int_t THcRawDCHit::GetReference(Int_t signal) {
  if(fHasRef) {
    return(fReferenceTime);
  } else {
    return(0);
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
    fNHits = rhs.fNHits;
    fReferenceTime = rhs.fReferenceTime;
    fHasRef = rhs.fHasRef;
    for(UInt_t ihit=0;ihit<fNHits;ihit++) {
      fTDC[ihit] = rhs.fTDC[ihit];
    }
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawDCHit)

