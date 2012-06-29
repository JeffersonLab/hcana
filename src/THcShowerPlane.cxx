//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcShowerPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcShowerPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "TClass.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcShowerPlane)

//______________________________________________________________________________
THcShowerPlane::THcShowerPlane( const char* name, 
					    const char* description,
					    const Int_t layernum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHitsClass = fPosADCHits->GetClass();
  fNegADCHitsClass = fNegADCHits->GetClass();
  fLayerNum = layernum;
}

//______________________________________________________________________________
THcShowerPlane::~THcShowerPlane()
{
  // Destructor
  delete fPosADCHits;
  delete fNegADCHits;

}
THaAnalysisObject::EStatus THcShowerPlane::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

  cout << "THcShowerPlane::Init called " << GetName() << endl;

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
Int_t THcShowerPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,"cal_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);

  strcat(parname,"_nr");
  cout << " Getting value of SHOWER!!!" << parname << endl;
//   fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();
// 
//   parname[plen]='\0';
//   strcat(parname,"_spacing");
// 
//   fSpacing =  gHcParms->Find(parname)->GetValue(0);
  
  // First letter of GetParent()->GetPrefix() tells us what prefix to
  // use on parameter names.  


  //  Find the number of elements
  
  // Create arrays to hold results here


  return kOK;
}
//_____________________________________________________________________________
Int_t THcShowerPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  cout << "THcShowerPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  //cout << " Calling THcShowerPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosADCHits->Clear();
  fNegADCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcShowerPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcShowerPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
Int_t THcShowerPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list
  // Assumes that the hit list is sorted by layer, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcShowerHit* hit = (THcShowerHit *) rawhits->At(ihit);
    if(hit->fPlane > fLayerNum) {
      break;
    }


if(hit->fADC_pos >  0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
	sighit->Set(hit->fCounter, hit->fADC_pos);
#else
	TObject* obj = (*fPosADCHits)[nPosADCHits++];
	R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
	fPosADCHitsClass->New(obj);
	THcSignalHit *sighit = (THcSignalHit*)obj;
#endif
   sighit->Set(hit->fCounter, hit->fADC_pos);
}

if(hit->fADC_neg >  0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
	sighit->Set(hit->fCounter, hit->fADC_neg);
#else
	TObject* obj = (*fPosADCHits)[nNegADCHits++];
	R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
	fNegADCHitsClass->New(obj);
	THcSignalHit *sighit = (THcSignalHit*)obj;
#endif
   sighit->Set(hit->fCounter, hit->fADC_neg);
}

    ihit++;
  }
  return(ihit);
}

    
  
  

