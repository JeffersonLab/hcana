/** \class THcHelicityScaler
   \ingroup Base

\brief Event handler for Hall C helicity scalers

~~~
~~~
     THcHelcityScaler *phelscaler = new THcHelicityScaler("P","HC helicity scalers");
     phelscaler->SetDebugFile("PHelScaler.txt");
     phelscaler->SetROC(8);   // 5 for HMS defaults to 8 for SHMS
     phelscaler->SetBankID(9801); // Will default to this
     gHaEvtHandlers->Add (phelscaler);
~~~
\authors:  S. A. Wood (saw@jlab.org)
           C. Yero (cyero@jlab.org)

*/

//#include "THaEvtTypeHandler.h"
#include "THcHelicityScaler.h"
#include "GenScaler.h"
#include "Scaler3801.h"
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
static const UInt_t ICURRENT  = 3;
static const UInt_t ICHARGE   = 4;
static const UInt_t ITIME     = 5;
static const UInt_t ICUT      = 6;
static const UInt_t MAXCHAN   = 32;
static const UInt_t defaultDT = 4;


THcHelicityScaler::THcHelicityScaler(const char *name, const char* description)
  : THaEvtTypeHandler(name,description),
    fBankID(9801),
    fUseFirstEvent(kTRUE), fDelayedType(-1),
    fBCM_Gain(0), fBCM_Offset(0), fBCM_SatOffset(0), fBCM_SatQuadratic(0), fBCM_delta_charge(0),
    evcount(0), evcountR(0.0), ifound(0),
    fNormIdx(-1), fNormSlot(-1),
    dvars(0), dvarsFirst(0),
    fScalerTree(0), fOnlyBanks(kFALSE),
    fClockChan(-1), fLastClock(0)
{

  fRocSet.clear();
  fModuleSet.clear();
 
  //---------------------------------------------------------------------------
  
  fROC=-1;
  fNScalerChannels = 32;

  AddEvtType(1);
  AddEvtType(2);
  AddEvtType(4);
  AddEvtType(5);
  AddEvtType(6);
  AddEvtType(7);
  AddEvtType(129);
  SetDelayedType(129);
  
}

THcHelicityScaler::~THcHelicityScaler()
{

  if (!TROOT::Initialized()) {
    delete fScalerTree;
  }
  Podd::DeleteContainer(scalers);
  Podd::DeleteContainer(scalerloc);
  delete [] dvars;
  delete [] dvarsFirst;
  delete [] fBCM_SatOffset;
  delete [] fBCM_SatQuadratic;
  delete [] fBCM_delta_charge;  
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
      
  for(std::vector<UInt_t*>::iterator it = fDelayedEvents.begin();
      it != fDelayedEvents.end(); ++it) {
    UInt_t* rdata = *it;
    evNumberR += 1;
    AnalyzeBuffer(rdata);
  }
  
  for( vector<UInt_t*>::iterator it = fDelayedEvents.begin();
       it != fDelayedEvents.end(); ++it )
    delete [] *it;
  fDelayedEvents.clear();
  
  //Write the helicity variables to scaler tree
  if (fScalerTree) { fScalerTree->Write(); }

  
  //Compute Scaler Asymmetries
  /*
  for(Int_t i=0;i<fNScalerChannels;i++) {
    if(fScalerSums[i]>0.5) {
      fScalAsymmetry[i] = (fHScalers[0][i]-fHScalers[1][i])/fScalerSums[i];
      fScalAsymmetryError[i] = 2*TMath::Sqrt(fHScalers[0][i]*fHScalers[1][i]
					 *fScalerSums[i])
	/(fScalerSums[i]*fScalerSums[i]);
    } else {
      fScalAsymmetry[i] = 0.0;
      fScalAsymmetryError[i] = 0.0;
    }
    
  }
  */

    //------Compute Time Asymmetries-------

  //C.Y. 12/15/2020  Time Asymmetry / Error Calculations (with scaler_current cut)
      
    if(fQuartetCount <= 1) {
      fTimeAsymmetry       = -1000.;
      fTimeAsymmetryError  = 0.0;
    } else {
      fTimeAsymmetry = fTimeAsymSum/fQuartetCount;  //normalize asymmetry to total number of quartets
      if(fTimeAsymSum2 >= fQuartetCount*TMath::Power(fTimeAsymmetry,2)) {
	fTimeAsymmetryError = TMath::Sqrt((fTimeAsymSum2 -
						fQuartetCount*TMath::Power(fTimeAsymmetry,2)) /
					       (fQuartetCount*(fQuartetCount-1)));
      } else {
	fTimeAsymmetryError = 0.0;
      }
    }

  
  
  //------Compute Scaler Asymmetries-------

  //C.Y. 12/15/2020  Scaler Asymmetry / Error Calculations (with scaler_current cut)
  
  for(Int_t i=0; i<fNScalerChannels; i++) {
    
    if(fQuartetCount <= 1) {
      fScalAsymmetry[i]       = -1000.;
      fScalAsymmetryError[i]  = 0.0;
    } else {
      fScalAsymmetry[i] = fScalAsymSum[i]/fQuartetCount;  //normalize asymmetry to total number of quartets
      if(fScalAsymSum2[i] >= fQuartetCount*TMath::Power(fScalAsymmetry[i],2)) {
	fScalAsymmetryError[i] = TMath::Sqrt((fScalAsymSum2[i] -
						fQuartetCount*TMath::Power(fScalAsymmetry[i],2)) /
					       (fQuartetCount*(fQuartetCount-1)));
      } else {
	fScalAsymmetryError[i] = 0.0;
      }
    }

  }

  
  //------Compute Charge Asymmetries-------

  //C.Y. 12/14/2020  Charge Asymmetry / Error Calculations (with scaler_current cut)
  
  //Set the helicity scaler module channels for each BCM
  std::map<std::string, Int_t> bcmindex;
  bcmindex["BCM1_Hel.scal"] = 0;
  bcmindex["BCM2_Hel.scal"] = 2;
  bcmindex["Unser_Hel.scal"] = 6;
  bcmindex["BCM4A_Hel.scal"] = 10;
  bcmindex["BCM4B_Hel.scal"] = 4;
  bcmindex["BCM4C_Hel.scal"] = 12;
  //bcmindex["1MHz_Hel.scal"] = 8;
  

  //cout << endl << "---------------------- Beam Charge Asymmetries ---------------------- " << endl;
  //cout << "  BCM        Total     Charge        Beam ON     Beam ON      Asymmetry" << endl;
  //cout << " Name       Charge    Asymmetry       Charge    Asymmetry        Error"    << endl;
  
  for(Int_t i=0;i<fNumBCMs;i++) {
    
    if(fQuartetCount <= 1) {
      fChargeAsymmetry[i]       = -1000.;
      fChargeAsymmetryError[i]  = 0.0;
    } else {
      fChargeAsymmetry[i] = fChargeAsymSum[i]/fQuartetCount;  //normalize charge asymmetry to total number of quartets (as the sum is for every quartet)

      if(fChargeAsymSum2[i] >= fQuartetCount*TMath::Power(fChargeAsymmetry[i],2)) {
	fChargeAsymmetryError[i] = TMath::Sqrt((fChargeAsymSum2[i] -
						fQuartetCount*TMath::Power(fChargeAsymmetry[i],2)) /
					       (fQuartetCount*(fQuartetCount-1)));
      } else {
	fChargeAsymmetryError[i] = 0.0;
      }
    }

    //printf("%6s %12.2f %12.8f %12.2f %12.8f %12.8f\n",fBCM_Name[i].c_str(),fCharge[i],
    //	   fChargeAsymmetry[i],fChargeSum[i],asy,asyerr);
  }


  //Compute +/- helicity Times  (no BCM cut)
  Double_t pclock = fHScalers[0][fClockChan];
  Double_t mclock = fHScalers[1][fClockChan];

  fTimePlus = pclock/fClockFreq;
  fTimeMinus = mclock/fClockFreq;
  //fTime = (pclock+mclock)/fClockFreq;
  //if(pclock+mclock>0) {
    //fTimeAsymmetry = (pclock-mclock)/(pclock+mclock);
    //} else {
    //  fTimeAsymmetry = 0.0;
    //}
  //printf("TIME(s)%12.2f %12.8f %12.2f\n",fTime, fTimeAsymmetry, fTimeSum);
  

  //Compute Helicity Trigger Asymmetries (no BCM cut)
  if(fNTriggersPlus+fNTriggersMinus > 0) {
    fTriggerAsymmetry = ((Double_t) (fNTriggersPlus-fNTriggersMinus))/(fNTriggersPlus+fNTriggersMinus);
  } else {
    fTriggerAsymmetry = 0.0;
  }
  
  return 0;
}


