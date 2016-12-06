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
    void SetSample(Int_t signal, Int_t data);
    void SetDataTimePedestalPeak(
      Int_t signal, Int_t data, Int_t time, Int_t pedestal, Int_t peak
    );
    void SetReference(Int_t signal, Int_t reference);

    Int_t GetData(Int_t signal);
    Int_t GetData(Int_t signal, UInt_t iHit);
    Int_t GetRawData(Int_t signal);
    Int_t GetRawData(Int_t signal, UInt_t iHit);
    Int_t GetAdcTime(UInt_t iHit);
    Int_t GetAdcPedestal(UInt_t iHit);
    Int_t GetAdcPeak(UInt_t iHit);
    Int_t GetNSignals();
    ESignalType GetSignalType(Int_t signal);
    Int_t GetReference(Int_t signal);
    Int_t GetMultiplicity(Int_t signal);

    Bool_t HasMulti(Int_t signal);
    Bool_t HasReference(Int_t signal);


  protected:
    static const Int_t fMaxNPulsesAdc = 4;
    static const Int_t fMaxNSamplesAdc = 160;
    static const Int_t fMaxNHitsTdc = 16;
    static const Int_t fNPlanes = 2;

    Int_t fAdc[fMaxNPulsesAdc];
    Int_t fAdcTime[fMaxNPulsesAdc];
    Int_t fAdcPedestal[fMaxNPulsesAdc];
    Int_t fAdcPeak[fMaxNPulsesAdc];

    Int_t fAdcSamples[fMaxNSamplesAdc];

    Int_t fTdc[fMaxNHitsTdc];

    Int_t fReferenceTime[fNPlanes];
    Bool_t fHasReference[fNPlanes];
    Bool_t fHasMulti[fNPlanes];
    UInt_t fNRawHits[fNPlanes];

    UInt_t fNRawSamples;

  private:
    ClassDef(THcTrigRawHit, 0);
};


#endif  // ROOT_THcTrigRawHit
