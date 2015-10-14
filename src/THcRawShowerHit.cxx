///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawShowerHit                                                           //
//                                                                           //
// Class representing a single raw hit for a hodoscope paddle                //
//                                                                           //
// Contains plane, counter and pos/neg adc and tdc values                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawShowerHit.h"
#include <iostream>

using namespace std;


void THcRawShowerHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos[fNPosSamples++] = data;
  } else if (signal==1) {
    fADC_neg[fNNegSamples++] = data;
  } 
}

// Return sum of samples
Int_t THcRawShowerHit::GetData(Int_t signal) {
  Int_t adcsum=0;
  if(signal==0) {
    for(UInt_t isample=0;isample<fNPosSamples;isample++) {
      adcsum += fADC_pos[isample];
    }
    return(adcsum);
  } else if (signal==1) {
    for(UInt_t isample=0;isample<fNNegSamples;isample++) {
      adcsum += fADC_neg[isample];
    }
    return(adcsum);
  } 
  return(-1); // Actually should throw exception
}

// Return a requested sample
Int_t THcRawShowerHit::GetData(Int_t signal, UInt_t isample) {
  if(signal==0) {
    if(isample >=0 && isample< fNPosSamples) {
      return(fADC_pos[isample]);
    }
  } else if (signal==1) {
    if(isample >=0 && isample< fNNegSamples) {
      return(fADC_neg[isample]);
    }
  }
  return(-1);
}
//_____________________________________________________________________________
THcRawShowerHit& THcRawShowerHit::operator=( const THcRawShowerHit& rhs )
{
  // Assignment operator.

  cout << "YES THIS HAPPENS" << endl;
  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane = rhs.fPlane;
    fCounter = rhs.fCounter;
    for(UInt_t isample=0;isample<fNPosSamples;isample++) {
      fADC_pos[isample] = rhs.fADC_pos[isample];
    }
    for(UInt_t isample=0;isample<fNNegSamples;isample++) {
      fADC_pos[isample] = rhs.fADC_pos[isample];
    }
    fNPosSamples = rhs.fNPosSamples;
    fNNegSamples = rhs.fNNegSamples;
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawShowerHit)

 