Int_t THcHelicityScaler::ReadDatabase(const TDatime& date )
{
    
  char prefix[2];
  prefix[0]='g';
  prefix[1]='\0';
  fNumBCMs = 0;
  DBRequest list[]={
    {"NumBCMs",&fNumBCMs, kInt, 0, 1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  
  if(fNumBCMs > 0) {
    fBCM_Gain = new Double_t[fNumBCMs];
    fBCM_Offset = new Double_t[fNumBCMs];
    fBCM_SatOffset = new Double_t[fNumBCMs];
    fBCM_SatQuadratic = new Double_t[fNumBCMs];
    fBCM_delta_charge= new Double_t[fNumBCMs];
    
    string bcm_namelist;
    DBRequest list2[]={
      {"BCM_Gain",      fBCM_Gain,         kDouble, (UInt_t) fNumBCMs},
      {"BCM_Offset",     fBCM_Offset,       kDouble,(UInt_t) fNumBCMs},
      {"BCM_SatQuadratic",     fBCM_SatQuadratic,       kDouble,(UInt_t) fNumBCMs,1},
      {"BCM_SatOffset",     fBCM_SatOffset,       kDouble,(UInt_t) fNumBCMs,1},
      {"BCM_Names",     &bcm_namelist,       kString},
      {"BCM_Current_threshold",     &fbcm_Current_Threshold,       kDouble,0, 1},
      {"BCM_Current_threshold_index",     &fbcm_Current_Threshold_Index,       kInt,0,1},
      {0}
    };
    
    fbcm_Current_Threshold = 0.0;
    fbcm_Current_Threshold_Index = 0;
    for(Int_t i=0;i<fNumBCMs;i++) {
      fBCM_SatOffset[i]=0.;
      fBCM_SatQuadratic[i]=0.;
    }
    gHcParms->LoadParmValues((DBRequest*)&list2, prefix);
    vector<string> bcm_names = vsplit(bcm_namelist);
    for(Int_t i=0;i<fNumBCMs;i++) {
      fBCM_Name.push_back(bcm_names[i]+"_Hel.scal");
      fBCM_delta_charge[i]=0.;
    }
  }
  fTotalTime=0.;
  fPrevTotalTime=0.;
  fDeltaTime=-1.;
   
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
  
  //C.Y. | THcScalerEvtHandler::Analyze() uses this flag (which is forced to be 1),
  //but as to why, it is beyond me. For consistency, I have also used it here.
  Int_t lfirst=1;  
   
  if(evdata->GetEvNum() > 0) {
    evNumber=evdata->GetEvNum();
    evNumberR = evNumber;
  }

  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebugFile) {
    *fDebugFile << endl << "---------------------------------- "<<endl<<endl;
    *fDebugFile << "\nEnter THcHelicityScaler  for fName = "<<fName<<endl;
    EvDump(evdata);
  }
  
  
  if (lfirst && !fScalerTree) {

    lfirst = 0; 

    //Assign a name to the helicity scaler tree
    TString sname1 = "TSHel";
    TString sname2 = sname1 + fName;
    TString sname3 = fName + "  Scaler Data";

    if (fDebugFile) {
      *fDebugFile << "\nAnalyze 1st time for fName = "<<fName<<endl;
      *fDebugFile << sname2 << "      " <<sname3<<endl;
    }

    //Create Scaler Tree
    fScalerTree = new TTree(sname2.Data(),sname3.Data());
    fScalerTree->SetAutoSave(200000000);

    TString name, tinfo;

    name = "evcount";
    tinfo = name + "/D";
    fScalerTree->Branch(name.Data(), &evcountR, tinfo.Data(), 4000);
 
    name = "evNumber";
    tinfo = name + "/D";
    fScalerTree->Branch(name.Data(), &evNumberR, tinfo.Data(), 4000);

    //C.Y. 12/02/2020 Added actual helicity to be stored in scaler tree
    name = "actualHelicity";                                                                                                                      
    tinfo = name + "/D";                                                                                                  
    fScalerTree->Branch(name.Data(), &actualHelicityR, tinfo.Data(), 4000); 

    //C.Y. 12/09/2020 Added quartetphase to be stored in scaler tree
    name = "quartetPhase";
    tinfo = name + "/D";                                                                                                  
    fScalerTree->Branch(name.Data(), &quartetphaseR, tinfo.Data(), 4000);
    
    for (size_t i = 0; i < scalerloc.size(); i++) {
      name = scalerloc[i]->name;
      tinfo = name + "/D";
      fScalerTree->Branch(name.Data(), &dvars[i], tinfo.Data(), 4000);
    }
    
  }
  
  UInt_t *rdata = (UInt_t*) evdata->GetRawDataBuffer();
  
  if(evdata->GetEvType() == fDelayedType) { // Save this event for processing later
    Int_t evlen = evdata->GetEvLength();
    
    UInt_t *datacopy = new UInt_t[evlen];
    fDelayedEvents.push_back(datacopy);
    memcpy(datacopy,rdata,evlen*sizeof(UInt_t));
    return 1;
  }
  
  else { 			// A normal event
    if (fDebugFile) *fDebugFile<<"\n\nTHcHelicityScaler :: Debugging event type "<<dec<<evdata->GetEvType()<< " event num = " << evdata->GetEvNum() << endl<<endl;
    Int_t ret;
    if((ret=AnalyzeBuffer(rdata))) 
      {	
	if (fDebugFile) *fDebugFile << "scaler tree ptr 1 "<<fScalerTree<<endl;
	if (fDebugFile) *fDebugFile << "ret = "<< ret <<endl; 
      }
    
    return ret;
  }  
  
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
	// cout << "ROC: " << roc << " " << evlen << " " << *(p-1) << hex << " " << *p << dec << endl;
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
      // UInt_t num = (*p) & 0xff;
      UInt_t *pnext = p+banklen;	// Next bank
      p++;			// First data word
      // If the bank is not a helicity scaler bank
      // or it is not one of the ROC containing helcity scaler data
      // skip to the next bank
      //cout << "BankID=" << tag << endl;

      if (tag != fBankID) {
	p = pnext;		// Fall through to end of the above else if
	// cout << "  Skipping to next bank" << endl;

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

	    //C.Y. 11/26/2020 This methods extracts the raw helicity information
	    AnalyzeHelicityScaler(p+index); 
	  }
	}
      }
    
      while(p < pnext) {
	Int_t nskip = 0;
	if(fDebugFile) {
	  *fDebugFile << "Scaler Header: " << hex << *p << dec;
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

  if (!ifound) return 0;

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
      // Helicity at start of quartet is same as last of quartet, so we can start filling the seed
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
    quartetphase = (quartetphase+1)%4;
#else
    actualhelicity = (fRingSeed_actual&1)?+1:-1;
    if(quartetphase == 1 || quartetphase == 2) {
      actualhelicity = -actualhelicity;
    }
#endif
  } else {
    fRingSeed_actual = 0;
  }

  //C.Y. 12/09/2020 : Pass quartet phase to scaler tree varable
  quartetphaseR = quartetphase;
  
  //C.Y. 11/26/2020  The block of code below is used to extract the helicity information from each
  //channel of the helicity scaler onto a variable to be stored in the scaler tree. For each channel,
  //each helicity state (+, -, or undefined) is stored in a single varibale. Each helicity state
  //is tagged to the corresponding scaler read via 'actualHelicityR' which is stored below..
  
  //C.Y. Assign actual helicity value to a variable to be written to tree (the value may be (0, +hel (+1), or -hel(-1))
  //Where actualhelicity=0  is NOT the MPS.  It is zero when the actual helicity has not been determined.
  actualHelicityR = actualhelicity;
  
  //C.Y. 11/26/2020  Loop over all 32 scaler channels for a specific helicity scaler module (SIS 3801)
  for(Int_t i=0;i<fNScalerChannels;i++) {
    
    //C.Y. 11/26/2020 the count expression below gets the scaler raw helicity information (+, -, or MPS helicity states) for the ith channel
    Int_t count = p[i]&0xFFFFFF; // Bottom 24 bits  equivalent of scalers->Decode()
    fScalerChan[i] = count;        //pass the helicity raw information to each helicity scaler channel array element
  }

  
  
  //C.Y. 11/26/2020  The block of code below is used to get a cumulative sum of +/- helicity used
  //for calculation of the cumulative beam charge asymmetry and other quantities in the ::End() method
  if(actualhelicity!=0) {

    //index=0 (+ helicity), index=1 (- helicity)
    Int_t hindex = (actualhelicity>0)?0:1;  
    
    //C.Y. 11/24/2020  increment the counter for either '+' or '-' helicity states
    (actualhelicity>0)?(fNTriggersPlus++):(fNTriggersMinus++);
    
    //C.Y. 11/24/2020  Loop over all 32 scaler channels for a specific helicity scaler module (SIS 3801)
    for(Int_t i=0;i<fNScalerChannels;i++) {
      
      Int_t count = p[i]&0xFFFFFF; // Bottom 24 bits  equivalent of scalers->Decode()
      
      //Increment either the '+' (hindex=0) or '-' (hindex=1) helicity counts for each [i] scaler channel channel of a given module
      fHScalers[hindex][i] += count;
      fScalerSums[i] += count;
    }
  }

  
  
  //Set the helicity scaler clock to define the time
  fDeltaTime = fScalerChan[fClockChan]/fClockFreq;    //total clock counts / clock_frequency (1MHz) for a specific scaler read interval
  fTotalTime = fPrevTotalTime + fDeltaTime;           //cumulative scaler time  
  
  if (fDeltaTime==0) {
    cout << " *******************   Severe Warning ****************************" << endl;
    cout << "       In THcHelicityScaler have found fDeltaTime is zero !!      " << endl;
    cout << " ******************* Alert DAQ experts ***************************" << endl;
    if (fDebugFile) *fDebugFile << " In THcHelicityScaler have found fDeltaTime is zero !!   " << endl;   
  }
  
  fPrevTotalTime=fTotalTime; //set the current total time to the previous time for the upcoming read
 
  //C.Y. Nov 27, 2020 : (below) code to write the helicity raw data to a variable
  //and map the variable to the scaler location

  Double_t scal_current=0;
  //Loop over each scaler variable from the map
  for (size_t i = 0; i < scalerloc.size(); i++)  {
    size_t ivar = scalerloc[i]->ivar;
    size_t idx = scalerloc[i]->index;  
    size_t ichan = scalerloc[i]->ichan;

    //ANALYZE 1ST SCALER READ SEPARATE (There is no previous read before this one)
    if (evcount==0) {

      //Loop over the scaler variable (ivar), helicity slot (idx), and slot channel (ichan) --> idx=0 (only one helicity module per spectrometer arm)
      if ((ivar < scalerloc.size()) &&
	  (idx < scalers.size()) &&
	  (ichan < MAXCHAN)) {
	
	//If fUseFirstEvent=kTRUE (do not skip 1st read)
	if(fUseFirstEvent){

	  //Write scaler counts 
	  if (scalerloc[ivar]->ikind == ICOUNT){
	    dvars[ivar] = fScalerChan[ichan];
	    dvarsFirst[ivar] = 0.0;          
	  }

	  //Write scaler time
	  if (scalerloc[ivar]->ikind == ITIME){
	    dvars[ivar] = fDeltaTime; 
	    dvarsFirst[ivar] = 0.0;
	  }

	  //Write scaler rate
	  if (scalerloc[ivar]->ikind == IRATE) {
	    dvars[ivar] = (fScalerChan[ichan])/fDeltaTime;
	    dvarsFirst[ivar] = 0.0;  
	  }

	  //Write either scaler current or scaler charge
	  if(scalerloc[ivar]->ikind == ICURRENT || scalerloc[ivar]->ikind == ICHARGE){

	    //Get BCM index
	    Int_t bcm_ind=-1;
	    for(Int_t itemp =0; itemp<fNumBCMs;itemp++)
	      {		
		size_t match = string(scalerloc[ivar]->name.Data()).find(string(fBCM_Name[itemp]));
		if (match!=string::npos)
		  {
		    bcm_ind=itemp;
		  }
	      }

	    //Calculate and write scaler current
	    if (scalerloc[ivar]->ikind == ICURRENT) {

	      //set default to zero
	      dvars[ivar]=0.;
	      //Check bcm index and calculate current in a temporary placeholder, "cur_temp"
	      if (bcm_ind != -1) {
		Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		cur_temp = cur_temp + fBCM_SatQuadratic[bcm_ind]*TMath::Max(cur_temp-fBCM_SatOffset[i],0.0);
		//Assign the calculated scaler current to dvars
		dvars[ivar]=cur_temp;		
	      }
	      //Check which bcm index to place a bcm current threshold cut later on. Assign the the beam current read to "scal_current" for later use.
	      if (bcm_ind == fbcm_Current_Threshold_Index) {scal_current= dvars[ivar];}
	    }

	    //Calculate andd write scaler charge
	    if (scalerloc[ivar]->ikind == ICHARGE) {
	      //Check bcm index and calculate current in a temporary placeholder, "cur_temp", to determine the charge later on.
	      if (bcm_ind != -1) {
		Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.0);
		//Calculate the charge for this scaler read
		fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
		//Assign the charge to dvars
		dvars[ivar] = fBCM_delta_charge[bcm_ind];  
	      }
	    }
	  } 
	}
	
	//If user has set fUseFirstEvent=kFALSE (then use dvarsFirst to store the information for the 1st event, and leave dvars at 0.0)
	//(The same calculations as above, but assign dvars=0.0, so that it does not count when written to the scaler tree)
	else { //ifnotusefirstevent
	  
	  if (scalerloc[ivar]->ikind == ICOUNT) {	    
	    dvarsFirst[ivar] = fScalerChan[ichan];  
	    dvars[ivar] = 0.0;
	  }
	  
	  if (scalerloc[ivar]->ikind == ITIME){
	    dvarsFirst[ivar] = fDeltaTime; 
	    dvars[ivar] = 0.0;
	  }
	  
	  if (scalerloc[ivar]->ikind == IRATE)  {
	    dvarsFirst[ivar] = (fScalerChan[ichan])/fDeltaTime;
	    dvars[ivar] = 0.0;
	  }
	  
	  if(scalerloc[ivar]->ikind == ICURRENT || scalerloc[ivar]->ikind == ICHARGE)
	    {
	      Int_t bcm_ind=-1;
	      for(Int_t itemp =0; itemp<fNumBCMs;itemp++)
		{		
		  size_t match = string(scalerloc[ivar]->name.Data()).find(string(fBCM_Name[itemp]));
		  if (match!=string::npos)
		    {
		      bcm_ind=itemp;
		    }
		}
	      
	      if (scalerloc[ivar]->ikind == ICURRENT) {

		dvarsFirst[ivar] = 0.0;
		dvars[ivar] = 0.0;
		
                if (bcm_ind != -1) {
		  Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		  cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[i],0.0),2.0);
		  dvarsFirst[ivar] = cur_temp;
		}
		
         	if (bcm_ind == fbcm_Current_Threshold_Index) scal_current= dvarsFirst[ivar];
	      }
	      
	      if (scalerloc[ivar]->ikind == ICHARGE) {
		
		if (bcm_ind != -1) {
		  Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		  cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.0);
		  fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
		  dvarsFirst[ivar] = fBCM_delta_charge[bcm_ind];
		}
	      }
	    }
	}
      }      
      else {
	cout << "THcHelicityScaler:: ERROR:: incorrect index "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      }
    }

    //Calculate Scaler Quantities for ALL OTHER SCALER READS (OTHER THAN THE FIRST READ)
    else{ // evcount != 0
    
      if ((ivar < scalerloc.size()) &&
	  (idx < scalers.size()) &&
	  (ichan < MAXCHAN)) {
	
	if (scalerloc[ivar]->ikind == ICOUNT) {	  
	  dvars[ivar] = fScalerChan[ichan];
	  }
	
	if (scalerloc[ivar]->ikind == ITIME) {
	  dvars[ivar] = fDeltaTime; 
	}
	
	if (scalerloc[ivar]->ikind == IRATE) {
	  dvars[ivar] =  fScalerChan[ichan]/fDeltaTime; 
	}
	
	if(scalerloc[ivar]->ikind == ICURRENT || scalerloc[ivar]->ikind == ICHARGE)
	  {
	    Int_t bcm_ind=-1;
	    for(Int_t itemp =0; itemp<fNumBCMs;itemp++)
	      {		
		size_t match = string(scalerloc[ivar]->name.Data()).find(string(fBCM_Name[itemp]));
		if (match!=string::npos)
		  {
		    bcm_ind=itemp;
		  }
	      }
	    if (scalerloc[ivar]->ikind == ICURRENT) {
	      dvars[ivar]=0.;

	      if (bcm_ind != -1) {

		if (fDeltaTime>0) {
		  Double_t cur_temp=(fScalerChan[ichan]/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];  
		  cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.0);		  
		  dvars[ivar]=cur_temp;
		}
	      }
	      if (bcm_ind == fbcm_Current_Threshold_Index) scal_current= dvars[ivar];
	    }
	    
	    if (scalerloc[ivar]->ikind == ICHARGE) {
	      
	      if (bcm_ind != -1) {
		dvars[ivar] = 0.0;

		if (fDeltaTime>0)  {  
		  Double_t cur_temp=(fScalerChan[ichan]/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];  
		  cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.0);
		  fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
		}
		dvars[ivar] = fBCM_delta_charge[bcm_ind];   
	      }
	    }
	  }
	
      } else {
	cout << "THcHelicityScaler:: ERROR:: incorrect index "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      }
    } 
  }
  
  //Analyze Scaler Reads ONLY FOR SCAL_CURRENTS > BCM_CURRENT THRESHOLD
  //(These variables have the name structure: varibaleName_Cut)

  for (size_t i = 0; i < scalerloc.size(); i++)  {
    size_t ivar = scalerloc[i]->ivar;
    size_t ichan = scalerloc[i]->ichan;

    if (scalerloc[ivar]->ikind == ICUT+ICOUNT){
      if ( scal_current > fbcm_Current_Threshold) {
	dvars[ivar] = fScalerChan[ichan];
      }
    }
    
    if (scalerloc[ivar]->ikind == ICUT+ICHARGE){
      Int_t bcm_ind=-1;
      for(Int_t itemp =0; itemp<fNumBCMs;itemp++)
	{		
	  size_t match = string(scalerloc[ivar]->name.Data()).find(string(fBCM_Name[itemp]));
	  if (match!=string::npos)
	    {
	      bcm_ind=itemp;
	    }
	}
      if ( scal_current > fbcm_Current_Threshold && bcm_ind != -1) {
	dvars[ivar] = fBCM_delta_charge[bcm_ind];  
      } 
    }
    
    if (scalerloc[ivar]->ikind == ICUT+ITIME){
      if ( scal_current > fbcm_Current_Threshold) {
	dvars[ivar] = fDeltaTime; 
      }
    }
    
  }

  //--------------------------------------

  //C.Y. 12/13/2020 : Calculate the asymmetries / errors at the end of each quartet  (S.A. Wood approach)

  if(actualhelicity!=0) {
    
    //Reset fHaveCycle to kFALSE at the start of each quartet (e.g.  - + + -,  + - - +
    if(quartetphase==0) {
      fHaveCycle[0] = fHaveCycle[1] = fHaveCycle[2] = fHaveCycle[3] = kFALSE;
    }
        
    //Check if BCM scaler current is above set threshold  
    if(scal_current > fbcm_Current_Threshold && (quartetphase==0 || fHaveCycle[max(quartetphase-1,0)])) {
      fHaveCycle[quartetphase] = kTRUE;

      //Loop over each BCM to get the charge for each cycle of a quartet
      for(Int_t i=0; i<fNumBCMs; i++) {	 
	fChargeCycle[quartetphase][i] = fBCM_delta_charge[i];
      }

      //Loop over each Scaler Channel to the the counts for each cycle of a quartet 
      for(Int_t i=0; i<fNScalerChannels; i++) {	 
	fScalCycle[quartetphase][i] = fScalerChan[i];
      }

      //Set the time for each cycle of the quartet
      fTimeCycle[quartetphase] = fDeltaTime;

    }
    
    // Compute asymmetries at the end of each quartet
    if(quartetphase == 3 && fHaveCycle[3]) {	

      //Loop over BCMs
      for(Int_t i=0;i<fNumBCMs;i++) {

	//compute charge asymmetry for each quartet at the end of said quartet
	Double_t asy = actualhelicity*(fChargeCycle[0][i]+fChargeCycle[3][i]
				       - fChargeCycle[1][i]-fChargeCycle[2][i]) /
	  (fChargeCycle[0][i]+fChargeCycle[3][i]+fChargeCycle[1][i]+fChargeCycle[2][i]);
	
	fChargeSum[i] += fChargeCycle[0][i]+fChargeCycle[1][i]
	  +fChargeCycle[2][i]+fChargeCycle[3][i];
	
	//keep track of sums for proper error calculation
	fChargeAsymSum[i] += asy;
	fChargeAsymSum2[i] += asy*asy;
      }
      
      //-------
      
      //Loop over Scaler Channels
      for(Int_t i=0; i<fNScalerChannels; i++) {

	//compute scaler asymmetry for each quartet at the end of said quartet
	Double_t asy = actualhelicity*(fScalCycle[0][i]+fScalCycle[3][i]
				       - fScalCycle[1][i]-fScalCycle[2][i]) /
	  (fScalCycle[0][i]+fScalCycle[3][i]+fScalCycle[1][i]+fScalCycle[2][i]);
	
	fScalSum[i] += fScalCycle[0][i]+fScalCycle[1][i]
	  +fScalCycle[2][i]+fScalCycle[3][i];
	
	//keep track of sums for proper error calculation
	fScalAsymSum[i] += asy;
	fScalAsymSum2[i] += asy*asy;
      }

      //-------

      //compute time asymmetry for each quartet at the end of said quartet
      Double_t asy = actualhelicity*(fTimeCycle[0]+fTimeCycle[3]
				     - fTimeCycle[1]-fTimeCycle[2]) /
	(fTimeCycle[0]+fTimeCycle[3]+fTimeCycle[1]+fTimeCycle[2]);

      //keep track of total time
      fTimeSum += fTimeCycle[0]+fTimeCycle[1]
	+fTimeCycle[2]+fTimeCycle[3];
      
      //keep track of sums for proper error calculation
      fTimeAsymSum += asy;
      fTimeAsymSum2 += asy*asy;
      
      //------

      //keep track of the total number of quartets
      fQuartetCount++;   


    }


  }
  
  
  //--------------------------------------
  
  //increment scaler reads
  evcount = evcount + 1;
  evcountR = evcount;
  
  //clear Genscaler scalers
  for (size_t j=0; j<scalers.size(); j++) scalers[j]->Clear("");
  
  
  //C.Y. 12/02/2020  Fill Scaler Tree Here
  if (fScalerTree) {
    fScalerTree->Fill(); 
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
  const int LEN = 200;
  char cbuf[LEN];
  
  fStatus = kOK;
  fNormIdx = -1;
 
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

  
  //C.Y. 11/26/2020 : Read In and Parse the variables in the helicity scaler map file 
  TString dfile;
  dfile = fName + "scaler.txt";

  TString sname0 = "Scalevt";
  TString sname;
  sname = "hel"+fName+sname0;   //This should be: 'helPScalevt' or 'helHScalevt'

  //Open helicity scaler .dat map file 
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
    std::string sin(cbuf);
    std::string sinput(sin.substr(0,sin.find_first_of("#")));
    if (fDebugFile) *fDebugFile << "string input "<<sinput<<endl;
    dbline = vsplit(sinput);
    if (dbline.size() > 0) {
      pos1 = FindNoCase(dbline[0],scomment);
      if (pos1 != minus1) continue;
      pos1 = FindNoCase(dbline[0],svariable);
      
      if (pos1 != minus1 && dbline.size()>4) {
	string sdesc = "";
	for (size_t j=5; j<dbline.size(); j++) sdesc = sdesc+" "+dbline[j];
	UInt_t islot = atoi(dbline[1].c_str());
	UInt_t ichan = atoi(dbline[2].c_str());
	UInt_t ikind = atoi(dbline[3].c_str());
	if (fDebugFile)
	  *fDebugFile << "add var "<<dbline[1]<<"   desc = "<<sdesc<<"    islot= "<<islot<<"  "<<ichan<<"  "<<ikind<<endl;
	TString tsname(dbline[4].c_str());
	TString tsdesc(sdesc.c_str());
	AddVars(tsname,tsdesc,islot,ichan,ikind);
	// add extra scaler which is cut on the current
	if (ikind == ICOUNT ||ikind == ITIME ||ikind == ICHARGE  ) {
	  tsname=tsname+"Cut";
	  AddVars(tsname,tsdesc,islot,ichan,ICUT+ikind);
	  }
      }
      
      pos1 = FindNoCase(dbline[0],smap);
      if (fDebugFile) *fDebugFile << "map ? "<<dbline[0]<<"  "<<smap<<"   "<<pos1<<"   "<<dbline.size()<<endl;
      if (pos1 != minus1 && dbline.size()>6) {
	Int_t imodel, icrate, islot, inorm;
	UInt_t header, mask;
	char cdum[20];
	sscanf(sinput.c_str(),"%s %d %d %d %x %x %d \n",cdum,&imodel,&icrate,&islot, &header, &mask, &inorm);
	if ((fNormSlot >= 0) && (fNormSlot != inorm)) cout << "THcHelicityScaler::WARN:  contradictory norm slot  "<<fNormSlot<<"   "<<inorm<<endl;
	fNormSlot = inorm;  // slot number used for normalization.  This variable is not used but is checked.
	Int_t clkchan = -1;
	Double_t clkfreq = 1;
	if (dbline.size()>8) {
	  clkchan = atoi(dbline[7].c_str());
	  clkfreq = 1.0*atoi(dbline[8].c_str());
	  fClockChan=clkchan;
	  fClockFreq=clkfreq;
	}
	if (fDebugFile) {
	  *fDebugFile << "map line "<<dec<<imodel<<"  "<<icrate<<"  "<<islot<<endl;
	  *fDebugFile <<"   header  0x"<<hex<<header<<"  0x"<<mask<<dec<<"  "<<inorm<<"  "<<clkchan<<"  "<<clkfreq<<endl;
	}
	
	switch (imodel) {
	case 3801:   //Hall C Helicity Scalers (SIS 3801)
	  scalers.push_back(new Scaler3801(icrate, islot));
	  if(!fOnlyBanks) fRocSet.insert(icrate);
	  fModuleSet.insert(imodel);
	  break;
	}

	if (scalers.size() > 0) {
	  UInt_t idx = scalers.size()-1;
	  // Headers must be unique over whole event, not
	  // just within a ROC
	  scalers[idx]->SetHeader(header, mask);
	  // The normalization slot has the clock in it, so we automatically recognize it.
	  // fNormIdx is the index in scaler[] and 
	  // fNormSlot is the slot#, checked for consistency
	  if (clkchan >= 0) {
	    scalers[idx]->SetClock(defaultDT, clkchan, clkfreq);
	    cout << "Setting scaler clock ... channel = "<<clkchan<<" ... freq = "<<clkfreq<<endl;
	    if (fDebugFile) *fDebugFile <<"Setting scaler clock ... channel = "<<clkchan<<" ... freq = "<<clkfreq<<endl;
	    fNormIdx = idx;
	    if (islot != fNormSlot) cout << "THcHelicityScaler:: WARN: contradictory norm slot ! "<<islot<<endl;  
	    
	  }
	}	
	
      }
    }
    
  } //end while loop
  
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
  
  //Called after AddVars() has been called
  DefVars();
  
  // Verify that the slots are not defined twice
  for (UInt_t i1=0; i1 < scalers.size()-1; i1++) {
    for (UInt_t i2=i1+1; i2 < scalers.size(); i2++) {
      if (scalers[i1]->GetSlot()==scalers[i2]->GetSlot())
	cout << "THcHelicityScaler:: WARN:  same slot defined twice"<<endl;
    }
  }
  // Identify indices of scalers[] vector to variables.
  for (UInt_t i=0; i < scalers.size(); i++) {
    for (UInt_t j = 0; j < scalerloc.size(); j++) {
      if (scalerloc[j]->islot==static_cast<UInt_t>(scalers[i]->GetSlot()))
	scalerloc[j]->index = i;
    }
  }
  
  if(fDebugFile) *fDebugFile << "THcHelicityScaler:: Name of scaler bank "<<fName<<endl;
  for (size_t i=0; i<scalers.size(); i++) {
    if(fDebugFile) {
      *fDebugFile << "Scaler  #  "<<i<<endl;
      scalers[i]->SetDebugFile(fDebugFile);
      scalers[i]->DebugPrint(fDebugFile);
    }
  }

  
  //------Initialize Helicity Variables / Arrays-----
  fNTriggers = 0;
  fNTrigsInBuf = 0;
  fFirstCycle = -100;
  fRingSeed_reported = 0;
  fRingSeed_actual = 0;
  fNBits = 0;
  fNTriggersPlus = fNTriggersMinus = 0;
  fHScalers[0] = new Double_t[fNScalerChannels];  //+ helicity scaler counts  
  fHScalers[1] = new Double_t[fNScalerChannels];  //- helicity scaler counts
  fScalerSums = new Double_t[fNScalerChannels];   //total helicity scaler counts (hel=0, excluded)
  fScalerChan = new Double_t[fNScalerChannels]; //C.Y. 11/26/2020 : added array to store helicity information per channel 

  for(Int_t i=0;i<fNScalerChannels;i++) {
    fHScalers[0][i] = 0.0;
    fHScalers[1][i] = 0.0;
    fScalerSums[i] = 0.0;
    fScalerChan[i] = 0.0;
  }


  fTimePlus = fTimeMinus = 0.0;
  fTriggerAsymmetry = 0.0;



  //------C.Y.: 12/13/2020  Initialize Variables for quartet-by-quartet asymmetries (include BCM current threshold cut)---------
  //(and error calculation)

  for(Int_t i=0; i<4; i++) {
    
    fChargeCycle[i] = new Double_t[fNumBCMs];
    fScalCycle[i]   = new Double_t[fNScalerChannels];

    fHaveCycle[i] = kFALSE;
    
    for(Int_t j=0; j<fNumBCMs; j++) {
      fChargeCycle[i][j] = 0.0;
    }
    
    for(Int_t j=0; j<fNScalerChannels; j++) {
      fScalCycle[i][j] = 0.0;
    }    
    
    fTimeCycle[i] = 0.0;

  }

  //Initialize quartet counter
  fQuartetCount      = 0.0;
 
  //Initialize variables for time asymmetry calculation
  fTimeSum            = 0.0;
  fTimeAsymmetry      = 0.0;
  fTimeAsymmetryError = 0.0;
  fTimeAsymSum        = 0.0;
  fTimeAsymSum2       = 0.0;
 

  //Initialize variables for charge asymmetry calculation
  fChargeSum            = new Double_t[fNumBCMs];
  fChargeAsymmetry      = new Double_t[fNumBCMs];
  fChargeAsymmetryError = new Double_t[fNumBCMs];
  fChargeAsymSum        = new Double_t[fNumBCMs];
  fChargeAsymSum2       = new Double_t[fNumBCMs];

  for(Int_t i=0;i<fNumBCMs;i++) {
    fChargeSum[i]       = 0.0;
    fChargeAsymmetry[i] = 0.0;
    fChargeAsymSum[i]   = 0.0;
    fChargeAsymSum2[i]  = 0.0;
  }

  //Initialize variables for scaler asymmetry calculation
  fScalSum            = new Double_t[fNScalerChannels];
  fScalAsymmetry      = new Double_t[fNScalerChannels];
  fScalAsymmetryError = new Double_t[fNScalerChannels];
  fScalAsymSum        = new Double_t[fNScalerChannels];
  fScalAsymSum2       = new Double_t[fNScalerChannels];

  for(Int_t i=0;i<fNScalerChannels;i++) {
    fScalSum[i]       = 0.0;
    fScalAsymmetry[i] = 0.0;
    fScalAsymSum[i]   = 0.0;
    fScalAsymSum2[i]  = 0.0;
  }
  
  
  //------------------------------------------------------------------------------------------

  
  //Call MakeParms() to define variables to be used in report file 
  MakeParms();
  
  return kOK;
}

