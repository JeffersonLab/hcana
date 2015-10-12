//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcShowerArray
//
//////////////////////////////////////////////////////////////////////////

#include "THcShowerArray.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
#include "math.h"
#include "THaTrack.h"
#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <fstream>
using namespace std;

ClassImp(THcShowerArray)

//______________________________________________________________________________
THcShowerArray::THcShowerArray( const char* name, 
				const char* description,
				const Int_t layernum,
				THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  fADCHits = new TClonesArray("THcSignalHit",100);
  fLayerNum = layernum;
}

THcShowerArray::~THcShowerArray()
{
  // Destructor
  delete fADCHits;

  delete [] fA;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShowerArray::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcShowerArray::ReadDatabase( const TDatime& date )
{

  char prefix[2];
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  cout << "Parent name: " << GetParent()->GetPrefix() << endl;
  DBRequest list[]={
    {"cal_nrows", &fNRows, kInt},
    {"cal_ncolumns", &fNColumns, kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  fNelem = fNRows*fNColumns;

  // Here read the 2-D arras of pedestals, gains, etc.


  fA = new Double_t[fNelem];

  return kOK;
}

//_____________________________________________________________________________
Int_t THcShowerArray::DefineVariables( EMode mode )
{
  // Initialize global variables

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"adchits", "List of ADC hits", "fADCHits.THcSignalHit.GetPaddleNumber()"},
    {"a", "Raw ADC Amplitude", "fA"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerArray::Clear( Option_t* )
{
  // Clears the hit lists
  fADCHits->Clear();

}

//_____________________________________________________________________________
Int_t THcShowerArray::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead

  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::CoarseProcess( TClonesArray& tracks )
{

  // Nothing is done here. See ProcessHits method instead.
  //  

 return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list
  // Assumes that the hit list is sorted by layer, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  THcShower* fParent;
  fParent = (THcShower*) GetParent();

  // Initialize variables.

  fADCHits->Clear();

  for(Int_t i=0;i<fNelem;i++) {
    fA[i] = 0;
  }

  // Process raw hits. Get ADC hits for the plane, assign variables for each
  // channel.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // Should probably check that counter # is in range
    fA[hit->fCounter-1] = hit->fADC_pos;

    // Do other stuff like comparison to thresholds, signal hits, energy sums

    ihit++;
  }

  return(ihit);
}
//_____________________________________________________________________________
Int_t THcShowerArray::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Doesn't do anything yet except skip over hits

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // OK for hit list sorted by layer.
    if(hit->fPlane > fLayerNum) {
      break;
    }
    ihit++;
  }
  fNPedestalEvents++;

  return(ihit);
}
//_____________________________________________________________________________
void THcShowerArray::CalculatePedestals( )
{
  // Doesn't do anything yet

}
//_____________________________________________________________________________
void THcShowerArray::InitializePedestals( )
{
  // Doesn't do anything yet
  fNPedestalEvents = 0;
} 
