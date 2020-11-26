/** \class THcHelicityScaler
   \ingroup Base

\brief Event handler for Hall C helicity scalers

~~~
~~~
     THcHelcityScaler *hhelscaler = new THcHelicityScaler("H","HC helicity scalers");
     // hscaler->SetDebugFile("HHelScaler.txt");
     hhelscaler->SetROC(8);   // 5 for HMS defaults to 8 for SHMS
     hhelscaler->SetBankID(0x9801); // Will default to this
     gHaEvtHandlers->Add (hhelscaler);
~~~
\author  
*/

//#include "THaEvtTypeHandler.h"
#include "THcHelicityScaler.h"
#include "THaCodaData.h"
#include "THaEvData.h"
#include "THcGlobals.h"
#include "THaGlobals.h"
#include "THcParmList.h"
#include "THcHelicity.h"
#include "TNamed.h"
#include "TMath.h"
#include "TString.h"
#include "TROOT.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <bitset>
#include <iterator>
#include "THaVarList.h"
#include "VarDef.h"
#include "Helper.h"

using namespace std;
using namespace Decoder;

static const UInt_t ICOUNT    = 1;
static const UInt_t IRATE     = 2;
static const UInt_t ICURRENT = 3;
static const UInt_t ICHARGE   = 4;
static const UInt_t ITIME   = 5;
static const UInt_t ICUT = 6;
static const UInt_t MAXCHAN   = 32;
static const UInt_t defaultDT = 4;

THcHelicityScaler::THcHelicityScaler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description),
    fBankID(9801),
    fUseFirstEvent(kTRUE),
    fDelayedType(-1),
    fBCM_Gain(0), fBCM_Offset(0), fBCM_SatOffset(0), fBCM_SatQuadratic(0), fBCM_delta_charge(0),
    evcount(0), evcountR(0.0), ifound(0), fNormIdx(-1),
    fNormSlot(-1),
    dvars(0),dvars_prev_read(0), dvarsFirst(0), fScalerTree(0),
    fOnlySyncEvents(kFALSE), fOnlyBanks(kFALSE), fDelayedType(-1),
    fClockChan(-1), fLastClock(0), fClockOverflows(0)
{

  //-----C.Y. Sep 19, 2020: The constructor has been updated to initialize-----
  //the necessary variables to write the helicity to a scaler tree.

  fRocSet.clear();
  fModuleSet.clear();
  scal_prev_read.clear();
  scal_present_read.clear();
  scal_overflows.clear();   
  //---------------------------------------------------------------------------
  
  fROC=-1;
  fNScalerChannels = 32;

  AddEvtType(1);
  AddEvtType(2);
  AddEvtType(4);
  AddEvtType(5);
  AddEvtType(6);
  AddEvtType(7);
  SetDelayedType(129);
  
}

THcHelicityScaler::~THcHelicityScaler()
{

    /*
    C.Y. Sep 19, 2020: The destructor needs to be updated to delete the pointer variables that were initialized in the constructor..
   */
  
  delete [] fBCM_Gain;
  delete [] fBCM_Offset;

  for( vector<UInt_t*>::iterator it = fDelayedEvents.begin();
       it != fDelayedEvents.end(); ++it )
    delete [] *it;
  fDelayedEvents.clear();
}

