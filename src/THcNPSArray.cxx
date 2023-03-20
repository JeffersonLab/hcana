/** \class THcNPSArray
    \ingroup DetSupport

\brief Fly's eye array of shower blocks

*/

#include "THcNPSArray.h"
#include "THcHodoscope.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcNPSCalorimeter.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
//#include "TSpectrum.h"   // DJH 09/04/22 -- commented out for now
#include "math.h"
#include "THaTrack.h"
#include "THaTrackProj.h"
#include "THcCherenkov.h"         //for efficiency calculations
#include "THcHallCSpectrometer.h"
#include "Helper.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

ClassImp(THcNPSArray)

//______________________________________________________________________________
THcNPSArray::THcNPSArray( const char* name,
                                const char* description,
				const Int_t layernum,
				THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  fADCHits = new TClonesArray("THcSignalHit",100);
  fLayerNum = layernum;

  //C.Y. Mar 15, 2021: Is '16' the maximum total number of hits for all blocks / event ?
  //This limit seems low, as there may be more total hits (see shms_all_10300.dat) (specially for higher block density of NPS)
  //Need to remember and increase this upper limit when the time comes.
  frAdcPedRaw       = new TClonesArray("THcSignalHit", 16);
  frAdcErrorFlag    = new TClonesArray("THcSignalHit", 16);
  frAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frAdcPed       = new TClonesArray("THcSignalHit", 16);
  frAdcPulseInt  = new TClonesArray("THcSignalHit", 16);
  frAdcPulseAmp  = new TClonesArray("THcSignalHit", 16);
  frAdcPulseTime = new TClonesArray("THcSignalHit", 16);

  // DJH 14 Sep 22
  frAdcSampPedRaw       = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frAdcSampPed          = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseInt     = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseAmp     = new TClonesArray("THcSignalHit", 16);
  frAdcSampPulseTime    = new TClonesArray("THcSignalHit", 16);

  fClusterList = new THcNPSShowerClusterList;         // List of hit clusters



}

