//*-- Author :    Julie Roche, November 2010
// this is a modified version of THaGOHelicity.C
////////////////////////////////////////////////////////////////////////
//
// THcHelicity
//
// Helicity of the beam from QWEAK electronics in delayed mode
//    +1 = plus,  -1 = minus,  0 = unknown
//
// Also supports in-time mode with delay = 0
// 
////////////////////////////////////////////////////////////////////////

#include "THcHelicity.h"

#include "THaApparatus.h"
#include "THaEvData.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "TH1F.h"
#include "TMath.h"
#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcHelicity::THcHelicity( const char* name, const char* description,
				    THaApparatus* app ):
  THaHelicityDet( name, description, app ), 
  fnQrt(-1), fHelDelay(8), fMAXBIT(30)
{
  //  for( Int_t i = 0; i < NHIST; ++i )
  //    fHisto[i] = 0;
  //  memset(fHbits, 0, sizeof(fHbits));
}

//_____________________________________________________________________________
THcHelicity::THcHelicity()
  : fnQrt(-1), fHelDelay(8), fMAXBIT(30)
{
  // Default constructor for ROOT I/O

  //  for( Int_t i = 0; i < NHIST; ++i )
  //    fHisto[i] = 0;
}

//_____________________________________________________________________________
THcHelicity::~THcHelicity() 
{
  DefineVariables( kDelete );

  // for( Int_t i = 0; i < NHIST; ++i ) {
  //   delete fHisto[i];
  // }
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHelicity::Init(const TDatime& date) {
  // Call `Setup` before everything else.
  Setup(GetName(), GetTitle());

  fFirstEvProcessed = kFALSE;
  fActualHelicity = kUnknown;
  fPredictedHelicity = kUnknown;
  fLastMPSTime = 0;
  fFoundMPS = kFALSE;

  // Call initializer for base class.
  // This also calls `ReadDatabase` and `DefineVariables`.
  EStatus status = THaDetector::Init(date);
  if (status) {
    fStatus = status;
    return fStatus;
  }

  fStatus = kOK;
  return fStatus;
}

//_____________________________________________________________________________
void THcHelicity::Setup(const char* name, const char* description) {
  // Prefix for parameters in `param` file.
  string kwPrefix = string(GetApparatus()->GetName()) + "_" + name;
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
}

//_____________________________________________________________________________
Int_t THcHelicity::ReadDatabase( const TDatime& date )
{

  cout << "In THcHelicity::ReadDatabase" << endl;
  // Read general HelicityDet database values (e.g. fSign)
  //  Int_t st = THaHelicityDet::ReadDatabase( date );
  //  if( st != kOK )
  //    return st;

  // Read readout parameters (ROC addresses etc.)
  Int_t st = THcHelicityReader::ReadDatabase( GetDBFileName(), GetPrefix(),
					date, fQWEAKDebug );
  if( st != kOK )
      return st;

  fSign = 1;			// Default helicity sign
  fRingSeed_reported_initial = 0; // Initial see that should predict reported
                                // helicity of first quartet.
  fFirstCycle = -1; // First Cycle that starts a quad (0 to 3)
  fFreq = 29.5596;
  fHelDelay=8;

  DBRequest list[]= {
    //	  {"_hsign", &fSign, kInt, 0, 1},
    {"helicity_delay", &fHelDelay, kInt, 0, 1},
    {"helicity_freq", &fFreq, kDouble, 0, 1},
    //    {"helicity_seed", &fRingSeed_reported_initial, kInt, 0, 1},
    //    {"helicity_cycle", &fFirstCycle, kInt, 0, 1},
    {0}
  };

  gHcParms->LoadParmValues(list, "");

  fMAXBIT=30;

  fTIPeriod = 250000000.0/fFreq;

  // maximum of event in the pattern, for now we are working with quartets
  //  Int_t localpattern[4]={1,-1,-1,1}; 
  // careful, the first value here should always +1
  //  for(int i=0;i<fQWEAKNPattern;i++)
  //    {
  //      fPatternSequence.push_back(localpattern[i]);
  //    }
  HWPIN=kTRUE;

  fQuartet[0]=fQuartet[1]=fQuartet[2]=fQuartet[3]=0;

  if (fFirstCycle>=0 && fRingSeed_reported_initial!=0) {
    // Set the seed for predicted reported and predicted actual
  } else {
    // Initialize mode to find quartets and then seed
  }

  return kOK;
}

//_____________________________________________________________________________

void THcHelicity::MakePrefix()
{
 THaDetector::MakePrefix();
}
//_____________________________________________________________________________
Int_t THcHelicity::DefineVariables( EMode mode )
{
  // Initialize global variables

  cout << "Called THcHelicity::DefineVariables with mode == "
       << mode << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Define standard variables from base class
  THaHelicityDet::DefineVariables( mode );

  const RVarDef var[] = {
    { "nqrt",     "position of cycle in quartet",          "fnQrt" },
    { "hel",      "actual helicity for event",             "fActualHelicity" },
    { "helrep",   "reported helicity for event",           "fReportedHelicity" },
    { "helpred",  "predicted reported helicity for event", "fPredictedHelicity" },
    { "mps", "In MPS blanking period", "fMPS"},
    { 0 }
  };
  cout << "Calling THcHelicity DefineVarsFromList" << endl;
  return DefineVarsFromList( var, mode );
}
//_____________________________________________________________________________

void THcHelicity::PrintEvent(Int_t evtnum)
{

  cout<<" ++++++ THcHelicity::Print ++++++\n";

  cout<<" +++++++++++++++++++++++++++++++++++++\n";

  return;
}

//_____________________________________________________________________________
Int_t THcHelicity::Begin( THaRunBase* )
{
  THcHelicityReader::Begin();

  //  fHisto[0] = new TH1F("hel.seed","hel.seed",32,-1.5,30.5);
  //  fHisto[1] = new TH1F("hel.error.code","hel.error.code",35,-1.5,33.5);
 
  return 0;
}

//_____________________________________________________________________________
//void THcHelicity::FillHisto()
//{
//  fHisto[0]->Fill(fRing_NSeed);
//  fHisto[1]->Fill(fErrorCode);
//  return;
//}
//_____________________________________________________________________________
void THcHelicity::SetErrorCode(Int_t error)
{
 // used as a control for the helciity computation
  // 2^0: if the reported number of events in a  pattern is larger than fQWEAKNPattern
  // 2^1: if the offset between the ring reported value and TIR value is not fOffsetTIRvsRing
  // 2^2: if the reported time in the ring is 0
  // 2^3: if the predicted reported helicity doesn't match the reported helicity in the ring
  // 2^4: if the helicity cannot be computed using the SetHelicity routine
  // 2^5: if seed is being gathered

  if(fErrorCode==0)
    fErrorCode=(1<<error);
  // only one reported error at the time

  return;
}

//_____________________________________________________________________________
void THcHelicity::Clear( Option_t* opt ) 
{
  // Clear event-by-event data

  THaHelicityDet::Clear(opt);
  THcHelicityReader::Clear(opt);
  fEvtype = 0;

  fQrt=0;
  fErrorCode=0;

  return;
}

//_____________________________________________________________________________
Int_t THcHelicity::Decode( const THaEvData& evdata )
{

  // Decode Helicity data.
  // Return 1 if helicity was assigned, 0 if not, <0 if error.

  Int_t err = ReadData( evdata ); // from THcHelicityReader class
  if( err ) {
    Error( Here("THcHelicity::Decode"), "Error decoding helicity data." );
    return err;
  }

  fReportedHelicity = (fIsHelp?(fIsHelm?kUnknown:kPlus):(fIsHelm?kMinus:kUnknown));
  fMPS = fIsMPS?1:0;
  if(fHelDelay == 0) {		// If no delay actual=reported (but zero if in MPS)
    fActualHelicity = fIsMPS?kUnknown:fReportedHelicity;
    return 0;
  }
  Long64_t lastlastmpstime = fLastMPSTime;
  if(fFirstEvProcessed) {	// Normal processing
    Int_t missed = 0;
    //    Double_t elapsed_time = (fTITime - fFirstEvTime)/250000000.0;
    if(fIsMPS) {
      if(fFoundMPS) {
	missed = TMath::Nint(fTITime/fTIPeriod-fLastMPSTime/fTIPeriod);
	if(missed < 1) { // was <=1
	  fLastMPSTime = (fTITime+fLastMPSTime+missed*fTIPeriod)/2;
	  fIsNewCycle = kTRUE;
	  fActualHelicity = kUnknown;
	  fPredictedHelicity = kUnknown;
	} else {
	  fLastMPSTime = (fLastMPSTime + fTITime - missed*fTIPeriod)/2;
	}
	// If there is a skip, pass it off to next non MPS event
	// Need to also check here for missed MPS's
	//      cout << "Found MPS" << endl;
	// check for Nint((time-last)/period) > 1
      } else {
	fFoundMPS = kTRUE;
	fLastMPSTime = fTITime;
      }
    } else if (fFoundMPS) {	//
      if(fTITime - fLastMPSTime > fTIPeriod) { // We missed MPS periods
	missed = TMath::Nint(floor((fTITime-fLastMPSTime)/fTIPeriod));
	if(missed > 1) {
	  //	  cout << "Missed " << missed << " MPSes" << endl;
	  Int_t newNCycle = fNCycle + missed -1; // How many cycles really missed
	  Int_t quartets_missed = (newNCycle-fFirstCycle)/4 - (fNCycle-fFirstCycle)/4;
	  for(Int_t i=0;i<quartets_missed;i++) { // Advance the seeds.
	    fRingSeed_reported = RanBit30(fRingSeed_reported);
	    fRingSeed_actual = RanBit30(fRingSeed_actual);
	  }
	  int quartetphase = (newNCycle-fFirstCycle)%4;
	  //	  cout << "  " << fNCycle << " " << newNCycle << " " << fFirstCycle << " " << quartets_missed << " " << quartetphase << endl;
	  fQuartetStartHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
	  fQuartetStartPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
	  fActualHelicity = (quartetphase==0||quartetphase==3)?
	    fQuartetStartHelicity:-fQuartetStartHelicity;
	  fPredictedHelicity = (quartetphase==0||quartetphase==3)?
	    fQuartetStartPredictedHelicity:-fQuartetStartPredictedHelicity;
	  //	  cout << "Cycles " << fNCycle << " " << newNCycle << " " << fFirstCycle 
	  //	       << " skipped " << quartets_missed << " quartets" << endl;
	  fNCycle = newNCycle;
	  // Need to reset fQuartet to reflect where we are based on the current
	  // reported helicity.  So we don't fail quartet testing.
	  // But only do this if we are calibrated.
	  if(fNBits >= fMAXBIT) {
	    if (((fNCycle - fFirstCycle)%2)==1) {
	      fQuartet[0] = fReportedHelicity;
	      fQuartet[1] = fQuartet[2] = -fQuartet[0];
	    } else {
	      fQuartet[0] = fQuartet[1] = -fReportedHelicity;
	      fQuartet[2] = -fQuartet[1];
	    }
	  } else {
	    fQuartet[0] = fReportedHelicity;
	    fQuartet[1] = 0;
	  }
	}
	fLastMPSTime += missed*fTIPeriod;
	fIsNewCycle = kTRUE;
	fLastReportedHelicity = fReportedHelicity;
      }
      if(fIsNewCycle) {
	fQuartet[3]=fQuartet[2]; fQuartet[2]=fQuartet[1]; fQuartet[1]=fQuartet[0];
	fQuartet[0]=fReportedHelicity;
	fNCycle++;
	if((fNCycle-fFirstCycle)%4 == 3) {// Test if last in a quartet
	  if((abs(fQuartet[0]+fQuartet[3]-fQuartet[1]-fQuartet[2])==4)) {
	    if(!fFoundQuartet) {
	      //	      fFirstCycle = fNCycle - 3;
	      cout << "Quartet potentially found, starting at cycle " << fFirstCycle
		   << " - event " << evdata.GetEvNum() << endl;
	      fFoundQuartet = kTRUE;
	    }
	  } else {
	    if(fNCycle - fFirstCycle > 4) { // Not at start of run.  Reset
	      cout << "Lost quartet sync at cycle " << fNCycle << " - event "
		   << evdata.GetEvNum() << endl;
	      cout << fQuartet[0] << " "  << fQuartet[1] << " "  << fQuartet[2] << " "
		   << fQuartet[3] << endl;
	      
	      fFirstCycle += 4*((fNCycle-fFirstCycle)/4); // Update, but don't change phase
	    }
	    fFoundQuartet = kFALSE;
	    fNBits = 0;
	    cout << "Searching for first of a quartet at cycle " << " " << fFirstCycle
		 << " - event " << evdata.GetEvNum() << endl;
	    cout << fQuartet[0] << " "  << fQuartet[1] << " "  << fQuartet[2] << " "
		 << fQuartet[3] << endl;
	    fFirstCycle++;
	  }
	}
	// Load the actual helicity.  Calibrate if not calibrated.
	fActualHelicity = kUnknown;
	LoadHelicity(fReportedHelicity, fNCycle, missed);
	fLastReportedHelicity = fReportedHelicity;
	fIsNewCycle = kFALSE;
	//	cout << fTITime/250000000.0 << " " << fNCycle << " " << fReportedHelicity << endl;
	//	cout << fNCycle << ": " << fReportedHelicity << " "
	//	     << fPredictedHelicity << " " << fActualHelicity << endl;
      }
      // Ignore until a MPS Is found
    }
  } else {
    cout << "Initializing" << endl;
    fLastReportedHelicity = fReportedHelicity;
    fActualHelicity = kUnknown;
    fPredictedHelicity = kUnknown;
    fFirstEvTime = fTITime;
    fLastEvTime = fTITime;
    fLastMPSTime = fTITime;  // Not necessarily during the MPS
    fNCycle = 0;
    fFirstEvProcessed = kTRUE;
    fFoundMPS = kFALSE;
    fFoundQuartet = kFALSE;
    fIsNewCycle = kFALSE;
    fNBits = 0;
  }
  //  cout << setprecision(9) << "HEL " << fTITime/250000000.0 << " " << fNCycle << "(" << (fNCycle-fFirstCycle)%4 << "): "
  //       << fMPS << " " << fReportedHelicity << " "
  //       << fPredictedHelicity << " " << fActualHelicity << " " << lastlastmpstime/250000000.0 << endl;
  // bitset<32>(v)
  return 0;
}

//_____________________________________________________________________________
Int_t THcHelicity::End( THaRunBase* )
{
  // End of run processing. Write histograms.
  THcHelicityReader::End();

  //  for( Int_t i = 0; i < NHIST; ++i )
  //    fHisto[i]->Write();

  return 0;
}

//_____________________________________________________________________________
void THcHelicity::SetDebug( Int_t level )
{
  // Set debug level of this detector as well as the THcHelicityReader 
  // helper class.

  THaHelicityDet::SetDebug( level );
  fQWEAKDebug = level;
}

//_____________________________________________________________________________
void THcHelicity::LoadHelicity(Int_t reportedhelicity, Int_t cyclecount, Int_t missedcycles)
{
  //  static const char* const here = "THcHelicity::LoadHelicity";
  int quartetphase = (cyclecount-fFirstCycle)%4;
  fnQrt = quartetphase;

  if(missedcycles > 1) {	// If we missed windows
    if(fNBits< fMAXBIT) {	// and we haven't gotten the seed, start over
      fNBits = 0;
      return;
    }
  }
  if(!fFoundQuartet) {		// Wait until we have found quad phase before starting
    return;			// to calibrate
  }
  if(quartetphase == 0) { // Start of a quad
    if(fNBits < fMAXBIT) {
      if(fNBits == 0) {
	cout << "Start calibrating at cycle " << cyclecount << endl;
	fRingSeed_reported = 0;
      }
      if(fReportedHelicity == kPlus) {
	fRingSeed_reported = ((fRingSeed_reported<<1) | 1) & 0x3FFFFFFF;
      } else {
	fRingSeed_reported = (fRingSeed_reported<<1) & 0x3FFFFFFF;
      }
      fNBits++;
      if(fReportedHelicity == kUnknown) {
	fNBits = 0;
	fRingSeed_reported = 0;
      } else if (fNBits==fMAXBIT) {
	cout << "Seed Found " << hex << fRingSeed_reported << dec << " at cycle " << cyclecount << " with first cycle " << fFirstCycle << endl;
	Int_t backseed = GetSeed30(fRingSeed_reported);
	cout << "Seed at cycle " << fFirstCycle << " should be " << hex << backseed << dec << endl;
      }
      fActualHelicity = kUnknown;
    } else if (fNBits >= fMAXBIT) {
      fRingSeed_reported = RanBit30(fRingSeed_reported);
      if(fNBits==fMAXBIT) {
	fRingSeed_actual = fRingSeed_reported;
	for(Int_t i=0;i<fHelDelay/4; i++) {
	  fRingSeed_actual = RanBit30(fRingSeed_actual);
	}
	fNBits++;
      } else {
	fRingSeed_actual = RanBit30(fRingSeed_actual);
      }
      fActualHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
      fPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
      //      if(fTITime/250000000.0 > 380.0) cout << fTITime/250000000.0 << " " << fNCycle << " " << hex <<
      //					fRingSeed_reported << " " << fRingSeed_actual << dec << endl;
      if(fReportedHelicity != fPredictedHelicity) {
	cout << "Helicity prediction failed " << fReportedHelicity << " "
	     << fPredictedHelicity << " " << fActualHelicity << endl;
	cout << hex << fRingSeed_reported << " " << fRingSeed_actual << dec << endl;
	fNBits = 0;		// Need to reaquire seed
	fActualHelicity = kUnknown;
	fPredictedHelicity = kUnknown;
      }
    }
    fQuartetStartHelicity = fActualHelicity;
    fQuartetStartPredictedHelicity = fPredictedHelicity;
  } else { 			// Not the beginning of a quad
    if(fNBits>=fMAXBIT) {
      fActualHelicity = (quartetphase==0||quartetphase==3)?
	fQuartetStartHelicity:-fQuartetStartHelicity;
      fPredictedHelicity = (quartetphase==0||quartetphase==3)?
	fQuartetStartPredictedHelicity:-fQuartetStartPredictedHelicity;
    }
  }
  return;
}
//_____________________________________________________________________________
Int_t  THcHelicity::RanBit30(Int_t ranseed)
{
  
  UInt_t bit7    = (ranseed & 0x00000040) != 0;
  UInt_t bit28   = (ranseed & 0x08000000) != 0;
  UInt_t bit29   = (ranseed & 0x10000000) != 0;
  UInt_t bit30   = (ranseed & 0x20000000) != 0;

  UInt_t newbit = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;

  
  if(ranseed<=0) {
    if(fQWEAKDebug>1)
      std::cerr<<"ranseed must be greater than zero!"<<"\n";

    newbit = 0;
  }
  ranseed =  ( (ranseed<<1) | newbit ) & 0x3FFFFFFF;
  //here ranseed is changed
  if(fQWEAKDebug>1)
    {
      cout<< "THcHelicity::RanBit30, newbit="<<newbit<<"\n";
    }

  return ranseed;

}
//_____________________________________________________________________________
Int_t  THcHelicity::GetSeed30(Int_t currentseed)
/* Back track the seed by 30 samples */
{
#if 1
  Int_t seed = currentseed;
  for(Int_t i=0;i<30;i++) {
    UInt_t bit1    = (seed & 0x00000001) != 0;
    UInt_t bit8    = (seed & 0x00000080) != 0;
    UInt_t bit29   = (seed & 0x10000000) != 0;
    UInt_t bit30   = (seed & 0x20000000) != 0;
    
    UInt_t newbit30 = (bit30 ^ bit29 ^ bit8 ^ bit1) & 0x1;
    seed = (seed >> 1) | (newbit30<<29);
  }
#else
  Int_t bits = currentseed;
  Int_t seed=0;
  for(Int_t i=0;i<30;i++) {
    Int_t val;
    // XOR at virtual position 0 and 29
    if(i==0) {
      val = ((bits & (1<<(i)))!=0) ^ ((bits & (1<<(i+29)))!=0);
    } else {
      val = ((bits & (1<<(i)))!=0) ^ ((seed & (1<<(i-1)))!=0);
    }
    if(i<=1) {
      val = ((bits & (1<<(1-i)))!=0) ^ val;
    } else {
      val = ((seed & (1<<(i-2)))!=0) ^ val;
    }
    if(i<=22) {
      val = ((bits & (1<<(i-22)))!=0) ^ val;
    } else {
      val = ((seed & (1<<(i-23)))!=0) ^ val;
    }
    seed |= (val<<i);
  }
#endif
  return seed;
}

ClassImp(THcHelicity)

