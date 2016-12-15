 {
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Drift Distance", 800, 800);
  c1->Divide(2, 6);

  TH1F* h[12];

  h[0] = hdc1x1_dd;
  h[1] = hdc1y1_dd;
  h[2] = hdc1u1_dd;
  h[3] = hdc1v1_dd;
  h[4] = hdc1y2_dd;
  h[5] = hdc1x2_dd;
  h[6] = hdc2x1_dd;
  h[7] = hdc2y1_dd;
  h[8] = hdc2u1_dd;
  h[9] = hdc2v1_dd;
  h[10] = hdc2y2_dd;
  h[11] = hdc2x2_dd;

  for(Int_t ih=0;ih<12;ih++) {
    Int_t ipad = 2*(ih%6)+1 + ih/6;
    cout << ipad << endl;
    c1->cd(ipad);
    h[ih]->SetFillColor(kBlue);
    h[ih]->SetFillStyle(3354);
    h[ih]->Draw();
  }

}
