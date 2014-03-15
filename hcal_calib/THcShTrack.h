#include "THcShHit.h"
#include "TMath.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>

using namespace std;

// Track class for the HMS calorimeter calibration.
// Comprises the spectrometer track parameters and calorimeter hits.
//

// Container (collection) of hits and its iterator.
//
typedef vector<THcShHit*> THcShHitList;
typedef THcShHitList::iterator THcShHitIt;

class THcShTrack {

  Double_t P;   // track momentum
  Double_t X;   // at the calorimater face
  Double_t Xp;  // slope
  Double_t Y;   // at the calorimater face
  Double_t Yp;  // slope

  THcShHitList Hits;

 public:
  THcShTrack();
  THcShTrack(Double_t p, Double_t x, Double_t xp, Double_t y, Double_t yp);
  ~THcShTrack();

  void Reset(Double_t p, Double_t x, Double_t xp, Double_t y, Double_t yp);

  void AddHit(Double_t adc_pos, Double_t adc_neg,
	      Double_t e_pos, Double_t e_neg,
	      UInt_t blk_number);

  THcShHit* GetHit(UInt_t k);

  UInt_t GetNhits() {return Hits.size();};

  void Print(ostream & ostrm);

  void SetEs(Double_t* alpha);

  Double_t Enorm();

  Double_t GetP() {return P*1000.;}      //MeV

  Float_t Ycor(Double_t);         // coord. corection for single PMT module
  Float_t Ycor(Double_t, Int_t);  // coord. correction for double PMT module

  // Coordinate correction constants from hcana.param.
  //
  static const Double_t fAcor = 200.;
  static const Double_t fBcor = 8000.;
  static const Double_t fCcor = 64.36;
  static const Double_t fDcor = 1.66;

  // Calorimeter geometry constants.
  //
  static const Double_t fZbl = 10;   //cm, block transverse size
  static const UInt_t fNrows = 13;
  static const UInt_t fNcols =  4;
  static const UInt_t fNnegs = 26;   // number of blocks with neg. side PMTs.
  static const UInt_t fNpmts = 78;   // total number of PMTs.
  static const UInt_t fNblks = fNrows*fNcols;

};

//------------------------------------------------------------------------------

THcShTrack::THcShTrack() { };

THcShTrack::THcShTrack(Double_t p,
		       Double_t x, Double_t xp, Double_t y, Double_t yp) {
  P = p;
  X = x;
  Xp = xp;
  Y = y;
  Yp =yp;
};

//------------------------------------------------------------------------------

void THcShTrack::Reset(Double_t p,
		       Double_t x, Double_t xp, Double_t y, Double_t yp) {

  // Reset track parameters, clear hit list.

  P = p;
  X = x;
  Xp = xp;
  Y = y;
  Yp =yp;
  Hits.clear();
};

//------------------------------------------------------------------------------

void THcShTrack::AddHit(Double_t adc_pos, Double_t adc_neg,
			Double_t e_pos, Double_t e_neg,
			UInt_t blk_number) {

  // Add a hit to the hit list.

  THcShHit* hit = new THcShHit(adc_pos, adc_neg, blk_number);
  hit->SetEpos(e_pos);
  hit->SetEneg(e_neg);
  Hits.push_back(hit);
};

//------------------------------------------------------------------------------

THcShHit* THcShTrack::GetHit(UInt_t k) {
  THcShHitIt it = Hits.begin();
  for (UInt_t i=0; i<k; i++) it++;
  return *it;
}

void THcShTrack::Print(ostream & ostrm) {

  // Output the track parameters and hit list through the stream ostrm.

  ostrm << P << " " << X << " " << Xp << " " << Y << " " << Yp << " "
	<< Hits.size() << endl;

  for (THcShHitIt iter = Hits.begin(); iter != Hits.end(); iter++) {
    (*iter)->Print(ostrm);
  };

};

//------------------------------------------------------------------------------

THcShTrack::~THcShTrack() {
  for (THcShHitIt i = Hits.begin(); i != Hits.end(); ++i) {
    delete *i;
    *i = 0;
  }
};

//------------------------------------------------------------------------------

void THcShTrack::SetEs(Double_t* alpha) {

  // Set hit energy depositions seen from postive and negative sides,
  // by use of calibration (gain) constants alpha.
  
  for (THcShHitIt iter = Hits.begin(); iter != Hits.end(); iter++) {
  
    Double_t adc_pos = (*iter)->GetADCpos();
    Double_t adc_neg = (*iter)->GetADCneg();
    UInt_t nblk = (*iter)->GetBlkNumber();

    Int_t ncol=(nblk-1)/fNrows+1;
    Double_t xh=X+Xp*(ncol-0.5)*fZbl;
    Double_t yh=Y+Yp*(ncol-0.5)*fZbl;
    if (nblk <= fNnegs) {
      (*iter)->SetEpos(adc_pos*Ycor(yh,0)*alpha[nblk-1]);
      (*iter)->SetEneg(adc_neg*Ycor(yh,1)*alpha[fNblks+nblk-1]);
    }
    else {
      (*iter)->SetEpos(adc_pos*Ycor(yh)*alpha[nblk-1]);
      (*iter)->SetEneg(0.);
    };

  };

}

//------------------------------------------------------------------------------

Double_t THcShTrack::Enorm() {

  // Normalized to the track momentum energy depostion in the calorimeter.

  Double_t sum = 0;

  for (THcShHitIt iter = Hits.begin(); iter != Hits.end(); iter++) {
    sum += (*iter)->GetEpos();
    sum += (*iter)->GetEneg();
  };

  return sum/P/1000.;
}

//------------------------------------------------------------------------------

//Coordinate correction for single PMT modules.
//PMT attached at right (positive) side.

Float_t THcShTrack::Ycor(Double_t y) {
  return TMath::Exp(y/fAcor)/(1. + y*y/fBcor);
}

//Coordinate correction for double PMT modules.
//

Float_t THcShTrack::Ycor(Double_t y, Int_t side) {
  if (side!=0&&side!=1) {
    cout << "THcShower::Ycor : wrong side " << side << endl;
    return 0.;
  }
  Int_t sign = 1 - 2*side;
  return (fCcor + sign*y)/(fCcor + sign*y/fDcor);
}
