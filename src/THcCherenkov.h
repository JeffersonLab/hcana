#ifndef ROOT_THcCherenkov
#define ROOT_THcCherenkov

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Cherenkov                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcCherenkovHit.h"

class THcCherenkov : public THaNonTrackingDetector, public THcHitList {

 public:
  THcCherenkov( const char* name, const char* description = "",
		THaApparatus* a = NULL );
  virtual ~THcCherenkov();
  
  virtual void 	     Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  void               InitArrays();
  void               DeleteArrays();
  virtual Int_t      ReadDatabase( const TDatime& date );
  virtual Int_t      DefineVariables( EMode mode = kDefine );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );

  virtual void AccumulatePedestals(TClonesArray* rawhits);
  virtual void CalculatePedestals();

  virtual Int_t      ApplyCorrections( void );

  virtual void Print(const Option_t* opt) const;

  Int_t GetCerIndex(Int_t nRegion, Int_t nValue);

  THcCherenkov();  // for ROOT I/O		
 protected:
  Int_t         fAnalyzePedestals;

  // Parameters
  Double_t*     fGain;
  Double_t*     fCerWidth;

  // Event information
  Int_t         fNhits;
  Int_t*        fNPMT;            // [fNelem] Array of ADC amplitudes
  Double_t*     fADC;             // [fNelem] Array of ADC amplitudes
  Double_t*     fADC_P;           // [fNelem] Array of ADC amplitudes
  Double_t*     fNPE;             // [fNelem] Array of ADC amplitudes
  Double_t      fNPEsum;          // [fNelem] Array of ADC amplitudes
  Double_t      fNCherHit;        // [fNelem] Array of ADC amplitudes

  Double_t*        fCerRegionValue;
  Double_t         fCerChi2Max;
  Double_t         fCerBetaMin;
  Double_t         fCerBetaMax;
  Double_t         fCerETMin;
  Double_t         fCerETMax;
  Double_t         fCerMirrorZPos;
  Int_t            fCerNRegions;
  Int_t            fCerRegionsValueMax;
  Int_t*           fCerTrackCounter;     // [fCerNRegions] Array of Cher regions
  Int_t*           fCerFiredCounter;     // [fCerNRegions] Array of Cher regions
  Double_t         fCerThresh;

  Int_t            fCerEvent;

  // Hits
  TClonesArray* fADCHits;

  // Pedestals
  Int_t         fNPedestalEvents;
  Int_t         fMinPeds;
  Int_t*        fPedSum;	  /* Accumulators for pedestals */
  Int_t*        fPedSum2;
  Int_t*        fPedLimit; 
  Double_t*     fPedMean; 	  /* Can be supplied in parameters and then */ 
  Int_t*        fPedCount; 
  Double_t*     fPed;
  Double_t*     fThresh;
  
  void Setup(const char* name, const char* description);
  virtual void  InitializePedestals( );

  ClassDef(THcCherenkov,0)        // Generic cherenkov class
};

#endif
