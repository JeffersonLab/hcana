#ifndef ROOT_THcBPM
#define ROOT_THcBPM

// Simple bpm det module to process HallC BPM signal
// Based on THaBPM, but with Hall C style hit processing

#include "THaBeamDet.h"
#include "THcHitList.h"
#include "TMatrixD.h"
#include "TVectorT.h"
#include "TClonesArray.h"

class THcBPM : public THaBeamDet, public THcHitList {
 public:
  
  THcBPM( const char* name, const char* description="", THaApparatus* a = nullptr);
  ~THcBPM();
  
  void     Clear( Option_t* = "" );
  Int_t    Decode( const THaEvData& );
  Int_t    Process();
  EStatus  Init( const TDatime& run_time );

  void     AccumulatePedestals(TClonesArray* rawhits);
  void     CalculatePedestals();
 
  TVector3 GetPosition()  const { return fPosition; }
  TVector3 GetDirection() const { return fDirection; }

  Double_t GetRawSignal0() {return fRawSignal(0);}
  Double_t GetRawSignal1() {return fRawSignal(1);}
  Double_t GetRawSignal2() {return fRawSignal(2);}
  Double_t GetRawSignal3() {return fRawSignal(3);}

  Double_t GetRotPosX() {return fRotPos(0); }
  Double_t GetRotPosY() {return fRotPos(1); }

 protected:

  Int_t fNhits;

  Int_t fAnalyzePedestals;

  // Raw signal variables
  TVectorD fRawSignal;  // X+, X-, Y+, Y-
  TVectorD fPedestals;  // Pedestals
  TVectorD fCorSignal;  // Pedestal subtracted signal
  TVectorD fRotPos;     // Position in the BPM system
  TMatrixD fRot2HCSPos; // rotation matrix from BPM sys to Hall Coord sys

  TVector3 fOffset;
  TVector3 fPosition;
  TVector3 fDirection;

  Double_t fCalibRot;   

  // Pedestal variables
  Int_t* fPedSum;
  Int_t* fPedLimit;
  Int_t* fPedCount;
  Int_t  fMinPed;
  Int_t  fNPedestalEvents;

  TClonesArray* frAdcPulseIntRaw;


  Int_t ReadDatabase( const TDatime& date );
  Int_t DefineVariables( EMode mode = kDefine );
  void  InitializePedestals();

  ClassDef(THcBPM,0)

};

#endif
