{
  TFile* f = new TFile("hodtest.root");

  TCanvas *c1 = new TCanvas("c1", "Scintillator Hit Maps", 800, 800);
  c1->Divide(2, 4);

  TH1F* h[16];

  h[0] = hpostdc1;
  h[1] = hnegtdc1;
  h[4] = hpostdc2;
  h[5] = hnegtdc2;
  h[8] = hpostdc3;
  h[9] = hnegtdc3;
  h[12] = hpostdc4;
  h[13] = hnegtdc4;

  h[2] = hposadc1;
  h[3] = hnegadc1;
  h[6] = hposadc2;
  h[7] = hnegadc2;
  h[10] = hposadc3;
  h[11] = hnegadc3;
  h[14] = hposadc4;
  h[15] = hnegadc4;

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
