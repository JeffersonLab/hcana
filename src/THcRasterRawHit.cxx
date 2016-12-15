// Author : Buddhini Waidyawansa
// Date : 23-01-2014


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRasterRawHit                                                           //
//                                                                           //
// Class representing a single raw hit for the raster                        //
//                                                                           //
// Contains the X, Y raster voltage signals and sync signals                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcRasterRawHit.h"

using namespace std;


void THcRasterRawHit::SetData(Int_t signal, Int_t data) {
  if(signal==0) {
    fADC_xsync = data;
  } else if (signal==1) {
    fADC_xsig = data;
  } else if(signal==2) {
    fADC_ysync = data;
  } else if (signal==3) {
    fADC_ysig = data;
  }

  // std::cout<<" xsync = "<<fADC_xsync
  // 	   <<" xsig  = "<<fADC_xsig
  // 	   <<" ysync  = "<<fADC_ysync
  // 	   <<" ysig  = "<<fADC_ysig << std::endl;

}

Int_t THcRasterRawHit::GetData(Int_t signal) {

  if(signal==1) {
    return(fADC_xsync);
  } else if (signal==2) {
    return(fADC_xsig);
  } else if(signal==3) {
    return(fADC_ysync);
  } else if (signal==4) {
    return(fADC_ysig);
  }
  return(-1);
}

// Int_t THcRasterRawHit::Compare(const TObject* obj) const
// {
//   // Compare to sort by the plane
//   // There is only one raster so no need for an additional check on the counter

//   const THcRasterRawHit* hit = dynamic_cast<const THcRasterRawHit*>(obj);

//   if(!hit) return -1;
//   Int_t p1 = fPlane;
//   Int_t p2 = hit->fPlane;
//   if(p1 < p2) return -1;
//   else if(p1 > p2) return 1;

// }

//_____________________________________________________________________________
THcRasterRawHit& THcRasterRawHit::operator=( const THcRasterRawHit& rhs )
{
  // Assignment operator.

  THcRawHit::operator=(rhs);
  if ( this != &rhs ) {
    fPlane     = rhs.fPlane;
    fCounter   = rhs.fCounter;
    fADC_xsync = rhs.fADC_xsync;
    fADC_xsig  = rhs.fADC_xsig;
    fADC_ysync = rhs.fADC_ysync;
    fADC_ysig  = rhs.fADC_ysig;
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////////
ClassImp(THcRasterRawHit)

