#ifndef ROOT_THcPShowerCalib
#define ROOT_THcPShowerCalib

#include "THcPShTrack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompLU.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <iomanip>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#define D_CALO_FP 275.    //distance from FP to the Preshower

using namespace std;

//
// SHMS Calorimeter calibration class.
//

class THcPShowerCalib {

 public:

  THcPShowerCalib(Int_t);
  THcPShowerCalib();
  ~THcPShowerCalib();

  void Init();
  void ReadShRawTrack(THcPShTrack &trk, UInt_t ientry);
  void CalcThresholds();
  void ComposeVMs();
  void SolveAlphas();
  void FillHEcal();
  void SaveAlphas();
  void SaveRawData();

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH2F* hDPvsEcal;

 private:

  Int_t fRunNumber;
  Double_t fLoThr;     // Low and high thresholds on the normalized uncalibrated
  Double_t fHiThr;     // energy deposition.
  UInt_t fNev;         // Number of processed events.

  static const UInt_t fMinHitCount = 5;     // Minimum number of hits for a PMT
                                            // to be calibrated.

  TTree* fTree;
  UInt_t fNentries;

  // Quantities for calculations of the calibration constants.

  Double_t fe0;
  Double_t fqe[THcPShTrack::fNpmts];
  Double_t fq0[THcPShTrack::fNpmts];
  Double_t fQ[THcPShTrack::fNpmts][THcPShTrack::fNpmts];
  Double_t falphaU[THcPShTrack::fNpmts];   // 'unconstrained' calib. constants
  Double_t falphaC[THcPShTrack::fNpmts];   // the sought calibration constants
  Double_t falpha0[THcPShTrack::fNpmts];   // initial gains
  Double_t falpha1[THcPShTrack::fNpmts];   // unit gains

  UInt_t fHitCount[THcPShTrack::fNpmts];

};

//------------------------------------------------------------------------------

THcPShowerCalib::THcPShowerCalib() {};

//------------------------------------------------------------------------------

THcPShowerCalib::THcPShowerCalib(Int_t RunNumber) {
  fRunNumber = RunNumber;
};

//------------------------------------------------------------------------------

THcPShowerCalib::~THcPShowerCalib() {
};

//------------------------------------------------------------------------------

void THcPShowerCalib::SaveRawData() {

  // Output raw data into file for debug purposes. To be called after
  // calibration constants are determined.

  cout << "SaveRawData: Output raw data into Pcal_calib.raw_data." << endl;

  ofstream fout;
  fout.open("Pcal_calib.raw_data",ios::out);

  THcPShTrack trk;

  for (UInt_t ientry=0; ientry<fNentries; ientry++) {
    ReadShRawTrack(trk, ientry);
    trk.SetEs(falphaC);
    trk.Print(fout);
  }

  fout.close();

}

//------------------------------------------------------------------------------

void THcPShowerCalib::Init() {

  //Reset ROOT and connect tree file.

  gROOT->Reset();

  char* fname = Form("Root_files/Pcal_calib_%d.root",fRunNumber);
  cout << "THcPShowerCalib::Init: Root file name = " << fname << endl;

  TFile *f = new TFile(fname);
  f->GetObject("T",fTree);

  fNentries = fTree->GetEntries();
  cout << "THcPShowerCalib::Init: fNentries= " << fNentries << endl;

  // Histogram declarations.

  hEunc = new TH1F("hEunc", "Edep/P uncalibrated", 500, 0., 5.);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 150, 0., 1.5);
  hDPvsEcal = new TH2F("hDPvsEcal", "#DeltaP versus Edep/P ",
		       350,0.,1.5, 250,-12.5,22.5);

  // Initialize cumulative quantities.
  
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) fHitCount[i] = 0;

  fe0 = 0.;

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    fqe[i] = 0.;
    fq0[i] = 0.;
    falphaU[i] = 0.;
    falphaC[i] = 0.;
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++) {
      fQ[i][j] = 0.;
    }
  }

  // Initial gains, 1 for all.

  for (UInt_t ipmt=0; ipmt<THcPShTrack::fNpmts; ipmt++) {
    falpha0[ipmt] = 1.;
  };

  // Unit gains.

  for (UInt_t ipmt=0; ipmt<THcPShTrack::fNpmts; ipmt++) {
    falpha1[ipmt] = 1.;
  }

};

//------------------------------------------------------------------------------

