void compedeps(Int_t run)
{

  // Compare deposited energies in 4 planes from ENGINE with hcana.

  TFile* f = new TFile(Form("hodtest_%d.root",run));
  TH1F* h[4];
  h[0] = edep1;
  h[1] = edep2;
  h[2] = edep3;
  h[3] = edep4;

  TFile* f1 = new TFile(Form("%d_hbk.root",run));
  TH1F* h1[4];
  switch (run) {
  case 50017 :
    h1[0] = h683;   //A
    h1[1] = h684;   //B
    h1[2] = h685;   //C
    h1[3] = h686;   //D
    break;
  default :
    h1[0] = h632;   //A
    h1[1] = h633;   //B
    h1[2] = h634;   //C
    h1[3] = h635;   //D
  }

  TCanvas *c1 = new TCanvas("c1", "Shower raw Edeps", 1000, 667);
  c1->Divide(2, 2);

  for (int j = 0; j < 4; j++){

    c1->cd(j+1);

    h1[j]->SetFillColor(kBlue);
    h1[j]->SetFillStyle(3354);
    h1[j]->Draw();

    h[j]->SetFillColor(kGreen);
    h[j]->SetFillStyle(3345);
    h[j]->Draw("same");

    //    gPad->SetLogy();
  }

  //
  // Difference between the histograms.
  //

  TCanvas *c2 = new TCanvas("c2", "Edep histogram differences", 1000, 667);
  c2->Divide(2, 2);

  TH1F* d[4] = {h[0]->Clone(),h[0]->Clone(),h[0]->Clone(),h[0]->Clone()};

  for (Int_t j = 0; j < 4; j++){

    //    d[j]->Divide(h[j],h1[j]);
    d[j]->Add(h[j],h1[j],1.,-1.);

    c2->cd(j+1);

    d[j]->SetFillColor(kRed);
    d[j]->SetFillStyle(1111);
    d[j]->Draw();

    //    gPad->SetLogy();
  }

}
