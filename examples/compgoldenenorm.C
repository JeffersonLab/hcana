void compgoldenenorm(Int_t run=52949)
{

  //Comapare hsshtrk from Engine with gold.enorm from hcana.

  TFile* f = new TFile(Form("hodtest_%d.root",run));
  cout << "hcana root file " << Form("hodtest_%d.root",run) << endl;
  TH1F* h = golden_enorm;

  TFile* f1 = new TFile(Form("%d_hbk.root",run));
  cout << "Engine root file " << Form("%d_hbk.root",run) << endl;
  TH1F* h1;
  switch (run) {
  case 50017 :
  //    h1 = h212;   //A+
    break;
  default :
    //    h1 = h438;   //hsshtrk, engine/replay/paw
    h1 = h426;   //hsshtrk, engine/replay_mkj/paw
  }

  TCanvas *c1 = new TCanvas("c1", "Golden track E/P", 800, 1000);

  c1->Divide(1,2);

  //  gPad->SetLogy();

  c1->cd(1);

  h1->SetFillColor(kGreen);
  h1->SetLineColor(kGreen);
  h1->Draw();

  h->SetLineColor(kBlue);
  h->SetFillStyle(0);
  h->SetLineWidth(2);
  h->Draw("same");

  TLatex l;
  l.SetTextSize(0.04);
  Float_t maxy = h1->GetBinContent(h1->GetMaximumBin());
  Float_t xmin = h1->GetXaxis()->GetXmin();
  Float_t xmax = h1->GetXaxis()->GetXmax();
  Float_t xt = xmin + 0.75*(xmax-xmin);

  l.SetTextColor(kGreen);
  l.DrawLatex(xt,0.65*maxy,"Engine");
  l.SetTextColor(kBlue);
  l.DrawLatex(xt,0.75*maxy,"hcana");

  // Difference between the histograms.

  c1->cd(2);

  TH1F* dif = h->Clone();

  dif->Add(h,h1,1.,-1.);

  dif->SetTitle("Enorm Difference");
  dif->SetFillColor(kRed);
  dif->SetLineColor(kRed);
  dif->SetLineWidth(1);
  dif->SetFillStyle(1111);
  dif->Draw();
}
