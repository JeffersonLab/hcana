/** \class THcDCHit
    \ingroup DetSupport

    \brief Drift chamber wire hit info

*/
#include "THcDCHit.h"
#include "THcDCTimeToDistConv.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(THcDCHit)

const Double_t THcDCHit::kBig = 1.e38; // Arbitrary large value

//_____________________________________________________________________________
void THcDCHit::Print( Option_t* opt ) const
{
  // Print hit info

  cout << "Hit: wire=" << GetWireNum()
       << "/" << (fWirePlane ? fWirePlane->GetName() : "??")
       << " wpos="     << GetPos()
       << " time="     << GetTime()
       << " drift="    << GetDist();
  //       << " res="      << GetResolution()
  //       << " z="        << GetZ()
  if( *opt != 'C' )
    cout << endl;
}

//_____________________________________________________________________________
Double_t THcDCHit::ConvertTimeToDist()
{
  // Converts TDC time to drift distance
  // Takes the (estimated) slope of the track as an argument

  THcDCTimeToDistConv* ttdConv = (fWire) ? fWire->GetTTDConv() : NULL;

  if (ttdConv) {
    // If a time to distance algorithm exists, use it to convert the TDC time
    // to the drift distance
    fDist = ttdConv->ConvertTimeToDist(fTime);
    return fDist;
  }

  Error("ConvertTimeToDist()", "No Time to dist algorithm available");
  return 0.0;

}

//_____________________________________________________________________________
Int_t THcDCHit::Compare( const TObject* obj ) const
{
  // Used to sort hits
  // A hit is "less than" another hit if it occurred on a lower wire number.
  // Also, for hits on the same wire, the first hit on the wire (the one with
  // the smallest time) is "less than" one with a higher time.  If the hits
  // are sorted according to this scheme, they will be in order of increasing
  // wire number and, for each wire, will be in the order in which they hit
  // the wire

  if( !obj || IsA() != obj->IsA() || !fWire )
    return -1;

  const THcDCHit* hit = static_cast<const THcDCHit*>( obj );

  Int_t myWireNum = fWire->GetNum();
  Int_t hitWireNum = hit->GetWire()->GetNum();
  Int_t myPlaneNum = GetPlaneNum();
  Int_t hitPlaneNum = hit->GetPlaneNum();
  if (myPlaneNum < hitPlaneNum) return -1;
  if (myPlaneNum > hitPlaneNum) return 1;
  // If planes are the same, compare wire numbers
  if (myWireNum < hitWireNum) return -1;
  if (myWireNum > hitWireNum) return  1;
  // If wire numbers are the same, compare times
  Double_t hitTime = hit->GetTime();
  if (fTime < hitTime) return -1;
  if (fTime > hitTime) return  1;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
