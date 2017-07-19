#ifndef ROOT_THcPeriodicReport
#define ROOT_THcPeriodicReport

//////////////////////////////////////////////////////////////////////////
//
// THcPeriodicReport
//
//////////////////////////////////////////////////////////////////////////

#include "THaPhysicsModule.h"
#include "THcAnalyzer.h"

class THcPeriodicReport : public THaPhysicsModule {

public:
  THcPeriodicReport(const char *name, const char *description,
                    const char *templatefile, const char *ofile);
  virtual ~THcPeriodicReport();

  virtual EStatus Init(const TDatime &run_time);
  virtual Int_t Begin(THaRunBase *r = 0);
  virtual Int_t End(THaRunBase *r = 0);
  virtual Int_t Process(const THaEvData &);
  void PrintReport();

  virtual void SetEventPeriod(Int_t ev) { fEventPeriod = ev; }
  virtual void SetTimePeriod(UInt_t t) { fTimePeriod = t; }

protected:
  UInt_t fLastPrintTime;
  Int_t fEventsSincePrint;
  UInt_t fTimePeriod;
  Int_t fEventPeriod;
  Bool_t fDoPrint;
  THcAnalyzer *fAnalyzer;
  TString fTemplateFilename;
  TString fOutputFilename;

  ClassDef(THcPeriodicReport, 0)
};
#endif
