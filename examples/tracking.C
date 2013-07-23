{
  TFile* f = new TFile("hodtest.root");
  TTree *T=(TTree*)f->Get("T");
  TCanvas *c1 = new TCanvas("c1", "Drift Tracking", 800, 1000); 
  c1->Divide(2,3);

  c1->cd(1);
  T->Draw("H.dc.nhit");
  c1->cd(2);
  T->Draw("H.dc.ntrack");
  c1->cd(3);
  T->Draw("H.dc.x:H.dc.y");
  c1->cd(4);
  T->Draw("H.dc.x:H.dc.xp");
  c1->cd(5);
  T->Draw("H.dc.y:H.dc.yp");
  c1->cd(6);
  T->Draw("H.dc.xp:H.dc.yp");
}
