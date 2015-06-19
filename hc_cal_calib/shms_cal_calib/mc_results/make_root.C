#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

using namespace std;

// Generate root file output from input file from the SHMS calorimeter
// simulation.

#define Nrows_pr 14
#define Ncols_pr  2
#define Nrows_sh 16
#define Ncols_sh 14

void make_root(string inp_name="5", int p_cent=5.) {

  // Input and output.

  string inp_file = inp_name + ".r";
  string out_file = inp_name + ".root";
  cout << inp_file << " ==> " << out_file << endl;

  ifstream fin;
  fin.open(inp_file.c_str(),ios::in);

  TFile *file_out = new TFile(out_file.c_str(), "Recreate");

  // Variables.

  int nhit;
  double x, y, xp, yp, p;
  int ind;
  double tg_dp;

  double adc_pr[Nrows_pr][Ncols_pr];
  double adc_sh[Nrows_sh][Ncols_sh];

  // The output tree.

  TTree *tr = new TTree("T", "SHMS calo. MC events");
  string leaf = Form("P.pr.a_p[%i][%i]/D", Nrows_pr, Ncols_pr);
  tr->Branch("P.pr.a_p", &adc_pr, leaf.c_str());
  leaf = Form("P.sh.a_p[%i][%i]/D", Nrows_sh, Ncols_sh);
  tr->Branch("P.sh.a_p", &adc_sh, leaf.c_str());
  tr->Branch("P.tr.y", &x, "P.tr.y/D");      // swap X in MC and Y in hcana
  tr->Branch("P.tr.x", &y, "P.tr.x/D");      // swap X in MC and Y in hcana
  tr->Branch("P.tr.ph", &xp, "P.tr.ph/D");   // swap Xp in MC and Yp in hcana
  tr->Branch("P.tr.th", &yp, "P.tr.th/D");   // swap Xp in MC and Yp in hcana
  tr->Branch("P.tr.p",  &p,  "P.tr.p/D");
  tr->Branch("P.tr.tg_dp",  &tg_dp,  "P.tr.tg_dp/D");

  // Read and fill tree cicle.

  while (fin >> nhit >> x >> y >> xp >> yp >> p >> ind) {

    //    cout << nhit << " " << x << " " << y << " " << xp << " " << yp << " "
    //	 << p << " " << ind << endl;

    tg_dp = (p/p_cent - 1.) * 100.;

    for (int k=0; k<Ncols_pr; k++)
      for (int i=0; i<Nrows_pr; i++)
	adc_pr[i][k] = 0.;

    for (int k=0; k<Ncols_sh; k++)
      for (int i=0; i<Nrows_sh; i++)
	adc_sh[i][k] = 0.;

    for (int i=0; i<nhit; i++) {

      int adc, row, col, det;
      fin >> adc >> row >> col >> det;
      //      cout << adc << " " << row << " " << col << " " << det << endl;

      if (adc > 0) {
	switch (det) {
	case 1 :                        //Preshower
	  adc_pr[row-1][col-1] = adc;
	  break;
	case 2 :                        //Shower
	  adc_sh[row-1][col-1] = adc;
	  break;
	default:
	  cout << "*** Wrong detector: " << det << " ***" << endl;
	}
      }

    }   //hits

    tr->Fill();

  }     //events

  tr->Write();

  fin.close();
  file_out->Close();
}
