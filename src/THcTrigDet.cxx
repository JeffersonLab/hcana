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
{
  // Guess at spectrometer name that this trigger detector is associated with
  // Can override with SetSpectName
  fSpectName = name[0];
}


THcTrigDet::~THcTrigDet() {
  delete [] fAdcTimeWindowMin; fAdcTimeWindowMin = NULL;
  delete [] fAdcTimeWindowMax; fAdcTimeWindowMax = NULL;
  delete [] fTdcTimeWindowMin; fTdcTimeWindowMin = NULL;
  delete [] fTdcTimeWindowMax; fTdcTimeWindowMax = NULL;

}


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
  InitHitList(fDetMap, "THcTrigRawHit", 200,fTDC_RefTimeCut,fADC_RefTimeCut);
  CreateMissReportParms(fKwPrefix.c_str());

  fPresentP = 0;
  THaVar* vpresent = gHaVars->Find(Form("%s.present",fSpectName.Data()));
  if(vpresent) {
    fPresentP = (Bool_t *) vpresent->GetValuePointer();
  }

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

//Added function to SET coincidence trigger times
void THcTrigDet::SetCoinTrigTimes() 
{ 
  pTrig1_ROC1 = fTdcTimeRaw[fidx0];
  pTrig4_ROC1 = fTdcTimeRaw[fidx1];
  pTrig1_ROC2 = fTdcTimeRaw[fidx2];
  pTrig4_ROC2 = fTdcTimeRaw[fidx3];

}


