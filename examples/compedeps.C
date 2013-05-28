{

  // Compare deposited energies in 4 planes from ENGINE with hcana.

  TFile* f = new TFile("hodtest.root");
  TH1F* h[4];
  h[0] = edep1;
  h[1] = edep2;
  h[2] = edep3;
  h[3] = edep4;

  TFile* f1 = new TFile("50017_hbk.root");
  TH1F* h1[4];
  h1[0] = h683;   //A
  h1[1] = h684;   //B
  h1[2] = h685;   //C
  h1[3] = h686;   //C

  TCanvas *c1 = new TCanvas("c1", "Shower raw Edeps", 1000, 750); 
  c1->Divide(2, 2);

  for (int j = 0; j < 4; j++){
    c1->cd(j+1);
    h[j]->SetFillColor(kGreen);
    h[j]->SetFillStyle(3345);
    h[j]->Draw();
    
    h1[j]->SetFillColor(kBlue);
    h1[j]->SetFillStyle(3354);
    h1[j]->Draw("same");

    gPad->SetLogy();
  }

} 
