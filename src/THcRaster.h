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

  //  TVector3 GetPosition()  const { return fPosition[2]; }
  TVector3 GetPosition()  const { return fPosition[0]; }
  TVector3 GetDirection() const { return fDirection; } // Hall C we don't use raster direction yet.

  Double_t GetCurrentX() { return fRawPos[0]; }
  Double_t GetCurrentY() { return fRawPos[1]; }

 protected:

  /* void InitializeReconstruction(); */
  Int_t  ReadDatabase( const TDatime& date );
  Int_t DefineVariables( EMode mode );

  Double_t       fgpbeam;     //

  Double_t       fXADC;     // X current
  Double_t       fYADC;     // Y current
  Double_t       fXpos;     // X position
  Double_t       fYpos;     // Y position
  
  Double_t       fRawADC[2];     // raw ADC values
  Double_t       fPedADC[2];     // ADC poedestals 
  Double_t       fAvgPedADC[2];     // Avergage ADC poedestals 

  Double_t       fRawPos[2];     // current in Raster ADCs for position
  TVector3       fPosition[2];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3       fDirection;

 private:
  Bool_t    fAnalyzePedestals;
  Int_t     fNPedestalEvents;
  Double_t  fFrCalMom;
  Double_t  fFrXADCperCM;
  Double_t  fFrYADCperCM;


  void   CalculatePedestals();
  void   AccumulatePedestals(TClonesArray* rawhits);

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};  

////////////////////////////////////////////////////////////////////////////////


#endif
