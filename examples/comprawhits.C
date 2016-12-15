void comprawhits(Int_t run)
{
  TFile* f = new TFile(Form("hodtest_%d.root",run));
  cout << "hcana root file " << Form("hodtest_%d.root",run) << endl;
  TH1F* h[6];
  h[0] = chposadc1;
  h[1] = chnegadc1;
  h[2] = chposadc2;
  h[3] = chnegadc2;
  h[4] = chposadc3;
  h[5] = chposadc4;

  TFile* f1 = new TFile(Form("%d_hbk.root",run));
  cout << "Engine root file " << Form("%d_hbk.root",run) << endl;
  TH1F* h1[6];
  switch (run) {
  case 50017 :
    h1[0] = h212;   //A+
    h1[1] = h213;   //A-
    h1[2] = h214;   //B+
    h1[3] = h215;   //B-
    h1[4] = h216;   //C+
    h1[5] = h217;   //D+
    break;
  default :
    h1[0] = h213;   //A+
    h1[1] = h217;   //A-
    h1[2] = h214;   //B+
    h1[3] = h218;   //B-
    h1[4] = h215;   //C+
    h1[5] = h216;   //D+
  }

  TCanvas *c1 = new TCanvas("c1", "Shower Hit Maps", 1000, 667);
  c1->Divide(2, 3);

  for (int j = 0; j < 6; j++){
    c1->cd(j+1);
    h[j]->SetFillColor(kGreen);
    h[j]->SetFillStyle(3345);
    h[j]->Draw();

    h1[j]->SetFillColor(kBlue);
    h1[j]->SetFillStyle(3354);
    h1[j]->Draw("same");
  }

  //Differences

  TCanvas *c2 = new TCanvas("c2", "Shower Hit Map Differences", 1000, 667);
  c2->Divide(2, 3);

  for (int j = 0; j < 6; j++){

    c2->cd(j+1);

    TH1F* dif = h[j]->Clone();
    dif->Add(h[j],h1[j],1.,-1.);

    dif->SetTitle(Form("Difference %d",j+1));
    dif->SetFillColor(kRed);
    dif->SetLineColor(kRed);
    dif->SetLineWidth(1);
    dif->SetFillStyle(1111);
    dif->Draw();
  }

}