//______________________________________________________________________________
THcNPSArray::~THcNPSArray()
{
  // Destructor

  Clear(); // deletes allocations in fClusterList
  for (UInt_t i=0; i<fNRows; i++) {
    delete [] fXPos[i];
    delete [] fYPos[i];
    delete [] fZPos[i];
  }
  delete [] fXPos; fXPos = 0;
  delete [] fYPos; fYPos = 0;
  delete [] fZPos; fZPos = 0;

  delete [] fPedLimit;
  delete [] fGain;
  delete [] fPedSum;
  delete [] fPedSum2;
  delete [] fPedCount;
  delete [] fSig;
  delete [] fPed;
  delete [] fThresh;

  delete fADCHits; fADCHits = NULL;

  delete frAdcPedRaw; frAdcPedRaw = NULL;
  delete frAdcErrorFlag; frAdcErrorFlag = NULL;
  delete frAdcPulseIntRaw; frAdcPulseIntRaw = NULL;
  delete frAdcPulseAmpRaw; frAdcPulseAmpRaw = NULL;
  delete frAdcPulseTimeRaw; frAdcPulseTimeRaw = NULL;
  delete frAdcPed; frAdcPed = NULL;
  delete frAdcPulseInt; frAdcPulseInt = NULL;
  delete frAdcPulseAmp; frAdcPulseAmp = NULL;
  delete frAdcPulseTime; frAdcPulseTime = NULL;

  // DJH 14 Sep 22
  delete frAdcSampPedRaw;       frAdcSampPedRaw       = NULL;
  delete frAdcSampPulseIntRaw;  frAdcSampPulseIntRaw  = NULL;
  delete frAdcSampPulseAmpRaw;  frAdcSampPulseAmpRaw  = NULL;
  delete frAdcSampPulseTimeRaw; frAdcSampPulseTimeRaw = NULL;
  delete frAdcSampPed;          frAdcSampPed          = NULL;
  delete frAdcSampPulseInt;     frAdcSampPulseInt     = NULL;
  delete frAdcSampPulseAmp;     frAdcSampPulseAmp     = NULL;
  delete frAdcSampPulseTime;    frAdcSampPulseTime    = NULL;

  //delete [] fA;
  //delete [] fP;
  //delete [] fA_p;

  //delete [] fE;
  delete [] fBlock_ClusterID;

  delete fClusterList; fClusterList = 0;

  delete [] fAdcTimeWindowMin; fAdcTimeWindowMin = 0;
  delete [] fAdcTimeWindowMax; fAdcTimeWindowMax = 0;

  delete [] fAdcPulseTimeMin; fAdcPulseTimeMin = 0;
  delete [] fAdcPulseTimeMax; fAdcPulseTimeMax = 0;
  delete [] fPedDefault; fPedDefault = 0;
  
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcNPSArray::Init( const TDatime& date )
{

  string kwPrefix = GetApparatus()->GetPrefix();
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);

  //C.Y. Jan 21, 2021  : For some odd reason, the kwPrefix returns the spectrometer apparatus prefix as "nps."
  //so I had to manually erase this last "." character for the prefix to be fixed and properly read the NPS
  //parameters
  fKwPrefix = kwPrefix.erase(kwPrefix.size()-1);
 
  // Extra initialization for shower layer: set up DataDest map
  if( IsZombie())
    return fStatus = kInitError;

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcNPSArray::ReadDatabase( const TDatime& date )
{

  // cout << "Parent name: " << GetParent()->GetPrefix() << endl;
  fNRows=fNColumns=0;
  fXFront=fYFront=fZFront=0.;
  fXStep=fYStep=fZSize=0.;
  fUsingFADC=0;
  fPedSampLow=0;
  fPedSampHigh=9;
  fDataSampLow=23;
  fDataSampHigh=49;
  fStatCerMin=1.;
  fStatSlop=3.;
  fStatMaxChi2=10.;
  DBRequest list[]={
    {"_cal_arr_nrows", &fNRows, kInt},
    {"_cal_arr_ncolumns", &fNColumns, kInt},
    {"_cal_arr_front_x", &fXFront, kDouble},
    {"_cal_arr_front_y", &fYFront, kDouble},
    {"_cal_arr_front_z", &fZFront, kDouble},
    {"_cal_arr_xstep", &fXStep, kDouble},
    {"_cal_arr_ystep", &fYStep, kDouble},
    {"_cal_arr_zsize", &fZSize, kDouble},
    {"_cal_using_fadc", &fUsingFADC, kInt, 0, 1},
    {"_cal_clustering", &fClustMethod, kInt, 0, 1},
    //    {"_cal_arr_ADCMode", &fADCMode, kInt, 0, 1},
    {"_cal_arr_adc_tdc_offset", &fAdcTdcOffset, kDouble, 0, 1},
    {"_cal_arr_AdcThreshold", &fAdcThreshold, kDouble, 0, 1},
    {"_cal_arr_adc_samp_threshold", &fAdcSampThreshold, kDouble, 0, 1},
    {"_cal_arr_adc_peak_samp_width", &fDataSampWidth, kDouble, 0, 1},
    {"_cal_ped_sample_low", &fPedSampLow, kInt, 0, 1},
    {"_cal_ped_sample_high", &fPedSampHigh, kInt, 0, 1},
    {"_cal_data_sample_low", &fDataSampLow, kInt, 0, 1},
    {"_cal_data_sample_high", &fDataSampHigh, kInt, 0, 1},
    {"_cal_debug_adc", &fDebugAdc, kInt, 0, 1},
    {"_stat_cermin", &fStatCerMin, kDouble, 0, 1},
    {"_stat_slop_array", &fStatSlop, kDouble, 0, 1},
    {"_stat_maxchisq", &fStatMaxChi2, kDouble, 0, 1},
    {0}
  };

  fClustMethod = 0;  //Set default clusterin method to HMS/SHMS original 
  fDebugAdc = 0;  // Set ADC debug parameter to false unless set in parameter file
  //  fADCMode=kADCDynamicPedestal;
  fAdcTdcOffset=0.0;
  fAdcThreshold=0.;
  fAdcSampThreshold=30;		// Peak found if this amount above pedestal
  fDataSampWidth=15;		// Integrate this # of samples to get peak area

  gHcParms->LoadParmValues((DBRequest*)&list, fKwPrefix.c_str());

  fNelem = fNRows*fNColumns;

  fXPos = new Double_t* [fNRows];
  fYPos = new Double_t* [fNRows];
  fZPos = new Double_t* [fNRows];
  for (UInt_t i=0; i<fNRows; i++) {
    fXPos[i] = new Double_t [fNColumns];
    fYPos[i] = new Double_t [fNColumns];
    fZPos[i] = new Double_t [fNColumns];
  }

  //Looking to the front, the numbering goes from left to right, and from bottom
  //to top.

  for (UInt_t j=0; j<fNColumns; j++)
    for (UInt_t i=0; i<fNRows; i++) {
      fXPos[i][j] = fXFront - (fNColumns-1)*fXStep/2 + fXStep*j;
      fYPos[i][j] = fYFront - (fNRows-1)*fYStep/2 + fYStep*i;
      fZPos[i][j] = fZFront ;
    }

  fOrigin.SetXYZ(fXFront, fYFront, fZFront);

  // Debug output.

  fParent = GetParent();

  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSArray::ReadDatabase for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "  Layer #" << fLayerNum << ", number of elements " << dec << fNelem
	 << endl;
    cout << "  Columns " << fNColumns << ", Rows " << fNRows << endl;

    cout << "Front X, Y Z: " << fXFront << ", " << fYFront << ", " << fZFront
	 << " cm" << endl;

    cout << "  Block to block X and Y distances: " << fXStep << ", " << fYStep
	 << " cm" << endl;

    cout << "  Block size along Z: " << fZSize << " cm" << endl;

    cout << "Block X coordinates:" << endl;
    for (UInt_t i=0; i<fNRows; i++) {
      for (UInt_t j=0; j<fNColumns; j++) {
	cout << fXPos[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << "Block Y coordinates:" << endl;
    for (UInt_t i=0; i<fNRows; i++) {
      for (UInt_t j=0; j<fNColumns; j++) {
    	cout << fYPos[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << "Block Z coordinates:" << endl;
    for (UInt_t i=0; i<fNRows; i++) {
      for (UInt_t j=0; j<fNColumns; j++) {
    	cout << fZPos[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << "  Origin of Array:" << endl;
    cout << "    Xorig = " << GetOrigin().X() << endl;
    cout << "    Yorig = " << GetOrigin().Y() << endl;
    cout << "    Zorig = " << GetOrigin().Z() << endl;
    cout << endl;

    cout << "  Using FADC " << fUsingFADC << endl;
    if (fUsingFADC) {
      cout << "  FADC pedestal sample low = " << fPedSampLow << ",  high = "
	   << fPedSampHigh << endl;
      cout << "  FADC data sample low = " << fDataSampLow << ",  high = "
	   << fDataSampHigh << endl;
    }

  }

  // Here read the 2-D arrays of pedestals, gains, etc.

  // Pedestal limits per channel.
  fPedLimit = new Int_t [fNelem];

  Double_t cal_arr_cal_const[fNelem];
  Double_t cal_arr_gain_cor[fNelem];

  fAdcTimeWindowMin = new Double_t [fNelem];
  fAdcTimeWindowMax = new Double_t [fNelem];

  fAdcPulseTimeMin = new Double_t [fNelem];
  fAdcPulseTimeMax = new Double_t [fNelem];
  fPedDefault = new Int_t [fNelem];
  
  DBRequest list1[]={
    {"_cal_arr_ped_limit", fPedLimit, kInt, static_cast<UInt_t>(fNelem),1},
    {"_cal_arr_cal_const", cal_arr_cal_const, kDouble, static_cast<UInt_t>(fNelem)},
    {"_cal_arr_gain_cor",  cal_arr_gain_cor,  kDouble, static_cast<UInt_t>(fNelem)},
    {"_cal_arr_AdcTimeWindowMin", fAdcTimeWindowMin, kDouble, static_cast<UInt_t>(fNelem),1},
    {"_cal_arr_AdcTimeWindowMax", fAdcTimeWindowMax, kDouble, static_cast<UInt_t>(fNelem),1},
    {"_cal_arr_AdcPulseTimeMin", fAdcPulseTimeMin, kDouble, static_cast<UInt_t>(fNelem),1},
    {"_cal_arr_AdcPulseTimeMax", fAdcPulseTimeMax, kDouble, static_cast<UInt_t>(fNelem),1},
    // DJH 14 Sep 22
    {"_cal_arr_PedDefault", fPedDefault, kInt, static_cast<UInt_t>(fNelem),1},
    {"_cal_arr_SampThreshold",     &fSampThreshold,       kDouble,0,1},
    {"_cal_arr_SampNSA",     &fSampNSA,       kInt,0,1},
    {"_cal_arr_SampNSAT",     &fSampNSAT,       kInt,0,1},
    {"_cal_arr_SampNSB",     &fSampNSB,       kInt,0,1},
    {"_cal_arr_OutputSampWaveform",     &fOutputSampWaveform,       kInt,0,1},
    {"_cal_arr_OutputSampRawWaveform",     &fOutputSampRawWaveform,       kInt,0,1},
    {"_cal_arr_UseSampWaveform",     &fUseSampWaveform,       kInt,0,1},
    {0}
  };

   for(Int_t ip=0;ip<fNelem;ip++) {
    fAdcTimeWindowMin[ip] = -1000.;
    fAdcTimeWindowMax[ip] = 1000.;
    fAdcPulseTimeMin[ip] = -1000.;
    fAdcPulseTimeMax[ip] = 1000.;
    fPedDefault[ip] = 0;
   }

   // DJH 14 Sep 22
   fSampThreshold = 5.;
   fSampNSA = 0; // use value stored in event 125 info
   fSampNSB = 0; // use value stored in event 125 info
   fSampNSAT = 2; // default value in THcRawHit::SetF250Params
   cout << " fSampThreshold 1 = " << fSampThreshold << endl;
   fOutputSampWaveform = 1; // 0= no output , 1 = output Sample Waveform
   fOutputSampRawWaveform = 0; // 0= output pedestal subtracted (mV) , 1 = output Sample Raw Waveform (ADC chan)
   fUseSampWaveform = 1; // 0= do not use , 1 = use Sample Waveform
   
   gHcParms->LoadParmValues((DBRequest*)&list1, fKwPrefix.c_str());
  
  // Debug output.
  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_init_cal) {

    cout << "  fPedLimit:" << endl;
    Int_t el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << fPedLimit[el++] << " ";
      };
      cout <<  endl;
    };

    cout << "  cal_arr_cal_const:" << endl;
    el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << cal_arr_cal_const[el++] << " ";
      };
      cout <<  endl;
    };

    cout << "  cal_arr_gain_cor:" << endl;
    el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << cal_arr_gain_cor[el++] << " ";
      };
      cout <<  endl;
    };

  }    // end of debug output

  // Calibration constants (GeV / ADC channel).
  fGain = new Double_t [fNelem];
  for (Int_t i=0; i<fNelem; i++) {
    fGain[i] = cal_arr_cal_const[i] *  cal_arr_gain_cor[i];
  }

  // Debug output.
  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_init_cal) {

    cout << "  fGain:" << endl;
    Int_t el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << fGain[el++] << " ";
      };
      cout <<  endl;
    };

  }

  fMinPeds = static_cast<THcNPSCalorimeter*>(fParent)->GetMinPeds();

  InitializePedestals();

  // Event by event amplitude and pedestal
  //fA = new Double_t[fNelem];
  //fP = new Double_t[fNelem];
  //fA_p = new Double_t[fNelem];

  fE                       = vector<Double_t> (fNelem, 0.0);
  fNumGoodAdcHits          = vector<Int_t>    (fNelem, 0.0);
  fGoodAdcPulseIntRaw      = vector<Double_t> (fNelem, 0.0);
  fGoodAdcPed              = vector<Double_t> (fNelem, 0.0);
  fGoodAdcMult             = vector<Double_t> (fNelem, 0.0);
  fGoodAdcPulseInt         = vector<Double_t> (fNelem, 0.0);
  fGoodAdcPulseAmp         = vector<Double_t> (fNelem, 0.0);
  fGoodAdcPulseTime        = vector<Double_t> (fNelem, 0.0);
  fGoodAdcTdcDiffTime      = vector<Double_t> (fNelem, 0.0);

  /*
  //C.Y. Mar 16, 2021: Add "Good Variables" but in 2D vectors (to keep track of
  //the total number of hits for each block element (this is to be used by actual NPS cluster
  //algorithm, in which all  possible good hits will be used).
  //set vector of vectors size to be the total number of blocks, fNelem
  fE_2D.resize(fNelem);                 
  fNumGoodAdcHits_2D.resize(fNelem);    
  fGoodAdcPulseIntRaw_2D.resize(fNelem);                         
  fGoodAdcPed_2D.resize(fNelem);        
  fGoodAdcMult_2D.resize(fNelem);       
  fGoodAdcPulseInt_2D.resize(fNelem);   
  fGoodAdcPulseAmp_2D.resize(fNelem);   
  fGoodAdcPulseTime_2D.resize(fNelem);  
  fGoodAdcTdcDiffTime_2D.resize(fNelem);
                         
  //--------------------------------------
  */
    
  fBlock_ClusterID = new Int_t[fNelem];

  // Energy depositions per block.

  //fE = new Double_t[fNelem];

  // Numbers of tracks and hits , for efficiency calculations.
  
  fStatNumTrk = vector<Int_t> (fNelem, 0);
  fStatNumHit = vector<Int_t> (fNelem, 0);
  fTotStatNumTrk = 0;
  fTotStatNumHit = 0;

#ifdef HITPIC
  hitpic = new char*[fNRows];
  for(Int_t row=0;row<fNRows;row++) {
    hitpic[row] = new char[NPERLINE*(fNColumns+1)+2];
  }
  piccolumn=0;
