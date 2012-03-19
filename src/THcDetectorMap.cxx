//*-- Author: Stephen Wood

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorMap
//
// Class to read and hold Hall C style detector map
//
// Will need method to retrieve all map entries for a given
// detector id.
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetectorMap.h"

using namespace std;

ClassImp(THcDetectorMap)

inline static bool IsComment( const string& s, string::size_type pos )
{
  return ( pos != string::npos && pos < s.length() &&
	   (s[pos] == '!') );
}


void THcDetectorMap::Load(const char *fname)
{
  static const char* const here = "THcDetectorMap::Load";
  static const char* const whtspc = " \t";

  ifstream ifile;

  ifile.open(fname);
  if(!ifile.is_open) {
    Error(here, "error opening detector map file %s",fname);
    return;			// Need a success/failure argument?
  }
  string line;

  Int_t roc=0;
  Int_t nsubadd=0;
  Int_t mask=0;
  Int_t bsub=0;
  Int_t detector=0;
  Int_t slot=0;

  while(getline(ifile,line)) {
    // BLank line or comment
    if(line.empty()
       || (start = line.find_first_not_of( whtspc )) == string::npos
       || IsComment(line, start) )
      continue;
  }
  // Get rid of trailing comments and leading and trailing whitespace
  cout << "MAPA: " << line << endl;
  while ((pos = line.find_first_of("!", pos+1)) != string::npos) {
    if(IsComment(line, pos)) {
      line.erase(pos);
      break;
    }
  }
  cout << "MAPB: " << line << endl;

  // Decide if line is ROC/NSUBADD/MASK/BSUB/DETECTOR/SLOT = something
  // or chan, plane, counter[, signal]

  
  if((pos=line.find_first_of("=")) != string::npos) { // Setting parameter
    char varname[100];
    
    
  } else {			// Assume channel definition

  }

}

