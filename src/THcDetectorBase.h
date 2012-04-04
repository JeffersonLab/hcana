#ifndef ROOT_THcDetectorBase
#define ROOT_THcDetectorBase

#include "THaDetectorBase.h"
#include "THcRawHit.h"
#include "TClonesArray.h"


using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorBase
//
//////////////////////////////////////////////////////////////////////////

//class THaDetMap;

class THcDetectorBase : public THaDetectorBase {

 public:

  virtual ~THcDetectorBase();

  THcDetectorBase(); // only for ROOT I/O
  THcDetectorBase( const char* name, const char* description );


  virtual Int_t Decode( const THaEvData& );
  void          InitHitlist(const char *hitclass, Int_t maxhits);

  // This is a list of pointers to hit objects
  // Instead should we have a list of the actual objects so that we are
  // no delting and creating objects all the time.
  //
  Int_t         fNRawHits;
  Int_t         fNMaxRawHits;
  TClonesArray* fRawHitList; // List of raw hits
  TClass* fRawHitCLass;		  // Class of raw hit object to use

 protected:

  ClassDef(THcDetectorBase,0)
};
#endif
