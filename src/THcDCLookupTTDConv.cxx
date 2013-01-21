///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCLookupTTDConv                                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcDCLookupTTDConv.h"

ClassImp(THcDCLookupTTDConv)


//______________________________________________________________________________
THcDCLookupTTDConv::THcDCLookupTTDConv()
{
  //Normal constructor
}

//______________________________________________________________________________
THcDCLookupTTDConv::THcDCLookupTTDConv( Double_t vel) 
{
  // Normal constructor 
  fDriftVel = vel;

  // TODO: This should be read from database!!
  fA1tdcCor[0] = 2.12e-3;
  fA1tdcCor[1] = 0.0;
  fA1tdcCor[2] = 0.0;
  fA1tdcCor[3] = 0.0;
  fA2tdcCor[0] = -4.20e-4;
  fA2tdcCor[1] =  1.3e-3;
  fA2tdcCor[2] = 1.06e-4;
  fA2tdcCor[3] = 0.0;
  
  fdtime    = 4.e-9; // 4ns -> 200 microns
}



//______________________________________________________________________________
THcDCLookupTTDConv::~THcDCLookupTTDConv()
{
  // Destructor. Remove variables from global list.

}

//______________________________________________________________________________
Double_t THcDCLookupTTDConv::ConvertTimeToDist(Double_t time,
						  Double_t tanTheta,
						  Double_t *ddist)
{
  // Drift Velocity in m/s
  // time in s
  // Return m 
  
//    printf("Converting Drift Time to Drift Distance!\n");

  Double_t a1 = 0.0, a2 = 0.0;
  // Find the values of a1 and a2 by evaluating the proper polynomials
  // a = A_3 * x^3 + A_2 * x^2 + A_1 * x + A_0

  tanTheta = 1.0 / tanTheta;  // I assume this has to do w/ making the
                              // polynomial have the proper variable...

  for (Int_t i = 3; i >= 1; i--) {
    a1 = tanTheta * (a1 + fA1tdcCor[i]);
    a2 = tanTheta * (a2 + fA2tdcCor[i]);
  }
  a1 += fA1tdcCor[0];
  a2 += fA2tdcCor[0];

//    printf("a1(%e) = %e\n", tanTheta, a1);
//    printf("a2(%e) = %e\n", tanTheta, a2);

  // ESPACE software includes corrections to the time for
  // 1. Cluster t0 (offset applied to entire cluster)
  // 2. Time of flight to scintillators
  Double_t dist = fDriftVel * time;
  Double_t unc  = fDriftVel * fdtime;  // watch uncertainty in the timing
  if (dist < 0) {
    // something screwy is going on
  } else if (dist < a1 ) { 
    //    dist = fDriftVel * time * (1 + 1 / (a1/a2 + 1));
    dist *= ( 1 + a2 / a1);
    unc *=  ( 1 + a2 / a1);
  }  else {
    dist +=  a2;
  }

  if (ddist) *ddist = unc;
//    printf("D(%e) = %e\nUncorrected D = %e\n", time, dist,  fDriftVel * time);

  return dist;
  
}


////////////////////////////////////////////////////////////////////////////////
