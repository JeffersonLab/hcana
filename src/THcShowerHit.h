#ifndef ROOT_THcShowerHit
#define ROOT_THcShowerHit

// HMS calorimeter hit, version 2

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

class THcShowerHit {           //HMS calorimeter hit class

 private:
  unsigned int fCol, fRow; //hit colomn and row
  float fX, fZ;            //hit X (vert.) and Z (along spect.axis) coordinates
  float fE;                 //hit energy deposition

 public:

  THcShowerHit() {             //default constructor
  fCol=fRow=0;
  fX=fZ=0.;
  fE=0.;
  }

  THcShowerHit(unsigned int hRow, unsigned int hCol, float hX, float hZ,
	       float hE) {
    fRow=hRow;
    fCol=hCol;
    fX=hX;
    fZ=hZ;
    fE=hE;
  }

  ~THcShowerHit() {
    //    cout << " hit destructed" << endl;
  }

  unsigned int hitColumn() {
    return fCol;
  }

  unsigned int hitRow() {
    return fRow;
  }

  float hitX() {
    return fX;
  }

  float hitZ() {
    return fZ;
  }

  float hitE() {
    return fE;
  }

  bool isNeighbour(THcShowerHit* hit1) {      //Is hit1 neighbouring this hit?
    int dRow = fRow-(*hit1).fRow;
    int dCol = fCol-(*hit1).fCol;
    return TMath::Abs(dRow)<2 && TMath::Abs(dCol)<2;
  }

  //Print out hit information
  //
  void show() {
    cout << "row=" << fRow << "  column=" << fCol << 
      "  x=" << fX << "  z=" << fZ << "  E=" << fE << endl;
  }

};


typedef vector<THcShowerHit*> THcShowerHitList;      //alias for hit container
typedef THcShowerHitList::iterator THcShowerHitIt;   //and for its iterator

//Purge sequence container of pointers. Found in Echel, v.2, p.253.
//
template<class Seq> void purge(Seq& c) {
  typename Seq::iterator i;
  for(i = c.begin(); i != c.end(); i++) {
    delete *i;
    *i = 0;
  }
}

#endif
