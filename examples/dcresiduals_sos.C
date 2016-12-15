{
  TFile* f = new TFile("hodtest.root");
  TTree *T=(TTree*)f->Get("T");
  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Residuals", 800, 1000);
  c1->Divide(2,6);

  TH1F* h=new TH1F("h", "Residual", 100, -1.0, 1.0);

  c1->cd(1);
  T->Draw("S.dc.residual[0]","S.dc.residual[0] < 999");
  c1->cd(2);
  T->Draw("S.dc.residual[1]","S.dc.residual[1] < 999");
  c1->cd(3);
  T->Draw("S.dc.residual[2]","S.dc.residual[2] < 999");
  c1->cd(4);
  T->Draw("S.dc.residual[3]","S.dc.residual[3] < 999");
  c1->cd(5);
  T->Draw("S.dc.residual[4]","S.dc.residual[4] < 999");
  c1->cd(6);
  T->Draw("S.dc.residual[5]","S.dc.residual[5] < 999");
  c1->cd(7);
  T->Draw("S.dc.residual[6]","S.dc.residual[6] < 999");
  c1->cd(8);
  T->Draw("S.dc.residual[7]","S.dc.residual[7] < 999");
  c1->cd(9);
  T->Draw("S.dc.residual[8]","S.dc.residual[8] < 999");
  c1->cd(10);
  T->Draw("S.dc.residual[9]","S.dc.residual[9] < 999");
  c1->cd(11);
  T->Draw("S.dc.residual[10]","S.dc.residual[10] < 999");
  c1->cd(12);
  T->Draw("S.dc.residual[11]","S.dc.residual[11] < 999");
  delete h;
}
