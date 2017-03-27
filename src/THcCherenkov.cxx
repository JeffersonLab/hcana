/** \class THcCherenkov
    \ingroup Detectors

    Class for Cherenkov detectors

*/

#include "THcCherenkov.h"
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

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

using std::cout;
using std::cin;
using std::endl;

#include <iomanip>
using std::setw;
using std::setprecision;

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( const char* name, const char* description,
                            THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  fADCHits = new TClonesArray("THcSignalHit", 16);

  frAdcPedRaw       = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseIntRaw  = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseTimeRaw = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);

  frAdcPed      = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseInt = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  frAdcPulseAmp = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);
  fAdcErrorFlag = new TClonesArray("THcSignalHit", MaxNumCerPmt*MaxNumAdcPulse);

  fNpe                 = vector<Double_t>(MaxNumCerPmt, 0.0);
  fGoodAdcPed          = vector<Double_t>(MaxNumCerPmt, 0.0);
  fGoodAdcPulseInt     = vector<Double_t>(MaxNumCerPmt, 0.0);
  fGoodAdcPulseIntRaw  = vector<Double_t>(MaxNumCerPmt, 0.0);
  fGoodAdcPulseAmp     = vector<Double_t>(MaxNumCerPmt, 0.0);
  fGoodAdcPulseTime    = vector<Double_t>(MaxNumCerPmt, 0.0);
    
  InitArrays();
}

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( ) :
  THaNonTrackingDetector()
{
  // Constructor
  fADCHits = NULL;

  frAdcPedRaw       = NULL;
  frAdcPulseIntRaw  = NULL;
  frAdcPulseAmpRaw  = NULL;
  frAdcPulseTimeRaw = NULL;

  frAdcPed      = NULL;
  frAdcPulseInt = NULL;
  frAdcPulseAmp = NULL;
  fAdcErrorFlag = NULL;

  InitArrays();
}

//_____________________________________________________________________________
THcCherenkov::~THcCherenkov()
{
  // Destructor
  delete fADCHits; fADCHits = NULL;

  delete frAdcPedRaw;       frAdcPedRaw       = NULL;
  delete frAdcPulseIntRaw;  frAdcPulseIntRaw  = NULL;
  delete frAdcPulseAmpRaw;  frAdcPulseAmpRaw  = NULL;
  delete frAdcPulseTimeRaw; frAdcPulseTimeRaw = NULL;

  delete frAdcPed;      frAdcPed      = NULL;
  delete frAdcPulseInt; frAdcPulseInt = NULL;
  delete frAdcPulseAmp; frAdcPulseAmp = NULL;
  delete fAdcErrorFlag; fAdcErrorFlag = NULL;

  DeleteArrays();

}

