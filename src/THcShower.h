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
#include "THcShowerCluster.h"
#include "TMath.h"

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

  Int_t GetNHits() const { return fNhits; }
  
  // These are not used in the calorimeter code.
  //  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  //  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  Int_t GetNBlocks(Int_t NLayer) const { return fNBlocks[NLayer];}

  Double_t GetXPos(Int_t NLayer, Int_t NRaw) const {
    return XPos[NLayer][NRaw];
  }

  Double_t GetYPos(Int_t NLayer, Int_t Side) const {

    //Side = 0 for postive (right) side
    //Side = 1 for negative (left) side

    return YPos[2*NLayer+(1-Side)];
  }

  Double_t GetZPos(Int_t NLayer) const {return fNLayerZPos[NLayer];}

  Double_t GetBlockThick(Int_t NLayer) {return BlockThick[NLayer];}

  //  friend class THaScCalib; not needed for now.

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

  Int_t GetNLayers() {
    return fNLayers;
  }

  //Coordinate correction for single PMT modules.
  //PMT attached at right (positive) side.

  Float_t Ycor(Double_t y) {
    return TMath::Exp(y/fAcor)/(1. + y*y/fBcor);
  }

  //Coordinate correction for double PMT modules.
  //

  Float_t Ycor(Double_t y, Int_t side) {
    if (side!=0&&side!=1) {
      cout << "THcShower::Ycor : wrong side " << side << endl;
      return 0.;
    }
    Int_t sign = 1 - 2*side;
    return (fCcor + sign*y)/(fCcor + sign*y/fDcor);
  }

  THcShower();  // for ROOT I/O

protected:

  Int_t fAnalyzePedestals;   // Flag for pedestal analysis.

  Int_t* fShPosPedLimit;     // [fNtotBlocks] ADC limits for pedestal calc.-s.
  Int_t* fShNegPedLimit;

  Int_t fShMinPeds;          // Min.number of events to analyze pedestals.

  Double_t* fPosGain;        // [fNtotBlocks] Gain constants from calibration
  Double_t* fNegGain;

  // Per-event data

  Int_t fNhits;              // Total number of hits
  Int_t fNclust;             // Number of clusters
  Double_t fE;               // Energy of the largest cluster
  Double_t fEpr;             // Preshower Energy of the largest cluster
  Double_t fX;               // x-position (cm) of the largest cluster
  Double_t fZ;               // z-position (cm) of the largest cluster
  Int_t fMult;               // # of hits in the largest cluster
  //  Int_t fNblk;           // Number of blocks in main cluster
  //  Double_t* fEblk;       // Energies of blocks in main cluster

  // Track quantities, are here for test purposes. Better to move
  // to the track class(es) later on.

  Double_t fTRX;             // track x-position in det plane (1st track)
  Double_t fTRY;             // track y-position in det plane (1st track)
  Double_t fTRE;             // Energy of a cluster associated to the track
  Double_t fTREpr;           // Preshower Energy of the track's cluster
  Double_t fTRE_cor;         // Y-corrected track total energy
  Double_t fTREpr_cor;       // Y-corrected track Preshower energy
  Double_t* fTREpl_cor;      // Y-corrected track energy per plane
  Double_t* fTREpl_pos_cor;  // Y-corrected track positive energy per plane
  Double_t* fTREpl_neg_cor;  // Y-corrected track negative energy per plane

  // Potential Hall C parameters.  Mostly here for demonstration

  char** fLayerNames;
  Int_t fNLayers;               // Number of layers in the calorimeter
  Double_t* fNLayerZPos;	// Z positions of fronts of layers
  Double_t* BlockThick;		// Thickness of blocks
  Int_t* fNBlocks;              // [fNLayers] number of blocks per layer
  Int_t fNtotBlocks;            // Total number of shower counter blocks
  Double_t** XPos;		// [fNLayers] X,Y,Z positions of blocks
  Double_t* YPos;
  Double_t* ZPos;
  Int_t fNegCols;               // # of columns with neg. side PMTs only.
  Double_t fSlop;               // Track to cluster vertical slop distance.
  Int_t fvTest;                 // fiducial volume test flag for tracking
  Double_t fvDelta;             // Exclusion band width for fiducial volume

  Double_t fvXmin;              // Fiducial volume limits
  Double_t fvXmax;
  Double_t fvYmin;
  Double_t fvYmax;


  Int_t fdbg_decoded_cal;      // Shower debug flags
  Int_t fdbg_sparsified_cal;
  Int_t fdbg_clusters_cal;
  Int_t fdbg_tracks_cal;

  Double_t fAcor;               // Coordinate correction constants
  Double_t fBcor;
  Double_t fCcor;
  Double_t fDcor;

  THcShowerPlane** fPlanes;     // [fNLayers] Shower Plane objects

  TClonesArray*  fTrackProj;    // projection of track onto plane

  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  void Setup(const char* name, const char* description);

  // cluster to track association method.

  Int_t MatchCluster(THaTrack*, THcShowerClusterList*, Double_t&, Double_t&);

  friend class THcShowerPlane;   //to access debug flags.

  ClassDef(THcShower,0)          // Generic class
};

///////////////////////////////////////////////////////////////////////////////

#endif
