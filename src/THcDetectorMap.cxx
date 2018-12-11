/** \class THcDetectorMap
    \ingroup Base

\brief Class to read and hold a Hall C style detector map

FillMap method builds a map for a specific detector

\author S. A. Wood

*/
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
THcDetectorMap::THcDetectorMap() : hcana::ConfigLogging<TObject>(), fNchans(0), fNIDs(0)
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
{
/**
  \param detmap destination of DAQ hardware to detector element map
  \param name of the detector

  Called be each detector object to build a DAQ hardware to detector
  element map for the detector.
*/

  list<ModChanList>::iterator imod;
  list<ChaninMod>::iterator ichan;
  ChaninMod Achan;
  ModChanList Amod;

  // Translate detector name into and ID
  // For now just long if then else.  Could get it from the comments
  // at the beginning of the map file.
  Int_t did=-1;
  for(Int_t i=0; i < fNIDs; i++) {
    if(strcasecmp(detectorname,fIDMap[i].name) == 0) {
      did = fIDMap[i].id;
      break;
    }
  }
  if(did < 0) {
    cout << "FillMap Error: No detector ID registered for " << detectorname << endl;
    cout << "     Using detector id of 0" << endl;
    did = 0;
  }

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
      Achan.refchan = fTable[ich].refchan;
      Achan.refindex = fTable[ich].refindex;
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
//  if(mlist.size() <= 0) {
  if(mlist.empty()) {
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
    Int_t last_refchan = -1;
    Int_t last_refindex = -1;
    for(ichan=clistp->begin(); ichan!=clistp->end(); ++ichan) {
      Int_t this_chan = (*ichan).channel;
      Int_t this_counter = (*ichan).counter;
      Int_t this_signal = (*ichan).signal;
      Int_t this_plane = (*ichan).plane;
      Int_t this_refchan = (*ichan).refchan;
      Int_t this_refindex = (*ichan).refindex;
      if(last_chan+1!=this_chan || last_counter+1 != this_counter
	 || last_plane != this_plane || last_signal!=this_signal
	 || last_refchan != this_refchan || last_refindex != this_refindex) {
	if(last_chan >= 0) {
	  if(ichan != clistp->begin()) {
	    //	    cout << "AddModule " << slot << " " << first_chan <<
	    //  " " << last_chan << " " << first_counter << endl;
	    detmap->AddModule((UShort_t)roc, (UShort_t)slot,
			      (UShort_t)first_chan, (UShort_t)last_chan,
			      (UInt_t) first_counter, model, (Int_t) last_refindex,
			      (Int_t) last_refchan, (UInt_t)last_plane, (UInt_t)last_signal);
	  }
	}
	first_chan = this_chan;
	first_counter = this_counter;
      }
      last_chan = this_chan;
      last_refchan = this_refchan;
      last_refindex = this_refindex;
      last_counter = this_counter;
      last_plane = this_plane;
      last_signal = this_signal;
      //      cout << "  Channel " << (*ichan).channel << " " <<
      //	(*ichan).plane << " " <<  (*ichan).counter << endl;
    }
    detmap->AddModule((UShort_t)roc, (UShort_t)slot,
		      (UShort_t)first_chan, (UShort_t)last_chan,
		      (UInt_t) first_counter, model, (Int_t) last_refindex,
		      (Int_t) last_refchan, (UInt_t)last_plane, (UInt_t)last_signal);
  }

  return(0);
}

