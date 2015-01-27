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
#include "TMath.h"


// HMS calorimeter hits, version 2

#include <set>
#include <iterator>
#include <iostream>
#include <memory>

using namespace std;

class THcShowerHit {       //HMS calorimeter hit class

private:
  Int_t fCol, fRow;        //hit colomn and row
  Double_t fX, fZ;         //hit X (vert.) and Z (along spect.axis) coordinates
  Double_t fE;             //hit mean energy deposition
  Double_t fEpos;          //hit energy deposition from positive PMT
  Double_t fEneg;          //hit energy deposition from negative PMT
  
public:

  THcShowerHit() {         //default constructor
    fCol=fRow=0;
    fX=fZ=0.;
    fE=0.;
    fEpos=0.;
    fEneg=0.;
  }

  THcShowerHit(Int_t hRow, Int_t hCol, Double_t hX, Double_t hZ,
	       Double_t hE, Double_t hEpos, Double_t hEneg) {
    fRow=hRow;
    fCol=hCol;
    fX=hX;
    fZ=hZ;
    fE=hE;
    fEpos=hEpos;
    fEneg=hEneg;
  }

  ~THcShowerHit() {
    //    cout << " hit destructed" << endl;
  }

  Int_t hitColumn() {
    return fCol;
  }

  Int_t hitRow() {
    return fRow;
  }

  Double_t hitX() {
    return fX;
  }

  Double_t hitZ() {
    return fZ;
  }

  Double_t hitE() {
    return fE;
  }

  Double_t hitEpos() {
    return fEpos;
  }

  Double_t hitEneg() {
    return fEneg;
  }

  // Decide if a hit is neighbouring the current hit.
  // Two hits are neighbours if share a side or a corner,
  // or in the same row but separated by no more than a block.
  //
  bool isNeighbour(THcShowerHit* hit1) {      //Is hit1 neighbouring this hit?
    Int_t dRow = fRow-(*hit1).fRow;
    Int_t dCol = fCol-(*hit1).fCol;
    return (TMath::Abs(dRow)<2 && TMath::Abs(dCol)<2) ||
      (dRow==0 && TMath::Abs(dCol)<3);
  }

  //Print out hit information
  //
  void show() {
    cout << "row=" << fRow << "  column=" << fCol 
	 << "  x=" << fX << "  z=" << fZ 
	 << "  E=" << fE << "  Epos=" << fEpos << "  Eneg=" << fEneg << endl;
  }

  bool operator<(THcShowerHit rhs) const {
    if (fCol != rhs.fCol)
      return fCol < rhs.fCol;
    else
      fRow < rhs.fRow;
  }

};


//____________________________________________________________________________

// Container (collection) of hits and its iterator.
//
typedef set<THcShowerHit*> THcShowerHitSet;
typedef THcShowerHitSet::iterator THcShowerHitIt;

typedef THcShowerHitSet THcShowerCluster;
typedef THcShowerCluster::iterator THcShowerClusterIt;

//______________________________________________________________________________

//Alias for container of clusters and for its iterator
//
typedef vector<THcShowerCluster*> THcShClusterList;
typedef THcShClusterList::iterator THcShClusterIt;

//List of clusters
//
class THcShowerClusterList : private THcShClusterList {

 public:

  THcShowerClusterList() {
    //    cout << "Dummy THcShowerClusterList object created" << endl;
  }

  ~THcShowerClusterList() {
    purge();
  }

  // Purge cluster list
  //
  void purge() {
    for (THcShClusterIt i = THcShClusterList::begin();
	 i != THcShClusterList::end(); ++i) {
      delete *i;
      *i = 0;
    }
    THcShClusterList::clear();
  }

  //Put a cluster in the cluster list
  //
  void grow(THcShowerCluster* cluster) {
    THcShClusterList::push_back(cluster);
  }

