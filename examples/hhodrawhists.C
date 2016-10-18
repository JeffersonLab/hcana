//
// Make a set of histograms containing raw adcs and tdcs for
// each PMT of the HMS hodoscopes
// TDCs are after application of a common offset and
// ADCs are after subtraction of a pedestal
//
{
#define NPLANES 4
#define NSIDES 2
#define NSIGNALS 2
#define MAXBARS 16
  TString SPECTROMETER="H";
  TString DETECTOR="hod";
  TString plane_names[NPLANES]={"1x","1y","2x","2y"};
  Int_t nbars[NPLANES]={16,10,16,10};
  TString sides[NSIDES]={"neg","pos"};
  TString signals[NSIGNALS]={"adc","tdc"};
  Double_t ADC_MIN=0.0;
  Double_t ADC_MAX=4000.0;
  Int_t ADC_NBINS=200;
  Double_t TDC_MIN=0.0;
  Double_t TDC_MAX=4000.0;
  Int_t TDC_NBINS=200;

  Int_t nhits[NPLANES][NSIDES][NSIGNALS];
  Double_t paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];
  Double_t values[NPLANES][NSIDES][NSIGNALS][MAXBARS];

  TH1F* h[NPLANES*NSIDES*NSIGNALS*MAXBARS];

  TFile file("hodtest.root","READ");
  TFile hfile("hhodrawhists.root", "RECREATE");
  file.cd();

  TTree* T = (TTree*) file.Get("T");

  for(Int_t ip=0;ip<NPLANES;ip++){
    for(Int_t iside=0;iside<NSIDES;iside++) {
      for(Int_t isignal=0;isignal<NSIGNALS;isignal++) {

	TString base_name = SPECTROMETER+"."+DETECTOR+"."+
	  plane_names[ip]+"."+sides[iside]+signals[isignal];
	TString ndata_name = "Ndata."+base_name+"pad";
	TString padlist_name = base_name+"pad";
	TString vallist_name = base_name+"val";
	
	// Set branch addresses

	T->SetBranchAddress(ndata_name, &nhits[ip][iside][isignal]);
	T->SetBranchAddress(padlist_name, &paddles[ip][iside][isignal][0]);
	T->SetBranchAddress(vallist_name, &values[ip][iside][isignal][0]);

	// Create histograms
	// ADC and TDC histogram for each 
	Int_t nbins,hmin,hmax;
	if(signals[isignal]=="adc") {
	  nbins = ADC_NBINS;
	  hmin = ADC_MIN;
	  hmax = ADC_MAX;
	} else {
	  nbins = TDC_NBINS;
	  hmin = TDC_MIN;
	  hmax = TDC_MAX;
	}
	hfile.cd();
	for(Int_t ibar=0;ibar<nbars[ip];ibar++) {
	  Int_t hindex = ip*NSIDES*NSIGNALS*MAXBARS
	    +iside*NSIGNALS*MAXBARS+isignal*MAXBARS+ibar;
	  TString ibarname=Form("%d",ibar+1);
	  TString title= "hS"+plane_names[ip]+ibarname+sides[iside]
	    +" "+signals[isignal];
	  TString name="h"; name += Form("%d",hindex);
	  //	  cout << name << " " << title << " " << nbins << " " << hmin << " "
	  //	       << hmax << endl;
	  h[hindex]=new TH1F(name,title,nbins, hmin, hmax);
	}
	file.cd();
      }
    }
  }

  // Loop over the events, filling the histograms
  for(Int_t iev=0, N=T->GetEntries(); iev < N; iev++) {
    T->GetEntry(iev);

    for(Int_t ip=0;ip<NPLANES;ip++){
      for(Int_t iside=0;iside<NSIDES;iside++) {
	for(Int_t isignal=0;isignal<NSIGNALS;isignal++) {

	  Int_t hindex_base = ip*NSIDES*NSIGNALS*MAXBARS
	    +iside*NSIGNALS*MAXBARS+isignal*MAXBARS;

	  for(Int_t ihit=0;ihit<nhits[ip][iside][isignal];ihit++) {
	    Int_t ibar=TMath::Nint(paddles[ip][iside][isignal][ihit])-1;
	    Double_t val = values[ip][iside][isignal][ihit];
	    Int_t hindex = hindex_base + ibar;
	    
	    //	    cout << ip << " " << iside << " " << isignal << " " << ihit
	    //		 << " " << ibar << " " << hindex << endl;
	    h[hindex]->Fill(val);
	  }
	}
      }
    }
    // Display or save the histograms
  }
  hfile.cd();
  hfile.Write();
}