Int_t THcTrigDet::Decode(const THaEvData& evData) {
    
  // Decode raw data for this event.
  Bool_t present = kTRUE;	// Don't suppress reference time warnings
  if(HaveIgnoreList()) {
    if(IsIgnoreType(evData.GetEvType())) {
      present = kFALSE;
    }
  } else if(fPresentP) {		// if this spectrometer not part of trigger
    present = *fPresentP;
  }
  Int_t numHits = DecodeToHitList(evData, !present);

  // Process each hit and fill variables.
  Int_t iHit = 0;
  while (iHit < numHits) {
    THcTrigRawHit* hit = dynamic_cast<THcTrigRawHit*>(fRawHitList->At(iHit));

    Int_t cnt = hit->fCounter-1;
    if (hit->fPlane == 1) {
      THcRawAdcHit rawAdcHit = hit->GetRawAdcHit();
      fAdcMultiplicity[cnt] = rawAdcHit.GetNPulses();
      UInt_t good_hit=999;
         if (rawAdcHit.GetNPulses()>1) {
          for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
	    Int_t TestTime=rawAdcHit.GetPulseTime(thit)+fAdcTdcOffset;
	    if (TestTime>fAdcTimeWindowMin[cnt]&&TestTime<fAdcTimeWindowMax[cnt]&&good_hit==999) {
	      good_hit=thit;
	    }
	  }
	 }
       if (good_hit==999) good_hit=0;
       fAdcPedRaw[cnt] = rawAdcHit.GetPedRaw();
       fAdcPulseIntRaw[cnt] = rawAdcHit.GetPulseIntRaw(good_hit);
       fAdcPulseAmpRaw[cnt] = rawAdcHit.GetPulseAmpRaw(good_hit);
       fAdcPulseTimeRaw[cnt] = rawAdcHit.GetPulseTimeRaw(good_hit);
       fAdcPulseTime[cnt] = rawAdcHit.GetPulseTime(good_hit)+fAdcTdcOffset;
       fAdcPed[cnt] = rawAdcHit.GetPed();
       fAdcPulseInt[cnt] = rawAdcHit.GetPulseInt(good_hit);
       fAdcPulseAmp[cnt] = rawAdcHit.GetPulseAmp(good_hit);
    }
    else if (hit->fPlane == 2) {
      THcRawTdcHit rawTdcHit = hit->GetRawTdcHit();

      UInt_t good_hit=999;
         if (rawTdcHit.GetNHits()>1) {
          for (UInt_t thit=0; thit<rawTdcHit.GetNHits(); ++thit) {
	    Int_t TestTime=rawTdcHit.GetTime(thit)*fTdcChanperNS+fTdcOffset;
	    if (TestTime>fTdcTimeWindowMin[cnt]&&TestTime<fTdcTimeWindowMax[cnt]&&good_hit==999) {
	      good_hit=thit;
	    }
	  }
	 }
       if (good_hit==999) good_hit=0;
      fTdcTimeRaw[cnt] = rawTdcHit.GetTimeRaw(good_hit);
      fTdcTime[cnt] = rawTdcHit.GetTime(good_hit)*fTdcChanperNS+fTdcOffset;

      fTdcMultiplicity[cnt] = rawTdcHit.GetNHits();
    }
    else {
      throw std::out_of_range(
        "`THcTrigDet::Decode`: only planes `1` and `2` available!"
      );
    }

    ++iHit;
  }

  //Set raw Tdc coin. trigger times for pTRIG1/4
  SetCoinTrigTimes();

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
    {"_adc_tdc_offset", &fTdcOffset, kDouble,0,1},  // Offset of Adc Pulse time (ns)
    {"_tdcchanperns", &fTdcChanperNS, kDouble,0,1},  // Convert channesl to ns
    {"_trig_tdcrefcut", &fTDC_RefTimeCut, kInt, 0, 1},
    {"_trig_adcrefcut", &fADC_RefTimeCut, kInt, 0, 1},
    {0}
  };
  fTdcChanperNS=0.1;
  fTdcOffset=300.;
  fAdcTdcOffset=200.;
  fTDC_RefTimeCut=-1000.;
  fADC_RefTimeCut=-1000.;
  gHcParms->LoadParmValues(list, fKwPrefix.c_str());
  //
  fAdcTimeWindowMin = new Double_t [fNumAdc];
  fAdcTimeWindowMax = new Double_t [fNumAdc];
  fTdcTimeWindowMin = new Double_t [fNumTdc];
  fTdcTimeWindowMax = new Double_t [fNumTdc];
  for(Int_t ip=0;ip<fNumAdc;ip++) { // Set a large default window
    fAdcTimeWindowMin[ip]=-1000;
    fAdcTimeWindowMax[ip]=1000;
  }
  for(Int_t ip=0;ip<fNumTdc;ip++) { // Set a large default window
    fTdcTimeWindowMin[ip]=-1000;
    fTdcTimeWindowMax[ip]=1000;
  }
  DBRequest list2[]={
    {"_AdcTimeWindowMin", fAdcTimeWindowMin, kDouble,     (UInt_t) fNumAdc, 1},
    {"_AdcTimeWindowMax", fAdcTimeWindowMax, kDouble,     (UInt_t) fNumAdc, 1},
    {"_TdcTimeWindowMin", fTdcTimeWindowMin, kDouble,     (UInt_t) fNumTdc, 1},
    {"_TdcTimeWindowMax", fTdcTimeWindowMax, kDouble,     (UInt_t) fNumTdc, 1},
    {0}
  };

  gHcParms->LoadParmValues(list2, fKwPrefix.c_str());

  // Split the names to std::vector<std::string>.
  fAdcNames = vsplit(adcNames);
  fTdcNames = vsplit(tdcNames);

  //default index values
  fidx0 = 27;
  fidx1 = 30;
  fidx2 = 58;
  fidx3 = 61;	

  //Assign an index to coincidence trigger times strings
  for (int i = 0; i <fNumTdc; i++)
    {
     
      if(fTdcNames.at(i)=="pTRIG1_ROC1")
	{
	  fidx0 = i;
	}
      else if(fTdcNames.at(i)=="pTRIG4_ROC1")
	{
	  fidx1 = i;
	}
      else if(fTdcNames.at(i)=="pTRIG1_ROC2")
	{
	  fidx2 = i;
	}
      else if(fTdcNames.at(i)=="pTRIG4_ROC2")
	{
	  fidx3 = i;
	}

    }
  
  

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
void THcTrigDet::SetSpectName( const char* name)
{
  fSpectName = name;
}

void THcTrigDet::AddEvtType(int evtype) {
  eventtypes.push_back(evtype);
}
  
void THcTrigDet::SetEvtType(int evtype) {
  eventtypes.clear();
  AddEvtType(evtype);
}

Bool_t THcTrigDet::IsIgnoreType(Int_t evtype) const
{
  for (UInt_t i=0; i < eventtypes.size(); i++) {
    if (evtype == eventtypes[i]) return kTRUE;
  }
  return kFALSE; 
}

Bool_t THcTrigDet::HaveIgnoreList() const
{
  return( (eventtypes.size()>0) ? kTRUE : kFALSE);
}
//_____________________________________________________________________________
Int_t THcTrigDet::End(THaRunBase* run)
{
  MissReport(Form("%s.%s", GetApparatus()->GetName(), GetName()));
  return 0;
}

ClassImp(THcTrigDet)
