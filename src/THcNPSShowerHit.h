#ifndef ROOT_THcNPSShowerHit
#define ROOT_THcNPSShowerHit

// HMS calorimeter hits, version 2

#include <set>
#include <iterator>
#include <iostream>
#include <memory>
#include <vector>
#include "TMath.h"

using namespace std;

class THcNPSShowerHit {       //HMS calorimeter hit class

  Int_t fID, fCol, fRow;        //hit block ID, column and row
  Double_t fX,fY, fZ;         //hit X (vert.) , Y (hort) and Z (along spect.axis) coordinates
  Double_t fE;             //hit pulse energy
  Double_t fT;             //hit good pulse time
  Double_t fPI;          //hit good pulse integral (used to do clustering)
  
public:

  THcNPSShowerHit();

  THcNPSShowerHit(Int_t block_id, Int_t hRow, Int_t hCol, Double_t hX, Double_t hY, Double_t hZ,
		  Double_t hE, Double_t hT, Double_t hPI);

  ~THcNPSShowerHit() {
    //    cout << " hit destructed" << endl;
  }

  Int_t hitID() {
    return fID;
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

  Double_t hitY() {
    return fY;
  }

  Double_t hitZ() {
    return fZ;
  }

  Double_t hitE() {
    return fE;
  }
  Double_t hitT() {
    return fT;
  }
  Double_t hitPI() {
    return fPI;
  }
  
  bool isNeighbour(THcNPSShowerHit* hit1);
  void show();
  bool operator<(THcNPSShowerHit rhs) const;

  //  ClassDef(THcNPSShowerHit,0);
};


//____________________________________________________________________________

// Container (collection) of hits and its iterator.
//
typedef set<THcNPSShowerHit*> THcNPSShowerHitSet;
typedef THcNPSShowerHitSet::iterator THcNPSShowerHitIt;

typedef THcNPSShowerHitSet THcNPSShowerCluster;
typedef THcNPSShowerCluster::iterator THcNPSShowerClusterIt;


//______________________________________________________________________________

//Alias for container of clusters and for its iterator
//
typedef vector<THcNPSShowerCluster*> THcNPSShowerClusterList;
typedef THcNPSShowerClusterList::iterator THcNPSShowerClusterListIt;

//______________________________________________________________________________




#endif
