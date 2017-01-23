/**
\class THcRawAdcHit
\ingroup DetSupport
\brief Class representing a single raw ADC hit.

It supports rich data from flash 250 ADC modules.
*/

/**
\fn THcRawAdcHit::THcRawAdcHit()
\brief Constructor.
*/

/**
\fn THcRawAdcHit& THcRawAdcHit::operator=(const THcRawAdcHit& right)
\brief Assignment operator.
\param[in] right Raw ADC hit to be assigned.
*/

/**
\fn THcRawAdcHit::~THcRawAdcHit()
\brief Destructor.
*/

/**
\fn void THcRawAdcHit::Clear(Option_t* opt="")
\brief Clears variables before next event.
\param[in] opt Maybe used in base clas... Not sure.
*/

/**
\fn void THcRawAdcHit::SetData(Int_t data)
\brief Sets raw ADC value.
\param[in] data Raw ADC value. In channels.
\throw std::out_of_range Tried to set too many pulses.

Should be used for old style ADCs.
*/

/**
\fn void THcRawAdcHit::SetSample(Int_t data)
\brief Sets raw signal sample.
\param[in] data Raw signal sample. In channels.
\throw std::out_of_range Tried to set too many samples.
*/

/**
\fn void THcRawAdcHit::SetDataTimePedestalPeak(Int_t data, Int_t time, Int_t pedestal, Int_t peak)
\brief Sets various bits of ADC data.
\param[in] data Raw pulse integral. In channels.
\param[in] time Raw pulse time. In subsamples.
\param[in] pedestal Raw signal pedestal. In channels.
\param[in] peak Raw pulse amplitude. In channels.
\throw std::out_of_range Tried to set too many pulses.

Should be used for flash 250 modules.
*/

/**
\fn Int_t THcRawAdcHit::GetRawData(UInt_t iPulse=0) const
\brief Gets raw pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
*/

/**
\fn Int_t THcRawAdcHit::GetAdcTime(UInt_t iPulse=0) const
\brief Gets raw pulse time. In subsamples.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
Returns 0 if no pulse time is set.
*/

/**
\fn Int_t THcRawAdcHit::GetAdcPedestal(UInt_t iPulse=0) const
\brief Gets raw signal pedestal. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
Returns 0 if no signal pedestal is set.
*/

/**
\fn Int_t THcRawAdcHit::GetAdcPulse(UInt_t iPulse=0) const
\brief Gets raw pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
Returns 0 if no pulse peak is set.
*/

/**
\fn Int_t THcRawAdcHit::GetSample(UInt_t iSample) const
\brief Gets raw signal sample. In channels.
\param[in] iSample Sequential number of requested sample.
\throw std::out_of_range Tried to get nonexisting sample.

Returns 0 if tried to access first sample but no samples are set.
*/

/**
\fn Double_t THcRawAdcHit::GetAverage(UInt_t iSampleLow, UInt_t iSampleHigh) const
\brief Gets average of raw samples. In channels.
\param[in] iSampleLow Sequential number of first sample to be averaged.
\param[in] iSampleHigh Sequential number of last sample to be averaged.
\throw std::out_of_range Tried to average over nonexisting sample.
*/

/**
\fn Int_t THcRawAdcHit::GetIntegral(UInt_t iSampleLow, UInt_t iSampleHigh) const
\brief Gets integral of raw samples. In channels.
\param[in] iSampleLow Sequential number of first sample to be integrated.
\param[in] iSampleHigh Sequential number of last sample to be integrated.
\throw std::out_of_range Tried to integrate over nonexisting sample.
*/

/**
\fn Double_t THcRawAdcHit::GetData(UInt_t iPedLow, UInt_t iPedHigh, UInt_t iIntLow, UInt_t iIntHigh) const
\brief Gets pedestal subtracted integral of samples. In channels.
\param[in] iPedLow Sequential number of first sample to be averaged for pedestal value.
\param[in] iPedHigh Sequential number of last sample to be averaged for pedestal value.
\param[in] iIntLow Sequential number of first sample to be integrated.
\param[in] iIntHigh Sequential number of last sample to be integrated.
*/

/**
\fn UInt_t THcRawAdcHit::GetNPulses() const
\brief Gets number of set pulses.
*/

/**
\fn UInt_t THcRawAdcHit::GetNSamples() const
\brief Gets number of set samples.
*/

/**
\fn Bool_t THcRawAdcHit::HasMulti() const
\brief Queries whether data is from flash 250 module.
*/

