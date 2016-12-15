{
  TFile* f = new TFile("hodtest.root");
  TTree *T=(TTree*)f->Get("T");
  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Residuals", 800, 1000);
  c1->Divide(2,6);

  TH1F* h=new TH1F("h", "Residual", 100, -1.0, 1.0);

  c1->cd(1);
  T->Draw("H.dc.residual[0]","H.dc.residual[0] < 999");
  c1->cd(2);
  T->Draw("H.dc.residual[1]","H.dc.residual[1] < 999");
  c1->cd(3);
  T->Draw("H.dc.residual[2]","H.dc.residual[2] < 999");
  c1->cd(4);
  T->Draw("H.dc.residual[3]","H.dc.residual[3] < 999");
  c1->cd(5);
  T->Draw("H.dc.residual[4]","H.dc.residual[4] < 999");
  c1->cd(6);
  T->Draw("H.dc.residual[5]","H.dc.residual[5] < 999");
  c1->cd(7);
  T->Draw("H.dc.residual[6]","H.dc.residual[6] < 999");
  c1->cd(8);
  T->Draw("H.dc.residual[7]","H.dc.residual[7] < 999");
  c1->cd(9);
  T->Draw("H.dc.residual[8]","H.dc.residual[8] < 999");
  c1->cd(10);
  T->Draw("H.dc.residual[9]","H.dc.residual[9] < 999");
  c1->cd(11);
  T->Draw("H.dc.residual[10]","H.dc.residual[10] < 999");
  c1->cd(12);
  T->Draw("H.dc.residual[11]","H.dc.residual[11] < 999");
  delete h;
}
