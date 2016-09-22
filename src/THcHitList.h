#ifndef ROOT_THcHitList
#define ROOT_THcHitList

#include "THcRawHit.h"
#include "THaDetMap.h"
#include "THaEvData.h"
#include "TClonesArray.h"
#include "TObject.h"


using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// THcHitList
//
//////////////////////////////////////////////////////////////////////////

//class THaDetMap;

class THcHitList {

public:

  virtual ~THcHitList();

  THcHitList();

  virtual Int_t DecodeToHitList( const THaEvData& );
  void          InitHitList(THaDetMap* detmap,
			    const char *hitclass, Int_t maxhits);

  TClonesArray* GetHitList() const {return fRawHitList; }

  UInt_t         fNRawHits;
  Int_t         fNMaxRawHits;
  TClonesArray* fRawHitList; // List of raw hits
  TClass* fRawHitClass;		  // Class of raw hit object to use

  THaDetMap*    fdMap;

protected:

  struct RefIndexMap { // Mapping for one reference channel
    Bool_t defined;
    Bool_t hashit;
    Int_t crate;
    Int_t slot;
    Int_t channel;
    Int_t reftime;
  };
  std::vector<RefIndexMap> fRefIndexMaps;
  // Should this be a sparse list instead in case user
  // picks ridiculously large refindexes?

  Int_t fNRefIndex;

  ClassDef(THcHitList,0);  // List of raw hits sorted by plane, counter
};
#endif
