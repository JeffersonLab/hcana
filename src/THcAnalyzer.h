#ifndef ROOT_THcAnalyzer
#define ROOT_THcAnalyzer

//////////////////////////////////////////////////////////////////////////
//
// THcAnalyzer
//
//////////////////////////////////////////////////////////////////////////

#include "THaAnalyzer.h"

#include "Logger.h"

class THcAnalyzer : public hcana::ConfigLogging<THaAnalyzer> {

public:

  THcAnalyzer();
  virtual ~THcAnalyzer();

  void SetPedestalEvtype( Int_t evtype ) { fPedestalEvtype = evtype; }

  void PrintReport( const char* templatefile, const char* ofile);

protected:

  Int_t fPedestalEvtype;

private:
  //  THcAnalyzer( const THcAnalyzer& );
  //  THcAnalyzer& operator=( const THcAnalyzer& );
  void LoadInfo();

  ClassDef(THcAnalyzer,0)  //Hall C Analyzer Standard Event Loop

};

#endif
