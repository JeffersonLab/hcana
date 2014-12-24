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

#include <vector>
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
  // Two hits are neighbours if share a side or a corner.
  //
  bool isNeighbour(THcShowerHit* hit1) {      //Is hit1 neighbouring this hit?
    Int_t dRow = fRow-(*hit1).fRow;
    Int_t dCol = fCol-(*hit1).fCol;
    return TMath::Abs(dRow)<2 && TMath::Abs(dCol)<2;
  }

  //Print out hit information
  //
  void show() {
    cout << "row=" << fRow << "  column=" << fCol 
	 << "  x=" << fX << "  z=" << fZ 
	 << "  E=" << fE << "  Epos=" << fEpos << "  Eneg=" << fEneg << endl;
  }

};


// Container (collection) of hits and its iterator.
//
typedef vector<THcShowerHit*> THcShowerHitList;
typedef THcShowerHitList::iterator THcShowerHitIt;



//HMS calorimeter hit cluster
//
class THcShowerCluster : THcShowerHitList {

 public:

  THcShowerCluster() {
    //    cout << "Dummy THcShowerCluster object created" << endl;
  }

  ~THcShowerCluster() {
    for (THcShowerHitIt i = THcShowerHitList::begin();
    	 i != THcShowerHitList::end(); ++i) {
      delete *i;
      *i = 0;
    }
  }

  // Add a hit to the cluster hit list
  //
  void grow(THcShowerHit* hit) {
    THcShowerHitList::push_back(hit);
  }

  //Pointer to the hit #i in the cluster hit list
  //
  THcShowerHit* ClusteredHit(UInt_t i) {
    return * (THcShowerHitList::begin()+i);
  }

  //Print out a hit in the cluster
  //
  void showHit(UInt_t num) {
    (*(THcShowerHitList::begin()+num))->show();
  }

  // X coordinate of center of gravity of cluster,
  // calculated as hit energy weighted average.
  // Put X out of the calorimeter (-75 cm), if there is no energy deposition
  // in the cluster.
  //
  Double_t clX() {
    Double_t x_sum=0.;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); ++it) {
      x_sum += (*it)->hitX() * (*it)->hitE();
      Etot += (*it)->hitE();
    }
    //    cout << "x_sum=" << x_sum << "  Etot=" << Etot << endl;
    return (Etot != 0. ? x_sum/Etot : -75.);
  }

  // Z coordinate of center of gravity of cluster,
  // calculated as a hit energy weighted average.
  // Put Z out of the calorimeter (0 cm), if there is no energy deposition
  // in the cluster.
  //
  Double_t clZ() {
    Double_t z_sum=0.;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); ++it) {
      z_sum += (*it)->hitZ() * (*it)->hitE();
      Etot += (*it)->hitE();
    }
    //    cout << "z_sum=" << z_sum << "  Etot=" << Etot << endl;
    return (Etot != 0. ? z_sum/Etot : 0.);
  }

  //Energy depostion in a cluster
  //
  Double_t clE() {
    //    cout << "In ECl:" << endl;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); ++it) {
      Etot += (*it)->hitE();
    }
    return Etot;
  }

  //Energy deposition in the Preshower (1st plane) for a cluster
  //
  Double_t clEpr() {
    Double_t Epr=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); ++it) {
      if ((*it)->hitColumn() == 0) {
	Epr += (*it)->hitE();
      }
    }
    return Epr;
  }

  //Cluster energy deposition in plane iplane=0,..,3:
  // side=0 -- from positive PMTs only;
  // side=1 -- from negative PMTs only;
  // side=2 -- from postive and negative PMTs.
  //

  Double_t clEplane(Int_t iplane, Int_t side) {

    if (side!=0&&side!=1&&side!=2) {
      cout << "*** Wrong Side in clEplane:" << side << " ***" << endl;
      return -1;
    }

    Double_t Eplane=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); ++it) {

      if ((*it)->hitColumn() == iplane) 

	switch (side) {
	case 0 : Eplane += (*it)->hitEpos();
	  break;
	case 1 : Eplane += (*it)->hitEneg();
	  break;
	case 2 : Eplane += (*it)->hitE();
	  break;
	default : ;
	}

    }

    return Eplane;
  }


  //Cluster size (number of hits in the cluster).
  //
  UInt_t clSize() {
    return THcShowerHitList::size();
  }

};

//-----------------------------------------------------------------------------

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

  //____________________________________________________________________________

  void ClusterHits(THcShowerHitList HitList) {

    // Collect hits from the HitList into the clusters. The resultant clusters
    // of hits are saved in the ClusterList.

    while (HitList.size() != 0) {

      THcShowerCluster* cluster = new THcShowerCluster;

      (*cluster).grow(*(HitList.end()-1)); //Move the last hit from the hit list
      HitList.erase(HitList.end()-1);      //into the 1st cluster
      bool clustered = true;

      while (clustered) {                   //Proceed while a hit is clustered

	clustered = false;

	for (THcShowerHitIt i=HitList.begin(); i!=HitList.end(); ++i) {

	  for (UInt_t k=0; k!=(*cluster).clSize(); k++) {

	    if ((**i).isNeighbour((*cluster).ClusteredHit(k))) {

	      (*cluster).grow(*i);        //If the hit #i is neighbouring a hit
	      HitList.erase(i);           //in the cluster, then move it
	                                  //into the cluster.
	      clustered = true;
	    }

	    if (clustered) break;
	  }                               //k

	  if (clustered) break;
	}                                 //i

      }                                   //while clustered

      grow(cluster);                      //Put the cluster in the cluster list

    }                                     //While hit_list not exhausted

  }

};



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

#endif
