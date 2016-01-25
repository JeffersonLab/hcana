//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jan 18 03:00:04 2016 by ROOT version 5.34/30
// from TTree T/Hall A Analyzer Output DST
// found on file: shms_52949.root
//////////////////////////////////////////////////////////

#ifndef shms_class_h
#define shms_class_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "../../podd/src/THaEvent.h"
#include "../../podd/src/THaEvent.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class shms_class {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Double_t        Hhodtdif1;
   Double_t        Hhodtdif2;
   Double_t        Hhodtdif3;
   Double_t        Hhodtdif4;
   Double_t        Hhodtdif5;
   Double_t        Hhodtdif6;
   Int_t           Ndata_H_aero_aneg;
   Double_t        H_aero_aneg[8];   //[Ndata.H.aero.aneg]
   Int_t           Ndata_H_aero_aneg_p;
   Double_t        H_aero_aneg_p[8];   //[Ndata.H.aero.aneg_p]
   Int_t           Ndata_H_aero_apos;
   Double_t        H_aero_apos[8];   //[Ndata.H.aero.apos]
   Int_t           Ndata_H_aero_apos_p;
   Double_t        H_aero_apos_p[8];   //[Ndata.H.aero.apos_p]
   Int_t           Ndata_H_aero_neg_npe;
   Double_t        H_aero_neg_npe[8];   //[Ndata.H.aero.neg_npe]
   Int_t           Ndata_H_aero_negadchits;
   Double_t        H_aero_negadchits[8];   //[Ndata.H.aero.negadchits]
   Int_t           Ndata_H_aero_negtdchits;
   Double_t        H_aero_negtdchits[1];   //[Ndata.H.aero.negtdchits]
   Int_t           Ndata_H_aero_pos_npe;
   Double_t        H_aero_pos_npe[8];   //[Ndata.H.aero.pos_npe]
   Int_t           Ndata_H_aero_posadchits;
   Double_t        H_aero_posadchits[8];   //[Ndata.H.aero.posadchits]
   Int_t           Ndata_H_aero_postdchits;
   Double_t        H_aero_postdchits[1];   //[Ndata.H.aero.postdchits]
   Int_t           Ndata_H_aero_tneg;
   Double_t        H_aero_tneg[8];   //[Ndata.H.aero.tneg]
   Int_t           Ndata_H_aero_tpos;
   Double_t        H_aero_tpos[8];   //[Ndata.H.aero.tpos]
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
   Int_t           Ndata_H_cher_adc;
   Double_t        H_cher_adc[2];   //[Ndata.H.cher.adc]
   Int_t           Ndata_H_cher_adc_p;
   Double_t        H_cher_adc_p[2];   //[Ndata.H.cher.adc_p]
   Int_t           Ndata_H_cher_cerfiredcounter;
   Double_t        H_cher_cerfiredcounter[3];   //[Ndata.H.cher.cerfiredcounter]
   Int_t           Ndata_H_cher_certrackcounter;
   Double_t        H_cher_certrackcounter[3];   //[Ndata.H.cher.certrackcounter]
   Int_t           Ndata_H_cher_npe;
   Double_t        H_cher_npe[2];   //[Ndata.H.cher.npe]
   Int_t           Ndata_H_cher_phototubes;
   Double_t        H_cher_phototubes[2];   //[Ndata.H.cher.phototubes]
   Int_t           Ndata_H_dc_1u1_dist;
   Double_t        H_dc_1u1_dist[21];   //[Ndata.H.dc.1u1.dist]
   Int_t           Ndata_H_dc_1u1_rawtdc;
   Double_t        H_dc_1u1_rawtdc[21];   //[Ndata.H.dc.1u1.rawtdc]
   Int_t           Ndata_H_dc_1u1_tdchits;
   Double_t        H_dc_1u1_tdchits[21];   //[Ndata.H.dc.1u1.tdchits]
   Int_t           Ndata_H_dc_1u1_time;
   Double_t        H_dc_1u1_time[21];   //[Ndata.H.dc.1u1.time]
   Int_t           Ndata_H_dc_1v1_dist;
   Double_t        H_dc_1v1_dist[52];   //[Ndata.H.dc.1v1.dist]
   Int_t           Ndata_H_dc_1v1_rawtdc;
   Double_t        H_dc_1v1_rawtdc[52];   //[Ndata.H.dc.1v1.rawtdc]
   Int_t           Ndata_H_dc_1v1_tdchits;
   Double_t        H_dc_1v1_tdchits[52];   //[Ndata.H.dc.1v1.tdchits]
   Int_t           Ndata_H_dc_1v1_time;
   Double_t        H_dc_1v1_time[52];   //[Ndata.H.dc.1v1.time]
   Int_t           Ndata_H_dc_1x1_dist;
   Double_t        H_dc_1x1_dist[34];   //[Ndata.H.dc.1x1.dist]
   Int_t           Ndata_H_dc_1x1_rawtdc;
   Double_t        H_dc_1x1_rawtdc[34];   //[Ndata.H.dc.1x1.rawtdc]
   Int_t           Ndata_H_dc_1x1_tdchits;
   Double_t        H_dc_1x1_tdchits[34];   //[Ndata.H.dc.1x1.tdchits]
   Int_t           Ndata_H_dc_1x1_time;
   Double_t        H_dc_1x1_time[34];   //[Ndata.H.dc.1x1.time]
   Int_t           Ndata_H_dc_1x2_dist;
   Double_t        H_dc_1x2_dist[35];   //[Ndata.H.dc.1x2.dist]
   Int_t           Ndata_H_dc_1x2_rawtdc;
   Double_t        H_dc_1x2_rawtdc[35];   //[Ndata.H.dc.1x2.rawtdc]
   Int_t           Ndata_H_dc_1x2_tdchits;
   Double_t        H_dc_1x2_tdchits[35];   //[Ndata.H.dc.1x2.tdchits]
   Int_t           Ndata_H_dc_1x2_time;
   Double_t        H_dc_1x2_time[35];   //[Ndata.H.dc.1x2.time]
   Int_t           Ndata_H_dc_1y1_dist;
   Double_t        H_dc_1y1_dist[49];   //[Ndata.H.dc.1y1.dist]
   Int_t           Ndata_H_dc_1y1_rawtdc;
   Double_t        H_dc_1y1_rawtdc[49];   //[Ndata.H.dc.1y1.rawtdc]
   Int_t           Ndata_H_dc_1y1_tdchits;
   Double_t        H_dc_1y1_tdchits[49];   //[Ndata.H.dc.1y1.tdchits]
   Int_t           Ndata_H_dc_1y1_time;
   Double_t        H_dc_1y1_time[49];   //[Ndata.H.dc.1y1.time]
   Int_t           Ndata_H_dc_1y2_dist;
   Double_t        H_dc_1y2_dist[48];   //[Ndata.H.dc.1y2.dist]
   Int_t           Ndata_H_dc_1y2_rawtdc;
   Double_t        H_dc_1y2_rawtdc[48];   //[Ndata.H.dc.1y2.rawtdc]
   Int_t           Ndata_H_dc_1y2_tdchits;
   Double_t        H_dc_1y2_tdchits[48];   //[Ndata.H.dc.1y2.tdchits]
   Int_t           Ndata_H_dc_1y2_time;
   Double_t        H_dc_1y2_time[48];   //[Ndata.H.dc.1y2.time]
   Int_t           Ndata_H_dc_2u1_dist;
   Double_t        H_dc_2u1_dist[60];   //[Ndata.H.dc.2u1.dist]
   Int_t           Ndata_H_dc_2u1_rawtdc;
   Double_t        H_dc_2u1_rawtdc[60];   //[Ndata.H.dc.2u1.rawtdc]
   Int_t           Ndata_H_dc_2u1_tdchits;
   Double_t        H_dc_2u1_tdchits[60];   //[Ndata.H.dc.2u1.tdchits]
   Int_t           Ndata_H_dc_2u1_time;
   Double_t        H_dc_2u1_time[60];   //[Ndata.H.dc.2u1.time]
   Int_t           Ndata_H_dc_2v1_dist;
   Double_t        H_dc_2v1_dist[82];   //[Ndata.H.dc.2v1.dist]
   Int_t           Ndata_H_dc_2v1_rawtdc;
   Double_t        H_dc_2v1_rawtdc[82];   //[Ndata.H.dc.2v1.rawtdc]
   Int_t           Ndata_H_dc_2v1_tdchits;
   Double_t        H_dc_2v1_tdchits[82];   //[Ndata.H.dc.2v1.tdchits]
   Int_t           Ndata_H_dc_2v1_time;
   Double_t        H_dc_2v1_time[82];   //[Ndata.H.dc.2v1.time]
   Int_t           Ndata_H_dc_2x1_dist;
   Double_t        H_dc_2x1_dist[52];   //[Ndata.H.dc.2x1.dist]
   Int_t           Ndata_H_dc_2x1_rawtdc;
   Double_t        H_dc_2x1_rawtdc[52];   //[Ndata.H.dc.2x1.rawtdc]
   Int_t           Ndata_H_dc_2x1_tdchits;
   Double_t        H_dc_2x1_tdchits[52];   //[Ndata.H.dc.2x1.tdchits]
   Int_t           Ndata_H_dc_2x1_time;
   Double_t        H_dc_2x1_time[52];   //[Ndata.H.dc.2x1.time]
   Int_t           Ndata_H_dc_2x2_dist;
   Double_t        H_dc_2x2_dist[41];   //[Ndata.H.dc.2x2.dist]
   Int_t           Ndata_H_dc_2x2_rawtdc;
   Double_t        H_dc_2x2_rawtdc[41];   //[Ndata.H.dc.2x2.rawtdc]
   Int_t           Ndata_H_dc_2x2_tdchits;
   Double_t        H_dc_2x2_tdchits[41];   //[Ndata.H.dc.2x2.tdchits]
   Int_t           Ndata_H_dc_2x2_time;
   Double_t        H_dc_2x2_time[41];   //[Ndata.H.dc.2x2.time]
   Int_t           Ndata_H_dc_2y1_dist;
   Double_t        H_dc_2y1_dist[39];   //[Ndata.H.dc.2y1.dist]
   Int_t           Ndata_H_dc_2y1_rawtdc;
   Double_t        H_dc_2y1_rawtdc[39];   //[Ndata.H.dc.2y1.rawtdc]
   Int_t           Ndata_H_dc_2y1_tdchits;
   Double_t        H_dc_2y1_tdchits[39];   //[Ndata.H.dc.2y1.tdchits]
   Int_t           Ndata_H_dc_2y1_time;
   Double_t        H_dc_2y1_time[39];   //[Ndata.H.dc.2y1.time]
   Int_t           Ndata_H_dc_2y2_dist;
   Double_t        H_dc_2y2_dist[38];   //[Ndata.H.dc.2y2.dist]
   Int_t           Ndata_H_dc_2y2_rawtdc;
   Double_t        H_dc_2y2_rawtdc[38];   //[Ndata.H.dc.2y2.rawtdc]
   Int_t           Ndata_H_dc_2y2_tdchits;
   Double_t        H_dc_2y2_tdchits[38];   //[Ndata.H.dc.2y2.tdchits]
   Int_t           Ndata_H_dc_2y2_time;
   Double_t        H_dc_2y2_time[38];   //[Ndata.H.dc.2y2.time]
   Int_t           Ndata_H_dc_residual;
   Double_t        H_dc_residual[12];   //[Ndata.H.dc.residual]
   Int_t           Ndata_H_dc_x;
   Double_t        H_dc_x[10];   //[Ndata.H.dc.x]
   Int_t           Ndata_H_dc_xp;
   Double_t        H_dc_xp[10];   //[Ndata.H.dc.xp]
   Int_t           Ndata_H_dc_y;
   Double_t        H_dc_y[10];   //[Ndata.H.dc.y]
   Int_t           Ndata_H_dc_yp;
   Double_t        H_dc_yp[10];   //[Ndata.H.dc.yp]
   Int_t           Ndata_H_hod_1x_negadchits;
   Double_t        H_hod_1x_negadchits[7];   //[Ndata.H.hod.1x.negadchits]
   Int_t           Ndata_H_hod_1x_negtdchits;
   Double_t        H_hod_1x_negtdchits[7];   //[Ndata.H.hod.1x.negtdchits]
   Int_t           Ndata_H_hod_1x_posadchits;
   Double_t        H_hod_1x_posadchits[8];   //[Ndata.H.hod.1x.posadchits]
   Int_t           Ndata_H_hod_1x_postdchits;
   Double_t        H_hod_1x_postdchits[7];   //[Ndata.H.hod.1x.postdchits]
   Int_t           Ndata_H_hod_1y_negadchits;
   Double_t        H_hod_1y_negadchits[6];   //[Ndata.H.hod.1y.negadchits]
   Int_t           Ndata_H_hod_1y_negtdchits;
   Double_t        H_hod_1y_negtdchits[6];   //[Ndata.H.hod.1y.negtdchits]
   Int_t           Ndata_H_hod_1y_posadchits;
   Double_t        H_hod_1y_posadchits[6];   //[Ndata.H.hod.1y.posadchits]
   Int_t           Ndata_H_hod_1y_postdchits;
   Double_t        H_hod_1y_postdchits[6];   //[Ndata.H.hod.1y.postdchits]
   Int_t           Ndata_H_hod_2x_negadchits;
   Double_t        H_hod_2x_negadchits[6];   //[Ndata.H.hod.2x.negadchits]
   Int_t           Ndata_H_hod_2x_negtdchits;
   Double_t        H_hod_2x_negtdchits[6];   //[Ndata.H.hod.2x.negtdchits]
   Int_t           Ndata_H_hod_2x_posadchits;
   Double_t        H_hod_2x_posadchits[6];   //[Ndata.H.hod.2x.posadchits]
   Int_t           Ndata_H_hod_2x_postdchits;
   Double_t        H_hod_2x_postdchits[6];   //[Ndata.H.hod.2x.postdchits]
   Int_t           Ndata_H_hod_2y_negadchits;
   Double_t        H_hod_2y_negadchits[6];   //[Ndata.H.hod.2y.negadchits]
   Int_t           Ndata_H_hod_2y_negtdchits;
   Double_t        H_hod_2y_negtdchits[6];   //[Ndata.H.hod.2y.negtdchits]
   Int_t           Ndata_H_hod_2y_posadchits;
   Double_t        H_hod_2y_posadchits[5];   //[Ndata.H.hod.2y.posadchits]
   Int_t           Ndata_H_hod_2y_postdchits;
   Double_t        H_hod_2y_postdchits[6];   //[Ndata.H.hod.2y.postdchits]
   Int_t           Ndata_H_hod_fpHitsTime;
   Double_t        H_hod_fpHitsTime[4];   //[Ndata.H.hod.fpHitsTime]
   Int_t           Ndata_H_tr_beta;
   Double_t        H_tr_beta[10];   //[Ndata.H.tr.beta]
   Int_t           Ndata_H_tr_betachisq;
   Double_t        H_tr_betachisq[10];   //[Ndata.H.tr.betachisq]
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
   Int_t           Ndata_S_cal_1pr_aneg;
   Double_t        S_cal_1pr_aneg[11];   //[Ndata.S.cal.1pr.aneg]
   Int_t           Ndata_S_cal_1pr_aneg_p;
   Double_t        S_cal_1pr_aneg_p[11];   //[Ndata.S.cal.1pr.aneg_p]
   Int_t           Ndata_S_cal_1pr_apos;
   Double_t        S_cal_1pr_apos[11];   //[Ndata.S.cal.1pr.apos]
   Int_t           Ndata_S_cal_1pr_apos_p;
   Double_t        S_cal_1pr_apos_p[11];   //[Ndata.S.cal.1pr.apos_p]
   Int_t           Ndata_S_cal_1pr_emean;
   Double_t        S_cal_1pr_emean[11];   //[Ndata.S.cal.1pr.emean]
   Int_t           Ndata_S_cal_1pr_eneg;
   Double_t        S_cal_1pr_eneg[11];   //[Ndata.S.cal.1pr.eneg]
   Int_t           Ndata_S_cal_1pr_epos;
   Double_t        S_cal_1pr_epos[11];   //[Ndata.S.cal.1pr.epos]
   Int_t           Ndata_S_cal_1pr_negadchits;
   Double_t        S_cal_1pr_negadchits[6];   //[Ndata.S.cal.1pr.negadchits]
   Int_t           Ndata_S_cal_1pr_posadchits;
   Double_t        S_cal_1pr_posadchits[6];   //[Ndata.S.cal.1pr.posadchits]
   Int_t           Ndata_S_cal_2ta_a;
   Double_t        S_cal_2ta_a[11];   //[Ndata.S.cal.2ta.a]
   Int_t           Ndata_S_cal_2ta_a_p;
   Double_t        S_cal_2ta_a_p[11];   //[Ndata.S.cal.2ta.a_p]
   Int_t           Ndata_S_cal_2ta_adchits;
   Double_t        S_cal_2ta_adchits[4];   //[Ndata.S.cal.2ta.adchits]
   Int_t           Ndata_S_cal_2ta_e;
   Double_t        S_cal_2ta_e[11];   //[Ndata.S.cal.2ta.e]
   Int_t           Ndata_S_cal_2ta_p;
   Double_t        S_cal_2ta_p[11];   //[Ndata.S.cal.2ta.p]
   Int_t           Ndata_S_dc_1u1_dist;
   Double_t        S_dc_1u1_dist[14];   //[Ndata.S.dc.1u1.dist]
   Int_t           Ndata_S_dc_1u1_rawtdc;
   Double_t        S_dc_1u1_rawtdc[14];   //[Ndata.S.dc.1u1.rawtdc]
   Int_t           Ndata_S_dc_1u1_tdchits;
   Double_t        S_dc_1u1_tdchits[14];   //[Ndata.S.dc.1u1.tdchits]
   Int_t           Ndata_S_dc_1u1_time;
   Double_t        S_dc_1u1_time[14];   //[Ndata.S.dc.1u1.time]
   Int_t           Ndata_S_dc_1u2_dist;
   Double_t        S_dc_1u2_dist[23];   //[Ndata.S.dc.1u2.dist]
   Int_t           Ndata_S_dc_1u2_rawtdc;
   Double_t        S_dc_1u2_rawtdc[23];   //[Ndata.S.dc.1u2.rawtdc]
   Int_t           Ndata_S_dc_1u2_tdchits;
   Double_t        S_dc_1u2_tdchits[23];   //[Ndata.S.dc.1u2.tdchits]
   Int_t           Ndata_S_dc_1u2_time;
   Double_t        S_dc_1u2_time[23];   //[Ndata.S.dc.1u2.time]
   Int_t           Ndata_S_dc_1v1_dist;
   Double_t        S_dc_1v1_dist[14];   //[Ndata.S.dc.1v1.dist]
   Int_t           Ndata_S_dc_1v1_rawtdc;
   Double_t        S_dc_1v1_rawtdc[14];   //[Ndata.S.dc.1v1.rawtdc]
   Int_t           Ndata_S_dc_1v1_tdchits;
   Double_t        S_dc_1v1_tdchits[14];   //[Ndata.S.dc.1v1.tdchits]
   Int_t           Ndata_S_dc_1v1_time;
   Double_t        S_dc_1v1_time[14];   //[Ndata.S.dc.1v1.time]
   Int_t           Ndata_S_dc_1v2_dist;
   Double_t        S_dc_1v2_dist[14];   //[Ndata.S.dc.1v2.dist]
   Int_t           Ndata_S_dc_1v2_rawtdc;
   Double_t        S_dc_1v2_rawtdc[14];   //[Ndata.S.dc.1v2.rawtdc]
   Int_t           Ndata_S_dc_1v2_tdchits;
   Double_t        S_dc_1v2_tdchits[14];   //[Ndata.S.dc.1v2.tdchits]
   Int_t           Ndata_S_dc_1v2_time;
   Double_t        S_dc_1v2_time[14];   //[Ndata.S.dc.1v2.time]
   Int_t           Ndata_S_dc_1x1_dist;
   Double_t        S_dc_1x1_dist[11];   //[Ndata.S.dc.1x1.dist]
   Int_t           Ndata_S_dc_1x1_rawtdc;
   Double_t        S_dc_1x1_rawtdc[11];   //[Ndata.S.dc.1x1.rawtdc]
   Int_t           Ndata_S_dc_1x1_tdchits;
   Double_t        S_dc_1x1_tdchits[11];   //[Ndata.S.dc.1x1.tdchits]
   Int_t           Ndata_S_dc_1x1_time;
   Double_t        S_dc_1x1_time[11];   //[Ndata.S.dc.1x1.time]
   Int_t           Ndata_S_dc_1x2_dist;
   Double_t        S_dc_1x2_dist[23];   //[Ndata.S.dc.1x2.dist]
   Int_t           Ndata_S_dc_1x2_rawtdc;
   Double_t        S_dc_1x2_rawtdc[23];   //[Ndata.S.dc.1x2.rawtdc]
   Int_t           Ndata_S_dc_1x2_tdchits;
   Double_t        S_dc_1x2_tdchits[23];   //[Ndata.S.dc.1x2.tdchits]
   Int_t           Ndata_S_dc_1x2_time;
   Double_t        S_dc_1x2_time[23];   //[Ndata.S.dc.1x2.time]
   Int_t           Ndata_S_dc_2u1_dist;
   Double_t        S_dc_2u1_dist[11];   //[Ndata.S.dc.2u1.dist]
   Int_t           Ndata_S_dc_2u1_rawtdc;
   Double_t        S_dc_2u1_rawtdc[11];   //[Ndata.S.dc.2u1.rawtdc]
   Int_t           Ndata_S_dc_2u1_tdchits;
   Double_t        S_dc_2u1_tdchits[11];   //[Ndata.S.dc.2u1.tdchits]
   Int_t           Ndata_S_dc_2u1_time;
   Double_t        S_dc_2u1_time[11];   //[Ndata.S.dc.2u1.time]
   Int_t           Ndata_S_dc_2u2_dist;
   Double_t        S_dc_2u2_dist[9];   //[Ndata.S.dc.2u2.dist]
   Int_t           Ndata_S_dc_2u2_rawtdc;
   Double_t        S_dc_2u2_rawtdc[9];   //[Ndata.S.dc.2u2.rawtdc]
   Int_t           Ndata_S_dc_2u2_tdchits;
   Double_t        S_dc_2u2_tdchits[9];   //[Ndata.S.dc.2u2.tdchits]
   Int_t           Ndata_S_dc_2u2_time;
   Double_t        S_dc_2u2_time[9];   //[Ndata.S.dc.2u2.time]
   Int_t           Ndata_S_dc_2v1_dist;
   Double_t        S_dc_2v1_dist[11];   //[Ndata.S.dc.2v1.dist]
   Int_t           Ndata_S_dc_2v1_rawtdc;
   Double_t        S_dc_2v1_rawtdc[11];   //[Ndata.S.dc.2v1.rawtdc]
   Int_t           Ndata_S_dc_2v1_tdchits;
   Double_t        S_dc_2v1_tdchits[11];   //[Ndata.S.dc.2v1.tdchits]
   Int_t           Ndata_S_dc_2v1_time;
   Double_t        S_dc_2v1_time[11];   //[Ndata.S.dc.2v1.time]
   Int_t           Ndata_S_dc_2v2_dist;
   Double_t        S_dc_2v2_dist[9];   //[Ndata.S.dc.2v2.dist]
   Int_t           Ndata_S_dc_2v2_rawtdc;
   Double_t        S_dc_2v2_rawtdc[9];   //[Ndata.S.dc.2v2.rawtdc]
   Int_t           Ndata_S_dc_2v2_tdchits;
   Double_t        S_dc_2v2_tdchits[9];   //[Ndata.S.dc.2v2.tdchits]
   Int_t           Ndata_S_dc_2v2_time;
   Double_t        S_dc_2v2_time[9];   //[Ndata.S.dc.2v2.time]
   Int_t           Ndata_S_dc_2x1_dist;
   Double_t        S_dc_2x1_dist[11];   //[Ndata.S.dc.2x1.dist]
   Int_t           Ndata_S_dc_2x1_rawtdc;
   Double_t        S_dc_2x1_rawtdc[11];   //[Ndata.S.dc.2x1.rawtdc]
   Int_t           Ndata_S_dc_2x1_tdchits;
   Double_t        S_dc_2x1_tdchits[11];   //[Ndata.S.dc.2x1.tdchits]
   Int_t           Ndata_S_dc_2x1_time;
   Double_t        S_dc_2x1_time[11];   //[Ndata.S.dc.2x1.time]
   Int_t           Ndata_S_dc_2x2_dist;
   Double_t        S_dc_2x2_dist[16];   //[Ndata.S.dc.2x2.dist]
   Int_t           Ndata_S_dc_2x2_rawtdc;
   Double_t        S_dc_2x2_rawtdc[16];   //[Ndata.S.dc.2x2.rawtdc]
   Int_t           Ndata_S_dc_2x2_tdchits;
   Double_t        S_dc_2x2_tdchits[16];   //[Ndata.S.dc.2x2.tdchits]
   Int_t           Ndata_S_dc_2x2_time;
   Double_t        S_dc_2x2_time[16];   //[Ndata.S.dc.2x2.time]
   Int_t           Ndata_S_dc_residual;
   Double_t        S_dc_residual[12];   //[Ndata.S.dc.residual]
   Int_t           Ndata_S_dc_x;
   Double_t        S_dc_x[10];   //[Ndata.S.dc.x]
   Int_t           Ndata_S_dc_xp;
   Double_t        S_dc_xp[10];   //[Ndata.S.dc.xp]
   Int_t           Ndata_S_dc_y;
   Double_t        S_dc_y[10];   //[Ndata.S.dc.y]
   Int_t           Ndata_S_dc_yp;
   Double_t        S_dc_yp[10];   //[Ndata.S.dc.yp]
   Int_t           Ndata_S_hod_1x_negadchits;
   Double_t        S_hod_1x_negadchits[4];   //[Ndata.S.hod.1x.negadchits]
   Int_t           Ndata_S_hod_1x_negtdchits;
   Double_t        S_hod_1x_negtdchits[6];   //[Ndata.S.hod.1x.negtdchits]
   Int_t           Ndata_S_hod_1x_posadchits;
   Double_t        S_hod_1x_posadchits[5];   //[Ndata.S.hod.1x.posadchits]
   Int_t           Ndata_S_hod_1x_postdchits;
   Double_t        S_hod_1x_postdchits[6];   //[Ndata.S.hod.1x.postdchits]
   Int_t           Ndata_S_hod_1y_negadchits;
   Double_t        S_hod_1y_negadchits[5];   //[Ndata.S.hod.1y.negadchits]
   Int_t           Ndata_S_hod_1y_negtdchits;
   Double_t        S_hod_1y_negtdchits[7];   //[Ndata.S.hod.1y.negtdchits]
   Int_t           Ndata_S_hod_1y_posadchits;
   Double_t        S_hod_1y_posadchits[5];   //[Ndata.S.hod.1y.posadchits]
   Int_t           Ndata_S_hod_1y_postdchits;
   Double_t        S_hod_1y_postdchits[6];   //[Ndata.S.hod.1y.postdchits]
   Int_t           Ndata_S_hod_2x_negadchits;
   Double_t        S_hod_2x_negadchits[6];   //[Ndata.S.hod.2x.negadchits]
   Int_t           Ndata_S_hod_2x_negtdchits;
   Double_t        S_hod_2x_negtdchits[6];   //[Ndata.S.hod.2x.negtdchits]
   Int_t           Ndata_S_hod_2x_posadchits;
   Double_t        S_hod_2x_posadchits[6];   //[Ndata.S.hod.2x.posadchits]
   Int_t           Ndata_S_hod_2x_postdchits;
   Double_t        S_hod_2x_postdchits[6];   //[Ndata.S.hod.2x.postdchits]
   Int_t           Ndata_S_hod_2y_negadchits;
   Double_t        S_hod_2y_negadchits[5];   //[Ndata.S.hod.2y.negadchits]
   Int_t           Ndata_S_hod_2y_negtdchits;
   Double_t        S_hod_2y_negtdchits[6];   //[Ndata.S.hod.2y.negtdchits]
   Int_t           Ndata_S_hod_2y_posadchits;
   Double_t        S_hod_2y_posadchits[5];   //[Ndata.S.hod.2y.posadchits]
   Int_t           Ndata_S_hod_2y_postdchits;
   Double_t        S_hod_2y_postdchits[6];   //[Ndata.S.hod.2y.postdchits]
   Int_t           Ndata_S_hod_fpHitsTime;
   Double_t        S_hod_fpHitsTime[4];   //[Ndata.S.hod.fpHitsTime]
   Int_t           Ndata_S_tr_beta;
   Double_t        S_tr_beta[10];   //[Ndata.S.tr.beta]
   Int_t           Ndata_S_tr_betachisq;
   Double_t        S_tr_betachisq[10];   //[Ndata.S.tr.betachisq]
   Int_t           Ndata_S_tr_chi2;
   Double_t        S_tr_chi2[10];   //[Ndata.S.tr.chi2]
   Int_t           Ndata_S_tr_d_ph;
   Double_t        S_tr_d_ph[10];   //[Ndata.S.tr.d_ph]
   Int_t           Ndata_S_tr_d_th;
   Double_t        S_tr_d_th[10];   //[Ndata.S.tr.d_th]
   Int_t           Ndata_S_tr_d_x;
   Double_t        S_tr_d_x[10];   //[Ndata.S.tr.d_x]
   Int_t           Ndata_S_tr_d_y;
   Double_t        S_tr_d_y[10];   //[Ndata.S.tr.d_y]
   Int_t           Ndata_S_tr_dbeta;
   Double_t        S_tr_dbeta[10];   //[Ndata.S.tr.dbeta]
   Int_t           Ndata_S_tr_dtime;
   Double_t        S_tr_dtime[10];   //[Ndata.S.tr.dtime]
   Int_t           Ndata_S_tr_flag;
   Double_t        S_tr_flag[10];   //[Ndata.S.tr.flag]
   Int_t           Ndata_S_tr_ndof;
   Double_t        S_tr_ndof[10];   //[Ndata.S.tr.ndof]
   Int_t           Ndata_S_tr_p;
   Double_t        S_tr_p[10];   //[Ndata.S.tr.p]
   Int_t           Ndata_S_tr_pathl;
   Double_t        S_tr_pathl[10];   //[Ndata.S.tr.pathl]
   Int_t           Ndata_S_tr_ph;
   Double_t        S_tr_ph[10];   //[Ndata.S.tr.ph]
   Int_t           Ndata_S_tr_px;
   Double_t        S_tr_px[10];   //[Ndata.S.tr.px]
   Int_t           Ndata_S_tr_py;
   Double_t        S_tr_py[10];   //[Ndata.S.tr.py]
   Int_t           Ndata_S_tr_pz;
   Double_t        S_tr_pz[10];   //[Ndata.S.tr.pz]
   Int_t           Ndata_S_tr_r_ph;
   Double_t        S_tr_r_ph[10];   //[Ndata.S.tr.r_ph]
   Int_t           Ndata_S_tr_r_th;
   Double_t        S_tr_r_th[10];   //[Ndata.S.tr.r_th]
   Int_t           Ndata_S_tr_r_x;
   Double_t        S_tr_r_x[10];   //[Ndata.S.tr.r_x]
   Int_t           Ndata_S_tr_r_y;
   Double_t        S_tr_r_y[10];   //[Ndata.S.tr.r_y]
   Int_t           Ndata_S_tr_tg_dp;
   Double_t        S_tr_tg_dp[10];   //[Ndata.S.tr.tg_dp]
   Int_t           Ndata_S_tr_tg_ph;
   Double_t        S_tr_tg_ph[10];   //[Ndata.S.tr.tg_ph]
   Int_t           Ndata_S_tr_tg_th;
   Double_t        S_tr_tg_th[10];   //[Ndata.S.tr.tg_th]
   Int_t           Ndata_S_tr_tg_y;
   Double_t        S_tr_tg_y[10];   //[Ndata.S.tr.tg_y]
   Int_t           Ndata_S_tr_th;
   Double_t        S_tr_th[10];   //[Ndata.S.tr.th]
   Int_t           Ndata_S_tr_time;
   Double_t        S_tr_time[10];   //[Ndata.S.tr.time]
   Int_t           Ndata_S_tr_vx;
   Double_t        S_tr_vx[10];   //[Ndata.S.tr.vx]
   Int_t           Ndata_S_tr_vy;
   Double_t        S_tr_vy[10];   //[Ndata.S.tr.vy]
   Int_t           Ndata_S_tr_vz;
   Double_t        S_tr_vz[10];   //[Ndata.S.tr.vz]
   Int_t           Ndata_S_tr_x;
   Double_t        S_tr_x[10];   //[Ndata.S.tr.x]
   Int_t           Ndata_S_tr_y;
   Double_t        S_tr_y[10];   //[Ndata.S.tr.y]
   Double_t        H_aero_neg_npe_sum;
   Double_t        H_aero_ngood_hits;
   Double_t        H_aero_npe_sum;
   Double_t        H_aero_ntdc_neg_hits;
   Double_t        H_aero_ntdc_pos_hits;
   Double_t        H_aero_pos_npe_sum;
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
   Double_t        H_cal_etot;
   Double_t        H_cal_etotnorm;
   Double_t        H_cal_etrack;
   Double_t        H_cal_nclust;
   Double_t        H_cal_nhits;
   Double_t        H_cal_ntracks;
   Double_t        H_cher_ncherhit;
   Double_t        H_cher_npesum;
   Double_t        H_dc_1u1_nhit;
   Double_t        H_dc_1v1_nhit;
   Double_t        H_dc_1x1_nhit;
   Double_t        H_dc_1x2_nhit;
   Double_t        H_dc_1y1_nhit;
   Double_t        H_dc_1y2_nhit;
   Double_t        H_dc_2u1_nhit;
   Double_t        H_dc_2v1_nhit;
   Double_t        H_dc_2x1_nhit;
   Double_t        H_dc_2x2_nhit;
   Double_t        H_dc_2y1_nhit;
   Double_t        H_dc_2y2_nhit;
   Double_t        H_dc_Ch1_maxhits;
   Double_t        H_dc_Ch1_nhit;
   Double_t        H_dc_Ch1_spacepoints;
   Double_t        H_dc_Ch1_trawhit;
   Double_t        H_dc_Ch2_maxhits;
   Double_t        H_dc_Ch2_nhit;
   Double_t        H_dc_Ch2_spacepoints;
   Double_t        H_dc_Ch2_trawhit;
   Double_t        H_dc_nhit;
   Double_t        H_dc_nsp;
   Double_t        H_dc_ntrack;
   Double_t        H_dc_stubtest;
   Double_t        H_dc_tnhit;
   Double_t        H_dc_trawhit;
   Double_t        H_hod_1x_nhits;
   Double_t        H_hod_1y_nhits;
   Double_t        H_hod_2x_nhits;
   Double_t        H_hod_2y_nhits;
   Double_t        H_hod_betachisqnotrack;
   Double_t        H_hod_betanotrack;
   Double_t        H_hod_betap;
   Double_t        H_hod_goodscinhit;
   Double_t        H_hod_goodstarttime;
   Double_t        H_hod_scindid;
   Double_t        H_hod_scinshould;
   Double_t        H_hod_starttime;
   Double_t        H_tr_n;
   Double_t        HSCOINPRE;
   Double_t        HSCOINPREr;
   Double_t        HSCOINTRG;
   Double_t        HSCOINTRGr;
   Double_t        HSHMSPRE;
   Double_t        HSHMSPREr;
   Double_t        HSHMSTRG;
   Double_t        HSHMSTRGr;
   Double_t        HSPEDPRE;
   Double_t        HSPEDPREr;
   Double_t        HSPEDTRG;
   Double_t        HSPEDTRGr;
   Double_t        HSS1;
   Double_t        HSS1X;
   Double_t        HSS1Xr;
   Double_t        HSS1Y;
   Double_t        HSS1Yr;
   Double_t        HSS1r;
   Double_t        HSS2;
   Double_t        HSS2X;
   Double_t        HSS2Xr;
   Double_t        HSS2Y;
   Double_t        HSS2Yr;
   Double_t        HSS2r;
   Double_t        HSSHODO11;
   Double_t        HSSHODO11r;
   Double_t        HSSHODO13;
   Double_t        HSSHODO13r;
   Double_t        HSSHODO15;
   Double_t        HSSHODO15r;
   Double_t        HSSHODO9;
   Double_t        HSSHODO9r;
   Double_t        HSSOSPRE;
   Double_t        HSSOSPREr;
   Double_t        HSSOSTRG;
   Double_t        HSSOSTRGr;
   Double_t        HShADCGATES;
   Double_t        HShADCGATESr;
   Double_t        HShBCM1;
   Double_t        HShBCM1r;
   Double_t        HShBCM2;
   Double_t        HShBCM2r;
   Double_t        HShBEAMGATES;
   Double_t        HShBEAMGATESr;
   Double_t        HShCER;
   Double_t        HShCERHI;
   Double_t        HShCERHIr;
   Double_t        HShCERr;
   Double_t        HShCLOCK;
   Double_t        HShCLOCKr;
   Double_t        HShELCLEAN;
   Double_t        HShELCLEANr;
   Double_t        HShELHI;
   Double_t        HShELHIr;
   Double_t        HShELLO;
   Double_t        HShELLOr;
   Double_t        HShELREAL;
   Double_t        HShELREALr;
   Double_t        HShPION;
   Double_t        HShPIONHI;
   Double_t        HShPIONHIr;
   Double_t        HShPIONr;
   Double_t        HShPIPRE;
   Double_t        HShPIPREr;
   Double_t        HShPRE100;
   Double_t        HShPRE100r;
   Double_t        HShPRE150;
   Double_t        HShPRE150r;
   Double_t        HShPRE200;
   Double_t        HShPRE200r;
   Double_t        HShPRE50;
   Double_t        HShPRE50r;
   Double_t        HShPRETRG;
   Double_t        HShPRETRGr;
   Double_t        HShPRHI;
   Double_t        HShPRHIr;
   Double_t        HShPRLO;
   Double_t        HShPRLOr;
   Double_t        HShS1;
   Double_t        HShS1X;
   Double_t        HShS1Xr;
   Double_t        HShS1Y;
   Double_t        HShS1Yr;
   Double_t        HShS1r;
   Double_t        HShS2;
   Double_t        HShS2X;
   Double_t        HShS2Xr;
   Double_t        HShS2Y;
   Double_t        HShS2Yr;
   Double_t        HShS2r;
   Double_t        HShSCIN;
   Double_t        HShSCINr;
   Double_t        HShSHLO;
   Double_t        HShSHLOr;
   Double_t        HShSTOF;
   Double_t        HShSTOFr;
   Double_t        HShUNSER;
   Double_t        HShUNSERr;
   Double_t        HShUNUSED4;
   Double_t        HShUNUSED4r;
   Double_t        HShUNUSED5;
   Double_t        HShUNUSED5r;
   Double_t        HSsADCGATES;
   Double_t        HSsADCGATESr;
   Double_t        HSsBCM1;
   Double_t        HSsBCM1r;
   Double_t        HSsBCM2;
   Double_t        HSsBCM2r;
   Double_t        HSsCER;
   Double_t        HSsCERr;
   Double_t        HSsCLOCK;
   Double_t        HSsCLOCKr;
   Double_t        HSsELCLEAN;
   Double_t        HSsELCLEANr;
   Double_t        HSsELHI;
   Double_t        HSsELHIr;
   Double_t        HSsELLO;
   Double_t        HSsELLOr;
   Double_t        HSsELREAL;
   Double_t        HSsELREALr;
   Double_t        HSsPION;
   Double_t        HSsPIONr;
   Double_t        HSsPIPRE;
   Double_t        HSsPIPREr;
   Double_t        HSsPRE100;
   Double_t        HSsPRE100r;
   Double_t        HSsPRE150;
   Double_t        HSsPRE150r;
   Double_t        HSsPRE200;
   Double_t        HSsPRE200r;
   Double_t        HSsPRE50;
   Double_t        HSsPRE50r;
   Double_t        HSsPRETRG;
   Double_t        HSsPRETRGr;
   Double_t        HSsPRHI;
   Double_t        HSsPRHIr;
   Double_t        HSsPRLO;
   Double_t        HSsPRLOr;
   Double_t        HSsS1;
   Double_t        HSsS1X;
   Double_t        HSsS1Xr;
   Double_t        HSsS1Y;
   Double_t        HSsS1Yr;
   Double_t        HSsS1r;
   Double_t        HSsS2;
   Double_t        HSsS2X;
   Double_t        HSsS2Xr;
   Double_t        HSsS2Y;
   Double_t        HSsS2Yr;
   Double_t        HSsS2r;
   Double_t        HSsSCIN;
   Double_t        HSsSCINr;
   Double_t        HSsSHLO;
   Double_t        HSsSHLOr;
   Double_t        HSsSTOF;
   Double_t        HSsSTOFr;
   Double_t        S_cal_1pr_eplane;
   Double_t        S_cal_1pr_eplane_neg;
   Double_t        S_cal_1pr_eplane_pos;
   Double_t        S_cal_2ta_earray;
   Double_t        S_cal_2ta_nclust;
   Double_t        S_cal_2ta_nhits;
   Double_t        S_cal_2ta_ntracks;
   Double_t        S_cal_etot;
   Double_t        S_cal_etotnorm;
   Double_t        S_cal_etrack;
   Double_t        S_cal_nclust;
   Double_t        S_cal_nhits;
   Double_t        S_cal_ntracks;
   Double_t        S_dc_1u1_nhit;
   Double_t        S_dc_1u2_nhit;
   Double_t        S_dc_1v1_nhit;
   Double_t        S_dc_1v2_nhit;
   Double_t        S_dc_1x1_nhit;
   Double_t        S_dc_1x2_nhit;
   Double_t        S_dc_2u1_nhit;
   Double_t        S_dc_2u2_nhit;
   Double_t        S_dc_2v1_nhit;
   Double_t        S_dc_2v2_nhit;
   Double_t        S_dc_2x1_nhit;
   Double_t        S_dc_2x2_nhit;
   Double_t        S_dc_Ch1_maxhits;
   Double_t        S_dc_Ch1_nhit;
   Double_t        S_dc_Ch1_spacepoints;
   Double_t        S_dc_Ch1_trawhit;
   Double_t        S_dc_Ch2_maxhits;
   Double_t        S_dc_Ch2_nhit;
   Double_t        S_dc_Ch2_spacepoints;
   Double_t        S_dc_Ch2_trawhit;
   Double_t        S_dc_nhit;
   Double_t        S_dc_nsp;
   Double_t        S_dc_ntrack;
   Double_t        S_dc_stubtest;
   Double_t        S_dc_tnhit;
   Double_t        S_dc_trawhit;
   Double_t        S_hod_1x_nhits;
   Double_t        S_hod_1y_nhits;
   Double_t        S_hod_2x_nhits;
   Double_t        S_hod_2y_nhits;
   Double_t        S_hod_betachisqnotrack;
   Double_t        S_hod_betanotrack;
   Double_t        S_hod_betap;
   Double_t        S_hod_goodscinhit;
   Double_t        S_hod_goodstarttime;
   Double_t        S_hod_scindid;
   Double_t        S_hod_scinshould;
   Double_t        S_hod_starttime;
   Double_t        S_tr_n;
   Double_t        g_evtyp;
   Double_t        Hhodtdif1;
   Double_t        Hhodtdif2;
   Double_t        Hhodtdif3;
   Double_t        Hhodtdif4;
   Double_t        Hhodtdif5;
   Double_t        Hhodtdif6;
 //THaEvent        *Event_Branch;
   ULong64_t       fEvtHdr_fEvtTime;
   UInt_t          fEvtHdr_fEvtNum;
   Int_t           fEvtHdr_fEvtType;
   Int_t           fEvtHdr_fEvtLen;
   Int_t           fEvtHdr_fHelicity;
   Int_t           fEvtHdr_fTargetPol;
   Int_t           fEvtHdr_fRun;

   // List of branches
   TBranch        *b_Hhodtdif1;   //!
   TBranch        *b_Hhodtdif2;   //!
   TBranch        *b_Hhodtdif3;   //!
   TBranch        *b_Hhodtdif4;   //!
   TBranch        *b_Hhodtdif5;   //!
   TBranch        *b_Hhodtdif6;   //!
   TBranch        *b_Ndata_H_aero_aneg;   //!
   TBranch        *b_H_aero_aneg;   //!
   TBranch        *b_Ndata_H_aero_aneg_p;   //!
   TBranch        *b_H_aero_aneg_p;   //!
   TBranch        *b_Ndata_H_aero_apos;   //!
   TBranch        *b_H_aero_apos;   //!
   TBranch        *b_Ndata_H_aero_apos_p;   //!
   TBranch        *b_H_aero_apos_p;   //!
   TBranch        *b_Ndata_H_aero_neg_npe;   //!
   TBranch        *b_H_aero_neg_npe;   //!
   TBranch        *b_Ndata_H_aero_negadchits;   //!
   TBranch        *b_H_aero_negadchits;   //!
   TBranch        *b_Ndata_H_aero_negtdchits;   //!
   TBranch        *b_H_aero_negtdchits;   //!
   TBranch        *b_Ndata_H_aero_pos_npe;   //!
   TBranch        *b_H_aero_pos_npe;   //!
   TBranch        *b_Ndata_H_aero_posadchits;   //!
   TBranch        *b_H_aero_posadchits;   //!
   TBranch        *b_Ndata_H_aero_postdchits;   //!
   TBranch        *b_H_aero_postdchits;   //!
   TBranch        *b_Ndata_H_aero_tneg;   //!
   TBranch        *b_H_aero_tneg;   //!
   TBranch        *b_Ndata_H_aero_tpos;   //!
   TBranch        *b_H_aero_tpos;   //!
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
   TBranch        *b_Ndata_H_cher_adc;   //!
   TBranch        *b_H_cher_adc;   //!
   TBranch        *b_Ndata_H_cher_adc_p;   //!
   TBranch        *b_H_cher_adc_p;   //!
   TBranch        *b_Ndata_H_cher_cerfiredcounter;   //!
   TBranch        *b_H_cher_cerfiredcounter;   //!
   TBranch        *b_Ndata_H_cher_certrackcounter;   //!
   TBranch        *b_H_cher_certrackcounter;   //!
   TBranch        *b_Ndata_H_cher_npe;   //!
   TBranch        *b_H_cher_npe;   //!
   TBranch        *b_Ndata_H_cher_phototubes;   //!
   TBranch        *b_H_cher_phototubes;   //!
   TBranch        *b_Ndata_H_dc_1u1_dist;   //!
   TBranch        *b_H_dc_1u1_dist;   //!
   TBranch        *b_Ndata_H_dc_1u1_rawtdc;   //!
   TBranch        *b_H_dc_1u1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1u1_tdchits;   //!
   TBranch        *b_H_dc_1u1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1u1_time;   //!
   TBranch        *b_H_dc_1u1_time;   //!
   TBranch        *b_Ndata_H_dc_1v1_dist;   //!
   TBranch        *b_H_dc_1v1_dist;   //!
   TBranch        *b_Ndata_H_dc_1v1_rawtdc;   //!
   TBranch        *b_H_dc_1v1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1v1_tdchits;   //!
   TBranch        *b_H_dc_1v1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1v1_time;   //!
   TBranch        *b_H_dc_1v1_time;   //!
   TBranch        *b_Ndata_H_dc_1x1_dist;   //!
   TBranch        *b_H_dc_1x1_dist;   //!
   TBranch        *b_Ndata_H_dc_1x1_rawtdc;   //!
   TBranch        *b_H_dc_1x1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1x1_tdchits;   //!
   TBranch        *b_H_dc_1x1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1x1_time;   //!
   TBranch        *b_H_dc_1x1_time;   //!
   TBranch        *b_Ndata_H_dc_1x2_dist;   //!
   TBranch        *b_H_dc_1x2_dist;   //!
   TBranch        *b_Ndata_H_dc_1x2_rawtdc;   //!
   TBranch        *b_H_dc_1x2_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1x2_tdchits;   //!
   TBranch        *b_H_dc_1x2_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1x2_time;   //!
   TBranch        *b_H_dc_1x2_time;   //!
   TBranch        *b_Ndata_H_dc_1y1_dist;   //!
   TBranch        *b_H_dc_1y1_dist;   //!
   TBranch        *b_Ndata_H_dc_1y1_rawtdc;   //!
   TBranch        *b_H_dc_1y1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1y1_tdchits;   //!
   TBranch        *b_H_dc_1y1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1y1_time;   //!
   TBranch        *b_H_dc_1y1_time;   //!
   TBranch        *b_Ndata_H_dc_1y2_dist;   //!
   TBranch        *b_H_dc_1y2_dist;   //!
   TBranch        *b_Ndata_H_dc_1y2_rawtdc;   //!
   TBranch        *b_H_dc_1y2_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_1y2_tdchits;   //!
   TBranch        *b_H_dc_1y2_tdchits;   //!
   TBranch        *b_Ndata_H_dc_1y2_time;   //!
   TBranch        *b_H_dc_1y2_time;   //!
   TBranch        *b_Ndata_H_dc_2u1_dist;   //!
   TBranch        *b_H_dc_2u1_dist;   //!
   TBranch        *b_Ndata_H_dc_2u1_rawtdc;   //!
   TBranch        *b_H_dc_2u1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2u1_tdchits;   //!
   TBranch        *b_H_dc_2u1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2u1_time;   //!
   TBranch        *b_H_dc_2u1_time;   //!
   TBranch        *b_Ndata_H_dc_2v1_dist;   //!
   TBranch        *b_H_dc_2v1_dist;   //!
   TBranch        *b_Ndata_H_dc_2v1_rawtdc;   //!
   TBranch        *b_H_dc_2v1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2v1_tdchits;   //!
   TBranch        *b_H_dc_2v1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2v1_time;   //!
   TBranch        *b_H_dc_2v1_time;   //!
   TBranch        *b_Ndata_H_dc_2x1_dist;   //!
   TBranch        *b_H_dc_2x1_dist;   //!
   TBranch        *b_Ndata_H_dc_2x1_rawtdc;   //!
   TBranch        *b_H_dc_2x1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2x1_tdchits;   //!
   TBranch        *b_H_dc_2x1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2x1_time;   //!
   TBranch        *b_H_dc_2x1_time;   //!
   TBranch        *b_Ndata_H_dc_2x2_dist;   //!
   TBranch        *b_H_dc_2x2_dist;   //!
   TBranch        *b_Ndata_H_dc_2x2_rawtdc;   //!
   TBranch        *b_H_dc_2x2_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2x2_tdchits;   //!
   TBranch        *b_H_dc_2x2_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2x2_time;   //!
   TBranch        *b_H_dc_2x2_time;   //!
   TBranch        *b_Ndata_H_dc_2y1_dist;   //!
   TBranch        *b_H_dc_2y1_dist;   //!
   TBranch        *b_Ndata_H_dc_2y1_rawtdc;   //!
   TBranch        *b_H_dc_2y1_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2y1_tdchits;   //!
   TBranch        *b_H_dc_2y1_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2y1_time;   //!
   TBranch        *b_H_dc_2y1_time;   //!
   TBranch        *b_Ndata_H_dc_2y2_dist;   //!
   TBranch        *b_H_dc_2y2_dist;   //!
   TBranch        *b_Ndata_H_dc_2y2_rawtdc;   //!
   TBranch        *b_H_dc_2y2_rawtdc;   //!
   TBranch        *b_Ndata_H_dc_2y2_tdchits;   //!
   TBranch        *b_H_dc_2y2_tdchits;   //!
   TBranch        *b_Ndata_H_dc_2y2_time;   //!
   TBranch        *b_H_dc_2y2_time;   //!
   TBranch        *b_Ndata_H_dc_residual;   //!
   TBranch        *b_H_dc_residual;   //!
   TBranch        *b_Ndata_H_dc_x;   //!
   TBranch        *b_H_dc_x;   //!
   TBranch        *b_Ndata_H_dc_xp;   //!
   TBranch        *b_H_dc_xp;   //!
   TBranch        *b_Ndata_H_dc_y;   //!
   TBranch        *b_H_dc_y;   //!
   TBranch        *b_Ndata_H_dc_yp;   //!
   TBranch        *b_H_dc_yp;   //!
   TBranch        *b_Ndata_H_hod_1x_negadchits;   //!
   TBranch        *b_H_hod_1x_negadchits;   //!
   TBranch        *b_Ndata_H_hod_1x_negtdchits;   //!
   TBranch        *b_H_hod_1x_negtdchits;   //!
   TBranch        *b_Ndata_H_hod_1x_posadchits;   //!
   TBranch        *b_H_hod_1x_posadchits;   //!
   TBranch        *b_Ndata_H_hod_1x_postdchits;   //!
   TBranch        *b_H_hod_1x_postdchits;   //!
   TBranch        *b_Ndata_H_hod_1y_negadchits;   //!
   TBranch        *b_H_hod_1y_negadchits;   //!
   TBranch        *b_Ndata_H_hod_1y_negtdchits;   //!
   TBranch        *b_H_hod_1y_negtdchits;   //!
   TBranch        *b_Ndata_H_hod_1y_posadchits;   //!
   TBranch        *b_H_hod_1y_posadchits;   //!
   TBranch        *b_Ndata_H_hod_1y_postdchits;   //!
   TBranch        *b_H_hod_1y_postdchits;   //!
   TBranch        *b_Ndata_H_hod_2x_negadchits;   //!
   TBranch        *b_H_hod_2x_negadchits;   //!
   TBranch        *b_Ndata_H_hod_2x_negtdchits;   //!
   TBranch        *b_H_hod_2x_negtdchits;   //!
   TBranch        *b_Ndata_H_hod_2x_posadchits;   //!
   TBranch        *b_H_hod_2x_posadchits;   //!
   TBranch        *b_Ndata_H_hod_2x_postdchits;   //!
   TBranch        *b_H_hod_2x_postdchits;   //!
   TBranch        *b_Ndata_H_hod_2y_negadchits;   //!
   TBranch        *b_H_hod_2y_negadchits;   //!
   TBranch        *b_Ndata_H_hod_2y_negtdchits;   //!
   TBranch        *b_H_hod_2y_negtdchits;   //!
   TBranch        *b_Ndata_H_hod_2y_posadchits;   //!
   TBranch        *b_H_hod_2y_posadchits;   //!
   TBranch        *b_Ndata_H_hod_2y_postdchits;   //!
   TBranch        *b_H_hod_2y_postdchits;   //!
   TBranch        *b_Ndata_H_hod_fpHitsTime;   //!
   TBranch        *b_H_hod_fpHitsTime;   //!
   TBranch        *b_Ndata_H_tr_beta;   //!
   TBranch        *b_H_tr_beta;   //!
   TBranch        *b_Ndata_H_tr_betachisq;   //!
   TBranch        *b_H_tr_betachisq;   //!
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
   TBranch        *b_Ndata_S_cal_1pr_aneg;   //!
   TBranch        *b_S_cal_1pr_aneg;   //!
   TBranch        *b_Ndata_S_cal_1pr_aneg_p;   //!
   TBranch        *b_S_cal_1pr_aneg_p;   //!
   TBranch        *b_Ndata_S_cal_1pr_apos;   //!
   TBranch        *b_S_cal_1pr_apos;   //!
   TBranch        *b_Ndata_S_cal_1pr_apos_p;   //!
   TBranch        *b_S_cal_1pr_apos_p;   //!
   TBranch        *b_Ndata_S_cal_1pr_emean;   //!
   TBranch        *b_S_cal_1pr_emean;   //!
   TBranch        *b_Ndata_S_cal_1pr_eneg;   //!
   TBranch        *b_S_cal_1pr_eneg;   //!
   TBranch        *b_Ndata_S_cal_1pr_epos;   //!
   TBranch        *b_S_cal_1pr_epos;   //!
   TBranch        *b_Ndata_S_cal_1pr_negadchits;   //!
   TBranch        *b_S_cal_1pr_negadchits;   //!
   TBranch        *b_Ndata_S_cal_1pr_posadchits;   //!
   TBranch        *b_S_cal_1pr_posadchits;   //!
   TBranch        *b_Ndata_S_cal_2ta_a;   //!
   TBranch        *b_S_cal_2ta_a;   //!
   TBranch        *b_Ndata_S_cal_2ta_a_p;   //!
   TBranch        *b_S_cal_2ta_a_p;   //!
   TBranch        *b_Ndata_S_cal_2ta_adchits;   //!
   TBranch        *b_S_cal_2ta_adchits;   //!
   TBranch        *b_Ndata_S_cal_2ta_e;   //!
   TBranch        *b_S_cal_2ta_e;   //!
   TBranch        *b_Ndata_S_cal_2ta_p;   //!
   TBranch        *b_S_cal_2ta_p;   //!
   TBranch        *b_Ndata_S_dc_1u1_dist;   //!
   TBranch        *b_S_dc_1u1_dist;   //!
   TBranch        *b_Ndata_S_dc_1u1_rawtdc;   //!
   TBranch        *b_S_dc_1u1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1u1_tdchits;   //!
   TBranch        *b_S_dc_1u1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1u1_time;   //!
   TBranch        *b_S_dc_1u1_time;   //!
   TBranch        *b_Ndata_S_dc_1u2_dist;   //!
   TBranch        *b_S_dc_1u2_dist;   //!
   TBranch        *b_Ndata_S_dc_1u2_rawtdc;   //!
   TBranch        *b_S_dc_1u2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1u2_tdchits;   //!
   TBranch        *b_S_dc_1u2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1u2_time;   //!
   TBranch        *b_S_dc_1u2_time;   //!
   TBranch        *b_Ndata_S_dc_1v1_dist;   //!
   TBranch        *b_S_dc_1v1_dist;   //!
   TBranch        *b_Ndata_S_dc_1v1_rawtdc;   //!
   TBranch        *b_S_dc_1v1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1v1_tdchits;   //!
   TBranch        *b_S_dc_1v1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1v1_time;   //!
   TBranch        *b_S_dc_1v1_time;   //!
   TBranch        *b_Ndata_S_dc_1v2_dist;   //!
   TBranch        *b_S_dc_1v2_dist;   //!
   TBranch        *b_Ndata_S_dc_1v2_rawtdc;   //!
   TBranch        *b_S_dc_1v2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1v2_tdchits;   //!
   TBranch        *b_S_dc_1v2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1v2_time;   //!
   TBranch        *b_S_dc_1v2_time;   //!
   TBranch        *b_Ndata_S_dc_1x1_dist;   //!
   TBranch        *b_S_dc_1x1_dist;   //!
   TBranch        *b_Ndata_S_dc_1x1_rawtdc;   //!
   TBranch        *b_S_dc_1x1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1x1_tdchits;   //!
   TBranch        *b_S_dc_1x1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1x1_time;   //!
   TBranch        *b_S_dc_1x1_time;   //!
   TBranch        *b_Ndata_S_dc_1x2_dist;   //!
   TBranch        *b_S_dc_1x2_dist;   //!
   TBranch        *b_Ndata_S_dc_1x2_rawtdc;   //!
   TBranch        *b_S_dc_1x2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_1x2_tdchits;   //!
   TBranch        *b_S_dc_1x2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_1x2_time;   //!
   TBranch        *b_S_dc_1x2_time;   //!
   TBranch        *b_Ndata_S_dc_2u1_dist;   //!
   TBranch        *b_S_dc_2u1_dist;   //!
   TBranch        *b_Ndata_S_dc_2u1_rawtdc;   //!
   TBranch        *b_S_dc_2u1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2u1_tdchits;   //!
   TBranch        *b_S_dc_2u1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2u1_time;   //!
   TBranch        *b_S_dc_2u1_time;   //!
   TBranch        *b_Ndata_S_dc_2u2_dist;   //!
   TBranch        *b_S_dc_2u2_dist;   //!
   TBranch        *b_Ndata_S_dc_2u2_rawtdc;   //!
   TBranch        *b_S_dc_2u2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2u2_tdchits;   //!
   TBranch        *b_S_dc_2u2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2u2_time;   //!
   TBranch        *b_S_dc_2u2_time;   //!
   TBranch        *b_Ndata_S_dc_2v1_dist;   //!
   TBranch        *b_S_dc_2v1_dist;   //!
   TBranch        *b_Ndata_S_dc_2v1_rawtdc;   //!
   TBranch        *b_S_dc_2v1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2v1_tdchits;   //!
   TBranch        *b_S_dc_2v1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2v1_time;   //!
   TBranch        *b_S_dc_2v1_time;   //!
   TBranch        *b_Ndata_S_dc_2v2_dist;   //!
   TBranch        *b_S_dc_2v2_dist;   //!
   TBranch        *b_Ndata_S_dc_2v2_rawtdc;   //!
   TBranch        *b_S_dc_2v2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2v2_tdchits;   //!
   TBranch        *b_S_dc_2v2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2v2_time;   //!
   TBranch        *b_S_dc_2v2_time;   //!
   TBranch        *b_Ndata_S_dc_2x1_dist;   //!
   TBranch        *b_S_dc_2x1_dist;   //!
   TBranch        *b_Ndata_S_dc_2x1_rawtdc;   //!
   TBranch        *b_S_dc_2x1_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2x1_tdchits;   //!
   TBranch        *b_S_dc_2x1_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2x1_time;   //!
   TBranch        *b_S_dc_2x1_time;   //!
   TBranch        *b_Ndata_S_dc_2x2_dist;   //!
   TBranch        *b_S_dc_2x2_dist;   //!
   TBranch        *b_Ndata_S_dc_2x2_rawtdc;   //!
   TBranch        *b_S_dc_2x2_rawtdc;   //!
   TBranch        *b_Ndata_S_dc_2x2_tdchits;   //!
   TBranch        *b_S_dc_2x2_tdchits;   //!
   TBranch        *b_Ndata_S_dc_2x2_time;   //!
   TBranch        *b_S_dc_2x2_time;   //!
   TBranch        *b_Ndata_S_dc_residual;   //!
   TBranch        *b_S_dc_residual;   //!
   TBranch        *b_Ndata_S_dc_x;   //!
   TBranch        *b_S_dc_x;   //!
   TBranch        *b_Ndata_S_dc_xp;   //!
   TBranch        *b_S_dc_xp;   //!
   TBranch        *b_Ndata_S_dc_y;   //!
   TBranch        *b_S_dc_y;   //!
   TBranch        *b_Ndata_S_dc_yp;   //!
   TBranch        *b_S_dc_yp;   //!
   TBranch        *b_Ndata_S_hod_1x_negadchits;   //!
   TBranch        *b_S_hod_1x_negadchits;   //!
   TBranch        *b_Ndata_S_hod_1x_negtdchits;   //!
   TBranch        *b_S_hod_1x_negtdchits;   //!
   TBranch        *b_Ndata_S_hod_1x_posadchits;   //!
   TBranch        *b_S_hod_1x_posadchits;   //!
   TBranch        *b_Ndata_S_hod_1x_postdchits;   //!
   TBranch        *b_S_hod_1x_postdchits;   //!
   TBranch        *b_Ndata_S_hod_1y_negadchits;   //!
   TBranch        *b_S_hod_1y_negadchits;   //!
   TBranch        *b_Ndata_S_hod_1y_negtdchits;   //!
   TBranch        *b_S_hod_1y_negtdchits;   //!
   TBranch        *b_Ndata_S_hod_1y_posadchits;   //!
   TBranch        *b_S_hod_1y_posadchits;   //!
   TBranch        *b_Ndata_S_hod_1y_postdchits;   //!
   TBranch        *b_S_hod_1y_postdchits;   //!
   TBranch        *b_Ndata_S_hod_2x_negadchits;   //!
   TBranch        *b_S_hod_2x_negadchits;   //!
   TBranch        *b_Ndata_S_hod_2x_negtdchits;   //!
   TBranch        *b_S_hod_2x_negtdchits;   //!
   TBranch        *b_Ndata_S_hod_2x_posadchits;   //!
   TBranch        *b_S_hod_2x_posadchits;   //!
   TBranch        *b_Ndata_S_hod_2x_postdchits;   //!
   TBranch        *b_S_hod_2x_postdchits;   //!
   TBranch        *b_Ndata_S_hod_2y_negadchits;   //!
   TBranch        *b_S_hod_2y_negadchits;   //!
   TBranch        *b_Ndata_S_hod_2y_negtdchits;   //!
   TBranch        *b_S_hod_2y_negtdchits;   //!
   TBranch        *b_Ndata_S_hod_2y_posadchits;   //!
   TBranch        *b_S_hod_2y_posadchits;   //!
   TBranch        *b_Ndata_S_hod_2y_postdchits;   //!
   TBranch        *b_S_hod_2y_postdchits;   //!
   TBranch        *b_Ndata_S_hod_fpHitsTime;   //!
   TBranch        *b_S_hod_fpHitsTime;   //!
   TBranch        *b_Ndata_S_tr_beta;   //!
   TBranch        *b_S_tr_beta;   //!
   TBranch        *b_Ndata_S_tr_betachisq;   //!
   TBranch        *b_S_tr_betachisq;   //!
   TBranch        *b_Ndata_S_tr_chi2;   //!
   TBranch        *b_S_tr_chi2;   //!
   TBranch        *b_Ndata_S_tr_d_ph;   //!
   TBranch        *b_S_tr_d_ph;   //!
   TBranch        *b_Ndata_S_tr_d_th;   //!
   TBranch        *b_S_tr_d_th;   //!
   TBranch        *b_Ndata_S_tr_d_x;   //!
   TBranch        *b_S_tr_d_x;   //!
   TBranch        *b_Ndata_S_tr_d_y;   //!
   TBranch        *b_S_tr_d_y;   //!
   TBranch        *b_Ndata_S_tr_dbeta;   //!
   TBranch        *b_S_tr_dbeta;   //!
   TBranch        *b_Ndata_S_tr_dtime;   //!
   TBranch        *b_S_tr_dtime;   //!
   TBranch        *b_Ndata_S_tr_flag;   //!
   TBranch        *b_S_tr_flag;   //!
   TBranch        *b_Ndata_S_tr_ndof;   //!
   TBranch        *b_S_tr_ndof;   //!
   TBranch        *b_Ndata_S_tr_p;   //!
   TBranch        *b_S_tr_p;   //!
   TBranch        *b_Ndata_S_tr_pathl;   //!
   TBranch        *b_S_tr_pathl;   //!
   TBranch        *b_Ndata_S_tr_ph;   //!
   TBranch        *b_S_tr_ph;   //!
   TBranch        *b_Ndata_S_tr_px;   //!
   TBranch        *b_S_tr_px;   //!
   TBranch        *b_Ndata_S_tr_py;   //!
   TBranch        *b_S_tr_py;   //!
   TBranch        *b_Ndata_S_tr_pz;   //!
   TBranch        *b_S_tr_pz;   //!
   TBranch        *b_Ndata_S_tr_r_ph;   //!
   TBranch        *b_S_tr_r_ph;   //!
   TBranch        *b_Ndata_S_tr_r_th;   //!
   TBranch        *b_S_tr_r_th;   //!
   TBranch        *b_Ndata_S_tr_r_x;   //!
   TBranch        *b_S_tr_r_x;   //!
   TBranch        *b_Ndata_S_tr_r_y;   //!
   TBranch        *b_S_tr_r_y;   //!
   TBranch        *b_Ndata_S_tr_tg_dp;   //!
   TBranch        *b_S_tr_tg_dp;   //!
   TBranch        *b_Ndata_S_tr_tg_ph;   //!
   TBranch        *b_S_tr_tg_ph;   //!
   TBranch        *b_Ndata_S_tr_tg_th;   //!
   TBranch        *b_S_tr_tg_th;   //!
   TBranch        *b_Ndata_S_tr_tg_y;   //!
   TBranch        *b_S_tr_tg_y;   //!
   TBranch        *b_Ndata_S_tr_th;   //!
   TBranch        *b_S_tr_th;   //!
   TBranch        *b_Ndata_S_tr_time;   //!
   TBranch        *b_S_tr_time;   //!
   TBranch        *b_Ndata_S_tr_vx;   //!
   TBranch        *b_S_tr_vx;   //!
   TBranch        *b_Ndata_S_tr_vy;   //!
   TBranch        *b_S_tr_vy;   //!
   TBranch        *b_Ndata_S_tr_vz;   //!
   TBranch        *b_S_tr_vz;   //!
   TBranch        *b_Ndata_S_tr_x;   //!
   TBranch        *b_S_tr_x;   //!
   TBranch        *b_Ndata_S_tr_y;   //!
   TBranch        *b_S_tr_y;   //!
   TBranch        *b_H_aero_neg_npe_sum;   //!
   TBranch        *b_H_aero_ngood_hits;   //!
   TBranch        *b_H_aero_npe_sum;   //!
   TBranch        *b_H_aero_ntdc_neg_hits;   //!
   TBranch        *b_H_aero_ntdc_pos_hits;   //!
   TBranch        *b_H_aero_pos_npe_sum;   //!
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
   TBranch        *b_H_cal_etot;   //!
   TBranch        *b_H_cal_etotnorm;   //!
   TBranch        *b_H_cal_etrack;   //!
   TBranch        *b_H_cal_nclust;   //!
   TBranch        *b_H_cal_nhits;   //!
   TBranch        *b_H_cal_ntracks;   //!
   TBranch        *b_H_cher_ncherhit;   //!
   TBranch        *b_H_cher_npesum;   //!
   TBranch        *b_H_dc_1u1_nhit;   //!
   TBranch        *b_H_dc_1v1_nhit;   //!
   TBranch        *b_H_dc_1x1_nhit;   //!
   TBranch        *b_H_dc_1x2_nhit;   //!
   TBranch        *b_H_dc_1y1_nhit;   //!
   TBranch        *b_H_dc_1y2_nhit;   //!
   TBranch        *b_H_dc_2u1_nhit;   //!
   TBranch        *b_H_dc_2v1_nhit;   //!
   TBranch        *b_H_dc_2x1_nhit;   //!
   TBranch        *b_H_dc_2x2_nhit;   //!
   TBranch        *b_H_dc_2y1_nhit;   //!
   TBranch        *b_H_dc_2y2_nhit;   //!
   TBranch        *b_H_dc_Ch1_maxhits;   //!
   TBranch        *b_H_dc_Ch1_nhit;   //!
   TBranch        *b_H_dc_Ch1_spacepoints;   //!
   TBranch        *b_H_dc_Ch1_trawhit;   //!
   TBranch        *b_H_dc_Ch2_maxhits;   //!
   TBranch        *b_H_dc_Ch2_nhit;   //!
   TBranch        *b_H_dc_Ch2_spacepoints;   //!
   TBranch        *b_H_dc_Ch2_trawhit;   //!
   TBranch        *b_H_dc_nhit;   //!
   TBranch        *b_H_dc_nsp;   //!
   TBranch        *b_H_dc_ntrack;   //!
   TBranch        *b_H_dc_stubtest;   //!
   TBranch        *b_H_dc_tnhit;   //!
   TBranch        *b_H_dc_trawhit;   //!
   TBranch        *b_H_hod_1x_nhits;   //!
   TBranch        *b_H_hod_1y_nhits;   //!
   TBranch        *b_H_hod_2x_nhits;   //!
   TBranch        *b_H_hod_2y_nhits;   //!
   TBranch        *b_H_hod_betachisqnotrack;   //!
   TBranch        *b_H_hod_betanotrack;   //!
   TBranch        *b_H_hod_betap;   //!
   TBranch        *b_H_hod_goodscinhit;   //!
   TBranch        *b_H_hod_goodstarttime;   //!
   TBranch        *b_H_hod_scindid;   //!
   TBranch        *b_H_hod_scinshould;   //!
   TBranch        *b_H_hod_starttime;   //!
   TBranch        *b_H_tr_n;   //!
   TBranch        *b_HSCOINPRE;   //!
   TBranch        *b_HSCOINPREr;   //!
   TBranch        *b_HSCOINTRG;   //!
   TBranch        *b_HSCOINTRGr;   //!
   TBranch        *b_HSHMSPRE;   //!
   TBranch        *b_HSHMSPREr;   //!
   TBranch        *b_HSHMSTRG;   //!
   TBranch        *b_HSHMSTRGr;   //!
   TBranch        *b_HSPEDPRE;   //!
   TBranch        *b_HSPEDPREr;   //!
   TBranch        *b_HSPEDTRG;   //!
   TBranch        *b_HSPEDTRGr;   //!
   TBranch        *b_HSS1;   //!
   TBranch        *b_HSS1X;   //!
   TBranch        *b_HSS1Xr;   //!
   TBranch        *b_HSS1Y;   //!
   TBranch        *b_HSS1Yr;   //!
   TBranch        *b_HSS1r;   //!
   TBranch        *b_HSS2;   //!
   TBranch        *b_HSS2X;   //!
   TBranch        *b_HSS2Xr;   //!
   TBranch        *b_HSS2Y;   //!
   TBranch        *b_HSS2Yr;   //!
   TBranch        *b_HSS2r;   //!
   TBranch        *b_HSSHODO11;   //!
   TBranch        *b_HSSHODO11r;   //!
   TBranch        *b_HSSHODO13;   //!
   TBranch        *b_HSSHODO13r;   //!
   TBranch        *b_HSSHODO15;   //!
   TBranch        *b_HSSHODO15r;   //!
   TBranch        *b_HSSHODO9;   //!
   TBranch        *b_HSSHODO9r;   //!
   TBranch        *b_HSSOSPRE;   //!
   TBranch        *b_HSSOSPREr;   //!
   TBranch        *b_HSSOSTRG;   //!
   TBranch        *b_HSSOSTRGr;   //!
   TBranch        *b_HShADCGATES;   //!
   TBranch        *b_HShADCGATESr;   //!
   TBranch        *b_HShBCM1;   //!
   TBranch        *b_HShBCM1r;   //!
   TBranch        *b_HShBCM2;   //!
   TBranch        *b_HShBCM2r;   //!
   TBranch        *b_HShBEAMGATES;   //!
   TBranch        *b_HShBEAMGATESr;   //!
   TBranch        *b_HShCER;   //!
   TBranch        *b_HShCERHI;   //!
   TBranch        *b_HShCERHIr;   //!
   TBranch        *b_HShCERr;   //!
   TBranch        *b_HShCLOCK;   //!
   TBranch        *b_HShCLOCKr;   //!
   TBranch        *b_HShELCLEAN;   //!
   TBranch        *b_HShELCLEANr;   //!
   TBranch        *b_HShELHI;   //!
   TBranch        *b_HShELHIr;   //!
   TBranch        *b_HShELLO;   //!
   TBranch        *b_HShELLOr;   //!
   TBranch        *b_HShELREAL;   //!
   TBranch        *b_HShELREALr;   //!
   TBranch        *b_HShPION;   //!
   TBranch        *b_HShPIONHI;   //!
   TBranch        *b_HShPIONHIr;   //!
   TBranch        *b_HShPIONr;   //!
   TBranch        *b_HShPIPRE;   //!
   TBranch        *b_HShPIPREr;   //!
   TBranch        *b_HShPRE100;   //!
   TBranch        *b_HShPRE100r;   //!
   TBranch        *b_HShPRE150;   //!
   TBranch        *b_HShPRE150r;   //!
   TBranch        *b_HShPRE200;   //!
   TBranch        *b_HShPRE200r;   //!
   TBranch        *b_HShPRE50;   //!
   TBranch        *b_HShPRE50r;   //!
   TBranch        *b_HShPRETRG;   //!
   TBranch        *b_HShPRETRGr;   //!
   TBranch        *b_HShPRHI;   //!
   TBranch        *b_HShPRHIr;   //!
   TBranch        *b_HShPRLO;   //!
   TBranch        *b_HShPRLOr;   //!
   TBranch        *b_HShS1;   //!
   TBranch        *b_HShS1X;   //!
   TBranch        *b_HShS1Xr;   //!
   TBranch        *b_HShS1Y;   //!
   TBranch        *b_HShS1Yr;   //!
   TBranch        *b_HShS1r;   //!
   TBranch        *b_HShS2;   //!
   TBranch        *b_HShS2X;   //!
   TBranch        *b_HShS2Xr;   //!
   TBranch        *b_HShS2Y;   //!
   TBranch        *b_HShS2Yr;   //!
   TBranch        *b_HShS2r;   //!
   TBranch        *b_HShSCIN;   //!
   TBranch        *b_HShSCINr;   //!
   TBranch        *b_HShSHLO;   //!
   TBranch        *b_HShSHLOr;   //!
   TBranch        *b_HShSTOF;   //!
   TBranch        *b_HShSTOFr;   //!
   TBranch        *b_HShUNSER;   //!
   TBranch        *b_HShUNSERr;   //!
   TBranch        *b_HShUNUSED4;   //!
   TBranch        *b_HShUNUSED4r;   //!
   TBranch        *b_HShUNUSED5;   //!
   TBranch        *b_HShUNUSED5r;   //!
   TBranch        *b_HSsADCGATES;   //!
   TBranch        *b_HSsADCGATESr;   //!
   TBranch        *b_HSsBCM1;   //!
   TBranch        *b_HSsBCM1r;   //!
   TBranch        *b_HSsBCM2;   //!
   TBranch        *b_HSsBCM2r;   //!
   TBranch        *b_HSsCER;   //!
   TBranch        *b_HSsCERr;   //!
   TBranch        *b_HSsCLOCK;   //!
   TBranch        *b_HSsCLOCKr;   //!
   TBranch        *b_HSsELCLEAN;   //!
   TBranch        *b_HSsELCLEANr;   //!
   TBranch        *b_HSsELHI;   //!
   TBranch        *b_HSsELHIr;   //!
   TBranch        *b_HSsELLO;   //!
   TBranch        *b_HSsELLOr;   //!
   TBranch        *b_HSsELREAL;   //!
   TBranch        *b_HSsELREALr;   //!
   TBranch        *b_HSsPION;   //!
   TBranch        *b_HSsPIONr;   //!
   TBranch        *b_HSsPIPRE;   //!
   TBranch        *b_HSsPIPREr;   //!
   TBranch        *b_HSsPRE100;   //!
   TBranch        *b_HSsPRE100r;   //!
   TBranch        *b_HSsPRE150;   //!
   TBranch        *b_HSsPRE150r;   //!
   TBranch        *b_HSsPRE200;   //!
   TBranch        *b_HSsPRE200r;   //!
   TBranch        *b_HSsPRE50;   //!
   TBranch        *b_HSsPRE50r;   //!
   TBranch        *b_HSsPRETRG;   //!
   TBranch        *b_HSsPRETRGr;   //!
   TBranch        *b_HSsPRHI;   //!
   TBranch        *b_HSsPRHIr;   //!
   TBranch        *b_HSsPRLO;   //!
   TBranch        *b_HSsPRLOr;   //!
   TBranch        *b_HSsS1;   //!
   TBranch        *b_HSsS1X;   //!
   TBranch        *b_HSsS1Xr;   //!
   TBranch        *b_HSsS1Y;   //!
   TBranch        *b_HSsS1Yr;   //!
   TBranch        *b_HSsS1r;   //!
   TBranch        *b_HSsS2;   //!
   TBranch        *b_HSsS2X;   //!
   TBranch        *b_HSsS2Xr;   //!
   TBranch        *b_HSsS2Y;   //!
   TBranch        *b_HSsS2Yr;   //!
   TBranch        *b_HSsS2r;   //!
   TBranch        *b_HSsSCIN;   //!
   TBranch        *b_HSsSCINr;   //!
   TBranch        *b_HSsSHLO;   //!
   TBranch        *b_HSsSHLOr;   //!
   TBranch        *b_HSsSTOF;   //!
   TBranch        *b_HSsSTOFr;   //!
   TBranch        *b_S_cal_1pr_eplane;   //!
   TBranch        *b_S_cal_1pr_eplane_neg;   //!
   TBranch        *b_S_cal_1pr_eplane_pos;   //!
   TBranch        *b_S_cal_2ta_earray;   //!
   TBranch        *b_S_cal_2ta_nclust;   //!
   TBranch        *b_S_cal_2ta_nhits;   //!
   TBranch        *b_S_cal_2ta_ntracks;   //!
   TBranch        *b_S_cal_etot;   //!
   TBranch        *b_S_cal_etotnorm;   //!
   TBranch        *b_S_cal_etrack;   //!
   TBranch        *b_S_cal_nclust;   //!
   TBranch        *b_S_cal_nhits;   //!
   TBranch        *b_S_cal_ntracks;   //!
   TBranch        *b_S_dc_1u1_nhit;   //!
   TBranch        *b_S_dc_1u2_nhit;   //!
   TBranch        *b_S_dc_1v1_nhit;   //!
   TBranch        *b_S_dc_1v2_nhit;   //!
   TBranch        *b_S_dc_1x1_nhit;   //!
   TBranch        *b_S_dc_1x2_nhit;   //!
   TBranch        *b_S_dc_2u1_nhit;   //!
   TBranch        *b_S_dc_2u2_nhit;   //!
   TBranch        *b_S_dc_2v1_nhit;   //!
   TBranch        *b_S_dc_2v2_nhit;   //!
   TBranch        *b_S_dc_2x1_nhit;   //!
   TBranch        *b_S_dc_2x2_nhit;   //!
   TBranch        *b_S_dc_Ch1_maxhits;   //!
   TBranch        *b_S_dc_Ch1_nhit;   //!
   TBranch        *b_S_dc_Ch1_spacepoints;   //!
   TBranch        *b_S_dc_Ch1_trawhit;   //!
   TBranch        *b_S_dc_Ch2_maxhits;   //!
   TBranch        *b_S_dc_Ch2_nhit;   //!
   TBranch        *b_S_dc_Ch2_spacepoints;   //!
   TBranch        *b_S_dc_Ch2_trawhit;   //!
   TBranch        *b_S_dc_nhit;   //!
   TBranch        *b_S_dc_nsp;   //!
   TBranch        *b_S_dc_ntrack;   //!
   TBranch        *b_S_dc_stubtest;   //!
   TBranch        *b_S_dc_tnhit;   //!
   TBranch        *b_S_dc_trawhit;   //!
   TBranch        *b_S_hod_1x_nhits;   //!
   TBranch        *b_S_hod_1y_nhits;   //!
   TBranch        *b_S_hod_2x_nhits;   //!
   TBranch        *b_S_hod_2y_nhits;   //!
   TBranch        *b_S_hod_betachisqnotrack;   //!
   TBranch        *b_S_hod_betanotrack;   //!
   TBranch        *b_S_hod_betap;   //!
   TBranch        *b_S_hod_goodscinhit;   //!
   TBranch        *b_S_hod_goodstarttime;   //!
   TBranch        *b_S_hod_scindid;   //!
   TBranch        *b_S_hod_scinshould;   //!
   TBranch        *b_S_hod_starttime;   //!
   TBranch        *b_S_tr_n;   //!
   TBranch        *b_g_evtyp;   //!
   TBranch        *b_Hhodtdif1;   //!
   TBranch        *b_Hhodtdif2;   //!
   TBranch        *b_Hhodtdif3;   //!
   TBranch        *b_Hhodtdif4;   //!
   TBranch        *b_Hhodtdif5;   //!
   TBranch        *b_Hhodtdif6;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtTime;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtNum;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtType;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fEvtLen;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fHelicity;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fTargetPol;   //!
   TBranch        *b_Event_Branch_fEvtHdr_fRun;   //!

   shms_class(TTree *tree=0);
   virtual ~shms_class();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef shms_class_cxx
