#ifndef ROOT_THcTrigApp
#define ROOT_THcTrigApp

#include <string>
#include <vector>

#include "THaAnalysisObject.h"
#include "THaApparatus.h"


class TDatime;


class THcTrigApp : public THaApparatus {
  public:
    THcTrigApp(const char* name, const char* description);
    virtual ~THcTrigApp();

    virtual Int_t Reconstruct();

    std::string GetKwPrefix();

  protected:
    virtual Int_t ReadRunDatabase(const TDatime& date);
    virtual Int_t ReadDatabase(const TDatime& date);
    virtual Int_t DefineVariables(EMode mode=kDefine);

    std::string fKwPrefix;

  private:
    THcTrigApp();
    ClassDef(THcTrigApp, 0);
};


#endif  // ROOT_THcTrigApp
