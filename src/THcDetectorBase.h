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

class THcDetMap;

class THcDetectorBase : public THaDetectorBase {

 public:

  virtual ~THaDetectorBase();

  THaDetectorBase(); // only for ROOT I/O

  // This is a list of pointers to hit objects
  // Instead should we have a list of the actual objects so that we are
  // no delting and creating objects all the time.
  //
  TClonesArray* fRawHitList; // List of raw hits
  TClass* fRawHitCLass;		  // Class of raw hit object to use

 protected:

  ClassDef(ThcDetectorBase,0)
};
#endif
