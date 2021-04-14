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
  virtual Int_t  DefineVariables( EMode mode = kDefine );

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
  Double_t SHMS_RF_Offset;
  Double_t Bunch_Spacing_Override;

  Double_t Bunch_Spacing;
  Double_t Bunch_Spacing_Epics;
  Double_t HMS_RFtime; // HMS RF time
  Double_t SHMS_RFtime; // SHMS RF time
  Double_t HMS_FPtime;   //HMS focal plane time  
  Double_t SHMS_FPtime;   //SHMS focal plane time

  Double_t fHMS_RFtimeDist; // These are the two quantities we actually want to try to determine and produce in the end
  Double_t fSHMS_RFtimeDist;

  // Variables and stuff used in the CoinTime path length correction calculations
  // May want to re-use some of these in the future 
  //-----Declare Variables used in HMS/SHMS RF. time correction-----
  // This is just the coin timing stuff now, will need similar for RF
  //  Double_t lightSpeed;
  //Double_t elecMass;
  //Double_t positronMass;

  //hadron masses (the e- could be in coincidence with any of the hadrons)
  //Double_t protonMass;
  //Double_t kaonMass;
  //Double_t pionMass;
  
  // These variables and calculations are probably still good
  //Double_t SHMScentralPathLen;  
  //Double_t HMScentralPathLen;   

  //Double_t DeltaSHMSpathLength;
  //Double_t DeltaHMSpathLength;

  // Coin time values, not needed, delete soon
  /*
  Double_t fROC1_RAW_CoinTime;
  Double_t fROC2_RAW_CoinTime;
  Double_t fTRIG1_RAW_CoinTime;
  Double_t fTRIG4_RAW_CoinTime;
  
  Double_t fROC1_epCoinTime;
  Double_t fROC2_epCoinTime;
  Double_t fTRIG1_epCoinTime;
  Double_t fTRIG4_epCoinTime;

  Double_t fROC1_eKCoinTime;
  Double_t fROC2_eKCoinTime;
  Double_t fTRIG1_eKCoinTime;
  Double_t fTRIG4_eKCoinTime;

  Double_t fROC1_ePiCoinTime;
  Double_t fROC2_ePiCoinTime;
  Double_t fTRIG1_ePiCoinTime;
  Double_t fTRIG4_ePiCoinTime;
 
  Double_t fROC1_ePosCoinTime;   //electron-positron coin time 
  Double_t fROC2_ePosCoinTime;
  Double_t fTRIG1_ePosCoinTime;   //electron-positron coin time 
  Double_t fTRIG4_ePosCoinTime;
  */

  //Double_t elec_coinCorr;
  //Double_t elecArm_BetaCalc;
  //Double_t elec_hodFPtime;
  
  //Double_t had_coinCorr_proton;
  // Double_t hadArm_BetaCalc_proton;
  
  //Double_t had_coinCorr_Kaon;
  //Double_t hadArm_BetaCalc_Kaon;
   
  //Double_t had_coinCorr_Pion;
  //Double_t hadArm_BetaCalc_Pion;
  
  //Double_t had_coinCorr_Positron;
  //Double_t hadArm_BetaCalc_Positron;
  
  // Still needed as part of difference calculation? Keep? 
  //Double_t elec_P;     //electron golden track momentum
  //Double_t elec_dP;     //electron golden track delta-> (P-P0 / P0)
  //Double_t elec_xptar;    //electron golden track theta (xptar, :) 

  //Double_t had_P;     //hadron golden track momentum
  //Double_t had_xfp;      //hadron x-focal plane
  //Double_t had_xpfp;     //hadron xp focal plane
  //Double_t had_ypfp;     //hadron yp focal plane

  //--------------------------------------------------------------------

  ClassDef(THcRFTime,0) 	// RF Time Module
};

#endif
