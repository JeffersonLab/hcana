void comphh(Int_t run, TH1F* h1, TH1F* h2)
{
  TFile* f = new TFile(Form("hodtest_%d.root",run));
  cout << "hcana root file " << Form("hodtest_%d.root",run) << endl;

  TCanvas *c1 = new TCanvas("c1", "h1 vs h2", 1000, 667);

  //  gPad->SetLogy();

  h1->SetFillColor(kGreen);
  h1->SetLineColor(kGreen);
  h1->Draw();

  h2->SetLineColor(kBlue);
  h2->SetFillStyle(0);
  h2->SetLineWidth(2);
  h2->Draw("same");

  TLatex l;
  l.SetTextSize(0.04);
  Float_t maxy = h1->GetBinContent(h1->GetMaximumBin());
  Float_t xmin = h1->GetXaxis()->GetXmin();
  Float_t xmax = h1->GetXaxis()->GetXmax();
  Float_t xt = xmin + 0.75*(xmax-xmin);

  l.SetTextColor(kGreen);
  l.DrawLatex(xt,0.65*maxy,"h1");
  l.SetTextColor(kBlue);
  l.DrawLatex(xt,0.75*maxy,"h2");

  // Difference between the histograms.

  TCanvas *c2 = new TCanvas("c2", "Epr differences", 1000, 667);

  TH1F* dif = h2->Clone();

  dif->Add(h2,h1,1.,-1.);

  dif->SetTitle("Difference");
  dif->SetFillColor(kRed);
  dif->SetLineColor(kRed);
  dif->SetLineWidth(1);
  dif->SetFillStyle(1111);
  dif->Draw();
}
