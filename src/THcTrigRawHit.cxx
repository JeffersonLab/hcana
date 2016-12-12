/**
\class THcTrigRawHit
\ingroup DetSupport

\brief Class representing a single raw hit for the THcTrigDet.

  - `signal 0` is ADC
  - `signal 1` is TDC

It supports rich data from flash 250 ADC modules.
*/

/**
\fn THcTrigRawHit::THcTrigRawHit(Int_t plane=0, Int_t counter=0)

\brief A constructor.

\param[in] plane Internal plane number of the hit. Should be `1` for ADC and
  `2` for TDC.
\param[in] counter Internal bar number of the hit.
*/

/**
\fn THcTrigRawHit& THcTrigRawHit::operator=(const THcTrigRawHit& right)

\brief Assignment operator.

\param[in] right Raw hit to be assigned.
*/

/**
\fn THcTrigRawHit::~THcTrigRawHit()

\brief A destructor.
*/

/**
\fn void THcTrigRawHit::Clear(Option_t* opt="")

\brief Clears variables before next event.

\param[in] opt Maybe used in base clas... Not sure.

*/

/**
\fn void THcTrigRawHit::SetData(Int_t signal, Int_t data)

\brief Sets next data value.

\param[in] signal ADC or TDC.
\param[in] data Value to be set.
*/

/**
\fn void THcTrigRawHit::SetSample(Int_t signal, Int_t data)

\brief Sets next waveform sample value.

\param[in] signal ADC or TDC.
\param[in] data Sample value to be set.
*/

/**
\fn void THcTrigRawHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak)

\brief Sets multiple bits of data from flash 250 ADC modules.

\param[in] signal Usually ADC.
\param[in] data ADC sum value to be set.
\param[in] time Time of the peak.
\param[in] pedestal Pedestal value.
\param[in] peak Sample ADC value at peak.
*/

/**
\fn void THcTrigRawHit::SetReference(Int_t signal, Int_t reference)

\brief Sets reference time.

\param[in] signal ADC or TDC.
\param[in] reference Reference time to be set.
*/

/**
\fn Int_t THcTrigRawHit::GetData(Int_t signal)

\brief Gets data for first hit of signal.

\param[in] signal ADC or TDC.

Read also GetData(Int_t signal, UInt_t iHit).
*/

/**
\fn Int_t THcTrigRawHit::GetData(Int_t signal, UInt_t iHit)

\brief Gets data for specific hit of signal.

\param[in] signal ADC or TDC.
\param[in] iHit Sequential number of wanted hit.

If the signal is TDC, the returned value is corrected for reference time, if
available.

Read also GetRawData(Int_t signal, UInt_t iHit)
*/

/**
\fn Int_t THcTrigRawHit::GetRawData(Int_t signal)

\brief Gets raw data for first hit of signal.

\param[in] signal ADC or TDC.

Read also GetRawData(Int_t signal, UInt_t iHit).
*/

/**
\fn Int_t THcTrigRawHit::GetRawData(Int_t signal, UInt_t iHit)

\brief Gets raw data for specific hit of signal.

\param[in] signal ADC or TDC.
\param[in] iHit Sequential number of wanted hit.

If requesting the first hit where it does not exist, `0` is returned.
*/

/**
\fn Int_t THcTrigRawHit::GetAdcTime(UInt_t iHit)

\brief Gets reference time subtracted time of ADC peak.

\param[in] iHit Sequential number of wanted hit.

If requesting the first hit where it does not exist or if time is not
available, `0` is returned.
*/

/**
\fn Int_t THcTrigRawHit::GetAdcPedestal(UInt_t iHit)

\brief Gets the ADC pedestal value.

\param[in] iHit Sequential number of wanted hit.

The pedestal value is usually a sum of `N` samples, where `N` is defined in
ADC module settings.

If requesting the first hit where it does not exist or if pedestal is not
available, `0` is returned.
*/

/**
\fn Int_t THcTrigRawHit::GetAdcPeak(UInt_t iHit)

\brief Gets the position of ADC peak.

\param[in] iHit Sequential number of wanted hit.

If requesting the first hit where it does not exist or if peak is not
available, `0` is returned.
*/

/**
\fn Int_t THcTrigRawHit::GetNSignals()

\brief Returns number of signal handled by this class, i.e., `2`.
*/

/**
\fn THcRawHit::ESignalType THcTrigRawHit::GetSignalType(Int_t signal)

\brief Returns the signal type.

\param[in] signal ADC or TDC.
*/

/**
\fn Int_t THcTrigRawHit::GetReference(Int_t signal)

\brief Returns reference time.

\param[in] signal ADC or TDC.

Returns `0` if reference time is not available.
*/

