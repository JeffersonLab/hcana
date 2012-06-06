#ifndef ROOT_THcShowerHit
#define ROOT_THcShowerHit

#include "THcRawHit.h"

class THcShowerHit : public THcRawHit {

 public:

 THcShowerHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter), 
    fADC_pos(-1), fADC_neg(-1){
  }
  THcShowerHit& operator=( const THcShowerHit& );
  virtual ~THcShowerHit() {}

  virtual void Clear( Option_t* opt="" )
    { fADC_pos = -1; fADC_neg = -1; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);

  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;

  Int_t fADC_pos;
  Int_t fADC_neg;

 protected:

 private:

  ClassDef(THcShowerHit, 0);	// Shower hit class
};  

#endif
 
