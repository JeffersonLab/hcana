/**
\class THcRawAdcHit
\ingroup DetSupport

\brief Class representing a single raw ADC hit.

It supports rich data from flash 250 ADC modules.
*/

#include "THcRawAdcHit.h"

#include <stdexcept>

#include "TString.h"


THcRawAdcHit::THcRawAdcHit() :
  TObject(),
  fNPedestalSamples(4), fNPeakSamples(10),
  fPeakPedestalRatio(1.0*fNPeakSamples/fNPedestalSamples),
  fAdc(), fAdcTime(), fAdcPedestal(), fAdcPulse(), fAdcSample(),
  fHasMulti(kFALSE), fNPulses(0), fNSamples(0)
{}


THcRawAdcHit& THcRawAdcHit::operator=(const THcRawAdcHit& right) {
  TObject::operator=(right);

  if (this != &right) {
    for (UInt_t i=0; i<fMaxNPulses; ++i) {
      fAdc[i] = right.fAdc[i];
      fAdcTime[i] = right.fAdcTime[i];
      fAdcPedestal[i] = right.fAdcPedestal[i];
      fAdcPulse[i] = right.fAdcPulse[i];
    }
    for (UInt_t i=0; i<fMaxNSamples; ++i) {
      fAdcSample[i] = right.fAdcSample[i];
    }
    fHasMulti = right.fHasMulti;
    fNPulses = right.fNPulses;
    fNSamples = right.fNSamples;
  }

  return *this;
}


THcRawAdcHit::~THcRawAdcHit() {}


void THcRawAdcHit::Clear(Option_t* opt) {
  TObject::Clear(opt);

  for (UInt_t i=0; i<fNPulses; ++i) {
    fAdc[i] = 0;
    fAdcTime[i] = 0;
    fAdcPedestal[i] = 0;
    fAdcPulse[i] = 0;
  }
  for (UInt_t i=0; i<fNSamples; ++i) {
    fAdcSample[i] = 0 ;
  }
  fHasMulti = kFALSE;
  fNPulses = 0;
  fNSamples = 0;
}


void THcRawAdcHit::SetData(Int_t data) {
  if (fNPulses >= fMaxNPulses) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetData`: too many pulses!"
    );
  }
  fAdc[fNPulses] = data;
  ++fNPulses;
}


void THcRawAdcHit::SetSample(Int_t data) {
  if (fNSamples >= fMaxNSamples) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetSample`: too many samples!"
    );
  }
  fAdcSample[fNSamples] = data;
  ++fNSamples;
}


void THcRawAdcHit::SetDataTimePedestalPeak(
  Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (fNPulses >= fMaxNPulses) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetData`: too many pulses!"
    );
  }
  fAdc[fNPulses] = data;
  fAdcTime[fNPulses] = time;
  fAdcPedestal[fNPulses] = pedestal;
  fAdcPulse[fNPulses] = peak;
  fHasMulti = kTRUE;
  ++fNPulses;
}


Int_t THcRawAdcHit::GetRawData(UInt_t iPulse) {
  if (iPulse >= fNPulses && iPulse != 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetRawData`: requested pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iPulse >= fNPulses && iPulse == 0) {
    return 0;
  }
  else {
    return fAdc[iPulse];
  }
}


Int_t THcRawAdcHit::GetAdcTime(UInt_t iPulse) {
  if (iPulse >= fNPulses && iPulse != 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetAdcTime`: requested pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
  else if (fHasMulti) {
    return fAdcTime[iPulse];
  }
  else {
    return 0;
  }
}


Int_t THcRawAdcHit::GetAdcPedestal(UInt_t iPulse) {
  if (iPulse >= fNPulses && iPulse != 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetAdcPedestal`: requested pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
  else if (fHasMulti) {
    return fAdcPedestal[iPulse];
  }
  else {
    return 0;
  }
}


Int_t THcRawAdcHit::GetAdcPulse(UInt_t iPulse) {
  if (iPulse >= fNPulses && iPulse != 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetAdcPulse`: requested pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
  else if (fHasMulti) {
    return fAdcPulse[iPulse];
  }
  else {
    return 0;
  }
}


Int_t THcRawAdcHit::GetSample(UInt_t iSample) {
  if (iSample >= fNSamples && iSample != 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetSample`: requested sample %d where only %d sample available!",
      iSample, fNSamples
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iSample >= fNSamples && iSample == 0) {
    return 0;
  }
  else {
    return fAdcSample[iSample];
  }
}


Double_t THcRawAdcHit::GetAverage(UInt_t iSampleLow, UInt_t iSampleHigh) {
  if (iSampleHigh >= fNSamples || iSampleLow >= fNSamples) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetAverage`: not this many samples available!"
    );
    throw std::out_of_range(msg.Data());
  }
  else {
    Double_t average = 0.0;
    for (UInt_t i=iSampleLow; i<=iSampleHigh; ++i) {
      average += fAdcSample[i];
    }
    return average / (iSampleHigh - iSampleLow + 1);
  }
}


Int_t THcRawAdcHit::GetIntegral(UInt_t iSampleLow, UInt_t iSampleHigh) {
  if (iSampleHigh >= fNSamples || iSampleLow >= fNSamples) {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetAverage`: not this many samples available!"
    );
    throw std::out_of_range(msg.Data());
  }
  else {
    Int_t integral = 0;
    for (UInt_t i=iSampleLow; i<=iSampleHigh; ++i) {
      integral += fAdcSample[i];
    }
    return integral;
  }
}


Double_t THcRawAdcHit::GetData(
  UInt_t iPedLow, UInt_t iPedHigh, UInt_t iIntLow, UInt_t iIntHigh
) {
  return
    GetIntegral(iIntLow, iIntHigh)
    - GetAverage(iPedHigh, iPedLow) * (iIntHigh - iIntLow + 1);
}


UInt_t THcRawAdcHit::GetNPulses() {
  return fNPulses;
}


UInt_t THcRawAdcHit::GetNSamples() {
  return fNSamples;
}


Bool_t THcRawAdcHit::HasMulti() {
  return fHasMulti;
}


Int_t THcRawAdcHit::GetPedRaw() {
  return fAdcPedestal[0];
}


Int_t THcRawAdcHit::GetPulseIntRaw(UInt_t iPulse) {
  return fAdc[iPulse];
}


Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse) {
  return fAdcPulse[iPulse];
}


Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse) {
  return fAdcTime[iPulse];
}


Double_t THcRawAdcHit::GetPed() {
  return 1.0 * fAdcPedestal[0]/fNPedestalSamples;
}


Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse) {
  return fAdc[iPulse] - fAdcPedestal[0] * fPeakPedestalRatio;
}


Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse) {
  return fAdcPulse[iPulse] - 1.0 * fAdcPedestal[0]/fNPedestalSamples;
}


Int_t THcRawAdcHit::GetSampleIntRaw() {
  Int_t integral = 0;

  for (UInt_t iSample=0; iSample<fNSamples; ++iSample) {
    integral += fAdcSample[iSample];
  }

  return integral;
}


Double_t THcRawAdcHit::GetSampleInt() {
  return GetSampleIntRaw() - GetPed()*fNSamples;
}


ClassImp(THcRawAdcHit)
