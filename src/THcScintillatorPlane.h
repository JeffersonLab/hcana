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
  virtual Int_t PulseHeightCorrection();

  virtual Int_t AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit);
  virtual void  CalculatePedestals( );

  Int_t GetNelem(); // return number of paddles in this plane
  Int_t GetNScinHits(); // return the number of hits in this plane (that pass min/max TDC cuts)
  Double_t GetSpacing(); // return the spacing of paddles 
  Double_t GetSize();    // return paddle size
  Double_t GetHodoSlop(); // return the hodo slop
  Double_t GetZpos();   //return the z position
  Double_t GetDzpos();
  Double_t GetPosLeft();
  Double_t GetPosRight();
  Double_t GetPosOffset();
  Double_t GetPosCenter(Int_t PaddleNo); // here we're counting from zero!
  Double_t CalcFpTime();
  Double_t GetFpTime() {return fpTime;};
  Double_t GetFpTime(Int_t index) { return fpTimes[index];};
  Double_t GetScinTime(Int_t index) { return fScinTime[index];};
  Double_t GetScinSigma(Int_t index) { return fScinSigma[index];};
  Double_t GetScinZpos(Int_t index) { return fScinZpos[index];};
  Int_t GetNScinGoodHits() const {return fNScinGoodHits;};

  TClonesArray* fParentHitList;

  TClonesArray* GetPosADC() { return fPosADCHits;};  // Ahmed
  TClonesArray* GetNegADC() { return fNegADCHits;};  // Ahmed
  TClonesArray* GetPosTDC() { return fPosTDCHits;};  // Ahmed
  TClonesArray* GetNegTDC() { return fNegTDCHits;};  // Ahmed

 protected:

  TClonesArray* frPosTDCHits;
  TClonesArray* frNegTDCHits;
  TClonesArray* frPosADCHits;
  TClonesArray* frNegADCHits;
  TClonesArray* fPosTDCHits;
  TClonesArray* fNegTDCHits;
  TClonesArray* fPosADCHits;
  TClonesArray* fNegADCHits;

  Int_t fPlaneNum;		/* Which plane am I 1-4 */
  UInt_t fTotPlanes;            /* so we can read variables that are not indexed by plane id */
  UInt_t fNelem;		/* Need since we don't inherit from 
				 detector base class */
  Int_t fNScinHits;                 /* Number of hits in this plane */
  Int_t fMaxHits;               /* maximum number of hits to be considered - useful for dimensioning arrays */
  Double_t fSpacing;            /* paddle spacing */
  Double_t fSize;               /* paddle size */
  Double_t fZpos;               /* z position */
  Double_t fDzpos;
  Double_t fHodoSlop;           /* max allowed slop for this plane */
  Double_t fPosLeft;            /* NOTE: "left" = "top" for a Y scintillator */
  Double_t fPosRight;           /* NOTE: "right" = "bottom" for a Y scintillator */
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
  Double_t *fHodoVelLight;
  Double_t *fHodoSigma;

  Double_t fTolerance; /* need this for PulseHeightCorrection */
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


