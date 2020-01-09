////////////////////////////////////////////////////////////////////////
//
// THcHelicity
//
// Helicity of the beam in delayed mode using quartets
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
#include "THcHelicityScaler.h"
#include "TH1F.h"
#include "TMath.h"
#include <iostream>
#include <bitset>

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
  fglHelicityScaler = 0;
  fHelicityHistory = 0;
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

  fEvNumCheck = 0;
  fDisabled = kFALSE;

  fLastHelpCycle=-1;
  fQuadPattern[0] = fQuadPattern[1] = fQuadPattern[2] = fQuadPattern[3] = 0;
  fQuadPattern[4] = fQuadPattern[5] = fQuadPattern[6] = fQuadPattern[7] = 0;
  fHelperHistory=0;
  fHelperQuartetHistory=0;
  fScalerSeed=0;
  fNBits = 0;
  lastispos = 0;
  fLastReportedHelicity = kUnknown;
  fFixFirstCycle = kFALSE;
  fHaveQRT = kFALSE;
  fNQRTProblems = 0;
  fPeriodCheck = 0.0;
  fPeriodCheckOffset = 0.0;
  fCycle = 0.0;
  fRecommendedFreq = -1.0;

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
  fNLastQuartet = -1;
  fNQuartet = 0;
  //  fFreq = 29.5596;
  fFreq = 120.0007547169;
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

  cout << "Helicity decoder initialized with frequency of " << fFreq
       << " Hz and reporting delay of " << fHelDelay << " cycles." << endl;

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
    { "pcheck", "Period check", "fPeriodCheck"},
    { "cycle", "Helicity Cycle", "fCycle"},
    { "qrt", "Last cycle of quartet", "fQrt"},
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
void THcHelicity::SetHelicityScaler( THcHelicityScaler *f )
{
    fglHelicityScaler = f;
    fHelicityHistory = fglHelicityScaler->GetHelicityHistoryP();
}

