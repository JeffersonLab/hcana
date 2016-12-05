/** \class THcRawShowerHit
    \ingroup DetSupport

 Class representing a single raw hit for a hodoscope paddle                

 Contains plane, counter and pos/neg adc and tdc values                    
                                                                           
 Enhanced to work with FADC250 data samples.  If fNPosSamples/fNNegSamples 
 is greater than 1, assume that the data held in the hit is the sampled    
 waveform.  Signals 0,1 will return the integrated pulse with dynamic      
 pedestal subtraction (first four samples comprise the pedestal).  Signals 
 2,3 are reserved for time information.  Signals 4,5 are pedestals and     
 6 and 7 are the straight sum of all the samples.                          

*/

#include "THcRawShowerHit.h"
#include <iostream>
#include <cassert>

using namespace std;


void THcRawShowerHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos[fNPosSamples++] = data;
  } else if (signal==1) {
    fADC_neg[fNNegSamples++] = data;
  } 
}

Int_t THcRawShowerHit::GetData(Int_t signal, Int_t isamplow, Int_t isamphigh,
	      Int_t iintegrallow, Int_t iintegralhigh) {
  Int_t adcsum=0;
  Double_t pedestal=0.0;

  if(signal==0 || signal == 1) {
    pedestal = GetPedestal(signal, isamplow, isamphigh);

#if 0
    for(Int_t i=0;i<fNPosSamples;i++) {
      cout << fCounter << " " << i;
      if(i >= isamplow && i<=isamphigh) {
	cout << "P ";
      }	else if (i >= iintegrallow && i<=iintegralhigh) {
	cout << "D ";
      } else {
	cout << "  ";
      }
      cout << fADC_pos[i] << " " << fADC_pos[i] - pedestal << endl;
    }
#endif
  }
  if(signal==4 || signal==5) {
    pedestal = GetPedestal(signal-4, isamplow, isamphigh);
    return(pedestal);
  }
  if(signal==0 || signal==6) {
    assert(iintegralhigh<(Int_t) fNPosSamples);
    for(UInt_t isample=iintegrallow;isample<=iintegralhigh;isample++) {
      adcsum += fADC_pos[isample] - pedestal;
    }
    return(adcsum);
  } else if (signal==1 || signal==7) {
    assert(iintegralhigh<(Int_t) fNNegSamples);
    for(UInt_t isample=iintegrallow;isample<=iintegralhigh;isample++) {
      adcsum += fADC_neg[isample] - pedestal;
    }
    return(adcsum);
  } 
  return(-1); // Actually should throw exception
}
  
// Return sum of samples
// For Fastbus ADC, this will simply be the hardware ADC value as there
// is just one sample.  For Flash ADCs, this should return the
// integrated ADC value if the FADC provided that, otherwise the sum
// of all the samples
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
Int_t THcRawShowerHit::GetSample(Int_t signal, UInt_t isample) {
  if(signal==0) {
    if(isample < fNPosSamples) {
      return(fADC_pos[isample]);
    }
  } else if (signal==1) {
    if(isample < fNNegSamples) {
      return(fADC_neg[isample]);
    }
  }
  return(-1);
}

Double_t THcRawShowerHit::GetPedestal(Int_t signal, Int_t isamplow, Int_t isamphigh) {
  // No error checking on pedestal range
  Double_t pedestal=0.0;
  if(signal==0 && fNPosSamples > 1) {
    if(fNPosSamples > isamphigh) {
      for(Int_t i = isamplow;i<=isamphigh;i++) {
	pedestal += fADC_pos[i];
      }
      return(pedestal/(isamphigh-isamplow+1));
    }
  } else if (signal==1 && fNNegSamples > 1) {
    if(fNNegSamples > isamphigh) {
      for(Int_t i = isamplow;i<=isamphigh;i++) {
	pedestal += fADC_neg[i];
      }
      return(pedestal/(isamphigh-isamplow+1));
    }
  }
  return(pedestal);
}
      
// Return the number of samples
Int_t THcRawShowerHit::GetNSamples(Int_t signal) {
  if(signal==0) {
    return(fNPosSamples);
  } else if (signal==1) {
    return(fNNegSamples);
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

 
