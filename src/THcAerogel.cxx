/** \class THcAerogel
    \ingroup Detectors

    Class for an Aerogel detector consisting of pairs of PMT's
    attached to a diffuser box
    Will have a fixed number of pairs, but need to later make this
    configurable.
*/

#include "THcAerogel.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THaApparatus.h"
#include "VarDef.h"
#include "VarType.h" 
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include "THaTrackProj.h"
#include "THcRawAdcHit.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcAerogel::THcAerogel( const char* name, const char* description,
                        THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*16);
  fNegTDCHits = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*16);

  fPosADCHits = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  fNegADCHits = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);

  frPosAdcPedRaw       = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPed          = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPulseInt     = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  frPosAdcPulseAmp     = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);

  frNegAdcPedRaw       = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPulseIntRaw  = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPulseTimeRaw = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPed          = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPulseInt     = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  frNegAdcPulseAmp     = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);
  
  fPosAdcErrorFlag = new TClonesArray("THcSignalHit", MaxNumPosAeroPmt*MaxNumAdcPulse);
  fNegAdcErrorFlag = new TClonesArray("THcSignalHit", MaxNumNegAeroPmt*MaxNumAdcPulse);

  fNumPosAdcHits         = vector<Int_t>    (MaxNumPosAeroPmt, 0.0);
  fNumGoodPosAdcHits     = vector<Int_t>    (MaxNumPosAeroPmt, 0.0);
  fNumNegAdcHits         = vector<Int_t>    (MaxNumNegAeroPmt, 0.0);
  fNumGoodNegAdcHits     = vector<Int_t>    (MaxNumNegAeroPmt, 0.0);
  fNumTracksMatched      = vector<Int_t>    (MaxNumPosAeroPmt, 0.0);
  fNumTracksFired        = vector<Int_t>    (MaxNumPosAeroPmt, 0.0);
  fPosNpe                = vector<Double_t> (MaxNumPosAeroPmt, 0.0);
  fNegNpe                = vector<Double_t> (MaxNumPosAeroPmt, 0.0);

  fGoodPosAdcPed         = vector<Double_t> (MaxNumPosAeroPmt, 0.0);
  fGoodPosAdcPulseInt    = vector<Double_t> (MaxNumPosAeroPmt, 0.0);
  fGoodPosAdcPulseIntRaw = vector<Double_t> (MaxNumPosAeroPmt, 0.0);
  fGoodPosAdcPulseAmp    = vector<Double_t> (MaxNumPosAeroPmt, 0.0);
  fGoodPosAdcPulseTime   = vector<Double_t> (MaxNumPosAeroPmt, 0.0);

  fGoodNegAdcPed         = vector<Double_t> (MaxNumNegAeroPmt, 0.0);
  fGoodNegAdcPulseInt    = vector<Double_t> (MaxNumNegAeroPmt, 0.0);
  fGoodNegAdcPulseIntRaw = vector<Double_t> (MaxNumNegAeroPmt, 0.0);
  fGoodNegAdcPulseAmp    = vector<Double_t> (MaxNumNegAeroPmt, 0.0);
  fGoodNegAdcPulseTime   = vector<Double_t> (MaxNumNegAeroPmt, 0.0);

  InitArrays();

}

//_____________________________________________________________________________
THcAerogel::THcAerogel( ) :
  THaNonTrackingDetector()
{
  // Constructor
  fPosTDCHits = NULL;
  fNegTDCHits = NULL;

  fPosADCHits = NULL;
  fNegADCHits = NULL;

  frPosAdcPedRaw       = NULL;
  frPosAdcPulseIntRaw  = NULL;
  frPosAdcPulseAmpRaw  = NULL;
  frPosAdcPulseTimeRaw = NULL;
  frPosAdcPed          = NULL;
  frPosAdcPulseInt     = NULL;
  frPosAdcPulseAmp     = NULL;

  frNegAdcPedRaw       = NULL;
  frNegAdcPulseIntRaw  = NULL;
  frNegAdcPulseAmpRaw  = NULL;
  frNegAdcPulseTimeRaw = NULL;
  frNegAdcPed          = NULL;
  frNegAdcPulseInt     = NULL;
  frNegAdcPulseAmp     = NULL;

  fPosAdcErrorFlag = NULL;
  fNegAdcErrorFlag = NULL;

  InitArrays();

}

//_____________________________________________________________________________
THcAerogel::~THcAerogel()
{
  // Destructor
  delete fPosTDCHits; fPosTDCHits = NULL;
  delete fNegTDCHits; fNegTDCHits = NULL;

  delete fPosADCHits; fPosADCHits = NULL;
  delete fNegADCHits; fNegADCHits = NULL;

  delete frPosAdcPedRaw;       frPosAdcPedRaw       = NULL;
  delete frPosAdcPulseIntRaw;  frPosAdcPulseIntRaw  = NULL;
  delete frPosAdcPulseAmpRaw;  frPosAdcPulseAmpRaw  = NULL;
  delete frPosAdcPulseTimeRaw; frPosAdcPulseTimeRaw = NULL;
  delete frPosAdcPed;          frPosAdcPed          = NULL;
  delete frPosAdcPulseInt;     frPosAdcPulseInt     = NULL;
  delete frPosAdcPulseAmp;     frPosAdcPulseAmp     = NULL;

  delete frNegAdcPedRaw;       frNegAdcPedRaw       = NULL;
  delete frNegAdcPulseIntRaw;  frNegAdcPulseIntRaw  = NULL;
  delete frNegAdcPulseAmpRaw;  frNegAdcPulseAmpRaw  = NULL;
  delete frNegAdcPulseTimeRaw; frNegAdcPulseTimeRaw = NULL;
  delete frNegAdcPed;          frNegAdcPed          = NULL;
  delete frNegAdcPulseInt;     frNegAdcPulseInt     = NULL;
  delete frNegAdcPulseAmp;     frNegAdcPulseAmp     = NULL;

  delete fPosAdcErrorFlag; fPosAdcErrorFlag = NULL;
  delete fNegAdcErrorFlag; fNegAdcErrorFlag = NULL;

  DeleteArrays();

}

