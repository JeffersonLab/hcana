//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Sep 29 17:54:32 2014 by ROOT version 5.34/06
// from TTree h9010/
// found on file: hms52949_rz.root
//////////////////////////////////////////////////////////

#ifndef engine_class_h
#define engine_class_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class engine_class {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         hcer_npe;
   Float_t         hsp;
   Float_t         hse;
   Float_t         charge;
   Float_t         hsdelta;
   Float_t         hstheta;
   Float_t         hsphi;
   Float_t         w;
   Float_t         hszbeam;
   Float_t         hsdedx1;
   Float_t         hsbeta;
   Float_t         hsshtrk;
   Float_t         hsprtrk;
   Float_t         hsxfp;
   Float_t         hsyfp;
   Float_t         hsxpfp;
   Float_t         hsypfp;
   Float_t         hsytar;
   Float_t         hsxptar;
   Float_t         hsyptar;
   Float_t         hstart;
   Float_t         eventid;
   Float_t         ev_type;
   Float_t         gfrx_raw;
   Float_t         gfry_raw;
   Float_t         gbeam_x;
   Float_t         gbeam_y;
   Float_t         bpma_x;
   Float_t         bpma_y;
   Float_t         bpmb_x;
   Float_t         bpmb_y;
   Float_t         bpmc_x;
   Float_t         bpmc_y;
   Float_t         hseloss;
   Float_t         hntracks;
   Float_t         hcal_et;
   Float_t         hgoodsc;
   Float_t         hcal_e1;
   Float_t         hcal_e2;
   Float_t         hcal_e3;
   Float_t         hcal_e4;
   Float_t         hstrk_et;
   Float_t         hstrk_in;

   // List of branches
   TBranch        *b_hcer_npe;   //!
   TBranch        *b_hsp;   //!
   TBranch        *b_hse;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_hsdelta;   //!
   TBranch        *b_hstheta;   //!
   TBranch        *b_hsphi;   //!
   TBranch        *b_w;   //!
   TBranch        *b_hszbeam;   //!
   TBranch        *b_hsdedx1;   //!
   TBranch        *b_hsbeta;   //!
   TBranch        *b_hsshtrk;   //!
   TBranch        *b_hsprtrk;   //!
   TBranch        *b_hsxfp;   //!
   TBranch        *b_hsyfp;   //!
   TBranch        *b_hsxpfp;   //!
   TBranch        *b_hsypfp;   //!
   TBranch        *b_hsytar;   //!
   TBranch        *b_hsxptar;   //!
   TBranch        *b_hsyptar;   //!
   TBranch        *b_hstart;   //!
   TBranch        *b_eventid;   //!
   TBranch        *b_ev_type;   //!
   TBranch        *b_gfrx_raw;   //!
   TBranch        *b_gfry_raw;   //!
   TBranch        *b_gbeam_x;   //!
   TBranch        *b_gbeam_y;   //!
   TBranch        *b_bpma_x;   //!
   TBranch        *b_bpma_y;   //!
   TBranch        *b_bpmb_x;   //!
   TBranch        *b_bpmb_y;   //!
   TBranch        *b_bpmc_x;   //!
   TBranch        *b_bpmc_y;   //!
   TBranch        *b_hseloss;   //!
   TBranch        *b_hntracks;   //!
   TBranch        *b_hcal_et;   //!
   TBranch        *b_hgoodsc;   //!
   TBranch        *b_hcal_e1;   //!
   TBranch        *b_hcal_e2;   //!
   TBranch        *b_hcal_e3;   //!
   TBranch        *b_hcal_e4;   //!
   TBranch        *b_hstrk_et;   //!
   TBranch        *b_hstrk_in;   //!

   engine_class(TTree *tree=0);
   virtual ~engine_class();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef engine_class_cxx
engine_class::engine_class(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("hms52949_rz.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("hms52949_rz.root");
      }
      f->GetObject("h9010",tree);

   }
   Init(tree);
}

