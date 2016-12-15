/**
\class THcRawShowerHit
\ingroup DetSupport

\brief Class representing a single raw hit for a hodoscope paddle.

  - `signal 0` is ADC pos
  - `signal 1` is ADC neg
*/

#include "THcRawShowerHit.h"

#include <iostream>
#include <stdexcept>


THcRawShowerHit::THcRawShowerHit(Int_t plane, Int_t counter) :
  fAdcPos(), fAdcNeg()
{}


THcRawShowerHit& THcRawShowerHit::operator=(const THcRawShowerHit& right) {
  THcRawHit::operator=(right);

  if (this != &right) {
    fAdcPos = right.fAdcPos;
    fAdcNeg = right.fAdcNeg;
  }

  return *this;
}


THcRawShowerHit::~THcRawShowerHit() {}


void THcRawShowerHit::Clear(Option_t* opt) {
  THcRawHit::Clear(opt);

  fAdcPos.Clear();
  fAdcNeg.Clear();
}


void THcRawShowerHit::SetData(Int_t signal, Int_t data) {
  if (signal == 0) {
    fAdcPos.SetData(data);
  }
  else if (signal == 1) {
    fAdcNeg.SetData(data);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetSample(Int_t signal, Int_t data) {
  if (signal == 0) {
    fAdcPos.SetSample(data);
  }
  else if (signal == 1) {
    fAdcNeg.SetSample(data);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetDataTimePedestalPeak(
  Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
) {
  if (signal == 0) {
    fAdcPos.SetDataTimePedestalPeak(data, time, pedestal, peak);
  }
  else if (signal == 1) {
    fAdcNeg.SetDataTimePedestalPeak(data, time, pedestal, peak);
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


void THcRawShowerHit::SetReference(Int_t signal, Int_t reference) {
  std::cerr
    << "Warning:"
    << " `THcRawShowerHit::SetReference`:"
    << " ADC signal should not have reference time!"
    << std::endl;
}


Int_t THcRawShowerHit::GetData(Int_t signal) {
  if (signal == 0) {
    return fAdcPos.GetRawData();
  }
  else if (signal == 1) {
    return fAdcNeg.GetRawData();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


Int_t THcRawShowerHit::GetRawData(Int_t signal) {
  if (signal == 0) {
    return fAdcPos.GetRawData();
  }
  else if (signal == 1) {
    return fAdcNeg.GetRawData();
  }
  else {
    throw std::out_of_range(
      "`THcTrigRawHit::GetData`: only signals `0` and `1` available!"
    );
  }
}


THcRawHit::ESignalType THcRawShowerHit::GetSignalType(Int_t signal) {
  return kADC;
}


Int_t THcRawShowerHit::GetNSignals() {
  return 2;
}


THcRawAdcHit& THcRawShowerHit::GetRawAdcHitPos() {
  return fAdcPos;
}


THcRawAdcHit& THcRawShowerHit::GetRawAdcHitNeg() {
  return fAdcNeg;
}


ClassImp(THcRawShowerHit)