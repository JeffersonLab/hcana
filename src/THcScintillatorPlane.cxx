//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcScintillatorPlane.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
		    const char* description,
		    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description

}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor

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
  if( status=THaSubDetector::Init( date ) )
    return fStatus = status;

  //  const DataDest tmp[NDEST] = {
  //    { &fLTNhit, &fLANhit, fLT, fLA },
  //    { &fRTNhit, &fRANhit, fRT, fRA }
  //  };
  //  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  FILE* file = OpenFile( date );
  
  static const char* const here = "ReadDatabase()";
  const int LEN = 200;
  char buf[LEN];
  Int_t nelem;

  cout << "THcScintillatorPlane::ReadDatabase called " << GetName() << endl;
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
  return kOK;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::Decode( const THaEvData& evdata )
{
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
