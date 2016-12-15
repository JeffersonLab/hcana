 {
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Drift Distance", 800, 800);
  c1->Divide(2, 6);

  TH1F* h[12];

  h[0] = sdc1u1_dd;
  h[1] = sdc1u2_dd;
  h[2] = sdc1x1_dd;
  h[3] = sdc1x2_dd;
  h[4] = sdc1v1_dd;
  h[5] = sdc1v2_dd;
  h[6] = sdc2u1_dd;
  h[7] = sdc2u2_dd;
  h[8] = sdc2x1_dd;
  h[9] = sdc2x2_dd;
  h[10] = sdc2v1_dd;
  h[11] = sdc2v2_dd;

  for(Int_t ih=0;ih<12;ih++) {
    Int_t ipad = 2*(ih%6)+1 + ih/6;
    cout << ipad << endl;
    c1->cd(ipad);
    h[ih]->SetFillColor(kBlue);
    h[ih]->SetFillStyle(3354);
    h[ih]->Draw();
  }

}
