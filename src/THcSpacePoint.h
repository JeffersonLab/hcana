#ifndef ROOT_THcSpacePoint
#define ROOT_THcSpacePoint

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcSpacePoint                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "THcDCHit.h"

class THcSpacePoint : public TObject {

public:

  THcSpacePoint(Int_t nhits=0, Int_t ncombos=0) :
  fNHits(nhits), fNCombos(ncombos),fSetStubFlag(kFALSE) {
    fHits.clear();
  }
  virtual ~THcSpacePoint() {}

  struct Hit {
    THcDCHit* dchit;
    Double_t distCorr; 		// Drift distance corrected by propagation along wire
    Int_t lr;			// Left right flag (+/- 1)
    // Should we copy into here the information from hit
    // this is likely to be often used?
  };

  void SetXY(Double_t x, Double_t y) {fX = x; fY = y;};
  void Clear(Option_t* opt="") {fNHits=0; fNCombos=0; fHits.clear();};
  void AddHit(THcDCHit* hit) {
    Hit newhit;
    newhit.dchit = hit;
    newhit.distCorr = 0.0;
    newhit.lr = 0;
    fHits.push_back(newhit);
    fNHits++;
  }
  Int_t GetNHits() {return fNHits;};
  void SetNHits(Int_t nhits) {fNHits = nhits;};
  Double_t GetX() {return fX;};
  Double_t GetY() {return fY;};
  Bool_t GetSetStubFlag() {return fSetStubFlag;};
  THcDCHit* GetHit(Int_t ihit) {return fHits[ihit].dchit;};
  //  std::vector<THcDCHit*>* GetHitVectorP() {return &fHits;};
  //std::vector<Hit>* GetHitStuffVectorP() {return &fHits;};
  void ReplaceHit(Int_t ihit, THcDCHit *hit) {
    fHits[ihit].dchit = hit;
    fHits[ihit].distCorr = 0.0;
    fHits[ihit].lr = 0;
  };
  void SetHitDist(Int_t ihit, Double_t dist) {
    fHits[ihit].distCorr = dist;
  };
  void SetHitLR(Int_t ihit, Int_t lr) {
    fHits[ihit].lr = lr;
  };
  void SetStub(Double_t stub[4]) {
    for(Int_t i=0;i<4;i++) {
      fStub[i] = stub[i];
    }
    fSetStubFlag=kTRUE;
  };
  Double_t GetHitDist(Int_t ihit) { return fHits[ihit].distCorr; };
  Int_t GetHitLR(Int_t ihit) { return fHits[ihit].lr; };
  Double_t* GetStubP() { return fStub; };
  void IncCombos() { fNCombos++; };
  void SetCombos(Int_t ncombos) { fNCombos=ncombos; };
  Int_t GetCombos() { return fNCombos; };
  Double_t GetStubX() {return fStub[0];};
  Double_t GetStubXP() {return fStub[2];};
  Double_t GetStubY() {return fStub[1];};
  Double_t GetStubYP() {return fStub[3];};

  // This is the chamber number (1,2), not index (0,1).  Sometime
  // we need figure out how to avoid confusion between number and index.
  Int_t fNChamber;
  Int_t fNChamber_spnum;

protected:

  Double_t fX;
  Double_t fY;
  Int_t fNHits;
  Int_t fNCombos;
  // This adds more indirection to getting hit information.
  std::vector<Hit> fHits;
  //std::vector<THcDCHit*> fHits;
  Double_t fStub[4];
  Bool_t fSetStubFlag;
  // Should we also have a pointer back to the chamber object

  ClassDef(THcSpacePoint,0);   // Space Point/stub track in a single drift chamber
};

////////////////////////////////////////////////////////////////////////////////

#endif
