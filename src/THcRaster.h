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

  THcRaster();
  ~THcRaster();
  EStatus    Init( const TDatime& run_time );

  Int_t      Decode( const THaEvData& );
  Int_t      Process();

  //  TVector3 GetPosition()  const { return fPosition[2]; }
  TVector3 GetPosition()  const { return fPosition[1]; }
  TVector3 GetDirection() const { return fDirection; }

  Double_t GetCurrentX() { return fRawPos[0]; }
  Double_t GetCurrentY() { return fRawPos[1]; }

  void  InitArrays() { /* do nothing */;}
  void  DeleteArrays() { /* do nothing */;}


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

  Double_t       fRawXAADC;  // Xa raw ADC
  Double_t       fRawYAADC;  // Ya raw ADC
  Double_t       fXAADC;     // Xa ADC
  Double_t       fYAADC;     // Ya ADC
  Double_t       fXApos;     // Xa position
  Double_t       fYApos;     // Ya position

  Double_t       fRawXBADC;  // Xb raw ADC
  Double_t       fRawYBADC;  // Yb raw ADC
  Double_t       fXBADC;     // Xb ADC
  Double_t       fYBADC;     // Yb ADC
  Double_t       fXBpos;     // Xb position
  Double_t       fYBpos;     // Yb position 
  
  Double_t       fPedADC[4];     // ADC pedestals
  //Double_t       fAvgPedADC[2];     // Average ADC pedestals

  Double_t       fRawPos[2];     // current in Raster ADCs for position
  TVector3       fPosition[4];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3       fDirection;

  Double_t  fFrXAADC_zero_offset;
  Double_t  fFrYAADC_zero_offset;
  Double_t  fFrXBADC_zero_offset;
  Double_t  fFrYBADC_zero_offset;

  Double_t fNhits;

  TClonesArray* frPosAdcPulseIntRaw;

 private:
  Bool_t    fAnalyzePedestals;
  Int_t     fNPedestalEvents;
  Double_t  fFrCalMom;

  Double_t  fFrXAADCperCM;
  Double_t  fFrYAADCperCM;

  Double_t  fFrXBADCperCM; 
  Double_t  fFrYBADCperCM;


  void   CalculatePedestals();
  void   AccumulatePedestals(TClonesArray* rawhits);
  void   Clear(Option_t* opt="");

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};

////////////////////////////////////////////////////////////////////////////////


#endif
