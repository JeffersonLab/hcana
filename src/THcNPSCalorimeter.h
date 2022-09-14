#ifndef ROOT_THcNPSCalorimeter
#define ROOT_THcNPSCalorimeter

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcNPSCalorimeter                                                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//#include <tuple> 
#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcNPSArray.h"
#include "THcNPSShowerHit.h"
#include "THcNPSAnalyzer.h"
#include "TMath.h"

class THcNPSCalorimeter : public THaNonTrackingDetector, public THcHitList {

public:
  THcNPSCalorimeter( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcNPSCalorimeter();
  virtual void 	     Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      CoarseProcess(TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );

  Double_t GetNormETot();

  Int_t GetNHits() const { return fNhits; }

  Int_t GetADCMode() {
    return fADCMode;
  }
  Double_t GetAdcTdcOffset() {
    return fAdcTdcOffset;
  }
  Int_t GetMinPeds() {
    return fShMinPeds;
  }


  
  /*  ---- C.Y. I thing these A,B,C,D, layer correction is ONLY for HMS and pre-shower but NOT for NPS
  //Coordinate correction for HMS single PMT modules.
  //PMT attached at right (positive) side.

  Float_t Ycor(Double_t y) {
    return TMath::Exp(y/fAcor)/(1. + y*y/fBcor);
  }

  //Coordinate correction for HMS double PMT modules.
  //

  Float_t Ycor(Double_t y, Int_t side) {
    if (side!=0&&side!=1) {
      cout << "THcNPSCalorimeter::Ycor : wrong side " << side << endl;
      return 0.;
    }
    Int_t sign = 1 - 2*side;
    //    return (fCcor + sign*y)/(fCcor + sign*y/fDcor);
    return (fCcor[side] + sign*y)/(fCcor[side] + sign*y/fDcor[side]);
  }

  // Coordinate correction for SHMS Preshower modules.
  //
  
  Float_t YcorPr(Double_t y, Int_t side) {

    if (side!=0&&side!=1) {
      cout << "THcNPSCalorimeter::YcorPr : wrong side " << side << endl;
      return 0.;
    }

    Float_t cor;

    // Check if the hit coordinate matches the fired block's side.

    if ((y < 0. && side == 1) || (y > 0. && side == 0))
      cor = 1./(1. + TMath::Power(TMath::Abs(y)/fAcor, fBcor));
    else
      cor = 1.;

    return cor;
  }
  -------------------------------*/

  // Get part of energy deposited in the cluster matched to the given
  // spectrometer Track, limited by a range of layers.

  // Float_t GetShEnergy(THaTrack*, UInt_t NLayers, UInt_t L0=0);

  THcNPSCalorimeter();  // for ROOT I/O

protected:

  //C.Y. Feb 22, 2021 : Added output filestream for the purpose of
  //writing to file (and plotting a 2D grid of) the blocks
  //that participate on cluster formation
  ofstream *fOutFile;
  Int_t fMakeGrid; //If true (1), the write fOutFile to make 2D Grid

  //calorimeter geometry (to determine total number of elements)
  UInt_t fNRows;
  UInt_t fNColumns;
  
  Bool_t* fPresentP;
  Int_t fEvent;
  Int_t fADCMode;		//   != 0 if using FADC 
   //  1 == Use the pulse int - pulse ped
    //  2 == Use the sample integral - known ped
    //  3 == Use the sample integral - sample ped
  static const Int_t kADCStandard=0;
  static const Int_t kADCDynamicPedestal=1;
  static const Int_t kADCSampleIntegral=2;
  static const Int_t kADCSampIntDynPed=3;
  Double_t fAdcTdcOffset;

  Int_t fAnalyzePedestals;   // Flag for pedestal analysis.

  Int_t fShMinPeds;          // Min.number of events to analyze pedestals.

  // Per-event data

  Int_t fNhits;              // Total number of hits
  Int_t fNclust;             // Number of clusters
  Int_t fNblockHighEnergy;             // NUmber of array block (1-224) that has the highest energy in cluster
  Double_t fEtot;            // Total energy
  Double_t fEtotNorm;        // Total energy divided by spec central momentum
  Double_t fEtrack;          // Cluster energy associated to the best track
  Double_t fEtrackNorm;      // Cluster energy divided by momentum for the best track
  Double_t fEPRtrack;        // Preshower part of cluster energy of the best track
  Double_t fEPRtrackNorm;    // Preshower part of cluster energy divided by momentum for the best track
  Double_t fETotTrackNorm;   // Total energy divided by momentum of the best track

  THcNPSShowerClusterList* fClusterList;   // List of hit clusters

  // For making fake NPS data
  Int_t quadrant;               //variable to store [0-9] corresponding to different quadrants on NPS
  Int_t fCalReplicate;		// 1: translate data to a random quadrant
                                // 2: reflect data to a random quadrant
  THcNPSAnalyzer *fAnalyzer;

  Int_t fClustMethod;

  
  // Geometrical parameters.

  char** fLayerNames;
  UInt_t fNTotLayers;	        // Number of layers including array
  UInt_t fHasArray;		// If !=0 fly's eye array behind preshower
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

  /* --C.Y. I think this can be removed (need to ask S. Wood)
  Double_t fAcor;               // Coordinate correction constants
  Double_t fBcor;
  Double_t fCcor[2];            // for positive ad negative side PMTs
  Double_t fDcor[2];
  */
 
  THcNPSArray* fArray;

  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  void Setup(const char* name, const char* description);

  std::string fKwPrefix;

  // Cluster to track association method.
  //  Int_t MatchCluster(THaTrack*, Double_t&, Double_t&);

  void ClusterHits(THcNPSShowerHitSet& HitSet, THcNPSShowerClusterList* ClusterList);

  //C.Y. Feb 09, 2021 : create method for NPS clustering using cellular automata
  void ClusterNPS_Hits(THcNPSShowerHitSet& HitSet, THcNPSShowerClusterList* ClusterList);
  
  virtual Int_t      End(THaRunBase *r = 0);

  //  friend class THcShowerPlane;   //to access debug flags.
  friend class THcNPSArray;   //to access debug flags.

  ClassDef(THcNPSCalorimeter,0)          // Shower counter detector
};

///////////////////////////////////////////////////////////////////////////////

// Various helper functions to accumulate hit related quantities.

Double_t addE(Double_t x, THcNPSShowerHit* h);
Double_t addX(Double_t x, THcNPSShowerHit* h);
Double_t addY(Double_t x, THcNPSShowerHit* h);
Double_t addZ(Double_t x, THcNPSShowerHit* h);
Double_t addEpr(Double_t x, THcNPSShowerHit* h);

// Methods to calculate coordinates and energy depositions for a given cluster.

Double_t clX(THcNPSShowerCluster* cluster);
Double_t clY(THcNPSShowerCluster* cluster);
Double_t clZ(THcNPSShowerCluster* cluster);
Double_t clE(THcNPSShowerCluster* cluster);
Double_t clEpr(THcNPSShowerCluster* cluster);
//Double_t clEplane(THcNPSShowerCluster* cluster, Int_t iplane, Int_t side);

#endif