//_____________________________________________________________________________
void THcCherenkov::InitArrays()
{
  fGain = NULL;
  fNPMT = NULL;
  fPedSum = NULL;
  fPedSum2 = NULL;
  fPedLimit = NULL;
  fPedMean = NULL;
  fPedCount = NULL;
  fPed = NULL;
  fThresh = NULL;
}
//_____________________________________________________________________________
void THcCherenkov::DeleteArrays()
{
  delete [] fGain; fGain = NULL;
  delete [] fNPMT; fNPMT = NULL;
  
  // 6 Gev pedestal variables
  delete [] fPedSum; fPedSum = NULL;
  delete [] fPedSum2; fPedSum2 = NULL;
  delete [] fPedLimit; fPedLimit = NULL;
  delete [] fPedMean; fPedMean = NULL;
  delete [] fPedCount; fPedCount = NULL;
  delete [] fPed; fPed = NULL;
  delete [] fThresh; fThresh = NULL;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCherenkov::Init( const TDatime& date )
{
  cout << "THcCherenkov::Init " << GetName() << endl;

  string EngineDID = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(EngineDID.begin(), EngineDID.end(), EngineDID.begin(), ::toupper);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID.c_str()) < 0 ) {
    static const char* const here = "Init()";
    Error(Here(here), "Error filling detectormap for %s.", EngineDID.c_str());
    return kInitError;
  }

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  InitHitList(fDetMap, "THcCherenkovHit", fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  cout << "THcCherenkov::ReadDatabase " << GetName() << endl; // Ahmed

  string prefix = string(GetApparatus()->GetName()).substr(0, 1) + GetName();
  std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  DBRequest list_1[] = {
    {"_tot_pmts", &fNelem, kInt},
    {0}
  };
  gHcParms->LoadParmValues(list_1, prefix.c_str());

  //    fNelem = 2;      // Default if not defined
  fCerNRegions = 3;

  Bool_t optional = true;

  fNPMT = new Int_t[fNelem];
  fADC_hit = new Int_t[fNelem];

  fGain = new Double_t[fNelem];
  fPedLimit = new Int_t[fNelem];
  fPedMean = new Double_t[fNelem];

  fCerTrackCounter = new Int_t [fCerNRegions];
  fCerFiredCounter = new Int_t [fCerNRegions];
  for ( Int_t ireg = 0; ireg < fCerNRegions; ireg++ ) {
    fCerTrackCounter[ireg] = 0;
    fCerFiredCounter[ireg] = 0;
  }

  fCerRegionsValueMax = fCerNRegions * 8; // This value 8 should also be in paramter file
  fCerRegionValue = new Double_t [fCerRegionsValueMax];

  DBRequest list[]={
    {"_ped_limit",   fPedLimit,           kInt,    (UInt_t) fNelem, optional},
    {"_adc_to_npe",  fGain,               kDouble, (UInt_t) fNelem},
    {"_chi2max",     &fCerChi2Max,        kDouble},
    {"_beta_min",    &fCerBetaMin,        kDouble},
    {"_beta_max",    &fCerBetaMax,        kDouble},
    {"_et_min",      &fCerETMin,          kDouble},
    {"_et_max",      &fCerETMax,          kDouble},
    {"_mirror_zpos", &fCerMirrorZPos,     kDouble},
    {"_region",      &fCerRegionValue[0], kDouble, (UInt_t) fCerRegionsValueMax},
    {"_threshold",   &fCerThresh,         kDouble},
    {"_debug_adc",   &fDebugAdc,          kInt, 0, 1},
    //    {"cer_regions",     &fCerNRegions,       kInt},
    {0}
  };

  fDebugAdc = 1; // Set ADC debug parameter to false unless set in parameter file

  gHcParms->LoadParmValues((DBRequest*)&list,prefix.c_str());

  if (fDebugAdc) cout << "Cherenkov ADC Debug Flag Set To TRUE" << endl;

  fIsInit = true;


  for (Int_t i1 = 0; i1 < fCerNRegions; i1++ ) {
    cout << "Region " << i1 << endl;
    for (Int_t i2 = 0; i2 < 8; i2++ ) {
      cout << fCerRegionValue[GetCerIndex( i1, i2 )] << " ";
    }
    cout <<endl;
  }

  // Create arrays to hold pedestal results
  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcCherenkov::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  // Do we need to put the number of pos/neg TDC/ADC hits into the variables?
  // No.  They show up in tree as Ndata.H.aero.postdchits for example

  vector<RVarDef> vars;
  
  vars.push_back({"phototubes",      "Nuber of Cherenkov photo tubes",        "fNPMT"});
  vars.push_back({"adc_hit",         "ADC hit flag =1 means hit",             "fADC_hit"});
  
  vars.push_back({"ncherhit",        "Number of Hits(Cherenkov)",             "fNCherHit"});
  vars.push_back({"certrackcounter", "Tracks inside Cherenkov region",        "fCerTrackCounter"});
  vars.push_back({"cerfiredcounter", "Tracks with engough Cherenkov NPEs ",   "fCerFiredCounter"});
 
  vars.push_back({"npe",             "Number of PEs",            "fNpe"});
  vars.push_back({"npeSum",          "Total Number of PEs",      "fNpeSum"});

  vars.push_back({"adcCounter",      "List of ADC counter numbers.",      "frAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"});
  vars.push_back({"adcErrorFlag",    "Error Flag for When FPGA Fails",    "fAdcErrorFlag.THcSignalHit.GetData()"});

  if (fDebugAdc) {   
    vars.push_back({"adcPedRaw",       "List of raw ADC pedestals",         "frAdcPedRaw.THcSignalHit.GetData()"});
    vars.push_back({"adcPulseIntRaw",  "List of raw ADC pulse integrals.",  "frAdcPulseIntRaw.THcSignalHit.GetData()"});
    vars.push_back({"adcPulseAmpRaw",  "List of raw ADC pulse amplitudes.", "frAdcPulseAmpRaw.THcSignalHit.GetData()"});
    vars.push_back({"adcPulseTimeRaw", "List of raw ADC pulse times.",      "frAdcPulseTimeRaw.THcSignalHit.GetData()"});

    vars.push_back({"adcPed",          "List of ADC pedestals",             "frAdcPed.THcSignalHit.GetData()"});
    vars.push_back({"adcPulseInt",     "List of ADC pulse integrals.",      "frAdcPulseInt.THcSignalHit.GetData()"});
    vars.push_back({"adcPulseAmp",     "List of ADC pulse amplitudes.",     "frAdcPulseAmp.THcSignalHit.GetData()"});
  }

  vars.push_back({"goodAdcPed",          "Good ADC pedestals",           "fGoodAdcPed"});
  vars.push_back({"goodAdcPulseInt",     "Good ADC pulse integrals",     "fGoodAdcPulseInt"});
  vars.push_back({"goodAdcPulseIntRaw",  "Good ADC raw pulse integrals", "fGoodAdcPulseIntRaw"});
  vars.push_back({"goodAdcPulseAmp",     "Good ADC pulse amplitudes",    "fGoodAdcPulseAmp"});
  vars.push_back({"goodAdcPulseTime",    "Good ADC pulse times",         "fGoodAdcPulseTime"});
  
  RVarDef end {};
  vars.push_back(end);  

  return DefineVarsFromList(vars.data(), mode);

}
//_____________________________________________________________________________
inline
void THcCherenkov::Clear(Option_t* opt)
{
  // Clear the hit lists
  fADCHits->Clear();

  fNhits = 0;	     
  fNpeSum = 0.0;
  fNCherHit = 0;

  for(Int_t itube = 0; itube < fNelem; itube++) {
    fNPMT[itube] = 0;
    fADC_hit[itube] = 0;
  }

  frAdcPedRaw->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();
  fAdcErrorFlag->Clear();

  for (UInt_t ielem = 0; ielem < fGoodAdcPed.size(); ielem++) {
    fGoodAdcPed.at(ielem)         = 0.0;
    fGoodAdcPulseInt.at(ielem)    = 0.0;
    fGoodAdcPulseIntRaw.at(ielem) = 0.0;
    fGoodAdcPulseAmp.at(ielem)    = 0.0;
    fGoodAdcPulseTime.at(ielem)   = 0.0;
    fNpe.at(ielem)                = 0.0;
  }

}