//_____________________________________________________________________________
void THcAerogel::InitArrays()
{
  fA_Pos   = NULL;
  fA_Neg   = NULL;
  fA_Pos_p = NULL;
  fA_Neg_p = NULL;
  fT_Pos   = NULL;
  fT_Neg   = NULL;

  fPosGain = NULL;
  fNegGain = NULL;

  fPosPedLimit = NULL;
  fNegPedLimit = NULL;
  fPosPedMean  = NULL;
  fNegPedMean  = NULL;
  fPosPedSum   = NULL;
  fPosPedSum2  = NULL;
  fPosPedCount = NULL;
  fNegPedSum   = NULL;
  fNegPedSum2  = NULL;
  fNegPedCount = NULL;
  fPosPed      = NULL;
  fPosSig      = NULL;
  fPosThresh   = NULL;
  fNegPed      = NULL;
  fNegSig      = NULL;
  fNegThresh   = NULL;
}
//_____________________________________________________________________________
void THcAerogel::DeleteArrays()
{
  delete [] fA_Pos;   fA_Pos   = NULL;
  delete [] fA_Neg;   fA_Neg   = NULL;
  delete [] fA_Pos_p; fA_Pos_p = NULL;
  delete [] fA_Neg_p; fA_Neg_p = NULL;
  delete [] fT_Pos;   fT_Pos   = NULL;
  delete [] fT_Neg;   fT_Neg   = NULL;

  delete [] fPosGain; fPosGain = NULL;
  delete [] fNegGain; fNegGain = NULL;
  
  delete [] fPosPedLimit; fPosPedLimit = NULL;
  delete [] fNegPedLimit; fNegPedLimit = NULL;
  delete [] fPosPedMean;  fPosPedMean  = NULL;
  delete [] fNegPedMean;  fNegPedMean  = NULL;
  delete [] fPosPedSum;   fPosPedSum   = NULL;
  delete [] fPosPedSum2;  fPosPedSum2  = NULL;
  delete [] fPosPedCount; fPosPedCount = NULL;
  delete [] fNegPedSum;   fNegPedSum   = NULL;
  delete [] fNegPedSum2;  fNegPedSum2  = NULL;
  delete [] fNegPedCount; fNegPedCount = NULL;
  delete [] fPosPed;      fPosPed      = NULL;
  delete [] fPosSig;      fPosSig      = NULL;
  delete [] fPosThresh;   fPosThresh   = NULL;
  delete [] fNegPed;      fNegPed      = NULL;
  delete [] fNegSig;      fNegSig      = NULL;
  delete [] fNegThresh;   fNegThresh   = NULL;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcAerogel::Init( const TDatime& date )
{

  cout << "THcAerogel::Init " << GetName() << endl;

  char EngineDID[] = "xAERO";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID );
    return kInitError;
  }

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  InitHitList(fDetMap, "THcAerogelHit", fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  cout << "THcAerogel::ReadDatabase " << GetName() << endl;

  char prefix[2];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  fNelem = 8;			// Default if not defined
  fNRegions = 1;  // Default if not set in parameter file

  Bool_t optional = true ;
  DBRequest listextra[]={
    {"aero_num_pairs", &fNelem, kInt, 0, optional},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&listextra, prefix);

  fPosGain = new Double_t[fNelem];
  fNegGain = new Double_t[fNelem];

  fA_Pos   = new Float_t[fNelem];
  fA_Neg   = new Float_t[fNelem];
  fA_Pos_p = new Float_t[fNelem];
  fA_Neg_p = new Float_t[fNelem];

  // 6 GeV variables
  fTdcOffset   = 0; // Offset to make reference time subtracted times positve
  fPosPedLimit = new Int_t[fNelem];
  fNegPedLimit = new Int_t[fNelem];
  fT_Pos       = new Float_t[fNelem];
  fT_Neg       = new Float_t[fNelem];
  fPosPedMean  = new Double_t[fNelem];
  fNegPedMean  = new Double_t[fNelem];

  // Create arrays to hold pedestal results
  if (fSixGevData) InitializePedestals();

  // Region parameters
  fRegionsValueMax = fNRegions * 8; 
  fRegionValue     = new Double_t[fRegionsValueMax];

  DBRequest list[]={
    {"aero_num_regions",      &fNRegions,         kInt},
    {"aero_red_chi2_min",     &fRedChi2Min,       kDouble},
    {"aero_red_chi2_max",     &fRedChi2Max,       kDouble},
    {"aero_beta_min",         &fBetaMin,          kDouble},
    {"aero_beta_max",         &fBetaMax,          kDouble},
    {"aero_enorm_min",        &fENormMin,         kDouble},
    {"aero_enorm_max",        &fENormMax,         kDouble},
    {"aero_diff_box_zpos",    &fDiffBoxZPos,      kDouble},
    {"aero_npe_thresh",       &fNpeThresh,        kDouble},
    {"aero_adcTimeWindowMin", &fAdcTimeWindowMin, kDouble},
    {"aero_adcTimeWindowMax", &fAdcTimeWindowMax, kDouble},
    {"aero_debug_adc",        &fDebugAdc,         kInt,    0, 1},
    {"aero_six_gev_data",     &fSixGevData,       kInt,    0, 1},
    {"aero_pos_gain",         fPosGain,           kDouble, (UInt_t) fNelem},
    {"aero_neg_gain",         fNegGain,           kDouble, (UInt_t) fNelem},
    {"aero_pos_ped_limit",    fPosPedLimit,       kInt,    (UInt_t) fNelem, optional},
    {"aero_neg_ped_limit",    fNegPedLimit,       kInt,    (UInt_t) fNelem, optional},
    {"aero_pos_ped_mean",     fPosPedMean,        kDouble, (UInt_t) fNelem, optional},
    {"aero_neg_ped_mean",     fNegPedMean,        kDouble, (UInt_t) fNelem, optional},
    {"aero_tdc_offset",       &fTdcOffset,        kInt,    0,               optional},
    {"aero_min_peds",         &fMinPeds,          kInt,    0,               optional},
    {"aero_region",           &fRegionValue[0],   kDouble, (UInt_t) fRegionsValueMax},

    {0}
  };

  fSixGevData = 0; // Set 6 GeV data parameter to false unless set in parameter file
  fDebugAdc   = 0; // Set ADC debug parameter to false unless set in parameter file

  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  if (fSixGevData) cout << "6 GeV Data Analysis Flag Set To TRUE" << endl;

  fIsInit = true;

  cout << " Track Matching Parameters for: " << GetName() << endl;
  for (Int_t iregion = 0; iregion < fNRegions; iregion++) {
    cout << "Region = " << iregion + 1 << endl;
    for (Int_t ivalue = 0; ivalue < 8; ivalue++)
      cout << fRegionValue[GetIndex(iregion, ivalue)] << "  ";
    cout << endl;
  }

  return kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcAerogel::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  // Do we need to put the number of pos/neg TDC/ADC hits into the variables?
  // No.  They show up in tree as Ndata.H.aero.postdchits for example

  vector<RVarDef> vars;

  vars.push_back({"posGain", "Positive PMT gains", "fPosGain"});
  vars.push_back({"negGain", "Negative PMT gains", "fNegGain"});
  vars.push_back({"nGoodHits", "Total number of good hits", "fNGoodHits"});
  vars.push_back({"apos",          "Positive Raw ADC Amplitudes",            "fA_Pos"});
  vars.push_back({"aneg",          "Negative Raw ADC Amplitudes",            "fA_Neg"});
  vars.push_back({"apos_p",        "Positive Ped-subtracted ADC Amplitudes", "fA_Pos_p"});
  vars.push_back({"aneg_p",        "Negative Ped-subtracted ADC Amplitudes", "fA_Neg_p"});

  vars.push_back({"numPosAdcHits",        "Number of Positive ADC Hits Per PMT",      "fNumPosAdcHits"});        // Aerogel occupancy
  vars.push_back({"numGoodPosAdcHits",    "Number of Good Positive ADC Hits Per PMT", "fNumGoodPosAdcHits"});    // Aerogel occupancy
  vars.push_back({"totNumPosAdcHits",     "Total Number of Positive ADC Hits",        "fTotNumPosAdcHits"});     // Aerogel multiplicity
  vars.push_back({"totNumGoodPosAdcHits", "Total Number of Good Positive ADC Hits",   "fTotNumGoodPosAdcHits"}); // Aerogel multiplicity

  vars.push_back({"numNegAdcHits",        "Number of Negative ADC Hits Per PMT",      "fNumNegAdcHits"});        // Aerogel occupancy
  vars.push_back({"numGoodNegAdcHits",    "Number of Good Negative ADC Hits Per PMT", "fNumGoodNegAdcHits"});    // Aerogel occupancy
  vars.push_back({"totNumNegAdcHits",     "Total Number of Negative ADC Hits",        "fTotNumNegAdcHits"});     // Aerogel multiplicity
  vars.push_back({"totNumGoodNegAdcHits", "Total Number of Good Negative ADC Hits",   "fTotNumGoodNegAdcHits"}); // Aerogel multiplicity
  
  vars.push_back({"totnumAdcHits",       "Total Number of ADC Hits Per PMT",          "fTotNumAdcHits"});        // Aerogel multiplicity
  vars.push_back({"totnumGoodAdcHits",   "TotalNumber of Good ADC Hits Per PMT",      "fTotNumGoodAdcHits"});    // Aerogel multiplicity
  vars.push_back({"numTracksMatched",    "Number of Tracks Matched Per Region",       "fNumTracksMatched"});        
  vars.push_back({"numTracksFired",      "Number of Tracks that Fired Per Region",    "fNumTracksFired"});                
  vars.push_back({"totNumTracksMatched", "Total Number of Tracks Matched Per Region", "fTotNumTracksMatched"});
  vars.push_back({"totNumTracksFired",   "Total Number of Tracks that Fired",         "fTotNumTracksFired"});
  
  vars.push_back({"posNpe",    "Number of Positive PEs",       "fPosNpe"});
  vars.push_back({"negNpe",    "Number of Positive PEs",       "fNegNpe"});
  vars.push_back({"posNpeSum", "Total Number of Negative PEs", "fPosNpeSum"});
  vars.push_back({"negNpeSum", "Total Number of Negative PEs", "fNegNpeSum"});
  vars.push_back({"npeSum",    "Total Number of PEs",          "fNpeSum"});

  vars.push_back({"posAdcCounter",   "Positive ADC counter numbers",   "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"});
  vars.push_back({"negAdcCounter",   "Negative ADC counter numbers",   "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"});
  vars.push_back({"posAdcErrorFlag", "Error Flag for When FPGA Fails", "fPosAdcErrorFlag.THcSignalHit.GetData()"});
  vars.push_back({"negAdcErrorFlag", "Error Flag for When FPGA Fails", "fNegAdcErrorFlag.THcSignalHit.GetData()"});

  vars.push_back({"goodPosAdcPed",         "Good Negative ADC pedestals",           "fGoodPosAdcPed"});
  vars.push_back({"goodPosAdcPulseInt",    "Good Negative ADC pulse integrals",     "fGoodPosAdcPulseInt"});
  vars.push_back({"goodPosAdcPulseIntRaw", "Good Negative ADC raw pulse integrals", "fGoodPosAdcPulseIntRaw"});
  vars.push_back({"goodPosAdcPulseAmp",    "Good Negative ADC pulse amplitudes",    "fGoodPosAdcPulseAmp"});
  vars.push_back({"goodPosAdcPulseTime",   "Good Negative ADC pulse times",         "fGoodPosAdcPulseTime"});

  vars.push_back({"goodNegAdcPed",         "Good Negative ADC pedestals",           "fGoodNegAdcPed"});
  vars.push_back({"goodNegAdcPulseInt",    "Good Negative ADC pulse integrals",     "fGoodNegAdcPulseInt"});
  vars.push_back({"goodNegAdcPulseIntRaw", "Good Negative ADC raw pulse integrals", "fGoodNegAdcPulseIntRaw"});
  vars.push_back({"goodNegAdcPulseAmp",    "Good Negative ADC pulse amplitudes",    "fGoodNegAdcPulseAmp"});
  vars.push_back({"goodNegAdcPulseTime",   "Good Negative ADC pulse times",         "fGoodNegAdcPulseTime"});

  if (fDebugAdc) {
    vars.push_back({"posAdcPedRaw",       "Positive Raw ADC pedestals",        "frPosAdcPedRaw.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPulseIntRaw",  "Positive Raw ADC pulse integrals",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPulseAmpRaw",  "Positive Raw ADC pulse amplitudes", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPulseTimeRaw", "Positive Raw ADC pulse times",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPed",          "Positive ADC pedestals",            "frPosAdcPed.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPulseInt",     "Positive ADC pulse integrals",      "frPosAdcPulseInt.THcSignalHit.GetData()"});
    vars.push_back({"posAdcPulseAmp",     "Positive ADC pulse amplitudes",     "frPosAdcPulseAmp.THcSignalHit.GetData()"});

    vars.push_back({"negAdcPedRaw",       "Negative Raw ADC pedestals",        "frNegAdcPedRaw.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPulseIntRaw",  "Negative Raw ADC pulse integrals",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPulseAmpRaw",  "Negative Raw ADC pulse amplitudes", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPulseTimeRaw", "Negative Raw ADC pulse times",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPed",          "Negative ADC pedestals",            "frNegAdcPed.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPulseInt",     "Negative ADC pulse integrals",      "frNegAdcPulseInt.THcSignalHit.GetData()"});
    vars.push_back({"negAdcPulseAmp",     "Negative ADC pulse amplitudes",     "frNegAdcPulseAmp.THcSignalHit.GetData()"});
  }
  
  if (fSixGevData) {
    vars.push_back({"tpos",          "Positive Raw TDC",             "fT_Pos"});
    vars.push_back({"tneg",          "Negative Raw TDC",             "fT_Neg"});
    vars.push_back({"ntdc_pos_hits", "Number of Positive Tube Hits", "fNTDCPosHits"});
    vars.push_back({"ntdc_neg_hits", "Number of Negative Tube Hits", "fNTDCNegHits"});
    vars.push_back({"posadchits",    "Positive ADC hits",            "fPosADCHits.THcSignalHit.GetPaddleNumber()"});
    vars.push_back({"negadchits",    "Negative ADC hits",            "fNegADCHits.THcSignalHit.GetPaddleNumber()"});
    vars.push_back({"postdchits",    "Positive TDC hits",            "fPosTDCHits.THcSignalHit.GetPaddleNumber()"});
    vars.push_back({"negtdchits",    "Negative TDC hits",            "fNegTDCHits.THcSignalHit.GetPaddleNumber()"});
  }
  
  RVarDef end {0};
  vars.push_back(end);

  return DefineVarsFromList(vars.data(), mode);

}
//_____________________________________________________________________________
inline
void THcAerogel::Clear(Option_t* opt)
{
  // Clear the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  fNhits = 0;	    

  fNpeSum    = 0.0;
  fPosNpeSum = 0.0;
  fNegNpeSum = 0.0;

  fTotNumAdcHits        = 0;
  fTotNumGoodAdcHits    = 0;
  fTotNumPosAdcHits     = 0;
  fTotNumNegAdcHits     = 0;
  fTotNumGoodPosAdcHits = 0;
  fTotNumGoodNegAdcHits = 0;
  fTotNumTracksMatched  = 0;
  fTotNumTracksFired    = 0;

  fNGoodHits = 0;

  fNADCPosHits = 0;
  fNADCNegHits = 0;
  fNTDCPosHits = 0;
  fNTDCNegHits = 0;

  for(Int_t itube = 0;itube < fNelem;itube++) {
    fA_Pos[itube] = 0;
    fA_Neg[itube] = 0;
    fA_Pos_p[itube] = 0;
    fA_Neg_p[itube] = 0;
    fT_Pos[itube] = 0;
    fT_Neg[itube] = 0;
  }

  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();
  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();
  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

  fPosAdcErrorFlag->Clear();
  fNegAdcErrorFlag->Clear();

  for (UInt_t ielem = 0; ielem < fNumPosAdcHits.size(); ielem++)
    fNumPosAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumNegAdcHits.size(); ielem++)
    fNumNegAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumGoodPosAdcHits.size(); ielem++)
    fNumGoodPosAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumGoodNegAdcHits.size(); ielem++)
    fNumGoodNegAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumTracksMatched.size(); ielem++)
    fNumTracksMatched.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumTracksFired.size(); ielem++)
    fNumTracksFired.at(ielem) = 0;

  for (UInt_t ielem = 0; ielem < fGoodPosAdcPed.size(); ielem++) {
    fGoodPosAdcPed.at(ielem)         = 0.0;
    fGoodPosAdcPulseInt.at(ielem)    = 0.0;
    fGoodPosAdcPulseIntRaw.at(ielem) = 0.0;
    fGoodPosAdcPulseAmp.at(ielem)    = 0.0;
    fGoodPosAdcPulseTime.at(ielem)   = 0.0;
    fPosNpe.at(ielem)                = 0.0;
  }

  for (UInt_t ielem = 0; ielem < fGoodNegAdcPed.size(); ielem++) {
    fGoodNegAdcPed.at(ielem)         = 0.0;
    fGoodNegAdcPulseInt.at(ielem)    = 0.0;
    fGoodNegAdcPulseIntRaw.at(ielem) = 0.0;
    fGoodNegAdcPulseAmp.at(ielem)    = 0.0;
    fGoodNegAdcPulseTime.at(ielem)   = 0.0;
    fNegNpe.at(ielem)                = 0.0;
  }

}

