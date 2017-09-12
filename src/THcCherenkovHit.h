#ifndef ROOT_THcCherenkovHit
#define ROOT_THcCherenkovHit

#include "THcRawHodoHit.h"

class THcCherenkovHit : public THcRawHodoHit {

 public:
  friend class THcCherenkov;

 protected:

 private:

  ClassDef(THcCherenkovHit,0);	// Raw Cherenkov hit
};

#endif

