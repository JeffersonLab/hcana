#ifndef ROOT_THcHodoscope
#define ROOT_THcHodoscope

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawHodoHit.h"
#include "THcScintillatorPlane.h"
#include "THcShower.h"
#include "THcCherenkov.h"

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcSpacePoint.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include "TMath.h"

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <iostream>
#include <fstream>


class THaScCalib;

class THcHodoscope : public THaNonTrackingDetector, public THcHitList {

public:
  THcHodoscope( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcHodoscope();

  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  
  virtual Int_t      ApplyCorrections( void );
  Double_t GetStartTime() const { return fStartTime; }
  Bool_t IsStartTimeGood() const {return fGoodStartTime;};
  Int_t GetNfptimes() const {return fNfptimes;};
  Int_t GetScinIndex(Int_t nPlane, Int_t nPaddle);
  Int_t GetScinIndex(Int_t nSide, Int_t nPlane, Int_t nPaddle);
  Double_t GetPathLengthCentral();
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  Double_t GetTdcMin() const {return fScinTdcMin;}
  Double_t GetTdcMax() const {return fScinTdcMax;}
  Double_t GetTofTolerance() const {return fTofTolerance;}
  Double_t GetTdcToTime() const {return fScinTdcToTime;}
  Double_t GetBetaNominal() const {return fBetaNominal;}
  Double_t GetHodoPosPhcCoeff(Int_t iii) const {return fHodoPosPhcCoeff[iii];}
  Double_t GetHodoNegPhcCoeff(Int_t iii) const {return fHodoNegPhcCoeff[iii];}
  Double_t GetHodoPosMinPh(Int_t iii) const {return fHodoPosMinPh[iii];}
  Double_t GetHodoNegMinPh(Int_t iii) const {return fHodoNegMinPh[iii];}
  Double_t GetHodoPosTimeOffset(Int_t iii) const {return fHodoPosTimeOffset[iii];}
  Double_t GetHodoNegTimeOffset(Int_t iii) const {return fHodoNegTimeOffset[iii];}
  Double_t GetHodoVelLight(Int_t iii) const {return fHodoVelLight[iii];}
  Double_t GetStartTimeCenter() const {return fStartTimeCenter;}
  Double_t GetStartTimeSlop() const {return fStartTimeSlop;}
  Double_t GetBetaNotrk() const {return fBetaNotrk;}

  Int_t GetGoodRawPad(Int_t iii){return fTOFCalc[iii].good_raw_pad;}
  Double_t GetNScinHits(Int_t iii){return fNScinHits[iii];}

  UInt_t GetNPaddles(Int_t iii) { return fNPaddle[iii];}
  Double_t GetPlaneCenter(Int_t iii) { return fPlaneCenter[iii];}
  Double_t GetPlaneSpacing(Int_t iii) { return fPlaneSpacing[iii];}

  //  Double_t GetBeta() const {return fBeta[];}

  // Not used
  //Double_t GetBeta(Int_t itrack) const {return fBeta[itrack];} // Ahmed
  //  Int_t GetEvent(){ return fCheckEvent;}

  Double_t GetHodoPosSigma(Int_t iii) const {return fHodoPosSigma[iii];}
  Double_t GetHodoNegSigma(Int_t iii) const {return fHodoNegSigma[iii];}


  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  friend class THaScCalib;

  THcHodoscope();  // for ROOT I/O
protected:

  Int_t fAnalyzePedestals;

  // Calibration

  // Per-event data
  Bool_t fGoodStartTime;
  Double_t fStartTime; 
  Int_t fNfptimes;

  Double_t fBetaNotrk;
  // Per-event data

  // Potential Hall C parameters.  Mostly here for demonstration
  Int_t fNPlanes;		// Number of planes
  UInt_t fMaxScinPerPlane,fMaxHodoScin; // max number of scin/plane; product of the first two 
  Double_t fStartTimeCenter, fStartTimeSlop, fScinTdcToTime;
  Double_t fTofTolerance;
  Double_t fPathLengthCentral;
  Double_t fScinTdcMin, fScinTdcMax; // min and max TDC values
  char** fPlaneNames;
  UInt_t* fNPaddle;		// Number of paddles per plane

  Double_t* fHodoVelLight;
  Double_t* fHodoPosSigma;
  Double_t* fHodoNegSigma;

  Double_t* fHodoPosMinPh;
  Double_t* fHodoNegMinPh;
  Double_t* fHodoPosPhcCoeff;
  Double_t* fHodoNegPhcCoeff;
  Double_t* fHodoPosTimeOffset;
  Double_t* fHodoNegTimeOffset;
  Int_t* fHodoPosPedLimit;
  Int_t* fHodoNegPedLimit;
  Int_t fTofUsingInvAdc;
  Double_t* fHodoPosInvAdcOffset;
  Double_t* fHodoNegInvAdcOffset;
  Double_t* fHodoPosInvAdcLinear;
  Double_t* fHodoNegInvAdcLinear;
  Double_t* fHodoPosInvAdcAdc;
  Double_t* fHodoNegInvAdcAdc;

  Double_t fPartMass;		// Nominal particle mass
  Double_t fBetaNominal;	// Beta for central ray of nominal particle type

  THcScintillatorPlane** fPlanes; // List of plane objects

  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle

  //--------------------------   Ahmed   -----------------------------

  THcShower* fShower;
  THcCherenkov* fChern;


  Int_t        fCheckEvent;
  Int_t        fEventType;

  Int_t        fGoodTrack;
  Double_t     fScin2XZpos;
  Double_t     fScin2XdZpos;
  Double_t     fScin2YZpos;
  Double_t     fScin2YdZpos;

  Double_t     fChi2Min;
  Double_t*    fPlaneCenter;
  Double_t*    fPlaneSpacing;

  Double_t     fNormETot;
  Double_t     fNCerNPE;
  Int_t        fTestSum;
  Int_t        fTrackEffTestNScinPlanes;
  Int_t        fGoodScinHits;
  Int_t        fScinShould;
  Int_t        fScinDid;
  Int_t*       fxLoScin;
  Int_t*       fxHiScin;
  Int_t*       fyLoScin;
  Int_t*       fyHiScin;
  Int_t        fNHodoscopes;

  Int_t fHitSweet1X;
  Int_t fHitSweet1Y;
  Int_t fHitSweet2X;
  Int_t fHitSweet2Y;
  
  Int_t fSweet1XScin;
  Int_t fSweet1YScin;
  Int_t fSweet2XScin;
  Int_t fSweet2YScin;

  //  Double_t**   fScinHit;                // [fNPlanes] Array

  Double_t*    fFPTime;               // [fNPlanes] Array 


  Double_t* fSumPlaneTime; // [fNPlanes]

  Int_t* fNScinHits;  // [fNPlanes]
  Int_t* fNPlaneTime; // [fNPlanes]

  Bool_t* fGoodPlaneTime;  // [fNPlanes]

  TClonesArray* scinPosADC;
  TClonesArray* scinNegADC;
  TClonesArray* scinPosTDC;
  TClonesArray* scinNegTDC;

  //----------------------------------------------------------------

  // Useful derived quantities
  // double tan_angle, sin_angle, cos_angle;
  
  //  static const char NDEST = 2;
  //  struct DataDest {
  //    Int_t*    nthit;
  //    Int_t*    nahit;
  //    Double_t*  tdc;
  //    Double_t*  tdc_c;
  //    Double_t*  adc;
  //    Double_t*  adc_p;
  //    Double_t*  adc_c;
  //    Double_t*  offset;
  //    Double_t*  ped;
  //    Double_t*  gain;
  //  } fDataDest[NDEST];     // Lookup table for decoder

  // Used in TOF calculation (FineProcess) to hold information about hits
  // within a given plane
  struct TOFPInfo {
    Double_t time_pos;
    Double_t time_neg;
    Bool_t keep_pos;
    Bool_t keep_neg;
    Double_t adcPh;
    Double_t path;
    Double_t time;
    Double_t scin_pos_time;
    Double_t scin_neg_time;
    TOFPInfo () : time_pos(-99.0), time_neg(-99.0), keep_pos(kFALSE),
		  keep_neg(kFALSE), scin_pos_time(0.0), scin_neg_time(0.0) {}
  };
  std::vector<TOFPInfo> fTOFPInfo;
  
  // Used to hold information about all hits within the hodoscope for the TOF
  struct TOFCalc {
    Int_t hit_paddle;
    Int_t good_raw_pad;
    Bool_t good_scin_time;
    Bool_t good_tdc_pos;
    Bool_t good_tdc_neg;
    Double_t scin_time;
    Double_t scin_sigma;
    TOFCalc() : good_scin_time(kFALSE), good_tdc_pos(kFALSE),
		good_tdc_neg(kFALSE) {}
  };
  std::vector<TOFCalc> fTOFCalc;
    // This doesn't work because we clear this structure each track
    // Do we need an vector of vectors of structures?
    // Start with a separate vector of vectors for now.
  std::vector<std::vector<Double_t> > fdEdX;	        // Vector over track #
  std::vector<Int_t > fNScinHit;		        // # scins hit for the track
  std::vector<std::vector<Double_t> > fScinHitPaddle;	// Vector over hits in a plane #
  std::vector<Int_t > fNClust;		                // # scins clusters for the plane
  std::vector<Int_t > fThreeScin;	                // # scins three clusters for the plane
  std::vector<Int_t > fGoodScinHitsX;                   // # hits in fid x range
  // Could combine the above into a structure

  //
    
  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  Double_t DefineDoubleVariable(const char* fName);
  Int_t    DefineIntVariable(const char* fName);
  void DefineArray(const char* fName, const Int_t index, Double_t *myArray);
  void DefineArray(const char* fName, char** Suffix, const Int_t index, Double_t *myArray);
  void DefineArray(const char* fName, char** Suffix, const Int_t index, Int_t *myArray);
  enum ESide { kLeft = 0, kRight = 1 };
  
  virtual  Double_t TimeWalkCorrection(const Int_t& paddle,
					   const ESide side);
  void Setup(const char* name, const char* description);

  ClassDef(THcHodoscope,0)   // Hodoscope detector
};

////////////////////////////////////////////////////////////////////////////////

#endif