#endif

  // Debug output.

  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_init_cal) {

    cout << "  fMinPeds = " << fMinPeds << endl;

    cout << "---------------------------------------------------------------\n";
  }

  fADCMode=static_cast<THcNPSCalorimeter*>(fParent)->GetADCMode();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcNPSArray::DefineVariables( EMode mode )
{
  // Initialize global variables

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register counters for efficiency calculations in gHcParms so that the
  // variables can be used in end of run reports.

  gHcParms->Define(Form("%sstat_trksum_array", fParent->GetPrefix()),
		   "Number of tracks in calo. array", fTotStatNumTrk);
  gHcParms->Define(Form("%sstat_hitsum_array", fParent->GetPrefix()),
		   "Number of hits in calo. array", fTotStatNumHit);

  // Register variables in global list
  if (fDebugAdc) {
    RVarDef vars[] = {
      {"adcPedRaw",       "List of raw ADC pedestals",         "frAdcPedRaw.THcSignalHit.GetData()"},
      {"adcPulseIntRaw",  "List of raw ADC pulse integrals.",  "frAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"adcPulseAmpRaw",  "List of raw ADC pulse amplitudes.", "frAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"adcPulseTimeRaw", "List of raw ADC pulse times.",      "frAdcPulseTimeRaw.THcSignalHit.GetData()"},

      {"adcPed",          "List of ADC pedestals",             "frAdcPed.THcSignalHit.GetData()"},
      {"adcPulseInt",     "List of ADC pulse integrals.",      "frAdcPulseInt.THcSignalHit.GetData()"},
      {"adcPulseAmp",     "List of ADC pulse amplitudes.",     "frAdcPulseAmp.THcSignalHit.GetData()"},
      {"adcPulseTime",    "List of ADC pulse times.",          "frAdcPulseTime.THcSignalHit.GetData()"},

      // DJH 14 Sep 22
      {"adcSampPedRaw",       "Raw ADCSAMP pedestals",         "frAdcSampPedRaw.THcSignalHit.GetData()"},
      {"adcSampPulseIntRaw",  "Raw ADCSAMP pulse integrals",   "frAdcSampPulseIntRaw.THcSignalHit.GetData()"},
      {"adcSampPulseAmpRaw",  "Raw ADCSAMP pulse amplitudes",  "frAdcSampPulseAmpRaw.THcSignalHit.GetData()"},
      {"adcSampPulseTimeRaw", "Raw ADCSAMP pulse times",       "frAdcSampPulseTimeRaw.THcSignalHit.GetData()"},
      {"adcSampPed",          "ADCSAMP pedestals",             "frAdcSampPed.THcSignalHit.GetData()"},
      {"adcSampPulseInt",     "ADCSAMP pulse integrals",       "frAdcSampPulseInt.THcSignalHit.GetData()"},
      {"adcSampPulseAmp",     "ADCSAMP pulse amplitudes",      "frAdcSampPulseAmp.THcSignalHit.GetData()"},
      {"adcSampPulseTime",    "ADCSAMP pulse times",           "frAdcSampPulseTime.THcSignalHit.GetData()"},

      { 0 }
    };
    DefineVarsFromList( vars, mode);
  } //end debug statement

  // DJH 14 Sep 22
  if (fOutputSampWaveform==1) {
  RVarDef vars[] = {
    {"adcSampWaveform",          "FADC Sample Waveform",           "fSampWaveform"},
      { 0 }
    };
    DefineVarsFromList( vars, mode);
  }

  RVarDef vars[] = {
    //{"adchits", "List of ADC hits", "fADCHits.THcSignalHit.GetPaddleNumber()"}, // appears an empty histogram in the root file

    {"adcErrorFlag",       "Error Flag When FPGA Fails",      "frAdcErrorFlag.THcSignalHit.GetData()"},

    {"adcCounter",      "List of ADC counter numbers.",      "frAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},  //raw occupancy
    {"numGoodAdcHits", "Number of Good ADC Hits per PMT", "fNumGoodAdcHits" },                                   //good occupancy

    {"totNumAdcHits", "Total Number of ADC Hits", "fTotNumAdcHits" },                                            // raw multiplicity
    {"totNumGoodAdcHits", "Total Number of Good ADC Hits", "fTotNumGoodAdcHits" },                               // good multiplicity


    {"goodAdcPulseIntRaw", "Good Raw ADC Pulse Integrals", "fGoodAdcPulseIntRaw"},    //this is defined as pulseIntRaw, NOT ADC Amplitude in FillADC_DynamicPedestal() method
    {"goodAdcPed", "Good ADC Pedestals", "fGoodAdcPed"},
    {"goodAdcMult", "Good ADC Multiplicity", "fGoodAdcMult"},
    {"goodAdcPulseInt", "Good ADC Pulse Integrals", "fGoodAdcPulseInt"},     //this is defined as pulseInt, which is the pedestal subtracted pulse integrals, and is defined if threshold is passed
    {"goodAdcPulseAmp", "Good ADC Pulse Amplitudes", "fGoodAdcPulseAmp"},
    {"goodAdcPulseTime", "Good ADC Pulse Times", "fGoodAdcPulseTime"},     //this is defined as pulseInt, which is the pedestal subtracted pulse integrals, and is defined if threshold is passed
    {"goodAdcTdcDiffTime", "Good Hodo Starttime - ADC Pulse Times", "fGoodAdcTdcDiffTime"},     


    {"e", "Energy Depositions per block", "fE"},       //defined as fE = fA_p*fGain = pulseInt * Gain
    {"earray", "Energy Deposition in Shower Array", "fEarray"},   //defined as a Double_t and represents a sum of the total deposited energy in the shower counter

    {"nclust", "Number of clusters", "fNclust" },       //what is the difference between nclust defined here and that in THcShower.cxx ?
    {"block_clusterID", "Cluster ID number", "fBlock_ClusterID"}, // im NOT very clear about this. it is histogrammed at wither -1 or 0.
    {"ntracks", "Number of shower tracks", "fNtracks" }, //number of cluster-to-track associations


    { 0 }
  };

  return DefineVarsFromList(vars, mode );
}

//_____________________________________________________________________________
void THcNPSArray::Clear( Option_t* )
{
  // Clears the hit lists
  fADCHits->Clear();
  
  fTotNumAdcHits = 0;
  fTotNumGoodAdcHits = 0;
  fNclust = 0;
  fClustSize = 0;
  fNtracks = 0;
  fMatchClX = -1000.;
  fMatchClY = -1000.;
  fMatchClMaxEnergyBlock = -1000.;

  
  for (THcNPSShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    Podd::DeleteContainer(**i);
    delete *i;
  }
  fClusterList->clear();

  frAdcPedRaw->Clear();
  frAdcErrorFlag->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();
  frAdcPulseTime->Clear();

  // DJH 14 Sep 22
  frAdcSampPedRaw->Clear();
  frAdcSampPulseIntRaw->Clear();
  frAdcSampPulseAmpRaw->Clear();
  frAdcSampPulseTimeRaw->Clear();
  frAdcSampPed->Clear();
  frAdcSampPulseInt->Clear();
  frAdcSampPulseAmp->Clear();
  frAdcSampPulseTime->Clear();

  for (UInt_t ielem = 0; ielem < fGoodAdcPed.size(); ielem++) {
    fGoodAdcPulseIntRaw.at(ielem)      = 0.0;
    fGoodAdcPed.at(ielem)              = 0.0;
    fGoodAdcMult.at(ielem)             = 0.0;
    fGoodAdcPulseInt.at(ielem)         = 0.0;
    fGoodAdcPulseAmp.at(ielem)         = 0.0;
    fGoodAdcPulseTime.at(ielem)        = kBig;  //original (use only if considering single hit per event per block)
    fGoodAdcTdcDiffTime.at(ielem)      = kBig;
    fNumGoodAdcHits.at(ielem)          = 0.0;
    fE.at(ielem)                       = 0.0;    
  }
  /*
  for (UInt_t ielem = 0; ielem < fE_2D.size(); ielem++) {
    fGoodAdcPulseIntRaw_2D[ielem].clear();                         
    fGoodAdcPed_2D[ielem].clear();        
    fGoodAdcMult_2D[ielem].clear();       
    fGoodAdcPulseInt_2D[ielem].clear();   
    fGoodAdcPulseAmp_2D[ielem].clear();   
    fGoodAdcPulseTime_2D[ielem].clear();  
    fGoodAdcTdcDiffTime_2D[ielem].clear();
    fNumGoodAdcHits_2D[ielem].clear();    
    fE_2D[ielem].clear();                 
  }
  */
}

//_____________________________________________________________________________
Int_t THcNPSArray::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead

  return 0;
}