//_____________________________________________________________________________
void THcDetectorMap::Load(const char *fname)
{
  /**
  \param fname name of file containing ENGINE style detector map

  Load a Hall C ENGINE style detector map file.  The map file maps
  a given roc, slot/module, and channel # into a given detector id#, plane
  number, counter number and signal type.  The mapping between detector
  names and ids is found in the comments at the begging of the map file.
  This method looks for those comments, of the form:
  ~~~~
    ! XXX_ID = n
  ~~~~
  to establish that mapping between detector name and detector ID.

  Lines of the form
  ~~~~
      DETECTOR = n
      ROC = n
      SLOT = n [, subadd]
 ~~~~
 are used to establish the module (roc and slot) and the detector
 for the mapping lines that follow.  If a SLOT line has a second value,
 that value is the channel of that slot that contains a reference time.

 The actual mappings are of the form
~~~~
      subadd, plane, counter [, signal]
~~~~
 Each of these lines, combined with the detector, roc, slot values
 establish the roc, slot, subadess -> detector, plane, counter#, sigtype map
 Other lines that may be in the map file are
~~~~
      NSUBADD = n
      BSUB = n
      MASK = hex value
~~~~
 These define characteristics of the electronics module (# channels,
 The bit number specifying the location of the subaddress in a data word
 and hex mask that the data word is anded with to retrieve data)

 A channel giving a reference time for a given slot can be set by putting
~~~~
     REFCHAN = subadd
~~~~
 after a SLOT line.  (The above SLOT=n,subadd is deprecated).  The line:
~~~~
     REFINDEX = index
~~~~
 indicates that all following mapping definition use the index'th reference
 time.  It must be given after each SLOT command.  A channel for a reference
 time is given by specifying a plane number of at least 1000 in a mapping line
~~~~
     subadd, 1000, 0, index
~~~~
 The reference time may be used by any channel of the same detector in the
 same ROC.  If a reference time is used by several different detectors, this
 mapping line must be duplicated for each detector.  More than one reference
 time may be specified per ROC by mulitple values for index.
*/

  static const char* const whtspc = " \t";

  ifstream ifile;

  ifile.open(fname);
  if(!ifile.is_open()) {
    static const char* const here = "THcDetectorMap::Load";
    Error(here, "error opening detector map file %s",fname);
    return;			// Need a success/failure argument?
  }
  string line;

  Int_t roc=0;
  Int_t nsubadd=0;
  //  Int_t mask=0;
  Int_t bsub=0;
  Int_t detector=0;
  Int_t slot=0;
  Int_t refchan=-1;
  Int_t refindex=-1;
  Int_t model=0;

  fNchans = 0;

  string::size_type start, pos=0;

  char varname[100];

  while(getline(ifile,line)) {
    // BLank line or comment
    if(line.empty()) continue;
    if((start = line.find_first_not_of( whtspc )) == string::npos) continue;
    if(IsComment(line, start)) { // Check for ID assignments
      // Get rid of all white space
      while((pos=line.find_first_of(whtspc)) != string::npos) {
	line.erase(pos,1);
      }
      line.erase(0,1);	// Erase "!"
      if(! ((pos=line.find("_ID=")) == string::npos)) {
	string::size_type llen = line.length();
	fIDMap[fNIDs].name = new char [pos+1];
	strncpy(fIDMap[fNIDs].name,line.c_str(),pos);
	fIDMap[fNIDs].name[pos] = '\0';
	start = (pos += 4); // Move to after "="
	while(pos < llen) {
	  if(isdigit(line.at(pos))) {
	    pos++;
	  } else {
	    break;
	  }
	}
	fIDMap[fNIDs++].id = atoi(line.substr(start,pos-start).c_str());
      }
      continue;
    }

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
      size_t valuestartpos = pos+1;
      size_t commapos = line.find_first_of(",");
      Int_t value;
      Int_t value2 = -1;
      if(commapos != string::npos) {
	//	cout << line.substr(valuestartpos,commapos-valuestartpos) << "|"
	//	     << line.substr(commapos+1) << "|" << endl;
	value = atoi(line.substr(valuestartpos,commapos-valuestartpos).c_str());
	value2 = atoi(line.substr(commapos+1).c_str());
      } else {
	value = atoi(line.substr(valuestartpos).c_str());
      }
      // Some if statements
      if(strcasecmp(varname,"detector")==0) {
	detector = value;
	refindex = -1;		// New detector resets ref time info
	refchan = -1;
      } else if (strcasecmp(varname,"roc")==0) {
	roc = value;
	refindex = -1;		// New roc resets ref time info
	refchan = -1;
      } else if (strcasecmp(varname,"nsubadd")==0) {
	nsubadd = value;
      } else if (strcasecmp(varname,"mask")==0) { // mask not used here
	//mask = value;
      } else if (strcasecmp(varname,"bsub")==0) {
	bsub = value;
      } else if (strcasecmp(varname,"slot")==0) {
	slot = value;
	refchan = value2;  	// Deprecating this
	refindex = -1;
      } else if (strcasecmp(varname,"refchan")==0) {
	refchan = value;	// Applies to just current slot
      } else if (strcasecmp(varname,"refindex")==0) {
	refindex = value;	// Applies to just current slot
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
      delete vararr;		// Discard result of Tokenize

      fTable[fNchans].roc=roc;
      fTable[fNchans].slot=slot;
      fTable[fNchans].refchan=refchan;
      fTable[fNchans].refindex=refindex;
      fTable[fNchans].channel=channel;
      fTable[fNchans].did=detector;
      fTable[fNchans].plane=plane;
      fTable[fNchans].counter=counter;
      fTable[fNchans].signal=signal;
      fTable[fNchans].model=model;

      fNchans++;
    }
  }
  _logger->info("Detector ID Map");
  //cout << endl << " Detector ID Map" << endl << endl;

  for(Int_t i=0; i < fNIDs; i++) {
    //cout << "   ";
    //cout << fIDMap[i].name << " " << fIDMap[i].id << endl;
  _logger->info("   {:<10} {:<10} ", fIDMap[i].name ,fIDMap[i].id );
  }
  //cout << endl;

}
