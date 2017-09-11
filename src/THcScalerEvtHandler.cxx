/** \class THcScalerEvtHandler
    \ingroup Base

\brief Event handler for Hall C scalers


This class does the following

For a particular set of event types (here, event type 0)
decode the scalers and put some variables into global variables.
The global variables can then appear in the Podd output tree T.
In addition, a tree "TS" is created by this class; it contains
just the scaler data by itself.  Note, the "fName" is concatenated
with "TS" to ensure the tree is unqiue; further, "fName" is
concatenated with the name of the global variables, for uniqueness.
The list of global variables and how they are tied to the
scaler module and channels is defined here; eventually this
will be modified to use a scaler.map file

NOTE: if you don't have the scaler map file (e.g. Leftscalevt.map)
there will be no variable output to the Trees.

To use in the analyzer, your setup script needs something like this
~~~
     gHaEvtHandlers->Add (new THcScalerEvtHandler("HMS","HC scaler event type 0"));
~~~
To enable debugging you may try this in the setup script
~~~
     THcScalerEvtHandler *hscaler = new THcScalerEvtHandler("HS","HC scaler event type 0");
     hscaler->SetDebugFile("HScaler.txt");
     gHaEvtHandlers->Add (hscaler);
~~~
\author  E. Brash based on THaScalerEvtHandler by R. Michaels
*/

#include "THaEvtTypeHandler.h"
#include "THcScalerEvtHandler.h"
#include "GenScaler.h"
#include "Scaler3800.h"
#include "Scaler3801.h"
#include "Scaler1151.h"
#include "Scaler560.h"
#include "THaCodaData.h"
#include "THaEvData.h"
#include "TNamed.h"
#include "TMath.h"
#include "TString.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <iterator>
#include "THaVarList.h"
#include "VarDef.h"

using namespace std;
using namespace Decoder;

static const UInt_t ICOUNT    = 1;
static const UInt_t IRATE     = 2;
static const UInt_t MAXCHAN   = 32;
static const UInt_t defaultDT = 4;

THcScalerEvtHandler::THcScalerEvtHandler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description), evcount(0), ifound(0), fNormIdx(-1),
    dvars(0), dvarsFirst(0), fScalerTree(0), fUseFirstEvent(kFALSE),
    fDelayedType(-1)
{
}

THcScalerEvtHandler::~THcScalerEvtHandler()
{
  if (fScalerTree) {
    delete fScalerTree;
  }
}

Int_t THcScalerEvtHandler::End( THaRunBase* r)
{
  // Process any delayed events in order received

  cout << "THcScalerEvtHandler::End Analyzing " << fDelayedEvents.size() << " delayed scaler events" << endl;
  for(UInt_t* rdata : fDelayedEvents) {
    AnalyzeBuffer(rdata);
  }
  fDelayedEvents.clear();	// Does this free the arrays?

  if (fScalerTree) fScalerTree->Write();
  return 0;
}

void THcScalerEvtHandler::SetDelayedType(int evtype) {
  /**
   * \brief Delay analysis of this event type to end.
   *
   * Final scaler events generated in readout list end routines may not
   * come in order in the data stream.  If the event type of a end routine
   * scaler event is set, then the event contents will be saved and analyzed
   * at the end of the analysis so that time ordering of scaler events is preserved.
   */
  fDelayedType = evtype;
}
  
