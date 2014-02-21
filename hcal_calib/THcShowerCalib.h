#ifndef ROOT_THcShowerCalib
#define ROOT_THcShowerCalib

#include "THcShTrack.h"
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

using namespace std;

// HMS Shower Counter calibration class

class THcShowerCalib {

 public:
  THcShowerCalib(Int_t);
  THcShowerCalib();
  ~THcShowerCalib();

  void ExtractData();
  void Init();
  Bool_t ReadShRawTrack(THcShTrack &trk);
  void CalcThresholds();
  void ComposeVMs();
  void SolveAlphas();
  void FillHEcal();
  void SaveAlphas();
  //  void PlotHistos();

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH2F* hPvsEcal;

 private:
  Int_t fRunNumber;
  Double_t fLoThr;
  Double_t fHiThr;
  UInt_t fNev;
  static const UInt_t fMinHitCount = 200;

  ifstream fDataStream;

  //  TVectorD* fe0;
  //  TVectorD* fq0;
  //  TMatrixDSym* fQ;
  //  TVectorD* falphaU;
  //  TVectorD* falphaC;

  Double_t fe0;
  Double_t fqe[THcShTrack::fNpmts];
  Double_t fq0[THcShTrack::fNpmts];
  Double_t fQ[THcShTrack::fNpmts][THcShTrack::fNpmts];
  Double_t falphaU[THcShTrack::fNpmts];
  Double_t falphaC[THcShTrack::fNpmts];
  Double_t falpha0[THcShTrack::fNpmts];
  Double_t falpha1[THcShTrack::fNpmts];

  UInt_t fHitCount[THcShTrack::fNpmts];

};

//------------------------------------------------------------------------------

THcShowerCalib::THcShowerCalib() {};

//------------------------------------------------------------------------------

THcShowerCalib::THcShowerCalib(Int_t RunNumber) {
  fRunNumber = RunNumber;
};

//------------------------------------------------------------------------------

THcShowerCalib::~THcShowerCalib() {
};

//------------------------------------------------------------------------------

