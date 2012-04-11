//*-- Author :    Ole Hansen   7-Sep-00

//////////////////////////////////////////////////////////////////////////
//
// THcNonTrackingDetector
//
//////////////////////////////////////////////////////////////////////////

#include "THcNonTrackingDetector.h"

ClassImp(THcNonTrackingDetector)

//______________________________________________________________________________
THcNonTrackingDetector::THcNonTrackingDetector( const char* name, 
						const char* description,
						THaApparatus* apparatus )
  : THcSpectrometerDetector(name,description,apparatus)
{
  // Normal constructor with name and description

}

//______________________________________________________________________________
THcNonTrackingDetector::THcNonTrackingDetector( )
  : THcSpectrometerDetector( )
{
  // for ROOT I/O only

}

//______________________________________________________________________________
THcNonTrackingDetector::~THcNonTrackingDetector()
{
  // Destructor

}

