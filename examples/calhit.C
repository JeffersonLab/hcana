{
  TFile* f = new TFile("hodtest.root");
 
  TCanvas *c1 = new TCanvas("c1", "Shower Hit Maps", 800, 800); 
  c1->Divide(1, 4);

  TH1F* h[6];

  h[0] = chposadc1;
  h[1] = chnegadc1;
  h[2] = chposadc2;
  h[3] = chnegadc2;
  h[4] = chposadc3;
  h[5] = chposadc4;

  
    c1->cd(1);
    h[0]->SetFillColor(kGreen);
    h[0]->SetFillStyle(3345);
    h[0]->Draw();
    h[1]->SetFillColor(kBlue);
    h[1]->SetFillStyle(3354);
    h[1]->Draw("same");
  
    c1->cd(2);
    h[2]->SetFillColor(kGreen);
    h[2]->SetFillStyle(3345);
    h[2]->Draw();
    h[3]->SetFillColor(kBlue);
    h[3]->SetFillStyle(3354);
    h[3]->Draw("same");

    c1->cd(3);
    h[4]->SetFillColor(kGreen);
    h[4]->SetFillStyle(3345);
    h[4]->Draw();

    c1->cd(4);
    h[5]->SetFillColor(kGreen);
    h[5]->SetFillStyle(3345);
    h[5]->Draw();
}
 