void THcPShowerCalib::CalcThresholds() {

  // Calculate +/-3 RMS thresholds on the uncalibrated total energy
  // depositions. These thresholds are used mainly to exclude potential
  // hadronic events due to the gas Cherenkov inefficiency.

  // Histogram uncalibrated energy depositions, get mean and RMS from the
  // histogram, establish +/-3 * RMS thresholds.

  Int_t nev = 0;
  THcPShTrack trk;

  for (UInt_t ientry=0; ientry<fNentries; ientry++) {

    ReadShRawTrack(trk, ientry);

    //    trk.Print(cout);
    //    getchar();

    trk.SetEs(falpha0);             //Use initial gain constants here.
    Double_t Enorm = trk.Enorm();

    nev++;
    //    cout << "CalcThreshods: nev=" << nev << "  Enorm=" << Enorm << endl;

    hEunc->Fill(Enorm);    
  };

  Double_t mean = hEunc->GetMean();
  Double_t rms = hEunc->GetRMS();
  cout << "CalcThreshods: mean=" << mean << "  rms=" << rms << endl;

  fLoThr = mean - 3.*rms;
  fHiThr = mean + 3.*rms;
  //  fLoThr = 0.;              // Wide open thrsholds, for
  //  fHiThr = 1.e+8;           // comparison with the old code.

  cout << "CalcThreshods: fLoThr=" << fLoThr << "  fHiThr=" << fHiThr 
       << "  nev=" << nev << endl;

  Int_t nbins = hEunc->GetNbinsX();
  Int_t nlo = hEunc->FindBin(fLoThr);
  Int_t nhi = hEunc->FindBin(fHiThr);

  cout << "CalcThresholds: nlo=" << nlo << "  nhi=" << nhi 
       << "  nbins=" << nbins << endl;

  // Histogram of selected within the thresholds events.
  
  hEuncSel = (TH1F*)hEunc->Clone("hEuncSel");
  
  for (Int_t i=0; i<nlo; i++) hEuncSel->SetBinContent(i, 0.);
  for (Int_t i=nhi; i<nbins+1; i++) hEuncSel->SetBinContent(i, 0.);

};

//------------------------------------------------------------------------------

void THcPShowerCalib::ReadShRawTrack(THcPShTrack &trk, UInt_t ientry) {

  //
  // Set a Shower track event from ntuple ientry.
  //

  // Declaration of leaves types

  // Preshower and Shower ADC signals.

  Double_t        P_pr_a_p[THcPShTrack::fNrows_pr][THcPShTrack::fNcols_pr];
  Double_t        P_sh_a_p[THcPShTrack::fNrows_sh][THcPShTrack::fNcols_sh];

  // Track parameters.

  Double_t        P_tr_p;
  Double_t        P_tr_x;   //X FP
  Double_t        P_tr_xp;
  Double_t        P_tr_y;   //Y FP
  Double_t        P_tr_yp;

  const Double_t adc_thr = 15.;   //Low threshold on the ADC signals.

  // Set branch addresses.

  fTree->SetBranchAddress("P.pr.a_p",P_pr_a_p);
  fTree->SetBranchAddress("P.sh.a_p",P_sh_a_p);

  fTree->SetBranchAddress("P.tr.x", &P_tr_x);
  fTree->SetBranchAddress("P.tr.y", &P_tr_y);
  fTree->SetBranchAddress("P.tr.th",&P_tr_xp);
  fTree->SetBranchAddress("P.tr.ph",&P_tr_yp);
  fTree->SetBranchAddress("P.tr.p", &P_tr_p);

  fTree->GetEntry(ientry);

  // Set track coordinates and slopes at the face of Preshower.

  trk.Reset(P_tr_p, P_tr_x+D_CALO_FP*P_tr_xp, P_tr_xp,
  	    P_tr_y+D_CALO_FP*P_tr_yp, P_tr_yp);

  // Set Preshower hits.

  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    for (UInt_t j=0; j<THcPShTrack::fNrows_pr; j++) {

      Double_t adc = P_pr_a_p[j][k];

      if (adc > adc_thr) {
	UInt_t nb = j+1 + k*THcPShTrack::fNrows_pr;
	trk.AddHit(adc, 0., nb);
      }

    }
  }

  // Set Shower hits.

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    for (UInt_t j=0; j<THcPShTrack::fNrows_sh; j++) {

      Double_t adc = P_sh_a_p[j][k];

      if (adc > adc_thr) {
	UInt_t nb = THcPShTrack::fNpmts_pr + j+1 + k*THcPShTrack::fNrows_sh;
	trk.AddHit(adc, 0., nb);
      }

    }
  }

}

