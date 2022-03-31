#ifndef ROOT_THcShowerArray
#define ROOT_THcShowerArray

//#define HITPIC 1
#ifdef HITPIC
#define NPERLINE 5
#endif

//////////////////////////////////////////////////////////////////////////////
//
// THcShowerArray
//
// A Hall C Fly's Eye Shower Array
//
// Subdetector for the fly's eye part of the SHMS shower counter.
//
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "THcShowerHit.h"

#include <iostream>

#include <fstream>

class THaEvData;
class THaSignalHit;
class THcHodoscope;

class THcShowerArray : public THaSubDetector {

public:
  THcShowerArray( const char* name, const char* description,
		  Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcShowerArray();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
  Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);
  virtual Int_t CoarseProcessHits();
  virtual Int_t AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit);
  virtual void  CalculatePedestals( );
  virtual void  InitializePedestals( );
  virtual void  FillADC_DynamicPedestal( );
  virtual void  FillADC_SampleIntegral( );
  virtual void  FillADC_SampIntDynPed( );
  virtual void  FillADC_Standard( );
 
  // Cluster to track association method.
  Int_t MatchCluster(THaTrack*, Double_t&, Double_t&);

  // Get total energy deposited in the cluster matched to the given
  // spectrometer Track.

  Float_t GetShEnergy(THaTrack*);
  Double_t GetClMaxEnergyBlock() {
    return fMatchClMaxEnergyBlock;
  };
  Double_t GetClSize() {
    return fClustSize;
  };
  Double_t GetClX() {
    return fMatchClX;
  };
  Double_t GetClY() {
    return fMatchClY;
  };

  //  Double_t fSpacing;   not used

  //  TClonesArray* fParentHitList; not used

  Double_t GetEarray() {
    return fEarray;
  };

  // Fiducial volume limits.
  Double_t fvXmin();
  Double_t fvYmax();
  Double_t fvXmax();
  Double_t fvYmin();
  Double_t clMaxEnergyBlock(THcShowerCluster* cluster);

  Int_t AccumulateStat(TClonesArray& tracks);
    
protected:

#ifdef HITPIC
  char **hitpic;
  Int_t piccolumn;
