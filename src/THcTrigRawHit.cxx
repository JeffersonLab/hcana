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
\param[in] opt Maybe used in base class... Not sure.

*/

/**
\fn void THcTrigRawHit::SetData(Int_t signal, Int_t data)
\brief Sets next data value.
\param[in] signal ADC or TDC.
\param[in] data Value to be set.
\throw std::out_of_range Tried to set wrong signal.
*/

/**
\fn void THcTrigRawHit::SetSample(Int_t signal, Int_t data)
\brief Sets next waveform sample value.
\param[in] signal ADC.
\param[in] data Sample value to be set.
\throw std::out_of_range Tried to set wrong signal.
*/

/**
\fn void THcTrigRawHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak)
\brief Sets multiple bits of data from flash 250 ADC modules.
\param[in] signal ADC.
\param[in] data ADC sum value to be set.
\param[in] time Time of the peak.
\param[in] pedestal Pedestal value.
\param[in] peak Sample ADC value at peak.
\throw std::out_of_range Tried to set wrong signal.
*/

/**
\fn void THcTrigRawHit::SetReference(Int_t signal, Int_t reference)
\brief Sets reference time.
\param[in] signal TDC.
\param[in] reference Reference time to be set.
\throw std::out_of_range Tried to set wrong signal.
*/

/**
\fn Int_t THcTrigRawHit::GetData(Int_t signal)
\brief Gets data for first hit of signal.
\param[in] signal ADC or TDC.
\throw std::out_of_range Tried to get wrong signal.
*/

/**
\fn Int_t THcTrigRawHit::GetRawData(Int_t signal)
\brief Gets raw data for first hit of signal.
\param[in] signal ADC or TDC.
\throw std::out_of_range Tried to get wrong signal.
*/

/**
\fn Int_t THcTrigRawHit::GetReference(Int_t signal)
\brief Returns reference time.
\param[in] signal TDC.
\throw std::out_of_range Tried to get wrong signal.
*/

/**
\fn THcRawHit::ESignalType THcTrigRawHit::GetSignalType(Int_t signal)
\brief Returns the signal type.
\param[in] signal ADC or TDC.
\throw std::out_of_range Tried to get wrong signal.
*/

/**
\fn Int_t THcTrigRawHit::GetNSignals()
\brief Returns number of signal handled by this class, i.e., `2`.
*/

/**
\fn Bool_t THcTrigRawHit::HasReference(Int_t signal)
\brief Checks if reference time is available.
\param[in] signal TDC.
\throw std::out_of_range Tried to get wrong signal.
*/

/**
\fn THcRawAdcHit& THcTrigRawHit::GetRawAdcHit()
\brief Gets reference to THcRawAdcHit.
*/

/**
\fn THcRawTdcHit& THcTrigRawHit::GetRawTdcHit()
\brief Gets reference to THcRawTdcHit.
*/

/**
\fn void THcTrigRawHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED)
\brief See THcRawAdcHit::SetF250Params.
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
) : THcRawHit(plane, counter), fAdcHits(), fTdcHits() {}


THcTrigRawHit& THcTrigRawHit::operator=(const THcTrigRawHit& right) {
  // Call base class assignment operator.
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


THcTrigRawHit::~THcTrigRawHit() {}


void THcTrigRawHit::Clear(Option_t* opt) {
  THcRawHit::Clear(opt);

  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].Clear();
  }
  for (Int_t iTdcSig=0; iTdcSig<fNTdcSignals; ++iTdcSig) {
    fTdcHits[iTdcSig].Clear();
  }
}


void THcTrigRawHit::SetData(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetData(data);
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    fTdcHits[signal-fNAdcSignals].SetTime(data);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::SetData`: only signals `0` and `1` available!"
    );
  }
}


void THcTrigRawHit::SetSample(Int_t signal, Int_t data) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetSample(data);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::SetSample`: only signal `0` available!"
    );
  }
}


void THcTrigRawHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (0 <= signal && signal < fNAdcSignals) {
    fAdcHits[signal].SetDataTimePedestalPeak(data, time, pedestal, peak);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::SetDataTimePedestalPeak`: only signal `0` available!"
    );
  }
}


void THcTrigRawHit::SetReference(Int_t signal, Int_t reference) {
  if (signal < fNAdcSignals) {
    fAdcHits[signal].SetRefTime(reference);
  } else if (signal < fNAdcSignals+fNTdcSignals) {
    fTdcHits[signal-fNAdcSignals].SetRefTime(reference);
  } else {
    throw std::out_of_range(
      "`THcTrigRawHit::SetReference`: only signals `2` and `3` available!"
    );
  }
}


Int_t THcTrigRawHit::GetData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetPulseInt();
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetTime();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetRawData(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return fAdcHits[signal].GetPulseIntRaw();
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetTimeRaw();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetRawData`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetReference(Int_t signal) {
  if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].GetRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetReference`: only signal `1` available!"
    );
  }
}


THcRawHit::ESignalType THcTrigRawHit::GetSignalType(Int_t signal) {
  if (0 <= signal && signal < fNAdcSignals) {
    return kADC;
  }
  else if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return kTDC;
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetSignalType`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcTrigRawHit::GetNSignals() {
  return fNAdcSignals + fNTdcSignals;
}


Bool_t THcTrigRawHit::HasReference(Int_t signal) {
  if (fNAdcSignals <= signal && signal < fNAdcSignals+fNTdcSignals) {
    return fTdcHits[signal-fNAdcSignals].HasRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::HasReference`: only signal `1` available!"
    );
  }
}


THcRawAdcHit& THcTrigRawHit::GetRawAdcHit() {
  return fAdcHits[0];
}


THcRawTdcHit& THcTrigRawHit::GetRawTdcHit() {
  return fTdcHits[0];
}


void THcTrigRawHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED) {
  for (Int_t iAdcSig=0; iAdcSig<fNAdcSignals; ++iAdcSig) {
    fAdcHits[iAdcSig].SetF250Params(NSA, NSB, NPED);
  }
}


ClassImp(THcTrigRawHit)
