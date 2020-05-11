#ifndef ROOT_THcRawDCHit
#define ROOT_THcRawDCHit

#include "THcRawHit.h"
#include "THcRawTdcHit.h"


class THcRawDCHit : public THcRawHit {
  friend class THcDriftChamberPlane;
  friend class THcDC;

  public:
    THcRawDCHit(Int_t plane=0, Int_t counter=0);
    THcRawDCHit& operator=(const THcRawDCHit& right);
    virtual ~THcRawDCHit();

    virtual void Clear(Option_t* opt="");

    virtual void SetData(Int_t signal, Int_t data);
    virtual void SetReference(Int_t signal, Int_t reference);
    virtual void SetReferenceDiff(Int_t signal, Int_t reference);

    virtual Int_t GetData(Int_t signal);
    virtual Int_t GetRawData(Int_t signal);
    virtual Int_t GetReference(Int_t signal);
    virtual Int_t GetReferenceDiff(Int_t signal);
    virtual ESignalType GetSignalType(Int_t signal);
    virtual Int_t GetNSignals();

    virtual Bool_t HasReference(Int_t signal);

    THcRawTdcHit& GetRawTdcHit();

  protected:
    static const Int_t fNTdcSignals = 1;

    THcRawTdcHit fTdcHit;

  private:
    ClassDef(THcRawDCHit, 0);	// Raw Drift Chamber hit
};

#endif
