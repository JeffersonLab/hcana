#ifndef ROOT_THcHodoscope
#define ROOT_THcHodoscope

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcHodoscopeHit.h"
#include "THcScintillatorPlane.h"

class THaScCalib;

class THcHodoscope : public THaNonTrackingDetector, public THcHitList {

public:
  THcHodoscope( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcHodoscope();

  virtual void  Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  
  virtual Int_t      ApplyCorrections( void );

  //  Int_t GetNHits() const { return fNhit; }
  
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  const TClonesArray* GetTrackHits() const { return fTrackProj; }
  
  friend class THaScCalib;

  THcHodoscope();  // for ROOT I/O
protected:

  Int_t fAnalyzePedestals;

  // Calibration

  // Per-event data


  // Potential Hall C parameters.  Mostly here for demonstration
  Int_t fNPlanes;
  Double_t fStartTimeCenter, fStartTimeSlop, fScinTdcToTime;
  Double_t fTofTolerance;
  Int_t fScinTdcMin, fScinTdcMax; // min and max TDC values
  Double_t* fHodoSlop;
  Double_t** fHodoVelLight;
  Double_t** fHodoPosSigma;
  Double_t** fHodoNegSigma;

  char** fPlaneNames;
  Int_t* fNPaddle;		// Number of paddles per plane
  Double_t* fSpacing;		// Paddle spacing in cm
  Double_t** fCenter;           // Center position of each paddle

  THcScintillatorPlane** fPlanes; // List of plane objects

  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle
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

  ClassDef(THcHodoscope,0)   // Generic hodoscope class
};

////////////////////////////////////////////////////////////////////////////////

#endif
