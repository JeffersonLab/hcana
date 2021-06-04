#ifndef ROOT_THcDCPlaneCluster
#define ROOT_THcDCPlaneCluster

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCPlaneCluster                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "THcDCHit.h"

class THcDCPlaneCluster : public TObject {

public:

  THcDCPlaneCluster(Int_t nhits=0) :
  fNHits(nhits) {
    fHits.clear();
  }
  virtual ~THcDCPlaneCluster() {}
  void SetXY(Double_t x, Double_t y) {fX = x; fY = y;};
  void Clear(Option_t* opt="") {fNHits=0; fHits.clear();};
  void AddHit(THcDCHit* hit) {
    fHits.push_back(hit);
    fNHits++;
  }
  THcDCHit* GetHit(Int_t ihit) {return fHits[ihit];};
  Int_t GetNHits() {return fNHits;};
  Double_t GetX() {return fX;};
  Double_t GetY() {return fY;};

protected:

  Double_t fX;
  Double_t fY;
  Int_t fNHits;
  std::vector<THcDCHit*> fHits;


  ClassDef(THcDCPlaneCluster,0);   
};

////////////////////////////////////////////////////////////////////////////////

#endif
