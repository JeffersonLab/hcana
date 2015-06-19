#include <TFile.h>
#include <TROOT.h>
#include <iostream>
#include <TMath.h>
#include <TLegend.h>

using namespace std;

void comp_pcal_etot() {

  // Compare total energy depositions in the SHMS calorimeter,
  // obtained from calibrations with old (Fortran) and new (C++?Root) codes
  // of simulated data.

  // Histograms.

  TH1F* he_new = new TH1F("e_new", "Edep/P from C++/Root calibration",
				100, 0.011, 1.8);
  he_new->SetLineColor(kRed);

  TH1F* he_old = (TH1F*) he_new->Clone("Edep/P from Fortran calibration");
  he_old->SetLineColor(kBlue);

  TH1F* ee_diff = new TH1F("ee_diff",
		  "Event by event new - old Ecalo difference", 200,-1.,1.);
  TH1F* ee_abs_diff = new TH1F("ee_abs_diff",
		  "Event by event absolute new - old Ecalo difference",
			       100000,0.,0.1);
  TH2F* evse = new TH2F("evse","Event by event new vs old Ecalo",
			210,-0.1,2., 210,-0.1,2.);
  evse->SetMarkerStyle(3);

  TH3F* diff_vs_xy = new TH3F("diff_vs_xy","Event by event difference vs X & Y",
			      100, -50., 50., 120, -60., 60., 400, -0.02, 0.02);
  //  diff_vs_xy->SetMarkerStyle(3);

  TH2F* diff_vs_x = new TH2F("diff_vs_x","Event by event difference vs X",
			      100, -50., 50., 400, -0.02, 0.02);

  TH2F* diff_vs_y = new TH2F("diff_vs_y","Event by event difference vs Y",
			      120, -60., 60., 400, -0.02, 0.02);

  // Read data and fill in histograms.

  ifstream fin_old;
  fin_old.open("old_calib.cal_data",ios::in);

  ifstream fin_new;
  fin_new.open("new_calibrated.d",ios::in);

  double e_old, p_old, y_old;
  double e_new, p_new, x_new, y_new;

  // Loop over entries.

  while ((fin_old >> e_old >> p_old >> y_old) &&
	 (fin_new >> e_new >> p_new >> x_new >> y_new)) {

    double en_old = e_old/p_old;
    double en_new = e_new/p_new;

    he_new->Fill(en_new, 1.);
    he_old->Fill(en_old, 1.);
    ee_diff->Fill(en_new - en_old, 1.);
    ee_abs_diff->Fill(TMath::Abs(en_new - en_old), 1.);
    evse->Fill(en_old, en_new, 1.);
    diff_vs_xy->Fill(x_new, y_new, en_new-en_old, 1.);
    diff_vs_x->Fill(x_new, en_new-en_old, 1.);
    diff_vs_y->Fill(y_new, en_new-en_old, 1.);

  }

  fin_new.close();
  fin_old.close();

  // Plot.

  TCanvas* c1 = new TCanvas("c1","Normalized energy depositons",600,750);

  c1->Divide(1,2);
  c1->cd(1);
  he_new->Draw();
  he_old->Draw("same");

  TLegend* leg = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg->AddEntry(he_new,"New( C++/Root) calibration","l");
  leg->AddEntry(he_old,"Old (Fortran) calibration","l");
  leg->Draw();

  // Difference between the histograms.

  c1->cd(2);

  TH1F* dif = he_new->Clone("difference");
  dif->Reset();
  dif->Add(he_new,he_old,1.,-1.);

  dif->SetTitle("new (C++/Root) -- old (Fortran) difference");
  dif->SetFillColor(kRed);
  dif->SetLineColor(kRed);
  dif->SetLineWidth(1);
  dif->SetFillStyle(1111);
  dif->Draw();

  TCanvas* c2 = new TCanvas("c2","Event by event new - old Ecalo diff.",
			    600,750);

  c2->Divide(1,2);
  c2->cd(1);
  gPad->SetLogy();
  ee_diff->Draw();
  c2->cd(2);
  gPad->SetLogy();
  gPad->SetLogx();
  ee_abs_diff->Draw();

  TCanvas* c3 = new TCanvas("c3","Event by event new - old vs X & Y",
			    750, 750);
  c3->Divide(2,2);
  c3->cd(1);
  diff_vs_x->Draw();
  c3->cd(2);
  diff_vs_y->Draw();
  c3->cd(3);
  diff_vs_xy->Draw();

}
