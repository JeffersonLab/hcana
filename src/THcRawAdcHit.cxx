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
\fn void THcRawAdcHit::SetRefTime(Int_t refTime)
\brief Sets reference time. In channels.
\param[in] refTime Reference time. In channels.
*/

/**
\fn Int_t THcRawAdcHit::GetRawData(UInt_t iPulse=0) const
\brief Gets raw pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
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
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse=0) const
\brief Gets raw pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse=0) const
\brief Gets raw pulse time. In subsamples.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetSampleRaw(UInt_t iSample=0) const
\brief Gets raw sample. In channels.
\param[in] iSample Sequential number of requested sample.
\throw std::out_of_range Tried to get nonexisting sample.
*/

/**
\fn Double_t THcRawAdcHit::GetPed() const
\brief Gets sample pedestal. In channels.
*/

/**
\fn Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
*/

/**
\fn Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetSampleIntRaw() const
\brief Gets raw integral of sTimeFacamples. In channels.
*/

/**
\fn Double_t THcRawAdcHit::GetSampleInt() const
\brief Gets pedestal subtracted integral of samples. In channels.
*/

/**
\fn void THcRawAdcHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED)
\brief Sets F250 parameters used for pedestal subtraction.
\param [in] NSA NSA parameter of F250 modules.
\param [in] NSB NSB parameter of F250 modules.
\param [in] NPED NPED parameter of F250 modules.
*/

// TODO: Disallow using both SetData and SetDataTimePedestalPeak.


#include "THcRawAdcHit.h"
#include <stdexcept>
#include "TString.h"
    const Double_t THcRawAdcHit::fNAdcChan      = 4096.0; // Number of FADC channels in units of ADC channels
    const Double_t THcRawAdcHit::fAdcRange      = 1.0;    // Dynamic range of FADCs in units of V, // TO-DO: Get fAdcRange from pre-start event
    const Double_t THcRawAdcHit::fAdcImpedence  = 50.0;   // FADC input impedence in units of Ohms
    const Double_t THcRawAdcHit::fAdcTimeSample = 4000.0;    // Length of FADC time sample in units of ps
    const Double_t THcRawAdcHit::fAdcTimeRes    = 0.0625; // FADC time resolution in units of ns

THcRawAdcHit::THcRawAdcHit() :
  TObject(),
  fNPedestalSamples(4), fNPeakSamples(9),
  fPeakPedestalRatio(1.0*fNPeakSamples/fNPedestalSamples),
  fSubsampleToTimeFactor(0.0625),
  fPed(0), fPulseInt(), fPulseAmp(), fPulseTime(), fSample(),
  fRefTime(0), fHasMulti(kFALSE), fHasRefTime(kFALSE), fNPulses(0), fNSamples(0)
{}

THcRawAdcHit& THcRawAdcHit::operator=(const THcRawAdcHit& right) {
  TObject::operator=(right);

  if (this != &right) {
    fPed = right.fPed;
    for (UInt_t i=0; i<fMaxNPulses; ++i) {
      fPulseInt[i]  = right.fPulseInt[i];
      fPulseAmp[i]  = right.fPulseAmp[i];
      fPulseTime[i] = right.fPulseTime[i];
    }
    for (UInt_t i=0; i<fMaxNSamples; ++i) {
      fSample[i] = right.fSample[i];
    }
    fHasMulti = right.fHasMulti;
    fNPulses  = right.fNPulses;
    fNSamples = right.fNSamples;
    fRefTime = right.fRefTime;
    fHasRefTime = right.fHasRefTime;
  }

  return *this;
}

THcRawAdcHit::~THcRawAdcHit() {}

void THcRawAdcHit::Clear(Option_t* opt) {
  TObject::Clear(opt);

  fPed = 0;
  for (UInt_t i=0; i<fNPulses; ++i) {
    fPulseInt[i] = 0;
    fPulseAmp[i] = 0;
    fPulseTime[i] = 0;
  }
  for (UInt_t i=0; i<fNSamples; ++i) {
    fSample[i] = 0 ;
  }
  fHasMulti = kFALSE;
  fNPulses = 0;
  fNSamples = 0;
  fRefTime = 0;
  fHasRefTime = kFALSE;
}

void THcRawAdcHit::SetData(Int_t data) {
  if (fNPulses >= fMaxNPulses) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetData`: too many pulses!"
    );
  }
  fPulseInt[fNPulses] = data;
  ++fNPulses;
}

void THcRawAdcHit::SetRefTime(Int_t refTime) {
  fRefTime = refTime;
  fHasRefTime = kTRUE;
}

void THcRawAdcHit::SetRefDiffTime(Int_t refDiffTime) {
  fRefDiffTime = refDiffTime;
}

void THcRawAdcHit::SetSample(Int_t data) {
  if (fNSamples >= fMaxNSamples) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetSample`: too many samples!"
    );
  }
  fSample[fNSamples] = data;
  ++fNSamples;
}

