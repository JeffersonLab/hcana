{
  TFile* f = new TFile("hodtest.root");
  TH1F* h[6];
  h[0] = chposadc1;
  h[1] = chnegadc1;
  h[2] = chposadc2;
  h[3] = chnegadc2;
  h[4] = chposadc3;
  h[5] = chposadc4;

  TFile* f1 = new TFile("50017_hbk.root");
  TH1F* h1[6];
  h1[0] = h212;   //A+
  h1[1] = h216;   //A-
  h1[2] = h213;   //B+
  h1[3] = h217;   //B-
  h1[4] = h214;   //C+
  h1[5] = h215;   //D+

  TCanvas *c1 = new TCanvas("c1", "Shower Hit Maps", 1000, 750); 
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

} 
