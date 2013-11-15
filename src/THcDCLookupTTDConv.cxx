///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCLookupTTDConv                                                        //
//                                                                           //
// Upon initialization needs to be given the lookup table for time           //
// to distance conversion.                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcDCLookupTTDConv.h"

ClassImp(THcDCLookupTTDConv)


//______________________________________________________________________________
THcDCLookupTTDConv::THcDCLookupTTDConv(Double_t T0, Double_t MaxDriftDistance,
				       Double_t BinSize, Int_t NumBins,
				       Double_t* Table) :
fT0(T0), fMaxDriftDistance(MaxDriftDistance), fBinSize(BinSize),
  fNumBins(NumBins)
{
  //Normal constructor

  fTable = new Double_t[fNumBins];
  for(Int_t i=0;i<fNumBins;i++) {
    fTable[i] = Table[i];
  }

}

//______________________________________________________________________________
THcDCLookupTTDConv::~THcDCLookupTTDConv()
{
  // Destructor. Remove variables from global list.

}

//______________________________________________________________________________
Double_t THcDCLookupTTDConv::ConvertTimeToDist(Double_t time)
{
  // Lookup in table
  Int_t ib = (time-fT0)/fBinSize;
  Double_t frac = 0;
  if(ib >= 0 && ib+1 < fNumBins) {
    Double_t tfrac = (time - (ib*fBinSize + fT0)) / fBinSize;
    frac = fTable[ib]*(1-tfrac) + fTable[ib+1]*tfrac;
  } else if (ib+1 >= fNumBins) {
    frac = 1.0;
  }
  
  Double_t drift_distance = fMaxDriftDistance * frac;

  // Engine subtracts a hdc_card_delay from this.  Seems
  // to be zero in the PARAM files, bit is it always?  Delay implies
  // time?  Whis is a time subtracted from a distance?

  return(drift_distance);
}  

////////////////////////////////////////////////////////////////////////////////
