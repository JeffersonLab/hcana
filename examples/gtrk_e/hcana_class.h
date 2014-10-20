//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct  7 08:29:09 2014 by ROOT version 5.34/06
// from TTree T/Hall A Analyzer Output DST
// found on file: hodtest_52949.root
//////////////////////////////////////////////////////////

#ifndef hcana_class_h
#define hcana_class_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "./podd/src/THaEvent.h"
#include "./podd/src/THaEvent.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class hcana_class {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Ndata_H_cal_1pr_aneg;
   Double_t        H_cal_1pr_aneg[13];   //[Ndata.H.cal.1pr.aneg]
   Int_t           Ndata_H_cal_1pr_aneg_p;
   Double_t        H_cal_1pr_aneg_p[13];   //[Ndata.H.cal.1pr.aneg_p]
   Int_t           Ndata_H_cal_1pr_apos;
   Double_t        H_cal_1pr_apos[13];   //[Ndata.H.cal.1pr.apos]
   Int_t           Ndata_H_cal_1pr_apos_p;
   Double_t        H_cal_1pr_apos_p[13];   //[Ndata.H.cal.1pr.apos_p]
   Int_t           Ndata_H_cal_1pr_emean;
   Double_t        H_cal_1pr_emean[13];   //[Ndata.H.cal.1pr.emean]
   Int_t           Ndata_H_cal_1pr_eneg;
   Double_t        H_cal_1pr_eneg[13];   //[Ndata.H.cal.1pr.eneg]
   Int_t           Ndata_H_cal_1pr_epos;
   Double_t        H_cal_1pr_epos[13];   //[Ndata.H.cal.1pr.epos]
   Int_t           Ndata_H_cal_1pr_negadchits;
   Double_t        H_cal_1pr_negadchits[9];   //[Ndata.H.cal.1pr.negadchits]
   Int_t           Ndata_H_cal_1pr_posadchits;
   Double_t        H_cal_1pr_posadchits[8];   //[Ndata.H.cal.1pr.posadchits]
   Int_t           Ndata_H_cal_2ta_aneg;
   Double_t        H_cal_2ta_aneg[13];   //[Ndata.H.cal.2ta.aneg]
   Int_t           Ndata_H_cal_2ta_aneg_p;
   Double_t        H_cal_2ta_aneg_p[13];   //[Ndata.H.cal.2ta.aneg_p]
   Int_t           Ndata_H_cal_2ta_apos;
   Double_t        H_cal_2ta_apos[13];   //[Ndata.H.cal.2ta.apos]
   Int_t           Ndata_H_cal_2ta_apos_p;
   Double_t        H_cal_2ta_apos_p[13];   //[Ndata.H.cal.2ta.apos_p]
   Int_t           Ndata_H_cal_2ta_emean;
   Double_t        H_cal_2ta_emean[13];   //[Ndata.H.cal.2ta.emean]
   Int_t           Ndata_H_cal_2ta_eneg;
   Double_t        H_cal_2ta_eneg[13];   //[Ndata.H.cal.2ta.eneg]
   Int_t           Ndata_H_cal_2ta_epos;
   Double_t        H_cal_2ta_epos[13];   //[Ndata.H.cal.2ta.epos]
   Int_t           Ndata_H_cal_2ta_negadchits;
   Double_t        H_cal_2ta_negadchits[8];   //[Ndata.H.cal.2ta.negadchits]
   Int_t           Ndata_H_cal_2ta_posadchits;
   Double_t        H_cal_2ta_posadchits[7];   //[Ndata.H.cal.2ta.posadchits]
   Int_t           Ndata_H_cal_3ta_aneg;
   Double_t        H_cal_3ta_aneg[13];   //[Ndata.H.cal.3ta.aneg]
   Int_t           Ndata_H_cal_3ta_aneg_p;
   Double_t        H_cal_3ta_aneg_p[13];   //[Ndata.H.cal.3ta.aneg_p]
   Int_t           Ndata_H_cal_3ta_apos;
   Double_t        H_cal_3ta_apos[13];   //[Ndata.H.cal.3ta.apos]
   Int_t           Ndata_H_cal_3ta_apos_p;
   Double_t        H_cal_3ta_apos_p[13];   //[Ndata.H.cal.3ta.apos_p]
   Int_t           Ndata_H_cal_3ta_emean;
   Double_t        H_cal_3ta_emean[13];   //[Ndata.H.cal.3ta.emean]
   Int_t           Ndata_H_cal_3ta_eneg;
   Double_t        H_cal_3ta_eneg[13];   //[Ndata.H.cal.3ta.eneg]
   Int_t           Ndata_H_cal_3ta_epos;
   Double_t        H_cal_3ta_epos[13];   //[Ndata.H.cal.3ta.epos]
   Int_t           Ndata_H_cal_3ta_negadchits;
   Double_t        H_cal_3ta_negadchits[1];   //[Ndata.H.cal.3ta.negadchits]
   Int_t           Ndata_H_cal_3ta_posadchits;
   Double_t        H_cal_3ta_posadchits[6];   //[Ndata.H.cal.3ta.posadchits]
   Int_t           Ndata_H_cal_4ta_aneg;
   Double_t        H_cal_4ta_aneg[13];   //[Ndata.H.cal.4ta.aneg]
   Int_t           Ndata_H_cal_4ta_aneg_p;
   Double_t        H_cal_4ta_aneg_p[13];   //[Ndata.H.cal.4ta.aneg_p]
   Int_t           Ndata_H_cal_4ta_apos;
   Double_t        H_cal_4ta_apos[13];   //[Ndata.H.cal.4ta.apos]
   Int_t           Ndata_H_cal_4ta_apos_p;
   Double_t        H_cal_4ta_apos_p[13];   //[Ndata.H.cal.4ta.apos_p]
   Int_t           Ndata_H_cal_4ta_emean;
   Double_t        H_cal_4ta_emean[13];   //[Ndata.H.cal.4ta.emean]
   Int_t           Ndata_H_cal_4ta_eneg;
   Double_t        H_cal_4ta_eneg[13];   //[Ndata.H.cal.4ta.eneg]
   Int_t           Ndata_H_cal_4ta_epos;
   Double_t        H_cal_4ta_epos[13];   //[Ndata.H.cal.4ta.epos]
   Int_t           Ndata_H_cal_4ta_negadchits;
   Double_t        H_cal_4ta_negadchits[1];   //[Ndata.H.cal.4ta.negadchits]
   Int_t           Ndata_H_cal_4ta_posadchits;
   Double_t        H_cal_4ta_posadchits[5];   //[Ndata.H.cal.4ta.posadchits]
   Int_t           Ndata_H_tr_beta;
   Double_t        H_tr_beta[10];   //[Ndata.H.tr.beta]
   Int_t           Ndata_H_tr_chi2;
   Double_t        H_tr_chi2[10];   //[Ndata.H.tr.chi2]
   Int_t           Ndata_H_tr_d_ph;
   Double_t        H_tr_d_ph[10];   //[Ndata.H.tr.d_ph]
   Int_t           Ndata_H_tr_d_th;
   Double_t        H_tr_d_th[10];   //[Ndata.H.tr.d_th]
   Int_t           Ndata_H_tr_d_x;
   Double_t        H_tr_d_x[10];   //[Ndata.H.tr.d_x]
   Int_t           Ndata_H_tr_d_y;
   Double_t        H_tr_d_y[10];   //[Ndata.H.tr.d_y]
   Int_t           Ndata_H_tr_dbeta;
   Double_t        H_tr_dbeta[10];   //[Ndata.H.tr.dbeta]
   Int_t           Ndata_H_tr_dtime;
   Double_t        H_tr_dtime[10];   //[Ndata.H.tr.dtime]
   Int_t           Ndata_H_tr_flag;
   Double_t        H_tr_flag[10];   //[Ndata.H.tr.flag]
   Int_t           Ndata_H_tr_ndof;
   Double_t        H_tr_ndof[10];   //[Ndata.H.tr.ndof]
   Int_t           Ndata_H_tr_p;
   Double_t        H_tr_p[10];   //[Ndata.H.tr.p]
   Int_t           Ndata_H_tr_pathl;
   Double_t        H_tr_pathl[10];   //[Ndata.H.tr.pathl]
   Int_t           Ndata_H_tr_ph;
   Double_t        H_tr_ph[10];   //[Ndata.H.tr.ph]
   Int_t           Ndata_H_tr_px;
   Double_t        H_tr_px[10];   //[Ndata.H.tr.px]
   Int_t           Ndata_H_tr_py;
   Double_t        H_tr_py[10];   //[Ndata.H.tr.py]
   Int_t           Ndata_H_tr_pz;
   Double_t        H_tr_pz[10];   //[Ndata.H.tr.pz]
   Int_t           Ndata_H_tr_r_ph;
   Double_t        H_tr_r_ph[10];   //[Ndata.H.tr.r_ph]
   Int_t           Ndata_H_tr_r_th;
   Double_t        H_tr_r_th[10];   //[Ndata.H.tr.r_th]
   Int_t           Ndata_H_tr_r_x;
   Double_t        H_tr_r_x[10];   //[Ndata.H.tr.r_x]
   Int_t           Ndata_H_tr_r_y;
   Double_t        H_tr_r_y[10];   //[Ndata.H.tr.r_y]
   Int_t           Ndata_H_tr_tg_dp;
   Double_t        H_tr_tg_dp[10];   //[Ndata.H.tr.tg_dp]
   Int_t           Ndata_H_tr_tg_ph;
   Double_t        H_tr_tg_ph[10];   //[Ndata.H.tr.tg_ph]
   Int_t           Ndata_H_tr_tg_th;
   Double_t        H_tr_tg_th[10];   //[Ndata.H.tr.tg_th]
   Int_t           Ndata_H_tr_tg_y;
   Double_t        H_tr_tg_y[10];   //[Ndata.H.tr.tg_y]
   Int_t           Ndata_H_tr_th;
   Double_t        H_tr_th[10];   //[Ndata.H.tr.th]
   Int_t           Ndata_H_tr_time;
   Double_t        H_tr_time[10];   //[Ndata.H.tr.time]
   Int_t           Ndata_H_tr_vx;
   Double_t        H_tr_vx[10];   //[Ndata.H.tr.vx]
   Int_t           Ndata_H_tr_vy;
   Double_t        H_tr_vy[10];   //[Ndata.H.tr.vy]
   Int_t           Ndata_H_tr_vz;
   Double_t        H_tr_vz[10];   //[Ndata.H.tr.vz]
   Int_t           Ndata_H_tr_x;
   Double_t        H_tr_x[10];   //[Ndata.H.tr.x]
   Int_t           Ndata_H_tr_y;
   Double_t        H_tr_y[10];   //[Ndata.H.tr.y]
   Double_t        H_cal_1pr_eplane;
   Double_t        H_cal_1pr_eplane_neg;
   Double_t        H_cal_1pr_eplane_pos;
   Double_t        H_cal_2ta_eplane;
   Double_t        H_cal_2ta_eplane_neg;
   Double_t        H_cal_2ta_eplane_pos;
   Double_t        H_cal_3ta_eplane;
   Double_t        H_cal_3ta_eplane_neg;
   Double_t        H_cal_3ta_eplane_pos;
   Double_t        H_cal_4ta_eplane;
   Double_t        H_cal_4ta_eplane_neg;
   Double_t        H_cal_4ta_eplane_pos;
   Double_t        H_cal_emax;
   Double_t        H_cal_eprmax;
   Double_t        H_cal_mult;
   Double_t        H_cal_nclust;
   Double_t        H_cal_nhits;
   Double_t        H_cal_ntracks;
   Double_t        H_cal_tre;
   Double_t        H_cal_trecor;
   Double_t        H_cal_treplcor;
   Double_t        H_cal_trepr;
   Double_t        H_cal_treprcor;
   Double_t        H_cal_trx;
   Double_t        H_cal_try;
   Double_t        H_cal_xmax;
   Double_t        H_gold_dp;
   Double_t        H_gold_e;
   Double_t        H_gold_fp_ph;
   Double_t        H_gold_fp_th;
   Double_t        H_gold_fp_x;
   Double_t        H_gold_fp_y;
   Double_t        H_gold_index;
   Double_t        H_gold_ok;
   Double_t        H_gold_p;
   Double_t        H_gold_ph;
   Double_t        H_gold_px;
   Double_t        H_gold_py;
   Double_t        H_gold_pz;
   Double_t        H_gold_th;
   Double_t        H_gold_x;
   Double_t        H_gold_y;
   Double_t        H_tr_n;
   Double_t        g_evtyp;
 //THaEvent        *Event_Branch;
   ULong64_t       fEvtHdr_fEvtTime;
   UInt_t          fEvtHdr_fEvtNum;
   Int_t           fEvtHdr_fEvtType;
   Int_t           fEvtHdr_fEvtLen;
   Int_t           fEvtHdr_fHelicity;
   Int_t           fEvtHdr_fTargetPol;
   Int_t           fEvtHdr_fRun;

   // List of branches
   TBranch        *b_Ndata_H_cal_1pr_aneg;   //!
   TBranch        *b_H_cal_1pr_aneg;   //!
   TBranch        *b_Ndata_H_cal_1pr_aneg_p;   //!
   TBranch        *b_H_cal_1pr_aneg_p;   //!
   TBranch        *b_Ndata_H_cal_1pr_apos;   //!
   TBranch        *b_H_cal_1pr_apos;   //!
   TBranch        *b_Ndata_H_cal_1pr_apos_p;   //!
   TBranch        *b_H_cal_1pr_apos_p;   //!
   TBranch        *b_Ndata_H_cal_1pr_emean;   //!
   TBranch        *b_H_cal_1pr_emean;   //!
   TBranch        *b_Ndata_H_cal_1pr_eneg;   //!
   TBranch        *b_H_cal_1pr_eneg;   //!
   TBranch        *b_Ndata_H_cal_1pr_epos;   //!
   TBranch        *b_H_cal_1pr_epos;   //!
   TBranch        *b_Ndata_H_cal_1pr_negadchits;   //!
   TBranch        *b_H_cal_1pr_negadchits;   //!
   TBranch        *b_Ndata_H_cal_1pr_posadchits;   //!
   TBranch        *b_H_cal_1pr_posadchits;   //!
   TBranch        *b_Ndata_H_cal_2ta_aneg;   //!
   TBranch        *b_H_cal_2ta_aneg;   //!
   TBranch        *b_Ndata_H_cal_2ta_aneg_p;   //!
   TBranch        *b_H_cal_2ta_aneg_p;   //!
   TBranch        *b_Ndata_H_cal_2ta_apos;   //!
   TBranch        *b_H_cal_2ta_apos;   //!
   TBranch        *b_Ndata_H_cal_2ta_apos_p;   //!
   TBranch        *b_H_cal_2ta_apos_p;   //!
   TBranch        *b_Ndata_H_cal_2ta_emean;   //!
   TBranch        *b_H_cal_2ta_emean;   //!
   TBranch        *b_Ndata_H_cal_2ta_eneg;   //!
   TBranch        *b_H_cal_2ta_eneg;   //!
   TBranch        *b_Ndata_H_cal_2ta_epos;   //!
   TBranch        *b_H_cal_2ta_epos;   //!
   TBranch        *b_Ndata_H_cal_2ta_negadchits;   //!
   TBranch        *b_H_cal_2ta_negadchits;   //!
   TBranch        *b_Ndata_H_cal_2ta_posadchits;   //!
   TBranch        *b_H_cal_2ta_posadchits;   //!
   TBranch        *b_Ndata_H_cal_3ta_aneg;   //!
   TBranch        *b_H_cal_3ta_aneg;   //!
   TBranch        *b_Ndata_H_cal_3ta_aneg_p;   //!
   TBranch        *b_H_cal_3ta_aneg_p;   //!
   TBranch        *b_Ndata_H_cal_3ta_apos;   //!
   TBranch        *b_H_cal_3ta_apos;   //!
   TBranch        *b_Ndata_H_cal_3ta_apos_p;   //!
   TBranch        *b_H_cal_3ta_apos_p;   //!
   TBranch        *b_Ndata_H_cal_3ta_emean;   //!
   TBranch        *b_H_cal_3ta_emean;   //!
   TBranch        *b_Ndata_H_cal_3ta_eneg;   //!
   TBranch        *b_H_cal_3ta_eneg;   //!
   TBranch        *b_Ndata_H_cal_3ta_epos;   //!
   TBranch        *b_H_cal_3ta_epos;   //!
   TBranch        *b_Ndata_H_cal_3ta_negadchits;   //!
   TBranch        *b_H_cal_3ta_negadchits;   //!
   TBranch        *b_Ndata_H_cal_3ta_posadchits;   //!
   TBranch        *b_H_cal_3ta_posadchits;   //!
   TBranch        *b_Ndata_H_cal_4ta_aneg;   //!
   TBranch        *b_H_cal_4ta_aneg;   //!
   TBranch        *b_Ndata_H_cal_4ta_aneg_p;   //!
   TBranch        *b_H_cal_4ta_aneg_p;   //!
   TBranch        *b_Ndata_H_cal_4ta_apos;   //!
   TBranch        *b_H_cal_4ta_apos;   //!
   TBranch        *b_Ndata_H_cal_4ta_apos_p;   //!
   TBranch        *b_H_cal_4ta_apos_p;   //!
   TBranch        *b_Ndata_H_cal_4ta_emean;   //!
   TBranch        *b_H_cal_4ta_emean;   //!
   TBranch        *b_Ndata_H_cal_4ta_eneg;   //!
   TBranch        *b_H_cal_4ta_eneg;   //!
   TBranch        *b_Ndata_H_cal_4ta_epos;   //!
   TBranch        *b_H_cal_4ta_epos;   //!
   TBranch        *b_Ndata_H_cal_4ta_negadchits;   //!
   TBranch        *b_H_cal_4ta_negadchits;   //!
   TBranch        *b_Ndata_H_cal_4ta_posadchits;   //!
   TBranch        *b_H_cal_4ta_posadchits;   //!
   TBranch        *b_Ndata_H_tr_beta;   //!
   TBranch        *b_H_tr_beta;   //!
   TBranch        *b_Ndata_H_tr_chi2;   //!
   TBranch        *b_H_tr_chi2;   //!
   TBranch        *b_Ndata_H_tr_d_ph;   //!
   TBranch        *b_H_tr_d_ph;   //!
   TBranch        *b_Ndata_H_tr_d_th;   //!
   TBranch        *b_H_tr_d_th;   //!
   TBranch        *b_Ndata_H_tr_d_x;   //!
   TBranch        *b_H_tr_d_x;   //!
   TBranch        *b_Ndata_H_tr_d_y;   //!
   TBranch        *b_H_tr_d_y;   //!
   TBranch        *b_Ndata_H_tr_dbeta;   //!
   TBranch        *b_H_tr_dbeta;   //!
   TBranch        *b_Ndata_H_tr_dtime;   //!
   TBranch        *b_H_tr_dtime;   //!
   TBranch        *b_Ndata_H_tr_flag;   //!
   TBranch        *b_H_tr_flag;   //!
   TBranch        *b_Ndata_H_tr_ndof;   //!
   TBranch        *b_H_tr_ndof;   //!
   TBranch        *b_Ndata_H_tr_p;   //!
   TBranch        *b_H_tr_p;   //!
   TBranch        *b_Ndata_H_tr_pathl;   //!
   TBranch        *b_H_tr_pathl;   //!
   TBranch        *b_Ndata_H_tr_ph;   //!
   TBranch        *b_H_tr_ph;   //!
   TBranch        *b_Ndata_H_tr_px;   //!
   TBranch        *b_H_tr_px;   //!
   TBranch        *b_Ndata_H_tr_py;   //!
   TBranch        *b_H_tr_py;   //!
   TBranch        *b_Ndata_H_tr_pz;   //!
   TBranch        *b_H_tr_pz;   //!
   TBranch        *b_Ndata_H_tr_r_ph;   //!
   TBranch        *b_H_tr_r_ph;   //!
   TBranch        *b_Ndata_H_tr_r_th;   //!
   TBranch        *b_H_tr_r_th;   //!
   TBranch        *b_Ndata_H_tr_r_x;   //!
   TBranch        *b_H_tr_r_x;   //!
   TBranch        *b_Ndata_H_tr_r_y;   //!
   TBranch        *b_H_tr_r_y;   //!
   TBranch        *b_Ndata_H_tr_tg_dp;   //!
   TBranch        *b_H_tr_tg_dp;   //!
   TBranch        *b_Ndata_H_tr_tg_ph;   //!
   TBranch        *b_H_tr_tg_ph;   //!
   TBranch        *b_Ndata_H_tr_tg_th;   //!
   TBranch        *b_H_tr_tg_th;   //!
   TBranch        *b_Ndata_H_tr_tg_y;   //!
   TBranch        *b_H_tr_tg_y;   //!
   TBranch        *b_Ndata_H_tr_th;   //!
   TBranch        *b_H_tr_th;   //!
   TBranch        *b_Ndata_H_tr_time;   //!
   TBranch        *b_H_tr_time;   //!
   TBranch        *b_Ndata_H_tr_vx;   //!
   TBranch        *b_H_tr_vx;   //!
   TBranch        *b_Ndata_H_tr_vy;   //!
   TBranch        *b_H_tr_vy;   //!
   TBranch        *b_Ndata_H_tr_vz;   //!
   TBranch        *b_H_tr_vz;   //!
   TBranch        *b_Ndata_H_tr_x;   //!
   TBranch        *b_H_tr_x;   //!
   TBranch        *b_Ndata_H_tr_y;   //!
   TBranch        *b_H_tr_y;   //!
   TBranch        *b_H_cal_1pr_eplane;   //!
   TBranch        *b_H_cal_1pr_eplane_neg;   //!
   TBranch        *b_H_cal_1pr_eplane_pos;   //!
   TBranch        *b_H_cal_2ta_eplane;   //!
   TBranch        *b_H_cal_2ta_eplane_neg;   //!
   TBranch        *b_H_cal_2ta_eplane_pos;   //!
   TBranch        *b_H_cal_3ta_eplane;   //!
   TBranch        *b_H_cal_3ta_eplane_neg;   //!
   TBranch        *b_H_cal_3ta_eplane_pos;   //!
   TBranch        *b_H_cal_4ta_eplane;   //!
   TBranch        *b_H_cal_4ta_eplane_neg;   //!
   TBranch        *b_H_cal_4ta_eplane_pos;   //!
   TBranch        *b_H_cal_emax;   //!
   TBranch        *b_H_cal_eprmax;   //!
   TBranch        *b_H_cal_mult;   //!
   TBranch        *b_H_cal_nclust;   //!
   TBranch        *b_H_cal_nhits;   //!
   TBranch        *b_H_cal_ntracks;   //!
   TBranch        *b_H_cal_tre;   //!
   TBranch        *b_H_cal_trecor;   //!
   TBranch        *b_H_cal_treplcor;   //!
   TBranch        *b_H_cal_trepr;   //!
   TBranch        *b_H_cal_treprcor;   //!
   TBranch        *b_H_cal_trx;   //!
   TBranch        *b_H_cal_try;   //!
   TBranch        *b_H_cal_xmax;   //!
   TBranch        *b_H_gold_dp;   //!
   TBranch        *b_H_gold_e;   //!
   TBranch        *b_H_gold_fp_ph;   //!
   TBranch        *b_H_gold_fp_th;   //!
   TBranch        *b_H_gold_fp_x;   //!
   TBranch        *b_H_gold_fp_y;   //!
   TBranch        *b_H_gold_index;   //!
   TBranch        *b_H_gold_ok;   //!
   TBranch        *b_H_gold_p;   //!
   TBranch        *b_H_gold_ph;   //!
   TBranch        *b_H_gold_px;   //!
   TBranch        *b_H_gold_py;   //!
   TBranch        *b_H_gold_pz;   //!
   TBranch        *b_H_gold_th;   //!
   TBranch        *b_H_gold_x;   //!
   TBranch        *b_H_gold_y;   //!
   TBranch        *b_H_tr_n;   //!
   TBranch        *b_g_evtyp;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtTime;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtNum;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtType;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtLen;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fHelicity;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fTargetPol;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fRun;   //!

   hcana_class(TTree *tree=0);
   virtual ~hcana_class();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef hcana_class_cxx