//_____________________________________________________________________________
Int_t THcNPSArray::CoarseProcess(TClonesArray& tracks)
{
  
  //C.Y. Feb 07 2021: The tracks input on this method is irrelevant as NPS does NOT use tracks (should eventually be taken out)
  
  // Fill set of unclustered shower array hits.
  // Reuse hit class pertained to the HMS/SOS type calorimeters.
  // Save energy deposition in the module as hit mean energy, do not use
  // positive and negative side energies.
  
  THcNPSShowerHitSet HitSet;         //set of hits
  
  // Check that block id k computed correct for NPS
  UInt_t k=0;
  for(UInt_t j=0; j < fNColumns; j++) {
    for (UInt_t i=0; i<fNRows; i++) {

      if (fGoodAdcPulseInt.at(k) > 0) {    //hit

	THcNPSShowerHit* hit =
	  new THcNPSShowerHit(k, i, j, fXPos[i][j], fYPos[i][j], fZPos[i][j], fE[k], fGoodAdcPulseTime.at(k), fGoodAdcPulseInt.at(k));

	HitSet.insert(hit);
      }

      k++;			// k = j*fNRows + i
    }
  }


  
  //Debug output, print out hits before clustering.
  
  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_clusters_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSArray::CoarseProcess for " << GetName()
	 << endl;
    
    cout << "  List of unclustered hits. Total hits:     " << fTotNumAdcHits << endl;
    THcNPSShowerHitIt it = HitSet.begin();    //<set> version
    for (Int_t i=0; i!=fTotNumGoodAdcHits; i++) {
      cout << "  hit " << i << ": ";
      (*(it++))->show();
    }
  }
  
  ////Sanity check. (Vardan)
  
  // if ((int)HitSet.size() != fTotNumGoodAdcHits) {
  //	cout << "***" << endl;
  //	cout << "*** THcNPSArray::CoarseProcess: HitSet.size = " << HitSet.size()
  //	     << " != fTotNumGoodAdcHits = " << fTotNumGoodAdcHits << endl;
  //	cout << "***" << endl;
  //    }
  
  // Cluster hits and fill list of clusters.
  
  if(fClustMethod==0){  //Original HCANA calorimeter clustering method
    static_cast<THcNPSCalorimeter*>(fParent)->ClusterHits(HitSet, fClusterList);
  }

  if(fClustMethod==1){   //C.Y. Feb 09, 2021:  NPS Clustering using 'Cellular Automata' approach
    static_cast<THcNPSCalorimeter*>(fParent)->ClusterNPS_Hits(HitSet, fClusterList);
  }
  
  assert( HitSet.empty() );  // else bug in ClusterHits()
  fNclust = (*fClusterList).size();         //number of clusters
  
  
  // Set cluster ID for each block
  Int_t ncl=0;
  Int_t block;
  for (THcNPSShowerClusterListIt ppcl = (*fClusterList).begin();
       ppcl != (*fClusterList).end(); ++ppcl) {
    for (THcNPSShowerClusterIt pph=(**ppcl).begin(); pph!=(**ppcl).end();
	   ++pph) {
      //C.Y. Feb 09, 2021: I believe a +1 is not necessary if block numbering starts at zero (simply, "block = column * fNRows + Row" will do)
      block = ((**pph).hitColumn())*fNRows + (**pph).hitRow(); //+1; 
       fBlock_ClusterID[block-1] = ncl;
      }
      ncl++;
    }

  //Debug output, print out clustered hits.

  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_clusters_cal) {

    cout << "  Clustered hits. Number of clusters: " << fNclust << endl;

    UInt_t i = 0;
    for (THcNPSShowerClusterListIt ppcl = (*fClusterList).begin();
	 ppcl != (*fClusterList).end(); ++ppcl) {

      cout << "  Cluster #" << i++
	   <<":  E=" << clE(*ppcl)
	   << "  Epr=" << clEpr(*ppcl)
	   << "  X=" << clX(*ppcl)
	   << "  Y=" << clY(*ppcl)
	   << "  Z=" << clZ(*ppcl)
	   << "  size=" << (**ppcl).size()
	   << endl;

      Int_t j=0;
      for (THcNPSShowerClusterIt pph=(**ppcl).begin(); pph!=(**ppcl).end();
	   ++pph) {
	cout << "  hit " << j++ << ": ";
	(**pph).show();
      }

    }

    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcNPSArray::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcNPSArray::CoarseProcessHits()
{
  // cout<< "THcNPSArray::CoarseProcessHits() " << endl;
  /* This decision really belongs in decode */
  /*  if(fADCMode == kADCDynamicPedestal) {
    FillADC_DynamicPedestal();
  } else if (fADCMode == kADCSampleIntegral) {
    FillADC_SampleIntegral();
  } else if (fADCMode == kADCSampIntDynPed) {
    FillADC_SampIntDynPed();
  } else {
    FillADC_Standard();
    }*/
  FillADC_DynamicPedestal();
  //
  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSArray::ProcessHits for "
    	 << static_cast<THcNPSCalorimeter*>(fParent)->GetPrefix() << ":" << endl;

    cout << "  Sparsified hits for shower array, plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;
    Int_t k=0;
    for(UInt_t j=0; j < fNColumns; j++) {
    for (UInt_t i=0; i<fNRows; i++) {
      if(fGoodAdcPulseIntRaw.at(k) > fThresh[k]) {
	cout << "  counter =  " << k
	     << "  E = " << fE[k]
	     << endl;
	nspar++;
      }
       k++;
    }
    }

    if (nspar == 0) cout << "  No hits\n";

    cout << "  Earray = " << fEarray << endl;
    cout << "---------------------------------------------------------------\n";
  }
  //
  return 1;
}
//_____________________________________________________________________________
void THcNPSArray::FillADC_SampIntDynPed()
{
  /* For each hit noted by the ADC, get the sample data, analyze it, use the
     information from that analysis instead of the pulse data.
     How can we tell decoder to send us all the sample data?
  */
}

