///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoHit                                                                //
//                                                                           //
// Class representing a single hit for the Hodoscopes                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(THcHodoHit)

THcHodoHit::THcHodoHit( THcRawHodoHit *hit, Double_t posPed=0.0, 
			     Double_t negPed=0.0, THcScintillatorPlane* sp=NULL)
{
  if(hit) {
    fPosTDC = hit->fTDC_pos;
    fNegTDC = hit->fTDC_neg;
    fPosADC_Ped = hit->fADC_pos - posPed;
    fNegADC_Ped = hit->fADC_neg - negPed;
    fPaddleNumber = hit->fCounter;
  } else {
    fPosTDC = -1;
    fNegTDC = -1;
    fPosADC_Ped = -1000.0;
    fNegADC_Ped = -1000.0;
    fPaddleNumber = -1;
  }
  fPlane = sp;
}

////////////////////////////////////////////////////////////////////////////////
