#ifndef ROOT_THcAerogel
#define ROOT_THcAerogel

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcAerogel                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcAerogelHit.h"

class THcAerogel : public THaNonTrackingDetector, public THcHitList {

 public:
  THcAerogel( const char* name, const char* description = "",
		THaApparatus* a = NULL );
  virtual ~THcAerogel() {};
  
  virtual void 	     Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      ReadDatabase( const TDatime& date );
  virtual Int_t      DefineVariables( EMode mode = kDefine );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );

  
  virtual void AccumulatePedestals(TClonesArray* rawhits);
  virtual void CalculatePedestals();

  virtual Int_t      ApplyCorrections( void );

  THcAerogel();  // for ROOT I/O		
 protected:
  Int_t fAnalyzePedestals;

  // Parameters
  Double_t* fPosGain;
  Double_t* fNegGain;

  Int_t fNPedestalEvents;
  Int_t fMinPeds;
  Int_t *fPosPedSum;		/* Accumulators for pedestals */
  Int_t *fPosPedSum2;
  Int_t *fPosPedLimit;
  Int_t *fPosPedCount;
  Int_t *fNegPedSum;
  Int_t *fNegPedSum2;
  Int_t *fNegPedLimit;
  Int_t *fNegPedCount;

  Double_t *fPosPed;
  Double_t *fPosSig;
  Double_t *fPosThresh;
  Double_t *fNegPed;
  Double_t *fNegSig;
  Double_t *fNegThresh;

  void Setup(const char* name, const char* description);
  virtual void  InitializePedestals( );

  ClassDef(THcAerogel,0)   // Generic aerogel class
};

#endif
