/** \class THcShowerPlane
    \ingroup DetSupport

    \brief One plane of shower blocks with side readout

*/

#include "THcShowerPlane.h"
#include "THcHodoscope.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
#include "math.h"
#include "THaTrack.h"
#include "THaTrackProj.h"
#include "THcHallCSpectrometer.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <fstream>

using namespace std;

ClassImp(THcShowerPlane)

//______________________________________________________________________________
THcShowerPlane::THcShowerPlane( const char* name,
                                const char* description,
					    const Int_t layernum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosADCHits = new TClonesArray("THcSignalHit",fNelem);
  fNegADCHits = new TClonesArray("THcSignalHit",fNelem);

  frPosAdcErrorFlag    = new TClonesArray("THcSignalHit", 16);
  frPosAdcPedRaw       = new TClonesArray("THcSignalHit", 16);
  frPosAdcThreshold    = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frPosAdcPed       = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseInt  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmp  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTime = new TClonesArray("THcSignalHit", 16);

  frNegAdcErrorFlag    = new TClonesArray("THcSignalHit", 16);
  frNegAdcPedRaw       = new TClonesArray("THcSignalHit", 16);
  frNegAdcThreshold    = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frNegAdcPed       = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseInt  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmp  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTime = new TClonesArray("THcSignalHit", 16);

  frPosAdcSampPedRaw       = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPed          = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseInt	   = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseAmp	   = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseTime	   = new TClonesArray("THcSignalHit", 16);

  frNegAdcSampPedRaw       = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPed 	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseInt	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseAmp	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseTime    = new TClonesArray("THcSignalHit", 16);


  //#if ROOT_VERSION_CODE < ROOT_VERSION(5,32,0)
  //  fPosADCHitsClass = fPosADCHits->GetClass();
  //  fNegADCHitsClass = fNegADCHits->GetClass();
  //#endif

  fLayerNum = layernum;
}

