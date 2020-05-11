#ifndef ROOT_THcRawShowerHit
#define ROOT_THcRawShowerHit

#include "THcRawAdcHit.h"
#include "THcRawHit.h"


class THcRawShowerHit : public THcRawHit {
  friend class THcShowerPlane;
  friend class THcShowerArray;

  public:
    THcRawShowerHit(Int_t plane=0, Int_t counter=0);
    THcRawShowerHit& operator=(const THcRawShowerHit& right);
    virtual ~THcRawShowerHit();

    virtual void Clear(Option_t* opt="");

    virtual void SetData(Int_t signal, Int_t data);
    virtual void SetSample(Int_t signal, Int_t data);
    virtual void SetDataTimePedestalPeak(
      Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
    );
    virtual void SetReference(Int_t signal, Int_t reference);
    virtual void SetReferenceDiff(Int_t signal, Int_t reference);

    virtual Int_t GetData(Int_t signal);
    virtual Int_t GetRawData(Int_t signal);
    virtual ESignalType GetSignalType(Int_t signal);
    virtual Int_t GetNSignals();

    THcRawAdcHit& GetRawAdcHitPos();
    THcRawAdcHit& GetRawAdcHitNeg();

    void SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED);

  protected:
    static const Int_t fNAdcSignals = 2;

    THcRawAdcHit fAdcHits[fNAdcSignals];

  private:
    ClassDef(THcRawShowerHit, 0);  // Raw Shower counter hit
};


#endif
