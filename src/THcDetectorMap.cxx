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
  // This one not used, but we get a link error if we don't include
  // a compare method
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
  list<ChaninMod>::iterator ichan;
  ChaninMod Achan;
  ModChanList Amod;

  // Translate detector name into and ID
  // For now just long if then else.  Could get it from the comments
  // at the beginning of the map file.
  Int_t did;
  if(strcasecmp(detectorname,"HDC")==0) {
    did = 1;
  } else if (strcasecmp(detectorname,"HSCIN")==0) {
    did = 2;
  } else if (strcasecmp(detectorname,"HCER")==0) {
    did = 3;
  } else if (strcasecmp(detectorname,"HCAL")==0) {
    did = 4;
  } else if (strcasecmp(detectorname,"HMISC")==0) {
    did = 5;
  } else if (strcasecmp(detectorname,"GMISC")==0) {
    did = 6;
  } else if (strcasecmp(detectorname,"HAERO")==0) {
    did = 7;
  } else {
    did = 0;
  }
  // Start SHMS with S?  What about SOS?

  mlist.clear();
  //  cout << "fNchans=" << fNchans << endl;
  for(Int_t ich=0;ich<fNchans;ich++) {
    if(fTable[ich].did == did) {
      Int_t roc=fTable[ich].roc;
      Int_t slot=fTable[ich].slot;
      Int_t model=fTable[ich].model;
      //      cout << "ROC=" << fTable[ich].roc << "  SLOT=" << fTable[ich].slot
      //	   << " CHANNEL=" << fTable[ich].channel << endl;
      Achan.channel = fTable[ich].channel;
      Achan.plane = fTable[ich].plane;
      Achan.counter = fTable[ich].counter;
      Achan.signal = fTable[ich].signal;
      for(imod=mlist.begin(); imod!= mlist.end(); ++imod) {
	if((*imod).roc == roc && (*imod).slot == slot) {
	  //	  cout << "Pushing chan " << Achan.channel << " to " << roc
	  //	       << " " << slot << endl;
	  (*imod).clist.push_back(Achan);
	  break;
	}
      }
      if(imod == mlist.end()) {
	Amod.roc = roc;
	Amod.slot = slot;
	Amod.model = model;
	Amod.clist.clear();
	Amod.clist.push_back(Achan);
	mlist.push_back(Amod);
	// 	cout << "New module  " << Achan.channel << " to " << roc
	//	     << " " << slot << endl;
      }
    }
  }
  if(mlist.size() <= 0) {
    return(-1);
  }
  Functor f;
  for(imod=mlist.begin(); imod!= mlist.end(); ++imod) {
    //    cout << "Slot " << (*imod).slot << endl;
    list<ChaninMod> *clistp = &((*imod).clist);
    clistp->sort(f);//Sort by channel
  }
  // Copy the information to the Hall A style detector map
  // grouping consecutive channels that are all the same plane
  // and signal type
  for(imod=mlist.begin(); imod!= mlist.end(); ++imod) {
    UShort_t roc = (*imod).roc;
    UShort_t slot = (*imod).slot;
    UInt_t model=(*imod).model;
    //    cout << "Slot " << slot << endl;
    list<ChaninMod> *clistp = &((*imod).clist);
    Int_t first_chan = -1;
    Int_t last_chan = -1;
    Int_t last_plane = -1;
    Int_t last_signal = -1;
    Int_t first_counter = -1;
    Int_t last_counter = -1;
    for(ichan=clistp->begin(); ichan!=clistp->end(); ++ichan) {
      Int_t this_chan = (*ichan).channel;
      Int_t this_counter = (*ichan).counter;
      Int_t this_signal = (*ichan).signal;
      Int_t this_plane = (*ichan).plane;
      if(last_chan+1!=this_chan || last_counter+1 != this_counter 
	 || last_plane != this_plane || last_signal!=this_signal) {
	if(last_chan >= 0) {
	  if(ichan != clistp->begin()) {
	    //	    cout << "AddModule " << slot << " " << first_chan << 
	    //  " " << last_chan << " " << first_counter << endl;
	    detmap->AddModule((UShort_t)roc, (UShort_t)slot,
			      (UShort_t)first_chan, (UShort_t)last_chan, 
			      (UInt_t) first_counter, model, (Int_t) 0,
			      (Int_t) -1, (UInt_t)last_plane, (UInt_t)last_signal);
	  }
	}
	first_chan = this_chan;
	first_counter = this_counter;
      }
      last_chan = this_chan;
      last_counter = this_counter;
      last_plane = this_plane;
      last_signal = this_signal;
      //      cout << "  Channel " << (*ichan).channel << " " <<
      //	(*ichan).plane << " " <<  (*ichan).counter << endl;
    }
    detmap->AddModule((UShort_t)roc, (UShort_t)slot,
		      (UShort_t)first_chan, (UShort_t)last_chan, 
		      (UInt_t) first_counter, model, (Int_t) 0,
		      (Int_t) -1, (UInt_t)last_plane, (UInt_t)last_signal);
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
  Int_t model=0;

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
      if(nsubadd == 96) {
	model = 1877;
      } else if (nsubadd == 64) {
	if(bsub == 16) {
	  model = 1872;
	} else if(bsub == 17) {
	  model = 1881;
	} else {
	  model = 0;
	}
      } else {
	model = 0;
      }
    } else {			// Assume channel definition
      TString values(line.c_str());
      TObjArray *vararr = values.Tokenize(",");
      Int_t nvals = vararr->GetLast()+1;
      if(nvals<3 || nvals>4) {
	if(nvals > 1) {	// Silent for help, noecho, nodebug, override
	  cout << "Map file: Invalid value count: " << line << endl;
	}
	continue;
      }
      Int_t channel = ((TObjString*)vararr->At(0))->GetString().Atoi();
      Int_t plane = ((TObjString*)vararr->At(1))->GetString().Atoi();
      Int_t counter = ((TObjString*)vararr->At(2))->GetString().Atoi();
      Int_t signal = 0;
      if(nvals==4) {
	signal= ((TObjString*)vararr->At(3))->GetString().Atoi();
      }

      fTable[fNchans].roc=roc;
      fTable[fNchans].slot=slot;
      fTable[fNchans].channel=channel;
      fTable[fNchans].did=detector;
      fTable[fNchans].plane=plane;
      fTable[fNchans].counter=counter;
      fTable[fNchans].signal=signal;
      fTable[fNchans].model=model;

      fNchans++;
    }
  }

}

