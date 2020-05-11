/**
\class THcRawTdcHit
\ingroup DetSupport
\brief Class representing a single raw TDC hit.
*/

/**
\fn THcRawTdcHit::THcRawTdcHit()
\brief Constructor.
*/

/**
\fn THcRawTdcHit& THcRawTdcHit::operator=(const THcRawTdcHit& right)
\brief Assignment operator.
\param[in] right Raw TDC hit to be assigned.
*/

/**
\fn THcRawTdcHit::~THcRawTdcHit()
\brief Destructor.
*/

/**
\fn void THcRawTdcHit::Clear(Option_t* opt="")
\brief Clears variables before next event.
\param[in] opt Maybe used in base clas... Not sure.
*/

/**
\fn void THcRawTdcHit::SetTime(Int_t time)
\brief Sets raw TDC time from the modules. In channels.
\param[in] time Raw TDC time from the modules. In channels.
\throw std::out_of_range Tried to set too many hits.
*/

/**
\fn void THcRawTdcHit::SetRefTime(Int_t refTime)
\brief Sets reference time. In channels.
\param[in] refTime Reference time. In channels.
*/

/**
\fn Int_t THcRawTdcHit::GetTimeRaw(UInt_t iHit=0) const
\brief Gets raw TDC time. In channels.
\param[in] iHit Sequential number of requested hit.
\throw std::out_of_range Tried to access nonexisting hit.

Returns 0 if tried to access first hit but no hits are set.
*/

/**
\fn Int_T THcRawTdcHit::GetTime(UInt_t iHit=0) const
\brief Gets TDC time. In channels.
\param[in] iHit Sequential number of requested hit.

Returned time is corrected for reference time, if available.
*/

/**
\fn Int_t THcRawTdcHit::GetRefTime() const
\brief Gets reference time. In channels.
\throw std::runtime_error No reference time was set.
*/

/**
\fn Bool_t THcRawTdcHit::HasRefTime() const
\brief Queries whether reference time has been set.
*/

/**
\fn UInt_t THcRawTdcHit::GetNHits() const
\brief Gets the number of set hits.
*/


#include "THcRawTdcHit.h"

#include <stdexcept>

#include <TString.h>


THcRawTdcHit::THcRawTdcHit() :
  TObject(),
  fChannelToTimeFactor(0.1),
  fTime(), fRefTime(0), fHasRefTime(kFALSE), fNHits(0)
{}


THcRawTdcHit& THcRawTdcHit::operator=(const THcRawTdcHit& right) {
  TObject::operator=(right);

  if (this != &right) {
    for (UInt_t iHit=0; iHit<fMaxNHits; ++iHit) {
      fTime[iHit] = right.fTime[iHit];
    }
    fRefTime = right.fRefTime;
    fRefDiffTime = right.fRefDiffTime;
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
  fRefDiffTime = 0;
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

void THcRawTdcHit::SetRefDiffTime(Int_t refDiffTime) {
  fRefDiffTime = refDiffTime;
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

Int_t THcRawTdcHit::GetRefDiffTime() const {
  if (fHasRefTime) {
    return fRefDiffTime;
  }
  else {
    TString msg = TString::Format(
      "`THcRawTdcHit::GetRefDiffTime`: Reference time not available!"
    );
    throw std::runtime_error(msg.Data());
  }
}


Bool_t THcRawTdcHit::HasRefTime() const {
  return fHasRefTime;
}


UInt_t THcRawTdcHit::GetNHits() const {
  return fNHits;
}


ClassImp(THcRawTdcHit)
