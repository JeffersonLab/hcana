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
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
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
  // Constructor
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
  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  THcHitList::InitHitList(fDetMap, "THcAerogelHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  for(Int_t ip=0;ip<fNLayers;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }
  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HCAL") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HCAL");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
inline 
void THcAerogel::Clear(Option_t* opt)
{
//   Reset per-event data.
  for(Int_t ip=0;ip<fNLayers;ip++) {
    fPlanes[ip]->Clear(opt);
  }
 // fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcAerogel::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNLayers;ip++) {
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
//cout << "nexthit = " << nexthit << endl;
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

   if(fAnalyzePedestals) {
     for(Int_t ip=0;ip<fNLayers;ip++) {
       fPlanes[ip]->CalculatePedestals();
     }
     fAnalyzePedestals = 0;	// Don't analyze pedestals next event
   }



  Int_t nexthit = 0;
  for(Int_t ip=0;ip<fNLayers;ip++) {
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
  }
/*
//   fRawHitList is TClones array of THcAerogelHit objects
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcAerogelHit* hit = (THcAerogelHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_pos << " " << hit->fADC_neg << " "  << endl;
  }
  cout << endl;
*/
  return nhits;
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



ClassImp(THcAerogel)
////////////////////////////////////////////////////////////////////////////////
 
