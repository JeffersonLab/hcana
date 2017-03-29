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
  THcAerogel(const char* name, const char* description = "", THaApparatus* a = NULL);
  virtual ~THcAerogel();

  virtual void 	  Clear(Option_t* opt="");
  virtual void    Print(const Option_t* opt) const;
  virtual void    AccumulatePedestals(TClonesArray* rawhits);
  virtual void    CalculatePedestals();
  virtual Int_t   Decode(const THaEvData&);
  virtual Int_t   ReadDatabase(const TDatime& date);
  virtual Int_t   DefineVariables(EMode mode = kDefine);
  virtual Int_t   CoarseProcess(TClonesArray& tracks);
  virtual Int_t   FineProcess(TClonesArray& tracks);
  virtual Int_t   ApplyCorrections(void);
  virtual EStatus Init(const TDatime& run_time);

  void InitArrays();
  void DeleteArrays();

  THcAerogel();  // for ROOT I/O
 protected:
  Int_t fAnalyzePedestals;
  Int_t fSixGevData;

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

  Int_t    fNGoodHits;
  Int_t    fNADCPosHits;
  Int_t    fNADCNegHits;
  Int_t    fNTDCPosHits;
  Int_t    fNTDCNegHits;
  Double_t fPosNpeSum;
  Double_t fNegNpeSum;
  Double_t fNpeSum;

  Double_t* fPosNpe;		// [fNelem] # Photoelectrons per positive tube
  Double_t* fNegNpe;		// [fNelem] # Photoelectrons per negative tube

  // Hits
  TClonesArray* fPosTDCHits;
  TClonesArray* fNegTDCHits;
  TClonesArray* fPosADCHits;
  TClonesArray* fNegADCHits;

  // 6 GeV era variables
  Int_t    fTdcOffset; /* Global TDC offset */
  Int_t    fNPedestalEvents;
  Int_t    fMinPeds;
  Int_t    *fPosPedSum;		/* Accumulators for pedestals */
  Int_t    *fPosPedSum2;
  Int_t    *fPosPedLimit;
  Int_t    *fPosPedCount;
  Int_t    *fNegPedSum;
  Int_t    *fNegPedSum2;
  Int_t    *fNegPedLimit;
  Int_t    *fNegPedCount;
  Double_t *fPosPed;
  Double_t *fPosSig;
  Double_t *fPosThresh;
  Double_t *fNegPed;
  Double_t *fNegSig;
  Double_t *fNegThresh;
  Double_t *fPosPedMean; 	/* Can be supplied in parameters and then */
  Double_t *fNegPedMean;	/* be overwritten from ped analysis */

  // Vector/TClonesArray length parameters
  Int_t MaxNumPosAeroPmt = 8;
  Int_t MaxNumNegAeroPmt = 8;
  Int_t MaxNumAdcPulse   = 4;

  // 12 GeV FADC variables
  TClonesArray* frPosAdcPedRaw;
  TClonesArray* frPosAdcPulseIntRaw;
  TClonesArray* frPosAdcPulseAmpRaw;
  TClonesArray* frPosAdcPulseTimeRaw;
  TClonesArray* frPosAdcPed;
  TClonesArray* frPosAdcPulseInt;
  TClonesArray* frPosAdcPulseAmp;

  TClonesArray* frNegAdcPedRaw;
  TClonesArray* frNegAdcPulseIntRaw;
  TClonesArray* frNegAdcPulseAmpRaw;
  TClonesArray* frNegAdcPulseTimeRaw;
  TClonesArray* frNegAdcPed;
  TClonesArray* frNegAdcPulseInt;
  TClonesArray* frNegAdcPulseAmp;

  TClonesArray* fPosAdcErrorFlag;
  TClonesArray* fNegAdcErrorFlag;

  void Setup(const char* name, const char* description);
  virtual void  InitializePedestals( );

  ClassDef(THcAerogel,0)   // Generic aerogel class
}
;

#endif
