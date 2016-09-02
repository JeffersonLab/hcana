/** \class THcRawHodoHit
    \ingroup DetSupport

Raw Hodoscope Hit Info

Class representing a single raw hit for a hodoscope paddle  
                                                            
 Contains plane, counter and pos/neg adc and tdc values   
                                                          

*/

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcRawHodoHit.h"

using namespace std;

void THcRawHodoHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos[fNRawHits[0]++] = data;
  } else if (signal==1) {
    fADC_neg[fNRawHits[1]++] = data;
  } else if(signal==2) {
    fTDC_pos[fNRawHits[2]++] = data;
  } else if (signal==3) {
    fTDC_neg[fNRawHits[3]++] = data;
  }
}

Int_t THcRawHodoHit::GetData(Int_t signal) {
  return(GetData(signal,0));
}
Int_t THcRawHodoHit::GetData(Int_t signal, UInt_t ihit) {
  Int_t value;
  if(ihit>= fNRawHits[signal]) {
    return(-1);
  }
  if(signal==0) {
    value = fADC_pos[ihit];
  } else if (signal==1) {
    value = fADC_neg[ihit];
  } else if (signal==2) {
    value = fTDC_pos[ihit];
  } else if (signal==3) {
    value = fTDC_neg[ihit];
  } else {
    return(-1);			// Actually should throw an exception
  }
  // We are return -1 as a error, but reference subtracted
  // time can be negative.  
  if(fHasRef[signal]) {
    value -= fReferenceTime[signal];
  }
  return(value);
}

Int_t THcRawHodoHit::GetRawData(Int_t signal) {
  return(GetRawData(signal,0));
}

// Return a requested raw hit
Int_t THcRawHodoHit::GetRawData(Int_t signal, UInt_t ihit) {
  if(ihit>= fNRawHits[signal]) {
    return(-1);
  }
  if(signal==0) {
    return(fADC_pos[ihit]);
  } else if (signal==1) {
    return(fADC_neg[ihit]);
  } else if (signal==2) {
    return(fTDC_pos[ihit]);
  } else if (signal==3) {
    return(fTDC_neg[ihit]);
  } else {
    return(-1);			// Actually should throw an exception
  }
}

// Set the reference time
void THcRawHodoHit::SetReference(Int_t signal, Int_t reference) {
  fReferenceTime[signal] = reference;
  fHasRef[signal] = kTRUE;
}
Int_t THcRawHodoHit::GetReference(Int_t signal) {
  if(fHasRef[signal]) {
    return(fReferenceTime[signal]);
  } else {
    return(0);
  }
}

  // Do we use this?
#if 0
//_____________________________________________________________________________
THcRawHodoHit& THcRawHodoHit::operator=( const THcRawHodoHit& rhs )
{
  // Assignment operator.

  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane = rhs.fPlane;
    fCounter = rhs.fCounter;
    fADC_pos = rhs.fADC_pos;
    fADC_neg = rhs.fADC_neg;
    fTDC_pos = rhs.fTDC_pos;
    fTDC_neg = rhs.fTDC_neg;
  }
  return *this;
}
#endif


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawHodoHit)

