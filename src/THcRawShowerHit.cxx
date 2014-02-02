///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawShowerHit                                                           //
//                                                                           //
// Class representing a single raw hit for a hodoscope paddle                //
//                                                                           //
// Contains plane, counter and pos/neg adc and tdc values                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawShowerHit.h"

using namespace std;


void THcRawShowerHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos = data;
  } else if (signal==1) {
    fADC_neg = data;
  } 
}

Int_t THcRawShowerHit::GetData(Int_t signal) {
  if(signal==0) {
    return(fADC_pos);
  } else if (signal==1) {
    return(fADC_neg);
  } 

  return(-1); // Actually should throw exception
}

//_____________________________________________________________________________
THcRawShowerHit& THcRawShowerHit::operator=( const THcRawShowerHit& rhs )
{
  // Assignment operator.

  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane = rhs.fPlane;
    fCounter = rhs.fCounter;
    fADC_pos = rhs.fADC_pos;
    fADC_neg = rhs.fADC_neg;

  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRawShowerHit)

 
