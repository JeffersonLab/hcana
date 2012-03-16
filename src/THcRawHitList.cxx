//*-- Author : Stephen Wood  

//////////////////////////////////////////////////////////////////////////
//
// THcRawHistList
//
// Class to build raw hit lists from data
//
//////////////////////////////////////////////////////////////////////////

#include "THcRawHitList.h"

using namespace std;

THcRawHitList::THcRawHitList(const char* classname, Int_t size=1000, Int_t detctorid) {
  fHits = new TClonesArray(classname, size);
  fDetectorid = detectorid;
}

THcRawHitList::~THcRawHitList() {
  delete fHits;
}

void THcRawHitList::Clear( Option_t*)
{
  fHits->Clear();
}

//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawHistList)
