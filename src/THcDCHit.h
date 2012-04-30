#ifndef ROOT_THcDCHit
#define ROOT_THcDCHit

#include "THcRawHit.h"

#define MAXHITS 16

class THcDCHit : public THcRawHit {

 public:

 THcDCHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter), 
    fNHits(0) {
  }
  THcDCHit& operator=( const THcDCHit& );
  virtual ~THcDCHit() {}

  virtual void Clear( Option_t* opt="" ) { fNHits=0; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, Int_t ihit);

  virtual Bool_t  IsSortable () const {return kTRUE; }
  virtual Int_t   Compare(const TObject* obj) const;

  Int_t fNHits;
  Int_t fTDC[MAXHITS];

 protected:

 private:

  ClassDef(THcDCHit, 0);	// DC hit class
};  

#endif
