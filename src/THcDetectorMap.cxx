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

//_____________________________________________________________________________
THcDetectorMap::THcDetectorMap() : fNchans(0)
{
}

//_____________________________________________________________________________
THcDetectorMap::~THcDetectorMap()
{
}

bool THcDetectorMap::compare(const ChaninMod *first, const ChaninMod *second) {
      return((first->channel < second->channel)? true: false);
}
struct Functor
{
  bool operator() (const THcDetectorMap::ChaninMod &first, const THcDetectorMap::ChaninMod &second)
  { return((first.channel < second.channel)? true: false);}
};
//_____________________________________________________________________________
Int_t THcDetectorMap::FillMap(THaDetMap *detmap, const char *detectorname)
// Should probably return a status
{
  list<ModChanList>::iterator imod;
  ChaninMod Achan;
  ModChanList Amod;

  // Need one array of uniq crate/slot combos

  // Translate detector name into and ID
  Int_t did=3;  // Get this from detectorname
  mlist.clear();

  for(Int_t ich=0;ich<fNchans;ich++) {
    if(fTable[ich].did == did) {
      Int_t roc=fTable[ich].roc;
      Int_t slot=fTable[ich].slot;
      for(imod=mlist.begin(); imod!= mlist.end(); ++imod) {
	Achan.channel = fTable[ich].channel;
	Achan.plane = fTable[ich].plane;
	Achan.counter = fTable[ich].counter;
	Achan.signal = fTable[ich].signal;
	if((*imod).roc == roc && (*imod).slot == slot) {
	  (*imod).clist.push_back(Achan);
	}
      }
      if(imod == mlist.end()) {
	Amod.roc = roc;
	Amod.slot = slot;
	Amod.clist.clear();
	Amod.clist.push_back(Achan);
	mlist.push_back(Amod);
      }
    }
  }
  if(mlist.size() <= 0) {
    return(-1);
  }
  Functor f;
  for(imod=mlist.begin(); imod!= mlist.end(); ++imod) {
    list<ChaninMod> *clistp = &((*imod).clist);
    clistp->sort(f);//Sort by channel
  }
    
  return(0);
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

  fNchans = 0;

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

      fTable[fNchans].roc=roc;
      fTable[fNchans].roc=slot;
      fTable[fNchans].channel=channel;
      fTable[fNchans].did=detector;
      fTable[fNchans].plane=plane;
      fTable[fNchans].counter=counter;
      fTable[fNchans].signal=signal;

      fNchans++;
    }
  }

}

