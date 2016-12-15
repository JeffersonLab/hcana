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
  T->Draw("H.dc.x:H.dc.y>>h(,-20,20,,-50,50)");
  c1->cd(4);
  T->Draw("H.dc.x:H.dc.xp>>h(,-0.08,0.08,,-50,50");
  c1->cd(5);
  T->Draw("H.dc.y:H.dc.yp>>h(,-0.08,0.08,,-20,20");
  c1->cd(6);
  T->Draw("H.dc.xp:H.dc.yp>>h(,-0.08,0.08,,-0.08,0.08");
}
