#ifndef ROOT_THcShowerArray
#define ROOT_THcShowerArray 

//#define HITPIC 1
#ifdef HITPIC
#define NPERLINE 5
#endif

//////////////////////////////////////////////////////////////////////////////
//                         
// THcShowerArray
//
// A Hall C Fly's Eye Shower Array
//
// Subdetector for the fly's eye part of the SHMS shower counter.
// 
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "TClonesArray.h"

#include <iostream>

#include <fstream>

class THaEvData;
class THaSignalHit;

class THcShowerArray : public THaSubDetector {

public:
  THcShowerArray( const char* name, const char* description,
		  Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcShowerArray();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
  Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);
  virtual Int_t AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit);
  virtual void  CalculatePedestals( );
  virtual void  InitializePedestals( );

  //  Double_t fSpacing;   not used

  TClonesArray* fParentHitList;

  // Return zero for now
  Double_t GetEplane() {
    return 0.0;
  };


protected:

#ifdef HITPIC
  char **hitpic;
  Int_t piccolumn;
#endif
  Double_t* fA;                 // [fNelem] ADC amplitude of blocks
  Double_t* fP;                 // [fNelem] Event by event pedestals

  TClonesArray* fADCHits;	// List of ADC hits

  // Parameters
  Int_t fNRows;
  Int_t fNColumns;
  Int_t fUsingFADC;		// != 0 if using FADC in sample mode
  Int_t fPedSampLow;		// Sample range for
  Int_t fPedSampHigh;		// dynamic pedestal
  Int_t fDataSampLow;		// Sample range for
  Int_t fDataSampHigh;		// sample integration

  Int_t fLayerNum;		// 2 for SHMS

  // Accumulators for pedestals go here

  Int_t fNPedestalEvents;	/* Pedestal event counter */
  Int_t fMinPeds;		/* Only analyze/update if num events > */

  // 2D arrays

  Int_t *fPedSum;		/* Accumulators for pedestals */
  Int_t *fPedSum2;
  Int_t *fPedLimit;          // Analyze pedestal if ADC signal < PedLimit
  Int_t *fPedCount;          // [fNelem] counter of pedestal analysis

  Float_t *fPed;             // [fNelem] pedestal positions
  Float_t *fSig;             // [fNelem] pedestal rms-s
  Float_t *fThresh;          // [fNelem] ADC thresholds

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  ClassDef(THcShowerArray,0); // Fly;s Eye calorimeter array
};
#endif
