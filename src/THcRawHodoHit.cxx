/**
\class THcRawHodoHit
\ingroup DetSupport

\brief Class representing a single raw hit for a hodoscope paddle.

  - `signal 0` is ADC pos
  - `signal 1` is ADC neg
  - `signal 2` is TDC pos
  - `signal 3` is TDC neg
*/

#include "THcRawHodoHit.h"

#include <iostream>
#include <stdexcept>


THcRawHodoHit::THcRawHodoHit(Int_t plane, Int_t counter) :
  THcRawHit(plane, counter), fAdcHits(), fTdcHits()
{}


THcRawHodoHit& THcRawHodoHit::operator=(const THcRawHodoHit& right) {
  THcRawHit::operator=(right);

  if (this != &right) {
    for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
      fAdcHits[iAdcSig] = right.fAdcHits[iAdcSig];
    }
    for (Int_t iTdcSig=0; iTdcSig<fNTdcSignals; ++iTdcSig) {
      fTdcHits[iTdcSig] = right.fTdcHits[iTdcSig];
    }
  }

  return *this;
}


THcRawHodoHit::~THcRawHodoHit() {}


void THcRawHodoHit::Clear(Option_t* opt) {
  THcRawHit::Clear(opt);

  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].Clear();
  }
  for (Int_t iTdcSig=0; iTdcSig<fNTdcSignals; ++iTdcSig) {
    fTdcHits[iTdcSig].Clear();
  }
}


void THcRawHodoHit::SetData(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetData(data);
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    fTdcHits[signal-fNAdcSignals].SetTime(data);
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetData`: only signals `0` to `3` available!"
    );
  }
}


void THcRawHodoHit::SetSample(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetSample(data);
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetSample`: only signals `0` and `1` available!"
    );
  }
}

void THcRawHodoHit::SetSampThreshold(Int_t signal, Double_t thres) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetSampThreshold(thres);
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetSampThreshold`: only signals `0` and `1` available!"
    );
  }
}


void THcRawHodoHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetDataTimePedestalPeak(data, time, pedestal, peak);
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetDataTimePedestalPeak`: only signals `0` and `1` available!"
    );
  }
}

void THcRawHodoHit::SetSampIntTimePedestalPeak(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetSampIntTimePedestalPeak();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetDataTimePedestalPeak`: only signals `0` and `1` available!"
    );
  }
}


void THcRawHodoHit::SetReference(Int_t signal, Int_t reference) {
  if (signal < fNAdcSignals) {
    fAdcHits[signal].SetRefTime(reference);
  } else if (signal < fNAdcSignals+fNTdcSignals) {
    fTdcHits[signal-fNAdcSignals].SetRefTime(reference);
  } else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetReference`: only signals `2` and `3` available!"
    );
  }
}

void THcRawHodoHit::SetReferenceDiff(Int_t signal, Int_t referenceDiff) {
  if (signal < fNAdcSignals) {
    fAdcHits[signal].SetRefDiffTime(referenceDiff);
  } else if (signal < fNAdcSignals+fNTdcSignals) {
    fTdcHits[signal-fNAdcSignals].SetRefDiffTime(referenceDiff);
  } else {
    throw std::out_of_range(
      "`THcRawHodoHit::SetReference`: only signals `2` and `3` available!"
    );
  }
}



Int_t THcRawHodoHit::GetData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetPulseInt();
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::GetData`: only signals `0` to `3` available!"
    );
  }
}


Int_t THcRawHodoHit::GetRawData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetPulseIntRaw();
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetTimeRaw();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::GetRawData`: only signals `0` to `3` available!"
    );
  }
}


Int_t THcRawHodoHit::GetReference(Int_t signal) {
  if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::GetReference`: only signals `2` and `3` available!"
    );
  }
}

Int_t THcRawHodoHit::GetReferenceDiff(Int_t signal) {
  if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetRefDiffTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::GetReference`: only signals `2` and `3` available!"
    );
  }
}


THcRawHit::ESignalType THcRawHodoHit::GetSignalType(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return kADC;
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return kTDC;
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::GetSignalType`: only signals `0` to `3` available!"
    );
  }
}


Int_t THcRawHodoHit::GetNSignals() {
  return fNAdcSignals + fNTdcSignals;
}

Bool_t THcRawHodoHit::HasReference(Int_t signal) {
  if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].HasRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawHodoHit::HasReference`: only signals `2` and `3` available!"
    );
  }
}


THcRawAdcHit& THcRawHodoHit::GetRawAdcHitPos() {
  return fAdcHits[0];
}


THcRawAdcHit& THcRawHodoHit::GetRawAdcHitNeg() {
  return fAdcHits[1];
}


THcRawTdcHit& THcRawHodoHit::GetRawTdcHitPos() {
  return fTdcHits[0];
}


THcRawTdcHit& THcRawHodoHit::GetRawTdcHitNeg() {
  return fTdcHits[1];
}


void THcRawHodoHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED) {
  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].SetF250Params(NSA, NSB, NPED);
  }
}


ClassImp(THcRawHodoHit)