//------------------------------------------------------------------------------

void THcPShowerCalib::ComposeVMs() {

  //
  // Fill in vectors and matrixes for the gain constant calculations.
  //

  fNev = 0;
  THcPShTrack trk;

  // Loop over the shower track events in the ntuples.

  for (UInt_t ientry=0; ientry<fNentries; ientry++) {

    ReadShRawTrack(trk, ientry);

    // Set energy depositions with default gains.
    // Calculate normalized to the track momentum total energy deposition,
    // check it against the thresholds.

    trk.SetEs(falpha0);
    Double_t Enorm = trk.Enorm();
    if (Enorm>fLoThr && Enorm<fHiThr) {

      trk.SetEs(falpha1);   // Set energies with unit gains for now.
      // trk.Print(cout);

      fe0 += trk.GetP();    // Accumulate track momenta.

      vector<pmt_hit> pmt_hit_list;     // Container to save PMT hits

      // Loop over hits.

      for (UInt_t i=0; i<trk.GetNhits(); i++) {

	THcPShHit* hit = trk.GetHit(i);
	// hit->Print(cout);

	UInt_t nb = hit->GetBlkNumber();

	// Fill the qe and q0 vectors.

	fqe[nb-1] += hit->GetEdep() * trk.GetP();
	fq0[nb-1] += hit->GetEdep();

	// Save the PMT hit.

	pmt_hit_list.push_back( pmt_hit{hit->GetEdep(), nb} );

	fHitCount[nb-1]++;   //Accrue the hit counter.

      }      //over hits

      // Fill in the correlation matrix Q by retrieving the PMT hits.

      for (vector<pmt_hit>::iterator i=pmt_hit_list.begin();
	   i < pmt_hit_list.end(); i++) {

	UInt_t ic = (*i).channel;
	Double_t is = (*i).signal;

	for (vector<pmt_hit>::iterator j=i;
	     j < pmt_hit_list.end(); j++) {

	  UInt_t jc = (*j).channel;
	  Double_t js = (*j).signal;

	  fQ[ic-1][jc-1] += is*js;
	  if (jc != ic) fQ[jc-1][ic-1] += is*js;
	}
      }

      fNev++;

    };   // if within the thresholds

  };     // over entries

  // Take averages.

  fe0 /= fNev;
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    fqe[i] /= fNev;
    fq0[i] /= fNev;
  }

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++)
      fQ[i][j] /= fNev;

  // Output vectors and matrixes, for debug purposes.

  ofstream q0out;
  q0out.open("q0.d",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    q0out << setprecision(20) << fq0[i] << " " << i << endl;
  q0out.close();

  ofstream qeout;
  qeout.open("qe.d",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    qeout << setprecision(20) << fqe[i] << " " << i << endl;
  qeout.close();

  ofstream Qout;
  Qout.open("Q.d",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++)
      Qout << setprecision(20) << fQ[i][j] << " " << i << " " << j << endl;
  Qout.close();

};

//------------------------------------------------------------------------------

void THcPShowerCalib::SolveAlphas() {

  //
  // Solve for the sought calibration constants, by use of the Root
  // matrix algebra package.
  //

  TMatrixD Q(THcPShTrack::fNpmts,THcPShTrack::fNpmts);
  TVectorD q0(THcPShTrack::fNpmts);
  TVectorD qe(THcPShTrack::fNpmts);
  TVectorD au(THcPShTrack::fNpmts);
  TVectorD ac(THcPShTrack::fNpmts);
  Bool_t ok;

  cout << "Solving Alphas..." << endl;
  cout << endl;

  // Print out hit numbers.

  cout << "Hit counts:" << endl;
  UInt_t j = 0;
  
  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? cout << "Preshower:" : cout << "        :";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? cout << "Shower   :" : cout << "        :";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }

  // Initialize the vectors and the matrix of the Root algebra package.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    q0[i] = fq0[i];
    qe[i] = fqe[i];
    for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
      Q[i][k] = fQ[i][k];
    }
  }

  // Sanity check.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {

    // Check zero hit channels: the vector and matrix elements should be 0.

    if (fHitCount[i] == 0) {

      if (q0[i] != 0. || qe[i] != 0.) {

	cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	     << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i];

	for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
	  if (Q[i][k] !=0. || Q[k][i] !=0.)
	    cout << ", Q[" << i << "," << k << "]=" << Q[i][k]
		 << ", Q[" << k << "," << i << "]=" << Q[k][i];
	}

	cout << " ***" << endl;
      }
    }

    // The hit channels: the vector elements should be non zero.

    if ( (fHitCount[i] != 0) && (q0[i] == 0. || qe[i] == 0.) ) {
      cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	   << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i]
	   << " ***" << endl;
    }

  } //sanity check

  // Low hit number channels: exclude from calculation. Assign all the
  // correspondent elements 0, except self-correlation Q(i,i)=1.

  cout << endl;
  cout << "Channels with hit number less than " << fMinHitCount 
       << " will not be calibrated." << endl;
  cout << endl;

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {

    if (fHitCount[i] < fMinHitCount) {
      cout << "Channel " << i << ", " << fHitCount[i]
	   << " hits, will not be calibrated." << endl;
      q0[i] = 0.;
      qe[i] = 0.;
      for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
	Q[i][k] = 0.;
	Q[k][i] = 0.;
      }
      Q[i][i] = 1.;
    }

  }

  // Declare LU decomposition method for the correlation matrix Q.

  TDecompLU lu(Q);
  Double_t d1,d2;
  lu.Det(d1,d2);
  cout << "cond:" << lu.Condition() << endl;
  cout << "det :" << d1*TMath::Power(2.,d2) << endl;
  cout << "tol :" << lu.GetTol() << endl;

  // Solve equation Q x au = qe for the 'unconstrained' calibration (gain)
  // constants au.

  au = lu.Solve(qe,ok);
  cout << "au: ok=" << ok << endl;
  //  au.Print();

  // Find the sought 'constrained' calibration constants next.

  Double_t t1 = fe0 - au * q0;         // temporary variable.
  //  cout << "t1 =" << t1 << endl;

  TVectorD Qiq0(THcPShTrack::fNpmts);   // an intermittent result
  Qiq0 = lu.Solve(q0,ok);
  cout << "Qiq0: ok=" << ok << endl;
  //  Qiq0.Print();

  Double_t t2 = q0 * Qiq0;             // another temporary variable
  //  cout << "t2 =" << t2 << endl;

  ac = (t1/t2) *Qiq0 + au;             // the sought gain constants
  // cout << "ac:" << endl;
  //  ac.Print();

  // Assign the gain arrays.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    falphaU[i] = au[i];
    falphaC[i] = ac[i];
  }

}

