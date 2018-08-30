
//author Burcu Duran - Melanie Rehfuss (2017) 

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
#include "THaOutput.h"
#include "THaEpicsEvtHandler.h"

class THcRaster : public THaBeamDet, public THcHitList {

 public:

  THcRaster(const char* name, const char* description ="",THaApparatus* a = NULL );

  ~THcRaster();

  void  Clear(Option_t* opt="");
  void    AccumulatePedestals(TClonesArray* rawhits);
  void    CalculatePedestals();
 
  Int_t  Decode( const THaEvData& );
  Int_t  ReadDatabase( const TDatime& date );
  Int_t DefineVariables( EMode mode );

  EStatus    Init( const TDatime& run_time );

  void  InitArrays() { /* do nothing */;}
  void  DeleteArrays() { /* do nothing */;}
 
  Int_t Process();

  TVector3 GetPosition()  const { return fPosition[1]; }
  TVector3 GetDirection() const { return fDirection; }

  Double_t GetCurrentX() { return fRawPos[0]; }
  Double_t GetCurrentY() { return fRawPos[1]; }

  THcRaster();

 protected:

  //event information

  Int_t fNhits;

  /* void InitializeReconstruction(); */
  
  Double_t       fgpbeam;   //beam momentum
  Double_t       fgfrx_dist;     //Distance of raster to target
  Double_t       fgfry_dist;
  Double_t       fgbeam_xoff; // Beam offsets
  Double_t       fgbeam_xpoff; // Beam offsets
  Double_t       fgbeam_yoff; // Beam offsets
  Double_t       fgbeam_ypoff; // Beam offsets
  Double_t       fgbpmxa_slope; // BPM slope/offsets
  Double_t       fgbpmxa_off; // BPM slope/offsets
  Double_t       fgbpmxb_slope; // BPM slope/offsets
  Double_t       fgbpmxb_off; // BPM slope/offsets
  Double_t       fgbpmxc_slope; // BPM slope/offsets
  Double_t       fgbpmxc_off; // BPM slope/offsets
  Double_t       fgbpmya_slope; // BPM slope/offsets
  Double_t       fgbpmya_off; // BPM slope/offsets
  Double_t       fgbpmyb_slope; // BPM slope/offsets
  Double_t       fgbpmyb_off; // BPM slope/offsets
  Double_t       fgbpmyc_slope; // BPM slope/offsets
  Double_t       fgbpmyc_off; // BPM slope/offsets
  Double_t       fgbpma_zpos; // BPM A z position
  Double_t       fgbpmb_zpos; // BPM B z position
  Double_t       fgbpmc_zpos; // BPM C x position
  Int_t          fgusefr;   /* Use Raster for beam position */

  Double_t       FRXA_rawadc;  // XA raw ADC
  Double_t       FRYA_rawadc;  // YA raw ADC
  Double_t       FRXB_rawadc;  // XB raw ADC
  Double_t       FRYB_rawadc;  // YB raw ADC
  Double_t       BPMXA_raw; // BPM A Raw X Position
  Double_t       BPMYA_raw; // BPM A Raw Y Position
  Double_t       BPMXB_raw; // BPM B Raw X Position
  Double_t       BPMYB_raw; // BPM B Raw Y Position
  Double_t       BPMXC_raw; // BPM C Raw X Position
  Double_t       BPMYC_raw; // BPM C Raw Y Position
  Double_t       BPMXA_pos; // BPM A Corrected X Position
  Double_t       BPMYA_pos; // BPM A Corrected Y Position
  Double_t       BPMXB_pos; // BPM B Corrected X Position
  Double_t       BPMYB_pos; // BPM B Corrected Y Position
  Double_t       BPMXC_pos; // BPM C Corrected X Position
  Double_t       BPMYC_pos; // BPM C Corrected Y Position
  Double_t       fXA_ADC;     // XA ADC
  Double_t       fYA_ADC;     // YA ADC
  Double_t       fXB_ADC;     // XB ADC
  Double_t       fYB_ADC;     // YB ADC
  Double_t       fXA_pos;     // XA position
  Double_t       fYA_pos;     // YA position
  Double_t       fXB_pos;     // XB position
  Double_t       fYB_pos;     // YB position
  Double_t       fXbpm_tar;     // X BPM at target (+X is beam right)
  Double_t       fYbpm_tar;     // Y BPM at target  (+Y is up)
  Double_t       fXpbpm_tar;     // Xp BPM at target (+X is beam right)
  Double_t       fYpbpm_tar;     // Yp BPM at target  (+Y is up)
  Double_t       fXbpm_A;     // X BPM at BPMA (+X is beam right)
  Double_t       fYbpm_A;     // Y BPM at BPMA (+Y is up)
  Double_t       fXbpm_B;     // X BPM at BPMB (+X is beam right)
  Double_t       fYbpm_B;     // Y BPM at BPMB (+Y is up)
  Double_t       fXbpm_C;     // X BPM at BPMC (+X is beam right)
  Double_t       fYbpm_C;     // Y BPM at BPMC (+Y is up)
  Double_t       fXbeam_prev[4];     // 
  Double_t       fYbeam_prev[4];     // 
  Double_t       fXpbeam_prev;     // 
  Double_t       fYpbeam_prev;     // 
  Bool_t         fFlag_use_EPICS_bpm;
  //
  Double_t        fEbeamEpics;
  Double_t        fEbeamEpics_read;
  Double_t        fEbeamEpics_prev;
  //

  Double_t       fFrXA_ADC_zero_offset;
  Double_t       fFrYA_ADC_zero_offset;
  Double_t       fFrXB_ADC_zero_offset;
  Double_t       fFrYB_ADC_zero_offset;


  Double_t       fPedADC[4];     // ADC poedestals
  //Double_t       fAvgPedADC[4];     // Avergage ADC poedestals

  Double_t       fRawPos[2];     // current in Raster ADCs for position
  TVector3       fPosition[3];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3       fDirection;

  TClonesArray* frPosAdcPulseIntRaw;
  THaEpicsEvtHandler* fEpicsHandler;

 private:
  Bool_t    fAnalyzePedestals;
  Int_t     fNPedestalEvents;
  Double_t  fFrCalMom;
  Double_t  fFrXA_ADCperCM;
  Double_t  fFrYA_ADCperCM;
  Double_t  fFrXB_ADCperCM;
  Double_t  fFrYB_ADCperCM;

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};

////////////////////////////////////////////////////////////////////////////////


#endif