Int_t THcScalerEvtHandler::Analyze(THaEvData *evdata)
{
  Int_t lfirst=1;

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebugFile) {
    *fDebugFile << endl << "---------------------------------- "<<endl<<endl;
    *fDebugFile << "\nEnter THcScalerEvtHandler  for fName = "<<fName<<endl;
    EvDump(evdata);
  }

  if (lfirst && !fScalerTree) {

    lfirst = 0; // Can't do this in Init for some reason

    TString sname1 = "TS";
    TString sname2 = sname1 + fName;
    TString sname3 = fName + "  Scaler Data";

    if (fDebugFile) {
      *fDebugFile << "\nAnalyze 1st time for fName = "<<fName<<endl;
      *fDebugFile << sname2 << "      " <<sname3<<endl;
    }

    fScalerTree = new TTree(sname2.Data(),sname3.Data());
    fScalerTree->SetAutoSave(200000000);

    TString name, tinfo;

    name = "evcount";
    tinfo = name + "/D";
    fScalerTree->Branch(name.Data(), &evcount, tinfo.Data(), 4000);

    for (size_t i = 0; i < scalerloc.size(); i++) {
      name = scalerloc[i]->name;
      tinfo = name + "/D";
      fScalerTree->Branch(name.Data(), &dvars[i], tinfo.Data(), 4000);
    }

  }  // if (lfirst && !fScalerTree)

  UInt_t *rdata = (UInt_t*) evdata->GetRawDataBuffer();

  if( evdata->GetEvType() == fDelayedType) { // Save this event for processing later
    Int_t evlen = evdata->GetEvLength();
    UInt_t *datacopy = new UInt_t[evlen];
    fDelayedEvents.push_back(datacopy);
    for(Int_t i=0;i<evlen;i++) {
      datacopy[i] = rdata[i];
    }
    return 1;
  } else { 			// A normal event
    if (fDebugFile) *fDebugFile<<"\n\nTHcScalerEvtHandler :: Debugging event type "<<dec<<evdata->GetEvType()<<endl<<endl;
    return AnalyzeBuffer(rdata);
  }

}
Int_t THcScalerEvtHandler::AnalyzeBuffer(UInt_t* rdata)
{

  // Parse the data, load local data arrays.
  UInt_t *p = (UInt_t*) rdata;

  UInt_t *plast = p+*p;		// Index to last word in the bank

  ifound=0;
  while(p<plast) {
    p++;			  // point to header
    if (fDebugFile) {
      *fDebugFile << "Bank: " << hex << *p << dec << " len: " << *(p-1) << endl;
    }
    if((*p & 0xff00) == 0x1000) {	// Bank Containing banks
      p++;				// Now pointing to a bank in the bank
    } else if (((*p & 0xff00) == 0x100) && (*p != 0xC0000100)) {
      // Bank containing integers.  Look for scaler data
      // Assume that very first word is a scaler header
      // At any point in the bank where the word is not a matching
      // header, we stop.
      UInt_t *pnext = p+*(p-1);	// Next bank
      p++;			// First data word

      // Look for normalization scaler module first.
      if(fNormIdx >= 0) {
	UInt_t *psave = p;
	while(p < pnext) {
	  if(scalers[fNormIdx]->IsSlot(*p)) {
	    scalers[fNormIdx]->Decode(p);
	    ifound = 1;
	    break;
	  }
	  p += scalers[fNormIdx]->GetNumChan() + 1;
	}
	p = psave;
      }
      while(p < pnext) {
	Int_t nskip = 0;
	if(fDebugFile) {
	  *fDebugFile << "Scaler Header: " << hex << *p << dec;
	}
	for(size_t j=0; j<scalers.size(); j++) {
	  if(scalers[j]->IsSlot(*p)) {
	    nskip = scalers[j]->GetNumChan() + 1;
	    if((Int_t) j != fNormIdx) {
	      if(fDebugFile) {
		*fDebugFile << " found (" << j << ")  skip " << nskip << endl;
	      }
	      scalers[j]->Decode(p);
	      ifound = 1;
	    }
	    break;
	  }
	}
	if(nskip == 0) {
	  if(fDebugFile) {
	    *fDebugFile << endl;
	  }
	  break;	// Didn't find a matching header
	}
	p = p + nskip;
      }
      p = pnext;
    } else {
      p = p+*(p-1);		// Skip to next bank
    }
  }

  if (fDebugFile) {
    *fDebugFile << "Finished with decoding.  "<<endl;
    *fDebugFile << "   Found flag   =  "<<ifound<<endl;
  }

  // HMS has headers which are different from SOS, but both are
  // event type 0 and come here.  If you found no headers, return.

  if (!ifound) return 0;

  // The correspondance between dvars and the scaler and the channel
  // will be driven by a scaler.map file  -- later

  for (size_t i = 0; i < scalerloc.size(); i++) {
    size_t ivar = scalerloc[i]->ivar;
    size_t isca = scalerloc[i]->iscaler;
    size_t ichan = scalerloc[i]->ichan;
    if (evcount==0) {
    	if (fDebugFile) *fDebugFile << "Debug dvarsFirst "<<i<<"   "<<ivar<<"  "<<isca<<"  "<<ichan<<endl;
    	if ((ivar < scalerloc.size()) &&
		(isca < scalers.size()) &&
		(ichan < MAXCHAN)) {
                    if(fUseFirstEvent) {
                        if (scalerloc[ivar]->ikind == ICOUNT) {
                                dvars[ivar] = scalers[isca]->GetData(ichan);
                                dvarsFirst[ivar] = 0;
                        }
          	        if (scalerloc[ivar]->ikind == IRATE) {
                                dvars[ivar] = scalers[isca]->GetRate(ichan);
                                dvarsFirst[ivar] = dvars[ivar];
                        }
          		if (fDebugFile) *fDebugFile << "   dvarsFirst  "<<scalerloc[ivar]->ikind<<"  "<<dvarsFirst[ivar]<<endl;

                    } else {
      			if (scalerloc[ivar]->ikind == ICOUNT) dvarsFirst[ivar] = scalers[isca]->GetData(ichan);
      			if (scalerloc[ivar]->ikind == IRATE)  dvarsFirst[ivar] = scalers[isca]->GetRate(ichan);
      			if (fDebugFile) *fDebugFile << "   dvarsFirst  "<<scalerloc[ivar]->ikind<<"  "<<dvarsFirst[ivar]<<endl;
                    }
        } else {
      			cout << "THcScalerEvtHandler:: ERROR:: incorrect index "<<ivar<<"  "<<isca<<"  "<<ichan<<endl;
        }
    }else{
    	if (fDebugFile) *fDebugFile << "Debug dvars "<<i<<"   "<<ivar<<"  "<<isca<<"  "<<ichan<<endl;
    	if ((ivar < scalerloc.size()) &&
		(isca < scalers.size()) &&
		(ichan < MAXCHAN)) {
      			if (scalerloc[ivar]->ikind == ICOUNT) dvars[ivar] = scalers[isca]->GetData(ichan)-dvarsFirst[ivar];
      			if (scalerloc[ivar]->ikind == IRATE)  dvars[ivar] = scalers[isca]->GetRate(ichan);
      			if (fDebugFile) *fDebugFile << "   dvars  "<<scalerloc[ivar]->ikind<<"  "<<dvars[ivar]<<endl;
    	} else {
      			cout << "THcScalerEvtHandler:: ERROR:: incorrect index "<<ivar<<"  "<<isca<<"  "<<ichan<<endl;
    	}
    }
  }

  evcount = evcount + 1.0;

  for (size_t j=0; j<scalers.size(); j++) scalers[j]->Clear("");

  if (fDebugFile) *fDebugFile << "scaler tree ptr  "<<fScalerTree<<endl;

  if (fScalerTree) fScalerTree->Fill();

  return 1;
}


