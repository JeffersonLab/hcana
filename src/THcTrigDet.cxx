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

//_____________________________________________________________________________
THcTrigDet::THcTrigDet() {}

//_____________________________________________________________________________
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

//_____________________________________________________________________________
THcTrigDet::~THcTrigDet() {
  delete [] fAdcTimeWindowMin; fAdcTimeWindowMin = NULL;
  delete [] fAdcTimeWindowMax; fAdcTimeWindowMax = NULL;
  delete [] fTdcTimeWindowMin; fTdcTimeWindowMin = NULL;
  delete [] fTdcTimeWindowMax; fTdcTimeWindowMax = NULL;

}

//_____________________________________________________________________________
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
   DBRequest listextra[]={
    {"_trig_tdcrefcut", &fTDC_RefTimeCut, kInt, 0, 1},
    {"_trig_adcrefcut", &fADC_RefTimeCut, kInt, 0, 1},
    {0}
  };
  fTDC_RefTimeCut = -1000;		// Minimum allowed reference times
  fADC_RefTimeCut = -1000;
  gHcParms->LoadParmValues((DBRequest*)&listextra,fKwPrefix.c_str());
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

//_____________________________________________________________________________
void THcTrigDet::Clear(Option_t* opt) {
  THaAnalysisObject::Clear(opt);

  // Reset all data.
  fTdcRefTime = kBig;
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
    fAdcSampPedRaw[i] = 0;
    fAdcSampPulseIntRaw[i] = 0;
    fAdcSampPulseAmpRaw[i] = 0;
    fAdcSampPulseTimeRaw[i] = 0;
    fAdcSampPulseTime[i] = kBig;
    fAdcSampPed[i] = 0.0;
    fAdcSampPulseInt[i] = 0.0;
    fAdcSampPulseAmp[i] = 0.0;
    fAdcSampMultiplicity[i] = 0;
  };
  for (int i=0; i<fNumTdc; ++i) {
    fTdcTimeRaw[i] = 0;
    fTdcTime[i] = 0.0;
    fTdcMultiplicity[i] = 0;
  };
       fSampWaveform.clear();
}

