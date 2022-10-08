//*-- Author :    Ole Hansen    August 2006
// Extracted from Bob Michaels' THaHelicity CVS 1.19
////////////////////////////////////////////////////////////////////////
//
// THcHelicityReader
//
////////////////////////////////////////////////////////////////////////

#include "THcHelicityReader.h"
#include "THaEvData.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "TMath.h"
#include "TError.h"
#include "VarDef.h"
#include "THaAnalysisObject.h"   // For LoadDB
#include <iostream>
#include <vector>
#include "TH1F.h"

#define DEFAULT_FADCTHRESHOLD 8000

using namespace std;

//____________________________________________________________________
THcHelicityReader::THcHelicityReader()
  : fTITime(0), fTITime_last(0), fTITime_rollovers(0), fFADCModule(0), 
    fTTimeDiff(0), fHaveROCs(kFALSE)
{
  // Default constructor
  
}
//____________________________________________________________________
THcHelicityReader::~THcHelicityReader() 
{
  // Destructor

  // Histograms will be deleted by ROOT
  // for( Int_t i = 0; i < NHISTR; ++i ) {
  //   delete fHistoR[i];
  // }
}

//____________________________________________________________________
void THcHelicityReader::Clear( Option_t* ) 
{
  
  fIsMPS = fIsQrt = fIsHelp = fIsHelm = kFALSE;

}

//_____________________________________________________________________________
Int_t THcHelicityReader::ReadDatabase( const char* /*dbfilename*/,
					    const char* /*prefix*/,
					    const TDatime& /*date*/,
					    int /*debug_flag*/ )
{

  // Eventually get these from the parameter file

  // SHMS settings see https://logbooks.jlab.org/entry/3614445
  cout << "THcHelicityReader: Helicity information from ROC 2 (SHMS)" << endl;
  SetROCinfo(kHel,2,14,9);
  SetROCinfo(kHelm,2,14,8);
  SetROCinfo(kMPS,2,14,10);
  SetROCinfo(kQrt,2,14,7);	// Starting about run 5818
  SetROCinfo(kTime,2,21,2);

  fADCThreshold = DEFAULT_FADCTHRESHOLD;
  fADCRawSamples = 0;
  
  DBRequest list[] = {
    {"helicity_adcthreshold",&fADCThreshold, kInt, 0, 1},
    {"helicity_adcrawsamples",&fADCRawSamples, kInt, 0, 1},
    {0}
  };

  gHcParms->LoadParmValues(list, "");

  return THaAnalysisObject::kOK;
}

