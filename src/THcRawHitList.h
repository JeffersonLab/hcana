 #ifndef ROOT_THcRawHitList
 #define ROOT_THcRawHitList

//////////////////////////////////////////////////////////////////////////
//
// THcRawHistList.h
//
//////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THcRawHit.h"
#include "THcDetectorMap.h"
#include "THaEvData.h"
#include <cassert>

class THcRawHitList {

 public:

  THcRawHitList(const char* classname, Int_t detectorid, Int_t size);
  virtual ~THcRawHitList();

  Int_t Fill(const THaEvData& evdata, const THcDetectorMap& dmap);
  // Should detector map be a member variable too?


  TClonesArray* fHits;
  Int_t fMaxhit;

  Int_t          GetNHits()     const { return fHits->GetLast()+1; }
  TClonesArray*  GetHits()      const { return fHits; }

  // Should have a raw hit object so that certain methods are
  // garuanteed to exist.  (Like plane and counter)  Because we may have
  // methods to look up a certain plane or plane/counter, or sort etc.

  THcRawHit*       GetHit(Int_t i) const
  { assert(i >=0 && i<GetNHits() );
    return (THcRawHit*)fHits->UncheckedAt(i);}

  void Clear( Option_t*);

 protected:
  Int_t fDetectorid;

 private:
  ClassDef(THcRawHitList, 0); // Raw hit class
};
#endif

