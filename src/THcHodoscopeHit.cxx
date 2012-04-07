///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscopeHit                                                           //
//                                                                           //
// Class representing a single raw hit for a hodoscope paddle                //
//                                                                           //
// Contains plane, counter and pos/neg adc and tdc values                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoscopeHit.h"

using namespace std;


void THcHodoscopeHit::SetData(Int_t signal, Int_t data) {
  if(signal==1) {
    fADC_pos = data;
  } else if (signal==2) {
    fADC_neg = data;
  } else if(signal==3) {
    fADC_pos = data;
  } else if (signal==4) {
    fADC_neg = data;
  }
}

Int_t THcHodoscopeHit::GetData(Int_t signal) {
  if(signal==1) {
    return(fADC_pos);
  } else if (signal==2) {
    return(fADC_neg);
  } else if(signal==3) {
    return(fADC_pos);
  } else if (signal==4) {
    return(fADC_neg);
  }
  return(-1); // Actually should throw exception
}

Int_t THcHodoscopeHit::Compare(const TObject* obj) const
{
  // Compare to sort by plane and counter

  const THcHodoscopeHit* hit = dynamic_cast<const THcHodoscopeHit*>(obj);

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