//_____________________________________________________________________________
void THcHelicityReader::Begin()
{ 
  // static const char* const here = "THcHelicityReader::Begin";
  // cout<<here<<endl;

  fTITime_last = 0;
  fTITime = 0;
  fTITime_rollovers = 0;

  return;
}
//____________________________________________________________________
void THcHelicityReader::End()
{
  // static const char* const here = "THcHelicityReader::End";
  // cout<<here<<endl;

  return;
}
//____________________________________________________________________
Int_t THcHelicityReader::ReadData( const THaEvData& evdata ) 
{
  // Obtain the present data from the event for QWEAK helicity mode.

  static const char* here = "THcHelicityReader::ReadData";

  //  std::cout<<" kHel, kTime, kRing="<< kHel<<" "<<kTime<<" "<<kRing<<endl;
  //     for (int jk=0; jk<3; jk++)
  //     {
  //       std::cout<<" which="<<jk
  // 	       <<" roc="<<fROCinfo[jk].roc
  // 	       <<" header="<<fROCinfo[jk].header
  // 	       <<" index="<<fROCinfo[jk].index 
  // 	       <<endl;
  //     }
  
  //  std::cout<<" fHaveROCs="<<fHaveROCs<<endl;
  if( !fHaveROCs ) {
    ::Error( here, "ROC data (detector map) not properly set up." );
    return -1;
  }

  // Check if ROC info is correct

  if(!evdata.GetModule(fROCinfo[kTime].roc, fROCinfo[kTime].slot)) {
    //    cout << "ROC, Slot: " << fROCinfo[kTime].roc << " " << fROCinfo[kTime].slot << endl;
    //    cout << "Evtype = " << evdata.GetEvType() << endl;
    cout << "THcHelicityReader: ROC 2 not found" << endl;
    cout << "Changing to ROC 1 (HMS)" << endl;
    SetROCinfo(kHel,1,18,9);
    SetROCinfo(kHelm,1,18,8);
    SetROCinfo(kMPS,1,18,10);
    SetROCinfo(kQrt,1,18,7);
    SetROCinfo(kTime,1,21,2);
  }

  // Get the TI Data
  UInt_t titime = (UInt_t) evdata.GetData(fROCinfo[kTime].roc,
					  fROCinfo[kTime].slot,
					  fROCinfo[kTime].index, 0);
  
  if(!fFADCModule) {
    fFADCModule = dynamic_cast<Decoder::Fadc250Module*>
      (evdata.GetModule(fROCinfo[kHel].roc, fROCinfo[kHel].slot));
    fTTimeDiff = titime - fFADCModule->GetTriggerTime();
  }

  if(titime == 0) {
    cout << "Event " << evdata.GetEvNum() << " TI Trigger time missing." << endl;
    //    cout << "Event " << evdata.GetEvNum() << " TI Trigger time missing.  Using FADC trigger time" << endl;
    //    cout << "TI Evnum: " << fROCinfo[kTime].roc << " " <<
    //      evdata.GetData(fROCinfo[kTime].roc,fROCinfo[kTime].slot, 0, 0)
    //	 << " " << evdata.GetData(fROCinfo[kTime].roc,fROCinfo[kTime].slot, 1, 0) << endl;
    //    titime = fFADCModule->GetTriggerTime() + fTTimeDiff;
  }

  //  cout << "Time " << titime << " " << fTITime_last << " " <<
  //    evdata.GetData(1,21,2,0) << " " <<
  //    evdata.GetData(2,21,2,0) << " " << fFADCModule->GetTriggerTime() << endl;

  // Check again if ROC info is correct
  //    cout << "Evtype = " << evdata.GetEvType() << endl;
#if 0
  if(titime == 0 && fTITime_last==0) {
    cout << "B ROC, Slot: " << fROCinfo[kTime].roc << " " << fROCinfo[kTime].slot << endl;
    cout << "THcHelicityReader: ROC 2 not found" << endl;
    cout << "Changing to ROC 1 (HMS)" << endl;
    SetROCinfo(kHel,1,18,9);
    SetROCinfo(kHelm,1,18,8);
    SetROCinfo(kMPS,1,18,10);
    SetROCinfo(kQrt,1,18,7);
    SetROCinfo(kTime,1,21,2);
    titime = (UInt_t) evdata.GetData(fROCinfo[kTime].roc,
					  fROCinfo[kTime].slot,
					  fROCinfo[kTime].index, 0);
  }
#endif

  //  cout << fTITime_last << " " << titime << endl;
  if(titime < fTITime_last) {
    fTITime_rollovers++;
  }
  fTITime = titime + fTITime_rollovers*4294967296;
  fTITime_last = titime;

  const_cast<THaEvData&>(evdata).SetEvTime(fTITime);

  // Get the helicity control signals.  These are from the pedestals
  // acquired by FADC channels.  If helpraw and helmraw both
  // come back zero, assume that we have raw sample data instead
  // and switch to using raw sample data for the rest of the run.
  // If the FADC threshold has not be set by the user, set the threshold
  // to the default threshold divided by 4.

  Int_t helpraw, helmraw, mpsraw, qrtraw;
  if(!fADCRawSamples) {
    helpraw = evdata.GetData(Decoder::kPulsePedestal,
			     fROCinfo[kHel].roc,
			     fROCinfo[kHel].slot,
			     fROCinfo[kHel].index, 0);
    helmraw = evdata.GetData(Decoder::kPulsePedestal,
			     fROCinfo[kHelm].roc,
			     fROCinfo[kHelm].slot,
			     fROCinfo[kHelm].index, 0);
    mpsraw = evdata.GetData(Decoder::kPulsePedestal,
			    fROCinfo[kMPS].roc,
			    fROCinfo[kMPS].slot,
			    fROCinfo[kMPS].index, 0);
    qrtraw = evdata.GetData(Decoder::kPulsePedestal,
			    fROCinfo[kQrt].roc,
			    fROCinfo[kQrt].slot,
			    fROCinfo[kQrt].index, 0);
    if(helpraw <= 0 && helmraw <=0) {
      fADCRawSamples = 1;
      if(fADCThreshold == DEFAULT_FADCTHRESHOLD) {
	fADCThreshold = DEFAULT_FADCTHRESHOLD/4;
      }
    }
  }
  if(fADCRawSamples) {
    helpraw = evdata.GetData(Decoder::kSampleADC,
			     fROCinfo[kHel].roc,
			     fROCinfo[kHel].slot,
			     fROCinfo[kHel].index, 0);
    helmraw = evdata.GetData(Decoder::kSampleADC,
			     fROCinfo[kHelm].roc,
			     fROCinfo[kHelm].slot,
			     fROCinfo[kHelm].index, 0);
    mpsraw = evdata.GetData(Decoder::kSampleADC,
			    fROCinfo[kMPS].roc,
			    fROCinfo[kMPS].slot,
			    fROCinfo[kMPS].index, 0);
    qrtraw = evdata.GetData(Decoder::kSampleADC,
			    fROCinfo[kQrt].roc,
			    fROCinfo[kQrt].slot,
			    fROCinfo[kQrt].index, 0);
  }
    
  fIsQrt = qrtraw > fADCThreshold;
  fIsMPS = mpsraw > fADCThreshold;
  fIsHelp = helpraw > fADCThreshold;
  fIsHelm = helmraw > fADCThreshold;

  //  cout << helpraw << " " << helmraw << " " << mpsraw << " " << qrtraw << endl;
  //  cout << fADCThreshold << " " << fADCRawSamples << " " << fIsQrt << " " << fIsMPS << " " << fIsHelp << " " << fIsHelm << endl;

  return 0;
}

//TODO: this should not be needed once LoadDB can fill fROCinfo directly
//____________________________________________________________________
Int_t THcHelicityReader::SetROCinfo( EROC which, Int_t roc,
				       Int_t slot, Int_t index )
{

  // Define source and offset of data.  Normally called by ReadDatabase
  // of the detector that is a THcHelicityReader.
  //
  // "which" is one of  { kHel, kKelm, kQrt, kTime }.
  // You must define at least the kHel and kTime ROCs.
  // Returns <0 if parameter error, 0 if success

  if( which<kHel || which>=kCount )
    return -1;
  if( roc <= 0 || roc > 255 )
    return -2;

  fROCinfo[which].roc    = roc;
  fROCinfo[which].slot = slot;
  fROCinfo[which].index  = index;

  cout << "SetROCInfo: " << which << " " << fROCinfo[which].roc << " " << fROCinfo[which].slot <<
    " " << fROCinfo[which].index << endl;
  fHaveROCs = (fROCinfo[kHel].roc > 0 && fROCinfo[kTime].roc > 0 && fROCinfo[kMPS].roc);
  
  return 0;
}

//____________________________________________________________________
ClassImp(THcHelicityReader)

