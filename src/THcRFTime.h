#ifndef ROOT_THcRFTime
#define ROOT_THcRFTime

///////////////////////////////////////////////////////////////////////////////
// THcRFTime Physics Module                                                  //
// Author: Stephen JD Kay                                                    //
// University of Regina                                                      //
// Date: 12/04/21 (April 12th 2021)                                          //
// Based on THcCoinTime class from C Yero                                    //
///////////////////////////////////////////////////////////////////////////////

#include "THaEvData.h"
#include "THaCutList.h"
#include "VarDef.h"
#include "VarType.h"
#include "TClonesArray.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THaPhysicsModule.h"
#include "THaOutput.h"
#include "THcHitList.h"
#include "THcTrigDet.h" 
#include "THcHodoscope.h"
#include "THcHallCSpectrometer.h"
#include "THaTrack.h"
#include "THaEpicsEvtHandler.h"

class THcRFTime : public THaPhysicsModule {
public:
  // Single arm required? Single arm name specified
  THcRFTime( const char* name, const char* description, const char* hadArmName="", 
	       const char* elecArmName="", const char* RFname="");

  virtual ~THcRFTime();

  virtual EStatus Init( const TDatime& run_time );
  virtual Int_t  Process( const THaEvData& );

  void            Reset( Option_t* opt="" );
  void            Clear( Option_t* opt="" );
  
 protected:

  // Event Information
  Int_t fNhits;

  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t DefineVariables( EMode mode = kDefine );

  // Data needed for adding RF time dist as a Leaf Variable
  // Need offset and bunch spacing too
  TString     fHodName;		// Name of hodoscope
  TString     fCoinDetName;     // Name of Coin Trigger
  TString     fhadArmName;      // Name of hadron arm
  TString     felecArmName;     // Name of electron arm
  
  // A bit misleading for these to be "hadron" and "electron" spectrometers
  THcHallCSpectrometer* fhadSpectro;	// Hadron Spectrometer object
  THcHallCSpectrometer* felecSpectro;	// Electron Spectrometer object
  THcTrigDet* fCoinDet;                 // Coin Trigger detector object
  THaEpicsEvtHandler* fEpicsHandler; // Not working, need another include?

  THaTrack* theSHMSTrack;
  THaTrack* theHMSTrack;

  THcHodoscope* fHod;	                // Hodscope object

  Double_t HMS_RF_Offset;
  Double_t HMS_eRF_Offset;
  Double_t HMS_piRF_Offset;
  Double_t HMS_KRF_Offset;
  Double_t HMS_pRF_Offset;
  Double_t SHMS_RF_Offset;
  Double_t SHMS_eRF_Offset;
  Double_t SHMS_piRF_Offset;
  Double_t SHMS_KRF_Offset;
  Double_t SHMS_pRF_Offset;
  Double_t Bunch_Spacing_Override;

  Double_t Bunch_Spacing;
  Double_t Bunch_Spacing_Epics;
  Double_t HMS_RFtime; // HMS RF time
  Double_t SHMS_RFtime; // SHMS RF time

  Double_t fHMS_RFtimeDist; // These are the two quantities we actually want to try to determine and produce in the end
  Double_t fSHMS_RFtimeDist;

  // SJDK 21/10/22 - Adding in mass corrected RF times for HMS/SHMS. 4 particle species, e/pi/K/p
  Double_t fHMS_eRFtimeDist;
  Double_t fHMS_piRFtimeDist;
  Double_t fHMS_KRFtimeDist;
  Double_t fHMS_pRFtimeDist;

  Double_t fSHMS_eRFtimeDist;
  Double_t fSHMS_piRFtimeDist;
  Double_t fSHMS_KRFtimeDist;
  Double_t fSHMS_pRFtimeDist;

  //-----Declare Variables used in HMS/SHMS RF time correction-----
  Double_t lightSpeed;
  Double_t elecMass;
  Double_t protonMass;
  Double_t kaonMass;
  Double_t pionMass;

  Double_t HMS_CentralPathLen;  
  Double_t HMS_DeltaPathLen;
  Double_t SHMS_CentralPathLen;  
  Double_t SHMS_DeltaPathLen;

  Double_t HMS_Beta_Calc_e;
  Double_t HMS_Beta_Calc_pi;
  Double_t HMS_Beta_Calc_K;
  Double_t HMS_Beta_Calc_p;
  Double_t SHMS_Beta_Calc_e;
  Double_t SHMS_Beta_Calc_pi;
  Double_t SHMS_Beta_Calc_K;
  Double_t SHMS_Beta_Calc_p;

  Double_t HMS_RFCentralTime_e;
  Double_t HMS_RFCentralTime_pi;
  Double_t HMS_RFCentralTime_K;
  Double_t HMS_RFCentralTime_p;
  Double_t SHMS_RFCentralTime_e;
  Double_t SHMS_RFCentralTime_pi;
  Double_t SHMS_RFCentralTime_K;
  Double_t SHMS_RFCentralTime_p;

  Double_t HMS_RFDeltaTime_e;
  Double_t HMS_RFDeltaTime_pi;
  Double_t HMS_RFDeltaTime_K;
  Double_t HMS_RFDeltaTime_p;
  Double_t SHMS_RFDeltaTime_e;
  Double_t SHMS_RFDeltaTime_pi;
  Double_t SHMS_RFDeltaTime_K;
  Double_t SHMS_RFDeltaTime_p;
  
  ClassDef(THcRFTime,0) 	// RF Time Module
};

#endif