void THcRawAdcHit::SetDataTimePedestalPeak(
  Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (fNPulses >= fMaxNPulses) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetDataTimePedestalPeak`: too many pulses!"
    );
  }
  fPulseInt[fNPulses] = data;
  fPulseTime[fNPulses] = time;
  fPed = pedestal;
  fPulseAmp[fNPulses] = peak;
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
    return fPulseInt[iPulse];
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
      average += fSample[i];
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
      integral += fSample[i];
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
  return fPed;
}

Int_t THcRawAdcHit::GetPulseIntRaw(UInt_t iPulse) const {
  if (iPulse < fNPulses) {
    return fPulseInt[iPulse];
  }
  else if (iPulse == 0) {
    return 0;
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetPulseIntRaw`: Trying to get pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
}

Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse) const {
  if (iPulse < fNPulses) {
    return fPulseAmp[iPulse];
  }
  else if (iPulse == 0) {
    return 0;
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetPulseAmpRaw`: Trying to get pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
}

Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse) const {
  if (iPulse < fNPulses) {
    return fPulseTime[iPulse];
  }
  else if (iPulse == 0) {
    return 0;
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetPulseTimeRaw`: Trying to get pulse %d where only %d pulses available!",
      iPulse, fNPulses
    );
    throw std::out_of_range(msg.Data());
  }
}

Int_t THcRawAdcHit::GetSampleRaw(UInt_t iSample) const {
  if (iSample < fNSamples) {
    return fSample[iSample];
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetSampleRaw`: Trying to get sample %d where only %d samples available!",
      iSample, fNSamples
    );
    throw std::out_of_range(msg.Data());
  }
}

Double_t THcRawAdcHit::GetPed() const {
  return (static_cast<Double_t>(fPed)/static_cast<Double_t>(fNPedestalSamples))*GetAdcTomV();
}

Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse) const {
  return (static_cast<Double_t>(fPulseInt[iPulse]) - static_cast<Double_t>(fPed)*fPeakPedestalRatio)*GetAdcTopC();
}

Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse) const {
  return (static_cast<Double_t>(fPulseAmp[iPulse]) - static_cast<Double_t>(fPed)/static_cast<Double_t>(fNPedestalSamples))*GetAdcTomV();
}

Double_t THcRawAdcHit::GetPulseTime(UInt_t iPulse) const {
  Int_t rawtime = fPulseTime[iPulse];
  if (fHasRefTime) {
    rawtime -= fRefTime;
  }
  return (static_cast<Double_t>(rawtime)*GetAdcTons());
}

Int_t THcRawAdcHit::GetSampleIntRaw() const {
  Int_t integral = 0;

  for (UInt_t iSample=0; iSample<fNSamples; ++iSample) {
    integral += fSample[iSample];
  }

  return integral;
}

Double_t THcRawAdcHit::GetSampleInt() const {
  return static_cast<Double_t>(GetSampleIntRaw()) - GetPed()*static_cast<Double_t>(fNSamples);
}

void THcRawAdcHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED) {
  if (NSA < 0 || NSB < 0 || NPED < 0) {
    TString msg = TString::Format(
      "`THcRawAdcHit::SetF250Params`: One of the params is negative!  NSA = %d  NSB = %d  NPED = %d",
      NSA, NSB, NPED
    );
    throw std::invalid_argument(msg.Data());
  }
  fNPedestalSamples = NPED;
  fNPeakSamples = NSA + NSB;
  fPeakPedestalRatio = 1.0*fNPeakSamples/fNPedestalSamples;
}

// FADC conversion factors
// Convert pedestal and amplitude to mV
Double_t THcRawAdcHit::GetAdcTomV() const {
  // 1000 mV / 4096 ADC channels
  return (fAdcRange*1000. / fNAdcChan);
}

// Convert integral to pC
Double_t THcRawAdcHit::GetAdcTopC() const {
  // (1 V / 4096 adc channels) * (4000 ps time sample / 50 ohms input resistance) = 0.020 pc/channel 
  return (fAdcRange / fNAdcChan) * (fAdcTimeSample / fAdcImpedence);
}

// Convert time sub samples to ns
Double_t THcRawAdcHit::GetAdcTons() const {
  return fAdcTimeRes;
}

Int_t THcRawAdcHit::GetRefTime() const {
  if (fHasRefTime) {
    return fRefTime;
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetRefTime`: Reference time not available!"
    );
    throw std::runtime_error(msg.Data());
  }
}

Int_t THcRawAdcHit::GetRefDiffTime() const {
  if (fHasRefTime) {
    return fRefDiffTime;
  }
  else {
    TString msg = TString::Format(
      "`THcRawAdcHit::GetRefTime`: Reference time not available!"
    );
    throw std::runtime_error(msg.Data());
  }
}

Bool_t THcRawAdcHit::HasRefTime() const {
  return fHasRefTime;
}

ClassImp(THcRawAdcHit)