hcana_class::hcana_class(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("hodtest_52949.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("hodtest_52949.root");
      }
      f->GetObject("T",tree);

   }
   Init(tree);
}

hcana_class::~hcana_class()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t hcana_class::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t hcana_class::LoadTree(Long64_t entry)
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

void hcana_class::Init(TTree *tree)
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

   fChain->SetBranchAddress("Ndata.H.cal.1pr.aneg", &Ndata_H_cal_1pr_aneg, &b_Ndata_H_cal_1pr_aneg);
   fChain->SetBranchAddress("H.cal.1pr.aneg", H_cal_1pr_aneg, &b_H_cal_1pr_aneg);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.aneg_p", &Ndata_H_cal_1pr_aneg_p, &b_Ndata_H_cal_1pr_aneg_p);
   fChain->SetBranchAddress("H.cal.1pr.aneg_p", H_cal_1pr_aneg_p, &b_H_cal_1pr_aneg_p);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.apos", &Ndata_H_cal_1pr_apos, &b_Ndata_H_cal_1pr_apos);
   fChain->SetBranchAddress("H.cal.1pr.apos", H_cal_1pr_apos, &b_H_cal_1pr_apos);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.apos_p", &Ndata_H_cal_1pr_apos_p, &b_Ndata_H_cal_1pr_apos_p);
   fChain->SetBranchAddress("H.cal.1pr.apos_p", H_cal_1pr_apos_p, &b_H_cal_1pr_apos_p);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.emean", &Ndata_H_cal_1pr_emean, &b_Ndata_H_cal_1pr_emean);
   fChain->SetBranchAddress("H.cal.1pr.emean", H_cal_1pr_emean, &b_H_cal_1pr_emean);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.eneg", &Ndata_H_cal_1pr_eneg, &b_Ndata_H_cal_1pr_eneg);
   fChain->SetBranchAddress("H.cal.1pr.eneg", H_cal_1pr_eneg, &b_H_cal_1pr_eneg);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.epos", &Ndata_H_cal_1pr_epos, &b_Ndata_H_cal_1pr_epos);
   fChain->SetBranchAddress("H.cal.1pr.epos", H_cal_1pr_epos, &b_H_cal_1pr_epos);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.negadchits", &Ndata_H_cal_1pr_negadchits, &b_Ndata_H_cal_1pr_negadchits);
   fChain->SetBranchAddress("H.cal.1pr.negadchits", H_cal_1pr_negadchits, &b_H_cal_1pr_negadchits);
   fChain->SetBranchAddress("Ndata.H.cal.1pr.posadchits", &Ndata_H_cal_1pr_posadchits, &b_Ndata_H_cal_1pr_posadchits);
   fChain->SetBranchAddress("H.cal.1pr.posadchits", H_cal_1pr_posadchits, &b_H_cal_1pr_posadchits);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.aneg", &Ndata_H_cal_2ta_aneg, &b_Ndata_H_cal_2ta_aneg);
   fChain->SetBranchAddress("H.cal.2ta.aneg", H_cal_2ta_aneg, &b_H_cal_2ta_aneg);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.aneg_p", &Ndata_H_cal_2ta_aneg_p, &b_Ndata_H_cal_2ta_aneg_p);
   fChain->SetBranchAddress("H.cal.2ta.aneg_p", H_cal_2ta_aneg_p, &b_H_cal_2ta_aneg_p);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.apos", &Ndata_H_cal_2ta_apos, &b_Ndata_H_cal_2ta_apos);
   fChain->SetBranchAddress("H.cal.2ta.apos", H_cal_2ta_apos, &b_H_cal_2ta_apos);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.apos_p", &Ndata_H_cal_2ta_apos_p, &b_Ndata_H_cal_2ta_apos_p);
   fChain->SetBranchAddress("H.cal.2ta.apos_p", H_cal_2ta_apos_p, &b_H_cal_2ta_apos_p);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.emean", &Ndata_H_cal_2ta_emean, &b_Ndata_H_cal_2ta_emean);
   fChain->SetBranchAddress("H.cal.2ta.emean", H_cal_2ta_emean, &b_H_cal_2ta_emean);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.eneg", &Ndata_H_cal_2ta_eneg, &b_Ndata_H_cal_2ta_eneg);
   fChain->SetBranchAddress("H.cal.2ta.eneg", H_cal_2ta_eneg, &b_H_cal_2ta_eneg);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.epos", &Ndata_H_cal_2ta_epos, &b_Ndata_H_cal_2ta_epos);
   fChain->SetBranchAddress("H.cal.2ta.epos", H_cal_2ta_epos, &b_H_cal_2ta_epos);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.negadchits", &Ndata_H_cal_2ta_negadchits, &b_Ndata_H_cal_2ta_negadchits);
   fChain->SetBranchAddress("H.cal.2ta.negadchits", H_cal_2ta_negadchits, &b_H_cal_2ta_negadchits);
   fChain->SetBranchAddress("Ndata.H.cal.2ta.posadchits", &Ndata_H_cal_2ta_posadchits, &b_Ndata_H_cal_2ta_posadchits);
   fChain->SetBranchAddress("H.cal.2ta.posadchits", H_cal_2ta_posadchits, &b_H_cal_2ta_posadchits);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.aneg", &Ndata_H_cal_3ta_aneg, &b_Ndata_H_cal_3ta_aneg);
   fChain->SetBranchAddress("H.cal.3ta.aneg", H_cal_3ta_aneg, &b_H_cal_3ta_aneg);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.aneg_p", &Ndata_H_cal_3ta_aneg_p, &b_Ndata_H_cal_3ta_aneg_p);
   fChain->SetBranchAddress("H.cal.3ta.aneg_p", H_cal_3ta_aneg_p, &b_H_cal_3ta_aneg_p);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.apos", &Ndata_H_cal_3ta_apos, &b_Ndata_H_cal_3ta_apos);
   fChain->SetBranchAddress("H.cal.3ta.apos", H_cal_3ta_apos, &b_H_cal_3ta_apos);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.apos_p", &Ndata_H_cal_3ta_apos_p, &b_Ndata_H_cal_3ta_apos_p);
   fChain->SetBranchAddress("H.cal.3ta.apos_p", H_cal_3ta_apos_p, &b_H_cal_3ta_apos_p);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.emean", &Ndata_H_cal_3ta_emean, &b_Ndata_H_cal_3ta_emean);
   fChain->SetBranchAddress("H.cal.3ta.emean", H_cal_3ta_emean, &b_H_cal_3ta_emean);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.eneg", &Ndata_H_cal_3ta_eneg, &b_Ndata_H_cal_3ta_eneg);
   fChain->SetBranchAddress("H.cal.3ta.eneg", H_cal_3ta_eneg, &b_H_cal_3ta_eneg);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.epos", &Ndata_H_cal_3ta_epos, &b_Ndata_H_cal_3ta_epos);
   fChain->SetBranchAddress("H.cal.3ta.epos", H_cal_3ta_epos, &b_H_cal_3ta_epos);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.negadchits", &Ndata_H_cal_3ta_negadchits, &b_Ndata_H_cal_3ta_negadchits);
   fChain->SetBranchAddress("H.cal.3ta.negadchits", &H_cal_3ta_negadchits, &b_H_cal_3ta_negadchits);
   fChain->SetBranchAddress("Ndata.H.cal.3ta.posadchits", &Ndata_H_cal_3ta_posadchits, &b_Ndata_H_cal_3ta_posadchits);
   fChain->SetBranchAddress("H.cal.3ta.posadchits", H_cal_3ta_posadchits, &b_H_cal_3ta_posadchits);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.aneg", &Ndata_H_cal_4ta_aneg, &b_Ndata_H_cal_4ta_aneg);
   fChain->SetBranchAddress("H.cal.4ta.aneg", H_cal_4ta_aneg, &b_H_cal_4ta_aneg);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.aneg_p", &Ndata_H_cal_4ta_aneg_p, &b_Ndata_H_cal_4ta_aneg_p);
   fChain->SetBranchAddress("H.cal.4ta.aneg_p", H_cal_4ta_aneg_p, &b_H_cal_4ta_aneg_p);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.apos", &Ndata_H_cal_4ta_apos, &b_Ndata_H_cal_4ta_apos);
   fChain->SetBranchAddress("H.cal.4ta.apos", H_cal_4ta_apos, &b_H_cal_4ta_apos);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.apos_p", &Ndata_H_cal_4ta_apos_p, &b_Ndata_H_cal_4ta_apos_p);
   fChain->SetBranchAddress("H.cal.4ta.apos_p", H_cal_4ta_apos_p, &b_H_cal_4ta_apos_p);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.emean", &Ndata_H_cal_4ta_emean, &b_Ndata_H_cal_4ta_emean);
   fChain->SetBranchAddress("H.cal.4ta.emean", H_cal_4ta_emean, &b_H_cal_4ta_emean);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.eneg", &Ndata_H_cal_4ta_eneg, &b_Ndata_H_cal_4ta_eneg);
   fChain->SetBranchAddress("H.cal.4ta.eneg", H_cal_4ta_eneg, &b_H_cal_4ta_eneg);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.epos", &Ndata_H_cal_4ta_epos, &b_Ndata_H_cal_4ta_epos);
   fChain->SetBranchAddress("H.cal.4ta.epos", H_cal_4ta_epos, &b_H_cal_4ta_epos);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.negadchits", &Ndata_H_cal_4ta_negadchits, &b_Ndata_H_cal_4ta_negadchits);
   fChain->SetBranchAddress("H.cal.4ta.negadchits", &H_cal_4ta_negadchits, &b_H_cal_4ta_negadchits);
   fChain->SetBranchAddress("Ndata.H.cal.4ta.posadchits", &Ndata_H_cal_4ta_posadchits, &b_Ndata_H_cal_4ta_posadchits);
   fChain->SetBranchAddress("H.cal.4ta.posadchits", H_cal_4ta_posadchits, &b_H_cal_4ta_posadchits);
   fChain->SetBranchAddress("Ndata.H.tr.beta", &Ndata_H_tr_beta, &b_Ndata_H_tr_beta);
   fChain->SetBranchAddress("H.tr.beta", H_tr_beta, &b_H_tr_beta);
   fChain->SetBranchAddress("Ndata.H.tr.chi2", &Ndata_H_tr_chi2, &b_Ndata_H_tr_chi2);
   fChain->SetBranchAddress("H.tr.chi2", H_tr_chi2, &b_H_tr_chi2);
   fChain->SetBranchAddress("Ndata.H.tr.d_ph", &Ndata_H_tr_d_ph, &b_Ndata_H_tr_d_ph);
   fChain->SetBranchAddress("H.tr.d_ph", H_tr_d_ph, &b_H_tr_d_ph);
   fChain->SetBranchAddress("Ndata.H.tr.d_th", &Ndata_H_tr_d_th, &b_Ndata_H_tr_d_th);
   fChain->SetBranchAddress("H.tr.d_th", H_tr_d_th, &b_H_tr_d_th);
   fChain->SetBranchAddress("Ndata.H.tr.d_x", &Ndata_H_tr_d_x, &b_Ndata_H_tr_d_x);
   fChain->SetBranchAddress("H.tr.d_x", H_tr_d_x, &b_H_tr_d_x);
   fChain->SetBranchAddress("Ndata.H.tr.d_y", &Ndata_H_tr_d_y, &b_Ndata_H_tr_d_y);
   fChain->SetBranchAddress("H.tr.d_y", H_tr_d_y, &b_H_tr_d_y);
   fChain->SetBranchAddress("Ndata.H.tr.dbeta", &Ndata_H_tr_dbeta, &b_Ndata_H_tr_dbeta);
   fChain->SetBranchAddress("H.tr.dbeta", H_tr_dbeta, &b_H_tr_dbeta);
   fChain->SetBranchAddress("Ndata.H.tr.dtime", &Ndata_H_tr_dtime, &b_Ndata_H_tr_dtime);
   fChain->SetBranchAddress("H.tr.dtime", H_tr_dtime, &b_H_tr_dtime);
   fChain->SetBranchAddress("Ndata.H.tr.flag", &Ndata_H_tr_flag, &b_Ndata_H_tr_flag);
   fChain->SetBranchAddress("H.tr.flag", H_tr_flag, &b_H_tr_flag);
   fChain->SetBranchAddress("Ndata.H.tr.ndof", &Ndata_H_tr_ndof, &b_Ndata_H_tr_ndof);
   fChain->SetBranchAddress("H.tr.ndof", H_tr_ndof, &b_H_tr_ndof);
   fChain->SetBranchAddress("Ndata.H.tr.p", &Ndata_H_tr_p, &b_Ndata_H_tr_p);
   fChain->SetBranchAddress("H.tr.p", H_tr_p, &b_H_tr_p);
   fChain->SetBranchAddress("Ndata.H.tr.pathl", &Ndata_H_tr_pathl, &b_Ndata_H_tr_pathl);
   fChain->SetBranchAddress("H.tr.pathl", H_tr_pathl, &b_H_tr_pathl);
   fChain->SetBranchAddress("Ndata.H.tr.ph", &Ndata_H_tr_ph, &b_Ndata_H_tr_ph);
   fChain->SetBranchAddress("H.tr.ph", H_tr_ph, &b_H_tr_ph);
   fChain->SetBranchAddress("Ndata.H.tr.px", &Ndata_H_tr_px, &b_Ndata_H_tr_px);
   fChain->SetBranchAddress("H.tr.px", H_tr_px, &b_H_tr_px);
   fChain->SetBranchAddress("Ndata.H.tr.py", &Ndata_H_tr_py, &b_Ndata_H_tr_py);
   fChain->SetBranchAddress("H.tr.py", H_tr_py, &b_H_tr_py);
   fChain->SetBranchAddress("Ndata.H.tr.pz", &Ndata_H_tr_pz, &b_Ndata_H_tr_pz);
   fChain->SetBranchAddress("H.tr.pz", H_tr_pz, &b_H_tr_pz);
   fChain->SetBranchAddress("Ndata.H.tr.r_ph", &Ndata_H_tr_r_ph, &b_Ndata_H_tr_r_ph);
   fChain->SetBranchAddress("H.tr.r_ph", H_tr_r_ph, &b_H_tr_r_ph);
   fChain->SetBranchAddress("Ndata.H.tr.r_th", &Ndata_H_tr_r_th, &b_Ndata_H_tr_r_th);
   fChain->SetBranchAddress("H.tr.r_th", H_tr_r_th, &b_H_tr_r_th);
   fChain->SetBranchAddress("Ndata.H.tr.r_x", &Ndata_H_tr_r_x, &b_Ndata_H_tr_r_x);
   fChain->SetBranchAddress("H.tr.r_x", H_tr_r_x, &b_H_tr_r_x);
   fChain->SetBranchAddress("Ndata.H.tr.r_y", &Ndata_H_tr_r_y, &b_Ndata_H_tr_r_y);
   fChain->SetBranchAddress("H.tr.r_y", H_tr_r_y, &b_H_tr_r_y);
   fChain->SetBranchAddress("Ndata.H.tr.tg_dp", &Ndata_H_tr_tg_dp, &b_Ndata_H_tr_tg_dp);
   fChain->SetBranchAddress("H.tr.tg_dp", H_tr_tg_dp, &b_H_tr_tg_dp);
   fChain->SetBranchAddress("Ndata.H.tr.tg_ph", &Ndata_H_tr_tg_ph, &b_Ndata_H_tr_tg_ph);
   fChain->SetBranchAddress("H.tr.tg_ph", H_tr_tg_ph, &b_H_tr_tg_ph);
   fChain->SetBranchAddress("Ndata.H.tr.tg_th", &Ndata_H_tr_tg_th, &b_Ndata_H_tr_tg_th);
   fChain->SetBranchAddress("H.tr.tg_th", H_tr_tg_th, &b_H_tr_tg_th);
   fChain->SetBranchAddress("Ndata.H.tr.tg_y", &Ndata_H_tr_tg_y, &b_Ndata_H_tr_tg_y);
   fChain->SetBranchAddress("H.tr.tg_y", H_tr_tg_y, &b_H_tr_tg_y);
   fChain->SetBranchAddress("Ndata.H.tr.th", &Ndata_H_tr_th, &b_Ndata_H_tr_th);
   fChain->SetBranchAddress("H.tr.th", H_tr_th, &b_H_tr_th);
   fChain->SetBranchAddress("Ndata.H.tr.time", &Ndata_H_tr_time, &b_Ndata_H_tr_time);
   fChain->SetBranchAddress("H.tr.time", H_tr_time, &b_H_tr_time);
   fChain->SetBranchAddress("Ndata.H.tr.vx", &Ndata_H_tr_vx, &b_Ndata_H_tr_vx);
   fChain->SetBranchAddress("H.tr.vx", H_tr_vx, &b_H_tr_vx);
   fChain->SetBranchAddress("Ndata.H.tr.vy", &Ndata_H_tr_vy, &b_Ndata_H_tr_vy);
   fChain->SetBranchAddress("H.tr.vy", H_tr_vy, &b_H_tr_vy);
   fChain->SetBranchAddress("Ndata.H.tr.vz", &Ndata_H_tr_vz, &b_Ndata_H_tr_vz);
   fChain->SetBranchAddress("H.tr.vz", H_tr_vz, &b_H_tr_vz);
   fChain->SetBranchAddress("Ndata.H.tr.x", &Ndata_H_tr_x, &b_Ndata_H_tr_x);
   fChain->SetBranchAddress("H.tr.x", H_tr_x, &b_H_tr_x);
   fChain->SetBranchAddress("Ndata.H.tr.y", &Ndata_H_tr_y, &b_Ndata_H_tr_y);
   fChain->SetBranchAddress("H.tr.y", H_tr_y, &b_H_tr_y);
   fChain->SetBranchAddress("H.cal.1pr.eplane", &H_cal_1pr_eplane, &b_H_cal_1pr_eplane);
   fChain->SetBranchAddress("H.cal.1pr.eplane_neg", &H_cal_1pr_eplane_neg, &b_H_cal_1pr_eplane_neg);
   fChain->SetBranchAddress("H.cal.1pr.eplane_pos", &H_cal_1pr_eplane_pos, &b_H_cal_1pr_eplane_pos);
   fChain->SetBranchAddress("H.cal.2ta.eplane", &H_cal_2ta_eplane, &b_H_cal_2ta_eplane);
   fChain->SetBranchAddress("H.cal.2ta.eplane_neg", &H_cal_2ta_eplane_neg, &b_H_cal_2ta_eplane_neg);
   fChain->SetBranchAddress("H.cal.2ta.eplane_pos", &H_cal_2ta_eplane_pos, &b_H_cal_2ta_eplane_pos);
   fChain->SetBranchAddress("H.cal.3ta.eplane", &H_cal_3ta_eplane, &b_H_cal_3ta_eplane);
   fChain->SetBranchAddress("H.cal.3ta.eplane_neg", &H_cal_3ta_eplane_neg, &b_H_cal_3ta_eplane_neg);
   fChain->SetBranchAddress("H.cal.3ta.eplane_pos", &H_cal_3ta_eplane_pos, &b_H_cal_3ta_eplane_pos);
   fChain->SetBranchAddress("H.cal.4ta.eplane", &H_cal_4ta_eplane, &b_H_cal_4ta_eplane);
   fChain->SetBranchAddress("H.cal.4ta.eplane_neg", &H_cal_4ta_eplane_neg, &b_H_cal_4ta_eplane_neg);
   fChain->SetBranchAddress("H.cal.4ta.eplane_pos", &H_cal_4ta_eplane_pos, &b_H_cal_4ta_eplane_pos);
   fChain->SetBranchAddress("H.cal.emax", &H_cal_emax, &b_H_cal_emax);
   fChain->SetBranchAddress("H.cal.eprmax", &H_cal_eprmax, &b_H_cal_eprmax);
   fChain->SetBranchAddress("H.cal.mult", &H_cal_mult, &b_H_cal_mult);
   fChain->SetBranchAddress("H.cal.nclust", &H_cal_nclust, &b_H_cal_nclust);
   fChain->SetBranchAddress("H.cal.nhits", &H_cal_nhits, &b_H_cal_nhits);
   fChain->SetBranchAddress("H.cal.ntracks", &H_cal_ntracks, &b_H_cal_ntracks);
   fChain->SetBranchAddress("H.cal.tre", &H_cal_tre, &b_H_cal_tre);
   fChain->SetBranchAddress("H.cal.trecor", &H_cal_trecor, &b_H_cal_trecor);
   fChain->SetBranchAddress("H.cal.treplcor", &H_cal_treplcor, &b_H_cal_treplcor);
   fChain->SetBranchAddress("H.cal.trepr", &H_cal_trepr, &b_H_cal_trepr);
   fChain->SetBranchAddress("H.cal.treprcor", &H_cal_treprcor, &b_H_cal_treprcor);
   fChain->SetBranchAddress("H.cal.trx", &H_cal_trx, &b_H_cal_trx);
   fChain->SetBranchAddress("H.cal.try", &H_cal_try, &b_H_cal_try);
   fChain->SetBranchAddress("H.cal.xmax", &H_cal_xmax, &b_H_cal_xmax);
   fChain->SetBranchAddress("H.gold.dp", &H_gold_dp, &b_H_gold_dp);
   fChain->SetBranchAddress("H.gold.e", &H_gold_e, &b_H_gold_e);
   fChain->SetBranchAddress("H.gold.fp_ph", &H_gold_fp_ph, &b_H_gold_fp_ph);
   fChain->SetBranchAddress("H.gold.fp_th", &H_gold_fp_th, &b_H_gold_fp_th);
   fChain->SetBranchAddress("H.gold.fp_x", &H_gold_fp_x, &b_H_gold_fp_x);
   fChain->SetBranchAddress("H.gold.fp_y", &H_gold_fp_y, &b_H_gold_fp_y);
   fChain->SetBranchAddress("H.gold.index", &H_gold_index, &b_H_gold_index);
   fChain->SetBranchAddress("H.gold.ok", &H_gold_ok, &b_H_gold_ok);
   fChain->SetBranchAddress("H.gold.p", &H_gold_p, &b_H_gold_p);
   fChain->SetBranchAddress("H.gold.ph", &H_gold_ph, &b_H_gold_ph);
   fChain->SetBranchAddress("H.gold.px", &H_gold_px, &b_H_gold_px);
   fChain->SetBranchAddress("H.gold.py", &H_gold_py, &b_H_gold_py);
   fChain->SetBranchAddress("H.gold.pz", &H_gold_pz, &b_H_gold_pz);
   fChain->SetBranchAddress("H.gold.th", &H_gold_th, &b_H_gold_th);
   fChain->SetBranchAddress("H.gold.x", &H_gold_x, &b_H_gold_x);
   fChain->SetBranchAddress("H.gold.y", &H_gold_y, &b_H_gold_y);
   fChain->SetBranchAddress("H.tr.n", &H_tr_n, &b_H_tr_n);
   fChain->SetBranchAddress("g.evtyp", &g_evtyp, &b_g_evtyp);
   fChain->SetBranchAddress("fEvtHdr.fEvtTime", &fEvtHdr_fEvtTime, &b_Event_Branch_fEvtHdr_fEvtTime);
   fChain->SetBranchAddress("fEvtHdr.fEvtNum", &fEvtHdr_fEvtNum, &b_Event_Branch_fEvtHdr_fEvtNum);
   fChain->SetBranchAddress("fEvtHdr.fEvtType", &fEvtHdr_fEvtType, &b_Event_Branch_fEvtHdr_fEvtType);
   fChain->SetBranchAddress("fEvtHdr.fEvtLen", &fEvtHdr_fEvtLen, &b_Event_Branch_fEvtHdr_fEvtLen);
   fChain->SetBranchAddress("fEvtHdr.fHelicity", &fEvtHdr_fHelicity, &b_Event_Branch_fEvtHdr_fHelicity);
   fChain->SetBranchAddress("fEvtHdr.fTargetPol", &fEvtHdr_fTargetPol, &b_Event_Branch_fEvtHdr_fTargetPol);
   fChain->SetBranchAddress("fEvtHdr.fRun", &fEvtHdr_fRun, &b_Event_Branch_fEvtHdr_fRun);
   Notify();
}

Bool_t hcana_class::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void hcana_class::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t hcana_class::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef hcana_class_cxx
