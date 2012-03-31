//*-- Author :    Stephen Wood 30-March-2012

//////////////////////////////////////////////////////////////////////////
//
// THcDetectorBase
//
// Add hitlist to the Hall A detector base
//
//////////////////////////////////////////////////////////////////////////

#include "ThcDetectorBase.h"

using namespace std;

THcDetectorBase::THcDetectorBase( const char* name,
				  const char* description ) :
  THaDetectorBase(name, description)
{
}

THcDetectorBase::THcDetectorBase() : THaDetectorBase() {
}

THcDetectorBase::~THcDetectorBase() : ~THaDetectorBase() {
}

ClassImp(THcDetectorBase)
