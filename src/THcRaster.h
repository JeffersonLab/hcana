#ifndef ROOT_THcRaster
#define ROOT_THcRaster

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRaster                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaBeamDet.h"
#include "TVector.h"
#include "TClonesArray.h"
#include "THcHitList.h"
#include "THcDetectorMap.h"
#include "THcRasterRawHit.h"
#include "THaCutList.h"

class THcRaster : public THaBeamDet, public THcHitList {

 public:

  THcRaster(const char* name, const char* description ="",THaApparatus* a = NULL );

  ~THcRaster();
  EStatus    Init( const TDatime& run_time );

  Int_t      Decode( const THaEvData& );
  Int_t      Process();

  TVector3 GetPosition()  const { return fPosition[2]; }
  TVector3 GetDirection() const { return fDirection; } // Hall C we don't use raster direction yet.

  Double_t GetCurrentX() { return fRawPos[0]; }
  Double_t GetCurrentY() { return fRawPos[1]; }

 protected:

  void InitializeReconstruction();
  void           ClearEvent();
  Int_t  ReadDatabase( const TDatime& date );

  Double_t       fRawADC[2];     // raw ADC values
  Double_t       fPedADC[2];     // ADC poedestals 

  Double_t       fRawPos[2];     // current in Raster ADCs for position
  TVector3       fPosition[2];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3  fPosPed[2]; // pedestals 
  TVector3  fDirection;  // Beam angle at the target (meters)


  // Array to store channel numbers corresponding to X and Ysignals. 0 - X, 1 - Y.
  Double_t fChN0[2]; 

 private:
  Bool_t fAnalyzePedestals;
  void CalculatePedestals();
  void AccumulatePedestals(TClonesArray* rawhits);

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};  

////////////////////////////////////////////////////////////////////////////////


#endif
