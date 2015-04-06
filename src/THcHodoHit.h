#ifndef ROOT_THcHodoHit
#define ROOT_THcHodoHit

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoHit                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "THcScintillatorPlane.h"
//#include "THcDriftChamber.h"
#include "THcRawHodoHit.h"
#include <cstdio>

class THcHodoHit : public TObject {

public:
  THcHodoHit( THcRawHodoHit* hit, Double_t posPed, 
	      Double_t negPed, THcScintillatorPlane* sp);
      
  virtual ~THcHodoHit() {}

  Bool_t IsSortable () const { return kFALSE; }
  
  // Get and Set Functions
  Double_t GetPosADC() const { return fPosADC_Ped; }
  Double_t GetNegADC() const { return fNegADC_Ped; }
  Int_t GetPosTDC() const { return fPosTDC; }
  Int_t GetNegTDC() const { return fNegTDC; }
  Int_t GetPaddleNumber() const { return fPaddleNumber; }

protected:
  static const Double_t kBig;  //!

  Int_t fPaddleNumber;
  Int_t fPosTDC;
  Int_t fNegTDC;
  Double_t fPosADC_Ped;		// Pedestal subtracted ADC
  Double_t fNegADC_Ped;		// Pedestal subtracted ADC

  THcScintillatorPlane* fPlane;	// Pointer to parent scintillator plane
  
  
private:
  THcHodoHit( const THcHodoHit& );
  THcHodoHit& operator=( const THcHodoHit& );
  
  ClassDef(THcHodoHit,0)             // Drift Chamber Hit
};

////////////////////////////////////////////////////////////////////////////////

#endif
