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

  THcHodoHit(Int_t postdc, Int_t negtdc, Double_t posadc, Double_t negadc,
	     Int_t ipad, THcScintillatorPlane* sp) :
  fPosTDC(postdc), fNegTDC(negtdc), fPosADC_Ped(posadc), fNegADC_Ped(negadc),
    fPaddleNumber(ipad), fPlane(sp) {};

  virtual ~THcHodoHit() {}

  Bool_t IsSortable () const { return kFALSE; }
  
  // Get and Set Functions
  Double_t GetPosADC() const { return fPosADC_Ped; }
  Double_t GetNegADC() const { return fNegADC_Ped; }
  Int_t GetPosTDC() const { return fPosTDC; }
  Int_t GetNegTDC() const { return fNegTDC; }
  Double_t GetPosCorrectedTime() const { return fPosCorrectedTime;}
  Double_t GetNegCorrectedTime() const { return fNegCorrectedTime;}
  Double_t GetPosTOFCorrectedTime() const { return fPosTOFCorrectedTime;}
  Double_t GetNegTOFCorrectedTime() const { return fNegTOFCorrectedTime;}
  Double_t GetScinCorrectedTime() const { return fScinCorrectedTime;}
  Int_t GetPaddleNumber() const { return fPaddleNumber; }

  void SetCorrectedTimes(Double_t pos, Double_t neg, Double_t) {
    fPosCorrectedTime = pos; fNegCorrectedTime = neg;
  }
  void SetCorrectedTimes(Double_t pos, Double_t neg,
			 Double_t postof, Double_t negtof,
			 Double_t timeave) {
    fPosCorrectedTime = pos; fNegCorrectedTime = neg;
    fPosTOFCorrectedTime = postof; fNegTOFCorrectedTime = negtof;
    fScinCorrectedTime = timeave;
  }

protected:
  static const Double_t kBig;  //!

  Int_t fPosTDC;
  Int_t fNegTDC;
  Double_t fPosADC_Ped;		// Pedestal subtracted ADC
  Double_t fNegADC_Ped;		// Pedestal subtracted ADC
  Int_t fPaddleNumber;

  Double_t fPosCorrectedTime;	// Pulse height corrected time
  Double_t fNegCorrectedTime;	// Pulse height corrected time
  Double_t fScinCorrectedTime;  // Time average corrected for position
                                // based on ADCs.
  Double_t fPosTOFCorrectedTime; // Times corrected for z position
  Double_t fNegTOFCorrectedTime; // using nominal beta
  THcScintillatorPlane* fPlane;	// Pointer to parent scintillator plane
  
  
private:
  THcHodoHit( const THcHodoHit& );
  THcHodoHit& operator=( const THcHodoHit& );
  
  ClassDef(THcHodoHit,0)             // Drift Chamber Hit
};

////////////////////////////////////////////////////////////////////////////////

#endif