/**
\fn Int_t THcTrigRawHit::GetMultiplicity(Int_t signal)

\brief Gets the number of hits for this event.

\param[in] signal ADC or TDC.

*/

/**
\fn Bool_t THcTrigRawHit::HasMulti(Int_t signal)

\brief Checks if rich data is available.

\param[in] signal ADC or TDC.

*/

/**
\fn Bool_t THcTrigRawHit::HasReference(Int_t signal)

\brief Checks if reference time is available.

\param[in] signal ADC or TDC.

*/

// TODO: Check if signal matches plane.

#include "THcTrigRawHit.h"

#include <iostream>
#include <string>
#include <stdexcept>

#include "TObject.h"
#include "TString.h"


THcTrigRawHit::THcTrigRawHit(
  Int_t plane, Int_t counter
) :
  THcRawHit(plane, counter),
  fAdc(), fAdcTime(), fAdcPedestal(), fAdcPeak(), fAdcSamples(), fTdc(),
  fReferenceTime(), fHasReference(), fHasMulti(), fNRawHits(), fNRawSamples(0)
{}


THcTrigRawHit& THcTrigRawHit::operator=(const THcTrigRawHit& right) {
  // Call base class assignment operator.
  THcRawHit::operator=(right);

  if (this != &right) {
    for (UInt_t i=0; i<fMaxNPulsesAdc; ++i) {
      fAdc[i] = right.fAdc[i];
      fAdcTime[i] = right.fAdcTime[i];
      fAdcPedestal[i] = right.fAdcPedestal[i];
      fAdcPeak[i] = right.fAdcPeak[i];
    }
    for (UInt_t i=0; i<fMaxNSamplesAdc; ++i) {
      fAdcSamples[i] = right.fAdcSamples[i];
    }
    for (UInt_t i=0; i<fMaxNHitsTdc; ++i) {
      fTdc[i] = right.fTdc[i];
    }
    for (UInt_t i=0; i<fNPlanes; ++i) {
      fReferenceTime[i] = right.fReferenceTime[i];
      fHasReference[i] = right.fHasReference[i];
      fHasMulti[i] = right.fHasMulti[i];
      fNRawHits[i] = right.fNRawHits[i];
    }
    fNRawSamples = right.fNRawSamples;
  }

  return *this;
}


THcTrigRawHit::~THcTrigRawHit() {}


void THcTrigRawHit::Clear(Option_t* opt) {
  TObject::Clear(opt);

  for (UInt_t i=0; i<fNPlanes; ++i) {
    fHasReference[i] = kFALSE;
    fHasMulti[i] = kFALSE;
    fNRawHits[i] = 0;
  }
  fNRawSamples = 0;
}


void THcTrigRawHit::SetData(Int_t signal, Int_t data) {
  // Signal 0 is ADC; signal 1 is TDC.
  if (signal == 0) {
    if (fNRawHits[signal] >= fMaxNPulsesAdc) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many pulses for ADC signal!"
      );
    }
    fAdc[fNRawHits[signal]] = data;
    ++fNRawHits[signal];
  }
  // Signal 1 is TDC.
  else if (signal == 1) {
    if (fNRawHits[signal] >= fMaxNHitsTdc) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many hits for TDC signal!"
      );
    }
    fTdc[fNRawHits[signal]] = data;
    ++fNRawHits[signal];
  }
  else throw std::out_of_range(
    "`THcTrigRawHit::SetData`: only signals `0` (ADC) and `1` (TDC) available!"
  );
}


void THcTrigRawHit::SetSample(Int_t signal, Int_t data) {
  if (signal == 0) {
    if (fNRawSamples >= fMaxNSamplesAdc) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many samples for ADC signal!"
      );
    }
    fAdcSamples[fNRawSamples] = data;
    ++fNRawSamples;
  }
  else throw std::out_of_range(
    "`THcTrigRawHit::SetSample`: only signal `0` (ADC) support samples!"
  );
}


void THcTrigRawHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (signal == 0) {
    if (fNRawHits[signal] >= fMaxNPulsesAdc) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many pulses for ADC signal!"
      );
    }
    fAdc[fNRawHits[signal]] = data;
    fAdcTime[fNRawHits[signal]] = time;
    fAdcPedestal[fNRawHits[signal]] = pedestal;
    fAdcPeak[fNRawHits[signal]] = peak;
    fHasMulti[signal] = kTRUE;
    ++fNRawHits[signal];
  }
  else throw std::out_of_range(
    "`THcTrigRawHit::SetDataTimePedestalPeak`: only signal `0` (ADC) support rich data!"
  );
}


