#ifndef ROOT_THcCoinTime
#define ROOT_THcCoinTime

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcCoinTime                                                               //
//                                                                           //
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
  THcCoinTime( const char* name, const char* description, const char* hadArmName, 
	       const char* elecArmName, const char* coinname);

  virtual ~THcCoinTime();


  virtual EStatus Init( const TDatime& run_time );
  virtual Int_t   Process( const THaEvData& );

  void            Reset( Option_t* opt="" );
  

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

  THcHodoscope* fHod;	                // Hodscope object
 
  //-----Declare Variables used in HMS/SHMS Coin. time correction-----
  Double_t lightSpeed;
  Double_t elecMass;
  Double_t positronMass;

  //hadron masses (the e- could be in coincidence with any of the hadrons)
  Double_t protonMass;
  Double_t kaonMass;
  Double_t pionMass;
  
  Double_t SHMScentralPathLen;  //TODO: Put this in a param file, to be read in
  Double_t HMScentralPathLen;   //TODO: Put this in a param file, to be read in

  Double_t DeltaSHMSpathLength;
  Double_t DeltaHMSpathLength ;

  Double_t ROC1_epCoin;
  Double_t ROC2_epCoin;

  Double_t Elec_CentPL;       //E-ARM central path lenth
  Double_t Elec_coinCorr;
  Double_t elecArm_BetaCalc;
  Double_t elecArm_Delta_pathLength;
  Double_t elec_hodFPtime;
  Double_t pOffset;

  Double_t Had_CentPL;       //Hadron arm central path length
  Double_t Had_coinCorr;
  Double_t hadArm_BetaCalc;
  Double_t hadArm_Delta_pathLength;
  Double_t had_hodFPtime;

  Double_t elec_P;     //electron golden track momentum
  Double_t elec_dP;     //electron golden track delta-> (P-P0 / P0)
  Double_t elec_th;    //electron golden track theta (xptar, :) 
  Double_t elec_FPtime;   //electron focal plane time

  Double_t had_P;     //hadron golden track momentum
  Double_t had_xfp;      //hadron x-focal plane
  Double_t had_xpfp;     //hadron xp focal plane
  Double_t had_ypfp;     //hadron yp focal plane
  Double_t had_FPtime;   //hadron focal plane time

  //--------------------------------------------------------------------

  ClassDef(THcCoinTime,0) 	// Coincidence Time Module
};

#endif
