#define sos_class_cxx
#define shms_class_cxx

#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <iostream>
#include <TMath.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TCanvas.h>
#include "sos_class.h"
#include "shms_class.h"

using namespace std;

void comp_sos_shms() {

  // Compare energy depositions in sos and shms calorimeters.

  // sos tree

  sos_class* sos_tree = new sos_class(0);

  Long64_t sos_nentries = sos_tree->fChain->GetEntriesFast();
  cout << "sos_nentries = " << sos_nentries << endl;

  // shms tree

  shms_class* shms_tree = new shms_class(0);

  Long64_t shms_nentries = shms_tree->fChain->GetEntriesFast();
  cout << "shms_nentries = " << shms_nentries << endl;

  // Histograms.

  TH1F* h_sos_1pr_eplane = new TH1F("h_sos_1pr_e", "1pr e in sos", 100, 0., 1.);
  TH1F* h_sos_1pr_eplane_pos= (TH1F*)h_sos_1pr_eplane->Clone("h_sos_1pr_e_pos");
  TH1F* h_sos_1pr_eplane_neg= (TH1F*)h_sos_1pr_eplane->Clone("h_sos_1pr_e_neg");
  TH1F* h_sos_2ta_eplane_pos= (TH1F*)h_sos_1pr_eplane->Clone("h_sos_2ta_e_pos");
  TH1F* h_sos_etrack        = (TH1F*)h_sos_1pr_eplane->Clone("h_sos_etrack");

  h_sos_1pr_eplane_pos->SetTitle("1pr epos in sos");
  h_sos_1pr_eplane_neg->SetTitle("1pr eneg in sos");
  h_sos_2ta_eplane_pos->SetTitle("2pr epos in sos");
  h_sos_etrack->SetTitle("sos track energy");

  h_sos_1pr_eplane->SetLineColor(kRed);
  h_sos_1pr_eplane_pos->SetLineColor(kRed);
  h_sos_1pr_eplane_neg->SetLineColor(kRed);
  h_sos_2ta_eplane_pos->SetLineColor(kRed);
  h_sos_etrack->SetLineColor(kRed);

  TH1F* h_shms_1pr_eplane = (TH1F*) h_sos_1pr_eplane->Clone("h_shms_1pr_e");
  TH1F* h_shms_1pr_eplane_pos = (TH1F*) h_sos_1pr_eplane_pos->Clone(
  "h_shms_1pr_epos");
  TH1F* h_shms_1pr_eplane_neg = (TH1F*) h_sos_1pr_eplane_neg->Clone(
  "h_shms_1pr_eneg");
  TH1F* h_shms_2ta_earray = (TH1F*) h_sos_2ta_eplane_pos->Clone("h_shms_2ta_e");
  TH1F* h_shms_etrack = (TH1F*) h_sos_etrack->Clone("h_shms_etrack");

  h_shms_1pr_eplane->SetTitle("1pr e in shms");
  h_shms_1pr_eplane_pos->SetTitle("1pr epos in shms");
  h_shms_1pr_eplane_neg->SetTitle("1pr neg in shms");
  h_shms_2ta_earray->SetTitle("2ta e in shms");
  h_shms_etrack->SetTitle("track energy in shms");

  h_shms_1pr_eplane->SetLineColor(kBlue);
  h_shms_1pr_eplane_pos->SetLineColor(kBlue);
  h_shms_1pr_eplane_neg->SetLineColor(kBlue);
  h_shms_2ta_earray->SetLineColor(kBlue);
  h_shms_etrack->SetLineColor(kBlue);

  h_sos_1pr_eplane->SetLineWidth(2.);
  h_sos_1pr_eplane_pos->SetLineWidth(2.);
  h_sos_1pr_eplane_neg->SetLineWidth(2.);
  h_sos_2ta_eplane_pos->SetLineWidth(2.);
  h_sos_etrack->SetLineWidth(2.);

  TH1F* h_dif_1pr_eplane = new TH1F("h_dif_1pr_e",
    "Event by event sos - shms 1pr_eplane difference", 200,-1.e-4,1.e-4);
  TH1F* h_dif_1pr_eplane_pos = (TH1F*)h_dif_1pr_eplane->Clone("h_dif_1pr_epos");
  TH1F* h_dif_1pr_eplane_neg = (TH1F*)h_dif_1pr_eplane->Clone("h_dif_1pr_eneg");
  TH1F* h_dif_2ta_eplane = (TH1F*) h_dif_1pr_eplane->Clone("h_dif_2ta_e");
  TH1F* h_dif_etrack = (TH1F*) h_dif_1pr_eplane->Clone("h_dif_etrack");

  h_dif_1pr_eplane_pos->SetTitle
  ("Event by event sos - shms 1pr_eplane_pos difference");
  h_dif_1pr_eplane_neg->SetTitle
  ("Event by event sos - shms 1pr_eplane_neg difference");
  h_dif_2ta_eplane->SetTitle
  ("Event by event sos (pos) - shms 2ta_earray difference");
  h_dif_etrack->SetTitle
  ("Event by event sos - shms track energy difference");

  // Loop over entries.

  Long64_t nentries = TMath::Min(sos_nentries, shms_nentries);
  cout << "nentries = " << nentries << endl;

  Long64_t jentry_sos = 0;
  Long64_t jentry_shms = 0;
  for (Long64_t jentry=0; jentry<nentries; jentry++) {

    sos_tree->fChain->GetEntry(jentry_sos++);
    shms_tree->fChain->GetEntry(jentry_shms++);

    while (sos_tree->fEvtHdr_fEvtNum > shms_tree->fEvtHdr_fEvtNum &&
	   jentry_shms < shms_nentries) {
      shms_tree->fChain->GetEntry(jentry_shms++);
      cout<< sos_tree->fEvtHdr_fEvtNum << " " << shms_tree->fEvtHdr_fEvtNum
	  << endl;
    }

    while (sos_tree->fEvtHdr_fEvtNum < shms_tree->fEvtHdr_fEvtNum &&
	   jentry_sos < sos_nentries) {
      sos_tree->fChain->GetEntry(jentry_sos++);
      cout<< sos_tree->fEvtHdr_fEvtNum << " " << shms_tree->fEvtHdr_fEvtNum
	  << endl;
    }

    //    cout << "sos ev.type=" << sos_tree->fEvtHdr_fEvtType
    //	 << "  shms ev.type=" << shms_tree->fEvtHdr_fEvtType << endl;
    //    if (sos_tree->H_gold_index != 0 || engine_tree->hstrk_in != 0)
    //    if (engine_tree->hstrk_in != 0)
    //      cout << " hcana trk.ind=" << hcana_tree->H_gold_index
    //	   << " engine trk.ind=" <<  engine_tree->hstrk_in << endl;

    //Select sos triggers.
    if (sos_tree->fEvtHdr_fEvtType==2 && shms_tree->fEvtHdr_fEvtType==2) {

      h_sos_1pr_eplane->Fill(sos_tree->S_cal_1pr_eplane, 1.);
      h_sos_1pr_eplane_pos->Fill(sos_tree->S_cal_1pr_eplane_pos, 1.);
      h_sos_1pr_eplane_neg->Fill(sos_tree->S_cal_1pr_eplane_neg, 1.);
      h_sos_2ta_eplane_pos->Fill(sos_tree->S_cal_2ta_eplane_pos, 1.);
      h_sos_etrack->Fill(sos_tree->S_cal_etrack, 1.);

      //cout << sos_tree->S_cal_1pr_eplane << " " << sos_tree->g_evtyp << " " 
      //<< sos_tree->fEvtHdr_fEvtType << endl; 

      h_shms_1pr_eplane->Fill(shms_tree->S_cal_1pr_eplane, 1.);
      h_shms_1pr_eplane_pos->Fill(shms_tree->S_cal_1pr_eplane_pos, 1.);
      h_shms_1pr_eplane_neg->Fill(shms_tree->S_cal_1pr_eplane_neg, 1.);
      h_shms_2ta_earray->Fill(shms_tree->S_cal_2ta_earray, 1.);
      h_shms_etrack->Fill(shms_tree->S_cal_etrack, 1.);

      // cout << shms_tree->shms_1pr_eplane << " " << shms_tree->ev_type << " " 
      //	   << shms_tree->fEvtHdr_fEvtNum << endl; 

      h_dif_1pr_eplane->Fill(sos_tree->S_cal_1pr_eplane -
			     shms_tree->S_cal_1pr_eplane, 1.);
      h_dif_1pr_eplane_pos->Fill(sos_tree->S_cal_1pr_eplane_pos -
				 shms_tree->S_cal_1pr_eplane_pos, 1.);
      h_dif_1pr_eplane_neg->Fill(sos_tree->S_cal_1pr_eplane_neg -
				 shms_tree->S_cal_1pr_eplane_neg, 1.);
      h_dif_2ta_eplane->Fill(sos_tree->S_cal_2ta_eplane_pos -
      			     shms_tree->S_cal_2ta_earray, 1.);
      h_dif_etrack->Fill(sos_tree->S_cal_etrack -
			 shms_tree->S_cal_etrack, 1.);

      //Debug print out.
      /*
      if (TMath::Abs(engine_tree->hstrk_et - hcana_tree->H_gold_e) > 0.05 ) {

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
      */
    }

    if (jentry%1000 == 0) cout << jentry << "\r" << flush;
  }   //over entries

  // Plot.

  TCanvas* c1 = new TCanvas("c1","S_cal_1pr_eplane",600,750);

  c1->Divide(1,2);
  c1->cd(1);
  h_sos_1pr_eplane->Draw();
  h_shms_1pr_eplane->Draw("same");

  TLegend* leg1 = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg1->AddEntry(h_sos_1pr_eplane,"sos","l");
  leg1->AddEntry(h_shms_1pr_eplane,"shms","l");
  leg1->Draw();

  c1->cd(2);
  h_dif_1pr_eplane->Draw();

  TCanvas* c2 = new TCanvas("c2","S_cal_2ta_eplane",600,750);

  c2->Divide(1,2);
  c2->cd(1);
  h_sos_2ta_eplane_pos->Draw();
  h_shms_2ta_earray->Draw("same");

  TLegend* leg2 = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg2->AddEntry(h_sos_2ta_eplane_pos,"sos","l");
  leg2->AddEntry(h_shms_2ta_earray,"shms","l");
  leg2->Draw();

  c2->cd(2);
  h_dif_2ta_eplane->Draw();

  TCanvas* c3 = new TCanvas("c3","S_cal_1pr_eplane_pos",600,750);

  c3->Divide(1,2);
  c3->cd(1);
  h_sos_1pr_eplane_pos->Draw();
  h_shms_1pr_eplane_pos->Draw("same");

  TLegend* leg3 = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg3->AddEntry(h_sos_1pr_eplane_pos,"sos","l");
  leg3->AddEntry(h_shms_1pr_eplane_pos,"shms","l");
  leg3->Draw();

  c3->cd(2);
  h_dif_1pr_eplane_pos->Draw();

  TCanvas* c4 = new TCanvas("c4","S_cal_1pr_eplane_neg",600,750);

  c4->Divide(1,2);
  c4->cd(1);
  h_sos_1pr_eplane_neg->Draw();
  h_shms_1pr_eplane_neg->Draw("same");

  TLegend* leg4 = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg4->AddEntry(h_sos_1pr_eplane_neg,"sos","l");
  leg4->AddEntry(h_shms_1pr_eplane_neg,"shms","l");
  leg4->Draw();

  c4->cd(2);
  h_dif_1pr_eplane_neg->Draw();

  TCanvas* c5 = new TCanvas("c5","S_cal_etrack",600,750);

  c5->Divide(1,2);
  c5->cd(1);
  h_sos_etrack->Draw();
  h_shms_etrack->Draw("same");

  TLegend* leg5 = new TLegend(0.25, 0.70, 0.40, 0.82);
  leg5->AddEntry(h_sos_etrack,"sos","l");
  leg5->AddEntry(h_shms_etrack,"shms","l");
  leg5->Draw();

  c5->cd(2);
  h_dif_etrack->Draw();

}
