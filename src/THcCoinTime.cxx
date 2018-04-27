/** \class THcCoinTime
    \ingroup PhysMods

\brief Class for calculating and adding the Coincidence Time in the Tree.

*/
#include "THaEvData.h"
#include "THaCutList.h"
#include "VarDef.h"
#include "VarType.h"
#include "TClonesArray.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcCoinTime.h"
#include "THcTrigDet.h"
#include "THaApparatus.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"

using namespace std;

//_____________________________________________________________________________
THcCoinTime::THcCoinTime (const char *name, const char* description, const char* hadArmName, 
			  const char* elecArmName, const char* coinname) :
  
  THaPhysicsModule(name, description), 
  fhadArmName(hadArmName),                 //initialize spectro names
  felecArmName(elecArmName),
  fCoinDetName(coinname), 
  fhadSpectro(NULL),                      //initialize spectro objects
  felecSpectro(NULL),
  fCoinDet(NULL)
{

}

//_____________________________________________________________________________
THcCoinTime::~THcCoinTime()
{
  //Destructor

  RemoveVariables();

}

//_____________________________________________________________________________
void THcCoinTime::Reset( Option_t* opt)
// Clear event-by-event data
{
  Clear(opt);
}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCoinTime::Init( const TDatime& run_time )
{
  // Initialize THcCoinTime physics module
  
  cout << "*************************************************" << endl;
  cout << "Initializing THcCointTime Physics Modue" << endl;
  cout << "Hadron Arm   -------> " << fhadArmName << endl;
  cout << "Electron Arm -------> " << felecArmName << endl;
  cout << "**************************************************" << endl;

  fStatus = kOK;

  fhadSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( fhadArmName.Data(), "THcHallCSpectrometer"));
  if( !fhadSpectro ) {
    fStatus = kInitError;
    return fStatus;
  }
  
  felecSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( felecArmName.Data(), "THcHallCSpectrometer"));
  if( !felecSpectro ) {
    fStatus = kInitError;
    return fStatus;
  }
    
  fCoinDet = dynamic_cast<THcTrigDet*>
    ( FindModule( fCoinDetName.Data(), "THcTrigDet"));
  if( !fCoinDet ) {
    fStatus = kInitError;
    return fStatus;
  }
  

  
  if( (fStatus=THaPhysicsModule::Init( run_time )) != kOK ) {
    return fStatus;
  }

  return fStatus;
}

//_____________________________________________________________________________
Int_t THcCoinTime::ReadDatabase( const TDatime& date )
{
  // Read database. Gets variable needed for CoinTime calculation


  DBRequest list[]={
    {"epCoinTime_Offset",  &ep_CT_Offset, kDouble, 0, 1},   //coin time offset for ep coincidences
    {"eKCoinTime_Offset",  &eK_CT_Offset, kDouble, 0, 1},   //coin time offset for ep coincidences
    {"ePiCoinTime_Offset",  &ePi_CT_Offset, kDouble, 0, 1},   //coin time offset for ep coincidences
    {"ePosiCoinTime_Offset",  &eposi_CT_Offset, kDouble, 0, 1},   //coin time offset for e-positron coincidences

    {"HMS_CentralPathLen",  &HMScentralPathLen, kDouble, 0, 1},
    {"SHMS_CentralPathLen", &SHMScentralPathLen, kDouble, 0, 1},
    {0}
  };
  
  //Default values if not read from param file
  ep_CT_Offset = 0.0;
  eK_CT_Offset = 0.0;
  ePi_CT_Offset = 0.0;
  eposi_CT_Offset = 0.0;

  HMScentralPathLen = 22.0*100.;
  SHMScentralPathLen = 18.1*100.;

  
  gHcParms->LoadParmValues((DBRequest*)&list, "");

  return kOK;
}

