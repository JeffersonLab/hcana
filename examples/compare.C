{
  TFile* f = new TFile("hodtest.root");
  
  TCanvas *c1 = new TCanvas("c1", "Shower Hit Maps", 1000, 1200); 
  c1->Divide(2, 3);
  
  TH1F *h1[6];
  TH1F* h[6];
  
  for(int l = 0; l < 6; l++){
    h1[l] = new TH1F("","",13,0.4,13.4);
  }
  Double_t mean = 0.;
  Double_t hits;
  Double_t raw[13] = {1, 2,  3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  Double_t ratio[77];
  Double_t err[77];


//with peds and scalers
//   double v[6][13] = { 
//     {679, 2016, 2439, 2446, 2369, 2179, 2045, 1953, 1723, 1618, 1465, 1241, 1061},  //hcalapos
//     {0,   2125, 2521, 2420, 2364, 2006, 1935, 1911, 1769, 1536, 1450, 1242, 1090},  //hcalaneg
//     {803, 2069, 2446, 2422, 2188, 1986, 2043, 1881, 1736, 1630, 1455, 1130, 1086},  //hcalbpos
//     {788, 2034, 2372, 2217, 2141, 2002, 1956, 1938, 1700, 1605, 1442, 1219,  952},  //hcalbneg
//     {915, 1964, 2276, 2066, 2122, 1880, 1969, 1854, 1576, 1500, 1403, 1189,  875},  //hcalcpos
//     {7187, 1748, 1890, 1852, 1786, 1573, 1525, 1510, 1376, 1432, 1191, 1033, 809}   //hcaldpos
//   };

  double v[6][13] = { 
    {747, 2212, 2688, 2698, 2602, 2400, 2228, 2131, 1910, 1763, 1605, 1382, 1179},  //hcalapos
    {0,   2334, 2784, 2673, 2599, 2215, 2107, 2079, 1958, 1672, 1584, 1384, 1211},  //hcalaneg
    {882, 2285, 2709, 2650, 2405, 2184, 2230, 2062, 1921, 1789, 1589, 1252, 1204},  //hcalbpos
    {866, 2244, 2632, 2430, 2354, 2207, 2127, 2127, 1888, 1757, 1578, 1345, 1054},  //hcalbneg
    {1006, 2171, 2529, 2262, 2302, 2070, 2149, 2024, 1732, 1634, 1543, 1323, 957},  //hcalcpos
    {7913, 1918, 2084, 2027, 1947, 1739, 1668, 1646, 1521, 1572, 1302, 1136, 887}   //hcaldpos
  };


  // //Without thresholds
  //	double v[6][13] = {
  // 	{679, 2296, 2439, 2446, 2369, 2179, 2045, 1953, 1723, 1618, 1465, 1241, 1061};
  //      {0, 2125, 2521, 2420, 2364, 2006, 1935, 1911, 1769, 1536, 1450, 1242, 1090};
  // 	{803, 2069, 2446, 2422, 2188, 1986, 2043, 1881, 1736, 1630, 1455, 1130, 1086};
  //      {788, 2034, 2372, 2217, 2141, 2002, 1956, 1938, 1700, 1605, 1442, 1219, 952};
  //      {915, 1964, 2276, 2066, 2122, 1880, 1969, 1854, 1576, 1500, 1403, 1189, 875};
  // 	{12633, 1748, 1890, 1852, 1786, 1573, 1525, 1510, 1376, 1432, 1191, 1033, 809};
  
  h[0] = chposadc1;
  h[1] = chnegadc1;
  h[2] = chposadc2;
  h[3] = chnegadc2;
  h[4] = chposadc3;
  h[5] = chposadc4;


  for (int j = 0; j < 6; j++){
    c1->cd(j+1);
    h[j]->SetFillColor(kGreen);
    h[j]->SetFillStyle(3345);
    h[j]->Draw();
    
    for(int k = 0; k < 13; k++){
      for(int i = 0; i < v[j][k]; i++){
	hits = raw[k]-0.1;
	h1[j]->Fill(hits);
      }
    }
    h1[j]->SetFillColor(kBlue);
    h1[j]->SetFillStyle(3354);
    h1[j]->Draw("same");
  }

for (int i = 1; i < 14; i++){
cout << "hitsC = " << h[0].GetBinContent(i) << endl;
}


/*
int n = 0;
Double_t errC;
Double_t errE;
Double_t hitsC;
Double_t hitsE;
Double_t hitsCmean = 1876.81;
Double_t hitsEmean = 1783.7;
Double_t sum1 = 0.; //chislitel
Double_t sum2 = 0.; 
Double_t sum3 = 0.; 
Double_t chan[77];
Double_t r;

  for (Int_t l = 0; l < 6; l++){ 
//    cout << "ADC" << l+1 << ": " << endl;
    for (Int_t i = 1; i < h[l].GetNbinsX() +1; i++){
	hitsC = h[l].GetBinContent(i);
	hitsE = h1[l].GetBinContent(i);
      if (hitsC > 0 && hitsE > 0){
	errC = 1./sqrt(hitsC);
        errE = 1./sqrt(hitsE);
	ratio[n] = hitsC/hitsE;
//cout << "errC = " << errC << ", errE = " << errE << "corr = " <<  errC*errE << endl;
	err[n] = sqrt((errC**2 + errE**2 - 2*errC*errE))*ratio[n];
	mean = mean + ratio[n];




sum1 = sum1 + (hitsC - hitsCmean)*(hitsE - hitsEmean);
sum2 = sum2 + (hitsC - hitsCmean)**2;
sum3 = sum3 + (hitsE - hitsEmean)**2;
//	hitsCmean = hitsCmean + hitsC;
//	hitsEmean = hitsEmean + hitsE;
	chan[n] = n*1.;
	n++;
	}
	//cout << "Hits ratio C++/ENGINE = " << err[l][i] << endl;
    }
  }
mean = mean/n;
//hitsCmean = hitsCmean/n;
//hitsEmean = hitsEmean/n;
r = sum1/sqrt(sum2*sum3);
cout << "r = " << r << endl;

c1->Update();
TCanvas *c2 = new TCanvas("c2", "RATIO", 1200, 800); 
c2->cd();
// create the TGraphErrors and draw it
gr = new TGraphErrors(n,chan,ratio,0,err);
//gr->SetLineColor(8);
gr->SetMarkerStyle(20);
 TAxis *xaxis = gr->GetXaxis();
 TAxis *yaxis = gr->GetYaxis();
 //xaxis->SetLimits(10, 60);
 xaxis->SetTitle("Channel");
 yaxis->SetTitle("Hits ratio (C++/Engine)");
yaxis->SetTitleOffset(1.2);

gr->Draw("AP");

  TF1 *f1 = new TF1("f1","1.0527",0.,77.);
   f1->Draw("same");

c2->Update();
*/
} 
