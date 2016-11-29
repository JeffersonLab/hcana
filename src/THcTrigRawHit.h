#ifndef ROOT_THcTrigRawHit
#define ROOT_THcTrigRawHit


#include "THcRawHit.h"


class THcTrigRawHit : public THcRawHit {
  public:
    THcTrigRawHit(Int_t plane=0, Int_t counter=0);
    THcTrigRawHit& operator=(const THcTrigRawHit& right);
    virtual ~THcTrigRawHit();

    virtual void Clear(Option_t* opt="");

    void SetData(Int_t signal, Int_t data);
    void SetReference(Int_t signal, Int_t reference);
    Int_t GetData(Int_t signal);
    Int_t GetReference(Int_t signal);
    Bool_t HasReference(Int_t signal);

  protected:
    static const Int_t fMaxAdcSamples = 160;
    static const Int_t fMaxTdcSamples = 16;
    static const Int_t fNumPlanes = 2;

    Int_t fAdcVal[fMaxAdcSamples];
    Int_t fTdcVal[fMaxTdcSamples];
    UInt_t fNumAdcSamples;
    UInt_t fNumTdcSamples;

    Int_t fAdcReferenceTime;
    Int_t fTdcReferenceTime;
    Bool_t fHasAdcRef;
    Bool_t fHasTdcRef;

  private:
    ClassDef(THcTrigRawHit, 0);
};


#endif  // ROOT_THcTrigRawHit
