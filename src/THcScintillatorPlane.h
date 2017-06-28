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
  Double_t GetSpacing() {return fSpacing;}; // spacing of paddles
  Double_t GetSize() {return fSize;};    // paddle size
  Double_t GetHodoSlop() {return fHodoSlop;}; // hodo slop
  Double_t GetAdcTimeWindowMin() { return fAdcTimeWindowMin;}
  Double_t GetAdcTimeWindowMax() { return fAdcTimeWindowMax;}
  Double_t GetZpos() {return fZpos;};   //return the z position
  Double_t GetDzpos() {return fDzpos;};
  Double_t GetPosLeft() {return fPosLeft;};
  Double_t GetPosRight() {return fPosRight;};
  Double_t GetPosOffset() {return fPosOffset;};
  Double_t GetPosCenter(Int_t PaddleNo) {return fPosCenter[PaddleNo];}; // counting from zero!
  Double_t GetFpTime() {return fFptime;};

  void SetFpTime(Double_t f) {fFptime=f;};
  void SetNGoodHits(Int_t ng) {fNGoodHits=ng;};

  TClonesArray* fParentHitList;

  TClonesArray* GetHits() { return fHodoHits;};

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

  TClonesArray* frNegTdcTimeRaw;
  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;

  TClonesArray* frNegTdcTime;
  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;

  //Hodoscopes Multiplicities
  Int_t fTotNumPosAdcHits;
  Int_t fTotNumNegAdcHits;
  Int_t fTotNumAdcHits;

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

  vector<Double_t>  fGoodPosAdcPulseAmp;
  vector<Double_t>  fGoodNegAdcPulseAmp;

  vector<Double_t>  fGoodPosAdcPulseInt;
  vector<Double_t>  fGoodNegAdcPulseInt;

  vector<Double_t>  fGoodPosAdcPulseTime;
  vector<Double_t>  fGoodNegAdcPulseTime;

  //Hodoscopoe "GOOD" TDC Variables
  vector<Double_t>  fGoodPosTdcTimeUnCorr;
  vector<Double_t>  fGoodPosTdcTimeCorr;
  vector<Double_t>  fGoodPosTdcTimeTOFCorr;
  
  vector<Double_t>  fGoodNegTdcTimeUnCorr;
  vector<Double_t>  fGoodNegTdcTimeCorr;
  vector<Double_t>  fGoodNegTdcTimeTOFCorr;
  


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
  Int_t fMaxHits;               /* maximum number of hits to be considered - useful for dimensioning arrays */
  Double_t fSpacing;            /* paddle spacing */
  Double_t fSize;               /* paddle size */
  Double_t fZpos;               /* z position */
  Double_t fDzpos;
  Double_t fHodoSlop;           /* max allowed slop for this plane */
  Double_t fAdcTimeWindowMin;    
  Double_t fAdcTimeWindowMax;
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
  Double_t *fHodoPosInvAdcLinear;
  Double_t *fHodoNegInvAdcLinear;
  Double_t *fHodoPosInvAdcAdc;
  Double_t *fHodoNegInvAdcAdc;
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
  Int_t fNScinGoodHits; // number of hits for which both ends of the paddle fired in time!
  Double_t fpTime; // the original code only has one fpTime per plane!
  Double_t *fpTimes; // ... but also allows for more than one hit per plane
  Double_t *fScinTime; // array of scintillator times (only filled for goodhits)
  Double_t *fScinSigma; // errors for the above
  Double_t *fScinZpos; // zpositions for the above

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual void  InitializePedestals( );

  ClassDef(THcScintillatorPlane,0); // Scintillator bars in a plane
};
#endif