//_____________________________________________________________________________
Int_t THcAerogel::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  fNhits = DecodeToHitList(evdata);

  if (fSixGevData) {
    if(gHaCuts->Result("Pedestal_event")) {
      AccumulatePedestals(fRawHitList);
      fAnalyzePedestals = 1;	// Analyze pedestals first normal events
      return(0);
    }

    if(fAnalyzePedestals) {
      CalculatePedestals();
      Print("");
      fAnalyzePedestals = 0;	// Don't analyze pedestals next event
    }
  }

  Int_t ihit          = 0;
  UInt_t nrPosAdcHits = 0;
  UInt_t nrNegAdcHits = 0;

  //cout << ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:" << endl;

  while(ihit < fNhits) {

    THcAerogelHit* hit          = (THcAerogelHit*) fRawHitList->At(ihit);
    Int_t          npmt         = hit->fCounter;
    THcRawAdcHit&  rawPosAdcHit = hit->GetRawAdcHitPos();
    THcRawAdcHit&  rawNegAdcHit = hit->GetRawAdcHitNeg();

    for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {

      ((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPedRaw());
      ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPed());

      ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(npmt, rawPosAdcHit.GetPulseTimeRaw(thit));

      if (rawPosAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) fPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(npmt, 0);
      if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) fPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(npmt, 1);

      ++nrPosAdcHits;
      fTotNumAdcHits++;
      fTotNumPosAdcHits++;
      fNumPosAdcHits.at(npmt-1) = npmt;
    }

    for (UInt_t thit=0; thit<rawNegAdcHit.GetNPulses(); ++thit) {
      ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPedRaw());
      ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPed());

      ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(npmt, rawNegAdcHit.GetPulseTimeRaw(thit));

      if (rawNegAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) fNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(npmt, 0);
      if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) fNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(npmt, 1);

      ++nrNegAdcHits;
      fTotNumAdcHits++;
      fTotNumNegAdcHits++;
      fNumNegAdcHits.at(npmt-1) = npmt;
    }
    ihit++;
  }
  return ihit;
}

