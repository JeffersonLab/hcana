#ifndef ROOT_THcRawDCHit
#define ROOT_THcRawDCHit

#include "THcRawHit.h"

class THcRawDCHit : public THcRawHit {

public:
  friend class THcDriftChamberPlane;
  friend class THcDC;

  THcRawDCHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter),
    fNHits(0), fHasRef(kFALSE) {
  }
  THcRawDCHit& operator=( const THcRawDCHit& );
  virtual ~THcRawDCHit() {}

  virtual void Clear( Option_t* opt="" ) { fNHits=0; fHasRef=kFALSE; }

  void SetData(Int_t signal, Int_t data);
  void SetReference(Int_t signal, Int_t reference);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, UInt_t ihit);
  Int_t GetRawData(Int_t signal);
  Int_t GetRawData(Int_t signal, UInt_t ihit);
  Int_t GetReference(Int_t signal);
  Bool_t HasReference(Int_t signal) {return fHasRef;}

  Int_t GetNSignals() { return 1;}
  ESignalType GetSignalType(Int_t signal) {
    return(kTDC);
  }

  virtual Bool_t  IsSortable () const {return kTRUE; }
  virtual Int_t   Compare(const TObject* obj) const;

  UInt_t GetMaxNSamplesTDC() {return fMaxNSamplesTDC;}

protected:
  static const UInt_t fMaxNSamplesTDC = 128;
  UInt_t fNHits;
  Int_t fTDC[fMaxNSamplesTDC];
  Int_t fReferenceTime;
  Int_t fHasRef;

private:

  ClassDef(THcRawDCHit, 0);	// Raw Drift Chamber hit
};

#endif
