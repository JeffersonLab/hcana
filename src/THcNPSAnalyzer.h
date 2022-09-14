#ifndef ROOT_THcNPSAnalyzer
#define ROOT_THcNPSAnalyzer

//////////////////////////////////////////////////////////////////////////
//
// THcNPSAnalyzer
//
//////////////////////////////////////////////////////////////////////////

#include "THcAnalyzer.h"

class THcNPSAnalyzer : public THcAnalyzer {

public:

  THcNPSAnalyzer();
  virtual ~THcNPSAnalyzer();

  Int_t PhysicsAnalysis( Int_t code );
  void SetNevMerge(Int_t nmerge) {fNevMerge = nmerge;}
  Int_t GetNevMerge() {return fNevMerge;};
  Bool_t GetClearThisEvent() {return (fNevMerge<=1?kTRUE: (fNev%fNevMerge)==1);};
  Bool_t GetProcessThisEvent() {return (fNevMerge<=1?kTRUE: (fNev%fNevMerge)==0);};
  
protected:

  Int_t fPedestalEvtype;

  Int_t fNevMerge;

private:

  ClassDef(THcNPSAnalyzer,0)  //Hall C Analyzer Standard Event Loop

};

#endif
