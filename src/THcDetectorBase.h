#ifndef ROOT_THcDetectorBase
#define ROOT_THcDetectorBase

#include "THaDetectorBase.h"
<<<<<<< HEAD
=======
#include "THcRawHit.h"
#include "TClonesArray.h"


using namespace std;
>>>>>>> d715b6024d14d1acc253ad0eef3926e5d9f69035

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorBase
//
//////////////////////////////////////////////////////////////////////////

<<<<<<< HEAD
=======
//class THaDetMap;

>>>>>>> d715b6024d14d1acc253ad0eef3926e5d9f69035
class THcDetectorBase : public THaDetectorBase {

 public:

<<<<<<< HEAD
  virtual ~THaDetectorBase();

  THaDetectorBase(); // only for ROOT I/O

 protected:

  ClassDef(ThcDetectorBase,0)
=======
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
  TClass* fRawHitClass;		  // Class of raw hit object to use

 protected:

  ClassDef(THcDetectorBase,0)
>>>>>>> d715b6024d14d1acc253ad0eef3926e5d9f69035
};
#endif
