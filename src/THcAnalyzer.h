#ifndef ROOT_THcAnalyzer
#define ROOT_THcAnalyzer

//////////////////////////////////////////////////////////////////////////
//
// THcAnalyzer
// 
//////////////////////////////////////////////////////////////////////////

#include "THaAnalyzer.h"

class THcAnalyzer : public THaAnalyzer {

public:

  THcAnalyzer();
  virtual ~THcAnalyzer();

protected:
    
private:
  //  THcAnalyzer( const THcAnalyzer& );
  //  THcAnalyzer& operator=( const THcAnalyzer& );
  
  ClassDef(THcAnalyzer,0)  //Hall C Analyzer Standard Event Loop

};

#endif
