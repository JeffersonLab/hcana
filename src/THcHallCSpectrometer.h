#ifndef ROOT_THcHallCSpectrometer
#define ROOT_THcHallCSpectrometer

//////////////////////////////////////////////////////////////////////////
//
// THcHallCSpectrometer
//
//////////////////////////////////////////////////////////////////////////

#include "THaSpectrometer.h"

//class THaScintillator;

class THcHallCSpectrometer : public THaSpectrometer {
  
public:
  THcHallCSpectrometer( const char* name, const char* description );
  virtual ~THcHallCSpectrometer();

  virtual Int_t   ReadDatabase( const TDatime& date );
  virtual Int_t   FindVertices( TClonesArray& tracks );
  virtual Int_t   TrackCalc();
  virtual Int_t   TrackTimes( TClonesArray* tracks );

  Bool_t SetTrSorting( Bool_t set = kFALSE );
  Bool_t GetTrSorting() const;

protected:
  void InitializeReconstruction();

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
  Double_t fThetaOffset;
  Double_t fPhiOffset;
  Double_t fDeltaOffset;
  Double_t fPCentral;

  //  THaScintillator *sc_ref;  // calculate time track hits this plane

  // Flag for fProperties indicating that tracks are to be sorted by chi2
  static const UInt_t kSortTracks = BIT(16);

  ClassDef(THcHallCSpectrometer,0) //A Hall A High Resolution Spectrometer
};

#endif

