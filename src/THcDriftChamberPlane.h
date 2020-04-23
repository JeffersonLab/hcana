#ifndef ROOT_THcDriftChamberPlane
#define ROOT_THcDriftChamberPlane

//////////////////////////////////////////////////////////////////////////////
//
// THcDriftChamberPlane
//
// A Hall C scintillator plane
//
// May want to later inherit from a THcPlane class if there are similarities
// in what a plane is shared with other detector types (shower, etc.)
//
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <cassert>

class THaEvData;
class THcDCWire;
class THcDCHit;
class THcDCTimeToDistConv;
class THcHodoscope;

/*class THaSignalHit;*/

class THcDriftChamberPlane : public THaSubDetector {

public:
  THcDriftChamberPlane( const char* name, const char* description,
			Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcDriftChamberPlane();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);

  virtual Int_t SubtractStartTime();

  virtual Int_t GetReadoutSide(Int_t wirenum);
  
  // Get and Set functions
  Int_t        GetNWires()   const { return fWires->GetLast()+1; }
  THcDCWire*  GetWire(Int_t i) const
  { assert( i>=1 && i<=GetNWires() );
    return (THcDCWire*)fWires->UncheckedAt(i-1); }

  Int_t         GetNHits() const { return fHits->GetLast()+1; }
  Int_t         GetNRawhits() const {return fNRawhits; }
  TClonesArray* GetHits()  const { return fHits; }

  Int_t        GetPlaneNum() const { return fPlaneNum; }
  Int_t        GetChamberNum() const { return fChamberNum; }
  void         SetPlaneIndex(Int_t index) { fPlaneIndex = index; }
  Int_t        GetPlaneIndex() { return fPlaneIndex; }
  Double_t     GetXsp() const { return fXsp; }
  Double_t     GetYsp() const { return fYsp; }
  Int_t        GetReadoutX() { return fReadoutX; }
  Double_t     GetReadoutCorr() { return fReadoutCorr; }
  Double_t     GetCentralTime() { return fCentralTime; }
  Int_t        GetDriftTimeSign() { return fDriftTimeSign; }
  Double_t     GetBeta() { return fBeta; }
  Double_t     GetSigma() { return fSigma; }
  Double_t     GetPsi0() { return fPsi0; }
  Double_t*    GetStubCoef() { return fStubCoef; }
  Double_t*    GetPlaneCoef() { return fPlaneCoef; }
  THcDriftChamberPlane(); // for ROOT I/O
  Double_t     CalcWireFromPos(Double_t pos);
  Int_t        GetReadoutLR() const { return fReadoutLR;}
  Int_t        GetReadoutTB() const { return fReadoutTB;}
  Int_t        GetVersion() const {return fVersion;}

protected:

  TClonesArray* fParentHitList;

  TClonesArray* fHits;
  TClonesArray* fRawHits;
  TClonesArray* fWires;
 
  Double_t fTdcRefTime;

  Int_t fVersion;
  Int_t fWireOrder;
  Int_t fPlaneNum;
  Int_t fPlaneIndex;		/* Index of this plane within it's chamber */
  Int_t fChamberNum;
  Int_t fUsingTzeroPerWire;
  Int_t fUsingSigmaPerWire;
  Int_t fNRawhits;
  Int_t fNWires;
  Int_t fTdcWinMin;
  Int_t fTdcWinMax;
  Double_t fPitch;
  Double_t fCentralWire;
  Double_t fPlaneTimeZero;
  Double_t fXsp;
  Double_t fYsp;
  Double_t fSigma;
  Double_t fPsi0;
  Double_t fStubCoef[4];
  Double_t fBeta;
  Double_t fPlaneCoef[9];

  Int_t fReadoutX;
  Double_t fReadoutCorr;
  Double_t fCentralTime;
  Int_t fDriftTimeSign;
  Int_t fReadoutLR;
  Int_t fReadoutTB;

  Double_t fCenter;

  Double_t fNSperChan;		/* TDC bin size */

  Double_t* fTzeroWire;
  Double_t* fSigmaWire;

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  THcDCTimeToDistConv* fTTDConv;  // Time-to-distance converter for this plane's wires

  THcHodoscope* fglHod;		// Hodoscope to get start time

  ClassDef(THcDriftChamberPlane,0); // A single plane within a THcDriftChamber
};
#endif


