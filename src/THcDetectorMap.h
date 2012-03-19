#ifndef ROOT_THcDetectorMap
#define ROOT_THcDetectorMap

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorMap
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class THcDetectorMap : public TObject {

 public:
  THcDetectorMap() {}
  virtual ~THcDetectorMap() {}
  
  virtual void Load(const char *fname);

 protected:

  ClassDef(THcDetectorMap,0);
};
#endif

  
  
