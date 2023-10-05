/** \class THcRawHit
    \ingroup DetSupport

    \brief Base class detector specific raw hit that go into hit lists

    Contains plane, counter and at least one data value

*/

#include "THcRawHit.h"


Int_t THcRawHit::Compare(const TObject* obj) const
{
  // Comparison function for Sort().

#ifndef NDEBUG
  const auto* hit = dynamic_cast<const THcRawHit*>(obj);
  assert(hit);
#else
  // dynamic_cast turns out to be quite expensive here
  const auto* hit = static_cast<const THcRawHit*>(obj);
#endif
  Int_t dp = fPlane - hit->fPlane;
  if( dp == 0 )  {
    Int_t dc = fCounter - hit->fCounter;
    if( dc < 0 )
      return -1;
    else if( dc > 0 )
      return 1;
    else
      return 0;
  }
  else if( dp < 0 )
    return -1;
  else
    return 1;
}


ClassImp(THcRawHit)

//_____________________________________________________________________________