//______________________________________________________________________________
THcShowerPlane::~THcShowerPlane()
{
  // Destructor
  delete fPosADCHits; fPosADCHits = NULL;
  delete fNegADCHits; fNegADCHits = NULL;

  delete  frPosAdcErrorFlag; frPosAdcErrorFlag = NULL;
  delete  frPosAdcPedRaw; frPosAdcPedRaw = NULL;
  delete  frPosAdcThreshold; frPosAdcThreshold = NULL;
  delete  frPosAdcPulseIntRaw; frPosAdcPulseIntRaw = NULL;
  delete  frPosAdcPulseAmpRaw; frPosAdcPulseAmpRaw = NULL;
  delete  frPosAdcPulseTimeRaw; frPosAdcPulseTimeRaw = NULL;

  delete  frPosAdcPed; frPosAdcPed = NULL;
  delete  frPosAdcPulseInt; frPosAdcPulseInt = NULL;
  delete  frPosAdcPulseAmp; frPosAdcPulseAmp = NULL;
  delete  frPosAdcPulseTime; frPosAdcPulseTime = NULL;

  delete  frNegAdcErrorFlag; frNegAdcErrorFlag = NULL;
  delete  frNegAdcPedRaw; frNegAdcPedRaw = NULL;
  delete  frNegAdcThreshold; frNegAdcThreshold = NULL;
  delete  frNegAdcPulseIntRaw; frNegAdcPulseIntRaw = NULL;
  delete  frNegAdcPulseAmpRaw; frNegAdcPulseAmpRaw = NULL;
  delete  frNegAdcPulseTimeRaw; frNegAdcPulseTimeRaw = NULL;

  delete frPosAdcSampPedRaw;       frPosAdcSampPedRaw       = NULL;
  delete frPosAdcSampPulseIntRaw;  frPosAdcSampPulseIntRaw  = NULL;
  delete frPosAdcSampPulseAmpRaw;  frPosAdcSampPulseAmpRaw  = NULL;
  delete frPosAdcSampPulseTimeRaw; frPosAdcSampPulseTimeRaw = NULL;
  delete frPosAdcSampPed;          frPosAdcSampPed          = NULL;
  delete frPosAdcSampPulseInt;     frPosAdcSampPulseInt     = NULL;
  delete frPosAdcSampPulseAmp;     frPosAdcSampPulseAmp     = NULL;
  delete frPosAdcSampPulseTime;    frPosAdcSampPulseTime    = NULL;

  delete frNegAdcSampPedRaw;       frNegAdcSampPedRaw       = NULL;
  delete frNegAdcSampPulseIntRaw;  frNegAdcSampPulseIntRaw  = NULL;
  delete frNegAdcSampPulseAmpRaw;  frNegAdcSampPulseAmpRaw  = NULL;
  delete frNegAdcSampPulseTimeRaw; frNegAdcSampPulseTimeRaw = NULL;
  delete frNegAdcSampPed;          frNegAdcSampPed          = NULL;
  delete frNegAdcSampPulseInt;     frNegAdcSampPulseInt     = NULL;
  delete frNegAdcSampPulseAmp;     frNegAdcSampPulseAmp     = NULL;
  delete frNegAdcSampPulseTime;    frNegAdcSampPulseTime    = NULL;

  delete  frNegAdcPed; frNegAdcPed = NULL;
  delete  frNegAdcPulseInt; frNegAdcPulseInt = NULL;
  delete  frNegAdcPulseAmp; frNegAdcPulseAmp = NULL;
  delete  frNegAdcPulseTime; frNegAdcPulseTime = NULL;

  delete [] fPosPedSum;
  delete [] fPosPedSum2;
  delete [] fPosPedLimit;
  delete [] fPosPedCount;

  delete [] fNegPedSum;
  delete [] fNegPedSum2;
  delete [] fNegPedLimit;
  delete [] fNegPedCount;

  delete [] fPosPed;
  delete [] fPosSig;
  delete [] fPosThresh;

  delete [] fNegPed;
  delete [] fNegSig;
  delete [] fNegThresh;

//  delete [] fEpos;
  // delete [] fEneg;
  // delete [] fEmean;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShowerPlane::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();
  fParent = GetParent();

  // Get pointer to Cherenkov object.  "hgcer" if SHMS, "cer" if other
  THcHallCSpectrometer *app=dynamic_cast<THcHallCSpectrometer*>(GetApparatus());
  if (fParent->GetPrefix()[0] == 'P') {
    fCherenkov = dynamic_cast<THcCherenkov*>(app->GetDetector("hgcer"));
  } else {
    fCherenkov = dynamic_cast<THcCherenkov*>(app->GetDetector("cer"));
  }
  if (!fCherenkov) {
    cout << "****** THcShowerPlane::Init  Cherenkov not found! ******" << endl;
    cout << "****** THcShowerPlane::Accumulate will be skipped ******" << endl;
  }

  if(  !app ||
      !(fglHod = dynamic_cast<THcHodoscope*>(app->GetDetector("hod"))) ) {
    static const char* const here = "ReadDatabase()";
    Warning(Here(here),"Hodoscope \"%s\" not found. ","hod");
  }

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcShowerPlane::ReadDatabase( const TDatime& date )
{

  // Retrieve FADC parameters.  In principle may want different dynamic
  // pedestal and integration range for preshower and shower, but for now
  // use same parameters
  char prefix[2];
  prefix[0]=tolower(fParent->GetPrefix()[0]);
  prefix[1]='\0';
  fPedSampLow=0;
  fPedSampHigh=9;
  fDataSampLow=23;
  fDataSampHigh=49;
  fAdcNegThreshold=0.;
  fAdcPosThreshold=0.;
  fStatCerMin=1.;
  fStatSlop=2.;
  fStatMaxChi2=10.;
  DBRequest list[]={
    {"cal_AdcNegThreshold", &fAdcNegThreshold, kDouble, 0, 1},
    {"cal_AdcPosThreshold", &fAdcPosThreshold, kDouble, 0, 1},
    {"cal_ped_sample_low", &fPedSampLow, kInt, 0, 1},
    {"cal_ped_sample_high", &fPedSampHigh, kInt, 0, 1},
    {"cal_data_sample_low", &fDataSampLow, kInt, 0, 1},
    {"cal_data_sample_high", &fDataSampHigh, kInt, 0, 1},
    {"cal_debug_adc", &fDebugAdc, kInt, 0, 1},
    {"cal_SampThreshold",     &fSampThreshold,       kDouble,0,1},
    {"cal_SampNSA",     &fSampNSA,       kInt,0,1},
    {"cal_SampNSAT",     &fSampNSAT,       kInt,0,1},
    {"cal_SampNSB",     &fSampNSB,       kInt,0,1},
    {"cal_OutputSampWaveform",     &fOutputSampWaveform,       kInt,0,1},
    {"cal_UseSampWaveform",     &fUseSampWaveform,       kInt,0,1},
    {"stat_cermin", &fStatCerMin, kDouble, 0, 1},
    {"stat_slop", &fStatSlop, kDouble, 0, 1},
    {"stat_maxchisq", &fStatMaxChi2, kDouble, 0, 1},
    {0}
  };

  fDebugAdc   = 0; // Set ADC debug parameter to false unless set in parameter file

  fSampThreshold = 5.;
  fSampNSA = 0; // use value stored in event 125 info
  fSampNSB = 0; // use value stored in event 125 info
  fSampNSAT = 2; // default value in THcRawHit::SetF250Params
  fOutputSampWaveform = 0; // 0= no output , 1 = output Sample Waveform
  fUseSampWaveform = 0; // 0= do not use , 1 = use Sample Waveform

  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  // Retrieve more parameters we need from parent class
  //
  THcShower* parent = static_cast<THcShower*>(fParent);

  //  Find the number of elements
  fNelem = parent->GetNBlocks(fLayerNum-1);

  // Origin of the plane:
  //
  // X is average of top X coordinates of the top and bottom blocks
  // shifted by a half of the block thickness;
  // Y is average of left and right edges;
  // Z is _front_ position of the plane along the beam.

  Double_t BlockThick = parent->GetBlockThick(fLayerNum-1);

  Double_t xOrig = (parent->GetXPos(fLayerNum-1,0) +
		    parent->GetXPos(fLayerNum-1,fNelem-1))/2 +
    BlockThick/2;

  Double_t yOrig = (parent->GetYPos(fLayerNum-1,0) +
		    parent->GetYPos(fLayerNum-1,1))/2;

  Double_t zOrig = parent->GetZPos(fLayerNum-1);

  fOrigin.SetXYZ(xOrig, yOrig, zOrig);

  fAdcTdcOffset=parent->GetAdcTdcOffset();

  // Create arrays to hold results here
  //

  // Pedestal limits per channel.

  fPosPedLimit = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPosPedLimit[i] = parent->GetPedLimit(i,fLayerNum-1,0);
    fNegPedLimit[i] = parent->GetPedLimit(i,fLayerNum-1,1);
  }

  fMinPeds = parent->GetMinPeds();

  InitializePedestals();

  fNumGoodPosAdcHits          = vector<Int_t>    (fNelem, 0.0);
  fNumGoodNegAdcHits          = vector<Int_t>    (fNelem, 0.0);

  fGoodPosAdcPulseIntRaw      = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseIntRaw      = vector<Double_t> (fNelem, 0.0);

  fGoodPosAdcPed              = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseInt         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseAmp         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseTime        = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcTdcDiffTime        = vector<Double_t> (fNelem, 0.0);

  fGoodNegAdcPed              = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseInt         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseAmp         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseTime        = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcTdcDiffTime        = vector<Double_t> (fNelem, 0.0);
  
  fGoodPosAdcMult         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcMult         = vector<Double_t> (fNelem, 0.0);

  // Energy depositions per block (not corrected for track coordinate)

  fEpos                       = vector<Double_t> (fNelem, 0.0);
  fEneg                       = vector<Double_t> (fNelem, 0.0);
  fEmean                      = vector<Double_t> (fNelem, 0.0);
  //  fEpos = new Double_t[fNelem];
  // fEneg = new Double_t[fNelem];
  // fEmean= new Double_t[fNelem];

  // Numbers of tracks and hits , for efficiency calculations.
  
  fStatNumTrk = vector<Int_t> (fNelem, 0);
  fStatNumHit = vector<Int_t> (fNelem, 0);
  fTotStatNumTrk = 0;
  fTotStatNumHit = 0;
  
  // Debug output.

  if (parent->fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::ReadDatabase for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  Layer #" << fLayerNum << ", number of elements " << dec << fNelem
	 << endl;

    cout << "  Origin of Layer at  X = " << fOrigin.X()
	 << "  Y = " << fOrigin.Y() << "  Z = " << fOrigin.Z() << endl;

    cout << "  fPosPedLimit:";
    for(Int_t i=0;i<fNelem;i++) cout << " " << fPosPedLimit[i];
    cout << endl;
    cout << "  fNegPedLimit:";
    for(Int_t i=0;i<fNelem;i++) cout << " " << fNegPedLimit[i];
    cout << endl;

    cout << "  fMinPeds = " << fMinPeds << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return kOK;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  if (fDebugAdc) {
    RVarDef vars[] = {
      {"posAdcPedRaw",       "List of positive raw ADC pedestals",         "frPosAdcPedRaw.THcSignalHit.GetData()"},
      {"posAdcPulseIntRaw",  "List of positive raw ADC pulse integrals.",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"posAdcPulseAmpRaw",  "List of positive raw ADC pulse amplitudes.", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"posAdcPulseTimeRaw", "List of positive raw ADC pulse times.",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"},

      {"posAdcPed",          "List of positive ADC pedestals",             "frPosAdcPed.THcSignalHit.GetData()"},
      {"posAdcPulseInt",     "List of positive ADC pulse integrals.",      "frPosAdcPulseInt.THcSignalHit.GetData()"},
      {"posAdcPulseAmp",     "List of positive ADC pulse amplitudes.",     "frPosAdcPulseAmp.THcSignalHit.GetData()"},
      {"posAdcPulseTime",    "List of positive ADC pulse times.",          "frPosAdcPulseTime.THcSignalHit.GetData()"},

      {"posAdcSampPedRaw",       "Positive Raw Samp ADC pedestals",        "frPosAdcSampPedRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseIntRaw",  "Positive Raw Samp ADC pulse integrals",  "frPosAdcSampPulseIntRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseAmpRaw",  "Positive Raw Samp ADC pulse amplitudes", "frPosAdcSampPulseAmpRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseTimeRaw", "Positive Raw Samp ADC pulse times",      "frPosAdcSampPulseTimeRaw.THcSignalHit.GetData()"},
      {"posAdcSampPed",          "Positive Samp ADC pedestals",            "frPosAdcSampPed.THcSignalHit.GetData()"},
      {"posAdcSampPulseInt",     "Positive Samp ADC pulse integrals",      "frPosAdcSampPulseInt.THcSignalHit.GetData()"},
      {"posAdcSampPulseAmp",     "Positive Samp ADC pulse amplitudes",     "frPosAdcSampPulseAmp.THcSignalHit.GetData()"},
      {"posAdcSampPulseTime",    "Positive Samp ADC pulse times",          "frPosAdcSampPulseTime.THcSignalHit.GetData()"},

      {"negAdcPedRaw",       "List of negative raw ADC pedestals",         "frNegAdcPedRaw.THcSignalHit.GetData()"},
      {"negAdcPulseIntRaw",  "List of negative raw ADC pulse integrals.",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"negAdcPulseAmpRaw",  "List of negative raw ADC pulse amplitudes.", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"negAdcPulseTimeRaw", "List of negative raw ADC pulse times.",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"},

      {"negAdcPed",          "List of negative ADC pedestals",             "frNegAdcPed.THcSignalHit.GetData()"},
      {"negAdcPulseInt",     "List of negative ADC pulse integrals.",      "frNegAdcPulseInt.THcSignalHit.GetData()"},
      {"negAdcPulseAmp",     "List of negative ADC pulse amplitudes.",     "frNegAdcPulseAmp.THcSignalHit.GetData()"},
      {"negAdcPulseTime",    "List of negative ADC pulse times.",          "frNegAdcPulseTime.THcSignalHit.GetData()"},

      {"negAdcSampPedRaw",       "Negative Raw Samp ADC pedestals",        "frNegAdcSampPedRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseIntRaw",  "Negative Raw Samp ADC pulse integrals",  "frNegAdcSampPulseIntRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseAmpRaw",  "Negative Raw Samp ADC pulse amplitudes", "frNegAdcSampPulseAmpRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseTimeRaw", "Negative Raw Samp ADC pulse times",      "frNegAdcSampPulseTimeRaw.THcSignalHit.GetData()"},
      {"negAdcSampPed",          "Negative Samp ADC pedestals",            "frNegAdcSampPed.THcSignalHit.GetData()"},
      {"negAdcSampPulseInt",     "Negative Samp ADC pulse integrals",      "frNegAdcSampPulseInt.THcSignalHit.GetData()"},
      {"negAdcSampPulseAmp",     "Negative Samp ADC pulse amplitudes",     "frNegAdcSampPulseAmp.THcSignalHit.GetData()"},
      {"negAdcSampPulseTime",    "Negative Samp ADC pulse times",          "frNegAdcSampPulseTime.THcSignalHit.GetData()"},

      { 0 }
    };
    DefineVarsFromList( vars, mode);
  } //end debug statement

  if (fOutputSampWaveform==1) {
  RVarDef vars[] = {
    {"adcNegSampWaveform",          "FADC Neg ADCSample Waveform",           "fNegAdcSampWaveform"},
    {"adcPosSampWaveform",          "FADC Pos ADCSample Waveform",           "fPosAdcSampWaveform"},
      { 0 }
    };
    DefineVarsFromList( vars, mode);
  }
  //

  // Register counters for efficiency calculations in gHcParms so that the
  // variables can be used in end of run reports.

  gHcParms->Define(Form("%sstat_trksum%d", fParent->GetPrefix(), fLayerNum),
	  Form("Number of tracks in calo. layer %d",fLayerNum), fTotStatNumTrk);
  gHcParms->Define(Form("%sstat_hitsum%d", fParent->GetPrefix(), fLayerNum),
	 Form("Number of hits in calo. layer %d", fLayerNum), fTotStatNumHit);

  cout << "THcShowerPlane::DefineVariables: registered counters "
       << Form("%sstat_trksum%d",fParent->GetPrefix(),fLayerNum) << " and "
       << Form("%sstat_hitsum%d",fParent->GetPrefix(),fLayerNum) << endl;
  //  getchar();
    
  RVarDef vars[] = {
    {"posAdcErrorFlag",    "List of positive raw ADC Error Flags",  "frPosAdcErrorFlag.THcSignalHit.GetData()"},
    {"negAdcErrorFlag",    "List of negative raw ADC Error Flags ", "frNegAdcErrorFlag.THcSignalHit.GetData()"},

    {"posAdcCounter",      "List of positive ADC counter numbers.", "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"}, //PreSh+ raw occupancy
    {"negAdcCounter",      "List of negative ADC counter numbers.", "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"}, //PreSh- raw occupancy

    {"totNumPosAdcHits", "Total Number of Positive ADC Hits",   "fTotNumPosAdcHits"}, // PreSh+ raw multiplicity
    {"totNumNegAdcHits", "Total Number of Negative ADC Hits",   "fTotNumNegAdcHits"}, // PreSh+ raw multiplicity
    {"totnumAdcHits",    "Total Number of ADC Hits Per PMT",    "fTotNumAdcHits"},    // PreSh raw multiplicity

    {"numGoodPosAdcHits",  "Number of Good Positive ADC Hits Per PMT", "fNumGoodPosAdcHits"},    // PreSh occupancy
    {"numGoodNegAdcHits",  "Number of Good Negative ADC Hits Per PMT", "fNumGoodNegAdcHits"},    // PreSh occupancy
    {"totNumGoodPosAdcHits", "Total Number of Good Positive ADC Hits",   "fTotNumGoodPosAdcHits"}, // PreSh multiplicity
    {"totNumGoodNegAdcHits", "Total Number of Good Negative ADC Hits",   "fTotNumGoodNegAdcHits"}, // PreSh multiplicity
    {"totnumGoodAdcHits",  "TotalNumber of Good ADC Hits Per PMT",      "fTotNumGoodAdcHits"},    // PreSh multiplicity

    {"goodPosAdcPulseIntRaw", "Good Positive Raw ADC Pulse Integrals",                   "fGoodPosAdcPulseIntRaw"},
    {"goodNegAdcPulseIntRaw", "Good Negative Raw ADC Pulse Integrals",                   "fGoodNegAdcPulseIntRaw"},

    {"goodPosAdcPed",      "Good Positive ADC pedestals",           "fGoodPosAdcPed"},
    {"goodPosAdcPulseInt", "Good Positive ADC integrals",           "fGoodPosAdcPulseInt"},
    {"goodPosAdcPulseAmp", "Good Positive ADC amplitudes",          "fGoodPosAdcPulseAmp"},
    {"goodPosAdcPulseTime","Good Positive ADC times",               "fGoodPosAdcPulseTime"},
    {"goodPosAdcTdcDiffTime","Good Positive Hodo Start time-ADC times",               "fGoodPosAdcTdcDiffTime"},

    {"goodNegAdcPed",      "Good Negative ADC pedestals",           "fGoodNegAdcPed"},
    {"goodNegAdcPulseInt", "Good Negative ADC integrals",           "fGoodNegAdcPulseInt"},
    {"goodNegAdcPulseAmp", "Good Negative ADC amplitudes",          "fGoodNegAdcPulseAmp"},
    {"goodNegAdcPulseTime","Good Negative ADC times",               "fGoodNegAdcPulseTime"},
    {"goodNegAdcTdcDiffTime","Good Negative Hodo Start time-ADC times",               "fGoodNegAdcTdcDiffTime"},
    {"goodPosAdcMult",          "Good Positive ADC Multiplicity",           "fGoodPosAdcMult"},
    {"goodNegAdcMult",          "Good Negative ADC Multiplicity",           "fGoodNegAdcMult"},
    {"epos",       "Energy Depositions from Positive Side PMTs",    "fEpos"},
    {"eneg",       "Energy Depositions from Negative Side PMTs",    "fEneg"},
    {"emean",      "Mean Energy Depositions",                       "fEmean"},
    {"eplane",     "Energy Deposition per plane",                   "fEplane"},
    {"eplane_pos", "Energy Deposition per plane from pos. PMTs",    "fEplane_pos"},
    {"eplane_neg", "Energy Deposition per plane from neg. PMTs",    "fEplane_neg"},
    { 0 }
  };

  return DefineVarsFromList(vars, mode);
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  // Clears the hit lists

  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcErrorFlag->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcThreshold->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();
  frPosAdcPulseTime->Clear();

  frPosAdcSampPedRaw->Clear();
  frPosAdcSampPulseIntRaw->Clear();
  frPosAdcSampPulseAmpRaw->Clear();
  frPosAdcSampPulseTimeRaw->Clear();
  frPosAdcSampPed->Clear();
  frPosAdcSampPulseInt->Clear();
  frPosAdcSampPulseAmp->Clear();
  frPosAdcSampPulseTime->Clear();

  frNegAdcErrorFlag->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcThreshold->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();
  frNegAdcPulseTime->Clear();

  frNegAdcSampPedRaw->Clear();
  frNegAdcSampPulseIntRaw->Clear();
  frNegAdcSampPulseAmpRaw->Clear();
  frNegAdcSampPulseTimeRaw->Clear();
  frNegAdcSampPed->Clear();
  frNegAdcSampPulseInt->Clear();
  frNegAdcSampPulseAmp->Clear();
  frNegAdcSampPulseTime->Clear();

  for (UInt_t ielem = 0; ielem < fGoodPosAdcPed.size(); ielem++) {
    fGoodPosAdcPed.at(ielem)              = 0.0;
    fGoodPosAdcPulseIntRaw.at(ielem)      = 0.0;
    fGoodPosAdcPulseInt.at(ielem)         = 0.0;
    fGoodPosAdcPulseAmp.at(ielem)         = 0.0;
    fGoodPosAdcPulseTime.at(ielem)        = kBig;
    fGoodPosAdcTdcDiffTime.at(ielem)        = kBig;
    fGoodPosAdcMult.at(ielem)               = 0.0;
    fEpos.at(ielem)                       = 0.0;
    fNumGoodPosAdcHits.at(ielem)          = 0.0;
  }

  for (UInt_t ielem = 0; ielem < fGoodNegAdcPed.size(); ielem++) {
    fGoodNegAdcPed.at(ielem)              = 0.0;
    fGoodNegAdcPulseIntRaw.at(ielem)      = 0.0;
    fGoodNegAdcPulseInt.at(ielem)         = 0.0;
    fGoodNegAdcPulseAmp.at(ielem)         = 0.0;
    fGoodNegAdcPulseTime.at(ielem)        = kBig;
    fGoodNegAdcTdcDiffTime.at(ielem)        = kBig;
    fGoodNegAdcMult.at(ielem)               = 0.0;
    fEneg.at(ielem)                       = 0.0;
    fNumGoodNegAdcHits.at(ielem)          = 0.0;
  }

  for (UInt_t ielem = 0; ielem < fEmean.size(); ielem++) {
    fEmean.at(ielem)                                = 0.0;
  }

  fTotNumAdcHits       = 0;
  fTotNumPosAdcHits    = 0;
  fTotNumNegAdcHits    = 0;

  fTotNumGoodAdcHits       = 0;
  fTotNumGoodPosAdcHits    = 0;
  fTotNumGoodNegAdcHits    = 0;


 // Debug output.
  if ( static_cast<THcShower*>(GetParent())->fdbg_decoded_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::Clear for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << " Cleared ADC hits for plane " << GetName() << endl;
    cout << "---------------------------------------------------------------\n";
  }
}

//_____________________________________________________________________________
Int_t THcShowerPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead

  //Debug output.
  if ( static_cast<THcShower*>(fParent)->fdbg_decoded_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::Decode for "
      	 << fParent->GetPrefix() << ":" << endl;

    cout << " Called for plane " << GetName() << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcess( TClonesArray& tracks )
{

  // Nothing is done here. See ProcessHits method instead.
  //

 return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list
  // Assumes that the hit list is sorted by layer, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  // Initialize variables.

  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcErrorFlag->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcThreshold->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();
  frPosAdcPulseTime->Clear();

  frPosAdcSampPedRaw->Clear();
  frPosAdcSampPulseIntRaw->Clear();
  frPosAdcSampPulseAmpRaw->Clear();
  frPosAdcSampPulseTimeRaw->Clear();
  frPosAdcSampPed->Clear();
  frPosAdcSampPulseInt->Clear();
  frPosAdcSampPulseAmp->Clear();
  frPosAdcSampPulseTime->Clear();

  frNegAdcErrorFlag->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcThreshold->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();
  frNegAdcPulseTime->Clear();

  frNegAdcSampPedRaw->Clear();
  frNegAdcSampPulseIntRaw->Clear();
  frNegAdcSampPulseAmpRaw->Clear();
  frNegAdcSampPulseTimeRaw->Clear();
  frNegAdcSampPed->Clear();
  frNegAdcSampPulseInt->Clear();
  frNegAdcSampPulseAmp->Clear();
  frNegAdcSampPulseTime->Clear();

  /*
    for(Int_t i=0;i<fNelem;i++) {

    fEpos[i] = 0;
    fEneg[i] = 0;
    fEmean[i] = 0;
  }
  */

  fEplane = 0;
  fEplane_pos = 0;
  fEplane_neg = 0;

  UInt_t nrPosAdcHits = 0;
  UInt_t nrNegAdcHits = 0;
  UInt_t nrSampPosAdcHits = 0;
  UInt_t nrSampNegAdcHits = 0;
  fPosAdcSampWaveform.clear();
  fNegAdcSampWaveform.clear();

  // Process raw hits. Get ADC hits for the plane, assign variables for each
  // channel.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // This is OK as far as the hit list is sorted by layer.
    //
    if(hit->fPlane > fLayerNum) {
      break;
    }

    Int_t padnum = hit->fCounter;

    THcRawAdcHit& rawPosAdcHit = hit->GetRawAdcHitPos();

    if ( fUseSampWaveform == 0 ) {
      for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
	((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPedRaw());
	((THcSignalHit*) frPosAdcThreshold->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetPedRaw()*rawPosAdcHit.GetF250_PeakPedestalRatio()+fAdcPosThreshold);
	((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPed());
	
	((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseIntRaw(thit));
	((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseInt(thit));
	
	((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmpRaw(thit));
	((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmp(thit));
	
	((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTimeRaw(thit));
	((THcSignalHit*) frPosAdcPulseTime->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTime(thit)+fAdcTdcOffset);
	
	if (rawPosAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 0);
	if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 1);
	if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0 && rawPosAdcHit.GetNSamples() >0) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 2);

	if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0) {
	  Double_t PeakPedRatio= rawPosAdcHit.GetF250_PeakPedestalRatio();
	  Int_t NPedSamples= rawPosAdcHit.GetF250_NPedestalSamples();
	  Double_t AdcToC =  rawPosAdcHit.GetAdcTopC();
	  Double_t AdcToV =  rawPosAdcHit.GetAdcTomV();
	  Int_t PedDefaultTemp = static_cast<THcShower*>(fParent)->GetPedDefault(padnum-1,fLayerNum-1,0);
	  if (PedDefaultTemp !=0) {
	    Double_t tPulseInt = AdcToC*(rawPosAdcHit.GetPulseIntRaw(thit) - PedDefaultTemp*PeakPedRatio);
	    ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum, tPulseInt);
	    ((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, PedDefaultTemp);
	    ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, float(PedDefaultTemp)/float(NPedSamples)*AdcToV);
	    
	  }
	  ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum, 0.);	
	}
	++nrPosAdcHits;
	fTotNumAdcHits++;
	fTotNumPosAdcHits++;
	
      }
    }
   //
    if (rawPosAdcHit.GetNSamples()>0 ) {
      rawPosAdcHit.SetSampThreshold(fSampThreshold);
      if (fSampNSA == 0) fSampNSA=rawPosAdcHit.GetF250_NSA();
      if (fSampNSB == 0) fSampNSB=rawPosAdcHit.GetF250_NSB();
      rawPosAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
      if (fSampNSAT != 2) rawPosAdcHit.SetSampNSAT(fSampNSAT);
      rawPosAdcHit.SetSampIntTimePedestalPeak();
       fPosAdcSampWaveform.push_back(float(padnum));
       fPosAdcSampWaveform.push_back(float(rawPosAdcHit.GetNSamples()));
      for (UInt_t thit = 0; thit < rawPosAdcHit.GetNSamples(); thit++) {
	fPosAdcSampWaveform.push_back(rawPosAdcHit.GetSample(thit)); // ped subtracted sample (mV)
      }
      for (UInt_t thit = 0; thit < rawPosAdcHit.GetNSampPulses(); thit++) {
      ((THcSignalHit*) frPosAdcSampPedRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPedRaw());
      ((THcSignalHit*) frPosAdcSampPed->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPed());

      ((THcSignalHit*) frPosAdcSampPulseIntRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseIntRaw(thit));
      ((THcSignalHit*) frPosAdcSampPulseInt->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseInt(thit));

      ((THcSignalHit*) frPosAdcSampPulseAmpRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseAmpRaw(thit));
      ((THcSignalHit*) frPosAdcSampPulseAmp->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseAmp(thit));
      ((THcSignalHit*) frPosAdcSampPulseTimeRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTimeRaw(thit));
      ((THcSignalHit*) frPosAdcSampPulseTime->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);
      //
      if ( rawPosAdcHit.GetNPulses() ==0 || fUseSampWaveform ==1 ) {
      ((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPedRaw());
      ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPed());

      ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseIntRaw(thit));
      ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseInt(thit));

      ((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseAmpRaw(thit));
      ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseAmp(thit) );

      ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseTimeRaw(thit) );
      ((THcSignalHit*) frPosAdcPulseTime->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);
      ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 3);  
      if (fUseSampWaveform ==1) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 0);  
        ++nrPosAdcHits;
      fTotNumPosAdcHits++;
      fTotNumAdcHits++;
      }
      ++nrSampPosAdcHits;
      }	
    }

    THcRawAdcHit& rawNegAdcHit = hit->GetRawAdcHitNeg();

    if ( fUseSampWaveform == 0 ) {

      for (UInt_t thit=0; thit<rawNegAdcHit.GetNPulses(); ++thit) {
	((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPedRaw());
	((THcSignalHit*) frNegAdcThreshold->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetPedRaw()*rawNegAdcHit.GetF250_PeakPedestalRatio()+fAdcNegThreshold);
	((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPed());
	
	((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseIntRaw(thit));
	((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseInt(thit));
	
	((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmpRaw(thit));
	((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmp(thit));
	
	((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTimeRaw(thit));
	((THcSignalHit*) frNegAdcPulseTime->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTime(thit)+fAdcTdcOffset);
	
      if (rawNegAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 0);
      if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 1);
      if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0 && rawNegAdcHit.GetNSamples() >0) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 2);

	if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0) {
	  Double_t PeakPedRatio= rawNegAdcHit.GetF250_PeakPedestalRatio();
	  Int_t NPedSamples= rawNegAdcHit.GetF250_NPedestalSamples();
	  Double_t AdcToC =  rawNegAdcHit.GetAdcTopC();
	  Double_t AdcToV =  rawNegAdcHit.GetAdcTomV();
	  Int_t PedDefaultTemp = static_cast<THcShower*>(fParent)->GetPedDefault(padnum-1,fLayerNum-1,1);
	  if (PedDefaultTemp !=0) {
	    Double_t tPulseInt = AdcToC*(rawNegAdcHit.GetPulseIntRaw(thit) - PedDefaultTemp*PeakPedRatio);
	    ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum, tPulseInt);
	    ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, PedDefaultTemp);
	    ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, float(PedDefaultTemp)/float(NPedSamples)*AdcToV);
	    
	  }
	  ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum, 0.);	
	}
	++nrNegAdcHits;
	fTotNumAdcHits++;
	fTotNumNegAdcHits++;
      }
    }

    if (rawNegAdcHit.GetNSamples()>0 ) {
     rawNegAdcHit.SetSampThreshold(fSampThreshold);
      if (fSampNSA == 0) fSampNSA=rawNegAdcHit.GetF250_NSA();
      if (fSampNSB == 0) fSampNSB=rawNegAdcHit.GetF250_NSB();
      rawNegAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
      if (fSampNSAT != 2) rawNegAdcHit.SetSampNSAT(fSampNSAT);
      rawNegAdcHit.SetSampIntTimePedestalPeak();
       fNegAdcSampWaveform.push_back(float(padnum));
       fNegAdcSampWaveform.push_back(float(rawNegAdcHit.GetNSamples()));
      for (UInt_t thit = 0; thit < rawNegAdcHit.GetNSamples(); thit++) {
	fNegAdcSampWaveform.push_back(rawNegAdcHit.GetSample(thit)); // ped subtracted sample (mV)
      }
      for (UInt_t thit = 0; thit < rawNegAdcHit.GetNSampPulses(); thit++) {
      ((THcSignalHit*) frNegAdcSampPedRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPedRaw());
      ((THcSignalHit*) frNegAdcSampPed->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPed());
      ((THcSignalHit*) frNegAdcSampPulseIntRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseIntRaw(thit));
      ((THcSignalHit*) frNegAdcSampPulseInt->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseInt(thit));

      ((THcSignalHit*) frNegAdcSampPulseAmpRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseAmpRaw(thit));
      ((THcSignalHit*) frNegAdcSampPulseAmp->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseAmp(thit));
      ((THcSignalHit*) frNegAdcSampPulseTimeRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTimeRaw(thit));
      ((THcSignalHit*) frNegAdcSampPulseTime->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTime(thit));
      //
      if ( rawNegAdcHit.GetNPulses() ==0 || fUseSampWaveform ==1 ) {
      ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPedRaw());
      ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPed());

      ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseIntRaw(thit));
      ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseInt(thit));

      ((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseAmpRaw(thit));
      ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseAmp(thit) );

      ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseTimeRaw(thit) );
      ((THcSignalHit*) frNegAdcPulseTime->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTime(thit));
      ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 3);  
      if (fUseSampWaveform ==1) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 0);  
        ++nrNegAdcHits;
      fTotNumNegAdcHits++;
      fTotNumAdcHits++;
      }
      ++nrSampNegAdcHits;
      }	
    }
    
    //
    ihit++;
    }
  
  return(ihit);
}
//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcessHits()
{
    Int_t ADCMode=static_cast<THcShower*>(fParent)->GetADCMode();
    if(ADCMode == kADCDynamicPedestal) {
      FillADC_DynamicPedestal();
    } else if (ADCMode == kADCSampleIntegral) {
      FillADC_SampleIntegral();
    } else if (ADCMode == kADCSampIntDynPed) {
      FillADC_SampIntDynPed();
    } else {
      FillADC_Standard();
    }
    //
  if (static_cast<THcShower*>(fParent)->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::ProcessHits for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  Sparsified hits for HMS calorimeter plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;

    for (Int_t i=0; i<fNelem; i++) {

      if (GetAposP(i) > 0  || GetAnegP(i) >0) {    //hit
	cout << "  counter =  " << i
	     << "  Emean = " << fEmean[i]
	     << "  Epos = " << fEpos[i]
	     << "  Eneg = " << fEneg[i]
	     << endl;
	nspar++;
      }
    }

    if (nspar == 0) cout << "  No hits\n";

    cout << "  Eplane = " << fEplane
	 << "  Eplane_pos = " << fEplane_pos
	 << "  Eplane_neg = " << fEplane_neg
	 << endl;
    cout << "---------------------------------------------------------------\n";
  }
  //
  return 1;
    //
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_SampIntDynPed()
{
  //    adc_pos = hit->GetRawAdcHitPos().GetSampleInt();
  //    adc_neg = hit->GetRawAdcHitNeg().GetSampleInt();
  //   adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //   adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // Need to create
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_SampleIntegral()
{
  ///			adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[hit->fCounter -1];
  //			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[hit->fCounter -1];
  //			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // need to create
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_Standard()
{
  for (Int_t ielem=0;ielem<frNegAdcPulseIntRaw->GetEntries();ielem++) {
    Int_t npad = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    fGoodNegAdcPulseIntRaw.at(npad) = pulseIntRaw;
      if(fGoodNegAdcPulseIntRaw.at(npad) >  fNegThresh[npad]) {
	fGoodNegAdcPulseInt.at(npad) = pulseIntRaw-fNegPed[npad];
	fEneg.at(npad) = fGoodNegAdcPulseInt.at(npad)*static_cast<THcShower*>(fParent)->GetGain(npad,fLayerNum-1,1);
	fEmean.at(npad) += fEneg.at(npad);
	fEplane_neg += fEneg.at(npad);
      }
  }
  for (Int_t ielem=0;ielem<frPosAdcPulseIntRaw->GetEntries();ielem++) {
    Int_t npad = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    fGoodPosAdcPulseIntRaw.at(npad) =pulseIntRaw;
    if(fGoodPosAdcPulseIntRaw.at(npad) > fPosThresh[npad]) {
      fGoodPosAdcPulseInt.at(npad) =pulseIntRaw-fPosPed[npad] ;
      fEpos.at(npad) =fGoodPosAdcPulseInt.at(npad)*static_cast<THcShower*>(fParent)->GetGain(npad,fLayerNum-1,0);
      fEmean.at(npad) += fEpos.at(npad);
      fEplane_pos += fEpos.at(npad);
    }
  }
  fEplane= fEplane_neg+fEplane_pos;
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_DynamicPedestal()
{
  Double_t StartTime = 0.0;
  if( fglHod ) StartTime = fglHod->GetStartTime();
   Double_t OffsetTime = 0.0;
   if( fglHod ) OffsetTime = fglHod->GetOffsetTime();
  for (Int_t ielem=0;ielem<frNegAdcPulseInt->GetEntries();ielem++) {
   Int_t    npad         = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
   Double_t pulseInt     = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulsePed     = ((THcSignalHit*) frNegAdcPed->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp     = ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw  = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frNegAdcPulseTime->ConstructedAt(ielem))->GetData();
    Double_t adctdcdiffTime = StartTime-pulseTime+OffsetTime;
    Double_t threshold    = ((THcSignalHit*) frNegAdcThreshold->ConstructedAt(ielem))->GetData();
    Bool_t   pulseTimeCut = (adctdcdiffTime > static_cast<THcShower*>(fParent)->GetWindowMin(npad,fLayerNum-1,1)) && (adctdcdiffTime < static_cast<THcShower*>(fParent)->GetWindowMax(npad,fLayerNum-1,1) );
	fGoodNegAdcMult.at(npad) += 1;
    if (pulseTimeCut) {
      fGoodNegAdcPulseIntRaw.at(npad) =pulseIntRaw;

      if(fGoodNegAdcPulseIntRaw.at(npad) >  threshold && fGoodNegAdcPulseInt.at(npad)==0) {
        fGoodNegAdcPulseInt.at(npad) =pulseInt ;
	fEneg.at(npad) =  fGoodNegAdcPulseInt.at(npad)*static_cast<THcShower*>(fParent)->GetGain(npad,fLayerNum-1,1);
	fEmean.at(npad) += fEneg.at(npad);
	fEplane_neg += fEneg.at(npad);

	fGoodNegAdcPed.at(npad) = pulsePed;
      fGoodNegAdcPulseAmp.at(npad) = pulseAmp;
      fGoodNegAdcPulseTime.at(npad) = pulseTime;
      fGoodNegAdcTdcDiffTime.at(npad) = adctdcdiffTime;

      fTotNumGoodAdcHits++;
      fTotNumGoodNegAdcHits++;
      fNumGoodNegAdcHits.at(npad) = npad + 1;

      }

    }
  }
  //
  for (Int_t ielem=0;ielem<frPosAdcPulseInt->GetEntries();ielem++) {
   Int_t    npad         = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
      Double_t pulsePed     = ((THcSignalHit*) frPosAdcPed->ConstructedAt(ielem))->GetData();
    Double_t threshold    = ((THcSignalHit*) frPosAdcThreshold->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp     = ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(ielem))->GetData();
    Double_t pulseInt     = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frPosAdcPulseTime->ConstructedAt(ielem))->GetData();
     Double_t adctdcdiffTime = StartTime-pulseTime+OffsetTime;
   Bool_t   pulseTimeCut = (adctdcdiffTime > static_cast<THcShower*>(fParent)->GetWindowMin(npad,fLayerNum-1,0)) && (adctdcdiffTime < static_cast<THcShower*>(fParent)->GetWindowMax(npad,fLayerNum-1,0) );
	fGoodPosAdcMult.at(npad) += 1;
    if (pulseTimeCut) {
      fGoodPosAdcPulseIntRaw.at(npad) = pulseIntRaw;

      if(fGoodPosAdcPulseIntRaw.at(npad) >  threshold && fGoodPosAdcPulseInt.at(npad)==0) {

       	fGoodPosAdcPulseInt.at(npad) =pulseInt ;
	fEpos.at(npad) = fGoodPosAdcPulseInt.at(npad)*static_cast<THcShower*>(fParent)->GetGain(npad,fLayerNum-1,0);
	fEmean.at(npad) += fEpos[npad];
	fEplane_pos += fEpos.at(npad);

	fGoodPosAdcPed.at(npad) = pulsePed;
	fGoodPosAdcPulseAmp.at(npad) = pulseAmp;
	fGoodPosAdcPulseTime.at(npad) = pulseTime;
	fGoodPosAdcTdcDiffTime.at(npad) = adctdcdiffTime;

	fTotNumGoodAdcHits++;
	fTotNumGoodPosAdcHits++;
	fNumGoodPosAdcHits.at(npad) = npad + 1;

      }
    }
  }
  //
    fEplane= fEplane_neg+fEplane_pos;

}
//_____________________________________________________________________________
Int_t THcShowerPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list, accumulating into
  // arrays for calculating pedestals.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {

    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // OK for hit list sorted by layer.
    if(hit->fPlane > fLayerNum) {
      break;
    }
    Int_t element = hit->fCounter - 1; // Should check if in range
    Int_t adcpos = hit->GetData(0);
    Int_t adcneg = hit->GetData(1);

    if(adcpos <= fPosPedLimit[element]) {
      fPosPedSum[element] += adcpos;
      fPosPedSum2[element] += adcpos*adcpos;
      fPosPedCount[element]++;
      if(fPosPedCount[element] == fMinPeds/5) {
	fPosPedLimit[element] = 100 + fPosPedSum[element]/fPosPedCount[element];
      }
    }
    if(adcneg <= fNegPedLimit[element]) {
      fNegPedSum[element] += adcneg;
      fNegPedSum2[element] += adcneg*adcneg;
      fNegPedCount[element]++;
      if(fNegPedCount[element] == fMinPeds/5) {
	fNegPedLimit[element] = 100 + fNegPedSum[element]/fNegPedCount[element];
      }
    }
    ihit++;
  }

  fNPedestalEvents++;

  // Debug output.

  if ( static_cast<THcShower*>(fParent)->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::AcculatePedestals for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "Processed hit list for plane " << GetName() << ":\n";

    for (Int_t ih=nexthit; ih<nrawhits; ih++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ih);

      // OK for hit list sorted by layer.
      if(hit->fPlane > fLayerNum) {
	break;
      }

      cout << "  hit " << ih << ":"
	   << "  plane =  " << hit->fPlane
	   << "  counter = " << hit->fCounter
	   << "  ADCpos = " << hit->GetData(0)
	   << "  ADCneg = " << hit->GetData(1)
	   << endl;
    }

    cout << "---------------------------------------------------------------\n";

  }

  return(ihit);
}

