#ifndef ROOT_THcHodoscopeHit
#define ROOT_THcHodoscopeHit

// Should we have a generic hit object that we inherit from?  (A template,
// whatever that is?)  

#include "THcRawHit.h"

class THcHodoscopeHit : public THcRawHit {

 public:

  THcHodoscopeHit(Int_t plane, Int_t counter) :
  fPlane(plane), fCounter(counter), fADC_pos(-1), fADC_neg(-1),
    fTDC_pos(-1), fTDC_neg(-1) {}
  virtual ~THcHodoscopeHit() {}

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);

  Int_t fADC_pos;
  Int_t fADC_neg;
  Int_t fTDC_pos;
  Int_t fTDC_neg;

 protected:

 private:

  ClassDef(THcHodoscopeHit, 0);	// Hodoscope hit class
};  

#endif
