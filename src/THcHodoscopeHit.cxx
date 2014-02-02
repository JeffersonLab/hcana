///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscopeHit                                                           //
//                                                                           //
// Class representing a single raw hit for a hodoscope paddle                //
//                                                                           //
// Contains plane, counter and pos/neg adc and tdc values                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcHodoscopeHit.h"

using namespace std;


void THcHodoscopeHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos = data;
  } else if (signal==1) {
    fADC_neg = data;
  } else if(signal==2) {
    fTDC_pos = data;
  } else if (signal==3) {
    fTDC_neg = data;
  }
}

Int_t THcHodoscopeHit::GetData(Int_t signal) {
  if(signal==0) {
    return(fADC_pos);
  } else if (signal==1) {
    return(fADC_neg);
  } else if(signal==2) {
    return(fTDC_pos);
  } else if (signal==3) {
    return(fTDC_neg);
  }
  return(-1); // Actually should throw exception
}

//_____________________________________________________________________________
THcHodoscopeHit& THcHodoscopeHit::operator=( const THcHodoscopeHit& rhs )
{
  // Assignment operator.

  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane = rhs.fPlane;
    fCounter = rhs.fCounter;
    fADC_pos = rhs.fADC_pos;
    fADC_neg = rhs.fADC_neg;
    fTDC_pos = rhs.fTDC_pos;
    fTDC_neg = rhs.fTDC_neg;
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcHodoscopeHit)

