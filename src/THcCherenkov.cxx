/** \class THcCherenkov
    \ingroup Detectors

\brief Class for gas Cherenkov detectors

*/

#include "THcCherenkov.h"
#include "THcHodoscope.h"
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
#include "THcHallCSpectrometer.h"

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <iomanip>


using namespace std;

using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::setprecision;

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( const char* name, const char* description,
                            THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  frAdcPedRaw       = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseIntRaw  = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseTimeRaw = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPed          = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseInt     = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseAmp     = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseTime    = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  fAdcErrorFlag     = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);

  fNumAdcHits         = vector<Int_t>    (MaxNumCerPmt, 0.0);
  fNumGoodAdcHits     = vector<Int_t>    (MaxNumCerPmt, 0.0);
  fNumTracksMatched   = vector<Int_t>    (MaxNumCerPmt, 0.0);
  fNumTracksFired     = vector<Int_t>    (MaxNumCerPmt, 0.0);
  fNpe                = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcPed         = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcPulseInt    = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcPulseIntRaw = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcPulseAmp    = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcPulseTime   = vector<Double_t> (MaxNumCerPmt, 0.0);
  fGoodAdcTdcDiffTime   = vector<Double_t> (MaxNumCerPmt, 0.0);

  InitArrays();
}

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( ) :
  THaNonTrackingDetector()
{
  // Constructor
  frAdcPedRaw       = NULL;
  frAdcPulseIntRaw  = NULL;
  frAdcPulseAmpRaw  = NULL;
  frAdcPulseTimeRaw = NULL;
  frAdcPed          = NULL;
  frAdcPulseInt     = NULL;
  frAdcPulseAmp     = NULL;
  frAdcPulseTime    = NULL;
  fAdcErrorFlag     = NULL;

  InitArrays();
}

//_____________________________________________________________________________
THcCherenkov::~THcCherenkov()
{
  // Destructor
  delete frAdcPedRaw;       frAdcPedRaw       = NULL;
  delete frAdcPulseIntRaw;  frAdcPulseIntRaw  = NULL;
  delete frAdcPulseAmpRaw;  frAdcPulseAmpRaw  = NULL;
  delete frAdcPulseTimeRaw; frAdcPulseTimeRaw = NULL;
  delete frAdcPed;          frAdcPed          = NULL;
  delete frAdcPulseInt;     frAdcPulseInt     = NULL;
  delete frAdcPulseAmp;     frAdcPulseAmp     = NULL;
  delete frAdcPulseTime;    frAdcPulseTime    = NULL;
  delete fAdcErrorFlag;     fAdcErrorFlag     = NULL;

  DeleteArrays();
}

