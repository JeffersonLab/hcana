#ifndef ROOT_THcRawHodoHit
#define ROOT_THcRawHodoHit

#include "THcRawAdcHit.h"
#include "THcRawHit.h"
#include "THcRawTdcHit.h"


class THcRawHodoHit : public THcRawHit {
  friend class THcScintillatorPlane;
  friend class THcHodoscope;
  friend class THcHodoHit;

  public:

    THcRawHodoHit(Int_t plane=0, Int_t counter=0);
    THcRawHodoHit& operator=(const THcRawHodoHit& right);
    virtual ~THcRawHodoHit();

    virtual void Clear(Option_t* opt="");

    virtual void SetData(Int_t signal, Int_t data);
    virtual void SetSample(Int_t signal, Int_t data);
    virtual void SetSampThreshold(Int_t signal, Double_t thres);
    virtual void SetSampIntTimePedestalPeak(Int_t signal);
    virtual void SetDataTimePedestalPeak(
      Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
    );
    virtual void SetReference(Int_t signal, Int_t reference);
    virtual void SetReferenceDiff(Int_t signal, Int_t referenceDiff);

    virtual Int_t GetData(Int_t signal);
    virtual Int_t GetRawData(Int_t signal);
    virtual Int_t GetReference(Int_t signal);
    virtual Int_t GetReferenceDiff(Int_t signal);
    virtual ESignalType GetSignalType(Int_t signal);
    virtual Int_t GetNSignals();

    virtual Bool_t HasReference(Int_t signal);

    THcRawAdcHit& GetRawAdcHitPos();
    THcRawAdcHit& GetRawAdcHitNeg();
    THcRawTdcHit& GetRawTdcHitPos();
    THcRawTdcHit& GetRawTdcHitNeg();

    void SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED);

  protected:
    static const Int_t fNAdcSignals = 2;
    static const Int_t fNTdcSignals = 2;

    THcRawAdcHit fAdcHits[fNAdcSignals];
    THcRawTdcHit fTdcHits[fNTdcSignals];

  private:
    ClassDef(THcRawHodoHit, 0);  // Raw Hodoscope hit
};


#endif  // ROOT_THcRawHodoHit
