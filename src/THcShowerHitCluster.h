#ifndef ROOT_THcShowerHitCluster
#define ROOT_THcShowerHitCluster

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

  // Define < operator in order to fill in set of hits in a sorted manner.
  //
  bool operator<(THcShowerHit rhs) const {
    if (fCol != rhs.fCol)
      return fCol < rhs.fCol;
    else
      return fRow < rhs.fRow;
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
typedef vector<THcShowerCluster*> THcShowerClusterList;
typedef THcShowerClusterList::iterator THcShowerClusterListIt;

//______________________________________________________________________________

#endif
