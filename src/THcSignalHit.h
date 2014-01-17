#ifndef ROOT_THcSignalHit
#define ROOT_THcSignalHit

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// THcSignalHit                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include <cstdio>

class THcSignalHit : public TObject {

 public:
 THcSignalHit(Int_t paddle=0, Double_t data=0.0) :
  fPaddleNumber(paddle), fData(data) {}
  virtual ~THcSignalHit() {}

  Int_t GetPaddleNumber() {return fPaddleNumber;}
  Double_t GetData() {return fData;}

  virtual void Set(Int_t paddle, Int_t data)
  { fPaddleNumber=paddle; fData=data; }
  virtual void Set(Int_t paddle, Double_t data)
  { fPaddleNumber=paddle; fData=data; }

 private:
  Int_t fPaddleNumber;
  Double_t fData;

  ClassDef(THcSignalHit,0); // Single signal value and wire/counter number
};
/////////////////////////////////////////////////////////////////
#endif
