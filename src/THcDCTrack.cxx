///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCTrack                                                                //
//                                                                           //
// Class representing a track found from linking DC Space points             //
///////////////////////////////////////////////////////////////////////////////

#include "THcDCHit.h"
#include "THcDCTrack.h"
THcDCTrack::THcDCTrack(Int_t nplanes) : fnSP(0), fNHits(0)
{
  fHits.clear();
  fCoords.resize(nplanes);
  fResiduals.resize(nplanes);
  fDoubleResiduals.resize(nplanes);
}  

void THcDCTrack::AddHit(THcDCHit * hit)
{
  // Add a hit to the track
  fHits.push_back(hit);
  fNHits++;
}
void THcDCTrack::AddSpacePoint( Int_t spid )
{
  // Add to list of space points in this track
  // Need a check for maximum spacepoints of 10
  fspID[fnSP++] = spid;
}

void THcDCTrack::Clear( const Option_t* )
{
  // Clear the space point and hit lists
  fnSP = 0;
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