  //Pointer to the cluster #i in the cluster list
  //
  THcShowerCluster* ListedCluster(UInt_t i) {
    return *(THcShClusterList::begin()+i);
  }

  //Cluster list size.
  //
  UInt_t NbClusters() {
    return THcShClusterList::size();
  }

  void ClusterHits(THcShowerHitSet HitSet) {

    // Collect hits from the HitSet into the clusters. The resultant clusters
    // of hits are saved in the ClusterList.

    while (HitSet.size() != 0) {

      THcShowerCluster* cluster = new THcShowerCluster;

      THcShowerHitIt it = HitSet.end();
      (*cluster).insert(*(--it));   //Move the last hit from the hit list
      HitSet.erase(it);            //into the 1st cluster

      bool clustered = true;

      while (clustered) {                   //Proceed while a hit is clustered

	clustered = false;

	for (THcShowerHitIt i=HitSet.begin(); i!=HitSet.end(); ++i) {

	  for (THcShowerClusterIt k=(*cluster).begin(); k!=(*cluster).end();
	       k++) {

	    if ((**i).isNeighbour(*k)) {

	      (*cluster).insert(*i);      //If the hit #i is neighbouring a hit
	      HitSet.erase(i);           //in the cluster, then move it
	                                  //into the cluster.
	      clustered = true;
	    }

	    if (clustered) break;
	  }                               //k

	  if (clustered) break;
	}                                 //i

      }                                   //while clustered

      push_back(cluster);         //Put the cluster in the cluster list

    }                                     //While hit_list not exhausted

  }

};

//______________________________________________________________________________

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
  
  Int_t GetNHits() const { return fNhits; }
  
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
    return (fCcor + sign*y)/(fCcor + sign*y/fDcor);
  }

  // Get total energy deposited in the cluster matched to the given
  // spectrometer Track.

  Float_t GetShEnergy(THaTrack*);

  THcShower();  // for ROOT I/O

protected:

  Int_t fEvent;

  Int_t fAnalyzePedestals;   // Flag for pedestal analysis.

  Int_t* fShPosPedLimit;     // [fNtotBlocks] ADC limits for pedestal calc.-s.
  Int_t* fShNegPedLimit;

  Int_t fShMinPeds;          // Min.number of events to analyze pedestals.

  Double_t* fPosGain;        // [fNtotBlocks] Gain constants from calibration
  Double_t* fNegGain;

  // Per-event data

  Int_t fNhits;              // Total number of hits
  Int_t fNclust;             // Number of clusters
  Int_t fNtracks;            // Number of shower tracks, i.e. number of
                             // cluster-to-track association

  THcShowerClusterList* fClusterList;   // List of hit clusters

  // Geometrical parameters.

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

  Int_t fdbg_raw_cal;          // Shower debug flags
  Int_t fdbg_decoded_cal;
  Int_t fdbg_sparsified_cal;
  Int_t fdbg_clusters_cal;
  Int_t fdbg_tracks_cal;
  Int_t fdbg_init_cal;         // No counterpart in engine, added to debug
                               // calorimeter initialization

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

  // Cluster to track association method.
  Int_t MatchCluster(THaTrack*, Double_t&, Double_t&);

  friend class THcShowerPlane;   //to access debug flags.

  ClassDef(THcShower,0)          // Shower counter detector
};

///////////////////////////////////////////////////////////////////////////////

// Auxiliary methods to be used with the hit and cluster containers.

Double_t addE(Double_t x, THcShowerHit* h);
Double_t addX(Double_t x, THcShowerHit* h);
Double_t addZ(Double_t x, THcShowerHit* h);
Double_t addEpr(Double_t x, THcShowerHit* h);
Double_t clX(THcShowerCluster* cluster);
Double_t clZ(THcShowerCluster* cluster);
Double_t clE(THcShowerCluster* cluster);
Double_t clEpr(THcShowerCluster* cluster);
Double_t clEplane(THcShowerCluster* cluster, Int_t iplane, Int_t side);

#endif