//_____________________________________________________________________________
void THcCherenkov::InitArrays()
{
  fGain     = NULL;
  fPedSum   = NULL;
  fPedSum2  = NULL;
  fPedLimit = NULL;
  fPedMean  = NULL;
  fPedCount = NULL;
  fPed      = NULL;
  fThresh   = NULL;
}
//_____________________________________________________________________________
void THcCherenkov::DeleteArrays()
{
  delete [] fGain; fGain = NULL;

  // 6 Gev variables
  delete [] fPedSum;   fPedSum   = NULL;
  delete [] fPedSum2;  fPedSum2  = NULL;
  delete [] fPedLimit; fPedLimit = NULL;
  delete [] fPedMean;  fPedMean  = NULL;
  delete [] fPedCount; fPedCount = NULL;
  delete [] fPed;      fPed      = NULL;
  delete [] fThresh;   fThresh   = NULL;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCherenkov::Init( const TDatime& date )
{
  // cout << "THcCherenkov::Init for: " << GetName() << endl;

  string EngineDID = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(EngineDID.begin(), EngineDID.end(), EngineDID.begin(), ::toupper);
  if(gHcDetectorMap->FillMap(fDetMap, EngineDID.c_str()) < 0) {
    static const char* const here = "Init()";
    Error(Here(here), "Error filling detectormap for %s.", EngineDID.c_str());
    return kInitError;
  }

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  InitHitList(fDetMap, "THcCherenkovHit", fDetMap->GetTotNumChan()+1);

  EStatus status;
  if((status = THaNonTrackingDetector::Init( date )))
    return fStatus=status;

  THcHallCSpectrometer *app=dynamic_cast<THcHallCSpectrometer*>(GetApparatus());
   if(  !app ||
      !(fglHod = dynamic_cast<THcHodoscope*>(app->GetDetector("hod"))) ) {
    static const char* const here = "ReadDatabase()";
    Warning(Here(here),"Hodoscope \"%s\" not found. ","hod");
  }

 fPresentP = 0;
  THaVar* vpresent = gHaVars->Find(Form("%s.present",GetApparatus()->GetName()));
  if(vpresent) {
    fPresentP = (Bool_t *) vpresent->GetValuePointer();
  }
  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  // cout << "THcCherenkov::ReadDatabase for: " << GetName() << endl; // Ahmed

  string prefix = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  fNRegions = 4;  // Defualt if not set in paramter file

  DBRequest list_1[] = {
    {"_tot_pmts", &fNelem, kInt},
    {"_num_regions",      &fNRegions,         kInt},
    {0}
  };

  gHcParms->LoadParmValues(list_1, prefix.c_str());

  Bool_t optional = true;

  cout << "Created Cherenkov detector " << GetApparatus()->GetName() << "."
       << GetName() << " with " << fNelem << " PMTs" << endl;

  // 6 GeV pedestal paramters
  fPedLimit = new Int_t[fNelem];
  fGain     = new Double_t[fNelem];
  fPedMean  = new Double_t[fNelem];

  // Region parameters
  fRegionsValueMax = fNRegions * 8;
  fRegionValue     = new Double_t[fRegionsValueMax];

  DBRequest list[]={
    {"_ped_limit",        fPedLimit,          kInt,     (UInt_t) fNelem, optional},
    {"_adc_to_npe",       fGain,              kDouble,  (UInt_t) fNelem},
    {"_red_chi2_min",     &fRedChi2Min,       kDouble},
    {"_red_chi2_max",     &fRedChi2Max,       kDouble},
    {"_beta_min",         &fBetaMin,          kDouble},
    {"_beta_max",         &fBetaMax,          kDouble},
    {"_enorm_min",        &fENormMin,         kDouble},
    {"_enorm_max",        &fENormMax,         kDouble},
    {"_dp_min",           &fDpMin,            kDouble},
    {"_dp_max",           &fDpMax,            kDouble},
    {"_mirror_zpos",      &fMirrorZPos,       kDouble},
    {"_npe_thresh",       &fNpeThresh,        kDouble},
    {"_debug_adc",        &fDebugAdc,         kInt, 0, 1},
    {"_adcTimeWindowMin", &fAdcTimeWindowMin, kDouble},
    {"_adcTimeWindowMax", &fAdcTimeWindowMax, kDouble},
    {"_adc_tdc_offset",   &fAdcTdcOffset,     kDouble, 0, 1},
    {"_region",           &fRegionValue[0],   kDouble,  (UInt_t) fRegionsValueMax},
    {0}
  };

  fDebugAdc = 0; // Set ADC debug parameter to false unless set in parameter file
  fAdcTdcOffset = 0.0;

  gHcParms->LoadParmValues((DBRequest*)&list, prefix.c_str());

  // if (fDebugAdc) cout << "Cherenkov ADC Debug Flag Set To TRUE" << endl;

  fIsInit = true;

  // cout << "Track Matching Parameters for: " << GetName() << endl;
  // for (Int_t iregion = 0; iregion < fNRegions; iregion++) {
  //   cout << "Region = " << iregion + 1 << endl;
  //   for (Int_t ivalue = 0; ivalue < 8; ivalue++)
  //     cout << fRegionValue[GetIndex(iregion, ivalue)] << "  ";
  //   cout << endl;
  // }

  // Create arrays to hold pedestal results
  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree
  // cout << "THcCherenkov::DefineVariables called for: " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  if (fDebugAdc) {
    RVarDef vars[] = {
      {"numAdcHits",      "Number of ADC Hits Per PMT", "fNumAdcHits"},        // Cherenkov occupancy
      {"totNumAdcHits",   "Total Number of ADC Hits",   "fTotNumAdcHits"},     // Cherenkov multiplicity
      {"adcPedRaw",       "Raw ADC pedestals",          "frAdcPedRaw.THcSignalHit.GetData()"},
      {"adcPulseIntRaw",  "Raw ADC pulse integrals",    "frAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"adcPulseAmpRaw",  "Raw ADC pulse amplitudes",   "frAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"adcPulseTimeRaw", "Raw ADC pulse times",        "frAdcPulseTimeRaw.THcSignalHit.GetData()"},
      {"adcPed",          "ADC pedestals",              "frAdcPed.THcSignalHit.GetData()"},
      {"adcPulseInt",     "ADC pulse integrals",        "frAdcPulseInt.THcSignalHit.GetData()"},
      {"adcPulseAmp",     "ADC pulse amplitudes",       "frAdcPulseAmp.THcSignalHit.GetData()"},
      {"adcPulseTime",    "ADC pulse times",            "frAdcPulseTime.THcSignalHit.GetData()"},
      { 0 }
    };
    DefineVarsFromList( vars, mode);
  } //end debug statement

  RVarDef vars[] = {
    {"adcCounter",   "ADC counter numbers",            "frAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},
    {"adcErrorFlag", "Error Flag for When FPGA Fails", "fAdcErrorFlag.THcSignalHit.GetData()"},

    {"numGoodAdcHits",    "Number of Good ADC Hits Per PMT", "fNumGoodAdcHits"},    // Cherenkov occupancy
    {"totNumGoodAdcHits", "Total Number of Good ADC Hits",   "fTotNumGoodAdcHits"}, // Cherenkov multiplicity

    {"numTracksMatched",    "Number of Tracks Matched Per Region",       "fNumTracksMatched"},
    {"numTracksFired",      "Number of Tracks that Fired Per Region",    "fNumTracksFired"},
    {"totNumTracksMatched", "Total Number of Tracks Matched Per Region", "fTotNumTracksMatched"},
    {"totNumTracksFired",   "Total Number of Tracks that Fired",         "fTotNumTracksFired"},

    {"xAtCer",       "Track X at Cherenkov mirror",    "fXAtCer"},
    {"yAtCer",       "Track Y at Cherenkov mirror",    "fYAtCer"},

    {"npe",          "Number of PEs",                  "fNpe"},
    {"npeSum",       "Total Number of PEs",            "fNpeSum"},

    {"goodAdcPed",          "Good ADC pedestals",           "fGoodAdcPed"},
    {"goodAdcPulseInt",     "Good ADC pulse integrals",     "fGoodAdcPulseInt"},
    {"goodAdcPulseIntRaw",  "Good ADC raw pulse integrals", "fGoodAdcPulseIntRaw"},
    {"goodAdcPulseAmp",     "Good ADC pulse amplitudes",    "fGoodAdcPulseAmp"},
    {"goodAdcPulseTime",    "Good ADC pulse times",         "fGoodAdcPulseTime"},
     {"goodAdcTdcDiffTime",    "Good Hodo Start - ADC pulse times",         "fGoodAdcTdcDiffTime"},
   { 0 }
  };

  return DefineVarsFromList(vars, mode);
}
//_____________________________________________________________________________
inline
void THcCherenkov::Clear(Option_t* opt)
{
  // Clear the hit lists
  fNhits               = 0;
  fTotNumAdcHits       = 0;
  fTotNumGoodAdcHits   = 0;
  fTotNumTracksMatched = 0;
  fTotNumTracksFired   = 0;

  fXAtCer = 0.0;
  fYAtCer = 0.0;

  fNpeSum = 0.0;

  frAdcPedRaw->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();
  frAdcPulseTime->Clear();
  fAdcErrorFlag->Clear();

  for (UInt_t ielem = 0; ielem < fNumAdcHits.size(); ielem++)
    fNumAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumGoodAdcHits.size(); ielem++)
    fNumGoodAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumTracksMatched.size(); ielem++)
    fNumTracksMatched.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumTracksFired.size(); ielem++)
    fNumTracksFired.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fGoodAdcPed.size(); ielem++) {
    fGoodAdcPed.at(ielem)         = 0.0;
    fGoodAdcPulseInt.at(ielem)    = 0.0;
    fGoodAdcPulseIntRaw.at(ielem) = 0.0;
    fGoodAdcPulseAmp.at(ielem)    = 0.0;
    fGoodAdcPulseTime.at(ielem)   = kBig;
    fGoodAdcTdcDiffTime.at(ielem)   = kBig;
    fNpe.at(ielem)                = 0.0;
  }

}

