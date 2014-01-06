// Author : Buddhini Waidyawansa
// Date : 12-09-2013

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRaster                                                                 //
//                                                                           //
//  A class to decode the fast raster signals.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcRaster.h"

using namespace std;

//_____________________________________________________________________________
THcRaster::THcRaster( const char* name, const char* description,
		      THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Initializing channels
  fRasterXRaw = new TClonesArray("THcSignalHit",16);
  fRasterYRaw = new TClonesArray("THcSignalHit",16);
}

//_____________________________________________________________________________
THcRaster::THcRaster( ) :
  THaNonTrackingDetector()
{
  // Default constructor
}

//_____________________________________________________________________________
THcRaster::~THRaster()
{
  // Distructor
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
