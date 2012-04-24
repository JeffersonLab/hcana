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

  virtual Int_t   FindVertices( TClonesArray& tracks );
  virtual Int_t   TrackCalc();
  virtual Int_t   TrackTimes( TClonesArray* tracks );

  Bool_t SetTrSorting( Bool_t set = kFALSE );
  Bool_t GetTrSorting() const;
  
protected:
  //  THaScintillator *sc_ref;  // calculate time track hits this plane

  // Flag for fProperties indicating that tracks are to be sorted by chi2
  static const UInt_t kSortTracks = BIT(16);

  ClassDef(THcHallCSpectrometer,0) //A Hall A High Resolution Spectrometer
};

#endif

