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
