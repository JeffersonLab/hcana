#ifndef ROOT_THcScintillatorPlane
#define ROOT_THcScintillatorPlane

//////////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
// A Hall C scintillator plane
//
// May want to later inherit from a THcPlane class if there are similarities
// in what a plane is shared with other detector types (shower, etc.)
//
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include "THcScintPlaneCluster.h"

using namespace std;

class THaEvData;
class THaSignalHit;

class THcScintillatorPlane : public THaSubDetector {

 public:
  THcScintillatorPlane( const char* name, const char* description,
			Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcScintillatorPlane();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);

  virtual Int_t AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit);
  virtual void  CalculatePedestals( );

  Int_t GetNelem() {return fNelem;}; // return number of paddles in this plane
  Int_t GetNScinHits() {return fNScinHits;}; // Get # hits in plane (that pass min/max TDC cuts)
  Int_t GetNGoodHits() {return fNGoodHits;}; // Get # hits in plane (used in determining focal plane time)
  Double_t GetHitDistance() {return fHitDistance;}; // Distance between track and hit paddle
  Double_t GetTrackXPosition() {return fTrackXPosition;}; // Distance track X position at plane
  Double_t GetTrackYPosition() {return fTrackYPosition;}; // Distance track Y position at plane
  Double_t GetSpacing() {return fSpacing;}; // spacing of paddles
  Double_t GetSize() {return fSize;};    // paddle size
  Double_t GetHodoSlop() {return fHodoSlop;}; // hodo slop
  Double_t GetZpos() {return fZpos;};   //return the z position
  Double_t GetDzpos() {return fDzpos;};
  Double_t GetPosLeft() {return fPosLeft;};
  Double_t GetPosRight() {return fPosRight;};
  Double_t GetPosOffset() {return fPosOffset;};
  Double_t GetPosCenter(Int_t PaddleNo) {return fPosCenter[PaddleNo];}; // counting from zero!
  Double_t GetFpTime() {return fFptime;};
  Double_t GetNumberClusters() {return fNumberClusters;}; // number of paddle clusters

  void SetFpTime(Double_t f) {fFptime=f;};
  void SetNGoodHits(Int_t ng) {fNGoodHits=ng;};
  void SetHitDistance(Double_t f) {fHitDistance=f;}; // Distance between track and hit paddle
  void SetScinYPos(Double_t f) {fScinYPos=f;}; // Scint Average Y position at plane (cm)
  void SetScinXPos(Double_t f) {fScinXPos=f;}; // Scint Average X position at plane (cm)
  void SetTrackXPosition(Double_t f) {fTrackXPosition=f;}; // Distance track X position at plane
  void SetTrackYPosition(Double_t f) {fTrackYPosition=f;}; // Distance track Y position at plane
  void SetNumberClusters(Int_t nclus) {fNumberClusters=nclus;}; // number of paddle 
  void SetCluster(Int_t ic,Double_t pos) {((THcScintPlaneCluster*) fCluster->ConstructedAt(ic))->Set(ic,pos);}
  void SetClusterSize(Int_t ic,Double_t size) {((THcScintPlaneCluster*) fCluster->ConstructedAt(ic))->SetSize(size);}
  void SetClusterFlag(Int_t ic,Double_t flag) {((THcScintPlaneCluster*) fCluster->ConstructedAt(ic))->SetFlag(flag);}
  void SetClusterUsedFlag(Int_t ic,Double_t flag) {((THcScintPlaneCluster*) fCluster->ConstructedAt(ic))->SetUsedFlag(flag);}

  TClonesArray* fParentHitList;

  TClonesArray* GetHits() { return fHodoHits;};

  TClonesArray* fCluster;

 protected:

  TClonesArray* frPosAdcErrorFlag;
  TClonesArray* frNegAdcErrorFlag;


  TClonesArray* frPosTDCHits;
  TClonesArray* frNegTDCHits;
  TClonesArray* frPosADCHits;
  TClonesArray* frNegADCHits;
  TClonesArray* frPosADCSums;
  TClonesArray* frNegADCSums;
  TClonesArray* frPosADCPeds;
  TClonesArray* frNegADCPeds;
  TClonesArray* fHodoHits;

  TClonesArray* frPosTdcTimeRaw;
  TClonesArray* frPosAdcPedRaw;
  TClonesArray* frPosAdcPulseIntRaw;
  TClonesArray* frPosAdcPulseAmpRaw;
  TClonesArray* frPosAdcPulseTimeRaw;

  TClonesArray* frPosTdcTime;
  TClonesArray* frPosAdcPed;
  TClonesArray* frPosAdcPulseInt;
  TClonesArray* frPosAdcPulseAmp;
  TClonesArray* frPosAdcPulseTime;

  TClonesArray* frNegTdcTimeRaw;
  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;

  TClonesArray* frNegTdcTime;
  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;
  TClonesArray* frNegAdcPulseTime;

  TClonesArray* frPosAdcSampPedRaw;
  TClonesArray* frPosAdcSampPulseIntRaw;
  TClonesArray* frPosAdcSampPulseAmpRaw;
  TClonesArray* frPosAdcSampPulseTimeRaw;
  TClonesArray* frPosAdcSampPed;
  TClonesArray* frPosAdcSampPulseInt;
  TClonesArray* frPosAdcSampPulseAmp;
  TClonesArray* frPosAdcSampPulseTime;
  TClonesArray* frNegAdcSampPedRaw;
  TClonesArray* frNegAdcSampPulseIntRaw;
  TClonesArray* frNegAdcSampPulseAmpRaw;
  TClonesArray* frNegAdcSampPulseTimeRaw;
  TClonesArray* frNegAdcSampPed;
  TClonesArray* frNegAdcSampPulseInt;
  TClonesArray* frNegAdcSampPulseAmp;
  TClonesArray* frNegAdcSampPulseTime;

  //Hodoscopes Multiplicities
  Int_t fTotNumPosAdcHits;
  Int_t fTotNumNegAdcHits;
  Int_t fTotNumAdcHits;
  Int_t fNumberClusters;

  Int_t fTotNumPosTdcHits;
  Int_t fTotNumNegTdcHits;
  Int_t fTotNumTdcHits;

  Int_t fTotNumGoodPosAdcHits;
  Int_t fTotNumGoodNegAdcHits;
  Int_t fTotNumGoodAdcHits;

  Int_t fTotNumGoodPosTdcHits;
  Int_t fTotNumGoodNegTdcHits;
  Int_t fTotNumGoodTdcHits;
  
  //Hodoscope "GOOD" Occupancies
  vector<Int_t>    fNumGoodPosAdcHits;
  vector<Int_t>    fNumGoodNegAdcHits;
  
  vector<Int_t>    fNumGoodPosTdcHits;
  vector<Int_t>    fNumGoodNegTdcHits;

  //Hodoscopoe "GOOD" ADC Ped/Amps/Ints/Time
  vector<Double_t>  fGoodPosAdcPed;
  vector<Double_t>  fGoodNegAdcPed;

  vector<Double_t>  fGoodPosAdcMult;
  vector<Double_t>  fGoodNegAdcMult;

  vector<Double_t>  fGoodPosAdcHitUsed;
  vector<Double_t>  fGoodNegAdcHitUsed;

  vector<Double_t>  fGoodPosAdcPulseAmp;
  vector<Double_t>  fGoodNegAdcPulseAmp;

  vector<Double_t>  fGoodPosAdcPulseInt;
  vector<Double_t>  fGoodNegAdcPulseInt;

  vector<Double_t>  fGoodPosAdcPulseTime;
  vector<Double_t>  fGoodNegAdcPulseTime;
 
  vector<Double_t>  fGoodPosAdcTdcDiffTime;
  vector<Double_t>  fGoodNegAdcTdcDiffTime;

  vector<Double_t> fNegAdcSampWaveform;
  vector<Double_t> fPosAdcSampWaveform;

  //Hodoscopoe "GOOD" TDC Variables
  vector<Double_t>  fGoodPosTdcTimeUnCorr;
  vector<Double_t>  fGoodPosTdcTimeCorr;
  vector<Double_t>  fGoodPosTdcTimeTOFCorr;
  
  vector<Double_t>  fGoodNegTdcTimeUnCorr;
  vector<Double_t>  fGoodNegTdcTimeCorr;
  vector<Double_t>  fGoodNegTdcTimeTOFCorr;
  
  //Time Walk Corrected
  vector<Double_t>  fGoodPosTdcTimeWalkCorr;
  vector<Double_t>  fGoodNegTdcTimeWalkCorr;
  vector<Double_t>  fGoodDiffDistTrack;

  Int_t fDebugAdc;
  Double_t fPosTdcRefTime;
  Double_t fPosAdcRefTime;
  Double_t fNegTdcRefTime;
  Double_t fNegAdcRefTime;
  Double_t fPosTdcRefDiffTime;
  Double_t fPosAdcRefDiffTime;
  Double_t fNegTdcRefDiffTime;
  Double_t fNegAdcRefDiffTime;
  Double_t fHitDistance;
  Double_t fScinXPos;
  Double_t fScinYPos;
  Double_t fTrackXPosition;
  Double_t fTrackYPosition;
  Int_t fCosmicFlag; //
  Int_t fPlaneNum;		/* Which plane am I 1-4 */
  UInt_t fTotPlanes;            /* so we can read variables that are not indexed by plane id */
  UInt_t fNelem;		/* Need since we don't inherit from
				 detector base class */
  Int_t fNScinHits;                 /* number of hits in plane (that pass min/max TDC cuts) */
  Int_t fNGoodHits;                 /* number of hits in plane (used in determining focal plane time) */

  // Constants
  Int_t fADCMode;		// 0: standard, 1: use FADC ped, 2: integrate sample
                                // 3: integrate sample, subract dynamic pedestal
  static const Int_t kADCStandard=0;
  static const Int_t kADCDynamicPedestal=1;
  static const Int_t kADCSampleIntegral=2;
  static const Int_t kADCSampIntDynPed=3;
  Double_t fADCPedScaleFactor;	// Multiply dynamic pedestal by this before subtracting
  Int_t fADCDiagCut;		// Cut for ADC in hit maps.  Defaults to 50
  Int_t fTdcOffset;		/* Overall offset to raw tdc */
  Double_t fAdcTdcOffset;	/* Overall offset to raw adc times */

  Int_t  fOutputSampWaveform;
  Int_t  fUseSampWaveform;
  Double_t  fSampThreshold;
  Int_t  fSampNSA;
  Int_t  fSampNSAT;
  Int_t  fSampNSB;

  Int_t fMaxHits;               /* maximum number of hits to be considered - useful for dimensioning arrays */
  Double_t fSpacing;            /* paddle spacing */
  Double_t fSize;               /* paddle size */
  Double_t fZpos;               /* z position */
  Double_t fDzpos;
  Double_t fHodoSlop;           /* max allowed slop for this plane */
  Double_t fPosLeft;            /* NOTE: "left" = "bottom" for a Y scintillator */
  Double_t fPosRight;           /* NOTE: "right" = "top" for a Y scintillator */
  Double_t fPosOffset;
  Double_t *fPosCenter;         /* array with centers for all scintillators in the plane */
  Double_t fScinTdcMin;
  Double_t fScinTdcMax;
  Double_t fStartTimeCenter;
  Double_t fStartTimeSlop;
  Double_t fScinTdcToTime;
  Double_t fTofTolerance;
  Double_t fBetaNominal;
  Double_t *fHodoPosMinPh;	// Minimum pulse height per paddle for this plane
  Double_t *fHodoNegMinPh;	// Minimum pulse height per paddle for this plane
  Double_t *fHodoPosPhcCoeff;    // Pulse height to time coefficient per paddle for this plane
  Double_t *fHodoNegPhcCoeff;    // Pulse height to time coefficient per paddlefor this plane
  Double_t *fHodoPosTimeOffset;
  Double_t *fHodoNegTimeOffset;
  Int_t fTofUsingInvAdc;
  Double_t *fHodoVelLight;
  Double_t *fHodoPosInvAdcOffset;
  Double_t *fHodoNegInvAdcOffset;
  Double_t *fHodoPosAdcTimeWindowMin;    
  Double_t *fHodoPosAdcTimeWindowMax;
  Double_t *fHodoNegAdcTimeWindowMin;    
  Double_t *fHodoNegAdcTimeWindowMax;
  Double_t *fHodoPosInvAdcLinear;
  Double_t *fHodoNegInvAdcLinear;
  Double_t *fHodoPosInvAdcAdc;
  Double_t *fHodoNegInvAdcAdc;
  //Time-Walk Parameters
  Double_t* fHodoVelFit;
  Double_t* fHodoCableFit;
  Double_t* fHodo_LCoeff;
  Double_t* fHodoPos_c1;
  Double_t* fHodoNeg_c1;
  Double_t* fHodoPos_c2;
  Double_t* fHodoNeg_c2;
  Double_t  fTdc_Thrs;  

  Double_t tw_corr_pos;
  Double_t tw_corr_neg;

  Double_t *fHodoSigma;
  Double_t fTolerance; /* need this for Focal Plane Time estimation */
  Double_t fFptime;
  /* Pedestal Quantities */
  Int_t fNPedestalEvents;	/* Number of pedestal events */
  Int_t fMinPeds;		/* Only analyze/update if num events > */
  Int_t *fPosPedSum;		/* Accumulators for pedestals */
  Int_t *fPosPedSum2;
  Int_t *fPosPedLimit;
  Int_t *fPosPedCount;
  Int_t *fNegPedSum;
  Int_t *fNegPedSum2;
  Int_t *fNegPedLimit;
  Int_t *fNegPedCount;

  Double_t *fPosPed;
  Double_t *fPosSig;
  Double_t *fPosThresh;
  Double_t *fNegPed;
  Double_t *fNegSig;
  Double_t *fNegThresh;
  //
   Int_t        fEventType;
  Int_t        fEventNum;
 
  //
  Int_t fNScinGoodHits; // number of hits for which both ends of the paddle fired in time!
  Double_t fpTime; // the original code only has one fpTime per plane!

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );

  ClassDef(THcScintillatorPlane,0); // Scintillator bars in a plane
};
#endif
