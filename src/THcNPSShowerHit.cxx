/** \classTHcNPSShowerHit
    \ingroup DetSupport

*/
#include "THcNPSShowerHit.h"

//ClassImp(THcNPSShowerHit)

using namespace std;

THcNPSShowerHit::THcNPSShowerHit() {         //default constructor
  
  fID=fCol=fRow=0;
  fX=fY=fZ=0.;
  fE=0.;
  fT=0.;
  fPI=0.;

}

THcNPSShowerHit::THcNPSShowerHit(Int_t block_id, Int_t hRow, Int_t hCol, Double_t hX, Double_t hY, Double_t hZ,
				 Double_t hE, Double_t hT, Double_t hPI) {
  fID=block_id;
  fRow=hRow;
  fCol=hCol;
  fX=hX;
  fY=hY;
  fZ=hZ;
  fE=hE;
  fT=hT;
  fPI=hPI;
}

//____________________________________________________________________________
// Decide if a hit is neighbouring the current hit.
// Two hits are neighbours if share a side or a corner,
// or in the same row but separated by no more than a block.
//
bool THcNPSShowerHit::isNeighbour(THcNPSShowerHit* hit1) {
  //Is hit1 neighbouring this hit?
  Int_t dRow = fRow-(*hit1).fRow;
  Int_t dCol = fCol-(*hit1).fCol;
  return (TMath::Abs(dRow)<2 && TMath::Abs(dCol)<2) ||
			   (dRow==0 && TMath::Abs(dCol)<3);
}

//____________________________________________________________________________
//Print out hit information
//
void THcNPSShowerHit::show() {
  cout << "row=" << fRow+1 << "  column=" << fCol+1
       << "  x=" << fX  << "  y=" << fY << "  z=" << fZ
       << "  E=" << fE << "  T=" << fT << endl;
}

//____________________________________________________________________________
  // Define < operator in order to fill in set of hits in a sorted manner.
  //
bool THcNPSShowerHit::operator<(THcNPSShowerHit rhs) const {
  if (fCol != rhs.fCol)
    return fCol < rhs.fCol;
  else
    return fRow < rhs.fRow;
}
