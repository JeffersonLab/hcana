#ifndef ROOT_THcDCLookupTTDConv
#define ROOT_THcDCLookupTTDConv

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCLookupTTDConv                                                     //
//                                                                           //
// Uses a drift velocity (um/ns) to convert time (ns) into distance (cm)     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include "THcDCTimeToDistConv.h"

class THcDCLookupTTDConv : public THcDCTimeToDistConv{

public:
  THcDCLookupTTDConv( );
  THcDCLookupTTDConv(Double_t vel);

  virtual ~THcDCLookupTTDConv();

  virtual Double_t ConvertTimeToDist(Double_t time, Double_t tanTheta,
				     Double_t *ddist=0);


  // Get and Set Functions 
  Double_t GetDriftVel() { return fDriftVel; }

  void SetDriftVel(Double_t v) {fDriftVel = v; }

protected:

  Double_t fDriftVel;   // Drift velocity (m / s)

  // Coefficients for a polynomial yielding correction parameters
  // For now, hard code these values from db_eh845
  // Eventually, this need to be read directly from the database
  Double_t fA1tdcCor[4];
  Double_t fA2tdcCor[4];

  Double_t fdtime;      // uncertainty in the measured time

  ClassDef(THcDCLookupTTDConv,0)             // VDC Analytic TTD Conv class
};


////////////////////////////////////////////////////////////////////////////////

#endif
