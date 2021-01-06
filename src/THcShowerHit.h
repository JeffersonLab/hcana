#ifndef ROOT_THcShowerHit
#define ROOT_THcShowerHit

// HMS calorimeter hits, version 2

#include <set>
#include <iterator>
#include <iostream>
#include <memory>
#include <vector>
#include "TMath.h"

using namespace std;

class THcShowerHit {       //HMS calorimeter hit class

  Int_t fCol, fRow;        //hit colomn and row
  Double_t fX,fY, fZ;         //hit X (vert.) , Y (hort) and Z (along spect.axis) coordinates
  Double_t fE;             //hit mean energy deposition
  Double_t fEpos;          //hit energy deposition from positive PMT
  Double_t fEneg;          //hit energy deposition from negative PMT

public:

  THcShowerHit();

  THcShowerHit(Int_t hRow, Int_t hCol, Double_t hX, Double_t hY, Double_t hZ,
	       Double_t hE, Double_t hEpos, Double_t hEneg);

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

  Double_t hitY() {
    return fY;
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

  bool isNeighbour(THcShowerHit* hit1);
  void show();
  bool operator<(THcShowerHit rhs) const;

  //  ClassDef(THcShowerHit,0);
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
typedef vector<THcShowerCluster*> THcShowerClusterList;
typedef THcShowerClusterList::iterator THcShowerClusterListIt;

//______________________________________________________________________________

#endif
