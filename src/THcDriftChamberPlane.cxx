//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcDriftChamberPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcDriftChamberPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcDriftChamber.h"
#include "TClass.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcDriftChamberPlane)

//______________________________________________________________________________
THcDriftChamberPlane::THcDriftChamberPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fTDCHits = new TClonesArray("THcSignalHit",100);
#if ROOT_VERSION_CODE < ROOT_VERSION(5,32,0)
  fTDCHitsClass = fTDCHits->GetClass();
#endif
  fPlaneNum = planenum;
}

//______________________________________________________________________________
THcDriftChamberPlane::~THcDriftChamberPlane()
{
  // Destructor
  delete fTDCHits;

}
THaAnalysisObject::EStatus THcDriftChamberPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  cout << "THcDriftChamberPlane::Init called " << GetName() << endl;

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
Int_t THcDriftChamberPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  // Retrieve parameters we need

  return kOK;
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  //  cout << "THcDriftChamberPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"tdchits", "List of TDC hits", 
     "fTDCHits.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcDriftChamberPlane::Clear( Option_t* )
{
  //cout << " Calling THcDriftChamberPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fTDCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcDriftChamberPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcDriftChamberPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcDriftChamberPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
Int_t THcDriftChamberPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Int_t nTDCHits=0;
  fTDCHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcDriftChamberPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcDCHit* hit = (THcDCHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    // Just put in the first hit for now
    if(hit->fNHits > 0) {	// Should always be the case
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)      
      THcSignalHit *sighit = (THcSignalHit*) fTDCHits->ConstructedAt(nTDCHits++);
#else
      TObject* obj = (*fTDCHits)[nTDCHits++];
      R__ASSERT( obj );
      if(!obj->TestBit (TObject::kNotDeleted))
	fTDCHitsClass->New(obj);
      THcSignalHit *sighit = (THcSignalHit*)obj;
#endif  
      sighit->Set(hit->fCounter, hit->fTDC[0]);
    }
    ihit++;
  }
  return(ihit);
}

    
  
  
