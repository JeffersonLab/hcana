#ifndef ROOT_THcRawAdcHit
#define ROOT_THcRawAdcHit

#include "TObject.h"


class THcRawAdcHit : public TObject {
  public:
    THcRawAdcHit();
    THcRawAdcHit& operator=(const THcRawAdcHit& right);
    virtual ~THcRawAdcHit();

    virtual void Clear(Option_t* opt="");

    void SetData(Int_t data);
    void SetSample(Int_t data);
    void SetDataTimePedestalPeak(
      Int_t data, Int_t time, Int_t pedestal, Int_t peak
    );

    Int_t GetRawData(UInt_t iPulse=0);
    Int_t GetAdcTime(UInt_t iPulse=0);
    Int_t GetAdcPedestal(UInt_t iPulse=0);
    Int_t GetAdcPeak(UInt_t iPulse=0);
    Int_t GetSample(UInt_t iSample);

    Double_t GetAverage(UInt_t iSampleLow, UInt_t iSampleHigh);
    Int_t GetIntegral(UInt_t iSampleLow, UInt_t iSampleHigh);
    Double_t GetData(
      UInt_t iPedLow, UInt_t iPedHigh, UInt_t iIntLow, UInt_t iIntHigh
    );

    UInt_t GetNPulses();
    UInt_t GetNSamples();

    Bool_t HasMulti();

  protected:
    static const UInt_t fMaxNPulses = 4;
    static const UInt_t fMaxNSamples = 160;

    Int_t fAdc[fMaxNPulses];
    Int_t fAdcTime[fMaxNPulses];
    Int_t fAdcPedestal[fMaxNPulses];
    Int_t fAdcPeak[fMaxNPulses];
    Int_t fAdcSample[fMaxNSamples];

    Bool_t fHasMulti;
    UInt_t fNPulses;
    UInt_t fNSamples;

  private:
    ClassDef(THcRawAdcHit, 0)
};


#endif  // ROOT_THcRawAdcHit
