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
    fUseFirstEvent(kTRUE),
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

  
  //C.Y. Nov 26, 2020: The destructor has be updated to delete the pointer variables that were initialized in the constructor..
  //---------------------------------------
  if (!TROOT::Initialized()) {
    delete fScalerTree;
  }
  Podd::DeleteContainer(scalers);
  Podd::DeleteContainer(scalerloc);
  delete [] dvars_prev_read;
  delete [] dvars;
  delete [] dvarsFirst;
  delete [] fBCM_SatOffset;
  delete [] fBCM_SatQuadratic;
  delete [] fBCM_delta_charge;
  //--------------------------------------
  
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

  
  //C.Y. Nov 26, 2020 : This method has been updated to include additional BCM parameters. See ReadDatabase() in THcScalerEvtHandler.cxx
  
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
    fBCM_Name.push_back(bcm_names[i]+".scal");
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
  
  //THcScalerEvtHandler::Analyze() uses this flag (which is forced to be 1),
  //but as to why, it is beyond me. For consistency, I have also used it here.
  Int_t lfirst=1;  
   
  if ( !IsMyEvent(evdata->GetEvType()) ) return -1;

  if (fDebugFile) {
    *fDebugFile << endl << "---------------------------------- "<<endl<<endl;
    *fDebugFile << "\nEnter THcHelicityScaler  for fName = "<<fName<<endl;
    EvDump(evdata);
  }

  //C.Y. Nov 26, 2020 : Here goes the code to create a Helicity Scaler TTree, add the variables obtained in AnalyzeBuffer(), and Fill the tree.
  //(See Analyze() method in THcScalerEvtHandler.cxx)
  
  if (lfirst && !fScalerTree) {

    lfirst = 0; // Can't do this in Init for some reason

    //Assign a name to the helicity scaler tree
    TString sname1 = "TSHel";
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
    fScalerTree->Branch(name.Data(), &evcountR, tinfo.Data(), 4000);
 
   name = "evNumber";
    tinfo = name + "/D";
    fScalerTree->Branch(name.Data(), &evNumberR, tinfo.Data(), 4000);

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
  } else { 			// A normal event
    if (fDebugFile) *fDebugFile<<"\n\nTHcHelicityScaler :: Debugging event type "<<dec<<evdata->GetEvType()<< " event num = " << evdata->GetEvNum() << endl<<endl;
    evNumber=evdata->GetEvNum();
    evNumberR = evNumber;
    Int_t ret;
    if((ret=AnalyzeBuffer(rdata))) {
      if (fDebugFile) *fDebugFile << "scaler tree ptr  "<<fScalerTree<<endl;
      if (fScalerTree) fScalerTree->Fill();
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

	    //C.Y. 11/26/2020 This methods extracts the raw helicity information and writes to arrays
	    AnalyzeHelicityScaler(p+index); 
	    //	    cout << "H: " << evNumber << endl;
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

  //Sets the helicity scaler clock to define the time
  Double_t scal_current=0;
  UInt_t thisClock = scalers[fNormIdx]->GetData(fClockChan);
  if(thisClock < fLastClock) {	// Count clock scaler wrap arounds
    fClockOverflows++;
  }
  fTotalTime = (thisClock+(((Double_t) fClockOverflows)*kMaxUInt+fClockOverflows))/fClockFreq;
  fLastClock = thisClock;
  fDeltaTime= fTotalTime - fPrevTotalTime;
  if (fDeltaTime==0) {
    cout << " *******************   Severe Warning ****************************" << endl;
    cout << " In THcScalerEvtHandler have found fDeltaTime is zero !!   " << endl;
      cout << " ******************* Alert DAQ experts ****************************" << endl;
  }
  fPrevTotalTime=fTotalTime;
  
  //C.Y. Sep 19, 2020 : Here goes the code to write the helicity raw data to a variable
  //and to map the variable to the scaler location //( See THcScalerEvtHandler::AnalyzeBuffer() )

  Int_t nscal=0;
  for (size_t i = 0; i < scalerloc.size(); i++)  {
    size_t ivar = scalerloc[i]->ivar;
    size_t idx = scalerloc[i]->index;
    size_t ichan = scalerloc[i]->ichan;
    if (evcount==0) {
      if (fDebugFile) *fDebugFile << "Debug dvarsFirst "<<i<<"   "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      if ((ivar < scalerloc.size()) &&
	  (idx < scalers.size()) &&
	  (ichan < MAXCHAN)){
	if(fUseFirstEvent){
	  if (scalerloc[ivar]->ikind == ICOUNT){
	    UInt_t scaldata = fScalerChan[ichan];   //Pass the helicity information per channel to scaldata
	    dvars[ivar] = scaldata;
	    scal_present_read.push_back(scaldata);
	    scal_prev_read.push_back(0);
	    scal_overflows.push_back(0);
	    dvarsFirst[ivar] = 0.0;
	  }
	  if (scalerloc[ivar]->ikind == ITIME){
	    dvars[ivar] =fTotalTime;
	    dvarsFirst[ivar] = 0;
	  }
	  if (scalerloc[ivar]->ikind == IRATE) {
	    dvars[ivar] = (fScalerChan[ichan])/fDeltaTime;
	    dvarsFirst[ivar] = dvars[ivar];            
	    //printf("%s %f\n",scalerloc[ivar]->name.Data(),scalers[idx]->GetRate(ichan)); //checks
	  }
	  if(scalerloc[ivar]->ikind == ICURRENT || scalerloc[ivar]->ikind == ICHARGE){
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
                 dvars[ivar]=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		 dvars[ivar]=dvars[ivar]+fBCM_SatOffset[bcm_ind]*TMath::Max(dvars[ivar]-fBCM_SatOffset[i],0.0);
	      }
         	if (bcm_ind == fbcm_Current_Threshold_Index) scal_current= dvars[ivar];
	    }
	    if (scalerloc[ivar]->ikind == ICHARGE) {
	      if (bcm_ind != -1) {
		Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.0);
		fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
		dvars[ivar]+=fBCM_delta_charge[bcm_ind];
	      }
	    }
	    //	    printf("1st event %i index %i fBCMname %s scalerloc %s offset %f gain %f computed %f\n",evcount, bcm_ind, fBCM_Name[bcm_ind],scalerloc[ivar]->name.Data(),fBCM_Offset[bcm_ind],fBCM_Gain[bcm_ind],dvars[ivar]);
	  }
	  
	  if (fDebugFile) *fDebugFile << "   dvarsFirst  "<<scalerloc[ivar]->ikind<<"  "<<dvarsFirst[ivar]<<endl;
	  
	} else { //ifnotusefirstevent
	  if (scalerloc[ivar]->ikind == ICOUNT) {
              dvarsFirst[ivar] = fScalerChan[ichan];  //Pass the helicity information per channel to dvarsFirst
              scal_present_read.push_back(dvarsFirst[ivar]);
              scal_prev_read.push_back(0);
	  }
	  if (scalerloc[ivar]->ikind == ITIME){
	    dvarsFirst[ivar] = fTotalTime;
	  }
	  if (scalerloc[ivar]->ikind == IRATE)  {
	    dvarsFirst[ivar] = (fScalerChan[ichan])/fDeltaTime;
 	    //printf("%s %f\n",scalerloc[ivar]->name.Data(),scalers[idx]->GetRate(ichan)); //checks
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
	        dvarsFirst[ivar]=0.0;
                if (bcm_ind != -1) {
                 dvarsFirst[ivar]=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		 dvarsFirst[ivar]=dvarsFirst[ivar]+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(dvars[ivar]-fBCM_SatOffset[i],0.0),2.);
		}
         	if (bcm_ind == fbcm_Current_Threshold_Index) scal_current= dvarsFirst[ivar];
	    }
	    if (scalerloc[ivar]->ikind == ICHARGE) {
	      if (bcm_ind != -1) {
		Double_t cur_temp=((fScalerChan[ichan])/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.);
		fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
               dvarsFirst[ivar]+=fBCM_delta_charge[bcm_ind];
	      }
	    }
	    }
	  if (fDebugFile) *fDebugFile << "   dvarsFirst  "<<scalerloc[ivar]->ikind<<"  "<<dvarsFirst[ivar]<<endl;
	}
      }
      else {
	cout << "THcScalerEvtHandler:: ERROR:: incorrect index "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      }
    }else{ // evcount != 0
      if (fDebugFile) *fDebugFile << "Debug dvars "<<i<<"   "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      if ((ivar < scalerloc.size()) &&
	  (idx < scalers.size()) &&
	  (ichan < MAXCHAN)) {
	if (scalerloc[ivar]->ikind == ICOUNT) {
	    UInt_t scaldata = fScalerChan[ichan];
	    if(scaldata < scal_prev_read[nscal]) {
	      scal_overflows[nscal]++;
	    }
            dvars[ivar] = scaldata + (1+((Double_t)kMaxUInt))*scal_overflows[nscal]
	      -dvarsFirst[ivar];
            scal_present_read[nscal]=scaldata;
	    nscal++;
	}
	if (scalerloc[ivar]->ikind == ITIME) {
	  dvars[ivar] = fTotalTime;
	}
	if (scalerloc[ivar]->ikind == IRATE) {
	  UInt_t scaldata = fScalerChan[ichan];
	  UInt_t diff;
	  if(scaldata < scal_prev_read[nscal-1]) {
	    diff = (kMaxUInt-(scal_prev_read[nscal-1] - 1)) + scaldata;
	  } else {
	    diff = scaldata - scal_prev_read[nscal-1];
	  }
	  dvars[ivar] =  diff/fDeltaTime;
	  // printf("%s %f\n",scalerloc[ivar]->name.Data(),scalers[idx]->GetRate(ichan));//checks
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
              dvars[ivar]=0;
	      if (bcm_ind != -1) {
		UInt_t scaldata = fScalerChan[ichan];
		UInt_t diff;
		if(scaldata < scal_prev_read[nscal-1]) {
		  diff = (kMaxUInt-(scal_prev_read[nscal-1] - 1)) + scaldata;
		} else {
		  diff = scaldata - scal_prev_read[nscal-1];
		}
		dvars[ivar]=0.;
 		if (fDeltaTime>0) {
		Double_t cur_temp=(diff/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.);
		  
		dvars[ivar]=cur_temp;
		}
	      }
	      if (bcm_ind == fbcm_Current_Threshold_Index) scal_current= dvars[ivar];
	    }
	    if (scalerloc[ivar]->ikind == ICHARGE) {
	      if (bcm_ind != -1) {
		UInt_t scaldata = fScalerChan[ichan];
		UInt_t diff;
		if(scaldata < scal_prev_read[nscal-1]) {
		  diff = (kMaxUInt-(scal_prev_read[nscal-1] - 1)) + scaldata;
		} else {
		  diff = scaldata - scal_prev_read[nscal-1];
		}
		fBCM_delta_charge[bcm_ind]=0;
		if (fDeltaTime>0)  {
		  Double_t cur_temp=(diff/fDeltaTime-fBCM_Offset[bcm_ind])/fBCM_Gain[bcm_ind];
		  cur_temp=cur_temp+fBCM_SatQuadratic[bcm_ind]*TMath::Power(TMath::Max(cur_temp-fBCM_SatOffset[bcm_ind],0.0),2.);
		fBCM_delta_charge[bcm_ind]=fDeltaTime*cur_temp;
		}
                 dvars[ivar]+=fBCM_delta_charge[bcm_ind];
	      }
	    }
	    //	    printf("event %i index %i fBCMname %s scalerloc %s offset %f gain %f computed %f\n",evcount, bcm_ind, fBCM_Name[bcm_ind],scalerloc[ivar]->name.Data(),fBCM_Offset[bcm_ind],fBCM_Gain[bcm_ind],dvars[ivar]);
	  }
	if (fDebugFile) *fDebugFile << "   dvars  "<<scalerloc[ivar]->ikind<<"  "<<dvars[ivar]<<endl;
      } else {
	cout << "THcScalerEvtHandler:: ERROR:: incorrect index "<<ivar<<"  "<<idx<<"  "<<ichan<<endl;
      }
    }
    
  }

  //----

  for (size_t i = 0; i < scalerloc.size(); i++)  {
    size_t ivar = scalerloc[i]->ivar;
    size_t idx = scalerloc[i]->index;
    size_t ichan = scalerloc[i]->ichan;
    if (scalerloc[ivar]->ikind == ICUT+ICOUNT){
      UInt_t scaldata = fScalerChan[ichan];
      if ( scal_current > fbcm_Current_Threshold) {
	UInt_t diff;
	if(scaldata < dvars_prev_read[ivar]) {
	  diff = (kMaxUInt-(dvars_prev_read[ivar] - 1)) + scaldata;
	} else {
	  diff = scaldata - dvars_prev_read[ivar];
	}
	dvars[ivar] += diff;
      } 
      dvars_prev_read[ivar] = scaldata;
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
	dvars[ivar] += fBCM_delta_charge[bcm_ind];
      } 
    }
    if (scalerloc[ivar]->ikind == ICUT+ITIME){
      if ( scal_current > fbcm_Current_Threshold) {
	dvars[ivar] += fDeltaTime;
      } 
    }
  }
  
  //
  evcount = evcount + 1;
  evcountR = evcount;
  //
  for (size_t j=0; j<scal_prev_read.size(); j++) scal_prev_read[j]=scal_present_read[j];
  //  
  for (size_t j=0; j<scalers.size(); j++) scalers[j]->Clear("");
  
  
  
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

  //C.Y. 11/26/2020  The block of code below is used to extract the helicity information from each
  //channel of the helicity scaler onto a variable to be stored in the scaler tree. For each channel,
  //each helicity state (+, -, or MPS (undefined)) is stored in a single varibale. Each helicity state
  //will be tagged separately later on.

  //C.Y. 11/26/2020  Loop over all 32 scaler channels for a specific helicity scaler module (SIS 3801)
    for(Int_t i=0;i<fNScalerChannels;i++) {

      //C.Y. 11/26/2020 the count expression below gets the scaler raw helicity information (+, -, or MPS helicity states) for the ith channel
      Int_t count = p[i]&0xFFFFFF; // Bottom 24 bits  equivalent of scalers->Decode()

      fScalerChan[i] = count;        //pass the helicity raw information to each helicity scaler channel array element
    }


    


  //C.Y. 11/26/2020  The block of code below is used to get a cumulative sum of +/- helicity used
  //for calculation of beam charge asymmetry
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

      //Increment either the '+' (hindex=0) or '-' (hindex=1) helicity counts for each [i] scaler channel channel of a given module
      fHScalers[hindex][i] += count;
      fScalerSums[i] += count;

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
	if ((fNormSlot >= 0) && (fNormSlot != inorm)) cout << "THcScalerEvtHandler::WARN:  contradictory norm slot  "<<fNormSlot<<"   "<<inorm<<endl;
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
	    if (islot != fNormSlot) cout << "THcScalerEvtHandler:: WARN: contradictory norm slot ! "<<islot<<endl;  
	    
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
	cout << "THcScalerEvtHandler:: WARN:  same slot defined twice"<<endl;
    }
  }
  // Identify indices of scalers[] vector to variables.
  for (UInt_t i=0; i < scalers.size(); i++) {
    for (UInt_t j = 0; j < scalerloc.size(); j++) {
      if (scalerloc[j]->islot==static_cast<UInt_t>(scalers[i]->GetSlot()))
	scalerloc[j]->index = i;
    }
  }
  
  if(fDebugFile) *fDebugFile << "THcScalerEvtHandler:: Name of scaler bank "<<fName<<endl;
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
  fHScalers[0] = new Double_t[fNScalerChannels];
  fHScalers[1] = new Double_t[fNScalerChannels];
  fScalerSums = new Double_t[fNScalerChannels];
  fScalerChan = new Double_t[fNScalerChannels]; //C.Y. 11/26/2020 : added array to store helicity information per channel 
  fAsymmetry = new Double_t[fNScalerChannels];
  fAsymmetryError = new Double_t[fNScalerChannels];

  for(Int_t i=0;i<fNScalerChannels;i++) {
    fHScalers[0][i] = 0.0;
    fHScalers[1][i] = 0.0;
    fScalerSums[0] = 0.0;
    fScalerChan[0] = 0.0;
    fAsymmetry[0] = 0.0;
    fAsymmetryError[0] = 0.0;
  }

  fCharge = new Double_t[fNumBCMs];
  fChargeAsymmetry = new Double_t[fNumBCMs];

  fTime = fTimeAsymmetry = 0;
  fTriggerAsymmetry = 0.0;

  MakeParms();

  
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