//_____________________________________________________________________________
Int_t THcCherenkov::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  fNhits = DecodeToHitList(evdata);

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
  Int_t  nADCHits  = 0;
  UInt_t nrAdcHits = 0;

  while(ihit < fNhits) {

    THcCherenkovHit* hit       = (THcCherenkovHit*) fRawHitList->At(ihit);
    Int_t            padnum    = hit->fCounter;
    THcRawAdcHit&    rawAdcHit = hit->GetRawAdcHitPos();
    
    for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
           
      ((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPedRaw());
      ((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPed());

      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTimeRaw(thit));

      if (rawAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 0);
      if (rawAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 1);
      
      fADC_hit[padnum-1] = 1;  

      // cout << "fNhits = " << fNhits << "\t" << "npmt = " << padnum-1 << "\t" 
      // 	   << "thit = " << thit << "\t" 
      // 	   << "frAdcPulseInt = " << rawAdcHit.GetPulseInt(thit) << endl;

      ++nrAdcHits;
    }
    // ADC hit
    if(hit->GetRawAdcHitPos().GetPulseIntRaw() >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fADCHits->ConstructedAt(nADCHits++);
      sighit->Set(hit->fCounter, hit->GetRawAdcHitPos().GetPulseIntRaw());
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
  
  // Loop over the elements in the TClonesArray
  for(Int_t ielem = 0; ielem < frAdcPulseInt->GetEntries(); ielem++) {

    Int_t    npmt         = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulsePed     = ((THcSignalHit*) frAdcPed->ConstructedAt(ielem))->GetData();
    Double_t pulseInt     = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw  = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp     = ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
    Bool_t   errorFlag    = ((THcSignalHit*) fAdcErrorFlag->ConstructedAt(ielem))->GetData();  
    Bool_t   pulseTimeCut = pulseTime > 500 && pulseTime < 2500;

    // cout << "npmt = " << npmt << "\t" << "ielem = " << ielem << "\t"
    // 	 << "pulseInt = " << pulseInt << "\t" << endl;

    // By default, the last hit within the timing cut will be considered "good"
    if (!errorFlag && pulseTimeCut) {
      fGoodAdcPed.at(npmt)         = pulsePed;
      fGoodAdcPulseInt.at(npmt)    = pulseInt;
      fGoodAdcPulseIntRaw.at(npmt) = pulseIntRaw;
      fGoodAdcPulseAmp.at(npmt)    = pulseAmp;
      fGoodAdcPulseTime.at(npmt)   = pulseTime;
      
      fNpe.at(npmt) = fGain[npmt]*fGoodAdcPulseInt.at(npmt);
      fNpeSum += fNpe.at(npmt);
    }
    fNPMT[npmt] = npmt + 1;
    fNCherHit ++;  
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcCherenkov::FineProcess( TClonesArray& tracks )
{
  
  Int_t nTracks = tracks.GetLast() + 1;

  cout << "nTracks = " << nTracks << endl;

  if (ntracks >= 1) {

    THaTrack* track = dynamic_cast<THaTrack*> (tracks.At(0));
    if (!track) return -1;

    Double_t trackChi2   = track->GetChi2();
    Int_t    trackNDoF   = track->GetNDoF();
    Double_t trackBeta   = track->GetBeta();
    Double_t trackEnergy = track->GetEnergy();
    Double_t trackMom    = track->GetP();
    Double_t trackXfp    = track->GetX();
    Double_t trackYfp    = track->GetY();
    Double_t trackTheta  = track->GetTheta();
    Double_t trackPhi    = track->GetPhi();

    cout << "trackChi2 = " << trackChi2 << "\t" << "trackNDof = " << trackNDoF << "\t"
	 << "trackBeta = " << trackBeta << "\t" << "trackEnergy = " << trackEnergy << "\t"
	 << "trackMom = " << trackMom << endl;
    cout << "trackXfp = " << trackXfp << "\t" << "trackYfp = " << trackYfp << "\t"
	 << "trackTheta = " << trackTheta << "\t" << "trackPhi = " << trackPhi << endl;
    cout << "=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:" << endl;

    if (( track->GetChi2()/track->GetNDoF() > 0. ) &&
	( track->GetChi2()/track->GetNDoF() <  fCerChi2Max ) &&
	( track->GetBeta() > fCerBetaMin ) &&
	( track->GetBeta() < fCerBetaMax ) &&
	( ( track->GetEnergy() / track->GetP() ) > fCerETMin ) &&
	( ( track->GetEnergy() / track->GetP() ) < fCerETMax )
	) {

      Double_t cerX = track->GetX() + track->GetTheta() * fCerMirrorZPos;
      Double_t cerY = track->GetY() + track->GetPhi()   * fCerMirrorZPos;

      for ( Int_t ir = 0; ir < fCerNRegions; ir++ ) {

	//	*     hit must be inside the region in order to continue.

	if ( ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 0 )] - cerX ) <
	       fCerRegionValue[GetCerIndex( ir, 4 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 1 )] - cerY ) <
	       fCerRegionValue[GetCerIndex( ir, 5 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 2 )] - track->GetTheta() ) <
	       fCerRegionValue[GetCerIndex( ir, 6 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 3 )] - track->GetPhi() ) <
	       fCerRegionValue[GetCerIndex( ir, 7 )] )
	     ) {

	  // *     increment the 'should have fired' counters
	  fCerTrackCounter[ir] ++;

	  // *     increment the 'did fire' counters
	  if ( fNpeSum > fCerThresh ) {
	    fCerFiredCounter[ir] ++;
	  }
	}
      } // loop over regions
    }
  }

  return 0;
}

//_____________________________________________________________________________
void THcCherenkov::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPedSum = new Int_t [fNelem];
  fPedSum2 = new Int_t [fNelem];
  fPedCount = new Int_t [fNelem];

  fPed = new Double_t [fNelem];
  fThresh = new Double_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPedSum[i] = 0;
    fPedSum2[i] = 0;
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
      fPedSum[element] += nadc;
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
void THcCherenkov::CalculatePedestals( )
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
Int_t THcCherenkov::GetCerIndex( Int_t nRegion, Int_t nValue ) {

  return fCerNRegions * nValue + nRegion;
}


//_____________________________________________________________________________
void THcCherenkov::Print( const Option_t* opt) const {
  THaNonTrackingDetector::Print(opt);

  // Print out the pedestals

  cout << endl;
  cout << "Cherenkov Pedestals" << endl;

  // Ahmed
  cout << "No.   ADC" << endl;
  for(Int_t i=0; i<fNelem; i++){
    cout << " " << i << "    " << fPed[i] << endl;
  }

  cout << endl;
}

//_____________________________________________________________________________
Double_t THcCherenkov::GetCerNPE() {

  return fNpeSum;
}

ClassImp(THcCherenkov)
////////////////////////////////////////////////////////////////////////////////