/**
\fn Int_t THcRawAdcHit::GetPedRaw() const
\brief Gets raw signal pedestal. In channels.

Returns 0 if no signal pedestal is set.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseIntRaw(UInt_t iPulse=0) const
\brief Gets raw pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.

Check iPulse validity before calling!
*/

/**
\fn Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse=0) const
\brief Gets raw pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.

Check iPulse validity before calling!
*/

/**
\fn Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse=0) const
\brief Gets raw pulse time. In subsamples.
\param[in] iPulse Sequential number of requested pulse.

Check iPulse validity before calling!
*/

/**
\fn Double_t THcRawAdcHit::GetPed() const
\brief Gets sample pedestal. In channels.

Returns 0 if no signal pedestal is set.
*/

/**
\fn Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.

Check iPulse validity before calling!
*/

/**
\fn Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.

Check iPulse validity before calling!
*/

/**
\fn Int_t THcRawAdcHit::GetSampleIntRaw() const
\brief Gets raw integral of samples. In channels.
*/

/**
\fn Double_t THcRawAdcHit::GetSampleInt() const
\brief Gets pedestal subtracted integral of samples. In channels.
*/

// TODO: Disallow using both SetData and SetDataTimePedestalPeak.
// TODO: Add checks to new getters.
// TODO: Deprecate and remove old getters.


#include "THcRawAdcHit.h"

#include <stdexcept>

#include "TString.h"


THcRawAdcHit::THcRawAdcHit() :
  TObject(),
  fNPedestalSamples(4), fNPeakSamples(9),
  fPeakPedestalRatio(1.0*fNPeakSamples/fNPedestalSamples),
  fChannelToTimeFactor(0.0625),
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


Int_t THcRawAdcHit::GetRawData(UInt_t iPulse) const {
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


Int_t THcRawAdcHit::GetAdcTime(UInt_t iPulse) const {
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


Int_t THcRawAdcHit::GetAdcPedestal(UInt_t iPulse) const {
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


Int_t THcRawAdcHit::GetAdcPulse(UInt_t iPulse) const {
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


Int_t THcRawAdcHit::GetSample(UInt_t iSample) const {
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


Double_t THcRawAdcHit::GetAverage(UInt_t iSampleLow, UInt_t iSampleHigh) const {
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


Int_t THcRawAdcHit::GetIntegral(UInt_t iSampleLow, UInt_t iSampleHigh) const {
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
) const {
  return
    GetIntegral(iIntLow, iIntHigh)
    - GetAverage(iPedHigh, iPedLow) * (iIntHigh - iIntLow + 1);
}


UInt_t THcRawAdcHit::GetNPulses() const {
  return fNPulses;
}


UInt_t THcRawAdcHit::GetNSamples() const {
  return fNSamples;
}


Bool_t THcRawAdcHit::HasMulti() const {
  return fHasMulti;
}


Int_t THcRawAdcHit::GetPedRaw() const {
  return fAdcPedestal[0];
}


Int_t THcRawAdcHit::GetPulseIntRaw(UInt_t iPulse) const {
  return fAdc[iPulse];
}


Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse) const {
  return fAdcPulse[iPulse];
}


Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse) const {
  return fAdcTime[iPulse];
}


Double_t THcRawAdcHit::GetPed() const {
  return static_cast<Double_t>(fAdcPedestal[0])/static_cast<Double_t>(fNPedestalSamples);
}


Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse) const {
  return static_cast<Double_t>(fAdc[iPulse]) - static_cast<Double_t>(fAdcPedestal[0])*fPeakPedestalRatio;
}


Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse) const {
  return static_cast<Double_t>(fAdcPulse[iPulse]) - static_cast<Double_t>(fAdcPedestal[0])/static_cast<Double_t>(fNPedestalSamples);
}


//Int_t THcRawAdcHit::GetPulseTime(UInt_t iPulse) const {
//  return static_cast<Double_t>(fAdcTime[iPulse]);
//}


Int_t THcRawAdcHit::GetSampleIntRaw() const {
  Int_t integral = 0;

  for (UInt_t iSample=0; iSample<fNSamples; ++iSample) {
    integral += fAdcSample[iSample];
  }

  return integral;
}


Double_t THcRawAdcHit::GetSampleInt() const {
  return static_cast<Double_t>(GetSampleIntRaw()) - GetPed()*static_cast<Double_t>(fNSamples);
}


ClassImp(THcRawAdcHit)
