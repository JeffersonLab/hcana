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

  void SetCurrentCut(Double_t _threshold){ fThreshold = _threshold; }

 private:
  
  Int_t CheckGoodEvent( Int_t fevn );
  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t DefineVariables( EMode mode = kDefine );

  Int_t     fNscaler;
  Double_t  fThreshold;
  Double_t* fiBCM1;
  Double_t* fiBCM2;
  Int_t*    fEvtNum;

  Bool_t    fPrevious; 

  Int_t fBCM1flag;
  Int_t fBCM2flag;

  enum {kBeamOff, kBeamOn, kCarryOn};

  struct BCMInfo{
    Double_t bcm1_current;
    Double_t bcm2_current;
  };

  std::map<Int_t, BCMInfo> BCMInfoMap;

  ClassDef(THcBCMCurrent, 0)

};  

#endif
