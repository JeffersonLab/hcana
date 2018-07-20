/** \class THcRawHit
    \ingroup DetSupport

    \brief Base class detector specific raw hit that go into hit lists

    Contains plane, counter and at least one data value

*/

#include "THcRawHit.h"


Int_t THcRawHit::Compare(const TObject* obj) const
{
  // Comparision function for Sort().

  const THcRawHit* hit = dynamic_cast<const THcRawHit*>(obj);

  if(!hit) return -1;
  Int_t p1 = fPlane;
  Int_t p2 = hit->fPlane;
  if(p1 < p2) return -1;
  else if(p1 > p2) return 1;
  else {
    Int_t c1 = fCounter;
    Int_t c2 = hit->fCounter;
    if(c1 < c2) return -1;
    else if (c1 == c2) return 0;
    else return 1;
  }
}


ClassImp(THcRawHit)

//_____________________________________________________________________________