//_____________________________________________________________________________
Int_t THcCherenkov::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  Bool_t present = kTRUE;	// Suppress reference time warnings
  if(fPresentP) {		// if this spectrometer not part of trigger
    present = *fPresentP;
  }
  fNhits = DecodeToHitList(evdata, !present);

  if(gHaCuts->Result("Pedestal_event")) {
    AccumulatePedestals(fRawHitList);
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
    CalculatePedestals();
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t  ihit      = 0;
  UInt_t nrAdcHits = 0;

  while(ihit < fNhits) {

    THcCherenkovHit* hit         = (THcCherenkovHit*) fRawHitList->At(ihit);
    Int_t            npmt        = hit->fCounter;
    THcRawAdcHit&    rawAdcHit   = hit->GetRawAdcHitPos();

    for (UInt_t thit = 0; thit < rawAdcHit.GetNPulses(); thit++) {

      ((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPedRaw());
      ((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPed());

      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseTimeRaw(thit));
      ((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->Set(npmt, rawAdcHit.GetPulseTime(thit)+fAdcTdcOffset);

      if (rawAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(npmt, 0);
      if (rawAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(npmt, 1);

      ++nrAdcHits;
      fTotNumAdcHits++;
      fNumAdcHits.at(npmt-1) = npmt;
    }
    ihit++;
  }
  return ihit;
}

//_____________________________________________________________________________
Int_t THcCherenkov::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcCherenkov::CoarseProcess( TClonesArray&  )
{
  Double_t StartTime = 0.0;
  if( fglHod ) StartTime = fglHod->GetStartTime();

  // Loop over the elements in the TClonesArray
  for(Int_t ielem = 0; ielem < frAdcPulseInt->GetEntries(); ielem++) {

    Int_t    npmt         = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulsePed     = ((THcSignalHit*) frAdcPed->ConstructedAt(ielem))->GetData();
    Double_t pulseInt     = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw  = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp     = ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frAdcPulseTime->ConstructedAt(ielem))->GetData();
   Double_t adctdcdiffTime = StartTime-pulseTime;
     Bool_t   errorFlag    = ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(ielem))->GetData();
    Bool_t   pulseTimeCut = adctdcdiffTime > fAdcTimeWindowMin && adctdcdiffTime < fAdcTimeWindowMax;

    // By default, the last hit within the timing cut will be considered "good"
    if (!errorFlag && pulseTimeCut) {
      fGoodAdcPed.at(npmt)         = pulsePed;
      fGoodAdcPulseInt.at(npmt)    = pulseInt;
      fGoodAdcPulseIntRaw.at(npmt) = pulseIntRaw;
      fGoodAdcPulseAmp.at(npmt)    = pulseAmp;
      fGoodAdcPulseTime.at(npmt)   = pulseTime;
      fGoodAdcTdcDiffTime.at(npmt)   = adctdcdiffTime;

      fNpe.at(npmt) = fGain[npmt]*fGoodAdcPulseInt.at(npmt);
      fNpeSum += fNpe.at(npmt);

      fTotNumGoodAdcHits++;
      fNumGoodAdcHits.at(npmt) = npmt + 1;
    }
  }
  return 0;
}

//_____________________________________________________________________________
Int_t THcCherenkov::FineProcess( TClonesArray& tracks )
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
    Double_t trackDp      = track->GetDp();
    Double_t trackENorm   = trackEnergy/trackMom;
    Double_t trackXfp     = track->GetX();
    Double_t trackYfp     = track->GetY();
    Double_t trackTheta   = track->GetTheta();
    Double_t trackPhi     = track->GetPhi();

    Bool_t trackRedChi2Cut = trackRedChi2 > fRedChi2Min && trackRedChi2 < fRedChi2Max;
    Bool_t trackBetaCut    = trackBeta    > fBetaMin    && trackBeta    < fBetaMax;
    Bool_t trackENormCut   = trackENorm   > fENormMin   && trackENorm   < fENormMax;
    Bool_t trackDpCut      = trackDp      > fDpMin      && trackDp      < fDpMax;

    if (trackRedChi2Cut && trackBetaCut && trackENormCut && trackDpCut) {

        // Project the track to the Cherenkov mirror planes
        fXAtCer = trackXfp + trackTheta * fMirrorZPos;
        fYAtCer = trackYfp + trackPhi   * fMirrorZPos;

        // cout << "Cherenkov Detector: " << GetName() << " has fNRegions = " << fNRegions << endl;
        // cout << "nTracks = " << nTracks << "\t" << "trackChi2 = " << trackChi2
        // 	   << "\t" << "trackNDof = " << trackNDoF << "\t" << "trackRedChi2 = " << trackRedChi2 << endl;
        // cout << "trackBeta = " << trackBeta << "\t" << "trackEnergy = " << trackEnergy << "\t"
        // 	   << "trackMom = " << trackMom << "\t" << "trackENorm = " << trackENorm << endl;
        // cout << "trackXfp = " << trackXfp << "\t" << "trackYfp = " << trackYfp << "\t"
        // 	   << "trackTheta = " << trackTheta << "\t" << "trackPhi = " << trackPhi << endl;
        // cout << "fMirrorZPos = " << fMirrorZPos << "\t" << "fXAtCer = " << fXAtCer << "\t" << "fYAtCer = " << fYAtCer << endl;
        // cout << "=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:" << endl;

        for (Int_t iregion = 0; iregion < fNRegions; iregion++) {

            if ((TMath::Abs(fRegionValue[GetIndex(iregion, 0)] - fXAtCer)   < fRegionValue[GetIndex(iregion, 4)]) &&
                (TMath::Abs(fRegionValue[GetIndex(iregion, 1)] - fYAtCer)   < fRegionValue[GetIndex(iregion, 5)]) &&
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
void THcCherenkov::InitializePedestals()
{
  fNPedestalEvents = 0;
  fMinPeds         = 500; 		// In engine, this is set in parameter file
  fPedSum          = new Int_t [fNelem];
  fPedSum2         = new Int_t [fNelem];
  fPedCount        = new Int_t [fNelem];
  fPed             = new Double_t [fNelem];
  fThresh          = new Double_t [fNelem];
  for(Int_t i = 0; i < fNelem; i++) {
    fPedSum[i]   = 0;
    fPedSum2[i]  = 0;
    fPedCount[i] = 0;
  }
}

//_____________________________________________________________________________
void THcCherenkov::AccumulatePedestals(TClonesArray* rawhits)
{
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = 0;
  while(ihit < nrawhits) {
    THcCherenkovHit* hit = (THcCherenkovHit *) rawhits->At(ihit);

    Int_t element = hit->fCounter - 1;
    Int_t nadc = hit->GetRawAdcHitPos().GetPulseIntRaw();
    if(nadc <= fPedLimit[element]) {
      fPedSum[element]  += nadc;
      fPedSum2[element] += nadc*nadc;
      fPedCount[element]++;
      if(fPedCount[element] == fMinPeds/5) {
	fPedLimit[element] = 100 + fPedSum[element]/fPedCount[element];
      }
    }
    ihit++;
  }

  fNPedestalEvents++;

  return;
}

//_____________________________________________________________________________
void THcCherenkov::CalculatePedestals()
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")
  //  cout << "Plane: " << fPlaneNum << endl;
  for(Int_t i=0; i<fNelem;i++) {

    // PMT tubes
    fPed[i] = ((Double_t) fPedSum[i]) / TMath::Max(1, fPedCount[i]);
    fThresh[i] = fPed[i] + 15;

    // Just a copy for now, but allow the possibility that fXXXPedMean is set
    // in a parameter file and only overwritten if there is a sufficient number of
    // pedestal events.  (So that pedestals are sensible even if the pedestal events were
    // not acquired.)
    if(fMinPeds > 0) {
      if(fPedCount[i] > fMinPeds) {
	fPedMean[i] = fPed[i];
      }
    }
  }
  //  cout << " " << endl;

}

//_____________________________________________________________________________
Int_t THcCherenkov::GetIndex(Int_t nRegion, Int_t nValue)
{
  return fNRegions * nValue + nRegion;
}


//_____________________________________________________________________________
void THcCherenkov::Print(const Option_t* opt) const
{
  THaNonTrackingDetector::Print(opt);
  // Print out the pedestals
  cout << endl;
  cout << "Cherenkov Pedestals" << endl;
  // Ahmed
  cout << "No.   ADC" << endl;
  for(Int_t i=0; i<fNelem; i++)
    cout << " " << i << "    " << fPed[i] << endl;
  cout << endl;
}

//_____________________________________________________________________________
Double_t THcCherenkov::GetCerNPE()
{
  return fNpeSum;
}
//_____________________________________________________________________________
Int_t THcCherenkov::End(THaRunBase* run)
{
  MissReport(Form("%s.%s", GetApparatus()->GetName(), GetName()));
  return 0;
}
ClassImp(THcCherenkov)
////////////////////////////////////////////////////////////////////////////////
