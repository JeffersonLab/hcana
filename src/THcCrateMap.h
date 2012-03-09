#ifndef THcCrateMap_
#define THcCrateMap_

/////////////////////////////////////////////////////////////////////
//
//  THcCrateMap
//  Layout, or "map", of DAQ Crates.
//
//  Inheriting from THaCrateMap
//
//  THcCrateMap contains info on how the DAQ crates
//  are arranged in Hall A, i.e whether slots are
//  fastbus or vme, what the module types are, and
//  what header info to expect.  Probably nobody needs
//  to know about this except the author, and at present
//  an object of this class is a private member of the decoder.
//
//  author  Robert Michaels (rom@jlab.org)
//  author  Stephen Wood (saw@jlab.org)
//
/////////////////////////////////////////////////////////////////////


#include "THaCrateMap.h"

class THcCrateMap : public THaCrateMap {

 public:

  THcCrateMap( const char* db = "cratemap" );    // Construct uninitialized

 private:

  TString fMapfileName;

 protected:
  ClassDef(THcCrateMap,0) // Crate map


};     
#endif
