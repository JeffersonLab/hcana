#ifndef ROOT_THcRawHodoHit
#define ROOT_THcRawHodoHit

#include "THcRawHit.h"

#define MAXHITS 16

class THcRawHodoHit : public THcRawHit {

 public:
  friend class THcScintillatorPlane;
  friend class THcHodoscope;
  friend class THcHodoHit;

  THcRawHodoHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter) {
    for(Int_t i=0;i<4;i++) {
      fHasRef[i] = kFALSE;
      fNRawHits[i] = 0;
    }
  }
  THcRawHodoHit& operator=( const THcRawHodoHit& );
  virtual ~THcRawHodoHit() {}

  virtual void Clear( Option_t* opt="" ) {
    for(Int_t i=0;i<4;i++) {
      fHasRef[i] = kFALSE;
      fNRawHits[i] = 0;
    }
  }
  void SetData(Int_t signal, Int_t data);
  void SetReference(Int_t signal, Int_t reference);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, UInt_t ihit);
  Int_t GetRawData(Int_t signal);
  Int_t GetRawData(Int_t signal, UInt_t ihit);
  Int_t GetReference(Int_t signal);
  Bool_t HasReference(Int_t signal);
  
  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;
  Int_t GetADCPos() {return GetData(0, 0);}
  Int_t GetADCNeg() {return GetData(1, 0);}
  Int_t GetTDCPos() {return GetData(2, 0);}
  Int_t GetTDCNeg() {return GetData(3, 0);}

  Int_t GetTDCPos(UInt_t ihit) {return GetData(2, ihit);}
  Int_t GetTDCNeg(UInt_t ihit) {return GetData(3, ihit);}

 protected:
  Int_t fADC_pos[MAXHITS];
  Int_t fADC_neg[MAXHITS];
  Int_t fTDC_pos[MAXHITS];
  Int_t fTDC_neg[MAXHITS];
  Int_t fReferenceTime[4];
  Bool_t fHasRef[4];
  UInt_t fNRawHits[4];

 private:

  ClassDef(THcRawHodoHit, 0);	// Raw Hodoscope hit
};  

#endif
