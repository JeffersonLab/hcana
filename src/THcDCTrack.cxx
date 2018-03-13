/** \class THcDCTrack
    \ingroup DetSupport

 Class representing a track found from linking DC Space points

*/

#include "THcDCHit.h"
#include "THcDCTrack.h"
#include "THcSpacePoint.h"
THcDCTrack::THcDCTrack(Int_t nplanes) : fnSP(0), fNHits(0)
{
  fHits.clear();
  fCoords.resize(nplanes);
  fResiduals.resize(nplanes);
  fDoubleResiduals.resize(nplanes);
}

void THcDCTrack::AddHit(THcDCHit * hit, Double_t dist, Int_t lr)
{
  // Add a hit to the track
  Hit newhit;
  newhit.dchit = hit;
  newhit.distCorr = dist;
  newhit.lr = lr;
  fHits.push_back(newhit);
  fNHits++;
}
void THcDCTrack::AddSpacePoint( THcSpacePoint* sp )
{
  // Add to list of space points in this track
  // Need a check for maximum spacepoints of 10
  fSp[fnSP++] = sp;
  // Copy all the hits from the space point into the track
  // Will need to also copy the corrected distance and lr information
  for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
    AddHit(sp->GetHit(ihit),sp->GetHitDist(ihit),sp->GetHitLR(ihit));
  }
}

void THcDCTrack::Clear( const Option_t* )
{
  // Clear the space point and hit lists
  fnSP = 0;
  fSp1_ID=-1;
  fSp2_ID=-1;
  ClearHits();
  // Need to set default values  (0 or -100)
  //fCoords.clear();
  //fResiduals.clear();
  //fDoubleResiduals.clear();
}
void THcDCTrack::ClearHits( )
{
  fNHits = 0;
  fHits.clear();
}

ClassImp(THcDCTrack)

///////////////////////////////////////////////////////////////////////////////