//------------------------------------------------------------------------------

void THcPShowerCalib::FillHEcal() {

  //
  // Fill histogram of the normalized energy deposition, and 2-d histogram
  // of momentum deviation versus normalized energy deposition.
  // Output event by event energy depositions and momenta for debug purposes.
  //

  ofstream output;
  output.open("calibrated.d",ios::out);

  Int_t nev = 0;

  THcPShTrack trk;

  for (UInt_t ientry=0; ientry<fNentries; ientry++) {

    ReadShRawTrack(trk, ientry);
    //    trk.Print(cout);

    trk.SetEs(falphaC);          // use the 'constrained' calibration constants
    Double_t P = trk.GetP();
    Double_t Enorm = trk.Enorm();

    hEcal->Fill(Enorm);

    Double_t delta;
    fTree->SetBranchAddress("P.tr.tg_dp",&delta);
    hDPvsEcal->Fill(Enorm,delta,1.);

    output << Enorm*P/1000. << " " << P/1000. << " " << trk.GetX() << " "
	   << trk.GetY() << endl;

    nev++;
  };

  output.close();

  cout << "FillHEcal: " << nev << " events filled" << endl;
};

//------------------------------------------------------------------------------

void THcPShowerCalib::SaveAlphas() {

  //
  // Output the gain constants in a format suitable for inclusion in the
  // pcal.param file to be used in the analysis.
  //

  ofstream output;
  char* fname = Form("pcal.param.%d",fRunNumber);
  cout << "SaveAlphas: fname=" << fname << endl;

  output.open(fname,ios::out);

  output << "; Calibration constants for run " << fRunNumber 
	 << ", " << fNev << " events processed" << endl;
  output << endl;

  UInt_t j = 0;

  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? output << "shms_neg_pre_gain=" : output << "shms_pos_pre_gain=";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++)
      output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
    output << endl;
  }

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? output << "shms_shower_gain =" : output << "                  ";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++)
      output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
    output << endl;
  }

  output.close();
}

#endif
