#ifndef ROOT_THcTrigRawHit
#define ROOT_THcTrigRawHit


#include "THcRawHit.h"
#include "THcRawAdcHit.h"
#include "THcRawTdcHit.h"


class THcTrigRawHit : public THcRawHit {
  public:
    THcTrigRawHit(Int_t plane=0, Int_t counter=0);
    THcTrigRawHit& operator=(const THcTrigRawHit& right);
    virtual ~THcTrigRawHit();

    virtual void Clear(Option_t* opt="");

    void SetData(Int_t signal, Int_t data);
    void SetSample(Int_t signal, Int_t data);
    void SetDataTimePedestalPeak(
      Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
    );
    void SetReference(Int_t signal, Int_t reference);

    Int_t GetData(Int_t signal);
    Int_t GetRawData(Int_t signal);
    Int_t GetReference(Int_t signal);
    ESignalType GetSignalType(Int_t signal);
    Int_t GetNSignals();

    Bool_t HasReference(Int_t signal);

    THcRawAdcHit& GetRawAdcHit();
    THcRawTdcHit& GetRawTdcHit();

    void SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED);

  protected:
    static const Int_t fNAdcSignals = 1;
    static const Int_t fNTdcSignals = 1;

    THcRawAdcHit fAdcHits[fNAdcSignals];
    THcRawTdcHit fTdcHits[fNTdcSignals];

  private:
    ClassDef(THcTrigRawHit, 0);
};


#endif  // ROOT_THcTrigRawHit
