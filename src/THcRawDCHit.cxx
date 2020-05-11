/** \class  THcRawDCHit
    \ingroup DetSupport

    \brief Class representing for drift chamber wire (or other device with
    a single multihit TDC channel per detector element

*/

#include "THcRawDCHit.h"
#include <stdexcept>


THcRawDCHit::THcRawDCHit(Int_t plane, Int_t counter) :
  THcRawHit(plane, counter), fTdcHit()
{}


THcRawDCHit& THcRawDCHit::operator=(const THcRawDCHit& right) {
  THcRawHit::operator=(right);

  if (this != &right) {
    fTdcHit = right.fTdcHit;
  }

  return *this;
}


THcRawDCHit::~THcRawDCHit() {}


void THcRawDCHit::Clear(Option_t* opt) {
  THcRawHit::Clear(opt);

  fTdcHit.Clear();
}


void THcRawDCHit::SetData(Int_t signal, Int_t data) {
  if (signal == 0) {
    fTdcHit.SetTime(data);
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::SetData`: only signal `0` available!"
    );
  }
}


void THcRawDCHit::SetReference(Int_t signal, Int_t reference) {
  if (signal == 0) {
    fTdcHit.SetRefTime(reference);
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::SetReference`: only signal `0` available!"
    );
  }
}

void THcRawDCHit::SetReferenceDiff(Int_t signal, Int_t reference) {
  if (signal == 0) {
    fTdcHit.SetRefDiffTime(reference);
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::SetReference`: only signal `0` available!"
    );
  }
}


Int_t THcRawDCHit::GetData(Int_t signal) {
  if (signal == 0) {
    return fTdcHit.GetTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::GetData`: only signal `0` available!"
    );
  }
}


Int_t THcRawDCHit::GetRawData(Int_t signal) {
  if (signal == 0) {
    return fTdcHit.GetTimeRaw();
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::GetRawData`: only signal `0` available!"
    );
  }
}


Int_t THcRawDCHit::GetReference(Int_t signal) {
  if (signal == 0) {
    return fTdcHit.GetRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::GetReference`: only signal `0` available!"
    );
  }
}

Int_t THcRawDCHit::GetReferenceDiff(Int_t signal) {
  if (signal == 0) {
    return fTdcHit.GetRefDiffTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::GetReference`: only signal `0` available!"
    );
  }
}


THcRawHit::ESignalType THcRawDCHit::GetSignalType(Int_t signal) {
  if (signal == 0) {
    return kTDC;
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::GetReference`: only signal `0` available!"
    );
  }
}


Int_t THcRawDCHit::GetNSignals() {
  return fNTdcSignals;
}


Bool_t THcRawDCHit::HasReference(Int_t signal) {
  if (signal == 0) {
    return fTdcHit.HasRefTime();
  }
  else {
    throw std::out_of_range(
      "`THcRawDCHit::HasReference`: only signal `0` available!"
    );
  }
}


THcRawTdcHit& THcRawDCHit::GetRawTdcHit() {
  return fTdcHit;
}


ClassImp(THcRawDCHit)
