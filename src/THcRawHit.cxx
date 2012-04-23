///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawHit                                                                 //
//                                                                           //
// Abstract class for a single raw hit                                       //
//                                                                           //
// Contains plane, counter and at least one data value                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawHit.h"

THcRawHit::THcRawHit()
{
  // Constructor
}

THcRawHit::THcRawHit(Int_t plane, Int_t counter) : 
  fPlane(plane), fCounter(counter)
{
}

THcRawHit::~THcRawHit()
{}

ClassImp(THcRawHit)

//_____________________________________________________________________________