void THcShowerCalib::ExtractData() {

  char* fname = Form("Root_files/hcal_calib_%d.root",fRunNumber);
  cout << "THcShowerCalib::ExtractData: fname= " << fname << endl;

  //Reset ROOT and connect tree file
  gROOT->Reset();
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fname);
  if (!f) {
    f = new TFile(fname);
  }
  TTree* tree;
  f->GetObject("T",tree);

  //Declaration of leaves types

  //  Int_t           Ndata_H_cal_1pr_aneg_p;
  Double_t        H_cal_1pr_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_1pr_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_2ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_2ta_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_3ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_3ta_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_4ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_4ta_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_trp;
  Double_t        H_cal_trx;
  Double_t        H_cal_trxp;
  Double_t        H_cal_try;
  Double_t        H_cal_tryp;

   // Set branch addresses.

   // tree->SetBranchAddress("Ndata.H.cal.1pr.aneg_p",&Ndata_H_cal_1pr_aneg_p);
   tree->SetBranchAddress("H.cal.1pr.aneg_p",H_cal_1pr_aneg_p);
   tree->SetBranchAddress("H.cal.1pr.apos_p",H_cal_1pr_apos_p);

   tree->SetBranchAddress("H.cal.2ta.aneg_p",H_cal_2ta_aneg_p);
   tree->SetBranchAddress("H.cal.2ta.apos_p",H_cal_2ta_apos_p);

   tree->SetBranchAddress("H.cal.3ta.aneg_p",H_cal_3ta_aneg_p);
   tree->SetBranchAddress("H.cal.3ta.apos_p",H_cal_3ta_apos_p);

   tree->SetBranchAddress("H.cal.4ta.aneg_p",H_cal_4ta_aneg_p);
   tree->SetBranchAddress("H.cal.4ta.apos_p",H_cal_4ta_apos_p);

   tree->SetBranchAddress("H.cal.trp",&H_cal_trp);
   tree->SetBranchAddress("H.cal.trx",&H_cal_trx);
   tree->SetBranchAddress("H.cal.trxp",&H_cal_trxp);
   tree->SetBranchAddress("H.cal.try",&H_cal_try);
   tree->SetBranchAddress("H.cal.tryp",&H_cal_tryp);

   Long64_t nentries = tree->GetEntries();
   cout << "THcShowerCalib::ExtractData: nentries= " << nentries << endl;

   char* FName = Form("raw_data/%d_raw.dat",fRunNumber);
   cout << "ExtractData: FName=" << FName << endl;
   ofstream output;
   output.open(FName,ios::out);

   Long64_t nbytes = 0;
   for (Long64_t i=0; i<nentries;i++) {
     nbytes += tree->GetEntry(i);
     Int_t nhit = 0;
     for (Int_t j=0; j<THcShTrack::fNrows; j++) {
       if (H_cal_1pr_apos_p[j]>0. || H_cal_1pr_aneg_p[j]>0.) nhit++;
       if (H_cal_2ta_apos_p[j]>0. || H_cal_2ta_aneg_p[j]>0.) nhit++;
       if (H_cal_3ta_apos_p[j]>0. || H_cal_3ta_aneg_p[j]>0.) nhit++;
       if (H_cal_4ta_apos_p[j]>0. || H_cal_4ta_aneg_p[j]>0.) nhit++;
     }
     output << nhit << " " << H_cal_trp << " " 
	  << H_cal_trx << " " << H_cal_trxp << " " 
   	  << H_cal_try << " " << H_cal_tryp << endl;
     for (Int_t j=0; j<THcShTrack::fNrows; j++) {
       if (H_cal_1pr_apos_p[j]>0. || H_cal_1pr_aneg_p[j]>0.)
	 output << H_cal_1pr_apos_p[j] << " " << H_cal_1pr_aneg_p[j] << " "
	      << j+1 << endl;
     }
     for (Int_t j=0; j<THcShTrack::fNrows; j++) {
       if (H_cal_2ta_apos_p[j]>0. || H_cal_2ta_aneg_p[j]>0.)
	 output << H_cal_2ta_apos_p[j] << " " << H_cal_2ta_aneg_p[j] << " "
	      << j+1 + THcShTrack::fNrows << endl;
     }
     for (Int_t j=0; j<THcShTrack::fNrows; j++) {
       if (H_cal_3ta_apos_p[j]>0. || H_cal_3ta_aneg_p[j]>0.)
	 output << H_cal_3ta_apos_p[j] << " " << H_cal_3ta_aneg_p[j] << " "
	      << j+1 + 2*THcShTrack::fNrows << endl;
     }
     for (Int_t j=0; j<THcShTrack::fNrows; j++) {
       if (H_cal_4ta_apos_p[j]>0. || H_cal_4ta_aneg_p[j]>0.)
	 output << H_cal_4ta_apos_p[j] << " " << H_cal_4ta_aneg_p[j] << " "
	      << j+1 + 3*THcShTrack::fNrows << endl;
     }

   }

   output.close();
   cout << "THcShowerCalib::ExtractData: nbytes= " << nbytes << endl;
}

//------------------------------------------------------------------------------

void THcShowerCalib::Init() {

  char* FName = Form("raw_data/%d_raw.dat",fRunNumber);
  cout << "Init: FName=" << FName << endl;
  fDataStream.open(FName,ios::in);

  hEunc = new TH1F("hEunc", "Edep/P uncalibrated", 500, 0., 5.);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 150, 0., 1.5);
  //hPvsEcal = new TH2F("hPvsEcal", "P versus Edep/P ",150,0.,1.5, 100,1.9,2.4);