//_____________________________________________________________________________
Int_t THcAerogel::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcAerogel::CoarseProcess( TClonesArray&  ) //tracks
{

    // Loop over the elements in the TClonesArray
    for(Int_t ielem = 0; ielem < frPosAdcPulseInt->GetEntries(); ielem++) {

      Int_t    npmt         = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
      Double_t pulsePed     = ((THcSignalHit*) frPosAdcPed->ConstructedAt(ielem))->GetData();
      Double_t pulseInt     = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetData();
      Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
      Double_t pulseAmp     = ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(ielem))->GetData();
      Double_t pulseTime    = ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
      Bool_t   errorFlag    = ((THcSignalHit*) fPosAdcErrorFlag->ConstructedAt(ielem))->GetData();  
      Bool_t   pulseTimeCut = pulseTime > fAdcTimeWindowMin && pulseTime < fAdcTimeWindowMax;

      // By default, the last hit within the timing cut will be considered "good"
      if (!errorFlag && pulseTimeCut) {
    	fGoodPosAdcPed.at(npmt)         = pulsePed;
    	fGoodPosAdcPulseInt.at(npmt)    = pulseInt;
    	fGoodPosAdcPulseIntRaw.at(npmt) = pulseIntRaw;
    	fGoodPosAdcPulseAmp.at(npmt)    = pulseAmp;
    	fGoodPosAdcPulseTime.at(npmt)   = pulseTime;
      
    	fPosNpe.at(npmt) = fPosGain[npmt]*fGoodPosAdcPulseInt.at(npmt);
	fPosNpeSum += fPosNpe.at(npmt);
    	fNpeSum += fPosNpeSum;

	fTotNumGoodAdcHits++;
    	fTotNumGoodPosAdcHits++;
    	fNumGoodPosAdcHits.at(npmt) = npmt + 1;
      }
    }

    // Loop over the elements in the TClonesArray
    for(Int_t ielem = 0; ielem < frNegAdcPulseInt->GetEntries(); ielem++) {

      Int_t    npmt         = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
      Double_t pulsePed     = ((THcSignalHit*) frNegAdcPed->ConstructedAt(ielem))->GetData();
      Double_t pulseInt     = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetData();
      Double_t pulseIntRaw  = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
      Double_t pulseAmp     = ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(ielem))->GetData();
      Double_t pulseTime    = ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
      Bool_t   errorFlag    = ((THcSignalHit*) fNegAdcErrorFlag->ConstructedAt(ielem))->GetData();  
      Bool_t   pulseTimeCut = pulseTime > fAdcTimeWindowMin && pulseTime < fAdcTimeWindowMax;

      // By default, the last hit within the timing cut will be considered "good"
      if (!errorFlag && pulseTimeCut) {
    	fGoodNegAdcPed.at(npmt)         = pulsePed;
    	fGoodNegAdcPulseInt.at(npmt)    = pulseInt;
    	fGoodNegAdcPulseIntRaw.at(npmt) = pulseIntRaw;
    	fGoodNegAdcPulseAmp.at(npmt)    = pulseAmp;
    	fGoodNegAdcPulseTime.at(npmt)   = pulseTime;
      
    	fNegNpe.at(npmt) = fNegGain[npmt]*fGoodNegAdcPulseInt.at(npmt);
	fNegNpeSum += fNegNpe.at(npmt);
    	fNpeSum += fNegNpeSum;

	fTotNumGoodAdcHits++;
   	fTotNumGoodNegAdcHits++;
    	fNumGoodNegAdcHits.at(npmt) = npmt + 1;
      }
    }

    for(Int_t ihit=0; ihit < fNhits; ihit++) {

      Int_t nPosTDCHits = 0;
      Int_t nNegTDCHits = 0;
      Int_t nPosADCHits = 0;
      Int_t nNegADCHits = 0;
 
      // 6 GeV calculations
      Int_t adc_pos;
      Int_t adc_neg;
      Int_t tdc_pos = -1;
      Int_t tdc_neg = -1;
      if (fSixGevData) {
	THcAerogelHit* hit = (THcAerogelHit*) fRawHitList->At(ihit);
	Int_t npmt = hit->fCounter - 1;

	// // Sum positive and negative hits to fill tot_good_hits
	// if(fPosNpe.at(npmt) > 0.3) {fNADCPosHits++; fNGoodHits++;}
	// if(fNegNpe.at(npmt) > 0.3) {fNADCNegHits++; fNGoodHits++;}

	// ADC positive hit
	if((adc_pos = hit->GetRawAdcHitPos().GetPulseInt()) > 0) {
	  THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
	  sighit->Set(hit->fCounter, adc_pos);
	}
	// ADC negative hit
	if((adc_neg = hit->GetRawAdcHitNeg().GetPulseInt()) > 0) {
	  THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
	  sighit->Set(hit->fCounter, adc_neg);
	}
	// TDC positive hit
	if(hit->GetRawTdcHitPos().GetNHits() >  0) {
	  THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
	  tdc_pos = hit->GetRawTdcHitPos().GetTime()+fTdcOffset;
	  sighit->Set(hit->fCounter, tdc_pos);
	}
	// TDC negative hit
	if(hit->GetRawTdcHitNeg().GetNHits() >  0) {
	  THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
	  tdc_neg = hit->GetRawTdcHitNeg().GetTime()+fTdcOffset;
	  sighit->Set(hit->fCounter, tdc_neg);
	}    
	// For each TDC, identify the first hit that is positive.
	tdc_pos = -1;
	tdc_neg = -1;
	for(UInt_t thit=0; thit<hit->GetRawTdcHitPos().GetNHits(); thit++) {
	  Int_t tdc = hit->GetRawTdcHitPos().GetTime(thit);
	  if(tdc >=0 ) {
	    tdc_pos = tdc;
	    break;
	  }
	}
	for(UInt_t thit=0; thit<hit->GetRawTdcHitNeg().GetNHits(); thit++) {
	  Int_t tdc = hit->GetRawTdcHitNeg().GetTime(thit);
	  if(tdc >= 0) {
	    tdc_neg = tdc;
	    break;
	  }
	}

	fA_Pos[npmt] = adc_pos;  
	fA_Neg[npmt] = adc_neg;  
	fA_Pos_p[npmt] = fA_Pos[npmt] - fPosPedMean[npmt];
	fA_Neg_p[npmt] = fA_Neg[npmt] - fNegPedMean[npmt];
	fT_Pos[npmt] = tdc_pos;
	fT_Neg[npmt] = tdc_neg;

	if(fA_Pos[npmt] < 8000) fPosNpe[npmt] = fPosGain[npmt]*fA_Pos_p[npmt];
	else fPosNpe[npmt] = 100.0;
      
	if(fA_Neg[npmt] < 8000) fNegNpe[npmt] = fNegGain[npmt]*fA_Neg_p[npmt];
	else fNegNpe[npmt] = 100.0;
      
	fPosNpeSum += fPosNpe[npmt];
	fNegNpeSum += fNegNpe[npmt];

	// Sum positive and negative hits to fill tot_good_hits
	if(fPosNpe[npmt] > 0.3) {fNADCPosHits++; fNGoodHits++;}
	if(fNegNpe[npmt] > 0.3) {fNADCNegHits++; fNGoodHits++;}      

	if(fT_Pos[npmt] > 0 && fT_Pos[npmt] < 8000) fNTDCPosHits++;
	if(fT_Neg[npmt] > 0 && fT_Neg[npmt] < 8000) fNTDCNegHits++;
      }
    }
    return 0;
}

