#ifndef ROOT_THcHodoscopeHit
#define ROOT_THcHodoscopeHit

#include "THcRawHit.h"

class THcHodoscopeHit : public THcRawHit {

 public:

 THcHodoscopeHit(Int_t plane, Int_t counter) : THcRawHit(plane, counter), 
    fADC_pos(-1), fADC_neg(-1),
    fTDC_pos(-1), fTDC_neg(-1) {}
  THcHodoscopeHit& operator=( const THcHodoscopeHit& );

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
