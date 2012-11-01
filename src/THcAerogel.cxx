///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcAerogel                                                                //
//                                                                           //
// Class for an Aerogel detector consisting of pairs of PMT's                //
// attached to a diffuser box                                                //
// Will have a fixed number of pairs, but need to later make this            //
// configurable.                                                             //T
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcAerogel.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcAerogel::THcAerogel( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);

//  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
}

//_____________________________________________________________________________
THcAerogel::THcAerogel( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcAerogel::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  cout << "THcAerogel::Init " << GetName() << endl;

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  THcHitList::InitHitList(fDetMap, "THcAerogelHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HAERO") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HAERO");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  char prefix[2];
  // Pull values from THcParmList instead

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  fNelem = 8;// Number of tube pairs  // The ENGINE style parameter files don't define
                                // this.  Probably need an additional parameter file
                                // that contains these fixed parameters.

  fPosGain = new Double_t[fNelem];
  fNegGain = new Double_t[fNelem];
  fPosPedLimit = new Int_t[fNelem];
  fNegPedLimit = new Int_t[fNelem];

  DBRequest list[]={
    {"aero_pos_gain", fPosGain, kDouble},
    {"aero_neg_gain", fPosGain, kDouble},
    {"aero_pos_ped_limit", fPosPedLimit, kInt},
    {"aero_neg_ped_limit", fNegPedLimit, kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  fIsInit = true;

  // Create arrays to hold pedestal results
  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcAerogel::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );
  
  // Register variables in global list
  RVarDef vars[] = {
    {"postdchits", "List of Positive TDC hits", 
     "fPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdchits", "List of Negative TDC hits", 
     "fNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}
//_____________________________________________________________________________
inline 
void THcAerogel::Clear(Option_t* opt)
{
  // Clears the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcAerogel::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

  if(gHaCuts->Result("Pedestal_event")) {

    AccumulatePedestals(fRawHitList);

    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
     
    CalculatePedestals();
   
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();


  Int_t ihit = 0;
  while(ihit < nhits) {
    THcAerogelHit* hit = (THcAerogelHit *) fRawHitList->At(ihit);
    
   // TDC positive hit
    if(hit->fTDC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_pos);
    }

    // TDC negative hit
    if(hit->fTDC_neg >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_neg);
    }

    // ADC positive hit
    if(hit->fADC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_pos);
    }

    // ADC negative hit
    if(hit->fADC_neg >  0) {   
      THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_neg);
    }

    ihit++;
  }
  return ihit;
}

//_____________________________________________________________________________
Int_t THcAerogel::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcAerogel::CoarseProcess( TClonesArray&  ) //tracks
{
  
  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcAerogel::FineProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
void THcAerogel::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedLimit = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum = new Int_t [fNelem];
  fNegPedSum2 = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];
  fNegPedCount = new Int_t [fNelem];

  fPosPed = new Double_t [fNelem];
  fNegPed = new Double_t [fNelem];
  fPosThresh = new Double_t [fNelem];
  fNegThresh = new Double_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fNegPedCount[i] = 0;
  }
}

//_____________________________________________________________________________
void THcAerogel::AccumulatePedestals(TClonesArray* rawhits)
{
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = 0;
  while(ihit < nrawhits) {
    THcAerogelHit* hit = (THcAerogelHit *) rawhits->At(ihit);

    Int_t element = hit->fCounter - 1;
    Int_t adcpos = hit->fADC_pos;
    Int_t adcneg = hit->fADC_pos;
    if(adcpos <= fPosPedLimit[element]) {
      fPosPedSum[element] += adcpos;
      fPosPedSum2[element] += adcpos*adcpos;
      fPosPedCount[element]++;
      if(fPosPedCount[element] == fMinPeds/5) {
	fPosPedLimit[element] = 100 + fPosPedSum[element]/fPosPedCount[element];
      }
    }
    if(adcneg <= fNegPedLimit[element]) {
      fNegPedSum[element] += adcneg;
      fNegPedSum2[element] += adcneg*adcneg;
      fNegPedCount[element]++;
      if(fNegPedCount[element] == fMinPeds/5) {
	fNegPedLimit[element] = 100 + fNegPedSum[element]/fNegPedCount[element];
      }
    }
    ihit++;
  }

  fNPedestalEvents++;

  return;
}

//_____________________________________________________________________________
void THcAerogel::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")
  //  cout << "Plane: " << fPlaneNum << endl;
  for(Int_t i=0; i<fNelem;i++) {
    
    // Positive tubes
    fPosPed[i] = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosThresh[i] = fPosPed[i] + 15;

    // Negative tubes
    fNegPed[i] = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegThresh[i] = fNegPed[i] + 15;

    //    cout << i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;
  }
  //  cout << " " << endl;
  
}


ClassImp(THcAerogel)
////////////////////////////////////////////////////////////////////////////////
 