void THcHelicityScaler::MakeParms()
{
  
  //Put Various helicity scaler results in gHcParms so they can be included in results.

  //no bcm cut required
  gHcParms->Define(Form("g%s_hscaler_plus[%d]",fName.Data(),fNScalerChannels),
		   "Plus Helcity Scalers",*(fHScalers[0]));

  gHcParms->Define(Form("g%s_hscaler_minus[%d]",fName.Data(),fNScalerChannels),
		   "Minus Helcity Scalers",*(fHScalers[1]));

  //gHcParms->Define(Form("g%s_hscaler_sum[%d]",fName.Data(),fNScalerChannels),
  //		   "Helcity Scalers Sum",*fScalerSums);

  gHcParms->Define(Form("g%s_hscaler_triggers",fName.Data()),
		   "Total Helicity Scaler Triggers",fNTriggers);

  gHcParms->Define(Form("g%s_hscaler_triggers_plus",fName.Data()),
		   "Positive Helicity Scaler Triggers",fNTriggersPlus);

  gHcParms->Define(Form("g%s_hscaler_triggers_minus",fName.Data()),
		   "Negative Helicity Scaler Triggers",fNTriggersMinus);

  gHcParms->Define(Form("g%s_hscaler_trigger_asy",fName.Data()),  
                   "Helicity Trigger Asymmetry",fTriggerAsymmetry);

  //gHcParms->Define(Form("g%s_hscaler_time_plus",fName.Data()),
  //		   "Positive Helicity Time",fTimePlus);

  //gHcParms->Define(Form("g%s_hscaler_time_minus",fName.Data()),
  //		   "Negative Helicity Time",fTimeMinus);
  
  //bcm cut
  gHcParms->Define(Form("g%s_hscaler_sum[%d]",fName.Data(),fNScalerChannels),
		   "Helcity Scalers Sum",*fScalSum);

  gHcParms->Define(Form("g%s_hscaler_asy[%d]",fName.Data(),fNScalerChannels),
		   "Helicity Scaler Asymmetry[%d]",*fScalAsymmetry);

  gHcParms->Define(Form("g%s_hscaler_asyerr[%d]",fName.Data(),fNScalerChannels),
		   "Helicity Scaler Asymmetry Error[%d]",*fScalAsymmetryError);

  gHcParms->Define(Form("g%s_hscaler_charge[%d]",fName.Data(),fNumBCMs),
		   "Helicity Gated Charge",*fChargeSum);

  gHcParms->Define(Form("g%s_hscaler_charge_asy[%d]",fName.Data(),fNumBCMs),
		   "Helicity Gated Charge Asymmetry",*fChargeAsymmetry);

  gHcParms->Define(Form("g%s_hscaler_charge_asyerr[%d]",fName.Data(),fNumBCMs),
		   "Helicity Gated Charge Asymmetry Error",*fChargeAsymmetryError);
    
  gHcParms->Define(Form("g%s_hscaler_time",fName.Data()),
		   "Helicity Gated Time (sec)",fTimeSum);

  gHcParms->Define(Form("g%s_hscaler_time_asy",fName.Data()),
		   "Helicity Gated Time Asymmetry",fTimeAsymmetry);

  gHcParms->Define(Form("g%s_hscaler_time_asyerr",fName.Data()),  
                   "Helicity Gated Time Asymmetry Error",fTimeAsymmetryError);


  
}



//--------------------C.Y. Sep 20, 2020 : Added Utility Functions--------------------

void THcHelicityScaler::AddVars(TString name, TString desc, UInt_t islot,
				  UInt_t ichan, UInt_t ikind)
{
  if (fDebugFile) *fDebugFile << "C.Y. | Calling THcHelicityScaler::AddVars() "<<endl; 
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
  if (fDebugFile) *fDebugFile << "C.Y. | Calling THcHelicityScaler::DefVars() "<<endl; 
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
  if(fDebugFile) *fDebugFile << "THcHelicityScaler:: scalerloc size "<<scalerloc.size()<<endl;
  const Int_t* count = 0;
  for (size_t i = 0; i < scalerloc.size(); i++) {
    gHaVars->DefineByType(scalerloc[i]->name.Data(), scalerloc[i]->description.Data(),
			  &dvars[i], kDouble, count);
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