hPvsEcal = new TH2F("hPvsEcal", "P versus Edep/P ",150,0.,1.5, 100,0.7,0.9);
  
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) fHitCount[i] = 0;

  //  fe0 = new TVectorD(THcShRawTrack::fNpmts);
  //  fq0 = new TVectorD(THcShRawTrack::fNpmts);
  //  fQ = new TMatrixDSym(THcShRawTrack::fNpmts,THcShRawTrack::fNpmts);
  //  falphaU = new TVectorD(THcShRawTrack::fNpmts);
  //  falphaC = new TVectorD(THcShRawTrack::fNpmts);

  fe0 = 0.;

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    fqe[i] = 0.;
    fq0[i] = 0.;
    falphaU[i] = 0.;
    falphaC[i] = 0.;
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++) {
      fQ[i][j] = 0.;
    }
  }

  for (UInt_t iblk=0; iblk<THcShTrack::fNblks; iblk++) {
    if (iblk < THcShTrack::fNnegs) {
      falpha0[iblk] = 0.5;
      falpha0[THcShTrack::fNblks+iblk] = 0.5;
    }
    else {
      falpha0[iblk] = 1.;
    }
  };

  for (UInt_t ipmt=0; ipmt<THcShTrack::fNpmts; ipmt++) {
    falpha1[ipmt] = 1.;
  }

  //  for (UInt_t ipmt=0; ipmt<THcShRawTrack::fNpmts; ipmt++) {
  //    cout << "falpha0 " << ipmt << " = " << falpha0[ipmt] << endl;
  //  }

};

//------------------------------------------------------------------------------

