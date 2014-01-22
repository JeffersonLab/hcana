#ifndef ROOT_THcAerogelHit
#define ROOT_THcAerogelHit

#include "THcHodoscopeHit.h"

class THcAerogelHit : public THcHodoscopeHit {

 public:
  friend class THcAerogel;
 
 protected:

 private:

  ClassDef(THcAerogelHit,0);	// Raw Aerogel hit
};

#endif

