{
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "SOS Scintillator Hit Maps", 800, 800);
  c1->Divide(2, 4);

  TH1F* h[16];

  h[0] = spostdc1;
  h[1] = snegtdc1;
  h[4] = spostdc2;
  h[5] = snegtdc2;
  h[8] = spostdc3;
  h[9] = snegtdc3;
  h[12] = spostdc4;
  h[13] = snegtdc4;

  h[2] = sposadc1;
  h[3] = snegadc1;
  h[6] = sposadc2;
  h[7] = snegadc2;
  h[10] = sposadc3;
  h[11] = snegadc3;
  h[14] = sposadc4;
  h[15] = snegadc4;

  for(Int_t ipad=1;ipad<=8;ipad++) {
    c1->cd(ipad);
    Int_t ihp = 2*(ipad-1);
    Int_t ihn = ihp+1;
    h[ihp]->SetFillColor(kGreen);
    h[ihp]->SetFillStyle(3345);
    h[ihp]->Draw();
    h[ihn]->SetFillColor(kBlue);
    h[ihn]->SetFillStyle(3354);
    h[ihn]->Draw("same");
  }

}
