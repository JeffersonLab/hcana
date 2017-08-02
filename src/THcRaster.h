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
  TVector3 GetPosition()  const { return fPosition[1]; }
  TVector3 GetDirection() const { return fDirection; }

  Double_t GetCurrentX() { return fRawPos[0]; }
  Double_t GetCurrentY() { return fRawPos[1]; } // duplicate and index [2] and [3] for b coils?

 protected:

  /* void InitializeReconstruction(); */
  Int_t  ReadDatabase( const TDatime& date );
  Int_t DefineVariables( EMode mode );

  Double_t       fgpbeam;   //beam momentum
  Double_t       fgfrx_dist;     //Distance of raster to target
  Double_t       fgfry_dist;
  Double_t       fgbeam_xoff; // Beam offsets
  Double_t       fgbeam_xpoff; // Beam offsets
  Double_t       fgbeam_yoff; // Beam offsets
  Double_t       fgbeam_ypoff; // Beam offsets
  Int_t          fgusefr;   /* Use Raster for beam position */

  Double_t       fRawXaADC;  // Xa raw ADC
  Double_t       fRawYaADC;  // Ya raw ADC
  Double_t       fXaADC;     // Xa ADC
  Double_t       fYaADC;     // Ya ADC
  Double_t       fXapos;     // Xa position
  Double_t       fYapos;     // Ya position

  Double_t       fRawXbADC;  // Xb raw ADC
  Double_t       fRawYbADC;  // Yb raw ADC
  Double_t       fXbADC;     // Xb ADC
  Double_t       fYbADC;     // Yb ADC
  Double_t       fXbpos;     // Xb position
  Double_t       fYbpos;     // Yb position 
  
  Double_t       fPedADC[4];     // ADC pedestals
  //Double_t       fAvgPedADC[2];     // Average ADC pedestals

  Double_t       fRawPos[2];     // current in Raster ADCs for position
  TVector3       fPosition[2];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3       fDirection;

  Double_t  fFrXaADC_zero_offset;
  Double_t  fFrYaADC_zero_offset;
  Double_t  fFrXbADC_zero_offset;
  Double_t  fFrYbADC_zero_offset;

  TClonesArray* frPosAdcPulseIntRaw;

 private:
  Bool_t    fAnalyzePedestals;
  Int_t     fNPedestalEvents;
  Double_t  fFrCalMom;

  Double_t  fFrXaADCperCM;
  Double_t  fFrYaADCperCM;

  Double_t  fFrXbADCperCM; 
  Double_t  fFrYbADCperCM;


  void   CalculatePedestals();
  void   AccumulatePedestals(TClonesArray* rawhits);
  void   Clear(Option_t* opt="");

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};

////////////////////////////////////////////////////////////////////////////////


#endif
