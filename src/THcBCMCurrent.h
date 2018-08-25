#ifndef __THCBCMCURRENT_H__
#define __THCBCMCURRENT_H__

#include "THaPhysicsModule.h"
#include "THaEvData.h"
#include "VarDef.h"
#include "VarType.h"

#include <iostream>
#include <map>

class THcBCMCurrent : public THaPhysicsModule {
    
 public:
  
  THcBCMCurrent(const char* name, const char* description);
  virtual ~THcBCMCurrent();

  virtual EStatus Init( const TDatime& date);
  virtual Int_t Process( const THaEvData& );  

  enum BCMopt {BCM1, BCM2, UNSER, BCM4A, BCM4B, BCM4C};

 private:
  
  Int_t     fNscaler;
  Double_t  fThreshold;
  Int_t     fBCMIndex;
  Double_t* fiBCM1;
  Double_t* fiBCM2;
  Double_t* fiBCM4a;
  Double_t* fiBCM4b;
  Double_t* fiBCM4c;
  Int_t*    fEvtNum;

  Int_t    fBCMflag;

  Double_t fBCM1avg;
  Double_t fBCM2avg;
  Double_t fBCM4aavg;
  Double_t fBCM4bavg;
  Double_t fBCM4cavg;

  struct BCMInfo{
    Double_t bcm1_current;
    Double_t bcm2_current;
    Double_t bcm4a_current;
    Double_t bcm4b_current;
    Double_t bcm4c_current;
  };

  std::map<Int_t, BCMInfo> BCMInfoMap;

  Int_t GetAvgCurrent( Int_t fevn, BCMInfo &bcminfo );
  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t DefineVariables( EMode mode = kDefine );

  ClassDef(THcBCMCurrent, 0)

};  

#endif
