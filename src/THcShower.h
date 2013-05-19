#ifndef ROOT_THcShower
#define ROOT_THcShower

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcShower                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcShowerHit.h"
#include "THcShowerPlane.h"

class THaScCalib;

class THcShower : public THaNonTrackingDetector, public THcHitList {

public:
  THcShower( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcShower();
  virtual void 	     Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  
  virtual Int_t      ApplyCorrections( void );

  //  Int_t GetNHits() const { return fNhit; }
  
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  Int_t GetNBlocks(Int_t plane) const { return fNBlocks[plane-1];}

  //  friend class THaScCalib; not needed?

  Int_t fGetPedLimit(Int_t NBlock, Int_t NLayer, Int_t Side) {
    if (Side!=0&&Side!=1) {
      cout << "*** Wrong Side in fGetPedLimit:" << Side << " ***" << endl;
      return -1;
    }
    Int_t nelem = NBlock+NLayer*fNBlocks[NLayer];
    return ( Side == 0 ? fShPosPedLimit[nelem] : fShNegPedLimit[nelem]);
  }

  Int_t fGetMinPeds() {
    return fShMinPeds;
  }

  THcShower();  // for ROOT I/O

protected:

  Int_t fAnalyzePedestals;

  // ADC signal limits for pedestal calculations.
  Int_t* fShPosPedLimit;
  Int_t* fShNegPedLimit;

  Int_t fShMinPeds;   //Min.number of events to analize/update pedestals.

  // Calibration constants
  Double_t* fPosCalConst;
  Double_t* fNegCalConst;

  // Per-event data


  // Potential Hall C parameters.  Mostly here for demonstration

  char** fLayerNames;
  Int_t fNLayers;
  //  Int_t fNRows;             fNBlocks is used instead
  Double_t* fNLayerZPos;	// Z position of front of shower counter layers
  Double_t* BlockThick;		// Thickness of shower counter blocks, blocks
  Int_t* fNBlocks;              // Number of shower counter blocks per layer
  Int_t fNtotBlocks;            // Total number of shower counter blocks
  Double_t** YPos;		//X,Y positions of shower counter blocks
  Double_t* XPos;
  Int_t fNegCols; //number of columns with PMTTs on the negative side only.
  Double_t fSlop;               //Track to cluster vertical slop distance.
  Int_t fvTest;                 //fiducial volume test flag

  THcShowerPlane** fPlanes; // List of plane objects

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

  //  enum ESide { kLeft = 0, kRight = 1 };
  
  //  virtual  Double_t TimeWalkCorrection(const Int_t& paddle,
  //				       const ESide side);

  void Setup(const char* name, const char* description);

  ClassDef(THcShower,0)   // Generic hodoscope class
};

////////////////////////////////////////////////////////////////////////////////

#endif
 
