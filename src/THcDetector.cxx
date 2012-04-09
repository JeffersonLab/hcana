//*-- Author :    Ole Hansen   15-May-00

//////////////////////////////////////////////////////////////////////////
//
// THcDetector
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetector.h"
#include "THaApparatus.h"

ClassImp(THcDetector)

//_____________________________________________________________________________
THcDetector::THcDetector( const char* name, const char* description,
			  THaApparatus* apparatus )
  : THcDetectorBase(name,description), fApparatus(apparatus)
{
  // Constructor

  if( !name || !*name ) {
    Error( "THcDetector()", "Must construct detector with valid name! "
	   "Object construction failed." );
    MakeZombie();
    return;
  }
}

//_____________________________________________________________________________
THcDetector::THcDetector( ) : fApparatus(0) {
  // for ROOT I/O only
}

//_____________________________________________________________________________
THcDetector::~THcDetector()
{
  // Destructor
}

//_____________________________________________________________________________
void THcDetector::SetApparatus( THaApparatus* apparatus )
{
  // Associate this detector with the given apparatus.
  // Only possible before initialization.

  if( IsInit() ) {
    Warning( Here("SetApparatus()"), "Cannot set apparatus. "
	     "Object already initialized.");
    return;
  }
  fApparatus = apparatus;
}

//_____________________________________________________________________________
void THcDetector::MakePrefix()
{
  // Set up name prefix for global variables. Internal function called 
  // during initialization.

  const char* basename = NULL;
  THaApparatus *app = GetApparatus();
  if( app )
    basename = app->GetName();
  THcDetectorBase::MakePrefix( basename );

}
  
