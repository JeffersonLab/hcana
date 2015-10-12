#ifndef ROOT_THcRawShowerHit
#define ROOT_THcRawShowerHit

#include "THcRawHit.h"

class THcRawShowerHit : public THcRawHit {

 public:
  friend class THcShowerPlane;
  friend class THcShowerArray;

  THcRawShowerHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter), 
    fADC_pos(-1), fADC_neg(-1){
  }
  THcRawShowerHit& operator=( const THcRawShowerHit& );
  virtual ~THcRawShowerHit() {}

  virtual void Clear( Option_t* opt="" )
    { fADC_pos = -1; fADC_neg = -1; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);

  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;

 protected:
  Int_t fADC_pos;
  Int_t fADC_neg;

 private:

  ClassDef(THcRawShowerHit, 0);	// Raw Shower counter hit
};  

#endif
 
