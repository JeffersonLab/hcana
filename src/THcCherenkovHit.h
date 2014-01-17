#ifndef ROOT_THcCherenkovHit
#define ROOT_THcCherenkovHit

#include "THcHodoscopeHit.h"

class THcCherenkovHit : public THcHodoscopeHit {

 public:
  friend class THcCherenkov;
 
 protected:

 private:

  ClassDef(THcCherenkovHit,0);	// Raw Cherenkov hit
};

#endif

