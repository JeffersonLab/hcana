/**
\class THcRawShowerHit
\ingroup DetSupport

\brief Class representing a single raw hit for a shower paddle.

  - `signal 0` is ADC pos
  - `signal 1` is ADC neg
*/

#include "THcRawShowerHit.h"

#include <iostream>
#include <stdexcept>


THcRawShowerHit::THcRawShowerHit(Int_t plane, Int_t counter) :
  fAdcHits()
{}


THcRawShowerHit& THcRawShowerHit::operator=(const THcRawShowerHit& right) {
  THcRawHit::operator=(right);

  if (this != &right) {
    for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
      fAdcHits[iAdcSig] = right.fAdcHits[iAdcSig];
    }
  }

  return *this;
}


THcRawShowerHit::~THcRawShowerHit() {}


void THcRawShowerHit::Clear(Option_t* opt) {
  THcRawHit::Clear(opt);

  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].Clear();
  }
}


void THcRawShowerHit::SetData(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetData(data);
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::SetData`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetSample(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetSample(data);
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::SetSample`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetDataTimePedestalPeak(data, time, pedestal, peak);
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::SetDataTimePedestalPeak`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetReference(Int_t signal, Int_t reference) {
  if (signal < fNAdcSignals) {
    fAdcHits[signal].SetRefTime(reference);
  } else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetReference`: only signals `2` and `3` available!"
    );
  }
}

void THcRawShowerHit::SetReferenceDiff(Int_t signal, Int_t reference) {
  if (signal < fNAdcSignals) {
    fAdcHits[signal].SetRefDiffTime(reference);
  } else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetReference`: only signals `2` and `3` available!"
    );
  }
}


Int_t THcRawShowerHit::GetData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetRawData();
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcRawShowerHit::GetRawData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetRawData();
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::GetRawData`: only signals `0` and `1` available!"
    );
  }
}


THcRawHit::ESignalType THcRawShowerHit::GetSignalType(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return kADC;
  }
  else {
    throw std::out_of_range(
      "`THcRawShowerHit::GetSignalType`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcRawShowerHit::GetNSignals() {
  return fNAdcSignals;
}


THcRawAdcHit& THcRawShowerHit::GetRawAdcHitPos() {
  return fAdcHits[0];
}


THcRawAdcHit& THcRawShowerHit::GetRawAdcHitNeg() {
  return fAdcHits[1];
}


void THcRawShowerHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED) {
  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].SetF250Params(NSA, NSB, NPED);
  }
}


ClassImp(THcRawShowerHit)
