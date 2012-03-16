//*-- Author : Stephen Wood  

//////////////////////////////////////////////////////////////////////////
//
// THcRawHitList
//
// Class to build raw hit lists from data
//
//////////////////////////////////////////////////////////////////////////

#include "THcRawHitList.h"

using namespace std;

THcRawHitList::THcRawHitList(const char* classname, Int_t detectorid, Int_t size=1000) {
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
ClassImp(THcRawHitList)
