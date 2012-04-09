//*-- Author :    Ole Hansen   7-Sep-00

//////////////////////////////////////////////////////////////////////////
//
// THaNonTrackingDetector
//
//////////////////////////////////////////////////////////////////////////

#include "THaNonTrackingDetector.h"

ClassImp(THaNonTrackingDetector)

//______________________________________________________________________________
THaNonTrackingDetector::THaNonTrackingDetector( const char* name, 
						const char* description,
						THaApparatus* apparatus )
  : THaSpectrometerDetector(name,description,apparatus)
{
  // Normal constructor with name and description

}

//______________________________________________________________________________
THaNonTrackingDetector::THaNonTrackingDetector( )
  : THaSpectrometerDetector( )
{
  // for ROOT I/O only

}

//______________________________________________________________________________
THaNonTrackingDetector::~THaNonTrackingDetector()
{
  // Destructor

}

