#ifndef ROOT_THcRawDCHit
#define ROOT_THcRawDCHit

#include "THcRawHit.h"

#define MAXHITS 16

class THcRawDCHit : public THcRawHit {

public:
  friend class THcDriftChamberPlane;
  friend class THcDC;

  THcRawDCHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter), 
    fNHits(0) {
  }
  THcRawDCHit& operator=( const THcRawDCHit& );
  virtual ~THcRawDCHit() {}

  virtual void Clear( Option_t* opt="" ) { fNHits=0; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, UInt_t ihit);

  virtual Bool_t  IsSortable () const {return kTRUE; }
  virtual Int_t   Compare(const TObject* obj) const;


protected:
  UInt_t fNHits;
  Int_t fTDC[MAXHITS];

private:

  ClassDef(THcRawDCHit, 0);	// Raw Drift Chamber hit
};  

#endif
