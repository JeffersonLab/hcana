/**
\class THcTrigDet
\ingroup Detectors

\brief A mock detector to hold trigger related data.

This class behaves as a detector, but it does not correspond to any physical
detector in the hall. Its purpose is to gather all the trigger related data
comming from a specific source, like HMS.

Can hold up to 100 ADC and TDC channels, though the limit can be changed if
needed. It just seemed like a reasonable starting value.

Only outputs the first hit for each channel to the Root tree leaf.

# Defined variables

For ADC channels it defines:
  - raw pedestal: `var_adcPedRaw`
  - raw pulse integral: `var_adcPulseIntRaw`
  - raw pulse amplitude: `var_adcPulseAmpRaw`
  - raw pulse time: `var_adcPulseTimeRaw`
  - single sample pedestal value: `var_adcPed`
  - pedestal subtracted pulse integral: `var_adcPulseInt`
  - pedestal subtracted pulse amplitude: `var_adcPulseAmp`
  - multiplicity: `var_adcMult`

For TDC channels it defines:
  - raw TDC time: `var_tdcTimeRaw`
  - refence time subtracted TDC time: `var_tdcTime`
  - multiplicity: `var_tdcMult`

# Parameter file variables

The names and number of channels is defined in a parameter file. The detector
looks for next variables:
  - `prefix_numAdc = number_of_ADC_channels`
  - `prefix_numTdc = number_of_TDC_channels`
  - `prefix_adcNames = "varName1 varName2 ... varNameNumAdc"`
  - `prefix_tdcNames = "varName1 varName2 ... varNameNumTdc"`

# Map file information

ADC channels must be assigned plane `1` and signal `0` while TDC channels must
be assigned plane `2` and signal `1`.

Each channel within a plane must be assigned a consecutive "bar" number, which
is then used to get the correct variable name from parameter file.

Use only with THcTrigApp class.
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

\param[in] opt Maybe used in base clas... Not sure.
*/

/**
\fn Int_t THcTrigDet::Decode(const THaEvData& evData)

\brief Decodes and processes events.

\param[in] evData Raw data to decode.
*/

//TODO: Check if fNumAdc < fMaxAdcChannels && fNumTdc < fMaxTdcChannels.

#include "THcTrigDet.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "TDatime.h"
#include "TString.h"

#include "THaApparatus.h"
#include "THaEvData.h"

#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcRawAdcHit.h"
#include "THcRawTdcHit.h"
#include "THcTrigApp.h"
#include "THcTrigRawHit.h"


THcTrigDet::THcTrigDet() {}


THcTrigDet::THcTrigDet(
  const char* name, const char* description, THaApparatus* app
) :
  THaDetector(name, description, app), THcHitList(),
  fKwPrefix(""),
  fNumAdc(0), fNumTdc(0), fAdcNames(), fTdcNames(),
  fTdcTimeRaw(), fTdcTime(),
  fAdcPedRaw(), fAdcPulseIntRaw(), fAdcPulseAmpRaw(), fAdcPulseTimeRaw(),
  fAdcPed(), fAdcPulseInt(), fAdcPulseAmp(), fAdcPulseTime(),
  fTdcMultiplicity(), fAdcMultiplicity()
{}


THcTrigDet::~THcTrigDet() {}


THaAnalysisObject::EStatus THcTrigDet::Init(const TDatime& date) {
  // Call `Setup` before everything else.
  Setup(GetName(), GetTitle());

  // Initialize all variables.
  for (int i=0; i<fMaxAdcChannels; ++i) {
    fAdcPedRaw[i] = 0;
    fAdcPulseIntRaw[i] = 0;
    fAdcPulseAmpRaw[i] = 0;
    fAdcPulseTimeRaw[i] = 0;
    fAdcPulseTime[i] = kBig;
    fAdcPed[i] = 0.0;
    fAdcPulseInt[i] = 0.0;
    fAdcPulseAmp[i] = 0.0;
    fAdcMultiplicity[i] = 0;
  };
  for (int i=0; i<fMaxTdcChannels; ++i) {
    fTdcTimeRaw[i] = 0;
    fTdcTime[i] = 0.0;
    fTdcMultiplicity[i] = 0;
  };

  // Call initializer for base class.
  // This also calls `ReadDatabase` and `DefineVariables`.
  EStatus status = THaDetector::Init(date);
  if (status) {
    fStatus = status;
    return fStatus;
  }


  // Fill in detector map.
  string EngineDID = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(EngineDID.begin(), EngineDID.end(), EngineDID.begin(), ::toupper);
  if (gHcDetectorMap->FillMap(fDetMap, EngineDID.c_str()) < 0) {
    static const char* const here = "Init()";
    Error(Here(here), "Error filling detectormap for %s.", EngineDID.c_str());
    return kInitError;
  }
  // Initialize hitlist part of the class.
  // printf(" Init trig det hitlist\n");
  InitHitList(fDetMap, "THcTrigRawHit", 100);

  fStatus = kOK;
  return fStatus;
}


