//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcScintillatorPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcHodoscope.h"
#include "TClass.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  fPosTDCHitsClass = fPosTDCHits->GetClass();
  fNegTDCHitsClass = fNegTDCHits->GetClass();
  fPosADCHitsClass = fPosADCHits->GetClass();
  fNegADCHitsClass = fNegADCHits->GetClass();
  fPlaneNum = planenum;
}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor
  delete fPosTDCHits;
  delete fNegTDCHits;
  delete fPosADCHits;
  delete fNegADCHits;

}
THaAnalysisObject::EStatus THcScintillatorPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  cout << "THcScintillatorPlane::Init called " << GetName() << endl;

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  // Get the Hodoscope hitlist
  // Can't seem to cast to THcHitList.  What to do if we want to use
  // THcScintillatorPlane as a subdetector to other than THcHodoscope?
  //  fParentHitList = static_cast<THcHodoscope*>(GetParent())->GetHitList();

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,"scin_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);

  strcat(parname,"_nr");
  cout << " Getting value of " << parname << endl;
  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  parname[plen]='\0';
  strcat(parname,"_spacing");

  fSpacing =  gHcParms->Find(parname)->GetValue(0);
  
  // First letter of GetParent()->GetPrefix() tells us what prefix to
  // use on parameter names.  


  //  Find the number of elements
  
  // Think we will make special methods to pass most
  // How generic do we want to make this class?  
  // The way we get parameter data is going to be pretty specific to
  // our parameter file naming conventions.  But on the other hand,
  // the Hall A analyzer read database is pretty specific.
  // Is there any way for this class to know which spectrometer he
  // belongs too?


  // Create arrays to hold results here


  return kOK;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  cout << "THcScintillatorPlane::DefineVariables called " << GetName() << endl;

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
void THcScintillatorPlane::Clear( Option_t* )
{
  //cout << " Calling THcScintillatorPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcScintillatorPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
Int_t THcScintillatorPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }


if(hit->fTDC_pos >  0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
	sighit->Set(hit->fCounter, hit->fTDC_pos);
#else
TObject* obj = (*fPosTDCHits)[nPosTDCHits++];
R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
fPosTDCHitsClass->New(obj);
THcSignalHit *sighit = (THcSignalHit*)obj;
#endif
   sighit->Set(hit->fCounter, hit->fTDC_pos);
}


if(hit->fTDC_neg >  0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
	sighit->Set(hit->fCounter, hit->fTDC_neg);
#else
	TObject* obj = (*fNegTDCHits)[nNegTDCHits++];
	R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
	fNegTDCHitsClass->New(obj);
	THcSignalHit *sighit = (THcSignalHit*)obj;
#endif
   sighit->Set(hit->fCounter, hit->fTDC_neg);
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
	TObject* obj = (*fNegADCHits)[nNegADCHits++];
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

    
  
  
