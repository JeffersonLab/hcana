#ifndef ROOT_THcHallCSpectrometer
#define ROOT_THcHallCSpectrometer

//////////////////////////////////////////////////////////////////////////
//
// THcHallCSpectrometer
//
//////////////////////////////////////////////////////////////////////////

#include "THaSpectrometer.h"

#include <vector>

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawHodoHit.h"
#include "THcScintillatorPlane.h"
#include "THcShower.h"

//#include "THaTrackingDetector.h"
//#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcSpacePoint.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include "TMath.h"

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <iostream>
#include <fstream>


//class THaScintillator;

class THcHallCSpectrometer : public THaSpectrometer {
  
public:
  THcHallCSpectrometer( const char* name, const char* description );
  virtual ~THcHallCSpectrometer();

  virtual Int_t   ReadDatabase( const TDatime& date );
  virtual Int_t   FindVertices( TClonesArray& tracks );
  virtual Int_t   TrackCalc();
  virtual Int_t   TrackTimes( TClonesArray* tracks );

  virtual Int_t   ReadRunDatabase( const TDatime& date );

  Bool_t SetTrSorting( Bool_t set = kFALSE );
  Bool_t GetTrSorting() const;

protected:
  void InitializeReconstruction();

  Int_t        MAXHODHITS;

  Int_t        fGoodTrack;
  Int_t        fSelUsingScin;
  Int_t        fNPlanes;
  Int_t        fNtracks;

  Int_t*    f2XHits;
  Int_t*    f2YHits;

  Double_t*    fX2D;
  Double_t*    fY2D;

  Double_t     fChi2Min;
  Double_t     fSelNDegreesMin;
  Double_t     fSeldEdX1Min;
  Double_t     fSeldEdX1Max;
  Double_t     fSelBetaMin;
  Double_t     fSelBetaMax;
  Double_t     fSelEtMin;
  Double_t     fSelEtMax;
  Double_t     fScin2XZpos;
  Double_t     fScin2XdZpos;
  Double_t     fScin2YZpos;
  Double_t     fScin2YdZpos;

  Double_t     fHodoCenter4, fHodoCenter3;
  Double_t     fScin2YSpacing, fScin2XSpacing;

  //  Int_t**   fHodScinHit;                // [4] Array

  THcShower* fShower;
  THcHodoscope* fHodo;

  // Should look at the ThaMatrixElement class in THaVDC.h for better way
  // to store matrix element data
#define fMaxReconElements 1000
  Int_t fNReconTerms;
  Double_t fReconCoeff[fMaxReconElements][4];
  Int_t fReconExponents[fMaxReconElements][5];
  Double_t fAngSlope_x;
  Double_t fAngSlope_y;
  Double_t fAngOffset_x;
  Double_t fAngOffset_y;
  Double_t fDetOffset_x;
  Double_t fDetOffset_y;
  Double_t fZTrueFocus;
  Double_t fThetaOffset; // Zero order term in yptar optics  matrix (rad)
  Double_t fPhiOffset; // Zero order term in xptar optics matrix (rad)
  Double_t fDeltaOffset; // Zero order term in delta optics matrix (%)
  Double_t fThetaCentralOffset; // Offset of Central spectrometer angle (rad)
  Double_t fOopCentralOffset; //Offset of central out-of-plane angle (rad) 
  Double_t fPCentralOffset; // Offset Central spectrometer momentum (%)
  Double_t fPCentral; // Central spectrometer momentum (GeV)
  Double_t fTheta_lab; // Central spectrometer angle (deg)

  //  THaScintillator *sc_ref;  // calculate time track hits this plane

  // Flag for fProperties indicating that tracks are to be sorted by chi2
  static const UInt_t kSortTracks = BIT(16);

  ClassDef(THcHallCSpectrometer,0) //A Hall C Spectrometer
};

#endif

