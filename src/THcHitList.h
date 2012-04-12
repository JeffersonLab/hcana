#ifndef ROOT_THcHitList
#define ROOT_THcHitList

#include "THcRawHit.h"
#include "TClonesArray.h"


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

  THcHitList(); // only for ROOT I/O
  THcHitList( const char* name, const char* description );


  virtual Int_t Decode( const THaEvData& );
  void          InitHitlist(const char *hitclass, Int_t maxhits);

  // This is a list of pointers to hit objects
  // Instead should we have a list of the actual objects so that we are
  // no delting and creating objects all the time.
  //
  Int_t         fNRawHits;
  Int_t         fNMaxRawHits;
  TClonesArray* fRawHitList; // List of raw hits
  TClass* fRawHitClass;		  // Class of raw hit object to use

 protected:

  ClassDef(THcHitList,0)
};
#endif
