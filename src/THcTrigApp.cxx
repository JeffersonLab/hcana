/**
\class THcTrigApp
\ingroup Apparatuses

\brief A mock spectrometer to hold all trigger related data.

This class behaves as a spectrometer apparatus, but it does not correspond to
any physical spectrometer in the hall. Its purpose is to gather all the trigger
related data.

Use only with THcTrigDet class.
*/

/**
\fn THcTrigApp::THcTrigApp(const char* name, const char* description)

\brief A constructor.

\param[in] name Name of the apparatus. Is typically "T".
\param[in] description Description of the apparatus.
*/

/**
\fn virtual THcTrigApp::~THcTrigApp()

\brief A destructor.
*/

/**
\fn Int_t THcTrigApp::Reconstruct()

\brief Does nothing and returns 0.

This function is usually responsible for reconstructing the event from the
detectors in the spectrometer. Since this is a mock apparatus, we have a mock
reconstruction.
*/

/**
\fn std::string THcTrigApp::GetKwPrefix()

\brief Returns prefix used for parameters in `param` files.

All the parameters read by this apparatus are prefixed with the returned string.
*/

#include "THcTrigApp.h"

#include <algorithm>

#include "TDatime.h"

#include "THcGlobals.h"
#include "THcParmList.h"


THcTrigApp::THcTrigApp(const char* name, const char* description)
:
  THaApparatus(name, description),
  fKwPrefix("")
{
  // Construct the kwPrefix here. Better place would be in `Setup` method.
  string kwPrefix = name;
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
}


THcTrigApp::~THcTrigApp() {}


Int_t THcTrigApp::Reconstruct() {
  // Don't need reconstruction here.
  return 0;
}


std::string THcTrigApp::GetKwPrefix() {
  return fKwPrefix;
}


Int_t THcTrigApp::ReadRunDatabase(const TDatime& date) {
  return kOK;
}


Int_t THcTrigApp::ReadDatabase(const TDatime& date) {
  return kOK;
}


Int_t THcTrigApp::DefineVariables(THaAnalysisObject::EMode mode) {
  if (mode == kDefine && fIsSetup) return kOK;
  fIsSetup = (mode == kDefine);

  return kOK;
  //  std::vector<RVarDef> vars;
  //  RVarDef end {0};
  //  vars.push_back(end);

  //  return DefineVarsFromList(vars.data(), mode);
}


THcTrigApp::THcTrigApp() {}


ClassImp(THcTrigApp)
