/////////////////////////////////////////////////////////////////////
//
//  THcCrateMap
//  Layout, or "map", of DAQ Crates.
//
//  THaCrateMap contains info on how the DAQ crates
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


#include "THcCrateMap.h"

#include "TError.h"

using namespace std;

THcCrateMap::THcCrateMap( const char* db_filename )
{
  // Need to do something different since we are going to read
  // a Hall C style map file.

  // Construct uninitialized crate map. The argument is the name of
  // the database file to use for initialization

  if( !db_filename || !*db_filename ) {
    ::Warning( "THcCrateMap", "Undefined database file name, "
	       "using default \"db_cratemap.dat\"" );
    db_filename = "cratemap";
  }
  // Why does this give an error?  Can't I use a private
  // member of the class I inherit from?
  fMapfileName = db_filename;

}



ClassImp(THcCrateMap)
