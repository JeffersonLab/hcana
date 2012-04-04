//*-- Author :    Ole Hansen   16/05/00

//////////////////////////////////////////////////////////////////////////
//
// THcDetMap
//
// The standard detector map for a Hall A detector.
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetMap.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

using namespace std;

//_____________________________________________________________________________
THcDetMap::THcDetMap() : THaDetMap()
{
  // Default constructor. Creates an empty detector map.
}

//_____________________________________________________________________________
THcDetMap::THcDetMap( const THcDetMap& rhs ) : THaDetMap( rhs )
{
  // Copy constructor

}

//_____________________________________________________________________________
THcDetMap::~THcDetMap()
{
  // Destructor

  delete [] fMap;
}

//_____________________________________________________________________________
ClassImp(THcDetMap)
