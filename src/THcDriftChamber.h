#ifndef ROOT_THcDriftChamber
#define ROOT_THcDriftChamber

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDriftChamber                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcDriftChamberPlane.h"
#include "TMath.h"

//class THaScCalib;
class TClonesArray;

class THcDriftChamber : public THaTrackingDetector, public THcHitList {

public:
  THcDriftChamber( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcDriftChamber();

  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      CoarseTrack( TClonesArray& tracks );
  virtual Int_t      FineTrack( TClonesArray& tracks );
  
  virtual Int_t      ApplyCorrections( void );

  //  Int_t GetNHits() const { return fNhit; }
  
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  Int_t GetNWires(Int_t plane) const { return fNWires[plane-1];}
  Int_t GetNChamber(Int_t plane) const { return fNChamber[plane-1];}
  Int_t GetWireOrder(Int_t plane) const { return fWireOrder[plane-1];}
  Int_t GetPitch(Int_t plane) const { return fPitch[plane-1];}
  Int_t GetCentralWire(Int_t plane) const { return fCentralWire[plane-1];}
  Int_t GetTdcWinMin(Int_t plane) const { return fTdcWinMin[plane-1];}
  Int_t GetTdcWinMax(Int_t plane) const { return fTdcWinMax[plane-1];}

  Double_t GetPlaneTimeZero(Int_t plane) const { return fPlaneTimeZero[plane-1];}

  Double_t GetNSperChan() const { return fNSperChan;}

  Double_t GetCenter(Int_t plane) const {
    Int_t chamber = GetNChamber(plane)-1;
    return
      fXCenter[chamber]*sin(fAlphaAngle[plane-1]) +
      fYCenter[chamber]*cos(fAlphaAngle[plane-1]);
  }
  //  friend class THaScCalib;

  THcDriftChamber();  // for ROOT I/O
protected:

  // Calibration

  // Per-event data

  // Potential Hall C parameters.  Mostly here for demonstration
  Int_t fNPlanes;
  char** fPlaneNames;
  Int_t fNChambers;

  Double_t fNSperChan;		/* TDC bin size */
  Double_t fWireVelocity;

  // Each of these will be dimensioned with the number of chambers
  Double_t* fXCenter;
  Double_t* fYCenter;

  // Each of these will be dimensioned with the number of planes
  // A THcDriftChamberPlane class object will need to access the value for
  // its plane number.  Should we have a Get method for each or 
  Int_t* fTdcWinMin;
  Int_t* fTdcWinMax;
  Int_t* fCentralTime;
  Int_t* fNWires;		// Number of wires per plane
  Int_t* fNChamber;
  Int_t* fWireOrder;
  Int_t* fDriftTimeSign;

  Double_t* fZPos;
  Double_t* fAlphaAngle;
  Double_t* fBetaAngle;
  Double_t* fGammaAngle;
  Double_t* fPitch;
  Double_t* fCentralWire;
  Double_t* fPlaneTimeZero;

  THcDriftChamberPlane** fPlanes; // List of plane objects

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

  void Setup(const char* name, const char* description);

  ClassDef(THcDriftChamber,0)   // Drift Chamber class
};

////////////////////////////////////////////////////////////////////////////////

#endif