engine_class::~engine_class()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t engine_class::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t engine_class::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void engine_class::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("hcer_npe", &hcer_npe, &b_hcer_npe);
   fChain->SetBranchAddress("hsp", &hsp, &b_hsp);
   fChain->SetBranchAddress("hse", &hse, &b_hse);
   fChain->SetBranchAddress("charge", &charge, &b_charge);
   fChain->SetBranchAddress("hsdelta", &hsdelta, &b_hsdelta);
   fChain->SetBranchAddress("hstheta", &hstheta, &b_hstheta);
   fChain->SetBranchAddress("hsphi", &hsphi, &b_hsphi);
   fChain->SetBranchAddress("w", &w, &b_w);
   fChain->SetBranchAddress("hszbeam", &hszbeam, &b_hszbeam);
   fChain->SetBranchAddress("hsdedx1", &hsdedx1, &b_hsdedx1);
   fChain->SetBranchAddress("hsbeta", &hsbeta, &b_hsbeta);
   fChain->SetBranchAddress("hsshtrk", &hsshtrk, &b_hsshtrk);
   fChain->SetBranchAddress("hsprtrk", &hsprtrk, &b_hsprtrk);
   fChain->SetBranchAddress("hsxfp", &hsxfp, &b_hsxfp);
   fChain->SetBranchAddress("hsyfp", &hsyfp, &b_hsyfp);
   fChain->SetBranchAddress("hsxpfp", &hsxpfp, &b_hsxpfp);
   fChain->SetBranchAddress("hsypfp", &hsypfp, &b_hsypfp);
   fChain->SetBranchAddress("hsytar", &hsytar, &b_hsytar);
   fChain->SetBranchAddress("hsxptar", &hsxptar, &b_hsxptar);
   fChain->SetBranchAddress("hsyptar", &hsyptar, &b_hsyptar);
   fChain->SetBranchAddress("hstart", &hstart, &b_hstart);
   fChain->SetBranchAddress("eventid", &eventid, &b_eventid);
   fChain->SetBranchAddress("ev_type", &ev_type, &b_ev_type);
   fChain->SetBranchAddress("gfrx_raw", &gfrx_raw, &b_gfrx_raw);
   fChain->SetBranchAddress("gfry_raw", &gfry_raw, &b_gfry_raw);
   fChain->SetBranchAddress("gbeam_x", &gbeam_x, &b_gbeam_x);
   fChain->SetBranchAddress("gbeam_y", &gbeam_y, &b_gbeam_y);
   fChain->SetBranchAddress("bpma_x", &bpma_x, &b_bpma_x);
   fChain->SetBranchAddress("bpma_y", &bpma_y, &b_bpma_y);
   fChain->SetBranchAddress("bpmb_x", &bpmb_x, &b_bpmb_x);
   fChain->SetBranchAddress("bpmb_y", &bpmb_y, &b_bpmb_y);
   fChain->SetBranchAddress("bpmc_x", &bpmc_x, &b_bpmc_x);
   fChain->SetBranchAddress("bpmc_y", &bpmc_y, &b_bpmc_y);
   fChain->SetBranchAddress("hseloss", &hseloss, &b_hseloss);
   fChain->SetBranchAddress("hntracks", &hntracks, &b_hntracks);
   fChain->SetBranchAddress("hcal_et", &hcal_et, &b_hcal_et);
   fChain->SetBranchAddress("hgoodsc", &hgoodsc, &b_hgoodsc);
   fChain->SetBranchAddress("hcal_e1", &hcal_e1, &b_hcal_e1);
   fChain->SetBranchAddress("hcal_e2", &hcal_e2, &b_hcal_e2);
   fChain->SetBranchAddress("hcal_e3", &hcal_e3, &b_hcal_e3);
   fChain->SetBranchAddress("hcal_e4", &hcal_e4, &b_hcal_e4);
   fChain->SetBranchAddress("hstrk_et", &hstrk_et, &b_hstrk_et);
   fChain->SetBranchAddress("hstrk_in", &hstrk_in, &b_hstrk_in);
   Notify();
}

Bool_t engine_class::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void engine_class::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t engine_class::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef engine_class_cxx
