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

  Int_t GetNBlocks(Int_t NLayer) const { return fNBlocks[NLayer];}

  //  friend class THaScCalib; not needed?

  Int_t GetPedLimit(Int_t NBlock, Int_t NLayer, Int_t Side) {
    if (Side!=0&&Side!=1) {
      cout << "*** Wrong Side in GetPedLimit:" << Side << " ***" << endl;
      return -1;
    }
    Int_t nelem = 0;
    for (Int_t i=0; i<NLayer; i++) nelem += fNBlocks[i];
    nelem += NBlock;
    return ( Side == 0 ? fShPosPedLimit[nelem] : fShNegPedLimit[nelem]);
  }

  Double_t GetGain(Int_t NBlock, Int_t NLayer, Int_t Side) {
    if (Side!=0&&Side!=1) {
      cout << "*** Wrong Side in GetGain:" << Side << " ***" << endl;
      return -1;
    }
    Int_t nelem = 0;
    for (Int_t i=0; i<NLayer; i++) nelem += fNBlocks[i];
    nelem += NBlock;
    return ( Side == 0 ? fPosGain[nelem] : fNegGain[nelem]);
  }

  Int_t GetMinPeds() {
    return fShMinPeds;
  }

  Int_t GetMult() {
    return fMult;
  }

  THcShower();  // for ROOT I/O

protected:

  Int_t fAnalyzePedestals;

  // ADC signal limits for pedestal calculations.
  Int_t* fShPosPedLimit;
  Int_t* fShNegPedLimit;

  Int_t fShMinPeds;   //Min.number of events to analize/update pedestals.

  // Calibration constants
  Double_t* fPosGain;
  Double_t* fNegGain;

  // Per-event data

  Int_t fNhits;    // Number of hits
  Double_t** fA;    // Raw ADC amplitudes
  Double_t** fA_p;  // Ped-subtracted ADC amplitudes
  Double_t** fA_c;  // Calibrated ADC amplitudes
  Double_t fAsum_p; // Sum of ped-subtracted ADCs
  Double_t fAsum_c; // Sum of calibrated ADCs
  Int_t fNclust;   // Number of clusters
  Double_t fE;      // Energy (MeV) of largest cluster
  Double_t fEpr;    // Preshower Energy (MeV) of largest cluster
  Double_t fX;      // x-position (cm) of largest cluster
  Double_t fZ;      // z-position (cm) of largest cluster
  Int_t fMult;     // Multiplicity of largest cluster
  Int_t fNblk;     // Number of blocks in main cluster
  Double_t* fEblk;  // Energies of blocks in main cluster
  Double_t fTRX;    // track x-position in det plane"
  Double_t fTRY;    // track y-position in det plane",

  // Potential Hall C parameters.  Mostly here for demonstration

  char** fLayerNames;
  Int_t fNLayers;
  //  Int_t fNRows;             fNBlocks is used instead
  Double_t* fNLayerZPos;	// Z position of front of shower counter layers
  Double_t* BlockThick;		// Thickness of shower counter blocks, blocks
  Int_t* fNBlocks;              // Number of shower counter blocks per layer
  Int_t fNtotBlocks;            // Total number of shower counter blocks
  Double_t** XPos;		//X,Y,Z positions of shower counter blocks
  Double_t* YPos;
  Double_t* ZPos;
  Int_t fNegCols; //number of columns with PMTTs on the negative side only.
  Double_t fSlop;               //Track to cluster vertical slop distance.
  Int_t fvTest;                 //fiducial volume test flag
  Int_t fdbg_clusters_cal;                 // Shower debug flag

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
 
