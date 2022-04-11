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

    virtual void SetSpectName( const char* name);
    virtual void AddEvtType(int evtype);
    virtual void SetEvtType(int evtype);
    virtual Bool_t IsIgnoreType(Int_t evtype) const;
    virtual Bool_t HaveIgnoreList() const;
    Int_t          End(THaRunBase* run);
    //Funtions to get TDCtime for cointime module 
    Double_t Get_CT_Trigtime(Int_t ii) { return (fTrigId[ii]==-1 ? 0. : fTdcTime[fTrigId[ii]]) ;}

    // SJDK 12/04/21 - New Getter for RF time info
    // Function to get RFTDC time for RF module
    Double_t Get_RF_TrigTime(Int_t ii) { return (fRFId[ii]==-1 ? 0. : fTdcTime[fRFId[ii]]) ;}
    
  protected:
    void Setup(const char* name, const char* description);
    virtual Int_t ReadDatabase(const TDatime& date);
    virtual Int_t DefineVariables(EMode mode=kDefine);

    std::string fKwPrefix;

    Int_t fNumAdc;
    Int_t fNumTdc;
    Int_t fTrigId[4];
    Int_t fRFId[2]; // SJDK 12/04/21 -  New RF ID for getter above

    Double_t fAdcTdcOffset;
    Double_t fTdcOffset;
    Double_t fTdcChanperNS;

  Int_t  fOutputSampWaveform;
  Int_t  fUseSampWaveform;
  Double_t  fSampThreshold;
  Int_t  fSampNSA;
  Int_t  fSampNSAT;
  Int_t  fSampNSB;
 
    std::vector<std::string> fAdcNames;
    std::vector<std::string> fTdcNames;
    std::vector<std::string> fTrigNames;
    std::vector<std::string> fRFNames;

    static const int fMaxAdcChannels = 200;
    static const int fMaxTdcChannels = 200;

    Int_t fTdcTimeRaw[fMaxTdcChannels];
    Double_t fTdcTime[fMaxTdcChannels];
    Int_t fTDC_RefTimeCut;
    Int_t fADC_RefTimeCut;

    Double_t *fAdcTimeWindowMin;    
    Double_t *fAdcTimeWindowMax;    
    Double_t *fTdcTimeWindowMin;    
    Double_t *fTdcTimeWindowMax;    
    
    Int_t fAdcPedRaw[fMaxAdcChannels];
    Int_t fAdcPulseIntRaw[fMaxAdcChannels];
    Int_t fAdcPulseAmpRaw[fMaxAdcChannels];
    Int_t fAdcPulseTimeRaw[fMaxAdcChannels];

    Double_t fAdcPed[fMaxAdcChannels];
    Double_t fAdcPulseInt[fMaxAdcChannels];
    Double_t fAdcPulseAmp[fMaxAdcChannels];
    Double_t fAdcPulseTime[fMaxAdcChannels];

    Int_t fAdcSampPedRaw[fMaxAdcChannels];
    Int_t fAdcSampPulseIntRaw[fMaxAdcChannels];
    Int_t fAdcSampPulseAmpRaw[fMaxAdcChannels];
    Int_t fAdcSampPulseTimeRaw[fMaxAdcChannels];

    Double_t fAdcSampPed[fMaxAdcChannels];
    Double_t fAdcSampPulseInt[fMaxAdcChannels];
    Double_t fAdcSampPulseAmp[fMaxAdcChannels];
    Double_t fAdcSampPulseTime[fMaxAdcChannels];

    std::vector<Double_t> fSampWaveform;

    Int_t fTdcMultiplicity[fMaxTdcChannels];
    Int_t fAdcMultiplicity[fMaxAdcChannels];
    Int_t fAdcSampMultiplicity[fMaxAdcChannels];
    Double_t fTdcRefTime;

    TString fSpectName;
    std::vector<Int_t> eventtypes;
    Bool_t* fPresentP;

  private:
    THcTrigDet();
    ClassDef(THcTrigDet, 0);
};


#endif  // ROOT_THcTrigDet
