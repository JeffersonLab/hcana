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
  fNHits(nhits), fNCombos(ncombos) {
    fHits.clear();
  }
  virtual ~THcSpacePoint() {}

  void SetXY(Double_t x, Double_t y) {fX = x; fY = y;};
  void Clear(Option_t* opt="") {fNHits=0; fNCombos=0; fHits.clear();};
  void AddHit(THcDCHit* hit) {
    fHits.push_back(hit);
    fNHits++;
  }
  Int_t GetNHits() {return fNHits;};
  void SetNHits(Int_t nhits) {fNHits = nhits;};
  Double_t GetX() {return fX;};
  Double_t GetY() {return fY;};
  THcDCHit* GetHit(Int_t ihit) {return fHits[ihit];};
  std::vector<THcDCHit*>* GetHitVectorP() {return &fHits;};
  void ReplaceHit(Int_t ihit, THcDCHit *hit) {fHits[ihit] = hit;};
  void SetStub(Double_t stub[4]) {
    for(Int_t i=0;i<4;i++) {
      fStub[i] = stub[i];
    }
  };
  Double_t* GetStubP() { return fStub; };
  void IncCombos() { fNCombos++; };
  void SetCombos(Int_t ncombos) { fNCombos=ncombos; };
  Int_t GetCombos() { return fNCombos; };

  // This is the chamber number (1,2), not index (0,1).  Sometime
  // we need figure out how to avoid confusion between number and index.
  Int_t fNChamber;

 protected:

  Double_t fX;
  Double_t fY;
  Int_t fNHits;
  Int_t fNCombos;
  std::vector<THcDCHit*> fHits;
  Double_t fStub[4];
  // Should we also have a pointer back to the chamber object

  ClassDef(THcSpacePoint,0)   // Drift Chamber class
};

////////////////////////////////////////////////////////////////////////////////

#endif
