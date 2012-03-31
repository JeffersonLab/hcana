#ifndef ROOT_THcDetectorBase
#define ROOT_THcDetectorBase

#include "THaDetectorBase.h"

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorBase
//
//////////////////////////////////////////////////////////////////////////

class THcDetectorBase : public THaDetectorBase {

 public:

  virtual ~THaDetectorBase();

  THaDetectorBase(); // only for ROOT I/O

 protected:

  ClassDef(ThcDetectorBase,0)
};
#endif
