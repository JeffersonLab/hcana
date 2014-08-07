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
  Int_t TrackEnergies( TClonesArray* tracks );

  virtual Int_t   ReadRunDatabase( const TDatime& date );

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