THaAnalysisObject::EStatus THcScalerEvtHandler::Init(const TDatime& date)
{
  const int LEN = 200;
  char cbuf[LEN];

  fStatus = kOK;
  fNormIdx = -1;

  fDelayedEvents.clear();

  cout << "Howdy !  We are initializing THcScalerEvtHandler !!   name =   "
        << fName << endl;

  if(eventtypes.size()==0) {
    eventtypes.push_back(0);  // Default Event Type
  }

  TString dfile;
  dfile = fName + "scaler.txt";

// Parse the map file which defines what scalers exist and the global variables.

  TString sname0 = "Scalevt";
  TString sname;
  sname = fName+sname0;

  FILE *fi = OpenFile(sname.Data(), date);
  if ( !fi ) {
    cout << "Cannot find db file for "<<fName<<" scaler event handler"<<endl;
    return kFileError;
  }

  size_t minus1 = -1;
  size_t pos1;
  string scomment = "#";
  string svariable = "variable";
  string smap = "map";
  vector<string> dbline;

  while( fgets(cbuf, LEN, fi) != NULL) {
    std::string sinput(cbuf);
    if (fDebugFile) *fDebugFile << "string input "<<sinput<<endl;
    dbline = vsplit(sinput);
    if (dbline.size() > 0) {
      pos1 = FindNoCase(dbline[0],scomment);
      if (pos1 != minus1) continue;
      pos1 = FindNoCase(dbline[0],svariable);
      if (pos1 != minus1 && dbline.size()>4) {
	string sdesc = "";
	for (size_t j=5; j<dbline.size(); j++) sdesc = sdesc+" "+dbline[j];
	Int_t isca = atoi(dbline[1].c_str());
	Int_t ichan = atoi(dbline[2].c_str());
	Int_t ikind = atoi(dbline[3].c_str());
	if (fDebugFile)
	  *fDebugFile << "add var "<<dbline[1]<<"   desc = "<<sdesc<<"    isca= "<<isca<<"  "<<ichan<<"  "<<ikind<<endl;
	TString tsname(dbline[4].c_str());
	TString tsdesc(sdesc.c_str());
	AddVars(tsname,tsdesc,isca,ichan,ikind);
      }
      pos1 = FindNoCase(dbline[0],smap);
      if (fDebugFile) *fDebugFile << "map ? "<<dbline[0]<<"  "<<smap<<"   "<<pos1<<"   "<<dbline.size()<<endl;
      if (pos1 != minus1 && dbline.size()>6) {
	Int_t imodel, icrate, islot, inorm;
	UInt_t header, mask;
	char cdum[20];
	sscanf(sinput.c_str(),"%s %d %d %d %x %x %d \n",cdum,&imodel,&icrate,&islot, &header, &mask, &inorm);
	if ((fNormIdx >= 0) && (fNormIdx != inorm)) cout << "THcScalerEvtHandler::WARN:  contradictory norm index  "<<fNormIdx<<"   "<<inorm<<endl;
	Int_t clkchan = -1;
	Double_t clkfreq = 1;
	if (dbline.size()>8) {
	  clkchan = atoi(dbline[7].c_str());
	  clkfreq = 1.0*atoi(dbline[8].c_str());
	}
	if (fDebugFile) {
	  *fDebugFile << "map line "<<dec<<imodel<<"  "<<icrate<<"  "<<islot<<endl;
	  *fDebugFile <<"   header  0x"<<hex<<header<<"  0x"<<mask<<dec<<"  "<<inorm<<"  "<<clkchan<<"  "<<clkfreq<<endl;
	}
	switch (imodel) {
	case 560:
	  scalers.push_back(new Scaler560(icrate, islot));
	  break;
	case 1151:
	  scalers.push_back(new Scaler1151(icrate, islot));
	  break;
	case 3800:
	  scalers.push_back(new Scaler3800(icrate, islot));
	  break;
	case 3801:
	  scalers.push_back(new Scaler3801(icrate, islot));
	  break;
	}
	if (scalers.size() > 0) {
	  UInt_t idx = scalers.size()-1;
	  // Headers must be unique over whole event, not
	  // just within a ROC
	  scalers[idx]->SetHeader(header, mask);
	  if (clkchan >= 0) {
		  scalers[idx]->SetClock(defaultDT, clkchan, clkfreq);
		  cout << "Setting scaler clock ... channel = "<<clkchan<<" ... freq = "<<clkfreq<<endl;
		  if (fDebugFile) *fDebugFile <<"Setting scaler clock ... channel = "<<clkchan<<" ... freq = "<<clkfreq<<endl;
		  fNormIdx = idx;
	  }
	}
      }
    }
  }
  // can't compare UInt_t to Int_t (compiler warning), so do this
  nscalers=0;
  for (size_t i=0; i<scalers.size(); i++) nscalers++;
  // need to do LoadNormScaler after scalers created and if fNormIdx found
  if (fDebugFile) *fDebugFile <<"fNormIdx = "<<fNormIdx<<endl;
  if ((fNormIdx >= 0) && fNormIdx < nscalers) {
    for (Int_t i = 0; i < nscalers; i++) {
      if (i==fNormIdx) continue;
      scalers[i]->LoadNormScaler(scalers[fNormIdx]);
    }
  }

#ifdef HARDCODED
  // This code is superseded by the parsing of a map file above.  It's another way ...
  if (fName == "Left") {
    AddVars("TSbcmu1", "BCM x1 counts", 1, 4, ICOUNT);
    AddVars("TSbcmu1r","BCM x1 rate",  1, 4, IRATE);
    AddVars("TSbcmu3", "BCM u3 counts", 1, 5, ICOUNT);
    AddVars("TSbcmu3r", "BCM u3 rate",  1, 5, IRATE);
  } else {
    AddVars("TSbcmu1", "BCM x1 counts", 0, 4, ICOUNT);
    AddVars("TSbcmu1r","BCM x1 rate",  0, 4, IRATE);
    AddVars("TSbcmu3", "BCM u3 counts", 0, 5, ICOUNT);
    AddVars("TSbcmu3r", "BCM u3 rate",  0, 5, IRATE);
  }
#endif


  DefVars();

#ifdef HARDCODED
  // This code is superseded by the parsing of a map file above.  It's another way ...
  if (fName == "Left") {
    scalers.push_back(new Scaler1151(1,0));
    scalers.push_back(new Scaler3800(1,1));
    scalers.push_back(new Scaler3800(1,2));
    scalers.push_back(new Scaler3800(1,3));
    scalers[0]->SetHeader(0xabc00000, 0xffff0000);
    scalers[1]->SetHeader(0xabc10000, 0xffff0000);
    scalers[2]->SetHeader(0xabc20000, 0xffff0000);
    scalers[3]->SetHeader(0xabc30000, 0xffff0000);
    scalers[0]->LoadNormScaler(scalers[1]);
    scalers[1]->SetClock(4, 7, 1024);
    scalers[2]->LoadNormScaler(scalers[1]);
    scalers[3]->LoadNormScaler(scalers[1]);
  } else {
    scalers.push_back(new Scaler3800(2,0));
    scalers.push_back(new Scaler3800(2,0));
    scalers.push_back(new Scaler1151(2,1));
    scalers.push_back(new Scaler1151(2,2));
    scalers[0]->SetHeader(0xceb00000, 0xffff0000);
    scalers[1]->SetHeader(0xceb10000, 0xffff0000);
    scalers[2]->SetHeader(0xceb20000, 0xffff0000);
    scalers[3]->SetHeader(0xceb30000, 0xffff0000);
    scalers[0]->SetClock(4, 7, 1024);
    scalers[1]->LoadNormScaler(scalers[0]);
    scalers[2]->LoadNormScaler(scalers[0]);
    scalers[3]->LoadNormScaler(scalers[0]);
  }
#endif

  if(fDebugFile) *fDebugFile << "THcScalerEvtHandler:: Name of scaler bank "<<fName<<endl;
  for (size_t i=0; i<scalers.size(); i++) {
    if(fDebugFile) {
      *fDebugFile << "Scaler  #  "<<i<<endl;
      scalers[i]->SetDebugFile(fDebugFile);
      scalers[i]->DebugPrint(fDebugFile);
    }
  }
  return kOK;
}

