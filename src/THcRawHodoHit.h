#ifndef ROOT_THcRawHodoHit
#define ROOT_THcRawHodoHit

#include "THcRawHit.h"

class THcRawHodoHit : public THcRawHit {

 public:
  friend class THcScintillatorPlane;
  friend class THcHodoscope;
  friend class THcHodoHit;

  THcRawHodoHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter), 
    fADC_pos(-1), fADC_neg(-1),
    fTDC_pos(-1), fTDC_neg(-1) {
  }
  THcRawHodoHit& operator=( const THcRawHodoHit& );
  virtual ~THcRawHodoHit() {}

  virtual void Clear( Option_t* opt="" )
    { fADC_pos = -1; fADC_neg = -1; fTDC_pos = -1; fTDC_neg = -1; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);

  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;

 protected:
  Int_t fADC_pos;
  Int_t fADC_neg;
  Int_t fTDC_pos;
  Int_t fTDC_neg;

 private:

  ClassDef(THcRawHodoHit, 0);	// Raw Hodoscope hit
};  

#endif