//_____________________________________________________________________________
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
      THcRawAdcHit& rawAdcHit = hit->GetRawAdcHit();
      fAdcMultiplicity[cnt] = rawAdcHit.GetNPulses();
      UInt_t good_hit=999;
          for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
	    Int_t TestTime=rawAdcHit.GetPulseTimeRaw(thit);
	    if (TestTime>=fAdcTimeWindowMin[cnt]&&TestTime<=fAdcTimeWindowMax[cnt]&&good_hit==999) {
	      good_hit=thit;
	    }
	  }
	 if (good_hit<rawAdcHit.GetNPulses()) {
       fAdcPedRaw[cnt] = rawAdcHit.GetPedRaw();
       fAdcPulseIntRaw[cnt] = rawAdcHit.GetPulseIntRaw(good_hit);
       fAdcPulseAmpRaw[cnt] = rawAdcHit.GetPulseAmpRaw(good_hit);
       fAdcPulseTimeRaw[cnt] = rawAdcHit.GetPulseTimeRaw(good_hit);
       fAdcPulseTime[cnt] = rawAdcHit.GetPulseTime(good_hit)+fAdcTdcOffset;
       fAdcPed[cnt] = rawAdcHit.GetPed();
       fAdcPulseInt[cnt] = rawAdcHit.GetPulseInt(good_hit);
       fAdcPulseAmp[cnt] = rawAdcHit.GetPulseAmp(good_hit);
	 }
    if (rawAdcHit.GetNSamples() >0 ) {   
      rawAdcHit.SetSampThreshold(fSampThreshold);
      if (fSampNSA == 0) fSampNSA=rawAdcHit.GetF250_NSA();
      if (fSampNSB == 0) fSampNSB=rawAdcHit.GetF250_NSB();
      rawAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
      if (fSampNSAT != 2) rawAdcHit.SetSampNSAT(fSampNSAT);
       rawAdcHit.SetSampIntTimePedestalPeak();
     if (fOutputSampWaveform==1) {
       fSampWaveform.push_back(float(cnt));
       fSampWaveform.push_back(float(rawAdcHit.GetNSamples()));
       for (UInt_t thit = 0; thit < rawAdcHit.GetNSamples(); thit++) {
	fSampWaveform.push_back(rawAdcHit.GetSample(thit)); // ped subtracted sample (mV)
       }
      }
      fAdcSampMultiplicity[cnt] = rawAdcHit.GetNSampPulses();
       UInt_t sampgood_hit=999;
          for (UInt_t thit=0; thit<rawAdcHit.GetNSampPulses(); ++thit) {
	    Int_t TestTime=rawAdcHit.GetSampPulseTimeRaw(thit);
	    if (TestTime>=fAdcTimeWindowMin[cnt]&&TestTime<=fAdcTimeWindowMax[cnt]&&sampgood_hit==999) {
	      sampgood_hit=thit;
	    }
	  }
	 if (sampgood_hit<rawAdcHit.GetNSampPulses()) {
           fAdcSampPedRaw[cnt] = rawAdcHit.GetSampPedRaw();
           fAdcSampPulseIntRaw[cnt] = rawAdcHit.GetSampPulseIntRaw(sampgood_hit);
           fAdcSampPulseAmpRaw[cnt] = rawAdcHit.GetSampPulseAmpRaw(sampgood_hit);
           fAdcSampPulseTimeRaw[cnt] = rawAdcHit.GetSampPulseTimeRaw(sampgood_hit);
           fAdcSampPulseTime[cnt] = rawAdcHit.GetSampPulseTime(sampgood_hit)+fAdcTdcOffset;
           fAdcSampPed[cnt] = rawAdcHit.GetSampPed();
           fAdcSampPulseInt[cnt] = rawAdcHit.GetSampPulseInt(sampgood_hit);
           fAdcSampPulseAmp[cnt] = rawAdcHit.GetSampPulseAmp(sampgood_hit);
           if ( rawAdcHit.GetNPulses() ==0 || fUseSampWaveform ==1 ) {
             fAdcPedRaw[cnt] = rawAdcHit.GetSampPedRaw();
             fAdcPulseIntRaw[cnt] = rawAdcHit.GetSampPulseIntRaw(sampgood_hit);
             fAdcPulseAmpRaw[cnt] = rawAdcHit.GetSampPulseAmpRaw(sampgood_hit);
             fAdcPulseTimeRaw[cnt] = rawAdcHit.GetSampPulseTimeRaw(sampgood_hit);
             fAdcPulseTime[cnt] = rawAdcHit.GetSampPulseTime(sampgood_hit)+fAdcTdcOffset;
             fAdcPed[cnt] = rawAdcHit.GetSampPed();
             fAdcPulseInt[cnt] = rawAdcHit.GetSampPulseInt(sampgood_hit);
             fAdcPulseAmp[cnt] = rawAdcHit.GetSampPulseAmp(sampgood_hit);
	   }
 	 }
    }	 
    }
    else if (hit->fPlane == 2) {
      THcRawTdcHit& rawTdcHit = hit->GetRawTdcHit();
    if (rawTdcHit.GetNHits() >0 && rawTdcHit.HasRefTime() && fTdcRefTime == kBig) fTdcRefTime=rawTdcHit.GetRefTime() ;
      UInt_t good_hit=999;
      UInt_t closest_hit=999;
      Int_t TimeDiff=1000000;
           for (UInt_t thit=0; thit<rawTdcHit.GetNHits(); ++thit) {
	    Int_t TestTime= rawTdcHit.GetTimeRaw(thit);
	    if (abs(TestTime-fTdcTimeWindowMin[cnt]) < TimeDiff) {
	      closest_hit=thit;
	      TimeDiff=abs(TestTime-fTdcTimeWindowMin[cnt]);
	    }
	    if (TestTime>=fTdcTimeWindowMin[cnt]&&TestTime<=fTdcTimeWindowMax[cnt]&&good_hit==999) {
	      good_hit=thit;
	    }
	   }
	   if (good_hit == 999 and closest_hit != 999) good_hit=closest_hit;
	 if (good_hit<rawTdcHit.GetNHits()) {
      fTdcTimeRaw[cnt] = rawTdcHit.GetTimeRaw(good_hit);
      fTdcTime[cnt] = rawTdcHit.GetTime(good_hit)*fTdcChanperNS+fTdcOffset;
	 }
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

//_____________________________________________________________________________
void THcTrigDet::Setup(const char* name, const char* description) {
  // Prefix for parameters in `param` file.
  string kwPrefix = string(GetApparatus()->GetName()) + "_" + name;
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
}

//_____________________________________________________________________________
Int_t THcTrigDet::ReadDatabase(const TDatime& date) {
  std::string adcNames, tdcNames;
  
  std::string trigNames="pTRIG1_ROC1 pTRIG4_ROC1 pTRIG1_ROC2 pTRIG4_ROC2";
  
  //C.Y. Sep 08, 2021 | changed pTRIG4 to pTRIG3 (Since in hardware, pTRIG3 -> h3/4 trigger)
  // SJDK - 22/02/22 - Switched back, keep names here as is, can set in param files later
  //std::string trigNames="pTRIG1_ROC1 pTRIG3_ROC1 pTRIG1_ROC2 pTRIG3_ROC2";

  // SJDK 12/04/21 - Added new RF names for use in getter
  std::string RFNames="pRF hRF";
  DBRequest list[] = {
    {"_numAdc", &fNumAdc, kInt},  // Number of ADC channels.
    {"_numTdc", &fNumTdc, kInt},  // Number of TDC channels.
    {"_adcNames", &adcNames, kString},  // Names of ADC channels.
    {"_tdcNames", &tdcNames, kString},  // Names of TDC channels.
    {"_trigNames", &trigNames, kString,0,1},  // Names of Triggers for coincidence time.
    {"_RFNames", &RFNames, kString,0, 1}, // SJDK 12/04/21 -  Names for RF time
    {"_tdcoffset", &fTdcOffset, kDouble,0,1},  // Offset of tdc channels
    {"_adc_tdc_offset", &fAdcTdcOffset, kDouble,0,1},  // Offset of Adc Pulse time (ns)
    {"_tdcchanperns", &fTdcChanperNS, kDouble,0,1},  // Convert channesl to ns
    {0}
  };
  fTdcChanperNS=0.09766;
  fTdcOffset=300.;
  fAdcTdcOffset=200.;
  gHcParms->LoadParmValues(list, fKwPrefix.c_str());
  
  fAdcTimeWindowMin = new Double_t [fNumAdc];
  fAdcTimeWindowMax = new Double_t [fNumAdc];
  fTdcTimeWindowMin = new Double_t [fNumTdc];
  fTdcTimeWindowMax = new Double_t [fNumTdc];
  for(Int_t ip=0;ip<fNumAdc;ip++) { // Set a large default window
    fAdcTimeWindowMin[ip]=0;
    fAdcTimeWindowMax[ip]=100000;
  }
  for(Int_t ip=0;ip<fNumTdc;ip++) { // Set a large default window
    fTdcTimeWindowMin[ip]=0;
    fTdcTimeWindowMax[ip]=100000;
  }
  DBRequest list2[]={
    {"_SampThreshold",     &fSampThreshold,       kDouble,0,1},
    {"_SampNSA",     &fSampNSA,       kInt,0,1},
    {"_SampNSAT",     &fSampNSAT,       kInt,0,1},
    {"_SampNSB",     &fSampNSB,       kInt,0,1},
    {"_OutputSampWaveform",     &fOutputSampWaveform,       kInt,0,1},
    {"_UseSampWaveform",     &fUseSampWaveform,       kInt,0,1},
    {"_AdcTimeWindowMin", fAdcTimeWindowMin, kDouble,     (UInt_t) fNumAdc, 1},
    {"_AdcTimeWindowMax", fAdcTimeWindowMax, kDouble,     (UInt_t) fNumAdc, 1},
    {"_TdcTimeWindowMin", fTdcTimeWindowMin, kDouble,     (UInt_t) fNumTdc, 1},
    {"_TdcTimeWindowMax", fTdcTimeWindowMax, kDouble,     (UInt_t) fNumTdc, 1},
    {0}
  };

  fSampThreshold = 5.;
  fSampNSA = 0; // use value stored in event 125 info
  fSampNSB = 0; // use value stored in event 125 info
  fSampNSAT = 2; // default value in THcRawHit::SetF250Params
  fOutputSampWaveform = 0; // 0= no output , 1 = output Sample Waveform
  fUseSampWaveform = 0; // 0= do not use , 1 = use Sample Waveform

  gHcParms->LoadParmValues(list2, fKwPrefix.c_str());
  for(Int_t ip=0;ip<fNumTdc;ip++) { 
    //    cout << ip << " " << fTdcNames.at(ip) << " " << fTdcTimeWindowMin[ip] << " " << fTdcTimeWindowMax[ip] << endl;
  }
  // Split the names to std::vector<std::string>.
  fAdcNames = Podd::vsplit(adcNames);
  fTdcNames = Podd::vsplit(tdcNames);
  fTrigNames = Podd::vsplit(trigNames);
  fRFNames = Podd::vsplit(RFNames); // SJDK 12/04/21 - For RF getter
  //default index values
 
  //Assign an index to coincidence trigger times strings
  for (UInt_t j = 0; j <fTrigNames.size(); j++) {
    fTrigId[j]=-1;
  }
  for (int i = 0; i <fNumTdc; i++) {
    for (UInt_t j = 0; j <fTrigNames.size(); j++) {
      if(fTdcNames.at(i)==fTrigNames[j]) fTrigId[j]=i;
    }
  }
 
  cout << " Trig = " << fTrigNames.size() << endl;
  for (UInt_t j = 0; j <fTrigNames.size(); j++) {
    cout << fTrigNames[j] << " " << fTrigId[j] << endl;

  }
 
  // SJDK - 12/04/21 - For RF getter
  // Assign an index to RF times strings
  for (UInt_t j = 0; j <fRFNames.size(); j++) {
    fRFId[j]=-1;
  }
  for (int i = 0; i <fNumTdc; i++) {
    for (UInt_t j = 0; j <fRFNames.size(); j++) {
      if(fTdcNames.at(i)==fRFNames[j]) fRFId[j]=i;
    }
  } 
  for (UInt_t j = 0; j <fRFNames.size(); j++) {
    cout << fRFNames[j] << " " << fRFId[j] << endl;
  }

  return kOK;
}

//_____________________________________________________________________________
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

  std::vector<TString> adcSampPedRawTitle(fNumAdc), adcSampPedRawVar(fNumAdc);
  std::vector<TString> adcSampPulseIntRawTitle(fNumAdc), adcSampPulseIntRawVar(fNumAdc);
  std::vector<TString> adcSampPulseAmpRawTitle(fNumAdc), adcSampPulseAmpRawVar(fNumAdc);
  std::vector<TString> adcSampPulseTimeRawTitle(fNumAdc), adcSampPulseTimeRawVar(fNumAdc);
  std::vector<TString> adcSampPulseTimeTitle(fNumAdc), adcSampPulseTimeVar(fNumAdc);
  std::vector<TString> adcSampPedTitle(fNumAdc), adcSampPedVar(fNumAdc);
  std::vector<TString> adcSampPulseIntTitle(fNumAdc), adcSampPulseIntVar(fNumAdc);
  std::vector<TString> adcSampPulseAmpTitle(fNumAdc), adcSampPulseAmpVar(fNumAdc);
  std::vector<TString> adcSampMultiplicityTitle(fNumAdc), adcSampMultiplicityVar(fNumAdc);
  
  TString RefTimeTitle= "TdcRefTime";
   TString RefTimeVar= "fTdcRefTime";
   RVarDef entryRefTime {
      RefTimeTitle.Data(),
      RefTimeTitle.Data(),
      RefTimeVar.Data()
    };
     vars.push_back(entryRefTime);
 
  for (int i=0; i<fNumAdc; ++i) {
    adcPedRawTitle.at(i) = fAdcNames.at(i) + "_adcPedRaw";
    adcPedRawVar.at(i) = TString::Format("fAdcPedRaw[%d]", i);
    RVarDef entry1 {
      adcPedRawTitle.at(i).Data(),
      adcPedRawTitle.at(i).Data(),
      adcPedRawVar.at(i).Data()
    };
    vars.push_back(entry1);

    adcSampPedRawTitle.at(i) = fAdcNames.at(i) + "_adcSampPedRaw";
    adcSampPedRawVar.at(i) = TString::Format("fAdcSampPedRaw[%d]", i);
    RVarDef entry11 {
      adcSampPedRawTitle.at(i).Data(),
      adcSampPedRawTitle.at(i).Data(),
      adcSampPedRawVar.at(i).Data()
    };
    vars.push_back(entry11);

    adcPulseIntRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseIntRaw";
    adcPulseIntRawVar.at(i) = TString::Format("fAdcPulseIntRaw[%d]", i);
    RVarDef entry2 {
      adcPulseIntRawTitle.at(i).Data(),
      adcPulseIntRawTitle.at(i).Data(),
      adcPulseIntRawVar.at(i).Data()
    };
    vars.push_back(entry2);


    adcSampPulseIntRawTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseIntRaw";
    adcSampPulseIntRawVar.at(i) = TString::Format("fAdcSampPulseIntRaw[%d]", i);
    RVarDef entry22 {
      adcSampPulseIntRawTitle.at(i).Data(),
      adcSampPulseIntRawTitle.at(i).Data(),
      adcSampPulseIntRawVar.at(i).Data()
    };
    vars.push_back(entry22);

    adcPulseAmpRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseAmpRaw";
    adcPulseAmpRawVar.at(i) = TString::Format("fAdcPulseAmpRaw[%d]", i);
    RVarDef entry3 {
      adcPulseAmpRawTitle.at(i).Data(),
      adcPulseAmpRawTitle.at(i).Data(),
      adcPulseAmpRawVar.at(i).Data()
    };
    vars.push_back(entry3);


    adcSampPulseAmpRawTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseAmpRaw";
    adcSampPulseAmpRawVar.at(i) = TString::Format("fAdcSampPulseAmpRaw[%d]", i);
    RVarDef entry33 {
      adcSampPulseAmpRawTitle.at(i).Data(),
      adcSampPulseAmpRawTitle.at(i).Data(),
      adcSampPulseAmpRawVar.at(i).Data()
    };
    vars.push_back(entry33);

    adcPulseTimeRawTitle.at(i) = fAdcNames.at(i) + "_adcPulseTimeRaw";
    adcPulseTimeRawVar.at(i) = TString::Format("fAdcPulseTimeRaw[%d]", i);
    RVarDef entry4 {
      adcPulseTimeRawTitle.at(i).Data(),
      adcPulseTimeRawTitle.at(i).Data(),
      adcPulseTimeRawVar.at(i).Data()
    };
    vars.push_back(entry4);

    adcSampPulseTimeRawTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseTimeRaw";
    adcSampPulseTimeRawVar.at(i) = TString::Format("fAdcSampPulseTimeRaw[%d]", i);
    RVarDef entry44 {
      adcSampPulseTimeRawTitle.at(i).Data(),
      adcSampPulseTimeRawTitle.at(i).Data(),
      adcSampPulseTimeRawVar.at(i).Data()
    };
    vars.push_back(entry44);

    adcPedTitle.at(i) = fAdcNames.at(i) + "_adcPed";
    adcPedVar.at(i) = TString::Format("fAdcPed[%d]", i);
    RVarDef entry5 {
      adcPedTitle.at(i).Data(),
      adcPedTitle.at(i).Data(),
      adcPedVar.at(i).Data()
    };
    vars.push_back(entry5);


    adcSampPedTitle.at(i) = fAdcNames.at(i) + "_adcSampPed";
    adcSampPedVar.at(i) = TString::Format("fAdcSampPed[%d]", i);
    RVarDef entry55 {
      adcSampPedTitle.at(i).Data(),
      adcSampPedTitle.at(i).Data(),
      adcSampPedVar.at(i).Data()
    };
    vars.push_back(entry55);

    adcPulseIntTitle.at(i) = fAdcNames.at(i) + "_adcPulseInt";
    adcPulseIntVar.at(i) = TString::Format("fAdcPulseInt[%d]", i);
    RVarDef entry6 {
      adcPulseIntTitle.at(i).Data(),
      adcPulseIntTitle.at(i).Data(),
      adcPulseIntVar.at(i).Data()
    };
    vars.push_back(entry6);


    adcSampPulseIntTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseInt";
    adcSampPulseIntVar.at(i) = TString::Format("fAdcSampPulseInt[%d]", i);
    RVarDef entry66 {
      adcSampPulseIntTitle.at(i).Data(),
      adcSampPulseIntTitle.at(i).Data(),
      adcSampPulseIntVar.at(i).Data()
    };
    vars.push_back(entry66);

    adcPulseAmpTitle.at(i) = fAdcNames.at(i) + "_adcPulseAmp";
    adcPulseAmpVar.at(i) = TString::Format("fAdcPulseAmp[%d]", i);
    RVarDef entry7 {
      adcPulseAmpTitle.at(i).Data(),
      adcPulseAmpTitle.at(i).Data(),
      adcPulseAmpVar.at(i).Data()
    };
    vars.push_back(entry7);


    adcSampPulseAmpTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseAmp";
    adcSampPulseAmpVar.at(i) = TString::Format("fAdcSampPulseAmp[%d]", i);
    RVarDef entry77 {
      adcSampPulseAmpTitle.at(i).Data(),
      adcSampPulseAmpTitle.at(i).Data(),
      adcSampPulseAmpVar.at(i).Data()
    };
    vars.push_back(entry77);

    adcMultiplicityTitle.at(i) = fAdcNames.at(i) + "_adcMultiplicity";
    adcMultiplicityVar.at(i) = TString::Format("fAdcMultiplicity[%d]", i);
    RVarDef entry8 {
      adcMultiplicityTitle.at(i).Data(),
      adcMultiplicityTitle.at(i).Data(),
      adcMultiplicityVar.at(i).Data()
    };
    vars.push_back(entry8);
  

    adcSampMultiplicityTitle.at(i) = fAdcNames.at(i) + "_adcSampMultiplicity";
    adcSampMultiplicityVar.at(i) = TString::Format("fAdcSampMultiplicity[%d]", i);
    RVarDef entry88 {
      adcSampMultiplicityTitle.at(i).Data(),
      adcSampMultiplicityTitle.at(i).Data(),
      adcSampMultiplicityVar.at(i).Data()
    };
    vars.push_back(entry88);
  
    adcPulseTimeTitle.at(i) = fAdcNames.at(i) + "_adcPulseTime";
    adcPulseTimeVar.at(i) = TString::Format("fAdcPulseTime[%d]", i);
    RVarDef entry9 {
      adcPulseTimeTitle.at(i).Data(),
      adcPulseTimeTitle.at(i).Data(),
      adcPulseTimeVar.at(i).Data()
    };
    vars.push_back(entry9);
  
    adcSampPulseTimeTitle.at(i) = fAdcNames.at(i) + "_adcSampPulseTime";
    adcSampPulseTimeVar.at(i) = TString::Format("fAdcSampPulseTime[%d]", i);
    RVarDef entry99 {
      adcSampPulseTimeTitle.at(i).Data(),
      adcSampPulseTimeTitle.at(i).Data(),
      adcSampPulseTimeVar.at(i).Data()
    };
    vars.push_back(entry99);

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

  if (fOutputSampWaveform==1) {
  RVarDef vars2[] = {
    {"adcSampWaveform",          "FADC Sample Waveform",           "fSampWaveform"},
      { 0 }
    };
    DefineVarsFromList( vars2, mode);
  }

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

//_____________________________________________________________________________
Bool_t THcTrigDet::IsIgnoreType(Int_t evtype) const
{
  for (UInt_t i=0; i < eventtypes.size(); i++) {
    if (evtype == eventtypes[i]) return kTRUE;
  }
  return kFALSE; 
}

//_____________________________________________________________________________
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
