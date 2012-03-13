#ifndef ROOT_THcHodoscopeHit
#define ROOT_THcHodoscopeHit

#include "TObject.h"

class THcHodoscopeHit : public TObject {

 public:
 THcHodoscopeHit(Int_t plane, Int_t counter) :
  fPlane(plane), fCounter(counter), fADC_pos(-1), fADC_net(-1),
    fTDC_pos(-1), fTDC_net(-1) {}

  virtual ~THcHodosscopeHit() {}

  Int_t fPlane;
  Int_t fCounter;

  Int_t fADC_pos;
  Int_t fADC_neg;
  Int_t fTDC_pos;
  Int_t fTDC_neg;

 protected:

 private:

  ClassDef(THaHodoscopeHit, 0);
};  

#endif
