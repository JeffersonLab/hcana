/** \class THcDCLookupTTDConv
    \ingroup DetSupport

    \brief Drift time to distance conversion via lookup table.

*/
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
  /**
     Convert drift time to a distance from the wire by looking up in a table.
  */
  Int_t ib = (time-fT0)/fBinSize;
  Double_t frac = 0;
  if(ib >= 0 && ib+1 < fNumBins) {
    Double_t tfrac = (time - (ib*fBinSize + fT0)) / fBinSize;
    frac = fTable[ib]*(1-tfrac) + fTable[ib+1]*tfrac;
  } else if (ib+1 >= fNumBins) {
    frac = 1.0;
  }

  Double_t drift_distance = fMaxDriftDistance * frac;


  return(drift_distance);
}

////////////////////////////////////////////////////////////////////////////////
