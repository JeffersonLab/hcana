/** \classTHcShowerHit
    \ingroup DetSupport

*/
#include "THcShowerHit.h"

//ClassImp(THcShowerHit)

using namespace std;

THcShowerHit::THcShowerHit() {         //default constructor
  fCol=fRow=0;
  fX=fZ=0.;
  fE=0.;
  fEpos=0.;
  fEneg=0.;
}

THcShowerHit::THcShowerHit(Int_t hRow, Int_t hCol, Double_t hX, Double_t hZ,
			   Double_t hE, Double_t hEpos, Double_t hEneg) {
  fRow=hRow;
  fCol=hCol;
  fX=hX;
  fZ=hZ;
  fE=hE;
  fEpos=hEpos;
  fEneg=hEneg;
}

//____________________________________________________________________________
// Decide if a hit is neighbouring the current hit.
// Two hits are neighbours if share a side or a corner,
// or in the same row but separated by no more than a block.
//
bool THcShowerHit::isNeighbour(THcShowerHit* hit1) {
  //Is hit1 neighbouring this hit?
  Int_t dRow = fRow-(*hit1).fRow;
  Int_t dCol = fCol-(*hit1).fCol;
  return (TMath::Abs(dRow)<2 && TMath::Abs(dCol)<2) ||
			   (dRow==0 && TMath::Abs(dCol)<3);
}

//____________________________________________________________________________
//Print out hit information
//
void THcShowerHit::show() {
  cout << "row=" << fRow << "  column=" << fCol
       << "  x=" << fX << "  z=" << fZ
       << "  E=" << fE << "  Epos=" << fEpos << "  Eneg=" << fEneg << endl;
}

//____________________________________________________________________________
  // Define < operator in order to fill in set of hits in a sorted manner.
  //
bool THcShowerHit::operator<(THcShowerHit rhs) const {
  if (fCol != rhs.fCol)
    return fCol < rhs.fCol;
  else
    return fRow < rhs.fRow;
}
