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
  THcDCLookupTTDConv(Double_t T0, Double_t MaxDriftDistance, Double_t BinSize,
		     Int_t NumBins, Double_t* Table);

  virtual ~THcDCLookupTTDConv();

  virtual Double_t ConvertTimeToDist(Double_t time);


protected:

  Double_t fT0;
  Double_t fMaxDriftDistance;
  Double_t fBinSize;
  Int_t fNumBins;
  Double_t* fTable;

  ClassDef(THcDCLookupTTDConv,0)             // Time to Distance conversion lookup
};


////////////////////////////////////////////////////////////////////////////////

#endif
