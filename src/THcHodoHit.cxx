/** \class THcHodoHit
    \ingroup DetSupport

 Class representing a single hit for the Hodoscopes                        

*/
#include "THcHodoHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(THcHodoHit)

THcHodoHit::THcHodoHit( THcRawHodoHit *hit, Double_t posPed, 
			     Double_t negPed, THcScintillatorPlane* sp)
{
  if(hit) {
    fPosTDC = hit->GetTDCPos();
    fNegTDC = hit->GetTDCNeg();
    fPosADC_Ped = hit->GetADCPos() - posPed;
    fNegADC_Ped = hit->GetADCNeg() - negPed;
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
