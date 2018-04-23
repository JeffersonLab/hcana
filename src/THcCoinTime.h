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

  // Data needed for addind coincidence time as a Leaf Variable


  
  TString     fHodName;		// Name of hodoscope
  TString     fCoinDetName;         // Name of Coin Trigger
  TString     fhadArmName;       //name of hadron arm
  TString     felecArmName;     // name of electron arm



  THcHallCSpectrometer* fhadSpectro;	// hadron Spectrometer object
  THcHallCSpectrometer* felecSpectro;	// electron Spectrometer object
  THcTrigDet* fCoinDet;                 //Coin Trigger detector object

  THcHodoscope* fHod;	                // Hodscope object
 
 

  Long64_t fNevt;

  // Information about the hodoscopes that we get from the
  // THcHodoscope object


  //Information about the trigger apparatus that we get from the
  //THc

 


  ClassDef(THcCoinTime,0) 	// Coincidence Time Module
};

#endif
