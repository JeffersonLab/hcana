//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcScintillatorPlane.h"

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
		    const char* description,
		    THaApparatus* apparatus )
  : THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description

}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor

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
