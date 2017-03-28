#ifndef ROOT_THcTrigDet
#define ROOT_THcTrigDet

#include <string>
#include <vector>

#include "THaDetector.h"
#include "THcHitList.h"

class TDatime;

class THaApparatus;
class THaEvData;


class THcTrigDet : public THaDetector, public THcHitList {
  public:
    THcTrigDet(
      const char* name, const char* description="",
      THaApparatus* app=NULL
    );
    ~THcTrigDet();

    virtual EStatus Init(const TDatime& date);

    virtual void Clear(Option_t* opt="");
    Int_t Decode(const THaEvData& evData);

  protected:
    void Setup(const char* name, const char* description);
    virtual Int_t ReadDatabase(const TDatime& date);
    virtual Int_t DefineVariables(EMode mode=kDefine);

    std::string fKwPrefix;

    Int_t fNumAdc;
    Int_t fNumTdc;

    Double_t fTdcOffset;
    Double_t fTdcChanperNS;

    std::vector<std::string> fAdcNames;
    std::vector<std::string> fTdcNames;

    static const int fMaxAdcChannels = 100;
    static const int fMaxTdcChannels = 100;

    Int_t fTdcTimeRaw[fMaxTdcChannels];
    Double_t fTdcTime[fMaxTdcChannels];

    Int_t fAdcPedRaw[fMaxAdcChannels];
    Int_t fAdcPulseIntRaw[fMaxAdcChannels];
    Int_t fAdcPulseAmpRaw[fMaxAdcChannels];
    Int_t fAdcPulseTimeRaw[fMaxAdcChannels];

    Double_t fAdcPed[fMaxAdcChannels];
    Double_t fAdcPulseInt[fMaxAdcChannels];
    Double_t fAdcPulseAmp[fMaxAdcChannels];

    Int_t fTdcMultiplicity[fMaxTdcChannels];
    Int_t fAdcMultiplicity[fMaxAdcChannels];

  private:
    THcTrigDet();
    ClassDef(THcTrigDet, 0);
};


#endif  // ROOT_THcTrigDet
