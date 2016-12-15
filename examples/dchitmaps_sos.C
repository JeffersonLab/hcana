 {
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "Drift Chamber Hit Maps", 800, 800);
  c1->Divide(2, 6);

  TH1F* h[12];

  h[0] = sdc1u1_wm;
  h[1] = sdc1u2_wm;
  h[2] = sdc1x1_wm;
  h[3] = sdc1x2_wm;
  h[4] = sdc1v1_wm;
  h[5] = sdc1v2_wm;
  h[6] = sdc2u1_wm;
  h[7] = sdc2u2_wm;
  h[8] = sdc2x1_wm;
  h[9] = sdc2x2_wm;
  h[10] = sdc2v1_wm;
  h[11] = sdc2v2_wm;

  for(Int_t ih=0;ih<12;ih++) {
    Int_t ipad = 2*(ih%6)+1 + ih/6;
    cout << ipad << endl;
    c1->cd(ipad);
    h[ih]->SetFillColor(kBlue);
    h[ih]->SetFillStyle(3354);
    h[ih]->Draw();
  }

}
