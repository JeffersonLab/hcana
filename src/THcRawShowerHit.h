#ifndef ROOT_THcRawShowerHit
#define ROOT_THcRawShowerHit

#include "THcRawHit.h"

#define MAXSAMPLES 126

class THcRawShowerHit : public THcRawHit {

 public:
  friend class THcShowerPlane;
  friend class THcShowerArray;

  THcRawShowerHit(Int_t plane=0, Int_t counter=0) : 
    THcRawHit(plane, counter), fNPosSamples(0), fNNegSamples(0) {
  }
  THcRawShowerHit& operator=( const THcRawShowerHit& );
  virtual ~THcRawShowerHit() {}

  virtual void Clear( Option_t* opt="" )
  { fNPosSamples=0; fNNegSamples=0;}

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, Int_t isamplow, Int_t isamphigh,
		Int_t iintegrallow, Int_t iintegralhigh);
  Int_t GetSample(Int_t signal, UInt_t isample);
  Double_t GetPedestal(Int_t signal, Int_t isamplow, Int_t isamphigh);
  Int_t GetNSamples(Int_t signal);

  Int_t GetNSignals() { return 2;}
  ESignalType GetSignalType(Int_t signal) {
    return kADC;
  }
  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;

 protected:
  UInt_t fNPosSamples;
  UInt_t fNNegSamples;
  // Is there a way we could pass sample size from the detector initialization
  Int_t fADC_pos[MAXSAMPLES];
  Int_t fADC_neg[MAXSAMPLES];

 private:

  ClassDef(THcRawShowerHit, 0);	// Raw Shower counter hit
};  

#endif
 
