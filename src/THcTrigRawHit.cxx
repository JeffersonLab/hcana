/**
\class THcTrigRawHit
\ingroup DetSupport

\brief Class representing a single raw hit for the THcTrigDet.

Contains plane, counter and an ADC or a TDC value.

Note: not yet finalized!
*/

#include "THcTrigRawHit.h"

#include <stdexcept>


THcTrigRawHit::THcTrigRawHit(
  Int_t plane, Int_t counter
) :
  THcRawHit(plane, counter),
  fAdcVal(), fTdcVal(), fNumAdcSamples(0), fNumTdcSamples(0),
  fAdcReferenceTime(0), fTdcReferenceTime(0),
  fHasAdcRef(kFALSE), fHasTdcRef(kFALSE)
{}


THcTrigRawHit& THcTrigRawHit::operator=(const THcTrigRawHit& right) {
  // Call base class assignment operator.
  THcRawHit::operator=(right);

  for (UInt_t i=0; i<fNumAdcSamples; ++i) fAdcVal[i] = right.fAdcVal[i];
  for (UInt_t i=0; i<fNumTdcSamples; ++i) fTdcVal[i] = right.fTdcVal[i];

  fNumAdcSamples = right.fNumAdcSamples;
  fNumTdcSamples = right.fNumTdcSamples;

  fAdcReferenceTime = right.fAdcReferenceTime;
  fTdcReferenceTime = right.fTdcReferenceTime;
  fHasAdcRef = right.fHasAdcRef;
  fHasTdcRef = right.fHasTdcRef;

  return *this;
}


THcTrigRawHit::~THcTrigRawHit() {}


void THcTrigRawHit::Clear(Option_t* opt) {
  fNumAdcSamples = 0;
  fNumTdcSamples = 0;
  fHasAdcRef = kFALSE;
  fHasTdcRef = kFALSE;
}


void THcTrigRawHit::SetData(Int_t signal, Int_t data) {
  // TODO: check if signal matches plane.
  // Signal 0 is ADC.
  if (signal == 0) {
    if (fNumAdcSamples >= fMaxAdcSamples) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many samples for ADC signal!"
      );
    }
    fAdcVal[fNumAdcSamples] = data;
    ++fNumAdcSamples;
  }
  // Signal 1 is TDC.
  else if (signal == 1) {
    if (fNumTdcSamples >= fMaxTdcSamples) {
      throw std::out_of_range(
        "`THcTrigRawHit::SetData`: too many samples for TDC signal!"
      );
    }
    fTdcVal[fNumTdcSamples] = data;
    ++fNumTdcSamples;
  }
  else throw std::out_of_range("`THcTrigRawHit::SetData`: only signals `0` and `1` available!");
}


void THcTrigRawHit::SetReference(Int_t signal, Int_t reference) {

  if (signal == 0) fAdcReferenceTime = reference;
  else if (signal == 1) fTdcReferenceTime = reference;
  else throw std::out_of_range("`THcTrigRawHit::SetReference`: only signals `0` and `1` available!");
}


Int_t THcTrigRawHit::GetData(Int_t signal) {
  // TODO: expand this. Maybe add new methods.
  // For now only returns first value.
  if (signal == 0) return fAdcVal[0];
  else if (signal == 1) return fTdcVal[0];
  else throw std::out_of_range("`THcTrigRawHit::GetData`: only signals `0` and `1` available!");
}


Int_t THcTrigRawHit::GetReference(Int_t signal) {
  // Reference time.
  if (signal == 0) return fAdcReferenceTime;
  else if (signal == 1) return fTdcReferenceTime;
  else throw std::out_of_range("`THcTrigRawHit::GetReference`: only signals `0` and `1` available!");
}


Bool_t THcTrigRawHit::HasReference(Int_t signal) {
  // Reference time.
  if (signal == 0) return fHasAdcRef;
  else if (signal == 1) return fHasTdcRef;
  else throw std::out_of_range("`THcTrigRawHit::HasReference`: only signals `0` and `1` available!");
}


ClassImp(THcTrigRawHit)
