#ifndef ROOT_THcRawTdcHit
#define ROOT_THcRawTdcHit

#include "TObject.h"


class THcRawTdcHit : public TObject {
  public:
    THcRawTdcHit();
    THcRawTdcHit& operator=(const THcRawTdcHit& right);
    virtual ~THcRawTdcHit();

    virtual void Clear(Option_t* opt="");

    void SetTime(Int_t time);
    void SetRefTime(Int_t refTime);
    void SetRefDiffTime(Int_t refDiffTime);

    Int_t GetTimeRaw(UInt_t iHit=0) const;
    Int_t GetTime(UInt_t iHit=0) const;
    Int_t GetRefTime() const;
    Int_t GetRefDiffTime() const;

    Bool_t HasRefTime() const;

    UInt_t GetNHits() const;

  protected:
    static const UInt_t fMaxNHits = 128;

    Double_t fChannelToTimeFactor;

    Int_t fTime[fMaxNHits];
    Int_t fRefTime;
    Int_t fRefDiffTime;

    Bool_t fHasRefTime;
    UInt_t fNHits;

  private:
    ClassDef(THcRawTdcHit, 0)
};


#endif  // ROOT_THcRawTdcHit
