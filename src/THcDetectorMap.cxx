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
// Not sure we will keep this class, but still need the parsing of the map file
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetectorMap.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

ClassImp(THcDetectorMap)

inline static bool IsComment( const string& s, string::size_type pos )
{
  return ( pos != string::npos && pos < s.length() &&
	   (s[pos] == '!') );
}

virtual Int_t THcDetectorMap::FillMap(THaDetMap *detmap, string *detectorname) {
  Int_t detectorid=3;  // Get this from detectorname
  
  // Loop through the list looking for all hardware channels that
  //  are detectorid.  Sort and try to minimize AddModule calls
}

void THcDetectorMap::Load(const char *fname)
{
  static const char* const here = "THcDetectorMap::Load";
  static const char* const whtspc = " \t";

  ifstream ifile;

  ifile.open(fname);
  if(!ifile.is_open()) {
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

  string::size_type start, pos=0;

  char varname[100];

  while(getline(ifile,line)) {
    // BLank line or comment
    if(line.empty()
       || (start = line.find_first_not_of( whtspc )) == string::npos
       || IsComment(line, start) )
      continue;

    //    cout << "MAPA: " << line << endl;

    // Remove comment from line
    while ((pos = line.find_first_of("!", pos+1)) != string::npos) {
      if(IsComment(line, pos)) {
	line.erase(pos);
	break;
      }
    }

    // Get rid of all white space
    while((pos=line.find_first_of(whtspc)) != string::npos) {
      line.erase(pos,1);
    }

    //    cout << "MAPB: " << line << endl;

  // Decide if line is ROC/NSUBADD/MASK/BSUB/DETECTOR/SLOT = something
  // or chan, plane, counter[, signal]

  
    if((pos=line.find_first_of("=")) != string::npos) { // Setting parameter
      strcpy(varname, (line.substr(0,pos)).c_str());
      Int_t valuestartpos = pos+1;
      Int_t value = atoi(line.substr(valuestartpos).c_str());
      // Some if statements
      if(strcasecmp(varname,"detector")==0) {
	detector = value;
      } else if (strcasecmp(varname,"roc")==0) {
	roc = value;
      } else if (strcasecmp(varname,"nsubadd")==0) {
	nsubadd = value;
      } else if (strcasecmp(varname,"mask")==0) {
	mask = value;
      } else if (strcasecmp(varname,"bsub")==0) {
	bsub = value;
      } else if (strcasecmp(varname,"slot")==0) {
	slot = value;
      }
    } else {			// Assume channel definition
      TString values(line.c_str());
      TObjArray *vararr = values.Tokenize(",");
      Int_t nvals = vararr->GetLast()+1;
      if(nvals<2 || nvals>4) {
	cout << "Map file: Invalid value count: " << line << endl;
	continue;
      }
      Int_t channel = ((TObjString*)vararr->At(0))->GetString().Atoi();
      Int_t plane = ((TObjString*)vararr->At(1))->GetString().Atoi();
      Int_t counter = ((TObjString*)vararr->At(2))->GetString().Atoi();
      Int_t signal = 0;
      if(nvals==4) {
	signal= ((TObjString*)vararr->At(3))->GetString().Atoi();
      }
      cout << channel << " " << plane << " " << counter << " " << signal << endl;
      
    }
  }

}