#endif


 


  //counting variables
  Int_t fTotNumAdcHits;              // Total number of ADC hits
  Int_t fTotNumGoodAdcHits;          // Total number of good ADC hits (pass threshold)

  vector<Int_t>         fNumGoodAdcHits;          // shower good occupancy
  vector<Double_t>      fGoodAdcPulseIntRaw;      // [fNelem] Good Raw ADC pulse Integrals of blocks
  
  vector<Double_t>      fGoodAdcPed;             // [fNelem] Event by event (FADC) good pulse pedestals
  vector<Double_t>      fGoodAdcMult;             
  vector<Double_t>      fGoodAdcPulseInt;       // [fNelem] good pedestal subtracted pulse integrals
  vector<Double_t>      fGoodAdcPulseAmp;
  vector<Double_t>      fGoodAdcPulseTime;
  vector<Double_t>      fGoodAdcTdcDiffTime;
  vector<Double_t>      fSampWaveform;

  vector<Double_t>      fE;                    //[fNelem] energy deposition in shower blocks

  Int_t* fBlock_ClusterID;              // [fNelem] Cluster ID of the block -1 then not in a cluster
  Double_t  fEarray;                          // Total Energy deposition in the array.
  TClonesArray* fADCHits;	// List of ADC hits

  // Parameters

  UInt_t fNRows;
  UInt_t fNColumns;
  Double_t fXFront;              // Front position X
  Double_t fYFront;              // Front position Y
  Double_t fZFront;              // Front position Z, from FP
  Double_t fXStep;               // distance btw centers of blocks along X
  Double_t fYStep;               // distance btw centers of blocks along Y
  Double_t fZSize;               // Block size along Z
  Double_t** fXPos;              // block X coordinates
  Double_t** fYPos;              // block Y coordinates
  Double_t** fZPos;              // block Z coordinates

  Int_t fUsingFADC;		// != 0 if using FADC in sample mode
  Int_t fADCMode;		//    
  //  1 == Use the pulse int - pulse ped
  //  2 == Use the sample integral - known ped
  //  3 == Use the sample integral - sample ped
  static const Int_t kADCStandard=0;
  static const Int_t kADCDynamicPedestal=1;
  static const Int_t kADCSampleIntegral=2;
  static const Int_t kADCSampIntDynPed=3;
  Double_t *fAdcTimeWindowMin ;
  Double_t *fAdcTimeWindowMax ;
  Int_t *fPedDefault ;
  Double_t fAdcThreshold ;
  Double_t fAdcTdcOffset;

  Int_t fDebugAdc;
  Int_t fPedSampLow;		// Sample range for
  Int_t fPedSampHigh;		// dynamic pedestal
  Int_t fDataSampLow;		// Sample range for
  Int_t fDataSampHigh;		// sample integration

  Int_t fLayerNum;		// 2 for SHMS

  Int_t  fOutputSampWaveform;
  Int_t  fUseSampWaveform;
  Double_t  fSampThreshold;
  Int_t  fSampNSA;
  Int_t  fSampNSAT;
  Int_t  fSampNSB;

  // Accumulators for pedestals go here

  Int_t fNPedestalEvents;	/* Pedestal event counter */
  Int_t fMinPeds;		/* Only analyze/update if num events > */

  // 2D arrays

  Int_t *fPedSum;		/* Accumulators for pedestals */
  Int_t *fPedSum2;
  Int_t *fPedLimit;          // Analyze pedestal if ADC signal < PedLimit
  Int_t *fPedCount;          // [fNelem] counter of pedestal analysis

  Float_t *fPed;             // [fNelem] pedestal positions
  Float_t *fSig;             // [fNelem] pedestal rms-s
  Float_t *fThresh;          // [fNelem] ADC thresholds

  Double_t* fGain;           // [fNelem] Gain constants from calibration
  
  Int_t fNclust;             // Number of hit clusters
  Int_t fNtracks;            // Number of shower tracks, i.e. number of
                             // cluster-to-track associations

  Double_t fMatchClX;
  Double_t fMatchClY;
  Double_t fMatchClMaxEnergyBlock;
  Double_t fClustSize;

  THcShowerClusterList* fClusterList;   // List of hit clusters

  TClonesArray* frAdcSampPedRaw;
  TClonesArray* frAdcSampPulseIntRaw;
  TClonesArray* frAdcSampPulseAmpRaw;
  TClonesArray* frAdcSampPulseTimeRaw;
  TClonesArray* frAdcSampPed;
  TClonesArray* frAdcSampPulseInt;
  TClonesArray* frAdcSampPulseAmp;
  TClonesArray* frAdcSampPulseTime;
  TClonesArray* frAdcSampWaveform;

  TClonesArray* frAdcPedRaw;
  TClonesArray* frAdcPulseIntRaw;
  TClonesArray* frAdcPulseAmpRaw;
  TClonesArray* frAdcPulseTimeRaw;
  TClonesArray* frAdcPed;
  TClonesArray* frAdcPulseInt;
  TClonesArray* frAdcPulseAmp;
  TClonesArray* frAdcPulseTime;
  TClonesArray* frAdcErrorFlag;

  //Quatitites for efficiency calculations.

  Double_t fStatCerMin;
  Double_t fStatSlop;
  Double_t fStatMaxChi2;
  vector<Int_t> fStatNumTrk;
  vector<Int_t> fStatNumHit;
  Int_t fTotStatNumTrk;
  Int_t fTotStatNumHit;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  THcHodoscope* fglHod;		// Hodoscope to get start time

  THaDetectorBase* fParent;

  ClassDef(THcShowerArray,0); // Fly;s Eye calorimeter array
};

#endif