//_____________________________________________________________________________
Int_t THcAerogel::FineProcess( TClonesArray& tracks )
{

  Int_t nTracks = tracks.GetLast() + 1;

  for (Int_t itrack = 0; itrack < nTracks; itrack++) {

    THaTrack* track = dynamic_cast<THaTrack*> (tracks[itrack]);
    if (track->GetIndex() != 0) continue;  // Select the best track

    Double_t trackChi2    = track->GetChi2();
    Int_t    trackNDoF    = track->GetNDoF();
    Double_t trackRedChi2 = trackChi2/trackNDoF;
    Double_t trackBeta    = track->GetBeta();
    Double_t trackEnergy  = track->GetEnergy();
    Double_t trackMom     = track->GetP();
    Double_t trackENorm   = trackEnergy/trackMom;
    Double_t trackXfp     = track->GetX();
    Double_t trackYfp     = track->GetY();
    Double_t trackTheta   = track->GetTheta();
    Double_t trackPhi     = track->GetPhi();

    Bool_t trackRedChi2Cut = trackRedChi2 > fRedChi2Min && trackRedChi2 < fRedChi2Max;
    Bool_t trackBetaCut    = trackBeta    > fBetaMin    && trackBeta    < fBetaMax;
    Bool_t trackENormCut   = trackENorm   > fENormMin   && trackENorm   < fENormMax;

    if (trackRedChi2Cut && trackBetaCut && trackENormCut) {

      // Project the track to the Aerogel diffuser box plane
      Double_t xAtAero = trackXfp + trackTheta * fDiffBoxZPos;
      Double_t yAtAero = trackYfp + trackPhi   * fDiffBoxZPos;

      // cout << "Aerogel Detector: " << GetName() << endl;
      // cout << "nTracks = " << nTracks << "\t" << "trackChi2 = " << trackChi2 
      // 	   << "\t" << "trackNDof = " << trackNDoF << "\t" << "trackRedChi2 = " << trackRedChi2 << endl;
      // cout << "trackBeta = " << trackBeta << "\t" << "trackEnergy = " << trackEnergy << "\t"
      // 	   << "trackMom = " << trackMom << "\t" << "trackENorm = " << trackENorm << endl;
      // cout << "trackXfp = " << trackXfp << "\t" << "trackYfp = " << trackYfp << "\t"
      // 	   << "trackTheta = " << trackTheta << "\t" << "trackPhi = " << trackPhi << endl;
      // cout << "fDiffBoxZPos = " << fDiffBoxZPos << "\t" << "xAtAero = " << xAtAero << "\t" << "yAtAero = " << yAtAero << endl;
      // cout << "=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:" << endl;
      

      for (Int_t iregion = 0; iregion < fNRegions; iregion++) {

      	if ((TMath::Abs(fRegionValue[GetIndex(iregion, 0)] - xAtAero)    < fRegionValue[GetIndex(iregion, 4)]) &&
      	    (TMath::Abs(fRegionValue[GetIndex(iregion, 1)] - yAtAero)    < fRegionValue[GetIndex(iregion, 5)]) &&
      	    (TMath::Abs(fRegionValue[GetIndex(iregion, 2)] - trackTheta) < fRegionValue[GetIndex(iregion, 6)]) &&
      	    (TMath::Abs(fRegionValue[GetIndex(iregion, 3)] - trackPhi)   < fRegionValue[GetIndex(iregion, 7)])) {

	  fTotNumTracksMatched++;
      	  fNumTracksMatched.at(iregion) = iregion + 1;
	  
      	  if (fNpeSum > fNpeThresh) {
      	    fTotNumTracksFired++;
      	    fNumTracksFired.at(iregion) = iregion + 1;
      	  }  // NPE threshold cut
      	}  // Regional cuts
      }  // Loop over regions
    }  // Tracking cuts
  }  // Track loop

  return 0;
}

