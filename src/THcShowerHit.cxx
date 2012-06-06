///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcShowerHit                                                           //
//                                                                           //
// Class representing a single raw hit for a hodoscope paddle                //
//                                                                           //
// Contains plane, counter and pos/neg adc and tdc values                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcShowerHit.h"

using namespace std;


void THcShowerHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_pos = data;
  } else if (signal==1) {
    fADC_neg = data;
  } 
}

Int_t THcShowerHit::GetData(Int_t signal) {
  if(signal==0) {
    return(fADC_pos);
  } else if (signal==1) {
    return(fADC_neg);
  } 

  return(-1); // Actually should throw exception
}

#if 0
Int_t THcShowerHit::Compare(const TObject* obj) const
{
  // Compare to sort by plane and counter

  const THcShowerHit* hit = dynamic_cast<const THcShowerHit*>(obj);

  if(!hit) return -1;
  Int_t p1 = fPlane;
  Int_t p2 = hit->fPlane;
  if(p1 < p2) return -1;
  else if(p1 > p2) return 1;
  else {
    Int_t c1 = fCounter;
    Int_t c2 = hit->fCounter;
    if(c1 < c2) return -1;
    else if (c1 == c2) return 0;
    else return 1;
  }
}
#endif
//_____________________________________________________________________________
THcShowerHit& THcShowerHit::operator=( const THcShowerHit& rhs )
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
ClassImp(THcShowerHit)

 