shms_class::shms_class(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("shms_52949.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("shms_52949.root");
      }
      f->GetObject("T",tree);

   }
   Init(tree);
}

shms_class::~shms_class()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t shms_class::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t shms_class::LoadTree(Long64_t entry)
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

void shms_class::Init(TTree *tree)
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

   fChain->SetBranchAddress("Hhodtdif1", &Hhodtdif1, &b_Hhodtdif1);
   fChain->SetBranchAddress("Hhodtdif2", &Hhodtdif2, &b_Hhodtdif2);
   fChain->SetBranchAddress("Hhodtdif3", &Hhodtdif3, &b_Hhodtdif3);
   fChain->SetBranchAddress("Hhodtdif4", &Hhodtdif4, &b_Hhodtdif4);
   fChain->SetBranchAddress("Hhodtdif5", &Hhodtdif5, &b_Hhodtdif5);
   fChain->SetBranchAddress("Hhodtdif6", &Hhodtdif6, &b_Hhodtdif6);
   fChain->SetBranchAddress("Ndata.H.aero.aneg", &Ndata_H_aero_aneg, &b_Ndata_H_aero_aneg);
   fChain->SetBranchAddress("H.aero.aneg", H_aero_aneg, &b_H_aero_aneg);
   fChain->SetBranchAddress("Ndata.H.aero.aneg_p", &Ndata_H_aero_aneg_p, &b_Ndata_H_aero_aneg_p);
   fChain->SetBranchAddress("H.aero.aneg_p", H_aero_aneg_p, &b_H_aero_aneg_p);
   fChain->SetBranchAddress("Ndata.H.aero.apos", &Ndata_H_aero_apos, &b_Ndata_H_aero_apos);
   fChain->SetBranchAddress("H.aero.apos", H_aero_apos, &b_H_aero_apos);
   fChain->SetBranchAddress("Ndata.H.aero.apos_p", &Ndata_H_aero_apos_p, &b_Ndata_H_aero_apos_p);
   fChain->SetBranchAddress("H.aero.apos_p", H_aero_apos_p, &b_H_aero_apos_p);
   fChain->SetBranchAddress("Ndata.H.aero.neg_npe", &Ndata_H_aero_neg_npe, &b_Ndata_H_aero_neg_npe);
   fChain->SetBranchAddress("H.aero.neg_npe", H_aero_neg_npe, &b_H_aero_neg_npe);
   fChain->SetBranchAddress("Ndata.H.aero.negadchits", &Ndata_H_aero_negadchits, &b_Ndata_H_aero_negadchits);
   fChain->SetBranchAddress("H.aero.negadchits", H_aero_negadchits, &b_H_aero_negadchits);
   fChain->SetBranchAddress("Ndata.H.aero.negtdchits", &Ndata_H_aero_negtdchits, &b_Ndata_H_aero_negtdchits);
   fChain->SetBranchAddress("H.aero.negtdchits", &H_aero_negtdchits, &b_H_aero_negtdchits);
   fChain->SetBranchAddress("Ndata.H.aero.pos_npe", &Ndata_H_aero_pos_npe, &b_Ndata_H_aero_pos_npe);
   fChain->SetBranchAddress("H.aero.pos_npe", H_aero_pos_npe, &b_H_aero_pos_npe);
   fChain->SetBranchAddress("Ndata.H.aero.posadchits", &Ndata_H_aero_posadchits, &b_Ndata_H_aero_posadchits);
   fChain->SetBranchAddress("H.aero.posadchits", H_aero_posadchits, &b_H_aero_posadchits);
   fChain->SetBranchAddress("Ndata.H.aero.postdchits", &Ndata_H_aero_postdchits, &b_Ndata_H_aero_postdchits);
   fChain->SetBranchAddress("H.aero.postdchits", &H_aero_postdchits, &b_H_aero_postdchits);
   fChain->SetBranchAddress("Ndata.H.aero.tneg", &Ndata_H_aero_tneg, &b_Ndata_H_aero_tneg);
   fChain->SetBranchAddress("H.aero.tneg", H_aero_tneg, &b_H_aero_tneg);
   fChain->SetBranchAddress("Ndata.H.aero.tpos", &Ndata_H_aero_tpos, &b_Ndata_H_aero_tpos);
   fChain->SetBranchAddress("H.aero.tpos", H_aero_tpos, &b_H_aero_tpos);
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
   fChain->SetBranchAddress("Ndata.H.cher.adc", &Ndata_H_cher_adc, &b_Ndata_H_cher_adc);
   fChain->SetBranchAddress("H.cher.adc", H_cher_adc, &b_H_cher_adc);
   fChain->SetBranchAddress("Ndata.H.cher.adc_p", &Ndata_H_cher_adc_p, &b_Ndata_H_cher_adc_p);
   fChain->SetBranchAddress("H.cher.adc_p", H_cher_adc_p, &b_H_cher_adc_p);
   fChain->SetBranchAddress("Ndata.H.cher.cerfiredcounter", &Ndata_H_cher_cerfiredcounter, &b_Ndata_H_cher_cerfiredcounter);
   fChain->SetBranchAddress("H.cher.cerfiredcounter", H_cher_cerfiredcounter, &b_H_cher_cerfiredcounter);
   fChain->SetBranchAddress("Ndata.H.cher.certrackcounter", &Ndata_H_cher_certrackcounter, &b_Ndata_H_cher_certrackcounter);
   fChain->SetBranchAddress("H.cher.certrackcounter", H_cher_certrackcounter, &b_H_cher_certrackcounter);
   fChain->SetBranchAddress("Ndata.H.cher.npe", &Ndata_H_cher_npe, &b_Ndata_H_cher_npe);
   fChain->SetBranchAddress("H.cher.npe", H_cher_npe, &b_H_cher_npe);
   fChain->SetBranchAddress("Ndata.H.cher.phototubes", &Ndata_H_cher_phototubes, &b_Ndata_H_cher_phototubes);
   fChain->SetBranchAddress("H.cher.phototubes", H_cher_phototubes, &b_H_cher_phototubes);
   fChain->SetBranchAddress("Ndata.H.dc.1u1.dist", &Ndata_H_dc_1u1_dist, &b_Ndata_H_dc_1u1_dist);
   fChain->SetBranchAddress("H.dc.1u1.dist", H_dc_1u1_dist, &b_H_dc_1u1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1u1.rawtdc", &Ndata_H_dc_1u1_rawtdc, &b_Ndata_H_dc_1u1_rawtdc);
   fChain->SetBranchAddress("H.dc.1u1.rawtdc", H_dc_1u1_rawtdc, &b_H_dc_1u1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1u1.tdchits", &Ndata_H_dc_1u1_tdchits, &b_Ndata_H_dc_1u1_tdchits);
   fChain->SetBranchAddress("H.dc.1u1.tdchits", H_dc_1u1_tdchits, &b_H_dc_1u1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1u1.time", &Ndata_H_dc_1u1_time, &b_Ndata_H_dc_1u1_time);
   fChain->SetBranchAddress("H.dc.1u1.time", H_dc_1u1_time, &b_H_dc_1u1_time);
   fChain->SetBranchAddress("Ndata.H.dc.1v1.dist", &Ndata_H_dc_1v1_dist, &b_Ndata_H_dc_1v1_dist);
   fChain->SetBranchAddress("H.dc.1v1.dist", H_dc_1v1_dist, &b_H_dc_1v1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1v1.rawtdc", &Ndata_H_dc_1v1_rawtdc, &b_Ndata_H_dc_1v1_rawtdc);
   fChain->SetBranchAddress("H.dc.1v1.rawtdc", H_dc_1v1_rawtdc, &b_H_dc_1v1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1v1.tdchits", &Ndata_H_dc_1v1_tdchits, &b_Ndata_H_dc_1v1_tdchits);
   fChain->SetBranchAddress("H.dc.1v1.tdchits", H_dc_1v1_tdchits, &b_H_dc_1v1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1v1.time", &Ndata_H_dc_1v1_time, &b_Ndata_H_dc_1v1_time);
   fChain->SetBranchAddress("H.dc.1v1.time", H_dc_1v1_time, &b_H_dc_1v1_time);
   fChain->SetBranchAddress("Ndata.H.dc.1x1.dist", &Ndata_H_dc_1x1_dist, &b_Ndata_H_dc_1x1_dist);
   fChain->SetBranchAddress("H.dc.1x1.dist", H_dc_1x1_dist, &b_H_dc_1x1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1x1.rawtdc", &Ndata_H_dc_1x1_rawtdc, &b_Ndata_H_dc_1x1_rawtdc);
   fChain->SetBranchAddress("H.dc.1x1.rawtdc", H_dc_1x1_rawtdc, &b_H_dc_1x1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1x1.tdchits", &Ndata_H_dc_1x1_tdchits, &b_Ndata_H_dc_1x1_tdchits);
   fChain->SetBranchAddress("H.dc.1x1.tdchits", H_dc_1x1_tdchits, &b_H_dc_1x1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1x1.time", &Ndata_H_dc_1x1_time, &b_Ndata_H_dc_1x1_time);
   fChain->SetBranchAddress("H.dc.1x1.time", H_dc_1x1_time, &b_H_dc_1x1_time);
   fChain->SetBranchAddress("Ndata.H.dc.1x2.dist", &Ndata_H_dc_1x2_dist, &b_Ndata_H_dc_1x2_dist);
   fChain->SetBranchAddress("H.dc.1x2.dist", H_dc_1x2_dist, &b_H_dc_1x2_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1x2.rawtdc", &Ndata_H_dc_1x2_rawtdc, &b_Ndata_H_dc_1x2_rawtdc);
   fChain->SetBranchAddress("H.dc.1x2.rawtdc", H_dc_1x2_rawtdc, &b_H_dc_1x2_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1x2.tdchits", &Ndata_H_dc_1x2_tdchits, &b_Ndata_H_dc_1x2_tdchits);
   fChain->SetBranchAddress("H.dc.1x2.tdchits", H_dc_1x2_tdchits, &b_H_dc_1x2_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1x2.time", &Ndata_H_dc_1x2_time, &b_Ndata_H_dc_1x2_time);
   fChain->SetBranchAddress("H.dc.1x2.time", H_dc_1x2_time, &b_H_dc_1x2_time);
   fChain->SetBranchAddress("Ndata.H.dc.1y1.dist", &Ndata_H_dc_1y1_dist, &b_Ndata_H_dc_1y1_dist);
   fChain->SetBranchAddress("H.dc.1y1.dist", H_dc_1y1_dist, &b_H_dc_1y1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1y1.rawtdc", &Ndata_H_dc_1y1_rawtdc, &b_Ndata_H_dc_1y1_rawtdc);
   fChain->SetBranchAddress("H.dc.1y1.rawtdc", H_dc_1y1_rawtdc, &b_H_dc_1y1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1y1.tdchits", &Ndata_H_dc_1y1_tdchits, &b_Ndata_H_dc_1y1_tdchits);
   fChain->SetBranchAddress("H.dc.1y1.tdchits", H_dc_1y1_tdchits, &b_H_dc_1y1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1y1.time", &Ndata_H_dc_1y1_time, &b_Ndata_H_dc_1y1_time);
   fChain->SetBranchAddress("H.dc.1y1.time", H_dc_1y1_time, &b_H_dc_1y1_time);
   fChain->SetBranchAddress("Ndata.H.dc.1y2.dist", &Ndata_H_dc_1y2_dist, &b_Ndata_H_dc_1y2_dist);
   fChain->SetBranchAddress("H.dc.1y2.dist", H_dc_1y2_dist, &b_H_dc_1y2_dist);
   fChain->SetBranchAddress("Ndata.H.dc.1y2.rawtdc", &Ndata_H_dc_1y2_rawtdc, &b_Ndata_H_dc_1y2_rawtdc);
   fChain->SetBranchAddress("H.dc.1y2.rawtdc", H_dc_1y2_rawtdc, &b_H_dc_1y2_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.1y2.tdchits", &Ndata_H_dc_1y2_tdchits, &b_Ndata_H_dc_1y2_tdchits);
   fChain->SetBranchAddress("H.dc.1y2.tdchits", H_dc_1y2_tdchits, &b_H_dc_1y2_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.1y2.time", &Ndata_H_dc_1y2_time, &b_Ndata_H_dc_1y2_time);
   fChain->SetBranchAddress("H.dc.1y2.time", H_dc_1y2_time, &b_H_dc_1y2_time);
   fChain->SetBranchAddress("Ndata.H.dc.2u1.dist", &Ndata_H_dc_2u1_dist, &b_Ndata_H_dc_2u1_dist);
   fChain->SetBranchAddress("H.dc.2u1.dist", H_dc_2u1_dist, &b_H_dc_2u1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2u1.rawtdc", &Ndata_H_dc_2u1_rawtdc, &b_Ndata_H_dc_2u1_rawtdc);
   fChain->SetBranchAddress("H.dc.2u1.rawtdc", H_dc_2u1_rawtdc, &b_H_dc_2u1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2u1.tdchits", &Ndata_H_dc_2u1_tdchits, &b_Ndata_H_dc_2u1_tdchits);
   fChain->SetBranchAddress("H.dc.2u1.tdchits", H_dc_2u1_tdchits, &b_H_dc_2u1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2u1.time", &Ndata_H_dc_2u1_time, &b_Ndata_H_dc_2u1_time);
   fChain->SetBranchAddress("H.dc.2u1.time", H_dc_2u1_time, &b_H_dc_2u1_time);
   fChain->SetBranchAddress("Ndata.H.dc.2v1.dist", &Ndata_H_dc_2v1_dist, &b_Ndata_H_dc_2v1_dist);
   fChain->SetBranchAddress("H.dc.2v1.dist", H_dc_2v1_dist, &b_H_dc_2v1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2v1.rawtdc", &Ndata_H_dc_2v1_rawtdc, &b_Ndata_H_dc_2v1_rawtdc);
   fChain->SetBranchAddress("H.dc.2v1.rawtdc", H_dc_2v1_rawtdc, &b_H_dc_2v1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2v1.tdchits", &Ndata_H_dc_2v1_tdchits, &b_Ndata_H_dc_2v1_tdchits);
   fChain->SetBranchAddress("H.dc.2v1.tdchits", H_dc_2v1_tdchits, &b_H_dc_2v1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2v1.time", &Ndata_H_dc_2v1_time, &b_Ndata_H_dc_2v1_time);
   fChain->SetBranchAddress("H.dc.2v1.time", H_dc_2v1_time, &b_H_dc_2v1_time);
   fChain->SetBranchAddress("Ndata.H.dc.2x1.dist", &Ndata_H_dc_2x1_dist, &b_Ndata_H_dc_2x1_dist);
   fChain->SetBranchAddress("H.dc.2x1.dist", H_dc_2x1_dist, &b_H_dc_2x1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2x1.rawtdc", &Ndata_H_dc_2x1_rawtdc, &b_Ndata_H_dc_2x1_rawtdc);
   fChain->SetBranchAddress("H.dc.2x1.rawtdc", H_dc_2x1_rawtdc, &b_H_dc_2x1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2x1.tdchits", &Ndata_H_dc_2x1_tdchits, &b_Ndata_H_dc_2x1_tdchits);
   fChain->SetBranchAddress("H.dc.2x1.tdchits", H_dc_2x1_tdchits, &b_H_dc_2x1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2x1.time", &Ndata_H_dc_2x1_time, &b_Ndata_H_dc_2x1_time);
   fChain->SetBranchAddress("H.dc.2x1.time", H_dc_2x1_time, &b_H_dc_2x1_time);
   fChain->SetBranchAddress("Ndata.H.dc.2x2.dist", &Ndata_H_dc_2x2_dist, &b_Ndata_H_dc_2x2_dist);
   fChain->SetBranchAddress("H.dc.2x2.dist", H_dc_2x2_dist, &b_H_dc_2x2_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2x2.rawtdc", &Ndata_H_dc_2x2_rawtdc, &b_Ndata_H_dc_2x2_rawtdc);
   fChain->SetBranchAddress("H.dc.2x2.rawtdc", H_dc_2x2_rawtdc, &b_H_dc_2x2_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2x2.tdchits", &Ndata_H_dc_2x2_tdchits, &b_Ndata_H_dc_2x2_tdchits);
   fChain->SetBranchAddress("H.dc.2x2.tdchits", H_dc_2x2_tdchits, &b_H_dc_2x2_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2x2.time", &Ndata_H_dc_2x2_time, &b_Ndata_H_dc_2x2_time);
   fChain->SetBranchAddress("H.dc.2x2.time", H_dc_2x2_time, &b_H_dc_2x2_time);
   fChain->SetBranchAddress("Ndata.H.dc.2y1.dist", &Ndata_H_dc_2y1_dist, &b_Ndata_H_dc_2y1_dist);
   fChain->SetBranchAddress("H.dc.2y1.dist", H_dc_2y1_dist, &b_H_dc_2y1_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2y1.rawtdc", &Ndata_H_dc_2y1_rawtdc, &b_Ndata_H_dc_2y1_rawtdc);
   fChain->SetBranchAddress("H.dc.2y1.rawtdc", H_dc_2y1_rawtdc, &b_H_dc_2y1_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2y1.tdchits", &Ndata_H_dc_2y1_tdchits, &b_Ndata_H_dc_2y1_tdchits);
   fChain->SetBranchAddress("H.dc.2y1.tdchits", H_dc_2y1_tdchits, &b_H_dc_2y1_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2y1.time", &Ndata_H_dc_2y1_time, &b_Ndata_H_dc_2y1_time);
   fChain->SetBranchAddress("H.dc.2y1.time", H_dc_2y1_time, &b_H_dc_2y1_time);
   fChain->SetBranchAddress("Ndata.H.dc.2y2.dist", &Ndata_H_dc_2y2_dist, &b_Ndata_H_dc_2y2_dist);
   fChain->SetBranchAddress("H.dc.2y2.dist", H_dc_2y2_dist, &b_H_dc_2y2_dist);
   fChain->SetBranchAddress("Ndata.H.dc.2y2.rawtdc", &Ndata_H_dc_2y2_rawtdc, &b_Ndata_H_dc_2y2_rawtdc);
   fChain->SetBranchAddress("H.dc.2y2.rawtdc", H_dc_2y2_rawtdc, &b_H_dc_2y2_rawtdc);
   fChain->SetBranchAddress("Ndata.H.dc.2y2.tdchits", &Ndata_H_dc_2y2_tdchits, &b_Ndata_H_dc_2y2_tdchits);
   fChain->SetBranchAddress("H.dc.2y2.tdchits", H_dc_2y2_tdchits, &b_H_dc_2y2_tdchits);
   fChain->SetBranchAddress("Ndata.H.dc.2y2.time", &Ndata_H_dc_2y2_time, &b_Ndata_H_dc_2y2_time);
   fChain->SetBranchAddress("H.dc.2y2.time", H_dc_2y2_time, &b_H_dc_2y2_time);
   fChain->SetBranchAddress("Ndata.H.dc.residual", &Ndata_H_dc_residual, &b_Ndata_H_dc_residual);
   fChain->SetBranchAddress("H.dc.residual", H_dc_residual, &b_H_dc_residual);
   fChain->SetBranchAddress("Ndata.H.dc.x", &Ndata_H_dc_x, &b_Ndata_H_dc_x);
   fChain->SetBranchAddress("H.dc.x", H_dc_x, &b_H_dc_x);
   fChain->SetBranchAddress("Ndata.H.dc.xp", &Ndata_H_dc_xp, &b_Ndata_H_dc_xp);
   fChain->SetBranchAddress("H.dc.xp", H_dc_xp, &b_H_dc_xp);
   fChain->SetBranchAddress("Ndata.H.dc.y", &Ndata_H_dc_y, &b_Ndata_H_dc_y);
   fChain->SetBranchAddress("H.dc.y", H_dc_y, &b_H_dc_y);
   fChain->SetBranchAddress("Ndata.H.dc.yp", &Ndata_H_dc_yp, &b_Ndata_H_dc_yp);
   fChain->SetBranchAddress("H.dc.yp", H_dc_yp, &b_H_dc_yp);
   fChain->SetBranchAddress("Ndata.H.hod.1x.negadchits", &Ndata_H_hod_1x_negadchits, &b_Ndata_H_hod_1x_negadchits);
   fChain->SetBranchAddress("H.hod.1x.negadchits", H_hod_1x_negadchits, &b_H_hod_1x_negadchits);
   fChain->SetBranchAddress("Ndata.H.hod.1x.negtdchits", &Ndata_H_hod_1x_negtdchits, &b_Ndata_H_hod_1x_negtdchits);
   fChain->SetBranchAddress("H.hod.1x.negtdchits", H_hod_1x_negtdchits, &b_H_hod_1x_negtdchits);
   fChain->SetBranchAddress("Ndata.H.hod.1x.posadchits", &Ndata_H_hod_1x_posadchits, &b_Ndata_H_hod_1x_posadchits);
   fChain->SetBranchAddress("H.hod.1x.posadchits", H_hod_1x_posadchits, &b_H_hod_1x_posadchits);
   fChain->SetBranchAddress("Ndata.H.hod.1x.postdchits", &Ndata_H_hod_1x_postdchits, &b_Ndata_H_hod_1x_postdchits);
   fChain->SetBranchAddress("H.hod.1x.postdchits", H_hod_1x_postdchits, &b_H_hod_1x_postdchits);
   fChain->SetBranchAddress("Ndata.H.hod.1y.negadchits", &Ndata_H_hod_1y_negadchits, &b_Ndata_H_hod_1y_negadchits);
   fChain->SetBranchAddress("H.hod.1y.negadchits", H_hod_1y_negadchits, &b_H_hod_1y_negadchits);
   fChain->SetBranchAddress("Ndata.H.hod.1y.negtdchits", &Ndata_H_hod_1y_negtdchits, &b_Ndata_H_hod_1y_negtdchits);
   fChain->SetBranchAddress("H.hod.1y.negtdchits", H_hod_1y_negtdchits, &b_H_hod_1y_negtdchits);
   fChain->SetBranchAddress("Ndata.H.hod.1y.posadchits", &Ndata_H_hod_1y_posadchits, &b_Ndata_H_hod_1y_posadchits);
   fChain->SetBranchAddress("H.hod.1y.posadchits", H_hod_1y_posadchits, &b_H_hod_1y_posadchits);
   fChain->SetBranchAddress("Ndata.H.hod.1y.postdchits", &Ndata_H_hod_1y_postdchits, &b_Ndata_H_hod_1y_postdchits);
   fChain->SetBranchAddress("H.hod.1y.postdchits", H_hod_1y_postdchits, &b_H_hod_1y_postdchits);
   fChain->SetBranchAddress("Ndata.H.hod.2x.negadchits", &Ndata_H_hod_2x_negadchits, &b_Ndata_H_hod_2x_negadchits);
   fChain->SetBranchAddress("H.hod.2x.negadchits", H_hod_2x_negadchits, &b_H_hod_2x_negadchits);
   fChain->SetBranchAddress("Ndata.H.hod.2x.negtdchits", &Ndata_H_hod_2x_negtdchits, &b_Ndata_H_hod_2x_negtdchits);
   fChain->SetBranchAddress("H.hod.2x.negtdchits", H_hod_2x_negtdchits, &b_H_hod_2x_negtdchits);
   fChain->SetBranchAddress("Ndata.H.hod.2x.posadchits", &Ndata_H_hod_2x_posadchits, &b_Ndata_H_hod_2x_posadchits);
   fChain->SetBranchAddress("H.hod.2x.posadchits", H_hod_2x_posadchits, &b_H_hod_2x_posadchits);
   fChain->SetBranchAddress("Ndata.H.hod.2x.postdchits", &Ndata_H_hod_2x_postdchits, &b_Ndata_H_hod_2x_postdchits);
   fChain->SetBranchAddress("H.hod.2x.postdchits", H_hod_2x_postdchits, &b_H_hod_2x_postdchits);
   fChain->SetBranchAddress("Ndata.H.hod.2y.negadchits", &Ndata_H_hod_2y_negadchits, &b_Ndata_H_hod_2y_negadchits);
   fChain->SetBranchAddress("H.hod.2y.negadchits", H_hod_2y_negadchits, &b_H_hod_2y_negadchits);
   fChain->SetBranchAddress("Ndata.H.hod.2y.negtdchits", &Ndata_H_hod_2y_negtdchits, &b_Ndata_H_hod_2y_negtdchits);
   fChain->SetBranchAddress("H.hod.2y.negtdchits", H_hod_2y_negtdchits, &b_H_hod_2y_negtdchits);
   fChain->SetBranchAddress("Ndata.H.hod.2y.posadchits", &Ndata_H_hod_2y_posadchits, &b_Ndata_H_hod_2y_posadchits);
   fChain->SetBranchAddress("H.hod.2y.posadchits", H_hod_2y_posadchits, &b_H_hod_2y_posadchits);
   fChain->SetBranchAddress("Ndata.H.hod.2y.postdchits", &Ndata_H_hod_2y_postdchits, &b_Ndata_H_hod_2y_postdchits);
   fChain->SetBranchAddress("H.hod.2y.postdchits", H_hod_2y_postdchits, &b_H_hod_2y_postdchits);
   fChain->SetBranchAddress("Ndata.H.hod.fpHitsTime", &Ndata_H_hod_fpHitsTime, &b_Ndata_H_hod_fpHitsTime);
   fChain->SetBranchAddress("H.hod.fpHitsTime", H_hod_fpHitsTime, &b_H_hod_fpHitsTime);
   fChain->SetBranchAddress("Ndata.H.tr.beta", &Ndata_H_tr_beta, &b_Ndata_H_tr_beta);
   fChain->SetBranchAddress("H.tr.beta", H_tr_beta, &b_H_tr_beta);
   fChain->SetBranchAddress("Ndata.H.tr.betachisq", &Ndata_H_tr_betachisq, &b_Ndata_H_tr_betachisq);
   fChain->SetBranchAddress("H.tr.betachisq", H_tr_betachisq, &b_H_tr_betachisq);
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
   fChain->SetBranchAddress("Ndata.S.cal.1pr.aneg", &Ndata_S_cal_1pr_aneg, &b_Ndata_S_cal_1pr_aneg);
   fChain->SetBranchAddress("S.cal.1pr.aneg", S_cal_1pr_aneg, &b_S_cal_1pr_aneg);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.aneg_p", &Ndata_S_cal_1pr_aneg_p, &b_Ndata_S_cal_1pr_aneg_p);
   fChain->SetBranchAddress("S.cal.1pr.aneg_p", S_cal_1pr_aneg_p, &b_S_cal_1pr_aneg_p);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.apos", &Ndata_S_cal_1pr_apos, &b_Ndata_S_cal_1pr_apos);
   fChain->SetBranchAddress("S.cal.1pr.apos", S_cal_1pr_apos, &b_S_cal_1pr_apos);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.apos_p", &Ndata_S_cal_1pr_apos_p, &b_Ndata_S_cal_1pr_apos_p);
   fChain->SetBranchAddress("S.cal.1pr.apos_p", S_cal_1pr_apos_p, &b_S_cal_1pr_apos_p);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.emean", &Ndata_S_cal_1pr_emean, &b_Ndata_S_cal_1pr_emean);
   fChain->SetBranchAddress("S.cal.1pr.emean", S_cal_1pr_emean, &b_S_cal_1pr_emean);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.eneg", &Ndata_S_cal_1pr_eneg, &b_Ndata_S_cal_1pr_eneg);
   fChain->SetBranchAddress("S.cal.1pr.eneg", S_cal_1pr_eneg, &b_S_cal_1pr_eneg);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.epos", &Ndata_S_cal_1pr_epos, &b_Ndata_S_cal_1pr_epos);
   fChain->SetBranchAddress("S.cal.1pr.epos", S_cal_1pr_epos, &b_S_cal_1pr_epos);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.negadchits", &Ndata_S_cal_1pr_negadchits, &b_Ndata_S_cal_1pr_negadchits);
   fChain->SetBranchAddress("S.cal.1pr.negadchits", S_cal_1pr_negadchits, &b_S_cal_1pr_negadchits);
   fChain->SetBranchAddress("Ndata.S.cal.1pr.posadchits", &Ndata_S_cal_1pr_posadchits, &b_Ndata_S_cal_1pr_posadchits);
   fChain->SetBranchAddress("S.cal.1pr.posadchits", S_cal_1pr_posadchits, &b_S_cal_1pr_posadchits);
   fChain->SetBranchAddress("Ndata.S.cal.2ta.a", &Ndata_S_cal_2ta_a, &b_Ndata_S_cal_2ta_a);
   fChain->SetBranchAddress("S.cal.2ta.a", S_cal_2ta_a, &b_S_cal_2ta_a);
   fChain->SetBranchAddress("Ndata.S.cal.2ta.a_p", &Ndata_S_cal_2ta_a_p, &b_Ndata_S_cal_2ta_a_p);
   fChain->SetBranchAddress("S.cal.2ta.a_p", S_cal_2ta_a_p, &b_S_cal_2ta_a_p);
   fChain->SetBranchAddress("Ndata.S.cal.2ta.adchits", &Ndata_S_cal_2ta_adchits, &b_Ndata_S_cal_2ta_adchits);
   fChain->SetBranchAddress("S.cal.2ta.adchits", S_cal_2ta_adchits, &b_S_cal_2ta_adchits);
   fChain->SetBranchAddress("Ndata.S.cal.2ta.e", &Ndata_S_cal_2ta_e, &b_Ndata_S_cal_2ta_e);
   fChain->SetBranchAddress("S.cal.2ta.e", S_cal_2ta_e, &b_S_cal_2ta_e);
   fChain->SetBranchAddress("Ndata.S.cal.2ta.p", &Ndata_S_cal_2ta_p, &b_Ndata_S_cal_2ta_p);
   fChain->SetBranchAddress("S.cal.2ta.p", S_cal_2ta_p, &b_S_cal_2ta_p);
   fChain->SetBranchAddress("Ndata.S.dc.1u1.dist", &Ndata_S_dc_1u1_dist, &b_Ndata_S_dc_1u1_dist);
   fChain->SetBranchAddress("S.dc.1u1.dist", S_dc_1u1_dist, &b_S_dc_1u1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1u1.rawtdc", &Ndata_S_dc_1u1_rawtdc, &b_Ndata_S_dc_1u1_rawtdc);
   fChain->SetBranchAddress("S.dc.1u1.rawtdc", S_dc_1u1_rawtdc, &b_S_dc_1u1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1u1.tdchits", &Ndata_S_dc_1u1_tdchits, &b_Ndata_S_dc_1u1_tdchits);
   fChain->SetBranchAddress("S.dc.1u1.tdchits", S_dc_1u1_tdchits, &b_S_dc_1u1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1u1.time", &Ndata_S_dc_1u1_time, &b_Ndata_S_dc_1u1_time);
   fChain->SetBranchAddress("S.dc.1u1.time", S_dc_1u1_time, &b_S_dc_1u1_time);
   fChain->SetBranchAddress("Ndata.S.dc.1u2.dist", &Ndata_S_dc_1u2_dist, &b_Ndata_S_dc_1u2_dist);
   fChain->SetBranchAddress("S.dc.1u2.dist", S_dc_1u2_dist, &b_S_dc_1u2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1u2.rawtdc", &Ndata_S_dc_1u2_rawtdc, &b_Ndata_S_dc_1u2_rawtdc);
   fChain->SetBranchAddress("S.dc.1u2.rawtdc", S_dc_1u2_rawtdc, &b_S_dc_1u2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1u2.tdchits", &Ndata_S_dc_1u2_tdchits, &b_Ndata_S_dc_1u2_tdchits);
   fChain->SetBranchAddress("S.dc.1u2.tdchits", S_dc_1u2_tdchits, &b_S_dc_1u2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1u2.time", &Ndata_S_dc_1u2_time, &b_Ndata_S_dc_1u2_time);
   fChain->SetBranchAddress("S.dc.1u2.time", S_dc_1u2_time, &b_S_dc_1u2_time);
   fChain->SetBranchAddress("Ndata.S.dc.1v1.dist", &Ndata_S_dc_1v1_dist, &b_Ndata_S_dc_1v1_dist);
   fChain->SetBranchAddress("S.dc.1v1.dist", S_dc_1v1_dist, &b_S_dc_1v1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1v1.rawtdc", &Ndata_S_dc_1v1_rawtdc, &b_Ndata_S_dc_1v1_rawtdc);
   fChain->SetBranchAddress("S.dc.1v1.rawtdc", S_dc_1v1_rawtdc, &b_S_dc_1v1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1v1.tdchits", &Ndata_S_dc_1v1_tdchits, &b_Ndata_S_dc_1v1_tdchits);
   fChain->SetBranchAddress("S.dc.1v1.tdchits", S_dc_1v1_tdchits, &b_S_dc_1v1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1v1.time", &Ndata_S_dc_1v1_time, &b_Ndata_S_dc_1v1_time);
   fChain->SetBranchAddress("S.dc.1v1.time", S_dc_1v1_time, &b_S_dc_1v1_time);
   fChain->SetBranchAddress("Ndata.S.dc.1v2.dist", &Ndata_S_dc_1v2_dist, &b_Ndata_S_dc_1v2_dist);
   fChain->SetBranchAddress("S.dc.1v2.dist", S_dc_1v2_dist, &b_S_dc_1v2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1v2.rawtdc", &Ndata_S_dc_1v2_rawtdc, &b_Ndata_S_dc_1v2_rawtdc);
   fChain->SetBranchAddress("S.dc.1v2.rawtdc", S_dc_1v2_rawtdc, &b_S_dc_1v2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1v2.tdchits", &Ndata_S_dc_1v2_tdchits, &b_Ndata_S_dc_1v2_tdchits);
   fChain->SetBranchAddress("S.dc.1v2.tdchits", S_dc_1v2_tdchits, &b_S_dc_1v2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1v2.time", &Ndata_S_dc_1v2_time, &b_Ndata_S_dc_1v2_time);
   fChain->SetBranchAddress("S.dc.1v2.time", S_dc_1v2_time, &b_S_dc_1v2_time);
   fChain->SetBranchAddress("Ndata.S.dc.1x1.dist", &Ndata_S_dc_1x1_dist, &b_Ndata_S_dc_1x1_dist);
   fChain->SetBranchAddress("S.dc.1x1.dist", S_dc_1x1_dist, &b_S_dc_1x1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1x1.rawtdc", &Ndata_S_dc_1x1_rawtdc, &b_Ndata_S_dc_1x1_rawtdc);
   fChain->SetBranchAddress("S.dc.1x1.rawtdc", S_dc_1x1_rawtdc, &b_S_dc_1x1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1x1.tdchits", &Ndata_S_dc_1x1_tdchits, &b_Ndata_S_dc_1x1_tdchits);
   fChain->SetBranchAddress("S.dc.1x1.tdchits", S_dc_1x1_tdchits, &b_S_dc_1x1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1x1.time", &Ndata_S_dc_1x1_time, &b_Ndata_S_dc_1x1_time);
   fChain->SetBranchAddress("S.dc.1x1.time", S_dc_1x1_time, &b_S_dc_1x1_time);
   fChain->SetBranchAddress("Ndata.S.dc.1x2.dist", &Ndata_S_dc_1x2_dist, &b_Ndata_S_dc_1x2_dist);
   fChain->SetBranchAddress("S.dc.1x2.dist", S_dc_1x2_dist, &b_S_dc_1x2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.1x2.rawtdc", &Ndata_S_dc_1x2_rawtdc, &b_Ndata_S_dc_1x2_rawtdc);
   fChain->SetBranchAddress("S.dc.1x2.rawtdc", S_dc_1x2_rawtdc, &b_S_dc_1x2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.1x2.tdchits", &Ndata_S_dc_1x2_tdchits, &b_Ndata_S_dc_1x2_tdchits);
   fChain->SetBranchAddress("S.dc.1x2.tdchits", S_dc_1x2_tdchits, &b_S_dc_1x2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.1x2.time", &Ndata_S_dc_1x2_time, &b_Ndata_S_dc_1x2_time);
   fChain->SetBranchAddress("S.dc.1x2.time", S_dc_1x2_time, &b_S_dc_1x2_time);
   fChain->SetBranchAddress("Ndata.S.dc.2u1.dist", &Ndata_S_dc_2u1_dist, &b_Ndata_S_dc_2u1_dist);
   fChain->SetBranchAddress("S.dc.2u1.dist", S_dc_2u1_dist, &b_S_dc_2u1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2u1.rawtdc", &Ndata_S_dc_2u1_rawtdc, &b_Ndata_S_dc_2u1_rawtdc);
   fChain->SetBranchAddress("S.dc.2u1.rawtdc", S_dc_2u1_rawtdc, &b_S_dc_2u1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2u1.tdchits", &Ndata_S_dc_2u1_tdchits, &b_Ndata_S_dc_2u1_tdchits);
   fChain->SetBranchAddress("S.dc.2u1.tdchits", S_dc_2u1_tdchits, &b_S_dc_2u1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2u1.time", &Ndata_S_dc_2u1_time, &b_Ndata_S_dc_2u1_time);
   fChain->SetBranchAddress("S.dc.2u1.time", S_dc_2u1_time, &b_S_dc_2u1_time);
   fChain->SetBranchAddress("Ndata.S.dc.2u2.dist", &Ndata_S_dc_2u2_dist, &b_Ndata_S_dc_2u2_dist);
   fChain->SetBranchAddress("S.dc.2u2.dist", S_dc_2u2_dist, &b_S_dc_2u2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2u2.rawtdc", &Ndata_S_dc_2u2_rawtdc, &b_Ndata_S_dc_2u2_rawtdc);
   fChain->SetBranchAddress("S.dc.2u2.rawtdc", S_dc_2u2_rawtdc, &b_S_dc_2u2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2u2.tdchits", &Ndata_S_dc_2u2_tdchits, &b_Ndata_S_dc_2u2_tdchits);
   fChain->SetBranchAddress("S.dc.2u2.tdchits", S_dc_2u2_tdchits, &b_S_dc_2u2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2u2.time", &Ndata_S_dc_2u2_time, &b_Ndata_S_dc_2u2_time);
   fChain->SetBranchAddress("S.dc.2u2.time", S_dc_2u2_time, &b_S_dc_2u2_time);
   fChain->SetBranchAddress("Ndata.S.dc.2v1.dist", &Ndata_S_dc_2v1_dist, &b_Ndata_S_dc_2v1_dist);
   fChain->SetBranchAddress("S.dc.2v1.dist", S_dc_2v1_dist, &b_S_dc_2v1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2v1.rawtdc", &Ndata_S_dc_2v1_rawtdc, &b_Ndata_S_dc_2v1_rawtdc);
   fChain->SetBranchAddress("S.dc.2v1.rawtdc", S_dc_2v1_rawtdc, &b_S_dc_2v1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2v1.tdchits", &Ndata_S_dc_2v1_tdchits, &b_Ndata_S_dc_2v1_tdchits);
   fChain->SetBranchAddress("S.dc.2v1.tdchits", S_dc_2v1_tdchits, &b_S_dc_2v1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2v1.time", &Ndata_S_dc_2v1_time, &b_Ndata_S_dc_2v1_time);
   fChain->SetBranchAddress("S.dc.2v1.time", S_dc_2v1_time, &b_S_dc_2v1_time);
   fChain->SetBranchAddress("Ndata.S.dc.2v2.dist", &Ndata_S_dc_2v2_dist, &b_Ndata_S_dc_2v2_dist);
   fChain->SetBranchAddress("S.dc.2v2.dist", S_dc_2v2_dist, &b_S_dc_2v2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2v2.rawtdc", &Ndata_S_dc_2v2_rawtdc, &b_Ndata_S_dc_2v2_rawtdc);
   fChain->SetBranchAddress("S.dc.2v2.rawtdc", S_dc_2v2_rawtdc, &b_S_dc_2v2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2v2.tdchits", &Ndata_S_dc_2v2_tdchits, &b_Ndata_S_dc_2v2_tdchits);
   fChain->SetBranchAddress("S.dc.2v2.tdchits", S_dc_2v2_tdchits, &b_S_dc_2v2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2v2.time", &Ndata_S_dc_2v2_time, &b_Ndata_S_dc_2v2_time);
   fChain->SetBranchAddress("S.dc.2v2.time", S_dc_2v2_time, &b_S_dc_2v2_time);
   fChain->SetBranchAddress("Ndata.S.dc.2x1.dist", &Ndata_S_dc_2x1_dist, &b_Ndata_S_dc_2x1_dist);
   fChain->SetBranchAddress("S.dc.2x1.dist", S_dc_2x1_dist, &b_S_dc_2x1_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2x1.rawtdc", &Ndata_S_dc_2x1_rawtdc, &b_Ndata_S_dc_2x1_rawtdc);
   fChain->SetBranchAddress("S.dc.2x1.rawtdc", S_dc_2x1_rawtdc, &b_S_dc_2x1_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2x1.tdchits", &Ndata_S_dc_2x1_tdchits, &b_Ndata_S_dc_2x1_tdchits);
   fChain->SetBranchAddress("S.dc.2x1.tdchits", S_dc_2x1_tdchits, &b_S_dc_2x1_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2x1.time", &Ndata_S_dc_2x1_time, &b_Ndata_S_dc_2x1_time);
   fChain->SetBranchAddress("S.dc.2x1.time", S_dc_2x1_time, &b_S_dc_2x1_time);
   fChain->SetBranchAddress("Ndata.S.dc.2x2.dist", &Ndata_S_dc_2x2_dist, &b_Ndata_S_dc_2x2_dist);
   fChain->SetBranchAddress("S.dc.2x2.dist", S_dc_2x2_dist, &b_S_dc_2x2_dist);
   fChain->SetBranchAddress("Ndata.S.dc.2x2.rawtdc", &Ndata_S_dc_2x2_rawtdc, &b_Ndata_S_dc_2x2_rawtdc);
   fChain->SetBranchAddress("S.dc.2x2.rawtdc", S_dc_2x2_rawtdc, &b_S_dc_2x2_rawtdc);
   fChain->SetBranchAddress("Ndata.S.dc.2x2.tdchits", &Ndata_S_dc_2x2_tdchits, &b_Ndata_S_dc_2x2_tdchits);
   fChain->SetBranchAddress("S.dc.2x2.tdchits", S_dc_2x2_tdchits, &b_S_dc_2x2_tdchits);
   fChain->SetBranchAddress("Ndata.S.dc.2x2.time", &Ndata_S_dc_2x2_time, &b_Ndata_S_dc_2x2_time);
   fChain->SetBranchAddress("S.dc.2x2.time", S_dc_2x2_time, &b_S_dc_2x2_time);
   fChain->SetBranchAddress("Ndata.S.dc.residual", &Ndata_S_dc_residual, &b_Ndata_S_dc_residual);
   fChain->SetBranchAddress("S.dc.residual", S_dc_residual, &b_S_dc_residual);
   fChain->SetBranchAddress("Ndata.S.dc.x", &Ndata_S_dc_x, &b_Ndata_S_dc_x);
   fChain->SetBranchAddress("S.dc.x", S_dc_x, &b_S_dc_x);
   fChain->SetBranchAddress("Ndata.S.dc.xp", &Ndata_S_dc_xp, &b_Ndata_S_dc_xp);
   fChain->SetBranchAddress("S.dc.xp", S_dc_xp, &b_S_dc_xp);
   fChain->SetBranchAddress("Ndata.S.dc.y", &Ndata_S_dc_y, &b_Ndata_S_dc_y);
   fChain->SetBranchAddress("S.dc.y", S_dc_y, &b_S_dc_y);
   fChain->SetBranchAddress("Ndata.S.dc.yp", &Ndata_S_dc_yp, &b_Ndata_S_dc_yp);
   fChain->SetBranchAddress("S.dc.yp", S_dc_yp, &b_S_dc_yp);
   fChain->SetBranchAddress("Ndata.S.hod.1x.negadchits", &Ndata_S_hod_1x_negadchits, &b_Ndata_S_hod_1x_negadchits);
   fChain->SetBranchAddress("S.hod.1x.negadchits", S_hod_1x_negadchits, &b_S_hod_1x_negadchits);
   fChain->SetBranchAddress("Ndata.S.hod.1x.negtdchits", &Ndata_S_hod_1x_negtdchits, &b_Ndata_S_hod_1x_negtdchits);
   fChain->SetBranchAddress("S.hod.1x.negtdchits", S_hod_1x_negtdchits, &b_S_hod_1x_negtdchits);
   fChain->SetBranchAddress("Ndata.S.hod.1x.posadchits", &Ndata_S_hod_1x_posadchits, &b_Ndata_S_hod_1x_posadchits);
   fChain->SetBranchAddress("S.hod.1x.posadchits", S_hod_1x_posadchits, &b_S_hod_1x_posadchits);
   fChain->SetBranchAddress("Ndata.S.hod.1x.postdchits", &Ndata_S_hod_1x_postdchits, &b_Ndata_S_hod_1x_postdchits);
   fChain->SetBranchAddress("S.hod.1x.postdchits", S_hod_1x_postdchits, &b_S_hod_1x_postdchits);
   fChain->SetBranchAddress("Ndata.S.hod.1y.negadchits", &Ndata_S_hod_1y_negadchits, &b_Ndata_S_hod_1y_negadchits);
   fChain->SetBranchAddress("S.hod.1y.negadchits", S_hod_1y_negadchits, &b_S_hod_1y_negadchits);
   fChain->SetBranchAddress("Ndata.S.hod.1y.negtdchits", &Ndata_S_hod_1y_negtdchits, &b_Ndata_S_hod_1y_negtdchits);
   fChain->SetBranchAddress("S.hod.1y.negtdchits", S_hod_1y_negtdchits, &b_S_hod_1y_negtdchits);
   fChain->SetBranchAddress("Ndata.S.hod.1y.posadchits", &Ndata_S_hod_1y_posadchits, &b_Ndata_S_hod_1y_posadchits);
   fChain->SetBranchAddress("S.hod.1y.posadchits", S_hod_1y_posadchits, &b_S_hod_1y_posadchits);
   fChain->SetBranchAddress("Ndata.S.hod.1y.postdchits", &Ndata_S_hod_1y_postdchits, &b_Ndata_S_hod_1y_postdchits);
   fChain->SetBranchAddress("S.hod.1y.postdchits", S_hod_1y_postdchits, &b_S_hod_1y_postdchits);
   fChain->SetBranchAddress("Ndata.S.hod.2x.negadchits", &Ndata_S_hod_2x_negadchits, &b_Ndata_S_hod_2x_negadchits);
   fChain->SetBranchAddress("S.hod.2x.negadchits", S_hod_2x_negadchits, &b_S_hod_2x_negadchits);
   fChain->SetBranchAddress("Ndata.S.hod.2x.negtdchits", &Ndata_S_hod_2x_negtdchits, &b_Ndata_S_hod_2x_negtdchits);
   fChain->SetBranchAddress("S.hod.2x.negtdchits", S_hod_2x_negtdchits, &b_S_hod_2x_negtdchits);
   fChain->SetBranchAddress("Ndata.S.hod.2x.posadchits", &Ndata_S_hod_2x_posadchits, &b_Ndata_S_hod_2x_posadchits);
   fChain->SetBranchAddress("S.hod.2x.posadchits", S_hod_2x_posadchits, &b_S_hod_2x_posadchits);
   fChain->SetBranchAddress("Ndata.S.hod.2x.postdchits", &Ndata_S_hod_2x_postdchits, &b_Ndata_S_hod_2x_postdchits);
   fChain->SetBranchAddress("S.hod.2x.postdchits", S_hod_2x_postdchits, &b_S_hod_2x_postdchits);
   fChain->SetBranchAddress("Ndata.S.hod.2y.negadchits", &Ndata_S_hod_2y_negadchits, &b_Ndata_S_hod_2y_negadchits);
   fChain->SetBranchAddress("S.hod.2y.negadchits", S_hod_2y_negadchits, &b_S_hod_2y_negadchits);
   fChain->SetBranchAddress("Ndata.S.hod.2y.negtdchits", &Ndata_S_hod_2y_negtdchits, &b_Ndata_S_hod_2y_negtdchits);
   fChain->SetBranchAddress("S.hod.2y.negtdchits", S_hod_2y_negtdchits, &b_S_hod_2y_negtdchits);
   fChain->SetBranchAddress("Ndata.S.hod.2y.posadchits", &Ndata_S_hod_2y_posadchits, &b_Ndata_S_hod_2y_posadchits);
   fChain->SetBranchAddress("S.hod.2y.posadchits", S_hod_2y_posadchits, &b_S_hod_2y_posadchits);
   fChain->SetBranchAddress("Ndata.S.hod.2y.postdchits", &Ndata_S_hod_2y_postdchits, &b_Ndata_S_hod_2y_postdchits);
   fChain->SetBranchAddress("S.hod.2y.postdchits", S_hod_2y_postdchits, &b_S_hod_2y_postdchits);
   fChain->SetBranchAddress("Ndata.S.hod.fpHitsTime", &Ndata_S_hod_fpHitsTime, &b_Ndata_S_hod_fpHitsTime);
   fChain->SetBranchAddress("S.hod.fpHitsTime", S_hod_fpHitsTime, &b_S_hod_fpHitsTime);
   fChain->SetBranchAddress("Ndata.S.tr.beta", &Ndata_S_tr_beta, &b_Ndata_S_tr_beta);
   fChain->SetBranchAddress("S.tr.beta", S_tr_beta, &b_S_tr_beta);
   fChain->SetBranchAddress("Ndata.S.tr.betachisq", &Ndata_S_tr_betachisq, &b_Ndata_S_tr_betachisq);
   fChain->SetBranchAddress("S.tr.betachisq", S_tr_betachisq, &b_S_tr_betachisq);
   fChain->SetBranchAddress("Ndata.S.tr.chi2", &Ndata_S_tr_chi2, &b_Ndata_S_tr_chi2);
   fChain->SetBranchAddress("S.tr.chi2", S_tr_chi2, &b_S_tr_chi2);
   fChain->SetBranchAddress("Ndata.S.tr.d_ph", &Ndata_S_tr_d_ph, &b_Ndata_S_tr_d_ph);
   fChain->SetBranchAddress("S.tr.d_ph", S_tr_d_ph, &b_S_tr_d_ph);
   fChain->SetBranchAddress("Ndata.S.tr.d_th", &Ndata_S_tr_d_th, &b_Ndata_S_tr_d_th);
   fChain->SetBranchAddress("S.tr.d_th", S_tr_d_th, &b_S_tr_d_th);
   fChain->SetBranchAddress("Ndata.S.tr.d_x", &Ndata_S_tr_d_x, &b_Ndata_S_tr_d_x);
   fChain->SetBranchAddress("S.tr.d_x", S_tr_d_x, &b_S_tr_d_x);
   fChain->SetBranchAddress("Ndata.S.tr.d_y", &Ndata_S_tr_d_y, &b_Ndata_S_tr_d_y);
   fChain->SetBranchAddress("S.tr.d_y", S_tr_d_y, &b_S_tr_d_y);
   fChain->SetBranchAddress("Ndata.S.tr.dbeta", &Ndata_S_tr_dbeta, &b_Ndata_S_tr_dbeta);
   fChain->SetBranchAddress("S.tr.dbeta", S_tr_dbeta, &b_S_tr_dbeta);
   fChain->SetBranchAddress("Ndata.S.tr.dtime", &Ndata_S_tr_dtime, &b_Ndata_S_tr_dtime);
   fChain->SetBranchAddress("S.tr.dtime", S_tr_dtime, &b_S_tr_dtime);
   fChain->SetBranchAddress("Ndata.S.tr.flag", &Ndata_S_tr_flag, &b_Ndata_S_tr_flag);
   fChain->SetBranchAddress("S.tr.flag", S_tr_flag, &b_S_tr_flag);
   fChain->SetBranchAddress("Ndata.S.tr.ndof", &Ndata_S_tr_ndof, &b_Ndata_S_tr_ndof);
   fChain->SetBranchAddress("S.tr.ndof", S_tr_ndof, &b_S_tr_ndof);
   fChain->SetBranchAddress("Ndata.S.tr.p", &Ndata_S_tr_p, &b_Ndata_S_tr_p);
   fChain->SetBranchAddress("S.tr.p", S_tr_p, &b_S_tr_p);
   fChain->SetBranchAddress("Ndata.S.tr.pathl", &Ndata_S_tr_pathl, &b_Ndata_S_tr_pathl);
   fChain->SetBranchAddress("S.tr.pathl", S_tr_pathl, &b_S_tr_pathl);
   fChain->SetBranchAddress("Ndata.S.tr.ph", &Ndata_S_tr_ph, &b_Ndata_S_tr_ph);
   fChain->SetBranchAddress("S.tr.ph", S_tr_ph, &b_S_tr_ph);
   fChain->SetBranchAddress("Ndata.S.tr.px", &Ndata_S_tr_px, &b_Ndata_S_tr_px);
   fChain->SetBranchAddress("S.tr.px", S_tr_px, &b_S_tr_px);
   fChain->SetBranchAddress("Ndata.S.tr.py", &Ndata_S_tr_py, &b_Ndata_S_tr_py);
   fChain->SetBranchAddress("S.tr.py", S_tr_py, &b_S_tr_py);
   fChain->SetBranchAddress("Ndata.S.tr.pz", &Ndata_S_tr_pz, &b_Ndata_S_tr_pz);
   fChain->SetBranchAddress("S.tr.pz", S_tr_pz, &b_S_tr_pz);
   fChain->SetBranchAddress("Ndata.S.tr.r_ph", &Ndata_S_tr_r_ph, &b_Ndata_S_tr_r_ph);
   fChain->SetBranchAddress("S.tr.r_ph", S_tr_r_ph, &b_S_tr_r_ph);
   fChain->SetBranchAddress("Ndata.S.tr.r_th", &Ndata_S_tr_r_th, &b_Ndata_S_tr_r_th);
   fChain->SetBranchAddress("S.tr.r_th", S_tr_r_th, &b_S_tr_r_th);
   fChain->SetBranchAddress("Ndata.S.tr.r_x", &Ndata_S_tr_r_x, &b_Ndata_S_tr_r_x);
   fChain->SetBranchAddress("S.tr.r_x", S_tr_r_x, &b_S_tr_r_x);
   fChain->SetBranchAddress("Ndata.S.tr.r_y", &Ndata_S_tr_r_y, &b_Ndata_S_tr_r_y);
   fChain->SetBranchAddress("S.tr.r_y", S_tr_r_y, &b_S_tr_r_y);
   fChain->SetBranchAddress("Ndata.S.tr.tg_dp", &Ndata_S_tr_tg_dp, &b_Ndata_S_tr_tg_dp);
   fChain->SetBranchAddress("S.tr.tg_dp", S_tr_tg_dp, &b_S_tr_tg_dp);
   fChain->SetBranchAddress("Ndata.S.tr.tg_ph", &Ndata_S_tr_tg_ph, &b_Ndata_S_tr_tg_ph);
   fChain->SetBranchAddress("S.tr.tg_ph", S_tr_tg_ph, &b_S_tr_tg_ph);
   fChain->SetBranchAddress("Ndata.S.tr.tg_th", &Ndata_S_tr_tg_th, &b_Ndata_S_tr_tg_th);
   fChain->SetBranchAddress("S.tr.tg_th", S_tr_tg_th, &b_S_tr_tg_th);
   fChain->SetBranchAddress("Ndata.S.tr.tg_y", &Ndata_S_tr_tg_y, &b_Ndata_S_tr_tg_y);
   fChain->SetBranchAddress("S.tr.tg_y", S_tr_tg_y, &b_S_tr_tg_y);
   fChain->SetBranchAddress("Ndata.S.tr.th", &Ndata_S_tr_th, &b_Ndata_S_tr_th);
   fChain->SetBranchAddress("S.tr.th", S_tr_th, &b_S_tr_th);
   fChain->SetBranchAddress("Ndata.S.tr.time", &Ndata_S_tr_time, &b_Ndata_S_tr_time);
   fChain->SetBranchAddress("S.tr.time", S_tr_time, &b_S_tr_time);
   fChain->SetBranchAddress("Ndata.S.tr.vx", &Ndata_S_tr_vx, &b_Ndata_S_tr_vx);
   fChain->SetBranchAddress("S.tr.vx", S_tr_vx, &b_S_tr_vx);
   fChain->SetBranchAddress("Ndata.S.tr.vy", &Ndata_S_tr_vy, &b_Ndata_S_tr_vy);
   fChain->SetBranchAddress("S.tr.vy", S_tr_vy, &b_S_tr_vy);
   fChain->SetBranchAddress("Ndata.S.tr.vz", &Ndata_S_tr_vz, &b_Ndata_S_tr_vz);
   fChain->SetBranchAddress("S.tr.vz", S_tr_vz, &b_S_tr_vz);
   fChain->SetBranchAddress("Ndata.S.tr.x", &Ndata_S_tr_x, &b_Ndata_S_tr_x);
   fChain->SetBranchAddress("S.tr.x", S_tr_x, &b_S_tr_x);
   fChain->SetBranchAddress("Ndata.S.tr.y", &Ndata_S_tr_y, &b_Ndata_S_tr_y);
   fChain->SetBranchAddress("S.tr.y", S_tr_y, &b_S_tr_y);
   fChain->SetBranchAddress("H.aero.neg_npe_sum", &H_aero_neg_npe_sum, &b_H_aero_neg_npe_sum);
   fChain->SetBranchAddress("H.aero.ngood_hits", &H_aero_ngood_hits, &b_H_aero_ngood_hits);
   fChain->SetBranchAddress("H.aero.npe_sum", &H_aero_npe_sum, &b_H_aero_npe_sum);
   fChain->SetBranchAddress("H.aero.ntdc_neg_hits", &H_aero_ntdc_neg_hits, &b_H_aero_ntdc_neg_hits);
   fChain->SetBranchAddress("H.aero.ntdc_pos_hits", &H_aero_ntdc_pos_hits, &b_H_aero_ntdc_pos_hits);
   fChain->SetBranchAddress("H.aero.pos_npe_sum", &H_aero_pos_npe_sum, &b_H_aero_pos_npe_sum);
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
   fChain->SetBranchAddress("H.cal.etot", &H_cal_etot, &b_H_cal_etot);
   fChain->SetBranchAddress("H.cal.etotnorm", &H_cal_etotnorm, &b_H_cal_etotnorm);
   fChain->SetBranchAddress("H.cal.etrack", &H_cal_etrack, &b_H_cal_etrack);
   fChain->SetBranchAddress("H.cal.nclust", &H_cal_nclust, &b_H_cal_nclust);
   fChain->SetBranchAddress("H.cal.nhits", &H_cal_nhits, &b_H_cal_nhits);
   fChain->SetBranchAddress("H.cal.ntracks", &H_cal_ntracks, &b_H_cal_ntracks);
   fChain->SetBranchAddress("H.cher.ncherhit", &H_cher_ncherhit, &b_H_cher_ncherhit);
   fChain->SetBranchAddress("H.cher.npesum", &H_cher_npesum, &b_H_cher_npesum);
   fChain->SetBranchAddress("H.dc.1u1.nhit", &H_dc_1u1_nhit, &b_H_dc_1u1_nhit);
   fChain->SetBranchAddress("H.dc.1v1.nhit", &H_dc_1v1_nhit, &b_H_dc_1v1_nhit);
   fChain->SetBranchAddress("H.dc.1x1.nhit", &H_dc_1x1_nhit, &b_H_dc_1x1_nhit);
   fChain->SetBranchAddress("H.dc.1x2.nhit", &H_dc_1x2_nhit, &b_H_dc_1x2_nhit);
   fChain->SetBranchAddress("H.dc.1y1.nhit", &H_dc_1y1_nhit, &b_H_dc_1y1_nhit);
   fChain->SetBranchAddress("H.dc.1y2.nhit", &H_dc_1y2_nhit, &b_H_dc_1y2_nhit);
   fChain->SetBranchAddress("H.dc.2u1.nhit", &H_dc_2u1_nhit, &b_H_dc_2u1_nhit);
   fChain->SetBranchAddress("H.dc.2v1.nhit", &H_dc_2v1_nhit, &b_H_dc_2v1_nhit);
   fChain->SetBranchAddress("H.dc.2x1.nhit", &H_dc_2x1_nhit, &b_H_dc_2x1_nhit);
   fChain->SetBranchAddress("H.dc.2x2.nhit", &H_dc_2x2_nhit, &b_H_dc_2x2_nhit);
   fChain->SetBranchAddress("H.dc.2y1.nhit", &H_dc_2y1_nhit, &b_H_dc_2y1_nhit);
   fChain->SetBranchAddress("H.dc.2y2.nhit", &H_dc_2y2_nhit, &b_H_dc_2y2_nhit);
   fChain->SetBranchAddress("H.dc.Ch1.maxhits", &H_dc_Ch1_maxhits, &b_H_dc_Ch1_maxhits);
   fChain->SetBranchAddress("H.dc.Ch1.nhit", &H_dc_Ch1_nhit, &b_H_dc_Ch1_nhit);
   fChain->SetBranchAddress("H.dc.Ch1.spacepoints", &H_dc_Ch1_spacepoints, &b_H_dc_Ch1_spacepoints);
   fChain->SetBranchAddress("H.dc.Ch1.trawhit", &H_dc_Ch1_trawhit, &b_H_dc_Ch1_trawhit);
   fChain->SetBranchAddress("H.dc.Ch2.maxhits", &H_dc_Ch2_maxhits, &b_H_dc_Ch2_maxhits);
   fChain->SetBranchAddress("H.dc.Ch2.nhit", &H_dc_Ch2_nhit, &b_H_dc_Ch2_nhit);
   fChain->SetBranchAddress("H.dc.Ch2.spacepoints", &H_dc_Ch2_spacepoints, &b_H_dc_Ch2_spacepoints);
   fChain->SetBranchAddress("H.dc.Ch2.trawhit", &H_dc_Ch2_trawhit, &b_H_dc_Ch2_trawhit);
   fChain->SetBranchAddress("H.dc.nhit", &H_dc_nhit, &b_H_dc_nhit);
   fChain->SetBranchAddress("H.dc.nsp", &H_dc_nsp, &b_H_dc_nsp);
   fChain->SetBranchAddress("H.dc.ntrack", &H_dc_ntrack, &b_H_dc_ntrack);
   fChain->SetBranchAddress("H.dc.stubtest", &H_dc_stubtest, &b_H_dc_stubtest);
   fChain->SetBranchAddress("H.dc.tnhit", &H_dc_tnhit, &b_H_dc_tnhit);
   fChain->SetBranchAddress("H.dc.trawhit", &H_dc_trawhit, &b_H_dc_trawhit);
   fChain->SetBranchAddress("H.hod.1x.nhits", &H_hod_1x_nhits, &b_H_hod_1x_nhits);
   fChain->SetBranchAddress("H.hod.1y.nhits", &H_hod_1y_nhits, &b_H_hod_1y_nhits);
   fChain->SetBranchAddress("H.hod.2x.nhits", &H_hod_2x_nhits, &b_H_hod_2x_nhits);
   fChain->SetBranchAddress("H.hod.2y.nhits", &H_hod_2y_nhits, &b_H_hod_2y_nhits);
   fChain->SetBranchAddress("H.hod.betachisqnotrack", &H_hod_betachisqnotrack, &b_H_hod_betachisqnotrack);
   fChain->SetBranchAddress("H.hod.betanotrack", &H_hod_betanotrack, &b_H_hod_betanotrack);
   fChain->SetBranchAddress("H.hod.betap", &H_hod_betap, &b_H_hod_betap);
   fChain->SetBranchAddress("H.hod.goodscinhit", &H_hod_goodscinhit, &b_H_hod_goodscinhit);
   fChain->SetBranchAddress("H.hod.goodstarttime", &H_hod_goodstarttime, &b_H_hod_goodstarttime);
   fChain->SetBranchAddress("H.hod.scindid", &H_hod_scindid, &b_H_hod_scindid);
   fChain->SetBranchAddress("H.hod.scinshould", &H_hod_scinshould, &b_H_hod_scinshould);
   fChain->SetBranchAddress("H.hod.starttime", &H_hod_starttime, &b_H_hod_starttime);
   fChain->SetBranchAddress("H.tr.n", &H_tr_n, &b_H_tr_n);
   fChain->SetBranchAddress("HSCOINPRE", &HSCOINPRE, &b_HSCOINPRE);
   fChain->SetBranchAddress("HSCOINPREr", &HSCOINPREr, &b_HSCOINPREr);
   fChain->SetBranchAddress("HSCOINTRG", &HSCOINTRG, &b_HSCOINTRG);
   fChain->SetBranchAddress("HSCOINTRGr", &HSCOINTRGr, &b_HSCOINTRGr);
   fChain->SetBranchAddress("HSHMSPRE", &HSHMSPRE, &b_HSHMSPRE);
   fChain->SetBranchAddress("HSHMSPREr", &HSHMSPREr, &b_HSHMSPREr);
   fChain->SetBranchAddress("HSHMSTRG", &HSHMSTRG, &b_HSHMSTRG);
   fChain->SetBranchAddress("HSHMSTRGr", &HSHMSTRGr, &b_HSHMSTRGr);
   fChain->SetBranchAddress("HSPEDPRE", &HSPEDPRE, &b_HSPEDPRE);
   fChain->SetBranchAddress("HSPEDPREr", &HSPEDPREr, &b_HSPEDPREr);
   fChain->SetBranchAddress("HSPEDTRG", &HSPEDTRG, &b_HSPEDTRG);
   fChain->SetBranchAddress("HSPEDTRGr", &HSPEDTRGr, &b_HSPEDTRGr);
   fChain->SetBranchAddress("HSS1", &HSS1, &b_HSS1);
   fChain->SetBranchAddress("HSS1X", &HSS1X, &b_HSS1X);
   fChain->SetBranchAddress("HSS1Xr", &HSS1Xr, &b_HSS1Xr);
   fChain->SetBranchAddress("HSS1Y", &HSS1Y, &b_HSS1Y);
   fChain->SetBranchAddress("HSS1Yr", &HSS1Yr, &b_HSS1Yr);
   fChain->SetBranchAddress("HSS1r", &HSS1r, &b_HSS1r);
   fChain->SetBranchAddress("HSS2", &HSS2, &b_HSS2);
   fChain->SetBranchAddress("HSS2X", &HSS2X, &b_HSS2X);
   fChain->SetBranchAddress("HSS2Xr", &HSS2Xr, &b_HSS2Xr);
   fChain->SetBranchAddress("HSS2Y", &HSS2Y, &b_HSS2Y);
   fChain->SetBranchAddress("HSS2Yr", &HSS2Yr, &b_HSS2Yr);
   fChain->SetBranchAddress("HSS2r", &HSS2r, &b_HSS2r);
   fChain->SetBranchAddress("HSSHODO11", &HSSHODO11, &b_HSSHODO11);
   fChain->SetBranchAddress("HSSHODO11r", &HSSHODO11r, &b_HSSHODO11r);
   fChain->SetBranchAddress("HSSHODO13", &HSSHODO13, &b_HSSHODO13);
   fChain->SetBranchAddress("HSSHODO13r", &HSSHODO13r, &b_HSSHODO13r);
   fChain->SetBranchAddress("HSSHODO15", &HSSHODO15, &b_HSSHODO15);
   fChain->SetBranchAddress("HSSHODO15r", &HSSHODO15r, &b_HSSHODO15r);
   fChain->SetBranchAddress("HSSHODO9", &HSSHODO9, &b_HSSHODO9);
   fChain->SetBranchAddress("HSSHODO9r", &HSSHODO9r, &b_HSSHODO9r);
   fChain->SetBranchAddress("HSSOSPRE", &HSSOSPRE, &b_HSSOSPRE);
   fChain->SetBranchAddress("HSSOSPREr", &HSSOSPREr, &b_HSSOSPREr);
   fChain->SetBranchAddress("HSSOSTRG", &HSSOSTRG, &b_HSSOSTRG);
   fChain->SetBranchAddress("HSSOSTRGr", &HSSOSTRGr, &b_HSSOSTRGr);
   fChain->SetBranchAddress("HShADCGATES", &HShADCGATES, &b_HShADCGATES);
   fChain->SetBranchAddress("HShADCGATESr", &HShADCGATESr, &b_HShADCGATESr);
   fChain->SetBranchAddress("HShBCM1", &HShBCM1, &b_HShBCM1);
   fChain->SetBranchAddress("HShBCM1r", &HShBCM1r, &b_HShBCM1r);
   fChain->SetBranchAddress("HShBCM2", &HShBCM2, &b_HShBCM2);
   fChain->SetBranchAddress("HShBCM2r", &HShBCM2r, &b_HShBCM2r);
   fChain->SetBranchAddress("HShBEAMGATES", &HShBEAMGATES, &b_HShBEAMGATES);
   fChain->SetBranchAddress("HShBEAMGATESr", &HShBEAMGATESr, &b_HShBEAMGATESr);
   fChain->SetBranchAddress("HShCER", &HShCER, &b_HShCER);
   fChain->SetBranchAddress("HShCERHI", &HShCERHI, &b_HShCERHI);
   fChain->SetBranchAddress("HShCERHIr", &HShCERHIr, &b_HShCERHIr);
   fChain->SetBranchAddress("HShCERr", &HShCERr, &b_HShCERr);
   fChain->SetBranchAddress("HShCLOCK", &HShCLOCK, &b_HShCLOCK);
   fChain->SetBranchAddress("HShCLOCKr", &HShCLOCKr, &b_HShCLOCKr);
   fChain->SetBranchAddress("HShELCLEAN", &HShELCLEAN, &b_HShELCLEAN);
   fChain->SetBranchAddress("HShELCLEANr", &HShELCLEANr, &b_HShELCLEANr);
   fChain->SetBranchAddress("HShELHI", &HShELHI, &b_HShELHI);
   fChain->SetBranchAddress("HShELHIr", &HShELHIr, &b_HShELHIr);
   fChain->SetBranchAddress("HShELLO", &HShELLO, &b_HShELLO);
   fChain->SetBranchAddress("HShELLOr", &HShELLOr, &b_HShELLOr);
   fChain->SetBranchAddress("HShELREAL", &HShELREAL, &b_HShELREAL);
   fChain->SetBranchAddress("HShELREALr", &HShELREALr, &b_HShELREALr);
   fChain->SetBranchAddress("HShPION", &HShPION, &b_HShPION);
   fChain->SetBranchAddress("HShPIONHI", &HShPIONHI, &b_HShPIONHI);
   fChain->SetBranchAddress("HShPIONHIr", &HShPIONHIr, &b_HShPIONHIr);
   fChain->SetBranchAddress("HShPIONr", &HShPIONr, &b_HShPIONr);
   fChain->SetBranchAddress("HShPIPRE", &HShPIPRE, &b_HShPIPRE);
   fChain->SetBranchAddress("HShPIPREr", &HShPIPREr, &b_HShPIPREr);
   fChain->SetBranchAddress("HShPRE100", &HShPRE100, &b_HShPRE100);
   fChain->SetBranchAddress("HShPRE100r", &HShPRE100r, &b_HShPRE100r);
   fChain->SetBranchAddress("HShPRE150", &HShPRE150, &b_HShPRE150);
   fChain->SetBranchAddress("HShPRE150r", &HShPRE150r, &b_HShPRE150r);
   fChain->SetBranchAddress("HShPRE200", &HShPRE200, &b_HShPRE200);
   fChain->SetBranchAddress("HShPRE200r", &HShPRE200r, &b_HShPRE200r);
   fChain->SetBranchAddress("HShPRE50", &HShPRE50, &b_HShPRE50);
   fChain->SetBranchAddress("HShPRE50r", &HShPRE50r, &b_HShPRE50r);
   fChain->SetBranchAddress("HShPRETRG", &HShPRETRG, &b_HShPRETRG);
   fChain->SetBranchAddress("HShPRETRGr", &HShPRETRGr, &b_HShPRETRGr);
   fChain->SetBranchAddress("HShPRHI", &HShPRHI, &b_HShPRHI);
   fChain->SetBranchAddress("HShPRHIr", &HShPRHIr, &b_HShPRHIr);
   fChain->SetBranchAddress("HShPRLO", &HShPRLO, &b_HShPRLO);
   fChain->SetBranchAddress("HShPRLOr", &HShPRLOr, &b_HShPRLOr);
   fChain->SetBranchAddress("HShS1", &HShS1, &b_HShS1);
   fChain->SetBranchAddress("HShS1X", &HShS1X, &b_HShS1X);
   fChain->SetBranchAddress("HShS1Xr", &HShS1Xr, &b_HShS1Xr);
   fChain->SetBranchAddress("HShS1Y", &HShS1Y, &b_HShS1Y);
   fChain->SetBranchAddress("HShS1Yr", &HShS1Yr, &b_HShS1Yr);
   fChain->SetBranchAddress("HShS1r", &HShS1r, &b_HShS1r);
   fChain->SetBranchAddress("HShS2", &HShS2, &b_HShS2);
   fChain->SetBranchAddress("HShS2X", &HShS2X, &b_HShS2X);
   fChain->SetBranchAddress("HShS2Xr", &HShS2Xr, &b_HShS2Xr);
   fChain->SetBranchAddress("HShS2Y", &HShS2Y, &b_HShS2Y);
   fChain->SetBranchAddress("HShS2Yr", &HShS2Yr, &b_HShS2Yr);
   fChain->SetBranchAddress("HShS2r", &HShS2r, &b_HShS2r);
   fChain->SetBranchAddress("HShSCIN", &HShSCIN, &b_HShSCIN);
   fChain->SetBranchAddress("HShSCINr", &HShSCINr, &b_HShSCINr);
   fChain->SetBranchAddress("HShSHLO", &HShSHLO, &b_HShSHLO);
   fChain->SetBranchAddress("HShSHLOr", &HShSHLOr, &b_HShSHLOr);
   fChain->SetBranchAddress("HShSTOF", &HShSTOF, &b_HShSTOF);
   fChain->SetBranchAddress("HShSTOFr", &HShSTOFr, &b_HShSTOFr);
   fChain->SetBranchAddress("HShUNSER", &HShUNSER, &b_HShUNSER);
   fChain->SetBranchAddress("HShUNSERr", &HShUNSERr, &b_HShUNSERr);
   fChain->SetBranchAddress("HShUNUSED4", &HShUNUSED4, &b_HShUNUSED4);
   fChain->SetBranchAddress("HShUNUSED4r", &HShUNUSED4r, &b_HShUNUSED4r);
   fChain->SetBranchAddress("HShUNUSED5", &HShUNUSED5, &b_HShUNUSED5);
   fChain->SetBranchAddress("HShUNUSED5r", &HShUNUSED5r, &b_HShUNUSED5r);
   fChain->SetBranchAddress("HSsADCGATES", &HSsADCGATES, &b_HSsADCGATES);
   fChain->SetBranchAddress("HSsADCGATESr", &HSsADCGATESr, &b_HSsADCGATESr);
   fChain->SetBranchAddress("HSsBCM1", &HSsBCM1, &b_HSsBCM1);
   fChain->SetBranchAddress("HSsBCM1r", &HSsBCM1r, &b_HSsBCM1r);
   fChain->SetBranchAddress("HSsBCM2", &HSsBCM2, &b_HSsBCM2);
   fChain->SetBranchAddress("HSsBCM2r", &HSsBCM2r, &b_HSsBCM2r);
   fChain->SetBranchAddress("HSsCER", &HSsCER, &b_HSsCER);
   fChain->SetBranchAddress("HSsCERr", &HSsCERr, &b_HSsCERr);
   fChain->SetBranchAddress("HSsCLOCK", &HSsCLOCK, &b_HSsCLOCK);
   fChain->SetBranchAddress("HSsCLOCKr", &HSsCLOCKr, &b_HSsCLOCKr);
   fChain->SetBranchAddress("HSsELCLEAN", &HSsELCLEAN, &b_HSsELCLEAN);
   fChain->SetBranchAddress("HSsELCLEANr", &HSsELCLEANr, &b_HSsELCLEANr);
   fChain->SetBranchAddress("HSsELHI", &HSsELHI, &b_HSsELHI);
   fChain->SetBranchAddress("HSsELHIr", &HSsELHIr, &b_HSsELHIr);
   fChain->SetBranchAddress("HSsELLO", &HSsELLO, &b_HSsELLO);
   fChain->SetBranchAddress("HSsELLOr", &HSsELLOr, &b_HSsELLOr);
   fChain->SetBranchAddress("HSsELREAL", &HSsELREAL, &b_HSsELREAL);
   fChain->SetBranchAddress("HSsELREALr", &HSsELREALr, &b_HSsELREALr);
   fChain->SetBranchAddress("HSsPION", &HSsPION, &b_HSsPION);
   fChain->SetBranchAddress("HSsPIONr", &HSsPIONr, &b_HSsPIONr);
   fChain->SetBranchAddress("HSsPIPRE", &HSsPIPRE, &b_HSsPIPRE);
   fChain->SetBranchAddress("HSsPIPREr", &HSsPIPREr, &b_HSsPIPREr);
   fChain->SetBranchAddress("HSsPRE100", &HSsPRE100, &b_HSsPRE100);
   fChain->SetBranchAddress("HSsPRE100r", &HSsPRE100r, &b_HSsPRE100r);
   fChain->SetBranchAddress("HSsPRE150", &HSsPRE150, &b_HSsPRE150);
   fChain->SetBranchAddress("HSsPRE150r", &HSsPRE150r, &b_HSsPRE150r);
   fChain->SetBranchAddress("HSsPRE200", &HSsPRE200, &b_HSsPRE200);
   fChain->SetBranchAddress("HSsPRE200r", &HSsPRE200r, &b_HSsPRE200r);
   fChain->SetBranchAddress("HSsPRE50", &HSsPRE50, &b_HSsPRE50);
   fChain->SetBranchAddress("HSsPRE50r", &HSsPRE50r, &b_HSsPRE50r);
   fChain->SetBranchAddress("HSsPRETRG", &HSsPRETRG, &b_HSsPRETRG);
   fChain->SetBranchAddress("HSsPRETRGr", &HSsPRETRGr, &b_HSsPRETRGr);
   fChain->SetBranchAddress("HSsPRHI", &HSsPRHI, &b_HSsPRHI);
   fChain->SetBranchAddress("HSsPRHIr", &HSsPRHIr, &b_HSsPRHIr);
   fChain->SetBranchAddress("HSsPRLO", &HSsPRLO, &b_HSsPRLO);
   fChain->SetBranchAddress("HSsPRLOr", &HSsPRLOr, &b_HSsPRLOr);
   fChain->SetBranchAddress("HSsS1", &HSsS1, &b_HSsS1);
   fChain->SetBranchAddress("HSsS1X", &HSsS1X, &b_HSsS1X);
   fChain->SetBranchAddress("HSsS1Xr", &HSsS1Xr, &b_HSsS1Xr);
   fChain->SetBranchAddress("HSsS1Y", &HSsS1Y, &b_HSsS1Y);
   fChain->SetBranchAddress("HSsS1Yr", &HSsS1Yr, &b_HSsS1Yr);
   fChain->SetBranchAddress("HSsS1r", &HSsS1r, &b_HSsS1r);
   fChain->SetBranchAddress("HSsS2", &HSsS2, &b_HSsS2);
   fChain->SetBranchAddress("HSsS2X", &HSsS2X, &b_HSsS2X);
   fChain->SetBranchAddress("HSsS2Xr", &HSsS2Xr, &b_HSsS2Xr);
   fChain->SetBranchAddress("HSsS2Y", &HSsS2Y, &b_HSsS2Y);
   fChain->SetBranchAddress("HSsS2Yr", &HSsS2Yr, &b_HSsS2Yr);
   fChain->SetBranchAddress("HSsS2r", &HSsS2r, &b_HSsS2r);
   fChain->SetBranchAddress("HSsSCIN", &HSsSCIN, &b_HSsSCIN);
   fChain->SetBranchAddress("HSsSCINr", &HSsSCINr, &b_HSsSCINr);
   fChain->SetBranchAddress("HSsSHLO", &HSsSHLO, &b_HSsSHLO);
   fChain->SetBranchAddress("HSsSHLOr", &HSsSHLOr, &b_HSsSHLOr);
   fChain->SetBranchAddress("HSsSTOF", &HSsSTOF, &b_HSsSTOF);
   fChain->SetBranchAddress("HSsSTOFr", &HSsSTOFr, &b_HSsSTOFr);
   fChain->SetBranchAddress("S.cal.1pr.eplane", &S_cal_1pr_eplane, &b_S_cal_1pr_eplane);
   fChain->SetBranchAddress("S.cal.1pr.eplane_neg", &S_cal_1pr_eplane_neg, &b_S_cal_1pr_eplane_neg);
   fChain->SetBranchAddress("S.cal.1pr.eplane_pos", &S_cal_1pr_eplane_pos, &b_S_cal_1pr_eplane_pos);
   fChain->SetBranchAddress("S.cal.2ta.earray", &S_cal_2ta_earray, &b_S_cal_2ta_earray);
   fChain->SetBranchAddress("S.cal.2ta.nclust", &S_cal_2ta_nclust, &b_S_cal_2ta_nclust);
   fChain->SetBranchAddress("S.cal.2ta.nhits", &S_cal_2ta_nhits, &b_S_cal_2ta_nhits);
   fChain->SetBranchAddress("S.cal.2ta.ntracks", &S_cal_2ta_ntracks, &b_S_cal_2ta_ntracks);
   fChain->SetBranchAddress("S.cal.etot", &S_cal_etot, &b_S_cal_etot);
   fChain->SetBranchAddress("S.cal.etotnorm", &S_cal_etotnorm, &b_S_cal_etotnorm);
   fChain->SetBranchAddress("S.cal.etrack", &S_cal_etrack, &b_S_cal_etrack);
   fChain->SetBranchAddress("S.cal.nclust", &S_cal_nclust, &b_S_cal_nclust);
   fChain->SetBranchAddress("S.cal.nhits", &S_cal_nhits, &b_S_cal_nhits);
   fChain->SetBranchAddress("S.cal.ntracks", &S_cal_ntracks, &b_S_cal_ntracks);
   fChain->SetBranchAddress("S.dc.1u1.nhit", &S_dc_1u1_nhit, &b_S_dc_1u1_nhit);
   fChain->SetBranchAddress("S.dc.1u2.nhit", &S_dc_1u2_nhit, &b_S_dc_1u2_nhit);
   fChain->SetBranchAddress("S.dc.1v1.nhit", &S_dc_1v1_nhit, &b_S_dc_1v1_nhit);
   fChain->SetBranchAddress("S.dc.1v2.nhit", &S_dc_1v2_nhit, &b_S_dc_1v2_nhit);
   fChain->SetBranchAddress("S.dc.1x1.nhit", &S_dc_1x1_nhit, &b_S_dc_1x1_nhit);
   fChain->SetBranchAddress("S.dc.1x2.nhit", &S_dc_1x2_nhit, &b_S_dc_1x2_nhit);
   fChain->SetBranchAddress("S.dc.2u1.nhit", &S_dc_2u1_nhit, &b_S_dc_2u1_nhit);
   fChain->SetBranchAddress("S.dc.2u2.nhit", &S_dc_2u2_nhit, &b_S_dc_2u2_nhit);
   fChain->SetBranchAddress("S.dc.2v1.nhit", &S_dc_2v1_nhit, &b_S_dc_2v1_nhit);
   fChain->SetBranchAddress("S.dc.2v2.nhit", &S_dc_2v2_nhit, &b_S_dc_2v2_nhit);
   fChain->SetBranchAddress("S.dc.2x1.nhit", &S_dc_2x1_nhit, &b_S_dc_2x1_nhit);
   fChain->SetBranchAddress("S.dc.2x2.nhit", &S_dc_2x2_nhit, &b_S_dc_2x2_nhit);
   fChain->SetBranchAddress("S.dc.Ch1.maxhits", &S_dc_Ch1_maxhits, &b_S_dc_Ch1_maxhits);
   fChain->SetBranchAddress("S.dc.Ch1.nhit", &S_dc_Ch1_nhit, &b_S_dc_Ch1_nhit);
   fChain->SetBranchAddress("S.dc.Ch1.spacepoints", &S_dc_Ch1_spacepoints, &b_S_dc_Ch1_spacepoints);
   fChain->SetBranchAddress("S.dc.Ch1.trawhit", &S_dc_Ch1_trawhit, &b_S_dc_Ch1_trawhit);
   fChain->SetBranchAddress("S.dc.Ch2.maxhits", &S_dc_Ch2_maxhits, &b_S_dc_Ch2_maxhits);
   fChain->SetBranchAddress("S.dc.Ch2.nhit", &S_dc_Ch2_nhit, &b_S_dc_Ch2_nhit);
   fChain->SetBranchAddress("S.dc.Ch2.spacepoints", &S_dc_Ch2_spacepoints, &b_S_dc_Ch2_spacepoints);
   fChain->SetBranchAddress("S.dc.Ch2.trawhit", &S_dc_Ch2_trawhit, &b_S_dc_Ch2_trawhit);
   fChain->SetBranchAddress("S.dc.nhit", &S_dc_nhit, &b_S_dc_nhit);
   fChain->SetBranchAddress("S.dc.nsp", &S_dc_nsp, &b_S_dc_nsp);
   fChain->SetBranchAddress("S.dc.ntrack", &S_dc_ntrack, &b_S_dc_ntrack);
   fChain->SetBranchAddress("S.dc.stubtest", &S_dc_stubtest, &b_S_dc_stubtest);
   fChain->SetBranchAddress("S.dc.tnhit", &S_dc_tnhit, &b_S_dc_tnhit);
   fChain->SetBranchAddress("S.dc.trawhit", &S_dc_trawhit, &b_S_dc_trawhit);
   fChain->SetBranchAddress("S.hod.1x.nhits", &S_hod_1x_nhits, &b_S_hod_1x_nhits);
   fChain->SetBranchAddress("S.hod.1y.nhits", &S_hod_1y_nhits, &b_S_hod_1y_nhits);
   fChain->SetBranchAddress("S.hod.2x.nhits", &S_hod_2x_nhits, &b_S_hod_2x_nhits);
   fChain->SetBranchAddress("S.hod.2y.nhits", &S_hod_2y_nhits, &b_S_hod_2y_nhits);
   fChain->SetBranchAddress("S.hod.betachisqnotrack", &S_hod_betachisqnotrack, &b_S_hod_betachisqnotrack);
   fChain->SetBranchAddress("S.hod.betanotrack", &S_hod_betanotrack, &b_S_hod_betanotrack);
   fChain->SetBranchAddress("S.hod.betap", &S_hod_betap, &b_S_hod_betap);
   fChain->SetBranchAddress("S.hod.goodscinhit", &S_hod_goodscinhit, &b_S_hod_goodscinhit);
   fChain->SetBranchAddress("S.hod.goodstarttime", &S_hod_goodstarttime, &b_S_hod_goodstarttime);
   fChain->SetBranchAddress("S.hod.scindid", &S_hod_scindid, &b_S_hod_scindid);
   fChain->SetBranchAddress("S.hod.scinshould", &S_hod_scinshould, &b_S_hod_scinshould);
   fChain->SetBranchAddress("S.hod.starttime", &S_hod_starttime, &b_S_hod_starttime);
   fChain->SetBranchAddress("S.tr.n", &S_tr_n, &b_S_tr_n);
   fChain->SetBranchAddress("g.evtyp", &g_evtyp, &b_g_evtyp);
//    fChain->SetBranchAddress("Hhodtdif1", &Hhodtdif1, &b_Hhodtdif1);
//    fChain->SetBranchAddress("Hhodtdif2", &Hhodtdif2, &b_Hhodtdif2);
//    fChain->SetBranchAddress("Hhodtdif3", &Hhodtdif3, &b_Hhodtdif3);
//    fChain->SetBranchAddress("Hhodtdif4", &Hhodtdif4, &b_Hhodtdif4);
//    fChain->SetBranchAddress("Hhodtdif5", &Hhodtdif5, &b_Hhodtdif5);
//    fChain->SetBranchAddress("Hhodtdif6", &Hhodtdif6, &b_Hhodtdif6);
   fChain->SetBranchAddress("fEvtHdr.fEvtTime", &fEvtHdr_fEvtTime, &b_Event_Branch_fEvtHdr_fEvtTime);
   fChain->SetBranchAddress("fEvtHdr.fEvtNum", &fEvtHdr_fEvtNum, &b_Event_Branch_fEvtHdr_fEvtNum);
   fChain->SetBranchAddress("fEvtHdr.fEvtType", &fEvtHdr_fEvtType, &b_Event_Branch_fEvtHdr_fEvtType);
   fChain->SetBranchAddress("fEvtHdr.fEvtLen", &fEvtHdr_fEvtLen, &b_Event_Branch_fEvtHdr_fEvtLen);
   fChain->SetBranchAddress("fEvtHdr.fHelicity", &fEvtHdr_fHelicity, &b_Event_Branch_fEvtHdr_fHelicity);
   fChain->SetBranchAddress("fEvtHdr.fTargetPol", &fEvtHdr_fTargetPol, &b_Event_Branch_fEvtHdr_fTargetPol);
   fChain->SetBranchAddress("fEvtHdr.fRun", &fEvtHdr_fRun, &b_Event_Branch_fEvtHdr_fRun);
   Notify();
}

Bool_t shms_class::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void shms_class::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t shms_class::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef shms_class_cxx
