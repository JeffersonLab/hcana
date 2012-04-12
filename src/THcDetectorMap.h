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

  // Member variables needed
  // List of detector IDs.
  // Hardware to logical detector mapping for each detector
  // Mapping between detector names and numbers
  // Hardwire, but later configure
  struct Detector {
    UInt_t did; 	// Numberical Detector ID
    TList* 

 protected:

  ClassDef(THcDetectorMap,0);
};
#endif

  
  
