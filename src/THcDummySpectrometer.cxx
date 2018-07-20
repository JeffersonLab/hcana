/**
\class THcDummySpectrometer
\ingroup Apparatuses

\brief A dummy spectrometer apparatus for testing detectors.

This class behaves as a spectrometer apparatus in that it holds detector
classes, but it does no reconstruction. It is intended to provide an elegant
way to setup a teststand for a detector or a set of detectors, without
requiring some "needed" detectors or parameters.
*/

/**
\fn THcDummySpectrometer::THcDummySpectrometer(const char* name, const char* description)

\brief A constructor.

\param[in] name Name of the apparatus. Is typically "H", "P" or "S" for
HMS, SHMS and SOS, respectively.
\param[in] description Description of the apparatus.
*/

/**
\fn virtual THcDummySpectrometer::~THcDummySpectrometer()

\brief A destructor.
*/

/**
\fn Int_t THcDummySpectrometer::Reconstruct()

\brief Does nothing and returns 0.

This function is usually responsible for reconstructing the event from the
detectors in the spectrometer. Since this is a dummy apparatus, we have a dummy
reconstruction.
*/

/**
\fn std::string THcDummySpectrometer::GetKwPrefix()

\brief Returns prefix used for parameters in `param` files.

All the parameters read by this apparatus are prefixed with the returned string.
*/

#include "THcDummySpectrometer.h"

#include "TDatime.h"

#include "THcGlobals.h"
#include "THcParmList.h"


THcDummySpectrometer::THcDummySpectrometer(
  const char* name, const char* description
) :
  THaApparatus(name, description),
  fKwPrefix("")
{
  // Construct the kwPrefix here. Better place would be in `Setup` method.
  TString kwPrefix = name;
  kwPrefix.ToUpper();
  fKwPrefix = kwPrefix;
}


THcDummySpectrometer::~THcDummySpectrometer() {}


Int_t THcDummySpectrometer::Reconstruct() {
  // Don't need reconstruction here.
  return 0;
}


TString THcDummySpectrometer::GetKwPrefix() {
  return fKwPrefix;
}


Int_t THcDummySpectrometer::ReadRunDatabase(const TDatime& date) {
  return kOK;
}


Int_t THcDummySpectrometer::ReadDatabase(const TDatime& date) {
  return kOK;
}


Int_t THcDummySpectrometer::DefineVariables(THaAnalysisObject::EMode mode) {
  if (mode == kDefine && fIsSetup) return kOK;
  fIsSetup = (mode == kDefine);

  return kOK;
  //  std::vector<RVarDef> vars;
  //  RVarDef end {0};
  //  vars.push_back(end);

  //  return DefineVarsFromList(vars.data(), mode);
}


THcDummySpectrometer::THcDummySpectrometer() {}


ClassImp(THcDummySpectrometer)
