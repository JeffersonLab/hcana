 #ifndef ROOT_THcRawHitList
 #define ROOT_THcRawHitList

//////////////////////////////////////////////////////////////////////////
//
// THcRawHistList.h
//
//////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"

class THcRawHitList {

 public:

  

  TClonesArray* fHits;
  Int_t fMaxhit;
  Int_t fDetectorid;

  Int_t          GetNHits()     const { return fHits->GetLast()+1; }
  TClonesArray*  GetHits()      const { return fHits; }

  // Should have a raw hit object so that certain methods are
  // garuanteed to exist.  (Like plane and counter)  Because we may have
  // methods to look up a certain plane or plane/counter, or sort etc.

  THcRawHit*       GetHit(Int_t i) const
  { assert(i >=0 && i<GetNHits() );
    return (THcRawHit*)fHits->UncheckedAt(i);}



 private:
  ClassDef(THcRawHitList, 0); // Raw hit class
};
#endif

