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
#include "THcHodoscope.h"
#include "THaSpectrometer.h"
#include "THaTrack.h"

class THcCoinTime : public THaPhysicsModule {
public:
  THcCoinTime( const char* name, const char* description, const char* coinname);
  virtual ~THcCoinTime();

  virtual Int_t Begin( THaRunBase* r=0 );
  virtual Int_t End( THaRunBase* r=0 );
  virtual EStatus Init( const TDatime& run_time );
  virtual Int_t   Process( const THaEvData& );

  void            Reset( Option_t* opt="" );

protected:

  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  // Data needed for addind coincidence time as a Leaf Variable

  TString       fHodName;		// Name of hodoscope
  THcHodoscope* fHod;		// Hodscope object

  TString     fCoinName;         // Name of Coin Trigger
  THcTrigDet* fCoinTRG;         //Coin Trigger detector object
 
 
  THaSpectrometer* fSpectro;	// Spectrometer object

  Long64_t fNevt;

  // Information about the hodoscopes that we get from the
  // THcHodoscope object


  //Information about the trigger apparatus that we get from the
  //THc

 


  ClassDef(THcCoinTime,0) 	// Coincidence Time Module
};

#endif
