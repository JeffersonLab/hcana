#ifndef ROOT_THcHodoEff
#define ROOT_THcHodoEff

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoEff                                                                //
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

#include "Logger.h"

class THcHodoEff : public hcana::ConfigLogging<THaPhysicsModule> {
public:
  THcHodoEff( const char* name, const char* description, const char* hodname);
  virtual ~THcHodoEff();

  virtual Int_t Begin( THaRunBase* r=0 );
  virtual Int_t End( THaRunBase* r=0 );
  virtual EStatus Init( const TDatime& run_time );
  virtual Int_t   Process( const THaEvData& );

  void            Reset( Option_t* opt="" );

protected:

  virtual Int_t ReadDatabase( const TDatime& date);
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  /* Int_t GetScinIndex(Int_t nPlane, Int_t nPaddle); */

  // Data needed for efficiency calculation for one Hodoscope paddle

  Double_t* fZPos;		//

  TString       fName;		// Name of hodoscope
  THcHodoscope* fHod;		// Hodscope object
  THaSpectrometer* fSpectro;	// Spectrometer object

  Long64_t fNevt;

  // Information about the hodoscopes that we get from the
  // THcHodoscope object

  Int_t fEffiTest;
  Int_t fNPlanes;
  THcScintillatorPlane** fPlanes;
  Double_t* fPosZ;
  Double_t* fSpacing;
  Double_t* fCenterFirst;
  Int_t* fNCounters;
  //  Int_t* fHodoPlnContHit;
  Int_t* fHodoPosEffi;
  Int_t* fHodoNegEffi;
  Int_t* fHodoOrEffi;
  Int_t* fHodoAndEffi;
  Int_t* fStatTrk;
  Int_t* fStatTrkSum;
  Int_t* fStatAndSum;
  Double_t* fStatAndEff;
  Double_t fStatSlop;
  Double_t fHodoEff_CalEnergy_Cut;
  Double_t fMaxChisq;
  Double_t* fHodoSlop;
  Double_t fHodoEff_s1,fHodoEff_s2,fHodoEff_tof,fHodoEff_3_of_4,fHodoEff_4_of_4;

  // Arrays for accumulating statistics
  vector<vector<vector<Int_t> > > fHitShould;
  vector<vector<vector<Int_t> > > fStatAndHitDel;
  vector<vector<vector<Int_t> > > fStatTrkDel;
  vector<vector<Int_t> > fStatPosHit;
  vector<vector<Int_t> > fStatNegHit;
  vector<vector<Int_t> > fStatAndHit;
  vector<vector<Int_t> > fStatOrHit;
  vector<vector<Int_t> > fBothGood;
  vector<vector<Int_t> > fNegGood;
  vector<vector<Int_t> > fPosGood;

  Int_t* fHitPlane;

  ClassDef(THcHodoEff,0) 	// Hodoscope efficiency module
};

#endif