//_____________________________________________________________________________
void THcShowerPlane::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")

  for(Int_t i=0; i<fNelem;i++) {

    // Positive tubes
    fPosPed[i] = ((Float_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosSig[i] = sqrt(((Float_t)fPosPedSum2[i])/TMath::Max(1, fPosPedCount[i])
		      - fPosPed[i]*fPosPed[i]);
    fPosThresh[i] = fPosPed[i] + TMath::Min(50., TMath::Max(10., 3.*fPosSig[i]));

    // Negative tubes
    fNegPed[i] = ((Float_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegSig[i] = sqrt(((Float_t)fNegPedSum2[i])/TMath::Max(1, fNegPedCount[i])
		      - fNegPed[i]*fNegPed[i]);
    fNegThresh[i] = fNegPed[i] + TMath::Min(50., TMath::Max(10., 3.*fNegSig[i]));

  }

  // Debug output.

  if ( static_cast<THcShower*>(fParent)->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::CalculatePedestals for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  ADC pedestals and thresholds for calorimeter plane "
	 << GetName() << endl;
    for(Int_t i=0; i<fNelem;i++) {
      cout << "  element " << i << ": "
	   << "  Pos. pedestal = " << fPosPed[i]
	   << "  Pos. threshold = " << fPosThresh[i]
	   << "  Neg. pedestal = " << fNegPed[i]
	   << "  Neg. threshold = " << fNegThresh[i]
	   << endl;
    }
    cout << "---------------------------------------------------------------\n";

  }

}

//_____________________________________________________________________________
void THcShowerPlane::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum = new Int_t [fNelem];
  fNegPedSum2 = new Int_t [fNelem];
  fNegPedCount = new Int_t [fNelem];

  fPosSig = new Float_t [fNelem];
  fNegSig = new Float_t [fNelem];
  fPosPed = new Float_t [fNelem];
  fNegPed = new Float_t [fNelem];
  fPosThresh = new Float_t [fNelem];
  fNegThresh = new Float_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedCount[i] = 0;
  }
}

//_____________________________________________________________________________
Int_t THcShowerPlane::AccumulateStat(TClonesArray& tracks )
{
  // Accumumate statistics for efficiency calculations.
  //
  // Choose electron events in gas Cherenkov with good Chisq of the best track.
  // Project best track to the plane,
  // calculate row number for the track,
  // accrue number of tracks for the row,
  // accrue number of hits for the row, if row is hit.
  // Accrue total numbers of tracks and hits for plane.

  if(!fCherenkov) return 0;

  THaTrack* BestTrack = static_cast<THaTrack*>( tracks[0]);
  if (BestTrack->GetChi2()/BestTrack->GetNDoF() > fStatMaxChi2) return 0;

  if (fCherenkov->GetCerNPE() < fStatCerMin) return 0;
  
  Double_t XTrk = kBig;
  Double_t YTrk = kBig;
  Double_t pathl = kBig;

  // Track interception with plane. The coordinates are in the calorimeter's
  // local system.

  fOrigin = GetOrigin();
  static_cast<THcShower*>(fParent)->CalcTrackIntercept(BestTrack, pathl, XTrk, YTrk);

  // Transform coordiantes to the spectrometer's coordinate system.
  XTrk += GetOrigin().X();
  YTrk += GetOrigin().Y();
						     
  for (Int_t i=0; i<fNelem; i++) {

    if (TMath::Abs(XTrk - static_cast<THcShower*>(fParent)->GetXPos(fLayerNum-1,i)) < fStatSlop &&
	YTrk > static_cast<THcShower*>(fParent)->GetYPos(fLayerNum-1,1) &&
	YTrk < static_cast<THcShower*>(fParent)->GetYPos(fLayerNum-1,0) ) {

      fStatNumTrk.at(i)++;
      fTotStatNumTrk++;
      
      if (fGoodPosAdcPulseInt.at(i) > 0. || fGoodNegAdcPulseInt.at(i) > 0.) {
	fStatNumHit.at(i)++;
	fTotStatNumHit++;
      }
      
    }
    
  }

  if ( static_cast<THcShower*>(fParent)->fdbg_tracks_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "THcShowerPlane::AccumulateStat:" << endl;
    cout << "   Chi2/NDF = " <<BestTrack->GetChi2()/BestTrack->GetNDoF() << endl;
    cout << "   HGCER Npe = " << fCherenkov->GetCerNPE() << endl;
    cout << "   XTrk, YTrk = " << XTrk << "  " << YTrk << endl;						     
    for (Int_t i=0; i<fNelem; i++) {
      if (TMath::Abs(XTrk - static_cast<THcShower*>(fParent)->GetXPos(fLayerNum-1,i)) < fStatSlop) {

	cout << "   Module " << i << ", X=" << static_cast<THcShower*>(fParent)->GetXPos(fLayerNum-1,i)
	     << " matches track" << endl;

	if (fGoodPosAdcPulseInt.at(i) > 0. || fGoodNegAdcPulseInt.at(i) > 0.)
	  cout << "   PulseIntegrals = " << fGoodPosAdcPulseInt.at(i) << "  "
	       << fGoodNegAdcPulseInt.at(i) << endl;
      }
    }
    cout << "---------------------------------------------------------------\n";
    //    getchar();
  }
  
  return 1;
}
