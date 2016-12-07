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
#include <cassert>
#include <stdexcept>

#include "THcRawHodoHit.h"

using namespace std;

void THcRawHodoHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    if (fNRawHits[0] >= fMaxNPulsesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC+!");}
    fADC_pos[fNRawHits[0]++] = data;
  } else if (signal==1) {
    if (fNRawHits[1] >= fMaxNPulsesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC-!");}
    fADC_neg[fNRawHits[1]++] = data;
  } else if(signal==2) {
    if (fNRawHits[2] >= fMaxNHitsTDC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` TDC+!");}
    fTDC_pos[fNRawHits[2]++] = data;
  } else if (signal==3) {
    if (fNRawHits[3] >= fMaxNHitsTDC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` TDC-!");}
    fTDC_neg[fNRawHits[3]++] = data;
  }
}

void THcRawHodoHit::SetDataTimePedestalPeak(Int_t signal, Int_t data, Int_t time,
					    Int_t pedestal, Int_t peak) {
  if(signal==0) {
    if (fNRawHits[0] >= fMaxNPulsesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC+!");}
    fADC_pos[fNRawHits[0]] = data;
    fADC_Time_pos[fNRawHits[0]] = time;
    fADC_Pedestal_pos[fNRawHits[0]] = pedestal;
    fADC_Peak_pos[fNRawHits[0]++] = peak;
    fHasMulti[signal]=kTRUE;
    //    cout << fPlane << "/" << fCounter << "+ " << fNRawHits[0] <<  " " <<
    //      data << "/" << time << "/" << pedestal << "/" << peak << endl;
  } else if (signal==1) {
    if (fNRawHits[1] >= fMaxNPulsesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC-!");}
    fADC_neg[fNRawHits[1]] = data;
    fADC_Time_neg[fNRawHits[1]] = time;
    fADC_Pedestal_neg[fNRawHits[1]] = pedestal;
    fADC_Peak_neg[fNRawHits[1]++] = peak;
    fHasMulti[signal]=kTRUE;
    //    cout << fPlane << "/" << fCounter << "- " << fNRawHits[0] <<  " " <<
    //      data << "/" << time << "/" << pedestal << "/" << peak << endl;
  }
}

void THcRawHodoHit::SetSample(Int_t signal, Int_t data) {
  if(signal==0) {
    if (fNRawSamplesPos >= fMaxNSamplesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC+!");}
    fADC_Samples_pos[fNRawSamplesPos++] = data;
  } else if (signal==1) {
    if (fNRawSamplesNeg >= fMaxNSamplesADC) {throw std::runtime_error("Too many samples for `THcRawHodoHit` ADC-!");}
    fADC_Samples_neg[fNRawSamplesNeg++] = data;
  }
}

Int_t THcRawHodoHit::GetIntegralPos() {
  Int_t sum=0;
  for(UInt_t i=0;i<fNRawSamplesPos;i++) {
    sum += fADC_Samples_pos[i];
  }
  return(sum);
}

Int_t THcRawHodoHit::GetIntegralNeg() {
  Int_t sum=0;
  for(UInt_t i=0;i<fNRawSamplesNeg;i++) {
    sum += fADC_Samples_neg[i];
  }
  return(sum);
}

Int_t THcRawHodoHit::GetPedestalPos() {
  if(fHasMulti[0] && fNRawHits[0]>0) {
    return(fADC_Pedestal_pos[0]);
  } else {
    return(0);
  }
}

Int_t THcRawHodoHit::GetPedestalNeg() {
  if(fHasMulti[1] && fNRawHits[1]>0) {
    return(fADC_Pedestal_neg[0]);
  } else {
    return(0);
  }
}

Int_t THcRawHodoHit::GetData(Int_t signal) {
  return(GetData(signal,0));
}
Int_t THcRawHodoHit::GetData(Int_t signal, UInt_t ihit) {
  Int_t value;
  if(ihit>= fNRawHits[signal]) {
    if(ihit==0) {
      // We were not doing this, before, so we could have been sending
      // stale data for missing tdc or adc hits
      return(0);
    }
    cout << "THcRawHodoHit::GetData(): requested hit #" << ihit << " out of range: "
	 << fNRawHits[signal] << endl;
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
    cout << "THcRawHodoHit::GetData(): requested invalid signal #"
	 << signal << endl;
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
    cout << "THcRawHodoHit::GetRawData(): requested hit #" << ihit << " out of "
	 << fNRawHits[signal] << endl;
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
    cout << "THcRawHodoHit::GetData(): requested invalid signal #"
	 << signal << endl;
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
Bool_t THcRawHodoHit::HasReference(Int_t signal) {
  return(fHasRef[signal]);
}
Bool_t THcRawHodoHit::HasMulti(Int_t signal) {
  return(fHasMulti[signal]);
}

  // Do we use this?
//_____________________________________________________________________________
THcRawHodoHit& THcRawHodoHit::operator=( const THcRawHodoHit& rhs )
{
  // Assignment operator.

  cout << "operator=" << endl;
  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    for(Int_t is=0;is<4;is++) {
      fReferenceTime[is] = rhs.fReferenceTime[is];
      fNRawHits[is] = rhs.fNRawHits[is];
      fHasRef[is] = rhs.fHasRef[is];
      fHasMulti[is] = rhs.fHasMulti[is];
    }
    for(UInt_t ih=0;ih<fNRawHits[0];ih++) {
      fADC_pos[ih] = rhs.fADC_pos[ih];
      fADC_Time_pos[ih] = rhs.fADC_Time_pos[ih];
      fADC_Pedestal_pos[ih] = rhs.fADC_Pedestal_pos[ih];
      fADC_Peak_pos[ih] = rhs.fADC_Peak_pos[ih];
    }
    for(UInt_t ih=0;ih<fNRawHits[1];ih++) {
      fADC_neg[ih] = rhs.fADC_neg[ih];
      fADC_Time_neg[ih] = rhs.fADC_Time_neg[ih];
      fADC_Pedestal_neg[ih] = rhs.fADC_Pedestal_neg[ih];
      fADC_Peak_neg[ih] = rhs.fADC_Peak_neg[ih];
    }
    for(UInt_t ih=0;ih<fNRawHits[2];ih++) {
      fTDC_pos[ih] = rhs.fTDC_pos[ih];
    }
    for(UInt_t ih=0;ih<fNRawHits[3];ih++) {
      fTDC_neg[ih] = rhs.fTDC_neg[ih];
    }
    fNRawSamplesPos = rhs.fNRawSamplesPos;
    fNRawSamplesNeg = rhs.fNRawSamplesNeg;
    for(UInt_t is=0;is<fNRawSamplesPos;is++) {
      fADC_Samples_pos[is] = rhs.fADC_Samples_pos[is];
    }
    for(UInt_t is=0;is<fNRawSamplesNeg;is++) {
      fADC_Samples_neg[is] = rhs.fADC_Samples_neg[is];
    }
    
    
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawHodoHit)