Int_t THcHelicityScaler::End( THaRunBase* )
{
  // Process any delayed events in order received

  cout << "THcHelicityScaler::End Analyzing " << fDelayedEvents.size() << " delayed helicity scaler events" << endl;
    for(std::vector<UInt_t*>::iterator it = fDelayedEvents.begin();
      it != fDelayedEvents.end(); ++it) {
    UInt_t* rdata = *it;
    AnalyzeBuffer(rdata);
  }

  for( vector<UInt_t*>::iterator it = fDelayedEvents.begin();
       it != fDelayedEvents.end(); ++it )
    delete [] *it;
  fDelayedEvents.clear();

  //  cout << " -- Helicity Scalers -- " << endl;
  for(Int_t i=0;i<fNScalerChannels;i++) {
    if(fScalerSums[i]>0.5) {
      fAsymmetry[i] = (fHScalers[0][i]-fHScalers[1][i])/fScalerSums[i];
      fAsymmetryError[i] = 2*TMath::Sqrt(fHScalers[0][i]*fHScalers[1][i]
					*fScalerSums[i])
	/(fScalerSums[i]*fScalerSums[i]);
    } else {
      fAsymmetry[i] = 0.0;
      fAsymmetryError[i] = 0.0;
    }
    //    printf("%2d %12.0f %12.0f %12.0f %12.8f\n",i,fScalerSums[i],
    //	   fHScalers[0][i],fHScalers[1][i],
    //	   fAsymmetry[i]);
  }
  //  cout << " ---------------------- " << endl;

  // Compute Charge Asymmetries
  std::map<std::string, Int_t> bcmindex;
  bcmindex["BCM1"] = 0;
  bcmindex["BCM2"] = 2;
  //  bcmindex["Unser"] = 6;
  bcmindex["BCM4A"] = 10;
  bcmindex["BCM4B"] = 4;
  bcmindex["BCM4C"] = 12;
  //  bcmindex["1MHz"] = 8;
  Int_t clockindex=8;
  Double_t clockfreq=1000000.0;
  Double_t pclock = fHScalers[0][clockindex];
  Double_t mclock = fHScalers[1][clockindex];
  cout << " -- Beam Charge Asymmetries -- " << endl;
  for(Int_t i=0;i<fNumBCMs;i++) {
    if(bcmindex.find(fBCM_Name[i]) != bcmindex.end()) {
      Int_t index=bcmindex[fBCM_Name[i]];
      Double_t pcounts = fHScalers[0][index];
      Double_t mcounts = fHScalers[1][index];
      //      cout << index << " " << fBCM_Name[i] << " " << pcounts << " " << mcounts
      //	   << " " << fBCM_Gain[i]
      //      	   << " " << fBCM_Offset[i] << endl;
      Double_t pcharge = (pcounts - (pclock/clockfreq)*fBCM_Offset[i])
	/fBCM_Gain[i];
      Double_t mcharge = (mcounts - (mclock/clockfreq)*fBCM_Offset[i])
	/fBCM_Gain[i];
      fCharge[i] = pcharge+mcharge;
      if(fCharge[i]>0.0) {
	fChargeAsymmetry[i] = (pcharge-mcharge)/fCharge[i];
      } else {
	fChargeAsymmetry[i] = 0.0;
      }
      printf("%6s %12.2f %12.8f\n",fBCM_Name[i].c_str(),fCharge[i],fChargeAsymmetry[i]);
    }
  }
  fTime = (pclock+mclock)/clockfreq;
  if(pclock+mclock>0) {
    fTimeAsymmetry = (pclock-mclock)/(pclock+mclock);
  } else {
    fTimeAsymmetry = 0.0;
  }
  printf("TIME(s)%12.2f %12.8f\n",fTime,fTimeAsymmetry);
  if(fNTriggersPlus+fNTriggersMinus > 0) {
    fTriggerAsymmetry = ((Double_t) (fNTriggersPlus-fNTriggersMinus))/(fNTriggersPlus+fNTriggersMinus);
  } else {
    fTriggerAsymmetry = 0.0;
  }
  cout << " ----------------------------- " << endl;

  /*
    C.Y. Sep 19, 2020 : This method needs to be updated to add/write the delayed events to the Scaler Tree. See End() method in THcScalerEvtHandler.cxx
   */
  
  return 0;
}


