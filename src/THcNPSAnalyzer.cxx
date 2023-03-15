/** \class THcAnalyzer
    \ingroup Base

\brief NPS Fake data analyzer class.

Use this class to control merging of events to simulate multiplicity in the NPS.

Do analyzer->SetCountMode(0) so that only physics events are counted in
determining when to merge.

*/
#include "THcNPSAnalyzer.h"
#include "THcAnalyzer.h"
#include "THaAnalyzer.h"
#include "THaRunBase.h"
#include "THaEvent.h"
#include "THaOutput.h"
#include "THaSpectrometer.h"
#include "THaPhysicsModule.h"
#include "InterStageModule.h"
#include "THaBenchmark.h"

#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <vector>

using namespace std;


// Pointer to single instance of this object
//THcNPSAnalyzer* THcNPSAnalyzer::fgAnalyzer = 0;

//FIXME:
// do we need to "close" scalers/EPICS analysis if we reach the event limit?

//_____________________________________________________________________________
THcNPSAnalyzer::THcNPSAnalyzer()
{
  fNevMerge = 1;
}

//_____________________________________________________________________________
THcNPSAnalyzer::~THcNPSAnalyzer()
{
  // Destructor.

}

//_____________________________________________________________________________
Int_t THcNPSAnalyzer::PhysicsAnalysis( Int_t code )
{
  // Analysis of physics events

  static const char* const here = "PhysicsAnalysis";

  // Don't analyze events that did not pass RawDecode cuts
  if( code != kOK )
    return code;

  //--- Skip physics events until we reach the first requested event
  if( fNev < fRun->GetFirstEvent() )
    return kSkip;

  if( fFirstPhysics ) {
    fFirstPhysics = false;
    if( fVerbose>2 )
      cout << "Starting physics analysis at event " << GetCount(kNevPhysics)
	   << endl;
  }
  // Update counters
  fRun->IncrNumAnalyzed();
  Incr(kNevAnalyzed);

  //--- Process all apparatuses that are defined in fApps
  //    First Decode(), then Reconstruct()

  const char* stage = "";
  THaAnalysisObject* obj = nullptr;  // current module, for exception error message

  try {
    stage = "Decode";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* mod : fAnalysisModules ) {
      obj = mod;
      if(GetClearThisEvent())	// clear only after merging fNevMerge events
	mod->Clear();
    }
    for( auto* app : fApps ) {
      obj = app;
      app->Decode(*fEvData);
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kDecode ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( !EvalStage(kDecode) ) return kSkip;

    //--- Main physics analysis. Calls the following for each defined apparatus
    //    THaSpectrometer::CoarseTrack  (only for spectrometers)
    //    THaApparatus::CoarseReconstruct
    //    THaSpectrometer::Track        (only for spectrometers)
    //    THaApparatus::Reconstruct
    //
    // Test blocks are evaluated after each of these stages


    if(! GetProcessThisEvent())	// Only process every fNevMerge events
      return kSkip;
    //-- Coarse processing

    stage = "CoarseTracking";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* spectro : fSpectrometers ) {
      obj = spectro;
      spectro->CoarseTrack();
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kCoarseTrack ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( !EvalStage(kCoarseTrack) )  return kSkip;


    stage = "CoarseReconstruct";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* app : fApps ) {
      obj = app;
      app->CoarseReconstruct();
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kCoarseRecon ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( !EvalStage(kCoarseRecon) )  return kSkip;

    //-- Fine (Full) Reconstruct().

    stage = "Tracking";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* spectro : fSpectrometers ) {
      obj = spectro;
      spectro->Track();
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kTracking ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( !EvalStage(kTracking) )  return kSkip;


    stage = "Reconstruct";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* app : fApps ) {
      obj = app;
      app->Reconstruct();
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kReconstruct ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( !EvalStage(kReconstruct) )  return kSkip;

    //--- Process the list of physics modules

    stage = "Physics";
    if( fDoBench ) fBench->Begin(stage);
    for( auto* physmod : fPhysics ) {
      obj = physmod;
      Int_t err = physmod->Process( *fEvData );
      if( err == THaPhysicsModule::kTerminate )
        code = kTerminate;
      else if( err == THaPhysicsModule::kFatal ) {
        code = kFatal;
        break;
      }
    }
    for( auto* mod : fInterStage ) {
      if( mod->GetStage() == kPhysics ) {
        obj = mod;
        mod->Process(*fEvData);
      }
    }
    if( fDoBench ) fBench->Stop(stage);
    if( code == kFatal ) return kFatal;

    //--- Evaluate "Physics" test block

    if( !EvalStage(kPhysics) )
      // any status code form the physics analysis overrides skip code
      return (code == kOK) ? kSkip : code;

  } // end try
  catch( exception& e ) {
    TString module_name = (obj != nullptr) ? obj->GetName() : "unknown";
    TString module_desc = (obj != nullptr) ? obj->GetTitle() : "unknown";
    Error( here, "Caught exception %s in module %s (%s) during %s analysis "
	   "stage. Terminating analysis.", e.what(), module_name.Data(),
	   module_desc.Data(), stage );
    if( fDoBench ) fBench->Stop(stage);
    code = kFatal;
    goto errexit;
  }

  //---  Process output
  if( fDoBench ) fBench->Begin("Output");
  try {
    //--- If Event defined, fill it.
    if( fEvent ) {
      fEvent->GetHeader()->Set( fEvData->GetEvNum(),
				fEvData->GetEvType(),
				fEvData->GetEvLength(),
				fEvData->GetEvTime(),
				fEvData->GetHelicity(),
				0,
				fRun->GetNumber()
				);
      fEvent->Fill();
    }
    // Write to output file
    if( fOutput ) fOutput->Process();
  }
  catch( exception& e ) {
    Error( here, "Caught exception %s during output of event %u. "
	   "Terminating analysis.", e.what(), fNev );
    code = kFatal;
  }
  if( fDoBench ) fBench->Stop("Output");

 errexit:
  return code;
}

ClassImp(THcNPSAnalyzer)

