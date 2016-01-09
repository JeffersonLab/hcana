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
#include "THcShowerArray.h"
#include "THcShowerHitCluster.h"
#include "TMath.h"

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
  
  Double_t GetNormETot();

  Int_t GetNHits() const { return fNhits; }
  
  Int_t GetNBlocks(Int_t NLayer) const { return fNBlocks[NLayer];}

  Double_t GetXPos(Int_t NLayer, Int_t NRow) const {
    return fXPos[NLayer][NRow];
  }

  Double_t GetYPos(Int_t NLayer, Int_t Side) const {

    //Side = 0 for postive (right) side
    //Side = 1 for negative (left) side

    return fYPos[2*NLayer+(1-Side)];
  }

  Double_t GetZPos(Int_t NLayer) const {return fLayerZPos[NLayer];}

  Double_t GetBlockThick(Int_t NLayer) {return BlockThick[NLayer];}

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
    //    return (fCcor + sign*y)/(fCcor + sign*y/fDcor);
    return (fCcor[side] + sign*y)/(fCcor[side] + sign*y/fDcor[side]);
  }

  // Get total energy deposited in the cluster matched to the given
  // spectrometer Track.

  Float_t GetShEnergy(THaTrack*);

  THcShower();  // for ROOT I/O

protected:

  Int_t fEvent;

  Int_t fAnalyzePedestals;   // Flag for pedestal analysis.

  Int_t* fShPosPedLimit;     // [fNTotBlocks] ADC limits for pedestal calc.-s.
  Int_t* fShNegPedLimit;

  Int_t fShMinPeds;          // Min.number of events to analyze pedestals.

  Double_t* fPosGain;        // [fNTotBlocks] Gain constants from calibration
  Double_t* fNegGain;

  // Per-event data

  Int_t fNhits;              // Total number of hits
  Int_t fNclust;             // Number of clusters
  Int_t fNtracks;            // Number of shower tracks, i.e. number of
                             // cluster-to-track association
  Double_t fEtot;            // Total energy 
  Double_t fEtotNorm;        // Total energy divided by spec central momentum 

  THcShowerClusterList* fClusterList;   // List of hit clusters


  // Geometrical parameters.

  char** fLayerNames;
  UInt_t fNLayers;	        // Number of layers in the calorimeter
  UInt_t fNTotLayers;	        // Number of layers including array
  UInt_t fHasArray;		// If !=0 fly's eye array behind preshower
  Double_t* fLayerZPos;	        // Z positions of fronts of layers
  // Following apply to just sideways readout layers
  Double_t* BlockThick;		// Thickness of blocks
  UInt_t* fNBlocks;              // [fNLayers] number of blocks per layer
  UInt_t fNTotBlocks;            // Total number of shower counter blocks
  Double_t** fXPos;		// [fNLayers] X,Y,Z positions of blocks
  Double_t* fYPos;
  Double_t* fZPos;
  UInt_t fNegCols;               // # of columns with neg. side PMTs only.
  Double_t fSlop;               // Track to cluster vertical slop distance.
  Int_t fvTest;                 // fiducial volume test flag for tracking
  Double_t fvDelta;             // Exclusion band width for fiducial volume

  Double_t fvXmin;              // Fiducial volume limits
  Double_t fvXmax;
  Double_t fvYmin;
  Double_t fvYmax;

  Int_t fdbg_raw_cal;          // Shower debug flags
  Int_t fdbg_decoded_cal;
  Int_t fdbg_sparsified_cal;
  Int_t fdbg_clusters_cal;
  Int_t fdbg_tracks_cal;
  Int_t fdbg_init_cal;         // No counterpart in engine, added to debug
                               // calorimeter initialization

  Double_t fAcor;               // Coordinate correction constants
  Double_t fBcor;
  Double_t fCcor[2];            // for positive ad negative side PMTs
  Double_t fDcor[2];

  THcShowerPlane** fPlanes;     // [fNLayers] Shower Plane objects
  THcShowerArray* fArray;

  TClonesArray*  fTrackProj;    // projection of track onto plane

  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  void Setup(const char* name, const char* description);

  // Cluster to track association method.
  Int_t MatchCluster(THaTrack*, Double_t&, Double_t&);

  void ClusterHits(THcShowerHitSet& HitSet, THcShowerClusterList* ClusterList);

  friend class THcShowerPlane;   //to access debug flags.
  friend class THcShowerArray;   //to access debug flags.

  ClassDef(THcShower,0)          // Shower counter detector
};

///////////////////////////////////////////////////////////////////////////////

// Various helper functions to accumulate hit related quantities.

Double_t addE(Double_t x, THcShowerHit* h);
Double_t addX(Double_t x, THcShowerHit* h);
Double_t addZ(Double_t x, THcShowerHit* h);
Double_t addEpr(Double_t x, THcShowerHit* h);

// Methods to calculate coordinates and energy depositions for a given cluster.

Double_t clX(THcShowerCluster* cluster);
Double_t clZ(THcShowerCluster* cluster);
Double_t clE(THcShowerCluster* cluster);
Double_t clEpr(THcShowerCluster* cluster);
Double_t clEplane(THcShowerCluster* cluster, Int_t iplane, Int_t side);

#endif
