#ifndef ROOT_THcCoinTime
#define ROOT_THcCoinTime

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcCoinTime Physics Module                                                //
// Author: Carlos Yero                                                       //
// Date: April 27, 2018                                                      //
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
#include "THcTrigDet.h" 
#include "THcHodoscope.h"
//#include "THaSpectrometer.h"
#include "THcHallCSpectrometer.h"
#include "THaTrack.h"

class THcCoinTime : public THaPhysicsModule {
public:
  THcCoinTime( const char* name, const char* description, const char* hadArmName="", 
	       const char* elecArmName="", const char* coinname="");

  virtual ~THcCoinTime();

  virtual EStatus Init( const TDatime& run_time );
  virtual Int_t   Process( const THaEvData& );

  void            Reset( Option_t* opt="" );
  void            Clear( Option_t* opt="" );
  
 protected:

  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  // Data needed for adding coincidence time as a Leaf Variable
  
  TString     fHodName;		// Name of hodoscope
  TString     fCoinDetName;         // Name of Coin Trigger
  TString     fhadArmName;       //name of hadron arm
  TString     felecArmName;     // name of electron arm

  THcHallCSpectrometer* fhadSpectro;	// hadron Spectrometer object
  THcHallCSpectrometer* felecSpectro;	// electron Spectrometer object
  THcTrigDet* fCoinDet;                 //Coin Trigger detector object

  THaTrack* theSHMSTrack;
  THaTrack* theHMSTrack;

  THcHodoscope* fHod;	                // Hodscope object
 
  //-----Declare Variables used in HMS/SHMS Coin. time correction-----
  Double_t lightSpeed;
  Double_t elecMass;
  Double_t positronMass;

  //hadron masses (the e- could be in coincidence with any of the hadrons)
  Double_t protonMass;
  Double_t kaonMass;
  Double_t pionMass;

  Double_t eHad_CT_Offset;  //e-Hadron coin time Offset

  Double_t SHMScentralPathLen;  
  Double_t HMScentralPathLen;   

  Double_t DeltaSHMSpathLength;
  Double_t DeltaHMSpathLength;

  Double_t fROC1_RAW_CoinTime;
  Double_t fROC2_RAW_CoinTime;
  Double_t fSHMS_RAW_CoinTime;
  Double_t fHMS_RAW_CoinTime;  

  Double_t fROC1_RAW_CoinTime_NoTrack;
  Double_t fROC2_RAW_CoinTime_NoTrack;
  Double_t fSHMS_RAW_CoinTime_NoTrack;
  Double_t fHMS_RAW_CoinTime_NoTrack;  
  
  Double_t fROC1_epCoinTime;
  Double_t fROC2_epCoinTime;
  Double_t fSHMS_epCoinTime;
  Double_t fHMS_epCoinTime;

  Double_t fROC1_eKCoinTime;
  Double_t fROC2_eKCoinTime;
  Double_t fSHMS_eKCoinTime;
  Double_t fHMS_eKCoinTime;

  Double_t fROC1_ePiCoinTime;
  Double_t fROC2_ePiCoinTime;
  Double_t fSHMS_ePiCoinTime;
  Double_t fHMS_ePiCoinTime;
 
  Double_t fROC1_ePosCoinTime;   //electron-positron coin time 
  Double_t fROC2_ePosCoinTime;
  Double_t fSHMS_ePosCoinTime;   //electron-positron coin time 
  Double_t fHMS_ePosCoinTime;
  
  Double_t elec_coinCorr;
  Double_t elecArm_BetaCalc;
  Double_t elec_hodFPtime;
  
  Double_t had_coinCorr_proton;
  Double_t hadArm_BetaCalc_proton;
  
  Double_t had_coinCorr_Kaon;
  Double_t hadArm_BetaCalc_Kaon;
   
  Double_t had_coinCorr_Pion;
  Double_t hadArm_BetaCalc_Pion;
  
  Double_t had_coinCorr_Positron;
  Double_t hadArm_BetaCalc_Positron;
  
  Double_t elec_P;     //electron golden track momentum
  Double_t elec_dP;     //electron golden track delta-> (P-P0 / P0)
  Double_t elec_xptar;    //electron golden track theta (xptar, :) 
  Double_t elec_FPtime;   //electron focal plane time

  Double_t had_P;     //hadron golden track momentum
  Double_t had_xfp;      //hadron x-focal plane
  Double_t had_xpfp;     //hadron xp focal plane
  Double_t had_ypfp;     //hadron yp focal plane
  Double_t had_FPtime;   //hadron focal plane time

  // trigger times pTrig1 (SHMS 3/4 trig) and pTrig3 (HMS 3/4 trig)
  Double_t pSHMS_TdcTime_ROC1;
  Double_t pHMS_TdcTime_ROC1;
  Double_t pSHMS_TdcTime_ROC2;
  Double_t pHMS_TdcTime_ROC2;

  //--------------------------------------------------------------------

  ClassDef(THcCoinTime,0) 	// Coincidence Time Module
};

#endif
