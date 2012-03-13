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

ClassImp(ThcHodoscopeHit)

void SetData(Int_t signal, Int_t data) {
  if(signal==1) {
    fADC_pos = data;
  } else if (signal==2) {
    fADC_net = data;
  } else if(signal==3) {
    fADC_pos = data;
  } else if (signal==4) {
    fADC_net = data;
  }
}

Int_t GetData(Int_t signal) {
  if(signal==1) {
    return(fADC_pos);
  } else if (signal==2) {
    return(fADC_neg);
  } else if(signal==3) {
    return(fADC_pos);
  } else if (signal==4) {
    return(fADC_neg);
  }
}

//////////////////////////////////////////////////////////////////////////