//_____________________________________________________________________________
Int_t THcHelicity::Decode( const THaEvData& evdata )
{

  // Decode Helicity data.
  // Return 1 if helicity was assigned, 0 if not, <0 if error.
  evnum = evdata.GetEvNum();

  Int_t err = ReadData( evdata ); // from THcHelicityReader class
  if( err ) {
    Error( Here("THcHelicity::Decode"), "Error decoding helicity data." );
    return err;
  }

  fReportedHelicity = (fIsHelp?(fIsHelm?kUnknown:kPlus):(fIsHelm?kMinus:kUnknown));
  fMPS = fIsMPS?1:0;
  fQrt = fIsQrt?1:0;		// Last of quartet

#if 0
  if(fglHelicityScaler) {
    Int_t nhelev = fglHelicityScaler->GetNevents();
    Int_t ncycles = fglHelicityScaler->GetNcycles();
    if(nhelev >0) {
      for(Int_t i=0;i<nhelev;i++) {
	fScaleQuartet = (fHelicityHistory[i] & 2)!=0;
	Int_t ispos = fHelicityHistory[i]&1;
	if(fScaleQuartet) {
	  fScalerSeed = ((fScalerSeed<<1) | ispos) & 0x3FFFFFFF;
	  if(fNBits >= fMAXBIT+0) {
	    Int_t seedscan = fScalerSeed;
	    Int_t nbehind;
	    for(nbehind=0;nbehind<4;nbehind++) {
	      if(seedscan == fRingSeed_reported) {
		if(nbehind>1) {
		  cout << "Scaler seed behind " << nbehind
		       << " quartets" << endl;
		  cout << "Ev seed     " << bitset<32>(fRingSeed_reported) <<endl;
		  cout << "Scaler Seed " << bitset<32>(fScalerSeed) << endl;
		}
		break;
	      }
	      seedscan = RanBit30(seedscan);
	    }
	    if(nbehind>4) {
	      cout << "Scaler seed does not match" << endl;
	      cout << "Ev seed     " << bitset<32>(fRingSeed_reported) <<endl;
	      cout << "Scaler Seed " << bitset<32>(fScalerSeed) << endl;
	    }
	  }
	}
      }
    }
  }
#endif
  
  if(fHelDelay == 0) {		// If no delay actual=reported (but zero if in MPS)
    fActualHelicity = fIsMPS?kUnknown:fReportedHelicity;
    return 0;
  }

  if(fDisabled) {
    fActualHelicity = kUnknown;
    return 0;
  }

  fEvNumCheck++;
  Int_t evnum = evdata.GetEvNum();
  if(fEvNumCheck!=evnum) {
    cout << "THcHelicity: Missed " << evnum-fEvNumCheck << " events at event " << evnum << endl;
    cout << "             Disabling helicity decoding for rest of run." << endl;
    cout << "             Make sure \"RawDecode_master in cuts file accepts all physics events." <<endl;
    fDisabled = kTRUE;
    fActualHelicity = kUnknown;
    return 0;
  }

  fActualHelicity = -10.0;
  if(fFirstEvProcessed) {	// Normal processing
    //    cout << evnum << " " << fNCycle << " " << fIsMPS << " " << fFoundMPS << " " << fTITime << " "
    //	 << fLastMPSTime << " " << fNBits << endl;
    Int_t missed = 0;
    //    Double_t elapsed_time = (fTITime - fFirstEvTime)/250000000.0;
    if(fIsMPS) {
      fPeriodCheck = fmod(fTITime/(250000000.0/fFreq)-fPeriodCheckOffset,1.0);

      fCycle = (fTITime/fTIPeriod);
      fActualHelicity = kUnknown;
      fPredictedHelicity = kUnknown;
      if(fFoundMPS) {
	if(fRecommendedFreq < 0.0) {
	  if(TMath::Abs(fPeriodCheck-0.5) > 0.25) {
	    fRecommendedFreq = fFreq*(1-(fPeriodCheck-0.5)/fCycle);
	  }
	}
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
	fPeriodCheckOffset = (fPeriodCheck-.5);
      }
    } else if (fFoundMPS) {	//
      if(fTITime - fLastMPSTime > fTIPeriod) { // We missed MPS periods
	missed = TMath::Nint(floor((fTITime-fLastMPSTime)/fTIPeriod));
	if(missed > 1) {
	  //	  cout << "Missed " << missed << " MPSes" << endl;
	  Int_t newNCycle = fNCycle + missed -1; // How many cycles really missed
	  Int_t quartets_missed = (newNCycle-fFirstCycle)/4 - (fNCycle-fFirstCycle)/4;
	  int quartetphase = (newNCycle-fFirstCycle)%4;
	  //	  cout << "  " << fNCycle << " " << newNCycle << " " << fFirstCycle << " " << quartets_missed << " " << quartetphase << endl;
	  //	  cout << "Cycles " << fNCycle << " " << newNCycle << " " << fFirstCycle 
	  //	       << " skipped " << quartets_missed << " quartets" << endl;
	  fNCycle = newNCycle;
	  // Need to reset fQuartet to reflect where we are based on the current
	  // reported helicity.  So we don't fail quartet testing.
	  // But only do this if we are calibrated.
	  if(fNBits >= fMAXBIT+0) {

	    for(Int_t i=0;i<quartets_missed;i++) { // Advance the seeds.
	      //	      cout << "Advancing seed A " << fNBits << endl;
	      fRingSeed_reported = RanBit30(fRingSeed_reported);
	      fRingSeed_actual = RanBit30(fRingSeed_actual);
	    }

	    fQuartetStartHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
	    fQuartetStartPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
	    fActualHelicity = (quartetphase==0||quartetphase==3)?
	      fQuartetStartHelicity:-fQuartetStartHelicity;
	    fPredictedHelicity = (quartetphase==0||quartetphase==3)?
	      fQuartetStartPredictedHelicity:-fQuartetStartPredictedHelicity;

	    if (((fNCycle - fFirstCycle)%2)==1) {
	      fQuartet[0] = fReportedHelicity;
	      fQuartet[1] = fQuartet[2] = -fQuartet[0];
	    } else {
	      fQuartet[0] = fQuartet[1] = -fReportedHelicity;
	      fQuartet[2] = -fQuartet[1];
	    }
	  } else {
	    fActualHelicity = kUnknown;
	    fQuartet[0] = fReportedHelicity;
	    fQuartet[1] = 0;
	  }
	}
	fLastMPSTime += missed*fTIPeriod;
	fIsNewCycle = kTRUE;
	fLastReportedHelicity = fReportedHelicity;
      } else {			// No missed periods.  Get helicities from rings
	if(fNBits>=fMAXBIT+0) {
	  int quartetphase = (fNCycle-fFirstCycle)%4;
	  fQuartetStartHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
	  fQuartetStartPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
	  fActualHelicity = (quartetphase==0||quartetphase==3)?
	    fQuartetStartHelicity:-fQuartetStartHelicity;
	  fPredictedHelicity = (quartetphase==0||quartetphase==3)?
	  fQuartetStartPredictedHelicity:-fQuartetStartPredictedHelicity;
	} else {
	  fActualHelicity = 0;
	}
      }
      if(fIsNewCycle) {
	//	cout << "Scaler Seed " << bitset<32>(fScalerSeed) << endl;
	//Int_t predictedScalerSeed = RanBit30(fScalerSeed);
	//	cout << "Predct Seed " << bitset<32>(predictedScalerSeed) << endl;
	//cout << "Event Seed  " << bitset<32>(fRingSeed_reported) << endl;
	  
	fQuartet[3]=fQuartet[2]; fQuartet[2]=fQuartet[1]; fQuartet[1]=fQuartet[0];
	fQuartet[0]=fReportedHelicity;
	fNCycle++;
	//	cout << "XX: " << fNCycle << " " <<  fReportedHelicity << " " << fIsQrt <<endl;

	if(fIsQrt) {		// 
	  fNLastQuartet = fNQuartet;
	  fNQuartet = (fNCycle-fFirstCycle)/4;
	  if(fHaveQRT) {	// Should already have phase set
	    if((fNCycle-fFirstCycle)%4 != 0) {// Test if first in a quartet
	      fNQRTProblems++;
	      if(fNQRTProblems > 10) {
		cout << "QRT Problem resetting" << endl;
		fHaveQRT = kFALSE;
		fFoundQuartet = kFALSE;
		fNQRTProblems = 0;
	      } else {
		cout << "Ignored " << fNQRTProblems << " problems" << endl;
	      }
	    } else {
	      fNQRTProblems = 0;
	    }
	  }
	  if(!fHaveQRT) {
	    fHaveQRT = kTRUE;
	    fFoundQuartet = kTRUE;
	    
	    fFirstCycle = fNCycle; //
	    fNQuartet = (fNCycle-fFirstCycle)/4;
	    fNLastQuartet = fNQuartet - 1; // Make sure LoadHelicity uses
	    fNBits = 0;
	    fNQRTProblems = 0;
	    cout << "Phase found from QRT signal" << endl;
	    cout << "fFirstcycle = " << fFirstCycle << endl;
	  }
	} else {
	  if(fHaveQRT) {	// Using qrt signal.
	    fNLastQuartet = fNQuartet;
	    fNQuartet = (fNCycle-fFirstCycle)/4;
	    if((fNCycle-fFirstCycle)%4 == 0) { // Shouldn't happen
	      fNQRTProblems++;
	      if(fNQRTProblems > 10) {
		cout << "Shouldn't happen, cycle=" << fNCycle << "/" << fFirstCycle << endl;
		fHaveQRT = kFALSE; // False until a new QRT seen
		fNBits = 0;		       // Reset
		fNLastQuartet = fNQuartet; // Make sure LoadHelicity does not use
	      } else {
		cout << "Ignored " << fNQRTProblems << " problems" << endl;
	      }
	    }
	  } else { 		// Presumable pre qrt signal data
	    if((fNCycle-fFirstCycle)%4 == 3) {// Test if last in a quartet
	      if((abs(fQuartet[0]+fQuartet[3]-fQuartet[1]-fQuartet[2])==4)) {
		if(!fFoundQuartet) {
		  //	      fFirstCycle = fNCycle - 3;
		  cout << "Quartet potentially found, starting at cycle " << fFirstCycle
		       << endl;
		  fNQuartet = (fNCycle-fFirstCycle)/4;
		  fNLastQuartet = fNQuartet - 1; // Make sure LoadHelicity uses
		  fFoundQuartet = kTRUE;
		}
	      } else {
		if(fNCycle - fFirstCycle > 4) { // Not at start of run.  Reset
		  cout << "Lost quartet sync at cycle " << fNCycle << endl;
		  cout << fQuartet[0] << " "  << fQuartet[1] << " "  << fQuartet[2] << " "
		       << fQuartet[3] << endl;
		  fFirstCycle += 4*((fNCycle-fFirstCycle)/4); // Update, but don't change phase
		}
		fFoundQuartet = kFALSE;
		fNBits = 0;
		cout << "Searching for first of a quartet at cycle " << " " << fFirstCycle
		     << endl;
		cout << fQuartet[0] << " "  << fQuartet[1] << " "  << fQuartet[2] << " "
		     << fQuartet[3] << endl;
		fFirstCycle++;
	      }
	    } else {
	      fNLastQuartet = fNQuartet;
	      fNQuartet = (fNCycle-fFirstCycle)/4;
	    }
	  }
	}
	// Load the actual helicity.  Calibrate if not calibrated.
	fActualHelicity = kUnknown;
	// Here if we know we missed some earlier in the quartet, we need
	// to make sure we get here and call LoadHelicity for the missing
	// Cycles, reducing the missed count for each extra loadhelicity
	// call that we make.
	LoadHelicity(fReportedHelicity, fNCycle, missed);
	fLastReportedHelicity = fReportedHelicity;
	fIsNewCycle = kFALSE;
	//	cout << fTITime/250000000.0 << " " << fNCycle << " " << fReportedHelicity << endl;
	//	cout << fNCycle << ": " << fReportedHelicity << " "
	//	     << fPredictedHelicity << " " << fActualHelicity << endl;
      }
      // Ignore until a MPS Is found

    } else {			// No MPS found yet
      fActualHelicity = kUnknown;
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

  // Some sanity checks
  if(fActualHelicity < -5) {
    cout << "Actual Helicity never got defined" << endl;
  }
  if(fNBits < fMAXBIT+0) {
    if(fActualHelicity == -1 || fActualHelicity == 1) {
      cout << "Helicity of " << fActualHelicity << " reported prematurely at cycle " << fNCycle << endl;
    }
  }
  fLastActualHelicity = fActualHelicity;
  return 0;
}
//_____________________________________________________________________________
Int_t THcHelicity::End( THaRunBase* )
{
  // End of run processing. Write histograms.
  THcHelicityReader::End();

  //  for( Int_t i = 0; i < NHIST; ++i )
  //    fHisto[i]->Write();

  if(fRecommendedFreq < 0.0) {
    fRecommendedFreq = fFreq*(1-(fPeriodCheck-0.5)/fCycle);
  }
  if(TMath::Abs(1-fRecommendedFreq/fFreq) >= 0.5e-6) {
    cout << "------------- HELICITY DECODING ----------------------" << endl;
    cout << "Actual helicity reversal frequency differs from \"helicity_freq\" value" << endl;
    cout << "If there are helicity decoding errors beyond the start of the run, " << endl;
    streamsize ss = cout.precision();
    cout.precision(10);
    cout << "try replacing helicity_freq value of " << fFreq << " with " << fRecommendedFreq << endl;
    cout << "If that still gives helicity errors, try " << 0.9999999*fRecommendedFreq << endl;
    cout.precision(ss);
    cout << "------------------------------------------------------" << endl;
  }

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

  //  if(!fFixFirstCycle) {
    if(fNQuartet - fNLastQuartet > 1) {	// If we missed a quartet
      if(fNBits< fMAXBIT+0) {	// and we haven't gotten the seed, start over
	cout << "fNBits = 0, missedcycles=" << missedcycles <<
	  "    " << fNLastQuartet << " " << fNQuartet << endl;
	fNBits = 0;
	return;
      }
    }
    //  }
  if(!fFoundQuartet) {		// Wait until we have found quad phase before starting
    return;			// to calibrate
  }
  // LoadHelicity is called first event of each cycle.
  // But only do it's thing if it is first cycle of quartet.
  // Need to instead have it do it's thing on the first event of a quartet.
  // But only for seed building.  Current logic is OK once seed is found.
  if(fNBits < fMAXBIT+0) {
    if(fNQuartet != fNLastQuartet) {
      // Sanity check fNQuartet == fNLastQuartet+1
      if(fNBits == 0) {
	cout << "Start calibrating at cycle " << cyclecount << endl;
	fRingSeed_reported = 0;
      }
      // Do phase stuff right here
      if((fReportedHelicity == kPlus && (quartetphase==0 || quartetphase == 3))
	 || (fReportedHelicity == kMinus && (quartetphase==1 || quartetphase == 2))) {
	fRingSeed_reported = ((fRingSeed_reported<<1) | 1) & 0x3FFFFFFF;
	//	cout << "                 " << fNQuartet << " 1" << endl;
      } else {
	fRingSeed_reported = (fRingSeed_reported<<1) & 0x3FFFFFFF;
	//	cout << "                 " << fNQuartet << " 0" << endl;
      }
      fNBits++;
      if(fReportedHelicity == kUnknown) {
	fNBits = 0;
	fRingSeed_reported = 0;
      } else if (fNBits==fMAXBIT+0) {
	cout <<   " Seed Found   " << bitset<32>(fRingSeed_reported) << " at cycle " << cyclecount << " with first cycle " << fFirstCycle << endl;
	if(fglHelicityScaler) {
	  cout << "Scaler Seed  " << bitset<32>(fglHelicityScaler->GetReportedSeed()) << endl;
	}
	Int_t backseed = GetSeed30(fRingSeed_reported);
	cout << "Seed at cycle " << fFirstCycle << " should be " << hex << backseed << dec << endl;
	// Create the "actual seed"
	fRingSeed_actual = fRingSeed_reported;
	for(Int_t i=0;i<fHelDelay/4; i++) {
	  fRingSeed_actual = RanBit30(fRingSeed_actual);
	}
	fQuartetStartHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
	fQuartetStartPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
      } else if (fglHelicityScaler && fNBits>2) {			// Try the scalers
	if(fglHelicityScaler->IsSeedGood()) {
	  Int_t scalerseed = fglHelicityScaler->GetReportedSeed();
	  fRingSeed_reported = RanBit30(scalerseed);
	  cout << " -- Getting seed from scalers -- " << endl;
	  cout <<   " Seed Found   " << bitset<32>(fRingSeed_reported) << " at cycle " << cyclecount << " with first cycle " << fFirstCycle << endl;
	  cout << "Scaler Seed  " << bitset<32>(scalerseed) << endl;
	  // Create the "actual seed"
	  fRingSeed_actual = fRingSeed_reported;
	  for(Int_t i=0;i<fHelDelay/4; i++) {
	    fRingSeed_actual = RanBit30(fRingSeed_actual);
	  }
	  fQuartetStartHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
	  fQuartetStartPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
	  fNBits = fMAXBIT+0;
	}
      } 
      fActualHelicity = kUnknown;
    } // Need to change this to build seed even when not at start of quartet
  } else {
    if(quartetphase == 0) {
      // If quartetphase !=, the seeds will alread have been advanced
      // except that we won't have made the initial 
      //      cout << "Advancing seed B " << fNBits << endl;
      fRingSeed_reported = RanBit30(fRingSeed_reported); 
      fRingSeed_actual = RanBit30(fRingSeed_actual);

      fActualHelicity = (fRingSeed_actual&1)?kPlus:kMinus;
      fPredictedHelicity = (fRingSeed_reported&1)?kPlus:kMinus;
      //      if(fTITime/250000000.0 > 380.0) cout << fTITime/250000000.0 << " " << fNCycle << " " << hex <<
      //					fRingSeed_reported << " " << fRingSeed_actual << dec << endl;
      if(fReportedHelicity != fPredictedHelicity) {
	cout << "Helicity prediction failed " << fReportedHelicity << " "
	     << fPredictedHelicity << " " << fActualHelicity << endl;
	cout << bitset<32>(fRingSeed_reported) << " " << bitset<32>(fRingSeed_actual) << endl;
	fNBits = 0;		// Need to reaquire seed
	fActualHelicity = kUnknown;
	fPredictedHelicity = kUnknown;
      }
      fQuartetStartHelicity = fActualHelicity;
      fQuartetStartPredictedHelicity = fPredictedHelicity;
    }
    fActualHelicity = (quartetphase==0||quartetphase==3)?
      fQuartetStartHelicity:-fQuartetStartHelicity;
    fPredictedHelicity = (quartetphase==0||quartetphase==3)?
      fQuartetStartPredictedHelicity:-fQuartetStartPredictedHelicity;
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