//_____________________________________________________________________________
void THcNPSArray::FillADC_SampleIntegral()
{
  ///			adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[hit->fCounter -1];
  //			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[hit->fCounter -1];
  //			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // need to create
}
//_____________________________________________________________________________
void THcNPSArray::FillADC_Standard()
{
}
//_____________________________________________________________________________
void THcNPSArray::FillADC_DynamicPedestal()
{
  //C.Y. Feb 07 2021: Eventually, we want to re-define the start time, since NPS does not have hodoscopes. Maybe we can chose
  //another time as reference?  How about the RF time?  Something to be discussed w/ M. Jones and S. Wood
  Double_t StartTime = 60.0; 	// Arbitrary time to put tdc in range

  //cout << "THcNPSArray::FillADC_DynamicPed | total # of  hits: " << frAdcPulseInt->GetEntries() << endl;
  //if( fglHod ) StartTime = fglHod->GetStartTime();
  
  //Loop over all entries (all hits over all elements (i.e., all hits of all blocks))
  for (Int_t ielem=0;ielem<frAdcPulseInt->GetEntries();ielem++) {  
    //npad is block # of the corresponding ielem (hit)
    Int_t npad           = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber(); 
    Double_t pulseIntRaw = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulsePed    = ((THcSignalHit*) frAdcPed->ConstructedAt(ielem))->GetData();
    Double_t pulseInt    = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp    = ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(ielem))->GetData();
    Double_t pulseTime   = ((THcSignalHit*) frAdcPulseTime->ConstructedAt(ielem))->GetData();
    Double_t adctdcdiffTime = StartTime-pulseTime;
    Bool_t errorflag     = ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(ielem))->GetData();
    Bool_t pulseTimeCut  = (adctdcdiffTime > fAdcTimeWindowMin[npad]) &&  (adctdcdiffTime < fAdcTimeWindowMax[npad]);

    
    if (!errorflag)
      {
	fGoodAdcMult.at(npad) += 1;
      }

    if (!errorflag && pulseTimeCut) {
      
      fTotNumAdcHits++;
      fGoodAdcPulseIntRaw.at(npad) = pulseIntRaw;

      //store multiple raw pulse integral per paddle (in the case of >1 hits)
      //fGoodAdcPulseIntRaw_2D[npad].push_back(pulseIntRaw);

      
      //cout << "FADP " << npad << " " << fGoodAdcPulseIntRaw.at(npad) << " " << fThresh[npad] << " " << pulseInt <<  endl;
      /* Is this basically always true?  pulseInt is large (includes pedestals), fThresh is small
	 Why is fThresh small? In pulse mode, (1), fThresh is OK, but Good hists not getting
	 filled. */

      //if(fClustMethod == 0){
	/*C.Y. Mar 15, 2021 : This block of code (below) requires fGoodAdcPulseInt to NOT have been
	  previously filled (==0), so the quantities filled here only take the 1st hit within the 
	  adctdcdiffTime window. This is what is originally done in THcShower.cxx for HMS/SHMS */
	
	
	if(fGoodAdcPulseIntRaw.at(npad) >  fThresh[npad] && fGoodAdcPulseInt.at(npad)==0) {
	  
	  fTotNumGoodAdcHits++;
	  fGoodAdcPulseInt.at(npad) = pulseInt;
	  fE.at(npad) = fGoodAdcPulseInt.at(npad)*fGain[npad];
	  fEarray += fE.at(npad);
	  
	  fGoodAdcPed.at(npad) = pulsePed;
	  fGoodAdcPulseAmp.at(npad) = pulseAmp;
	  fGoodAdcPulseTime.at(npad) = pulseTime;
	  fGoodAdcTdcDiffTime.at(npad) = adctdcdiffTime;
	  
	  fNumGoodAdcHits.at(npad) = npad;// Looks wrong, but is correct. 
	  // See https://github.com/JeffersonLab/hcana/issues/463
	  // Not npad+1 here because we number first block as 0
	  	         
	}
	
	//}
      
      /*
      if(fClustMethod == 1){
	
	//C.Y. Mar 15, 2021 : This block of code (below) only requires raw integral to be above threshold,
	// but all possible hits that are within the adctdcdiffTime are NOT discarded, but are actually
	// considered as "good hits. Will most likely consider all possible good hits for NPS".
	// To store multiple hits separately to analyze later, we actually need a 2D vector to fill all hits per block.
	
	
	
	//multiple hits per npad are considered (and stored in _2D) here
	if(fGoodAdcPulseIntRaw.at(npad) >  fThresh[npad]) {
	  
	  fTotNumGoodAdcHits++;
	  
	  //-------Store possible multiple "good" hits in 2D Vector-----
	  //(Which of these is needed for NPS clustering?)
	  //store multiple hits separate for each [npad]
	  fGoodAdcPulseInt_2D[npad].push_back(pulseInt);
	  fE_2D[npad].push_back(pulseInt*fGain[npad]);
	  fGoodAdcPed_2D[npad].push_back(pulsePed);
	  fGoodAdcPulseAmp_2D[npad].push_back(pulseAmp);
	  fGoodAdcPulseTime_2D[npad].push_back(pulseTime);
	  fGoodAdcTdcDiffTime_2D[npad].push_back(adctdcdiffTime);
	  fNumGoodAdcHits_2D[npad].push_back(npad);
	  
	}
      }
      */
      
      
    }

  }

  /*
  if(fClustMethod == 1){

    //=====TESTING: SELECT HIT WITH HIGHEST PULESE INTEGRAL AS "GOOD HIT"====

    //Loop over all blocks
    for(Int_t i=0; i<fNelem; i++){
      
      //check if there is at least a good hit
      if(fGoodAdcPulseInt_2D[i].size()>0){
	
	//Get the index of the hit with the maximum pulse integral (and select as good hit)
	int maxIdx = std::max_element(fGoodAdcPulseInt_2D[i].begin(), fGoodAdcPulseInt_2D[i].end()) - fGoodAdcPulseInt_2D[i].begin();
	float maxElement = *std::max_element(fGoodAdcPulseInt_2D[i].begin(), fGoodAdcPulseInt_2D[i].end());
	
	
	//Assign the 1D vectors the corresponding value to the hit with the max pulse Int within the adctdcdiffTime window
	fGoodAdcPulseIntRaw.at(i) = fGoodAdcPulseIntRaw_2D[i].at(maxIdx);
	fGoodAdcPulseInt.at(i)    = fGoodAdcPulseInt_2D[i].at(maxIdx);
	fE.at(i)                  = fE_2D[i].at(maxIdx);
	fEarray                  += fE.at(i);
	
	fGoodAdcPed.at(i)         = fGoodAdcPed_2D[i].at(maxIdx);
	fGoodAdcPulseAmp.at(i)    = fGoodAdcPulseAmp_2D[i].at(maxIdx);
	fGoodAdcPulseTime.at(i)   = fGoodAdcPulseTime_2D[i].at(maxIdx);
	fGoodAdcTdcDiffTime.at(i) = fGoodAdcTdcDiffTime_2D[i].at(maxIdx);
	
	fNumGoodAdcHits.at(i)     = fNumGoodAdcHits_2D[i].at(maxIdx);
	
      }
      
    }
  
  }
  */
  
  
  
}
//_____________________________________________________________________________
Int_t THcNPSArray::ClearProcessedHits()
{
  // Extract the data for this layer from hit list.

  //THcNPSCalorimeter fParent;
  //fParent = (THcNPSCalorimeter) GetParent();

  // Initialize variables.

  fADCHits->Clear();

  frAdcPedRaw->Clear();
  frAdcErrorFlag->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();
  frAdcPulseTime->Clear();

  for(Int_t i=0;i<fNelem;i++) {
    //fA[i] = 0;
    //fA_p[i] = 0;
    //fE[i] = 0;

    fBlock_ClusterID[i] = -1;
  }

  fEarray = 0;
  return(0);
}
//_____________________________________________________________________________
Int_t THcNPSArray::AccumulateHits(TClonesArray* rawhits, Int_t nexthit, Int_t transform)
{
  // cout << "Calling THcNPSArray::AccumulateHits() . . ." << endl;
  // Extract the data for this layer from hit list without clearing.
  // accumulating for several events (to simulate high rate)
  // If transform is 1-3 or 5-7, transform the block numbers to other quadrants
  // 1-3 is a translation transformation, 5-7 is a reflection transformation

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;
  
  UInt_t nrAdcHits = 0;
  UInt_t nrSampAdcHits = 0;
  fSampWaveform.clear();

  //cout << "THcNPSArray::AccumulateHits | # of blocks hit: " << nrawhits << endl;
  //Loop over each block hit (nrawhits is total number of blocks hit)

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    if(hit->fPlane != fLayerNum) {
      break;
    }

    Int_t padnum = hit->fCounter;

    //    Int_t padnum = shms2nps_transform(hit->fCounter, transform);
    
    //Create rawAdcHit object (passed by ref.) to access raw ADC quantities, and Fill the frAdc* TClonesArray
    THcRawAdcHit& rawAdcHit = hit->GetRawAdcHitPos();

    if ( fUseSampWaveform == 0 ) {
      for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {

	((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPedRaw());
        fThresh[padnum-1]=rawAdcHit.GetPedRaw()*rawAdcHit.GetF250_PeakPedestalRatio()+fAdcThreshold;
	((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPed());
	
	((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseIntRaw(thit));
	((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseInt(thit));
	
	((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmpRaw(thit));
	((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmp(thit));
	
	((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTimeRaw(thit));
	((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTime(thit)+fAdcTdcOffset);

	if (rawAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 0);
	if (rawAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 1);
	if (rawAdcHit.GetPulseAmpRaw(thit) <= 0 && rawAdcHit.GetNSamples() >0) ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 2);
	
	if (rawAdcHit.GetPulseAmpRaw(thit) <= 0) {
	  Double_t PeakPedRatio= rawAdcHit.GetF250_PeakPedestalRatio();
	  Int_t NPedSamples= rawAdcHit.GetF250_NPedestalSamples();
	  Double_t AdcToC =  rawAdcHit.GetAdcTopC();
	  Double_t AdcToV =  rawAdcHit.GetAdcTomV();
	  if (fPedDefault[padnum-1] !=0) {
	    Double_t tPulseInt = AdcToC*(rawAdcHit.GetPulseIntRaw(thit) - fPedDefault[padnum-1]*PeakPedRatio);
	    ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, tPulseInt);
	    ((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, fPedDefault[padnum-1]);
	    ((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, float(fPedDefault[padnum-1])/float(NPedSamples)*AdcToV);
	    
	  }
	  ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, 0.);
	}

	++nrAdcHits;
	fTotNumAdcHits++;
      }
    }
    
  if (rawAdcHit.GetNSamples() >0 ) {   

    rawAdcHit.SetSampThreshold(fSampThreshold);
    if (fSampNSA == 0) fSampNSA=rawAdcHit.GetF250_NSA();
    if (fSampNSB == 0) fSampNSB=rawAdcHit.GetF250_NSB();
    rawAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
    if (fSampNSAT != 2) rawAdcHit.SetSampNSAT(fSampNSAT);
    rawAdcHit.SetSampIntTimePedestalPeak();
    fSampWaveform.push_back(float(padnum));
    fSampWaveform.push_back(float(rawAdcHit.GetNSamples()));

    for (UInt_t thit = 0; thit < rawAdcHit.GetNSamples(); thit++) {
      if (fOutputSampRawWaveform == 1) {
          fSampWaveform.push_back(rawAdcHit.GetSampleRaw(thit)); // raw sample (Adc chan)
       } else {
         fSampWaveform.push_back(rawAdcHit.GetSample(thit)); // ped subtracted sample (mV)
       }
    }
    
    for (UInt_t thit = 0; thit < rawAdcHit.GetNSampPulses(); thit++) {
      ((THcSignalHit*) frAdcSampPedRaw->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPedRaw());
      ((THcSignalHit*) frAdcSampPed->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPed());
      
      ((THcSignalHit*) frAdcSampPulseIntRaw->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseIntRaw(thit));
      ((THcSignalHit*) frAdcSampPulseInt->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseInt(thit));
      
      ((THcSignalHit*) frAdcSampPulseAmpRaw->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseAmpRaw(thit));
      ((THcSignalHit*) frAdcSampPulseAmp->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseAmp(thit));
      ((THcSignalHit*) frAdcSampPulseTimeRaw->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseTimeRaw(thit));
      ((THcSignalHit*) frAdcSampPulseTime->ConstructedAt(nrSampAdcHits))->Set(padnum, rawAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);
      //
      if ( rawAdcHit.GetNPulses() == 0 || fUseSampWaveform ==1 ) {
	((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetSampPedRaw());
	((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetSampPed());
	
	((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum,rawAdcHit.GetSampPulseIntRaw(thit));
	((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum,rawAdcHit.GetSampPulseInt(thit));
	
	((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum,rawAdcHit.GetSampPulseAmpRaw(thit));
	((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum,rawAdcHit.GetSampPulseAmp(thit) );
	
	((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum,rawAdcHit.GetSampPulseTimeRaw(thit) );
	((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);

	((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 3);  
	if (fUseSampWaveform ==1) ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum, 0);  
	++nrAdcHits;
	fTotNumAdcHits++;
      }
      ++nrSampAdcHits;
      }	
    }							      
  ihit++;
  }

  // DJH 09/04/22 -- commented out for now

//     //    cout << "fADCMode=" << fADCMode << endl;
//     if(fADCMode == kADCSampIntDynPed) {
//       /* Assume channels from fPedSampLow to fPedSampHigh are pedestal */
//       Int_t nsamples = rawAdcHit.GetNSamples();
//       if( nsamples <= 0 ) {
// 	cout << "No sample data when expected.  Falling back to pulse mode." << endl;
// 	fADCMode = kADCDynamicPedestal;
//       } else {
// 	Int_t pedsum=0;
// 	Double_t pedestal=0.0;
// 	for(Int_t i=fPedSampLow;i<=fPedSampHigh;i++) {
// 	  pedsum += rawAdcHit.GetSampleRaw(i);
// 	}
// 	pedestal = ((Double_t) pedsum)/(fPedSampHigh+1-fPedSampLow);
// 	//	cout << "PAD " << padnum << " Pedestal: " << pedestal << "  Threshold: " << fAdcSampThreshold << endl;
// 	//	for(Int_t i=0;i<nsamples;i++) {
// 	//	  cout << rawAdcHit.GetSampleRaw(i) << " ";
// 	//	}
// 	//	cout << endl;
// 	Int_t i = fPedSampHigh+1;
// 	while(i<nsamples) {
// 	  Int_t sampfirst = rawAdcHit.GetSampleRaw(i);
// 	  if(sampfirst >= pedestal+fAdcSampThreshold) {
// 	    Int_t lastchan = TMath::Min(i+fDataSampWidth-1,nsamples-1);
// 	    Int_t integralraw=sampfirst;
// 	    Double_t integral=sampfirst-pedestal;
// 	    Int_t sampmax=sampfirst;
// 	    Int_t chanmax=i;
// 	    Double_t rawtime = i*4/0.0625;
// 	    for(Int_t j=i+1;j<=lastchan;j++) {
// 	      Int_t sample = rawAdcHit.GetSampleRaw(j);
// 	      if(sample > sampmax) {
// 		sampmax = sample;
// 		chanmax = j;
// 	      }
// 	      integralraw += sample;
// 	      integral += sample-pedestal;
// 	    }
// 	    if(chanmax>i) {		// Update time calculation
// 	      // Interpolate between first sample and max sample to find where
// 	      // amplitude reaches half the maximum.
// 	      // This is what I presume calculation in FADC250 to be
// 	      //		cout << "Rawtime: " << rawtime/64.0 << " ";
// 	      rawtime += 0.5*(chanmax-i)*(1-(sampfirst-pedestal)/(sampmax-sampfirst))*4/0.0625;
// 	      //		cout << rawtime/64.0 << endl;
// 	    }
// 	    // Do something
// 	    ((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, pedsum);
// 	    ((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, pedestal*rawAdcHit.GetAdcTomV());
// 	    ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum, integralraw);
// 	    ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, integral*rawAdcHit.GetAdcTopC());
// 	    //	    fThresh[padnum] = xxx
// 	    // Need to check how it gets peak.  Is pedestal subtracted?  No.
// 	    ((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum, sampmax);
// 	    ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, (sampmax-pedestal)*rawAdcHit.GetAdcTomV());
// 	    ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawtime);
// 	    Double_t time = (rawtime - rawAdcHit.GetRefTime())*rawAdcHit.GetAdcTons()+fAdcTdcOffset;
// 	    //	    ((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->
// 	    //	      Set(padnum, (rawtime - rawAdcHit.GetRefTime())*rawAdcHit.GetAdcTons()+fAdcTdcOffset);
// 	    ((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->
// 	      Set(padnum, time);
// 	    if (sampmax-pedestal>0&&integralraw>0) {
// 	      ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,0);
// 	    } else {
// 	      ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,1);
// 	    }
// 	    ++nrAdcHits;	    
// 	    Int_t j = lastchan+1;
// 	    while(j<nsamples) {	// Skip ahead until we are below threshold
// 	      Int_t sample = rawAdcHit.GetSampleRaw(j);
// 	      j++;
// 	      if(sample < pedestal+fAdcSampThreshold) {
// 		break;
// 	      }
// 	    }
// 	    i = j;
// 	    //	    cout << "Resuming peak search at " << i << endl;
// 	  } else {
// 	    i++;
// 	  }
// 	}
//       }
//     }
//     //#define DEBUG112 1
// #ifdef DEBUG112
//     if(padnum == 112) {
//       for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
// 	Double_t rawtime = rawAdcHit.GetPulseTimeRaw(thit);
// 	Double_t time = rawAdcHit.GetPulseTime(thit)+fAdcTdcOffset;
// 	cout << "Pulse: " << thit << " " << time << " " << rawtime << " " << fAdcTdcOffset <<
// 	  " R" << rawAdcHit.GetRefTime() << endl;
//       }
//     }
// #endif    
//     if(fADCMode != kADCSampIntDynPed) {
//       //cout << "Total # of  NPulses ------>" << rawAdcHit.GetNPulses() << endl;

      
//       //Loop over total number of pulses per block hit
//       for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
	
// 	((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPedRaw());
// 	//	cout << "pedRaw " << rawAdcHit.GetPedRaw() << endl;
// 	fThresh[padnum]=rawAdcHit.GetPedRaw()*rawAdcHit.GetF250_PeakPedestalRatio()+fAdcThreshold;
// 	//	cout << padnum << " " << rawAdcHit.GetPedRaw() << " " << rawAdcHit.GetF250_PeakPedestalRatio()
// 	//	     << " " << fAdcThreshold << " " << fThresh[padnum] << endl;
// 	((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPed());
// 	//	cout << "pedNotRaw " << rawAdcHit.GetPed() << endl;	
// 	((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseIntRaw(thit));
// 	((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseInt(thit));
	
// 	((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmpRaw(thit));
// 	((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmp(thit));
	
// 	((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTimeRaw(thit));
// 	((THcSignalHit*) frAdcPulseTime->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTime(thit)+fAdcTdcOffset);

// 	if (rawAdcHit.GetPulseAmp(thit)>0&&rawAdcHit.GetPulseIntRaw(thit)>0) {
// 	  ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,0);
// 	} else {
// 	  ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,1);
// 	}


// 	//increment number of adc hits per block 
// 	++nrAdcHits;


	
//       }//end loop over ADC Pulses per block

      
//     } //end (fADCMode != kADCSampIntDynPed) requirement
    
// #define SAMPLEHACKING 0
// #ifdef SAMPLEHACKING
//     if(rawAdcHit.GetNPulses() > 0) {
//       Int_t NSamples = rawAdcHit.GetNSamples();
//       //      cout << "NSamples = " << NSamples << endl;
//       Double_t *spectrum = new Double_t[NSamples];
//       Double_t *dest = new Double_t[NSamples];
// 	Double_t maxamp=0.0;
//       for(Int_t i=0;i<NSamples;i++) {
// 	spectrum[i] = rawAdcHit.GetSampleRaw(i);
// 	if(spectrum[i] > maxamp) maxamp = spectrum[i];
//       }
      
      // TSpectrum s;
      // // Last sample is zero
      // Int_t npeaks = 1; //s.SearchHighRes(spectrum,dest,NSamples,3.0,30.0,kTRUE,5,kTRUE,3);
      // if(npeaks > 10) {
      // 	Double_t *pos = s.GetPositionX();
      // 	cout << "Nsamples " << NSamples << endl;
      // 	cout << "ADC";
      // 	for(UInt_t i=0;i<rawAdcHit.GetNPulses();i++) {
      // 	  cout << " " << 0.0625*rawAdcHit.GetPulseTimeRaw(i)/4.;
      // 	}
      // 	cout << endl;
      // 	cout << "FIT";
      // 	for(Int_t i=0;i<npeaks;i++) {
      // 	  cout << " " << pos[i];
      // 	}
      // 	if(npeaks==1 && rawAdcHit.GetNPulses() == 1) {
      // 	  cout << "  " << pos[0]-0.0625*rawAdcHit.GetPulseTimeRaw(0)/4.;
      // 	}
      // 	cout << endl;
      // }
      // if(npeaks > 4) {
      // 	for(Int_t i=0;i<NSamples;i++) {
      // 	  cout << i << " " << rawAdcHit.GetSampleRaw(i) << " " << spectrum[i] << endl;
      // 	}
      // }


      //      if(padnum==112) {
      //	cout << "PAD # " << padnum << "  MAX " << maxamp << endl;
      //	for(Int_t i=0;i<NSamples;i++) {
      //	  cout << rawAdcHit.GetSampleRaw(i);
      //	  if((i+1)%25==0) {
      //	    cout << ":";
      //	  }
      //	  cout << " ";
      //	}
      //	cout << endl;
      //      }
	
// }
	
// #endif
//     ihit++;
//   }

#if 0
  if(fTotNumGoodAdcHits > 0) {
    cout << "+";
    for(Int_t column=0;column<fNColumns;column++) {
      cout << "-";
    }
    cout << "+" << endl;
    for(Int_t row=0;row<fNRows;row++) {
      cout << "|";
      for(Int_t column=0;column<fNColumns;column++) {
	Int_t counter = column*fNRows + row;
	if(fGoodAdcPulseIntRaw.at(counter) > threshold) {
	  cout << "X";
	} else {
	  cout << " ";
	}
      }
      cout << "|" << endl;
    }
  }
#endif
#ifdef HITPIC
  if(fTotNumGoodAdcHits > 0) {
    for(Int_t row=0;row<fNRows;row++) {
      if(piccolumn==0) {
	hitpic[row][0] = '|';
      }
      for(Int_t column=0;column<fNColumns;column++) {
	Int_t counter = column*fNRows+row;
	if(fGoodAdcPulseIntRaw.at(counter) > threshold) {
	  hitpic[row][piccolumn*(fNColumns+1)+column+1] = 'X';
	} else {
	  hitpic[row][piccolumn*(fNColumns+1)+column+1] = ' ';
	}
	hitpic[row][(piccolumn+1)*(fNColumns+1)] = '|';
      }
    }
    piccolumn++;
    if(piccolumn==NPERLINE) {
      cout << "+";
      for(Int_t pc=0;pc<NPERLINE;pc++) {
	for(Int_t column=0;column<fNColumns;column++) {
	  cout << "-";
	}
	cout << "+";
      }
      cout << endl;
      for(Int_t row=0;row<fNRows;row++) {
	hitpic[row][(piccolumn+1)*(fNColumns+1)+1] = '\0';
	cout << hitpic[row] << endl;
      }
      piccolumn = 0;
    }
  }
#endif

  //Debug output.


  return(ihit);
}
//_____________________________________________________________________________
Int_t THcNPSArray::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract data for this plane from hit list and accumulate in
  // arrays for subsequent pedestal calculations.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {

    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    if(hit->fPlane != fLayerNum) {
      break;
    }

    Int_t element = hit->fCounter - 1; // Should check if in range
		// Should check that counter # is in range
		Int_t adc = 0;
		if (fUsingFADC) {
			adc = hit->GetRawAdcHitPos().GetData(
				fPedSampLow, fPedSampHigh, fDataSampLow, fDataSampHigh
			);
		}
		else {
			adc = hit->GetData(0);
		}

    if(adc <= fPedLimit[element]) {
      fPedSum[element] += adc;
      fPedSum2[element] += adc*adc;
      fPedCount[element]++;
      if(fPedCount[element] == fMinPeds/5) {
	fPedLimit[element] = 100 + fPedSum[element]/fPedCount[element];
      }
    }
    ihit++;
  }
  fNPedestalEvents++;

  // Debug output.

  if ( static_cast<THcNPSCalorimeter*>(fParent)->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSArray::AcculatePedestals for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "Processed hit list for plane " << GetName() << ":\n";

    for (Int_t ih=nexthit; ih<nrawhits; ih++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ih);

      if(hit->fPlane != fLayerNum) {
	break;
      }

			Int_t adc = 0;
			if (fUsingFADC) {
				adc = hit->GetRawAdcHitPos().GetData(
					fPedSampLow, fPedSampHigh, fDataSampLow, fDataSampHigh
				);
			}
			else {
				adc = hit->GetData(0);
			}

      cout << "  hit " << ih << ":"
	   << "  plane =  " << hit->fPlane
	   << "  counter = " << hit->fCounter
	   << "  ADC = " << adc
	   << endl;
    }

    cout << "---------------------------------------------------------------\n";

  }

  return(ihit);
}

//_____________________________________________________________________________
void THcNPSArray::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals.

  for(Int_t i=0; i<fNelem;i++) {

    fPed[i] = ((Float_t) fPedSum[i]) / TMath::Max(1, fPedCount[i]);
    fSig[i] = sqrt(((Float_t)fPedSum2[i])/TMath::Max(1, fPedCount[i])
		   - fPed[i]*fPed[i]);
    fThresh[i] = fPed[i] + TMath::Min(50., TMath::Max(10., 3.*fSig[i]));

  }

  // Debug output.

  if ( static_cast<THcNPSCalorimeter*>(fParent)->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSArray::CalculatePedestals for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  ADC pedestals and thresholds for calorimeter plane "
	 << GetName() << endl;
    for(Int_t i=0; i<fNelem;i++) {
      cout << "  element " << i << ": "
	   << "  Pedestal = " << fPed[i]
	   << "  threshold = " << fThresh[i]
	   << endl;
    }
    cout << "---------------------------------------------------------------\n";

  }

}
//_____________________________________________________________________________
void THcNPSArray::InitializePedestals( )
{
  fNPedestalEvents = 0;

  fPedSum = new Int_t [fNelem];
  fPedSum2 = new Int_t [fNelem];
  fPedCount = new Int_t [fNelem];

  fSig = new Float_t [fNelem];
  fPed = new Float_t [fNelem];
  fThresh = new Float_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPedSum[i] = 0;
    fPedSum2[i] = 0;
    fPedCount[i] = 0;
  }
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
Int_t THcNPSArray::shms2nps_transform(Int_t padnum, Int_t transform)
{
  /*
    C.Y. Feb 09, 2021
    Brief: Method to transform shms to nps block numbering used when moving data from 
    shms calorimeter to "fake" nps quadrants to mimic NPS. This transformation of block
    numbering is done explicitly in ::AccumulateHits() method.
    This method takes as input: 
    padnum    -> the shms calorimeter block number id (0-223)
    transform -> a integer to describe to which quadrant the block will be moved to.
    if transform(0, 4), data will be translated to one of the four quadrants
    --------------
    transform 0 -> bottom right quadrant
    transform 1 -> bottom left  quadrant
    transform 2 -> top    right quadrant
    transform 3 -> bottom left  quadrant
    --------------
    else if transform(4,8) data will be reflected to one of the four quadrants
  */

  // cout << "Calling  THcNPSArray::shms2nps_transform()  . . ." << endl;

  if(transform < 0) {
    return padnum;
  }
  Int_t column = padnum/(fNRows/2);
  Int_t row = padnum%(fNRows/2);
  if(transform>0 && transform < 4) {
    if(transform%2 == 1) {
      column += fNColumns/2;
    }
    if(transform/2 == 1) {
      row += fNRows/2;
    }
  } else if (transform>4 && transform < 8) {
    if(transform%2 == 1) {
      column = fNColumns/2 - column - 1 ;
    }
    if((transform-4)/2 == 1) {
      row = fNRows/2 - row - 1;
    }
  }


  //C.Y. Feb 15 : Column offsets necessary to get the "translation" mapping of SHMS to NPS blocks correct
  //For checking the transformation is correct, compare SHMS block (MAPS/DETEC/pcal_nps_standard.map) with NPS block ( MAPS/DETEC/pcal_nps_translation.map)
  //transform = 0 (bottom right) and 2 (top right) have offset of 13 
  if(transform==0 || transform==2){
    column = 13-column;
  }
  //transform = 1 (bottom left) and 3 (top left) have offset of 41 
  if(transform==1 || transform==3){
    column = 41-column;
  }
  
  padnum = column*fNRows + row;

  //return padnum (if no transformation is done, i.e., transform = 0, 4, 8),
  //then padnum is the same as input padnum (nothing changes)
  return padnum;
}

//C.Y. Feb 11, 2021 : Introduce various method (or function) necessary for
//the NPS Clustering algorithm

//------------------------------------------------------------------------
std::pair<int, int> THcNPSArray::GetBlockij(Int_t id)
{
  
  //Brief: This method returns the pair (irow, jcol) block indices based on the block id number
  
  //temporary placeholders (ith row, jth colum and block id)
  int itmp, jtmp, id_tmp;
  itmp = jtmp = id_tmp = -1; //defaults to -1 if no match is found
  
  for (UInt_t irow=0; irow<fNRows; irow++){
    for (UInt_t jcol=0; jcol<fNColumns; jcol++){

      id_tmp = jcol * fNRows + irow;

      //check if block id matches input id
      if(id_tmp == id){
	itmp = irow;
	jtmp = jcol;
	break;
      }   
    }
  }
  
  return std::make_pair(itmp, jtmp);
  
}

//------------------------------------------------------------------------
Int_t THcNPSArray::GetBlockID(UInt_t irow, UInt_t jcol)
{
  /*Brief: This method returns the block number id
    given the block (ith row, jth col).
    NOTE: Unlike in SHMS, the block ID and block number
    are the same for NPS, since both start at block 0
   */

  Int_t block_id;
  //  Int_t fNBlocks = fNRows * fNColumns; //total number of blocks (0 to  fNBlocks-1)
  
  //Calculate the block id based on the (i,j) indices
  block_id = jcol * fNRows + irow;

  //Check if input (irow, jcol) is out-of-bounds
  if( irow<0 || irow>=fNRows || jcol<0 || jcol>=fNColumns ){
    block_id = -1;
    return block_id;
  }
  else{
    return block_id;
  }

}

Int_t THcNPSArray::GetNeighbor(UInt_t id, UInt_t k)
{
  /*Brief: This method returns the neighbor block number (id_k) 
    of central block id given the index k of the neighboring block
    id: central block id number
    k : neighboring block index (0,1,2,....7) -> 8 nearest blocks
    If at an edge or corner, then neighboring blocks will be < 8
    
    NPS Block Numberbing Scheme (showing central block and its nearest neighbors) 
    NPS Front View (beam going into screen)
    
    k ->    (0)         (1)         (2)
    ____________________________________       fNRows
    |  (i+1,j+1) | (i+1,j)  | (i+1,j-1)  |        ^
    |____________|__________|____________|        |                            
(7) |   (i,j+1)  |   (i,j)  |   (i,j-1)  | (3)    | i rows (0, fNRows)
    |____________|__________|____________|        | 
    |  (i-1,j+1) |  (i-1,j) |  (i-1,j-1) |     
    --------------------------------------        0
       (6)         (5)         (4)

            fNColumns <--- 0
	    j columns (0, fNColumns )
  */
  
  
  //1) Determine the (i,j) corresponding to central block id
  Int_t i, j;  
  std::tie(i,j) = GetBlockij(id);  //unpack (i,j) pair

  //cout << "1) Central Block: (id, irow, jcol) = " << id << ", " << i << ", " << j << endl; 
  
  //2) Depending on the neighbor block index k (0-7),
  //and using the knowledge of the central block (i,j).
  //obtain the neighboring block (i +/- offset, j +/- offset)
  Int_t ik, jk;
  Int_t ik_offset, jk_offset;
  ik_offset = jk_offset = 1; 
  
  Bool_t shms_cal_map = kFALSE;
  if(shms_cal_map){
    ik_offset = 1, jk_offset = -1;  //reverse the column orientation if SHMS cal map is used for testing
  }
  
  if     (k==0) { ik = i + ik_offset, jk = j + jk_offset; }
  else if(k==1) { ik = i + ik_offset, jk = j;             }
  else if(k==2) { ik = i + ik_offset, jk = j - jk_offset; }
  else if(k==3) { ik = i,             jk = j - jk_offset; }  
  else if(k==4) { ik = i - ik_offset, jk = j - jk_offset; }
  else if(k==5) { ik = i - ik_offset, jk = j;             }
  else if(k==6) { ik = i - ik_offset, jk = j + jk_offset; }
  else if(k==7) { ik = i,             jk = j + jk_offset; }

  //Check if the central block index is valid, otherwise, invalidate kth neighboring block indices
  if(i<0 || j <0){ik=-1, jk=-1;}
  
  //cout << "2) Neighbor Block: (k, ik, jk) = " << k << ", " << ik << ", " << jk << endl;
  
  //3) Call method to find the neighboring block number
  //corresponding to (i +/- offset, j +/- offset)
  UInt_t id_k;
  id_k = GetBlockID(ik, jk);
  //cout << "3) (id_k, ik, jk) = " << id_k << ", " << ik << ", " << jk << endl;
  return id_k;


}

void THcNPSArray::GetMax(Float_t pInei[8], Int_t nei[8], Int_t &virus_blk, Float_t &max)
{
  
  /*
    Brief: This method returns (by reference) the highest
    pulse integral neighbor block hit index (virus_blk) and corresponding pulse
    integral (max) out of at most 8 neighboring blocks.
  */

  max = -1;
  virus_blk = -1;

  //loop over each of the (at most) 8 neighbors
  for(Int_t k=0; k<8; k++){

    if(pInei[k]>=max) {

      max       = pInei[k];
      virus_blk = nei[k];

    }

  }
  
}


// Fiducial volume limits.

Double_t THcNPSArray::fvXmin() {
  return fXPos[0][0] - fXStep/2 + static_cast<THcNPSCalorimeter*>(fParent)->fvDelta;
}

Double_t THcNPSArray::fvYmax() {
  return fYPos[0][0] + fYStep/2 - static_cast<THcNPSCalorimeter*>(fParent)->fvDelta;
}

Double_t THcNPSArray::fvXmax() {
  return fXPos[fNRows-1][fNColumns-1] + fXStep/2 - static_cast<THcNPSCalorimeter*>(fParent)->fvDelta;
}

Double_t THcNPSArray::fvYmin() {
  return fYPos[fNRows-1][fNColumns-1] - fYStep/2 + static_cast<THcNPSCalorimeter*>(fParent)->fvDelta;
}
Double_t THcNPSArray::clMaxEnergyBlock(THcNPSShowerCluster* cluster) {
  Double_t max_energy=-1.;
  Double_t max_block=-1.;
  for (THcNPSShowerClusterIt it=(*cluster).begin(); it!=(*cluster).end(); ++it) {
    if ( (**it).hitE() > max_energy )   {
       max_energy = (**it).hitE();
       max_block = ((**it).hitColumn())*fNRows + (**it).hitRow()+1;
    }
  }
  return max_block;
}




#if 0
//_____________________________________________________________________________
Int_t THcNPSArray::AccumulateStat(TClonesArray& tracks )
{
  // Accumumate statistics for efficiency calculations.
  //
  // Choose electron events in gas Cherenkov with good Chisq of the best track.
  // Project best track to Array,
  // calculate module number for the track,
  // accrue number of tracks for the module,
  // accrue number of hits for the module, if it is hit.
  // Accrue total numbers of tracks and hits for Array.

  THaTrack* BestTrack = static_cast<THaTrack*>( tracks[0]);
  if (BestTrack->GetChi2()/BestTrack->GetNDoF() > fStatMaxChi2) return 0;

  Double_t XTrk = kBig;
  Double_t YTrk = kBig;
  Double_t pathl = kBig;

  // Track interception with Array. The coordinates are in the calorimeter's
  // local system.

  fOrigin = GetOrigin();
  static_cast<THcNPSCalorimeter*>(fParent)->CalcTrackIntercept(BestTrack, pathl, XTrk, YTrk);

  // Transform coordiantes to the spectrometer's coordinate system.
  XTrk += GetOrigin().X();
  YTrk += GetOrigin().Y();
						     
  for (Int_t i=0; i<fNelem; i++) {

    Int_t row = i%fNRows;
    Int_t col =i/fNRows;

    if (TMath::Abs(XTrk - fXPos[row][col]) < fStatSlop &&
	TMath::Abs(YTrk - fYPos[row][col]) < fStatSlop) {

      fStatNumTrk.at(i)++;
      fTotStatNumTrk++;
      
      if (fGoodAdcPulseInt.at(i) > 0.) {
	fStatNumHit.at(i)++;
	fTotStatNumHit++;
      }
      
    }
    
  }

  if (static_cast<THcNPSCalorimeter*>(fParent)->fdbg_tracks_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "THcNPSArray::AccumulateStat:" << endl;
    cout << "   Chi2/NDF = " <<BestTrack->GetChi2()/BestTrack->GetNDoF() <<endl;
    //    cout << "   HGCER Npe = " << hgcer->GetCerNPE() << endl;
    cout << "   XTrk, YTrk = " << XTrk << "  " << YTrk << endl;						     
    for (Int_t i=0; i<fNelem; i++) {
      
      Int_t row = i%fNRows;
      Int_t col =i/fNRows;

      if (TMath::Abs(XTrk - fXPos[row][col]) < fStatSlop &&
	  TMath::Abs(YTrk - fYPos[row][col]) < fStatSlop) {

	cout << "   Module " << i << ", X=" << fXPos[i/fNRows][i%fNRows]
	     << ", Y=" << fYPos[i/fNRows][i%fNRows] << " matches track" << endl;

	if (fGoodAdcPulseInt.at(i) > 0.)
	  cout << "   PulseIntegral = " << fGoodAdcPulseInt.at(i) << endl;
      }
    }
      
    cout << "---------------------------------------------------------------\n";
    //    getchar();
  }
  
  return 1;
}
#endif


