/**
\class THcTrigDet
\ingroup Detectors

\brief A mock detector to hold trigger related data.

This class behaves as a detector, but it does not correspond to any physical
detector in the hall. Its purpose is to gather all the trigger related data
comming from a specific source, like HMS.

Use only with THcTrigApp class.

Note: not yet finalized!
*/

/**
\fn THcTrigDet::THcTrigDet(
  const char* name, const char* description="",
  THaApparatus* app=NULL)

\brief A constructor.

\param[in] name Name of the apparatus. Is typically named after spectrometer
  whose trigger data is collecting; like "HMS".
\param[in] description Description of the apparatus.
\param[in] app The parent apparatus pointer.
*/

/**
\fn virtual THcTrigDet::~THcTrigDet()

\brief A destructor.
*/

/**
\fn virtual THaAnalysisObject::EStatus THcTrigDet::Init(const TDatime& date)

\brief Initializes the detector variables.

\param[in] date Time of the current run.
*/

/**
\fn virtual void THcTrigDet::Clear(Option_t* opt="")

\brief Clears variables before next event.

\param[in] opt Maybe used in base clas...
*/

/**
\fn Int_t THcTrigDet::Decode(const THaEvData& evData)

\brief Decodes and processes events.

\param[in] evData Raw data to decode.
*/

#include "THcTrigDet.h"

#include <algorithm>

#include "TDatime.h"

#include "THaApparatus.h"
#include "THaEvData.h"

#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcTrigApp.h"
#include "THcTrigRawHit.h"


THcTrigDet::THcTrigDet() {}


THcTrigDet::THcTrigDet(
  const char* name, const char* description, THaApparatus* app
) :
  THaDetector(name, description, app), THcHitList(),
  fKwPrefix(""),
  fNumAdc(0), fNumTdc(0), fAdcNames(), fTdcNames(),
  fAdcVals(), fTdcVals()
{}


THcTrigDet::~THcTrigDet() {}


THaAnalysisObject::EStatus THcTrigDet::Init(const TDatime& date) {
  // Call `Setup` before everything else.
  Setup(GetName(), GetTitle());

  // Initialize all variables.
  for (int i=0; i<fMaxAdcChannels; ++i) fAdcVals[i] = -1.0;
  for (int i=0; i<fMaxTdcChannels; ++i) fTdcVals[i] = -1.0;

  // Call initializer for base class.
  // This also calls `ReadDatabase` and `DefineVariables`.
  EStatus status = THaDetector::Init(date);
  if (status) {
    fStatus = status;
    return fStatus;
  }

  // Initialize hitlist part of the class.
  InitHitList(fDetMap, "THcTrigRawHit", 100);

  // Fill in detector map.
  string EngineDID = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(EngineDID.begin(), EngineDID.end(), EngineDID.begin(), ::toupper);
  if (gHcDetectorMap->FillMap(fDetMap, EngineDID.c_str()) < 0) {
    static const char* const here = "Init()";
    Error(Here(here), "Error filling detectormap for %s.", EngineDID.c_str());
    return kInitError;
  }

  fStatus = kOK;
  return fStatus;
}


void THcTrigDet::Clear(Option_t* opt) {
  // Reset all data.
  for (int i=0; i<fNumAdc; ++i) fAdcVals[i] = 0.0;
  for (int i=0; i<fNumTdc; ++i) fTdcVals[i] = 0.0;
}


Int_t THcTrigDet::Decode(const THaEvData& evData) {
  // Decode raw data for this event.
  Int_t numHits = DecodeToHitList(evData);

  // Process each hit and fill variables.
  Int_t iHit = 0;
  while (iHit < numHits) {
    THcTrigRawHit* hit = dynamic_cast<THcTrigRawHit*>(fRawHitList->At(iHit));

    if (hit->fPlane == 1) fAdcVals[hit->fCounter-1] = hit->GetData(0);
    else if (hit->fPlane == 2) fTdcVals[hit->fCounter-1] = hit->GetData(1);
    else {
      throw std::out_of_range(
        "`THcTrigDet::Decode`: only planes `1` and `2` available!"
      );
    }

    ++iHit;
  }

  return 0;
}


void THcTrigDet::Setup(const char* name, const char* description) {
  // Prefix for parameters in `param` file.
  string kwPrefix = string(GetApparatus()->GetName()) + "_" + name;
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
}


Int_t THcTrigDet::ReadDatabase(const TDatime& date) {
  std::string adcNames, tdcNames;

  DBRequest list[] = {
    {"_numAdc", &fNumAdc, kInt},  // Number of ADC channels.
    {"_numTdc", &fNumTdc, kInt},  // Number of TDC channels.
    {"_adcNames", &adcNames, kString},  // Names of ADC channels.
    {"_tdcNames", &tdcNames, kString},  // Names of TDC channels.
    {0}
  };
  gHcParms->LoadParmValues(list, fKwPrefix.c_str());

  // Split the names to std::vector<std::string>.
  fAdcNames = vsplit(adcNames);
  fTdcNames = vsplit(tdcNames);

  return kOK;
}


Int_t THcTrigDet::DefineVariables(THaAnalysisObject::EMode mode) {
  if (mode == kDefine && fIsSetup) return kOK;
  fIsSetup = (mode == kDefine);

  std::vector<RVarDef> vars;

  // Push the variable names for ADC channels.
  std::vector<std::string> varNamesAdc(fNumAdc);
  std::vector<std::string> varTitlesAdc(fNumAdc);
  for (int i=0; i<fNumAdc; ++i) {
    varNamesAdc.at(i) = "fAdcVals[" + to_string(i) + "]";
    varTitlesAdc.at(i) = fAdcNames.at(i) + "_adc";
    vars.push_back({
      varTitlesAdc.at(i).c_str(),
      varTitlesAdc.at(i).c_str(),
      varNamesAdc.at(i).c_str()
    });
  }
  // Push the variable names for TDC channels.
  std::vector<std::string> varNamesTdc(fNumTdc);
  std::vector<std::string> varTitlesTdc(fNumTdc);
  for (int i=0; i<fNumTdc; ++i) {
    varNamesTdc.at(i) = "fTdcVals[" + to_string(i) + "]";
    varTitlesTdc.at(i) = fTdcNames.at(i) + "_tdc";
    vars.push_back({
      varTitlesTdc.at(i).c_str(),
      varTitlesTdc.at(i).c_str(),
      varNamesTdc.at(i).c_str()
    });
  }

  vars.push_back({0});

  return DefineVarsFromList(vars.data(), mode);
}


ClassImp(THcTrigDet)
