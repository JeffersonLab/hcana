#ifndef ROOT_THcDummySpectrometer
#define ROOT_THcDummySpectrometer

#include "TString.h"

#include "THaAnalysisObject.h"
#include "THaApparatus.h"


class TDatime;


class THcDummySpectrometer : public THaApparatus {
  public:
    THcDummySpectrometer(const char* name, const char* description);
    virtual ~THcDummySpectrometer();

    virtual Int_t Reconstruct();

    TString GetKwPrefix();

  protected:
    virtual Int_t ReadRunDatabase(const TDatime& date);
    virtual Int_t ReadDatabase(const TDatime& date);
    virtual Int_t DefineVariables(EMode mode=kDefine);

    TString fKwPrefix;

  private:
    THcDummySpectrometer();
    ClassDef(THcDummySpectrometer, 0);
};


#endif  // ROOT_THcDummySpectrometer