//_____________________________________________________________________________
// Method for initializing pedestals in the 6 GeV era
void THcAerogel::InitializePedestals()
{
  fNPedestalEvents = 0;
  fMinPeds         = 0;                    // Do not calculate pedestals by default

  fPosPedSum   = new Int_t [fNelem];
  fPosPedSum2  = new Int_t [fNelem];
  fPosPedLimit = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum   = new Int_t [fNelem];
  fNegPedSum2  = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];
  fNegPedCount = new Int_t [fNelem];
  fPosPed      = new Double_t [fNelem];
  fNegPed      = new Double_t [fNelem];
  fPosThresh   = new Double_t [fNelem];
  fNegThresh   = new Double_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i]   = 0;
    fPosPedSum2[i]  = 0;
    fPosPedLimit[i] = 1000;   // In engine, this are set in parameter file
    fPosPedCount[i] = 0;
    fNegPedSum[i]   = 0;
    fNegPedSum2[i]  = 0;
    fNegPedLimit[i] = 1000;   // In engine, this are set in parameter file
    fNegPedCount[i] = 0;
    fPosPedMean[i]  = 0;       // Default pedestal values
    fNegPedMean[i]  = 0;       // Default pedestal values
  }

}

//_____________________________________________________________________________
// Method for accumulating pedestals in the 6 GeV era
void THcAerogel::AccumulatePedestals(TClonesArray* rawhits)
{
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

  Int_t nrawhits = rawhits->GetLast()+1;
  Int_t ihit     = 0;

  while(ihit < nrawhits) {
    THcAerogelHit* hit = (THcAerogelHit *) rawhits->At(ihit);

    Int_t element = hit->fCounter - 1;
    Int_t adcpos  = hit->GetRawAdcHitPos().GetPulseInt();
    Int_t adcneg  = hit->GetRawAdcHitNeg().GetPulseInt();
    if(adcpos <= fPosPedLimit[element]) {
      fPosPedSum[element]  += adcpos;
      fPosPedSum2[element] += adcpos*adcpos;
      fPosPedCount[element]++;
      if(fPosPedCount[element] == fMinPeds/5) 
	fPosPedLimit[element] = 100 + fPosPedSum[element]/fPosPedCount[element];
    }
    if(adcneg <= fNegPedLimit[element]) {
      fNegPedSum[element] += adcneg;
      fNegPedSum2[element] += adcneg*adcneg;
      fNegPedCount[element]++;
      if(fNegPedCount[element] == fMinPeds/5) 
	fNegPedLimit[element] = 100 + fNegPedSum[element]/fNegPedCount[element];
    }
    ihit++;
  }
  fNPedestalEvents++;
  return;
}