void THcScalerEvtHandler::AddVars(TString name, TString desc, Int_t iscal,
				  Int_t ichan, Int_t ikind)
{
  // need to add fName here to make it a unique variable.  (Left vs Right HRS, for example)
  TString name1 = fName + name;
  TString desc1 = fName + desc;
  HCScalerLoc *loc = new HCScalerLoc(name1, desc1, iscal, ichan, ikind);
  loc->ivar = scalerloc.size();  // ivar will be the pointer to the dvars array.
  scalerloc.push_back(loc);
}

void THcScalerEvtHandler::DefVars()
{
  // called after AddVars has finished being called.
  Nvars = scalerloc.size();
  if (Nvars == 0) return;
  dvars = new Double_t[Nvars];  // dvars is a member of this class
  dvarsFirst = new Double_t[Nvars];  // dvarsFirst is a member of this class
  memset(dvars, 0, Nvars*sizeof(Double_t));
  memset(dvarsFirst, 0, Nvars*sizeof(Double_t));
  if (gHaVars) {
    if(fDebugFile) *fDebugFile << "THcScalerEVtHandler:: Have gHaVars "<<gHaVars<<endl;
  } else {
    cout << "No gHaVars ?!  Well, that's a problem !!"<<endl;
    return;
  }
  if(fDebugFile) *fDebugFile << "THcScalerEvtHandler:: scalerloc size "<<scalerloc.size()<<endl;
  const Int_t* count = 0;
  for (size_t i = 0; i < scalerloc.size(); i++) {
    gHaVars->DefineByType(scalerloc[i]->name.Data(), scalerloc[i]->description.Data(),
			  &dvars[i], kDouble, count);
    //gHaVars->DefineByType(scalerloc[i]->name.Data(), scalerloc[i]->description.Data(),
    //			  &dvarsFirst[i], kDouble, count);
  }
}

size_t THcScalerEvtHandler::FindNoCase(const string& sdata, const string& skey)
{
  // Find iterator of word "sdata" where "skey" starts.  Case insensitive.
  string sdatalc, skeylc;
  sdatalc = "";  skeylc = "";
  for (string::const_iterator p =
	 sdata.begin(); p != sdata.end(); ++p) {
    sdatalc += tolower(*p);
  }
  for (string::const_iterator p =
	 skey.begin(); p != skey.end(); ++p) {
    skeylc += tolower(*p);
  }
  if (sdatalc.find(skeylc,0) == string::npos) return -1;
  return sdatalc.find(skeylc,0);
};

ClassImp(THcScalerEvtHandler)