void THcTrigRawHit::SetReference(Int_t signal, Int_t reference) {
  if (signal == 0) {
    std::cerr
      << "Warning:"
      << " `THcTrigRawHit::SetReference`:"
      << " signal 0 (ADC) should not have reference time!"
      << " Check map file!";
  }
  else if (signal == 1) {
    fReferenceTime[signal] = reference;
    fHasReference[signal] = kTRUE;
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::SetReference`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetData(Int_t signal) {
  return GetData(signal, 0);
}


Int_t THcTrigRawHit::GetData(Int_t signal, UInt_t iHit) {
  Int_t value = GetRawData(signal, iHit);

  if (signal == 1 && fHasReference[signal]) value -= fReferenceTime[signal];

  return value;
}


Int_t THcTrigRawHit::GetRawData(Int_t signal) {
  return GetRawData(signal, 0);
}


Int_t THcTrigRawHit::GetRawData(Int_t signal, UInt_t iHit) {
  Int_t value = 0;

  if (iHit >= fNRawHits[signal] && iHit != 0) {
    TString msg = TString::Format(
      "`THcTrigRawHit::GetRawData`: requested hit %d for signal %d where only %d hit available!",
      iHit, signal, fNRawHits[signal]
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iHit >= fNRawHits[signal] && iHit == 0) {
    value = 0;
  }
  else {
    if (signal == 0) value = fAdc[iHit];
    else if (signal == 1) value = fTdc[iHit];
    else {
      throw std::out_of_range(
        "`THcTrigRawHit::GetRawData`: only signals `0` and `1` available!"
      );
    }
  }

  return value;
}


Int_t THcTrigRawHit::GetAdcTime(UInt_t iHit) {
  Int_t value = 0;
  Int_t signal = 0;

  if (iHit >= fNRawHits[signal] && iHit != 0) {
    TString msg = TString::Format(
      "`THcTrigRawHit::GetAdcTime`: requested hit %d for signal %d where only %d hit available!",
      iHit, signal, fNRawHits[signal]
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iHit >= fNRawHits[signal] && iHit == 0) {
    value = 0;
  }
  else {
    if (fHasMulti[0]) value = fAdcTime[iHit];
    else value = 0;
  }

  if (fHasReference[signal]) value -= fReferenceTime[signal];

  return value;
}


Int_t THcTrigRawHit::GetAdcPedestal(UInt_t iHit) {
  Int_t value = 0;
  Int_t signal = 0;

  if (iHit >= fNRawHits[signal] && iHit != 0) {
    TString msg = TString::Format(
      "`THcTrigRawHit::GetAdcPedestal`: requested hit %d for signal %d where only %d hit available!",
      iHit, signal, fNRawHits[signal]
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iHit >= fNRawHits[signal] && iHit == 0) {
    value = 0;
  }
  else {
    if (fHasMulti[0]) value = fAdcPedestal[iHit];
    else value = 0;
  }

  return value;
}


Int_t THcTrigRawHit::GetAdcPeak(UInt_t iHit) {
  Int_t value = 0;
  Int_t signal = 0;

  if (iHit >= fNRawHits[signal] && iHit != 0) {
    TString msg = TString::Format(
      "`THcTrigRawHit::GetAdcPeak`: requested hit %d for signal %d where only %d hit available!",
      iHit, signal, fNRawHits[signal]
    );
    throw std::out_of_range(msg.Data());
  }
  else if (iHit >= fNRawHits[signal] && iHit == 0) {
    value = 0;
  }
  else {
    if (fHasMulti[0]) value = fAdcPeak[iHit];
    else value = 0;
  }

  return value;
}


Int_t THcTrigRawHit::GetNSignals() {
  return 2;
}


THcRawHit::ESignalType THcTrigRawHit::GetSignalType(Int_t signal) {
  if (signal == 0) return kADC;
  else if (signal == 1) return kTDC;
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetSignalType`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetReference(Int_t signal) {
  if (signal == 0 || signal == 1) {
    if (fHasReference[signal]) return fReferenceTime[signal];
    else return 0;
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetReference`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetMultiplicity(Int_t signal) {
  if (signal == 0 || signal == 1) {
    return fNRawHits[signal];
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetMultiplicity`: only signals `0` and `1` available!"
    );
  }
}


Bool_t THcTrigRawHit::HasMulti(Int_t signal) {
  if (signal == 0 || signal == 1) {
    return fHasMulti[signal];
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::HasMulti`: only signals `0` and `1` available!"
    );
  }
}


Bool_t THcTrigRawHit::HasReference(Int_t signal) {
  if (signal == 0 || signal == 1) {
    return fHasReference[signal];
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::HasReference`: only signals `0` and `1` available!"
    );
  }
}


ClassImp(THcTrigRawHit)
