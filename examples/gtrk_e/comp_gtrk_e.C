#define hcana_class_cxx
#define engine_class_cxx

#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <iostream>
#include <TMath.h>
#include <TLegend.h>
#include "hcana_class.h"
#include "engine_class.h"

using namespace std;

void comp_gtrk_e() {

  // Compare Ecalo of the golden track from Engine and hcana.

  // hcana tree

  hcana_class* hcana_tree = new hcana_class(0);

  Long64_t hcana_nentries = hcana_tree->fChain->GetEntriesFast();
  cout << "hcana_nentries = " << hcana_nentries << endl;

  //  hcana_tree->fChain->SetBranchStatus("*",0);      // disable all branches
  //  hcana_tree->fChain->SetBranchStatus("*H.gold.e",1); // activate branchname
  //  hcana_tree->fChain->SetBranchStatus("*H.gold.p",1);
  //  hcana_tree->fChain->SetBranchStatus("*H.gold.index",1);
  //  hcana_tree->fChain->SetBranchStatus("*H.tr.n",1);
  //  hcana_tree->fChain->SetBranchStatus("*g.evtyp",1);
  //  hcana_tree->fChain->SetBranchStatus("*fEvtHdr.fEvtType",1);
  //  hcana_tree->fChain->SetBranchStatus("*fEvtHdr.fEvtNum",1);

  // Engine tree

  engine_class* engine_tree = new engine_class(0);

  Long64_t engine_nentries = engine_tree->fChain->GetEntriesFast();
  cout << "engine_nentries = " << engine_nentries << endl;

  //  engine_tree->fChain->SetBranchStatus("*",0);
  //  engine_tree->fChain->SetBranchStatus("*hstrk_et",1);
  //  engine_tree->fChain->SetBranchStatus("*hsp",1);
  //  engine_tree->fChain->SetBranchStatus("*ev_type",1);
  //  engine_tree->fChain->SetBranchStatus("*eventid",1);
  //  engine_tree->fChain->SetBranchStatus("*hstrk_in",1);
  //  engine_tree->fChain->SetBranchStatus("*hntracks",1);

  // Histograms.

  TH1F* he_hcana = new TH1F("e_hcana", "Golden track  Ecalo",
				100, 0.011, 1.8);
  he_hcana->SetLineColor(kRed);

  TH1F* he_engine = (TH1F*) he_hcana->Clone("e_engine");
  he_engine->SetLineColor(kBlue);

  TH1F* ee_diff = new TH1F("ee_diff",
		  "Event by event hcana - Engine Ecalo difference", 200,-1.,1.);
  TH1F* pp_diff = new TH1F("pp_diff","Event by event P difference",
			   200,-1.,1.);
  TH2F* pvse_diff = new TH2F("pvse_diff","Event by event P vs Ecalo difference",
			     200,-1.,1., 200,-100.,100.);
  pvse_diff->SetMarkerStyle(3);
  TH2F* evse = new TH2F("evse","Event by event hcana vs Engine Ecalo",
			210,-0.1,2., 210,-0.1,2.);
  evse->SetMarkerStyle(3);

  // Loop over entries.

  Long64_t nentries = TMath::Min(hcana_nentries, engine_nentries);
  cout << "nentries = " << nentries << endl;

  Long64_t jentry_hcana = 0;
  Long64_t jentry_engine = 0;
  for (Long64_t jentry=0; jentry<nentries; jentry++) {

    hcana_tree->fChain->GetEntry(jentry_hcana++);
    engine_tree->fChain->GetEntry(jentry_engine++);

    while (hcana_tree->fEvtHdr_fEvtNum > engine_tree->eventid &&
	   jentry_engine < engine_nentries) {
      engine_tree->fChain->GetEntry(jentry_engine++);
      //cout<< hcana_tree->fEvtHdr_fEvtNum << " " << engine_tree->eventid << endl;
    }

    while (hcana_tree->fEvtHdr_fEvtNum < engine_tree->eventid &&
	   jentry_hcana < hcana_nentries) {
      hcana_tree->fChain->GetEntry(jentry_hcana++);
      //cout<< hcana_tree->fEvtHdr_fEvtNum << " " << engine_tree->eventid << endl;
    }

    if (hcana_tree->fEvtHdr_fEvtType==1 && engine_tree->ev_type==1
	&& hcana_tree->H_gold_index>-1 && engine_tree->hstrk_in>0
	&& hcana_tree->H_gold_index == engine_tree->hstrk_in - 1) {

      he_hcana->Fill(hcana_tree->H_gold_e, 1.);
      //cout << hcana_tree->H_gold_e << " " << hcana_tree->g_evtyp << " " 
      //	 << hcana_tree->fEvtHdr_fEvtType << endl; 

      he_engine->Fill(engine_tree->hstrk_et, 1.);
      //     cout << engine_tree->hsshtrk << " " << engine_tree->ev_type << " " 
      //	   << engine_tree->eventid << endl; 

      ee_diff->Fill(hcana_tree->H_gold_e - engine_tree->hstrk_et, 1.);
      pp_diff->Fill(hcana_tree->H_gold_p - engine_tree->hsp, 1.);
      pvse_diff->Fill(hcana_tree->H_gold_e - engine_tree->hstrk_et,
		      hcana_tree->H_gold_p - engine_tree->hsp, 1.);
      evse->Fill(engine_tree->hstrk_et, hcana_tree->H_gold_e, 1.);

      //Debug print out.

      //      if (hcana_tree->H_gold_e>0.6 && hcana_tree->H_gold_e<0.95 &&
      //      	  engine_tree->hstrk_et<0.1) {
	//	if (hcana_tree->H_gold_e>0.6 && hcana_tree->H_gold_e<0.95 &&
      //  TMath::Abs(engine_tree->hstrk_et - hcana_tree->H_gold_e) < 0.0001 ) {
      //      if (engine_tree->hstrk_et>0.6 && engine_tree->hstrk_et<0.95 &&
      //      	  hcana_tree->H_gold_e<0.1) {
      if (TMath::Abs(engine_tree->hstrk_et - hcana_tree->H_gold_e) < 0.65 &&
	  TMath::Abs(engine_tree->hstrk_et - hcana_tree->H_gold_e) > 0.05 ) {

	cout << "===========================================================\n";
	cout << "Engine event " << engine_tree->eventid
	     << ": hstrk_et = " << engine_tree->hstrk_et
	     << ", hstrk_index = " << engine_tree->hstrk_in
	     << ", hntracks = " << engine_tree->hntracks << endl;
	cout << "  hsxfp = " << engine_tree->hsxfp
	     << ", hsyfp = " << engine_tree->hsyfp << endl;
	cout << "  hsxpfp= " << engine_tree->hsxpfp
	     << ", hsypfp= " << engine_tree->hsypfp << endl;
	cout << "  hsp   = " << engine_tree->hsp << endl;
	cout << "  At calorimeter:"
	     << "  x = " << engine_tree->hsxfp+338.69*engine_tree->hsxpfp
	     << "  y = " << engine_tree->hsyfp+338.69*engine_tree->hsypfp
	     << endl;
	cout << "-----------------------------------------------------------\n";
	cout << "hcana  event " << hcana_tree->fEvtHdr_fEvtNum
	     << ": H_gold_e = " << hcana_tree->H_gold_e
	     << "  H_gold_index = " << hcana_tree->H_gold_index
	     << "  H_tr_n = " << hcana_tree->H_tr_n << endl;
	cout << "  H_gold_fp_x = " << hcana_tree->H_gold_fp_x
	     << "  H_gold_fp_y = " << hcana_tree->H_gold_fp_y << endl;
	cout << "  H_gold_fp_th= " << hcana_tree->H_gold_fp_th
	     << "  H_gold_fp_ph= " << hcana_tree->H_gold_fp_ph << endl;
	cout << "  H_gold_p = " << hcana_tree->H_gold_p << endl;
	cout << "  At calorimeter:"
	  << "  x = " << hcana_tree->H_gold_fp_x+338.69*hcana_tree->H_gold_fp_th
	  << "  y = " << hcana_tree->H_gold_fp_y+338.69*hcana_tree->H_gold_fp_ph
	  << endl;

      }

    }

  }   //over entries

  // Plot.

  TCanvas* c1 = new TCanvas("c1","Golden Track E from hcana",600,750);

  c1->Divide(1,2);
  c1->cd(1);
  he_hcana->Draw();
  he_engine->Draw("same");

  TLegend* leg = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg->AddEntry(he_hcana,"hcana","l");
  leg->AddEntry(he_engine,"Engine","l");
  leg->Draw();

  // Difference between the histograms.

  c1->cd(2);

  TH1F* dif = he_hcana->Clone("difference");
  dif->Reset();
  dif->Add(he_hcana,he_engine,1.,-1.);

  dif->SetTitle("hcana -- Engine difference");
  dif->SetFillColor(kRed);
  dif->SetLineColor(kRed);
  dif->SetLineWidth(1);
  dif->SetFillStyle(1111);
  dif->Draw();

  TCanvas* c2 = new TCanvas("c2","Event by event hcana - Engine Ecalo diff.",
			    600,750);

  c2->Divide(1,2);
  c2->cd(1);
  gPad->SetLogy();
  ee_diff->Draw();
  c2->cd(2);
  //  gPad->SetLogy();
  //  pp_diff->Draw();
  //  pvse_diff->Draw();
  evse->Draw();

}
