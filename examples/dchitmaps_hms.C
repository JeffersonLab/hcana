 {
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Hit Maps", 800, 800);
  c1->Divide(2, 6);

  TH1F* h[12];

  h[0] = hdc1x1_wm;
  h[1] = hdc1y1_wm;
  h[2] = hdc1u1_wm;
  h[3] = hdc1v1_wm;
  h[4] = hdc1y2_wm;
  h[5] = hdc1x2_wm;
  h[6] = hdc2x1_wm;
  h[7] = hdc2y1_wm;
  h[8] = hdc2u1_wm;
  h[9] = hdc2v1_wm;
  h[10] = hdc2y2_wm;
  h[11] = hdc2x2_wm;

  for(Int_t ih=0;ih<12;ih++) {
    Int_t ipad = 2*(ih%6)+1 + ih/6;
    cout << ipad << endl;
    c1->cd(ipad);
    h[ih]->SetFillColor(kBlue);
    h[ih]->SetFillStyle(3354);
    h[ih]->Draw();
  }

}