Int_t THcHelicityScaler::ReadDatabase(const TDatime& date )
{

  /*
    C.Y. Sep 19, 2020 : This method needs to be updated to include additional BCM parameters. See ReadDatabase() in THcScalerEvtHandler.cxx
   */
  
  char prefix[2];
  prefix[0]='g'; prefix[1]='\0';

  fNumBCMs = 0;
  string bcm_namelist;
  DBRequest list[]={
		    {"NumBCMs",&fNumBCMs, kInt, 0, 1},
		    {"BCM_Names",     &bcm_namelist,       kString},
		    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  if(fNumBCMs > 0) {
    fBCM_Gain = new Double_t[fNumBCMs];
    fBCM_Offset = new Double_t[fNumBCMs];
    DBRequest list2[]={
      {"BCM_Gain",      fBCM_Gain,         kDouble, (UInt_t) fNumBCMs},
      {"BCM_Offset",     fBCM_Offset,       kDouble,(UInt_t) fNumBCMs},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list2, prefix);
    fBCM_Name = vsplit(bcm_namelist);
  }


  
  
  return kOK;
}
void THcHelicityScaler::SetDelayedType(int evtype) {
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
  
Int_t THcHelicityScaler::Analyze(THaEvData *evdata)
{

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebugFile) {
    *fDebugFile << endl << "---------------------------------- "<<endl<<endl;
    *fDebugFile << "\nEnter THcHelicityScaler  for fName = "<<fName<<endl;
    EvDump(evdata);
  }

  UInt_t *rdata = (UInt_t*) evdata->GetRawDataBuffer();
  
  if(evdata->GetEvType() == fDelayedType) { // Save this event for processing later
    Int_t evlen = evdata->GetEvLength();
    UInt_t *datacopy = new UInt_t[evlen];
    fDelayedEvents.push_back(datacopy);
    memcpy(datacopy,rdata,evlen*sizeof(UInt_t));
    return 1;
  } else { 			// A normal event
    if (fDebugFile) *fDebugFile<<"\n\nTHcHelicityScaler :: Debugging event type "<<dec<<evdata->GetEvType()<< " event num = " << evdata->GetEvNum() << endl<<endl;
    evNumber=evdata->GetEvNum();
    Int_t ret;
    if((ret=AnalyzeBuffer(rdata))) {
      //
    }
    return ret;
  }

  /*
    C.Y. Sep 19, 2020 : Here goes the code to create a Helicity Scaler TTree, add the variables obtained in AnalyzeBuffer(), and Fill the tree.
    (See Analyze() method in THcScalerEvtHandler.cxx)
   */
  
}
Int_t THcHelicityScaler::AnalyzeBuffer(UInt_t* rdata)
{
  fNTrigsInBuf = 0;

  // Parse the data, load local data arrays.
  UInt_t *p = (UInt_t*) rdata;

  UInt_t *plast = p+*p;		// Index to last word in the bank
  Int_t roc = -1;
  Int_t evlen = *p+1;

  Int_t ifound=0;
  
  while(p<plast) {
    Int_t banklen = *p;
    p++;			  // point to header

    if (fDebugFile) {
      *fDebugFile << "Bank: " << hex << *p << dec << " len: " << *(p-1) << endl;
    }
    if((*p & 0xff00) == 0x1000) {	// Bank Containing banks
      if(evlen-*(p-1) > 1) { // Don't use overall event header
        roc = (*p>>16) & 0xf;
	if(fDebugFile) *fDebugFile << "ROC: " << roc << " " << evlen << " " << *(p-1) << hex << " " << *p << dec << endl;
//		cout << "ROC: " << roc << " " << evlen << " " << *(p-1) << hex << " " << *p << dec << endl;
	if(roc != fROC) { // Not a ROC with helicity scaler
	  p+=*(p-1)-1;		// Skip to next ROC
	}
      }
      p++;				// Now pointing to a bank in the bank
    } else if (((*p & 0xff00) == 0x100) && (*p != 0xC0000100)) {
      // Bank containing integers.  Look for scalers
      // This is either ROC bank containing integers or
      // a bank within a ROC containing data from modules of a single type
      // Look for banks with the helicity scaler bank ID (9801)
      // Assume that very first word is a scaler header
      // At any point in the bank where the word is not a matching
      // header, we stop.
      UInt_t tag = (*p>>16) & 0xffff; // Bank ID (ROC #)
  //          UInt_t num = (*p) & 0xff;
      UInt_t *pnext = p+banklen;	// Next bank
      p++;			// First data word
      // If the bank is not a helicity scaler bank
      // or it is not one of the ROC containing helcity scaler data
      // skip to the next bank
      //cout << "BankID=" << tag << endl;

      if (tag != fBankID) {
	p = pnext;		// Fall through to end of the above else if
	//	cout << "  Skipping to next bank" << endl;

      } else {
	// This is a helicity scaler bank
	if (roc == fROC) {
	  Int_t nevents = (banklen-2)/fNScalerChannels;
	  //cout << "# of helicity events in bank:" << " " << nevents << endl;
	  if (nevents > 100) {
	    cout << "Error! Beam off for too long" << endl;	
	  }
	  
	  fNTrigsInBuf = 0;
	  // Save helcitiy and quad info for THcHelicity
	  for (Int_t iev = 0; iev < nevents; iev++) {  // find number of helicity events in each bank
	    Int_t index = fNScalerChannels*iev+1;
	    AnalyzeHelicityScaler(p+index);
	    //	    cout << "H: " << evNumber << endl;
	  }
	}
      }
    
      //C.Y. Sep 19, 2020 : We need to look for normalization scaler modules (the scaler with the clock) 
      /*  //This was copied directly from THcScalerEvtHandler.cxx, and it is where the clock scaler is decoded. The scalers[idx] 
          //represents vector with components -->  scalers.push_back(new Scaler3801(icrate, islot));  Will Ask Steve Wood for advice
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
      */
      
      while(p < pnext) {
	Int_t nskip = 0;
	if(fDebugFile) {
	  *fDebugFile << "Scaler Header: " << hex << *p << dec;
	}

	//C.Y. Sep 19, 2020 :: Here we would need to loop over scalers vector and decode the other scalers (ONLY helicity scalers)
	/*  //This part of the code was copied from THcScalerEvtHandler.cxx
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
	*/
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

  if (!ifound) return 0;


  //C.Y. Sep 19, 2020 : Here goes the code to write the helicity raw data to a variable, and to map the variable to the scaler location
  //( See AnalyzeBuffer() method in THcScalerEvtHandler.cxx )

  
  
  return 1;
 	
}

Int_t THcHelicityScaler::AnalyzeHelicityScaler(UInt_t *p)
{
  Int_t hbits = (p[0]>>30) & 0x3; // quartet and helcity bits in scaler word
  Bool_t isquartet = (hbits&2) != 0;
  Int_t ispos = hbits&1;
  Int_t actualhelicity = 0;
  fHelicityHistory[fNTrigsInBuf] = hbits;
  fNTrigsInBuf++;
  fNTriggers++;

  Int_t quartetphase = (fNTriggers-fFirstCycle)%4;
  if(fFirstCycle >= -10) {
    if(quartetphase == 0) {
      Int_t predicted = RanBit30(fRingSeed_reported);
      fRingSeed_reported = ((fRingSeed_reported<<1) | ispos) & 0x3FFFFFFF;
      // Check if ringseed_predicted agrees with reported if(fNBits>=30)
      if(fNBits >= 30 && predicted != fRingSeed_reported) {
	cout << "THcHelicityScaler: Helicity Prediction Failed" << endl;
	cout << "Reported  " << bitset<32>(fRingSeed_reported) << endl;
	cout << "Predicted " << bitset<32>(predicted) << endl;
      }
      fNBits++;
      if(fNBits==30) {
	cout << "THcHelicityScaler: A " << bitset<32>(fRingSeed_reported) <<
	  " found at cycle " << fNTriggers << endl;
      }
    } else if (quartetphase == 3) {
      if(!isquartet) {
	cout << "THcHelicityScaler: Quartet bit expected but not set (" <<
	  fNTriggers << ")" << endl;
	fNBits = 0;
	fRingSeed_reported = 0;
	fRingSeed_actual = 0;
	fFirstCycle = -100;
      }
    }
  } else { 			// First cycle not yet identified
    if(isquartet) { // Helicity and quartet signal for next set of scalers
      fFirstCycle = fNTriggers - 3;
      quartetphase = (fNTriggers-fFirstCycle)%4;
      //// Helicity at start of quartet is same as last of quartet, so we can start filling the seed
      fRingSeed_reported = ((fRingSeed_reported<<1) | ispos) & 0x3FFFFFFF;
      fNBits++;
      if(fNBits==30) {
	cout << "THcHelicityScaler: B " << bitset<32>(fRingSeed_reported) <<
	  " found at cycle " << fNTriggers << endl;
      }
    }
  }

  if(fNBits>=30) {
    fRingSeed_actual = RanBit30(fRingSeed_reported);
    fRingSeed_actual = RanBit30(fRingSeed_actual);

#define DELAY9
#ifdef DELAY9
    if(quartetphase == 3) {
      fRingSeed_actual = RanBit30(fRingSeed_actual);
      actualhelicity = (fRingSeed_actual&1)?+1:-1;
    } else {
      actualhelicity = (fRingSeed_actual&1)?+1:-1;
      if(quartetphase == 0 || quartetphase == 1) {
	actualhelicity = -actualhelicity;
      }
    }
#else
    actualhelicity = (fRingSeed_actual&1)?+1:-1;
    if(quartetphase == 1 || quartetphase == 2) {
      actualhelicity = -actualhelicity;
    }
#endif
  } else {
    fRingSeed_actual = 0;
  }

  if(actualhelicity!=0) {

    //C.Y. 11/24/2020  if-else notation--> expression 1 ? expression 2 : 3
    //This means: expression 1 is a condition which is evaluated, if the condition is true (non-zero)
    //then the control is transferred to expression 2, otherwise, the control passes to expression 3.
    //--> if (actualhelicity>0) {hindex = 0;} else {hindex = 1;}
    //from the if-else statement: hindex=0 is for pos helicity, hindex=1 is for neg helicity
    Int_t hindex = (actualhelicity>0)?0:1;  

    //C.Y. 11/24/2020  increment the counter for either '+' or '-' helicity states
    (actualhelicity>0)?(fNTriggersPlus++):(fNTriggersMinus++);

    //C.Y. 11/24/2020  Loop over all 32 scaler channels for a specific helicity scaler module (SIS 3801)
    for(Int_t i=0;i<fNScalerChannels;i++) {

      //C.Y. 11/24/2020 the count expression below gets the scaler raw helicity information for the ith channel
      Int_t count = p[i]&0xFFFFFF; // Bottom 24 bits  equivalent of scalers->Decode()
      
      //for every +/- pair event (catch), write to tree
      //Here is where the important information from scalers gets passed to the scalerloc
      //--> before 10/24/2018, were useless helicity scalers, 2 separate scalers do not help
      //--> with a single helicity scaler (for each ROC), for each event you determine whether
      //is + or - helicity or undefined (MPS)
      //--> Steve will look into this.  I will also look into this. 

      //Increment either the '+' (hindex=0) or '-' (hindex=1) helicity counts for each [i] scaler channel channel of a given module
      //Currently, we have a single helicity scaler module for each spectrometer arm.  Each module has copies of the other module
      //as well as its own helicity scaler values spanning over the 32 channels
      fHScalers[hindex][i] += count;
      fScalerSums[i] += count;

      //C.Y. 11/24/2020 : The helicity scaler information for each channel is stored in fHSCalers[hindex][i], for positive (hindex=0) and
      //negative (hindex=1) helicity states, so this variable is extremely important for writing the information to the helicity scaler tree. 
      //There is one channel reserved for the 1 MHz clock time.
    }
  }
  return(0);
}
//_____________________________________________________________________________
Int_t  THcHelicityScaler::RanBit30(Int_t ranseed)
{
  
  UInt_t bit7    = (ranseed & 0x00000040) != 0;
  UInt_t bit28   = (ranseed & 0x08000000) != 0;
  UInt_t bit29   = (ranseed & 0x10000000) != 0;
  UInt_t bit30   = (ranseed & 0x20000000) != 0;

  UInt_t newbit = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;

  ranseed =  ( (ranseed<<1) | newbit ) & 0x3FFFFFFF;

  return ranseed;

}
//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHelicityScaler::Init(const TDatime& date)
{
  
  ReadDatabase(date);

  fStatus = kOK;

  for( vector<UInt_t*>::iterator it = fDelayedEvents.begin();
       it != fDelayedEvents.end(); ++it )
    delete [] *it;
  fDelayedEvents.clear();

  cout << "Howdy !  We are initializing THcHelicityScaler !!   name =   "
        << fName << endl;

  if(eventtypes.size()==0) {
    eventtypes.push_back(0);  // Default Event Type
  }

  if(fROC < 0) {
    fROC = 8;			// Default to SHMS crate
  }

  fNTriggers = 0;
  fNTrigsInBuf = 0;
  fFirstCycle = -100;
  fRingSeed_reported = 0;
  fRingSeed_actual = 0;
  fNBits = 0;
  fNTriggersPlus = fNTriggersMinus = 0;
  fHScalers[0] = new Double_t[fNScalerChannels];
  fHScalers[1] = new Double_t[fNScalerChannels];
  fScalerSums = new Double_t[fNScalerChannels];
  fAsymmetry = new Double_t[fNScalerChannels];
  fAsymmetryError = new Double_t[fNScalerChannels];

  for(Int_t i=0;i<fNScalerChannels;i++) {
    fHScalers[0][i] = 0.0;
    fHScalers[1][i] = 0.0;
    fScalerSums[0] = 0.0;
    fAsymmetry[0] = 0.0;
    fAsymmetryError[0] = 0.0;
  }

  fCharge = new Double_t[fNumBCMs];
  fChargeAsymmetry = new Double_t[fNumBCMs];

  fTime = fTimeAsymmetry = 0;
  fTriggerAsymmetry = 0.0;

  MakeParms();


  
  /*C.Y. Sep 19, 2020 : Here goes the code to parse the scaler map file ( db_P(H)Scalevt.dat ) and extract the crate/slot/channel information
  and generate scaler names that will eventually be written to a TTree ( See Init() method in THcScalerEvtHandler.cxx )

  The problem here is that there are two helicity states per channel, so how do we actually write both to a single leaf variable?
  I would think the answer is to create a second variable and change the suffix name to "negHel", using AddVars(). This way, we can
  probably have scalers_posHel, and scalers_negHel vectors, and only the variable name would be different.

  */
  
  return kOK;
}

void THcHelicityScaler::MakeParms()
{
  /**
     Put Various helicity scaler results in gHcParms so they can be included in results.
  */
  gHcParms->Define(Form("g%s_hscaler_plus[%d]",fName.Data(),fNScalerChannels),
		   "Plus Helcity Scalers",*(fHScalers[0]));
  gHcParms->Define(Form("g%s_hscaler_minus[%d]",fName.Data(),fNScalerChannels),
		   "Minus Helcity Scalers",*(fHScalers[1]));
  gHcParms->Define(Form("g%s_hscaler_sum[%d]",fName.Data(),fNScalerChannels),
		   "Helcity Scalers Sum",*fScalerSums);
  gHcParms->Define(Form("g%s_hscaler_asy[%d]",fName.Data(),fNScalerChannels),
		   "Helicity Scaler Asymmetry[%d]",*fAsymmetry);
  gHcParms->Define(Form("g%s_hscaler_asyerr[%d]",fName.Data(),fNScalerChannels),
		   "Helicity Scaler Asymmetry Error[%d]",*fAsymmetryError);
  gHcParms->Define(Form("g%s_hscaler_triggers",fName.Data()),
		   "Total Helicity Scaler Triggers",fNTriggers);
  gHcParms->Define(Form("g%s_hscaler_triggers_plus",fName.Data()),
		   "Positive Helicity Scaler Triggers",fNTriggersPlus);
  gHcParms->Define(Form("g%s_hscaler_triggers_minus",fName.Data()),
		   "Negative Helicity Scaler Triggers",fNTriggersMinus);
  gHcParms->Define(Form("g%s_hscaler_charge[%d]",fName.Data(),fNumBCMs),
		   "Helicity Gated Charge",*fCharge);
  gHcParms->Define(Form("g%s_hscaler_charge_asy[%d]",fName.Data(),fNumBCMs),
		   "Helicity Gated Charge Asymmetry",*fChargeAsymmetry);
  gHcParms->Define(Form("g%s_hscaler_time",fName.Data()),
		   "Helicity Gated Time (sec)",fTime);
  gHcParms->Define(Form("g%s_hscaler_time_asy",fName.Data()),
		   "Helicity Gated Time Asymmetry",fTimeAsymmetry);
  gHcParms->Define(Form("g%s_hscaler_trigger_asy",fName.Data()),
		   "Helicity Trigger Asymmetry",fTriggerAsymmetry);
}

//--------------------C.Y. Sep 20, 2020 : Added Utility Functions--------------------

void THcHelicityScaler::AddVars(TString name, TString desc, UInt_t islot,
				  UInt_t ichan, UInt_t ikind)
{
  // need to add fName here to make it a unique variable.  (Left vs Right HRS, for example)
  TString name1 = fName + name;
  TString desc1 = fName + desc;
  // We don't yet know the correspondence between index of scalers[] and slots.
  // Will put that in later.
  scalerloc.push_back( new HCScalerLoc(name1, desc1, 0, islot, ichan, ikind,
				       scalerloc.size()) );
}

void THcHelicityScaler::DefVars()
{
  // called after AddVars has finished being called.
  Nvars = scalerloc.size();
  if (Nvars == 0) return;
  dvars_prev_read = new UInt_t[Nvars];  // dvars_prev_read is a member of this class
  dvars = new Double_t[Nvars];  // dvars is a member of this class
  dvarsFirst = new Double_t[Nvars];  // dvarsFirst is a member of this class
  memset(dvars, 0, Nvars*sizeof(Double_t));
  memset(dvars_prev_read, 0, Nvars*sizeof(UInt_t));
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

size_t THcHelicityScaler::FindNoCase(const string& sdata, const string& skey)
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

//---------------------------------------------------------------------------------


ClassImp(THcHelicityScaler)
