/**
\class THcPeriodicReport
\ingroup PhysMods

\brief A physics module to generate periodic reports from a template

This class is a physics module, but does not do analysis.  It repeatedly
outputs a report file from a template using an event count or time based
period.  This report file could be displayed or used by a GUI to show
a realtime status of an analysis.

The THcAnalyzer::PrintReport method is used to generate the report.
By default this report is generated every two seconds.

*/

/**
\fn THcPeriodicReport::THcPeriodicReport(
    const char* name, const char* description="",
    const char *templatefile, const char* ofile)

\brief A constructor

\param[in] name Name of the apparatus. Is typically named after spectrometer
  whose trigger data is collecting; like "HMS".
\param[in] description Description of the report
\param[in] templatefile Name of file containing report templte
\parma[in] ofile File to write the report to
*/

/**
\fn virtual THcPeriodicReport::~THcPeriodicReport()

\brief A destructor.
*/

/**
\fn virtual void SetEventPeriod(Int_t ev)

\brief Set report print out periodicity

\param[in] ev Print out report every ev events
*/

/**
\fn virtual void SetTimePeriod(UInt_t t)

\brief Set report print out periodicity

\param[in] t Print out report ever t seconds
*/

#include "THcPeriodicReport.h"

#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcPeriodicReport::THcPeriodicReport(const char *name, const char *description,
                                     const char *templatefile,
                                     const char *ofile)
    : THaPhysicsModule(name, description), fTimePeriod(2), fEventPeriod(0),
      fDoPrint(kFALSE), fAnalyzer(0) {
  // Constructor
  fTemplateFilename = templatefile;
  fOutputFilename = ofile;
}
//_____________________________________________________________________________
THcPeriodicReport::~THcPeriodicReport() {
  // destructor
}
//_____________________________________________________________________________
THaAnalysisObject::EStatus THcPeriodicReport::Init(const TDatime &run_time) {
  // Standard initialization. Calls this object's DefineVariables().
  if (THaPhysicsModule::Init(run_time) != kOK)
    return fStatus;

  fAnalyzer = static_cast<THcAnalyzer *>(THcAnalyzer::GetInstance());
  return fStatus = kOK;
}
//_____________________________________________________________________________
Int_t THcPeriodicReport::Begin(THaRunBase *) {
  fDoPrint = kTRUE; // Generate report on first event
  fLastPrintTime = TDatime().Convert();
  fEventsSincePrint = 0;

  return 0;
}
//_____________________________________________________________________________
Int_t THcPeriodicReport::End(THaRunBase *) {
  // Print out the report a final time
  PrintReport();

  return 0;
}
//_____________________________________________________________________________
Int_t THcPeriodicReport::Process(const THaEvData &) {
  UInt_t now = TDatime().Convert();
  fEventsSincePrint++;
  if (!fDoPrint && (fTimePeriod > 0) &&
      ((now - fLastPrintTime) >= fTimePeriod)) {
    fDoPrint = kTRUE;
  }
  if (!fDoPrint && (fEventPeriod > 0) && (fEventsSincePrint >= fEventPeriod)) {
    fDoPrint = kTRUE;
  }
  if (fDoPrint) {
    fLastPrintTime = now;
    fEventsSincePrint = 0;
    PrintReport();
    fDoPrint = kFALSE;
  }
  return (0);
}
//_____________________________________________________________________________
void THcPeriodicReport::PrintReport() {
  fAnalyzer->PrintReport(fTemplateFilename, fOutputFilename);
}
///////////////////////////////////////////////////////////////////////////////
ClassImp(THcPeriodicReport)