//_____________________________________________________________________________
// Method for calculating pedestals in the 6 GeV era
void THcAerogel::CalculatePedestals()
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")
  //  cout << "Plane: " << fPlaneNum << endl;
  for(Int_t i=0; i<fNelem;i++) {

    // Positive tubes
    fPosPed[i]    = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosThresh[i] = fPosPed[i] + 15;
    // Negative tubes
    fNegPed[i]    = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegThresh[i] = fNegPed[i] + 15;
    //    cout << i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;

    // Just a copy for now, but allow the possibility that fXXXPedMean is set
    // in a parameter file and only overwritten if there is a sufficient number of
    // pedestal events.  (So that pedestals are sensible even if the pedestal events were
    // not acquired.)
    if(fMinPeds > 0) {
      if(fPosPedCount[i] > fMinPeds)
	fPosPedMean[i] = fPosPed[i];
      if(fNegPedCount[i] > fMinPeds) 
	fNegPedMean[i] = fNegPed[i];
    }
  }
}

//_____________________________________________________________________________
Int_t THcAerogel::GetIndex(Int_t nRegion, Int_t nValue) {

  return fNRegions * nValue + nRegion;

}

//_____________________________________________________________________________
void THcAerogel::Print(const Option_t* opt) const {
  THaNonTrackingDetector::Print(opt);

  // Print out the pedestals
  cout << endl;
  cout << "Aerogel Pedestals" << endl;
  cout << "No.   Neg    Pos" << endl;
  for(Int_t i=0; i<fNelem; i++)
    cout << " " << i << "\t" << fNegPedMean[i] << "\t" << fPosPedMean[i] << endl;
  cout << endl;
  cout << " fMinPeds = " << fMinPeds << endl;
  cout << endl;
}

ClassImp(THcAerogel)
////////////////////////////////////////////////////////////////////////////////
