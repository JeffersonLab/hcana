#ifndef ROOT_THcDriftChamber
#define ROOT_THcDriftChamber

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDriftChamber                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcDCHit.h"
#include "THcDriftChamberPlane.h"

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
  
  //  friend class THaScCalib;

  THcDriftChamber();  // for ROOT I/O
protected:

  // Calibration

  // Per-event data


  // Potential Hall C parameters.  Mostly here for demonstration
  Int_t fNPlanes;
  char** fPlaneNames;
  Int_t fNChambers;
  Int_t* fNWires;		// Number of wires per plane

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