void THcTrigDet::Clear(Option_t* opt) {
  THaAnalysisObject::Clear(opt);

  // Reset all data.
  for (int i=0; i<fNumAdc; ++i) {
    fAdcPedRaw[i] = 0;
    fAdcPulseIntRaw[i] = 0;
    fAdcPulseAmpRaw[i] = 0;
    fAdcPulseTimeRaw[i] = 0;
    fAdcPulseTime[i] = kBig;
    fAdcPed[i] = 0.0;
    fAdcPulseInt[i] = 0.0;
    fAdcPulseAmp[i] = 0.0;
    fAdcMultiplicity[i] = 0;
  };
  for (int i=0; i<fNumTdc; ++i) {
    fTdcTimeRaw[i] = 0;
    fTdcTime[i] = 0.0;
    fTdcMultiplicity[i] = 0;
  };
}


Int_t THcTrigDet::Decode(const THaEvData& evData) {
  // Decode raw data for this event.
  Int_t numHits = DecodeToHitList(evData);

  // Process each hit and fill variables.
  Int_t iHit = 0;
  while (iHit < numHits) {
    THcTrigRawHit* hit = dynamic_cast<THcTrigRawHit*>(fRawHitList->At(iHit));

    Int_t cnt = hit->fCounter-1;
    if (hit->fPlane == 1) {
      THcRawAdcHit rawAdcHit = hit->GetRawAdcHit();

      fAdcPedRaw[cnt] = rawAdcHit.GetPedRaw();
      fAdcPulseIntRaw[cnt] = rawAdcHit.GetPulseIntRaw();
      fAdcPulseAmpRaw[cnt] = rawAdcHit.GetPulseAmpRaw();
      fAdcPulseTimeRaw[cnt] = rawAdcHit.GetPulseTimeRaw();
      fAdcPulseTime[cnt] = rawAdcHit.GetPulseTime();

      fAdcPed[cnt] = rawAdcHit.GetPed();
      fAdcPulseInt[cnt] = rawAdcHit.GetPulseInt();
      fAdcPulseAmp[cnt] = rawAdcHit.GetPulseAmp();

      fAdcMultiplicity[cnt] = rawAdcHit.GetNPulses();
    }
    else if (hit->fPlane == 2) {
      THcRawTdcHit rawTdcHit = hit->GetRawTdcHit();

      fTdcTimeRaw[cnt] = rawTdcHit.GetTimeRaw();
      fTdcTime[cnt] = rawTdcHit.GetTime()*fTdcChanperNS-fTdcOffset;

      fTdcMultiplicity[cnt] = rawTdcHit.GetNHits();
    }
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
    {"_tdcoffset", &fTdcOffset, kDouble,0,1},  // Offset of tdc channels
    {"_tdcchanperns", &fTdcChanperNS, kDouble,0,1},  // Convert channesl to ns
    {0}
  };
  fTdcChanperNS=0.1;
  fTdcOffset=300.;
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

  //Push the variable names for ADC channels.
  std::vector<TString> adcPedRawTitle(fNumAdc), adcPedRawVar(fNumAdc);
  std::vector<TString> adcPulseIntRawTitle(fNumAdc), adcPulseIntRawVar(fNumAdc);
  std::vector<TString> adcPulseAmpRawTitle(fNumAdc), adcPulseAmpRawVar(fNumAdc);
  std::vector<TString> adcPulseTimeRawTitle(fNumAdc), adcPulseTimeRawVar(fNumAdc);
  std::vector<TString> adcPulseTimeTitle(fNumAdc), adcPulseTimeVar(fNumAdc);
  std::vector<TString> adcPedTitle(fNumAdc), adcPedVar(fNumAdc);
  std::vector<TString> adcPulseIntTitle(fNumAdc), adcPulseIntVar(fNumAdc);
  std::vector<TString> adcPulseAmpTitle(fNumAdc), adcPulseAmpVar(fNumAdc);
  std::vector<TString> adcMultiplicityTitle(fNumAdc), adcMultiplicityVar(fNumAdc);

  for (int i=0; i<fNumAdc; ++i) {
    adcPedRawTitle.at(i) = fAdcNames.at(i) + "_adcPedRaw";
    adcPedRawVar.at(i) = TString::Format("fAdcPedRaw[%d]", i);
    RVarDef entry1 {
      adcPedRawTitle.at(i).Data(),
      adcPedRawTitle.at(i).Data(),
      adcPedRawVar.at(i).Data()
    };
    vars.push_back(entry1);

    adcPulseIntRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseIntRaw";
    adcPulseIntRawVar.at(i) = TString::Format("fAdcPulseIntRaw[%d]", i);
    RVarDef entry2 {
      adcPulseIntRawTitle.at(i).Data(),
      adcPulseIntRawTitle.at(i).Data(),
      adcPulseIntRawVar.at(i).Data()
    };
    vars.push_back(entry2);

    adcPulseAmpRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseAmpRaw";
    adcPulseAmpRawVar.at(i) = TString::Format("fAdcPulseAmpRaw[%d]", i);
    RVarDef entry3 {
      adcPulseAmpRawTitle.at(i).Data(),
      adcPulseAmpRawTitle.at(i).Data(),
      adcPulseAmpRawVar.at(i).Data()
    };
    vars.push_back(entry3);

    adcPulseTimeRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseTimeRaw";
    adcPulseTimeRawVar.at(i) = TString::Format("fAdcPulseTimeRaw[%d]", i);
    RVarDef entry4 {
      adcPulseTimeRawTitle.at(i).Data(),
      adcPulseTimeRawTitle.at(i).Data(),
      adcPulseTimeRawVar.at(i).Data()
    };
    vars.push_back(entry4);

    adcPedTitle.at(i) = fAdcNames.at(i) + "_adcPed";
    adcPedVar.at(i) = TString::Format("fAdcPed[%d]", i);
    RVarDef entry5 {
      adcPedTitle.at(i).Data(),
      adcPedTitle.at(i).Data(),
      adcPedVar.at(i).Data()
    };
    vars.push_back(entry5);

    adcPulseIntTitle.at(i) = fAdcNames.at(i) + "_adcPulseInt";
    adcPulseIntVar.at(i) = TString::Format("fAdcPulseInt[%d]", i);
    RVarDef entry6 {
      adcPulseIntTitle.at(i).Data(),
      adcPulseIntTitle.at(i).Data(),
      adcPulseIntVar.at(i).Data()
    };
    vars.push_back(entry6);

    adcPulseAmpTitle.at(i) = fAdcNames.at(i) + "_adcPulseAmp";
    adcPulseAmpVar.at(i) = TString::Format("fAdcPulseAmp[%d]", i);
    RVarDef entry7 {
      adcPulseAmpTitle.at(i).Data(),
      adcPulseAmpTitle.at(i).Data(),
      adcPulseAmpVar.at(i).Data()
    };
    vars.push_back(entry7);

    adcMultiplicityTitle.at(i) = fAdcNames.at(i) + "_adcMultiplicity";
    adcMultiplicityVar.at(i) = TString::Format("fAdcMultiplicity[%d]", i);
    RVarDef entry8 {
      adcMultiplicityTitle.at(i).Data(),
      adcMultiplicityTitle.at(i).Data(),
      adcMultiplicityVar.at(i).Data()
    };
    vars.push_back(entry8);
  
 

    adcPulseTimeTitle.at(i) = fAdcNames.at(i) + "_adcPulseTime";
    adcPulseTimeVar.at(i) = TString::Format("fAdcPulseTime[%d]", i);
    RVarDef entry9 {
      adcPulseTimeTitle.at(i).Data(),
      adcPulseTimeTitle.at(i).Data(),
      adcPulseTimeVar.at(i).Data()
    };
    vars.push_back(entry9);
  } // loop over fNumAdc 
  // Push the variable names for TDC channels.
  std::vector<TString> tdcTimeRawTitle(fNumTdc), tdcTimeRawVar(fNumTdc);
  std::vector<TString> tdcTimeTitle(fNumTdc), tdcTimeVar(fNumTdc);
  std::vector<TString> tdcMultiplicityTitle(fNumTdc), tdcMultiplicityVar(fNumTdc);

  for (int i=0; i<fNumTdc; ++i) {
    tdcTimeRawTitle.at(i) = fTdcNames.at(i) + "_tdcTimeRaw";
    tdcTimeRawVar.at(i) = TString::Format("fTdcTimeRaw[%d]", i);
    RVarDef entry1 {
      tdcTimeRawTitle.at(i).Data(),
      tdcTimeRawTitle.at(i).Data(),
      tdcTimeRawVar.at(i).Data()
    };
    vars.push_back(entry1);

    tdcTimeTitle.at(i) = fTdcNames.at(i) + "_tdcTime";
    tdcTimeVar.at(i) = TString::Format("fTdcTime[%d]", i);
    RVarDef entry2 {
      tdcTimeTitle.at(i).Data(),
      tdcTimeTitle.at(i).Data(),
      tdcTimeVar.at(i).Data()
    };
    vars.push_back(entry2);

    tdcMultiplicityTitle.at(i) = fTdcNames.at(i) + "_tdcMultiplicity";
    tdcMultiplicityVar.at(i) = TString::Format("fTdcMultiplicity[%d]", i);
    RVarDef entry3 {
      tdcMultiplicityTitle.at(i).Data(),
      tdcMultiplicityTitle.at(i).Data(),
      tdcMultiplicityVar.at(i).Data()
    };
    vars.push_back(entry3);
  }

  RVarDef end {0};
  vars.push_back(end);

  return DefineVarsFromList(vars.data(), mode);
}


ClassImp(THcTrigDet)