//_____________________________________________________________________________
Int_t THcCoinTime::DefineVariables( EMode mode )
{

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  const RVarDef vars[] = {
    {"epCoinTime_ROC1",    "ROC1 Corrected ep Coincidence Time",  "fROC1_epCoinTime"},
    {"epCoinTime_ROC2",    "ROC2 Corrected ep Coincidence Time",  "fROC2_epCoinTime"},
  
    {"eKCoinTime_ROC1",    "ROC1 Corrected eK Coincidence Time",  "fROC1_eKCoinTime"},
    {"eKCoinTime_ROC2",    "ROC2 Corrected eK Coincidence Time",  "fROC2_eKCoinTime"},
    
    {"ePiCoinTime_ROC1",    "ROC1 Corrected ePi Coincidence Time",  "fROC1_ePiCoinTime"},
    {"ePiCoinTime_ROC2",    "ROC2 Corrected ePi Coincidence Time",  "fROC2_ePiCoinTime"},
      
    {"ePositronCoinTime_ROC1",    "ROC1 Corrected e-Positorn Coincidence Time",  "fROC1_ePosCoinTime"},
    {"ePositronCoinTime_ROC2",    "ROC2 Corrected e-Positron Coincidence Time",  "fROC2_ePosCoinTime"},
    
    {"eHadCoinTime_RAW_ROC1",    "ROC1 RAW ep Coincidence Time",  "fROC1_RAW_eHadCoinTime"},
    {"eHadCoinTime_RAW_ROC2",    "ROC2 RAW ep Coincidence Time",  "fROC2_RAW_eHadCoinTime"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );

}



//_____________________________________________________________________________
Int_t THcCoinTime::Process( const THaEvData& evdata )
{
  
  if( !IsOK() || !gHaRun ) return -1;

  //Declare track information objects for hadron/electron arm
  THaTrackInfo* had_trkifo = fhadSpectro->GetTrackInfo();
  THaTrackInfo* elec_trkifo = felecSpectro->GetTrackInfo();

  //Check if the hadron/electron arm had a track
  if( !had_trkifo || !had_trkifo->IsOK() ) return 1;
  if( !elec_trkifo || !elec_trkifo->IsOK() ) return 1;

  //Create THaTrack object for hadron/elec arms to get relevant golden track quantities
  THaTrack* theHadTrack = /*static_cast<THaTrack*>*/(fhadSpectro->GetGoldenTrack());  //cast / no cast makes no diff.
  THaTrack* theElecTrack = /*static_cast<THaTrack*>*/(felecSpectro->GetGoldenTrack());
  
  //Gather relevant variables for Coincidence time calculation
  lightSpeed = 29.9792; // in cm/ns

  //Particle Masses (HardCoded)
  elecMass =  0.510998/1000.0; // electron mass in GeV/c^2
  positronMass =  0.510998/1000.0;
  protonMass = 938.27208/1000.0; // proton mass in GeV/c^2	
  kaonMass = 493.677/1000.0;    //charged kaon mass in GeV/c^2
  pionMass = 139.570/1000.0;    //charged pion mass in GeV/c^2

  //SHMScentralPathLen =  18.1*100;    //cm       //TODO: put in a GEN Param file, and read it from there 
  //HMScentralPathLen =  22.*100;   //cm          //TODO: put in a GEN Param file, and read it from there 
 
  //Check if there was a golden track in both arms
  if (!theHadTrack || !theElecTrack)
    {
      return 1;
    }

  //Check if Database is reading the correct elec-arm particle mass
  if (felecSpectro->GetParticleMass() > 0.00052) return 1;
   
     
      //electron arm
      elec_P = theElecTrack->GetP();              //electron golden track arm momentum
      elec_xptar = theElecTrack->GetTTheta();        //xp_tar
      elec_dP = theElecTrack->GetDp();            //electron arm deltaP 
      elec_FPtime = theElecTrack->GetFPTime();    //electron arm focal plane time
      
      //hadron arm
      had_P = theHadTrack->GetP();              //hadron golden track arm momentum
      had_xfp = theHadTrack->GetX();           //x_fp
      had_xpfp = theHadTrack->GetTheta();      //xp_fp
      had_ypfp = theHadTrack->GetPhi();        //yp_fp
      had_FPtime = theHadTrack->GetFPTime();    //hadron-arm focal plane time
      
      //Get raw TDC Times for HMS/SHMS (3/4 trigger)
      pTRIG1_rawTdcTime_ROC1 = fCoinDet->Get_pTRG1_ROC1_rawTdctime();  //TDC Channels (0.1 ns/Ch)
      pTRIG4_rawTdcTime_ROC1 = fCoinDet->Get_pTRG4_ROC1_rawTdctime();
      pTRIG1_rawTdcTime_ROC2 = fCoinDet->Get_pTRG1_ROC2_rawTdctime();
      pTRIG4_rawTdcTime_ROC2 = fCoinDet->Get_pTRG4_ROC2_rawTdctime();


      //Assume electron arm is SHMS 
      if (felecArmName=="P")
	{
	  
	  //pathlength corrections
	  DeltaSHMSpathLength = -0.11*atan2(elec_xptar,1)*1000 - 0.057*elec_dP;
	  DeltaHMSpathLength = 12.462*had_xpfp + 0.1138*had_xpfp*had_xfp - 0.0154*had_xfp - 72.292*had_xpfp*had_xpfp - 0.0000544*had_xfp*had_xfp - 116.52*had_ypfp*had_ypfp;

	  //beta calculations beta = v/c = p/E
	  elecArm_BetaCalc = elec_P / sqrt(elec_P*elec_P + elecMass*elecMass);
	  hadArm_BetaCalc_proton = had_P / sqrt(had_P*had_P + protonMass*protonMass);
	  hadArm_BetaCalc_Kaon = had_P / sqrt(had_P*had_P + kaonMass*kaonMass);
	  hadArm_BetaCalc_Pion = had_P / sqrt(had_P*had_P + pionMass*pionMass);
	
	  hadArm_BetaCalc_Positron = had_P / sqrt(had_P*had_P + positronMass*positronMass);


	  //Coincidence Corrections
	  elec_coinCorr = (SHMScentralPathLen +  DeltaSHMSpathLength) / (lightSpeed * elecArm_BetaCalc ) - elec_FPtime;
	  had_coinCorr_proton = (HMScentralPathLen +  DeltaHMSpathLength) / (lightSpeed * hadArm_BetaCalc_proton ) - had_FPtime;
	  had_coinCorr_Kaon = (HMScentralPathLen +  DeltaHMSpathLength) / (lightSpeed * hadArm_BetaCalc_Kaon ) - had_FPtime;
	  had_coinCorr_Pion = (HMScentralPathLen +  DeltaHMSpathLength) / (lightSpeed * hadArm_BetaCalc_Pion ) - had_FPtime;
	  had_coinCorr_Positron = (HMScentralPathLen +  DeltaHMSpathLength) / (lightSpeed * hadArm_BetaCalc_Positron ) - had_FPtime;

	  //Raw, Uncorrected e-Hadron Coincidence Time
	  fROC1_RAW_eHadCoinTime =  (pTRIG1_rawTdcTime_ROC1*0.1) - (pTRIG4_rawTdcTime_ROC1*0.1);
	  fROC2_RAW_eHadCoinTime =  (pTRIG1_rawTdcTime_ROC2*0.1) - (pTRIG4_rawTdcTime_ROC2*0.1);
	  
	  
	  //Corrected Coincidence Time for ROC1/ROC2 (Should be identical)
	  
	  //PROTON
	  fROC1_epCoinTime =  (pTRIG1_rawTdcTime_ROC1*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC1*0.1 - had_coinCorr_proton) - ep_CT_Offset;
	  
	  fROC2_epCoinTime =  (pTRIG1_rawTdcTime_ROC2*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC2*0.1 - had_coinCorr_proton) - ep_CT_Offset;

	  //KAON
	  fROC1_eKCoinTime =  (pTRIG1_rawTdcTime_ROC1*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC1*0.1 - had_coinCorr_Kaon) - eK_CT_Offset;
	  
	  fROC2_eKCoinTime =  (pTRIG1_rawTdcTime_ROC2*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC2*0.1 - had_coinCorr_Kaon) - eK_CT_Offset;
	
	  //PION
	  fROC1_ePiCoinTime =  (pTRIG1_rawTdcTime_ROC1*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC1*0.1 - had_coinCorr_Pion) - ePi_CT_Offset;
	  
	  fROC2_ePiCoinTime =  (pTRIG1_rawTdcTime_ROC2*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC2*0.1 - had_coinCorr_Pion) - ePi_CT_Offset;

	  //POSITRON
	  fROC1_ePosCoinTime =  (pTRIG1_rawTdcTime_ROC1*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC1*0.1 - had_coinCorr_Positron) - eposi_CT_Offset;
	  
	  fROC2_ePosCoinTime =  (pTRIG1_rawTdcTime_ROC2*0.1 - elec_coinCorr) - 
	    (pTRIG4_rawTdcTime_ROC2*0.1 - had_coinCorr_Positron) - eposi_CT_Offset;
	  
	  cout << "fROC1_epCoinTime: " << fROC1_epCoinTime << endl;
	  cout << "fROC2_epCoinTime: " << fROC2_epCoinTime << endl;
	  cout << "fROC1_eKCoinTime: " << fROC1_eKCoinTime << endl;
	  cout << "fROC2_eKCoinTime: " << fROC2_eKCoinTime << endl;
	  cout << "fROC1_ePiCoinTime: " << fROC1_ePiCoinTime << endl;
	  cout << "fROC2_ePiCoinTime: " << fROC2_ePiCoinTime << endl;
	  cout << "fROC1_ePosCoinTime: " << fROC1_ePosCoinTime << endl;
	  cout << "fROC2_ePosCoinTime: " << fROC2_ePosCoinTime << endl;

	}


   
  
  
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
