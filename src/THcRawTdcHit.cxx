/**
\class THcRawTdcHit
\ingroup DetSupport

\brief Class representing a single raw TDC hit.
*/

#include "THcRawTdcHit.h"

#include <stdexcept>

#include <TString.h>


THcRawTdcHit::THcRawTdcHit() :
  TObject(),
  fTime(), fRefTime(0), fHasRefTime(kFALSE), fNHits(0)
{}


THcRawTdcHit& THcRawTdcHit::operator=(const THcRawTdcHit& right) {
  TObject::operator=(right);

  if (this != &right) {
    for (UInt_t iHit=0; iHit<fMaxNHits; ++iHit) {
      fTime[iHit] = right.fTime[iHit];
    }
    fRefTime = right.fRefTime;
    fHasRefTime = right.fHasRefTime;
    fNHits = right.fNHits;
  }

  return *this;
}


THcRawTdcHit::~THcRawTdcHit() {}


void THcRawTdcHit::Clear(Option_t* opt) {
  TObject::Clear(opt);

  for (UInt_t iHit=0; iHit<fNHits; ++iHit) {
    fTime[iHit] = 0;
  }
  fRefTime = 0;
  fHasRefTime = kFALSE;
  fNHits = 0;
}


void THcRawTdcHit::SetTime(Int_t time) {
  if (fNHits < fMaxNHits) {
    fTime[fNHits] = time;
    ++fNHits;
  }
  else {
    TString msg = TString::Format(
      "`THcRawTdcHit::SetTime`: Trying to set too many hits! Only %d slots available.",
      fMaxNHits
    );
    throw std::out_of_range(msg.Data());
  }
}


void THcRawTdcHit::SetRefTime(Int_t refTime) {
  fRefTime = refTime;
  fHasRefTime = kTRUE;
}

Int_t THcRawTdcHit::GetTimeRaw(UInt_t iHit) const {
  if (iHit < fNHits) {
    return fTime[iHit];
  }
  else if (iHit == 0) {
    return 0;
  }
  else {
    TString msg = TString::Format(
      "`THcRawTdcHit::GetTimeRaw`: Trying to get hit %d where only %d hits available!",
      iHit, fNHits
    );
    throw std::out_of_range(msg.Data());
  }
}


Int_t THcRawTdcHit::GetTime(UInt_t iHit) const {
  Int_t time = GetTimeRaw(iHit);
  if (fHasRefTime) {
    time -= fRefTime;
  }
  return time;
}


Int_t THcRawTdcHit::GetRefTime() const {
  if (fHasRefTime) {
    return fRefTime;
  }
  else {
    TString msg = TString::Format(
      "`THcRawTdcHit::GetRefTime`: Reference time not available!"
    );
    throw std::runtime_error(msg.Data());
  }
}

Int_t THcRawTdcHit::HasRefTime() const {
  return fHasRefTime;
}


ClassImp(THcRawTdcHit)