void THcShowerCalib::CalcThresholds() {

  Int_t nev = 0;

  THcShTrack trk;

  while (ReadShRawTrack(trk)) {

    //    trk.Print();

    trk.SetEs(falpha0);
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

  cout << "CalcThreshods: fLoThr=" << fLoThr << "  fHiThr=" << fHiThr 
       << "  nev=" << nev << endl;

  Int_t nbins = hEunc->GetNbinsX();
  Int_t nlo = hEunc->FindBin(fLoThr);
  Int_t nhi = hEunc->FindBin(fHiThr);

  cout << "CalcThresholds: nlo=" << nlo << "  nhi=" << nhi 
       << "  nbins=" << nbins << endl;
  
  hEuncSel = (TH1F*)hEunc->Clone("hEuncSel");
  
  for (Int_t i=0; i<nlo; i++) hEuncSel->SetBinContent(i, 0.);
  for (Int_t i=nhi; i<nbins+1; i++) hEuncSel->SetBinContent(i, 0.);

};

//------------------------------------------------------------------------------

Bool_t THcShowerCalib::ReadShRawTrack(THcShTrack &trk) {

  UInt_t nhit;
  Double_t p, x,xp, y,yp;

  if (fDataStream >> nhit >> p >> x >> xp >> y >> yp) {
    //    cout << nhit << " " << p << " " << x << " " << xp << " " << y << " "
    //	 << yp << endl;

    trk.SetTrack(nhit, p, x, xp, y, yp);

    for (UInt_t i=0; i<nhit; i++) {

      Double_t adc_pos, adc_neg;
      UInt_t nb;

      fDataStream >> adc_pos >> adc_neg >> nb;
      //      cout << adc_pos << " " << adc_neg << " " << nb << endl;

      trk.AddHit(adc_pos, adc_neg, 0., 0., nb);
    };
    return 1;

  }
  else {
    return 0;
  };

}

//------------------------------------------------------------------------------

void THcShowerCalib::ComposeVMs() {

  // Reset.
  fDataStream.clear() ;
  fDataStream.seekg(0, ios::beg) ;

  fNev = 0;
  THcShTrack trk;

  while (ReadShRawTrack(trk)) {

    if (trk.CheckHitNumber()) {

	trk.SetEs(falpha0);
	Double_t Enorm = trk.Enorm();
	if (Enorm>fLoThr && Enorm<fHiThr) {

	  trk.SetEs(falpha1);
	  //	  trk.Print();

	  fe0 += trk.GetP();

	  vector<pmt_hit> pmt_hit_list;

	  for (UInt_t i=0; i<trk.GetNhits(); i++) {

	    THcShHit* hit = trk.GetHit(i);
	    //	    hit->Print();

	    UInt_t nb = hit->GetBlkNumber();

	    fqe[nb-1] += hit->GetEpos() * trk.GetP();
	    fq0[nb-1] += hit->GetEpos();

	    pmt_hit_list.push_back( pmt_hit{hit->GetEpos(), nb} );

	    fHitCount[nb-1]++;

	    if (nb <= THcShTrack::fNnegs) {
	      fqe[THcShTrack::fNblks+nb-1] += hit->GetEneg() * trk.GetP();
	      fq0[THcShTrack::fNblks+nb-1] += hit->GetEneg();

	      pmt_hit_list.push_back(pmt_hit{hit->GetEneg(),
		    THcShTrack::fNblks+nb} );

	      fHitCount[THcShTrack::fNblks+nb-1]++;
	    };

	    //	    cout << "  fqe " << nb-1 << " =" << fqe[nb-1];
	    //	    if (nb <= THcShTrack::fNnegs)
	    //	      cout << "  fqe " << THcShTrack::fNblks+nb-1 << " =" 
	    //		   << fqe[THcShTrack::fNblks+nb-1];
	    //	    cout << endl;

	    //	    cout << "  fq0 " << nb-1 << " =" << fq0[nb-1];
	    //	    if (nb <= THcShTrack::fNnegs)
	    //	      cout << "  fq0 " << THcShTrack::fNblks+nb-1 << " =" 
	    //		   << fq0[THcShTrack::fNblks+nb-1];
	    //	    cout << endl;
	  }      //over hits


	  //	  for (vector<pmt_hit>::iterator it=pmt_hit_list.begin();
	  //	       it < pmt_hit_list.end(); it++) {
	  //	    cout << "hit: " << (*it).signal << "  " << (*it).channel
	  //		 << endl;
	  //	  }

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

	  //	  getchar();

	  fNev++;
	};
    };
  };

  fe0 /= fNev;
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    fqe[i] /= fNev;
    fq0[i] /= fNev;
  }

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++)
      fQ[i][j] /= fNev;

  //  cout << "ComposeVMs: fNev=" << fNev << endl;  
  //  cout << "            fe0=" << fe0 << endl;
  //  cout << "            fqe:" << endl;
  //  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
  //    cout << "  fqe " << i << " = " << fqe[i] << endl;
  //  cout << "            fq0:" << endl;
  //  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
  //    cout << "  fq0 " << i << " = " << fq0[i] << endl;

  //  cout << "            fQ[i,i], fQ[i,1], fQ[1,i]:" << endl;
  //  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
  //    cout << i << ":  " << fQ[i][i] << "  " << fQ[i][1] << "  " << fQ[1][i]
  //	 << endl;

  //  cout << "ComposeVMs: vector fe0:" << endl;
  //  (*fe0).Print();
  //  cout << "ComposeVMs: matrix fQ:" << endl;
  //  (*fQ).Print();

  ofstream q0out;
  q0out.open("q0.d",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    q0out << fq0[i] << " " << i << endl;
  q0out.close();

  ofstream qeout;
  qeout.open("qe.d",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    qeout << fqe[i] << " " << i << endl;
  qeout.close();


  ofstream Qout;
  Qout.open("Q.d",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++)
      Qout << fQ[i][j] << " " << i << " " << j << endl;
  Qout.close();

};

//------------------------------------------------------------------------------

void THcShowerCalib::SolveAlphas() {

  TMatrixD Q(THcShTrack::fNpmts,THcShTrack::fNpmts);
  TVectorD q0(THcShTrack::fNpmts);
  TVectorD qe(THcShTrack::fNpmts);
  TVectorD au(THcShTrack::fNpmts);
  TVectorD ac(THcShTrack::fNpmts);
  Bool_t ok;

  cout << "Solving Alphas..." << endl;
  cout << endl;

  cout << "Hit couts:" << endl;
  UInt_t j = 0;
  cout << "Positives:";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;
  for (Int_t k=0; k<3; k++) {
    cout << "          ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }
  cout << "Negatives:";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;
  cout << "          ";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    q0[i] = fq0[i];
    qe[i] = fqe[i];
    for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
      Q[i][k] = fQ[i][k];
    }
  }

  // Sanity check.

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {

    if (fHitCount[i] == 0) {

      if (q0[i] != 0. || qe[i] != 0.) {

	cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	     << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i];

	for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
	  if (Q[i][k] !=0. || Q[k][i] !=0.)
	    cout << ", Q[" << i << "," << k << "]=" << Q[i][k]
		 << ", Q[" << k << "," << i << "]=" << Q[k][i];
	}

	cout << " ***" << endl;
      }
    }

    if ( (fHitCount[i] != 0) && (q0[i] == 0. || qe[i] == 0.) ) {
      cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	   << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i]
	   << " ***" << endl;
    }

  } //sanity check

  // Low hit number channels.

  cout << endl;
  cout << "Channels with hit number less than " << fMinHitCount 
       << " will not be calibrated." << endl;
  cout << endl;

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {

    if (fHitCount[i] < fMinHitCount) {
      cout << "Channel " << i << ", " << fHitCount[i]
	   << " hits, will not be calibrated." << endl;
      q0[i] = 0.;
      qe[i] = 0.;
      for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
	Q[i][k] = 0.;
	Q[k][i] = 0.;
      }
      Q[i][i] = 1.;
    }

  }

  TDecompLU lu(Q);
  Double_t d1,d2;
  lu.Det(d1,d2);
  cout << "cond:" << lu.Condition() << endl;
  cout << "det :" << d1*TMath::Power(2.,d2) << endl;
  cout << "tol :" << lu.GetTol() << endl;

  au = lu.Solve(qe,ok);
  cout << "au: ok=" << ok << endl;
  //  au.Print();

  Double_t t1 = fe0 - au * q0;
  //  cout << "t1 =" << t1 << endl;

  TVectorD Qiq0(THcShTrack::fNpmts);
  Qiq0 = lu.Solve(q0,ok);
  cout << "Qiq0: ok=" << ok << endl;
  //  Qiq0.Print();

  Double_t t2 = q0 * Qiq0;
  //  cout << "t2 =" << t2 << endl;

  ac = (t1/t2) *Qiq0 + au;
  //  cout << "ac:" << endl;
  //  ac.Print();

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    falphaU[i] = au[i];
    falphaC[i] = ac[i];
  }

}

