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
  virtual Int_t      ReadDatabase( const TDatime& date );
  virtual Int_t      DefineVariables( EMode mode = kDefine );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );

  virtual void AccumulatePedestals(TClonesArray* rawhits);
  virtual void CalculatePedestals();

  virtual Int_t      ApplyCorrections( void );

  virtual void Print(const Option_t* opt) const;

  THcCherenkov();  // for ROOT I/O		
 protected:
  Int_t fAnalyzePedestals;

  // Parameters
  Double_t* fPosGain;
  Double_t* fNegGain;

  // Event information
  Int_t fNhits;

  Float_t*   fA_Pos;         // [fNelem] Array of ADC amplitudes
  Float_t*   fA_Neg;         // [fNelem] Array of ADC amplitudes
  Float_t*   fA_Pos_p;	     // [fNelem] Array of ped-subtracted ADC amplitudes
  Float_t*   fA_Neg_p;	     // [fNelem] Array of ped-subtracted ADC amplitudes
  Float_t*   fT_Pos;         // [fNelem] Array of TDCs
  Float_t*   fT_Neg;         // [fNelem] Array of TDCs

  Float_t   fA_1;         // Ahmed
  Float_t   fA_2;         // Ahmed
  Float_t   fA_p_1;         // Ahmed
  Float_t   fA_p_2;         // Ahmed
  Double_t fNpe_1;		// Ahmed
  Double_t fNpe_2;		// Ahmed
  Int_t fNHits_1;         // Ahmed
  Int_t fNHits_2;         // Ahmed

  Double_t fPosNpeSum;
  Double_t fNegNpeSum;
  Double_t fNpeSum;
  Int_t fNGoodHits;
  Int_t fNADCPosHits;
  Int_t fNADCNegHits;
  Int_t fNTDCPosHits;
  Int_t fNTDCNegHits;

  Double_t* fPosNpe;		// [fNelem] # Photoelectrons per positive tube
  Double_t* fNegNpe;		// [fNelem] # Photoelectrons per negative tube


  // Hits
  TClonesArray* fPosTDCHits;
  TClonesArray* fNegTDCHits;
  TClonesArray* fPosADCHits;
  TClonesArray* fNegADCHits;

  // Pedestals
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

  Double_t *fPosPedMean; 	/* Can be supplied in parameters and then */
  Double_t *fNegPedMean;	/* be overwritten from ped analysis */
  
  void Setup(const char* name, const char* description);
  virtual void  InitializePedestals( );

  ClassDef(THcCherenkov,0)   // Generic cherenkov class
};

#endif