//------------------------------------------------------------------------------

void THcShowerCalib::FillHEcal() {

  // Reset.
  fDataStream.clear() ;
  fDataStream.seekg(0, ios::beg) ;

  ofstream output;
  output.open("calibrated.d",ios::out);

  Int_t nev = 0;

  THcShTrack trk;

  while (ReadShRawTrack(trk)) {

    //    trk.Print();

    trk.SetEs(falphaC);
    //    trk.SetEs(falphaU);
    Double_t P = trk.GetP();
    Double_t Enorm = trk.Enorm();

    hEcal->Fill(Enorm);
    hPvsEcal->Fill(Enorm,P/1000.,1.);

    output << Enorm*P/1000. << " " << P/1000. << endl;

    nev++;
  };

  output.close();

  cout << "FillHEcal: " << nev << " events filled" << endl;
};

//------------------------------------------------------------------------------

void THcShowerCalib::SaveAlphas() {

  ofstream output;
  char* fname = Form("hcal.param.%d",fRunNumber);
  cout << "SaveAlphas: fname=" << fname << endl;

  output.open(fname,ios::out);

  output << "; Calibration constants for run " << fRunNumber 
	 << ", " << fNev << " events processed" << endl;
  output << endl;

  UInt_t j = 0;
  output << "hcal_pos_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
  output << endl;
  for (Int_t k=0; k<3; k++) {
    output << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
    output << endl;
  }
  output << "hcal_neg_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
  output << endl;
  output << "                  ";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(3) << falphaC[j++] << ",";
  output << endl;
  for (Int_t k=0; k<2; k++) {
    output << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      output << fixed << setw(6) << setprecision(3) << 0. << ",";
    output << endl;
  }

  output.close();
}

//------------------------------------------------------------------------------

/*
void THcShowerCalib::PlotHistos() {

  TCanvas* Canvas =
    new TCanvas("Canvas", "HMS Shower Counter calibration", 1000, 667);
  Canvas->Divide(1,2);

  Canvas->cd(1);

  hEunc->DrawCopy();
  
  hEuncSel->SetFillColor(kGreen);
  hEuncSel->DrawCopy("same");

};
*/

#endif
