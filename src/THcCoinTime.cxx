/** \class THcCoinTime
    \ingroup PhysMods

\brief Class for calculating and adding the Coincidence Time in the Tree.
// SJDK - 08/09/21 - Switched Trig4 to Trig3 due to hardware changes

//Author: Carlos Yero
//Date: April 27, 2018
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
  fCoinDetName(coinname), 
  fhadArmName(hadArmName),                 //initialize spectro names
  felecArmName(elecArmName),
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
void THcCoinTime::Clear( Option_t* opt )
{
  // Clear all event-by-event variables.
  
  //  THaPhysicsModule::Clear(opt);
  fROC1_epCoinTime=kBig;
  fROC2_epCoinTime=kBig;
  fROC1_eKCoinTime=kBig;
  fROC2_eKCoinTime=kBig;
  fROC1_ePiCoinTime=kBig;
  fROC2_ePiCoinTime=kBig;
  fROC1_ePosCoinTime=kBig;
  fROC2_ePosCoinTime=kBig;
  fROC1_RAW_CoinTime=kBig;
  fROC2_RAW_CoinTime=kBig;
  fSHMS_ePosCoinTime=kBig;
  fHMS_ePosCoinTime=kBig;
  fSHMS_ePiCoinTime=kBig;
  fHMS_ePiCoinTime=kBig;
  fSHMS_eKCoinTime=kBig;
  fHMS_eKCoinTime=kBig;
  fSHMS_epCoinTime=kBig;
  fHMS_epCoinTime=kBig;

  /*
  // 31/01/23 - SJDK - Commented out for now
  // 18/11/22 - SJDK - Added RF time distributions with a CoinTime correction
  fSHMS_eRFtimeDist_CT=kBig;
  fSHMS_piRFtimeDist_CT=kBig;
  fSHMS_KRFtimeDist_CT=kBig;
  fSHMS_pRFtimeDist_CT=kBig;  
  */
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
  cout << "TrigDet  -------> " << fCoinDetName << endl;
  cout << "**************************************************" << endl;

  fStatus = kOK;

  fhadSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( fhadArmName.Data(), "THcHallCSpectrometer"));
  if( !fhadSpectro ) {
    cout << "THcCoinTime module  Cannnot find Hadron Arm = " <<  fhadArmName.Data() << endl;
    fStatus = kInitError;
    return fStatus;
  }
  
  felecSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( felecArmName.Data(), "THcHallCSpectrometer"));
  if( !felecSpectro ) {
    cout << "THcCoinTime module  Cannnot find Electron Arm = " << felecArmName.Data() << endl;
    fStatus = kInitError;
    return fStatus;
  }
    
  fCoinDet = dynamic_cast<THcTrigDet*>
    ( FindModule( fCoinDetName.Data(), "THcTrigDet"));
  if( !fCoinDet ) {
    cout << "THcCoinTime module  Cannnot find TrigDet = " << fCoinDetName.Data() << endl;
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
    {"eHadCoinTime_Offset",  &eHad_CT_Offset, kDouble, 0, 1},   //coin time offset for ep coincidences
    {"HMS_CentralPathLen",  &HMScentralPathLen, kDouble, 0, 1},
    {"SHMS_CentralPathLen", &SHMScentralPathLen, kDouble, 0, 1},
    /*
    // 31/01/23 - SJDK - Commented out for now, this is for testing the CT corrected RF times
    // 18/11/22 - SJDK - New variables read in for the RF calculations included here
    {"SHMS_RF_Offset",  &SHMS_RF_Offset, kDouble, 0, 1}, // RF offset for SHMS
    {"SHMS_eRF_Offset",  &SHMS_eRF_Offset, kDouble, 0, 1},   // Electron/Positron RF offset for SHMS
    {"SHMS_piRF_Offset",  &SHMS_piRF_Offset, kDouble, 0, 1},   // Pion RF offset for SHMS
    {"SHMS_KRF_Offset",  &SHMS_KRF_Offset, kDouble, 0, 1},   //  Kaon RF offset for SHMS
    {"SHMS_pRF_Offset",  &SHMS_pRF_Offset, kDouble, 0, 1},   // Proton RF offset for SHMS
    {"Bunch_Spacing_Override",  &Bunch_Spacing_Override, kDouble, 0, 1}, // Bunch spacing value, can be manually set to override the Epics read in,
    */
    {0}
  };
  
  //Default values if not read from param file
  eHad_CT_Offset = 0.0;
  
  // SJDK 16/11/22 - This must be to a specific point? Where? The first hodo plane?
  HMScentralPathLen = 22.0*100.;
  SHMScentralPathLen = 18.1*100.;
 
  /*
  // 31/01/23 - Again, just for testing the CT corrected RF time, commented out for now
  // SJDK 18/11/22 - New variables associated with RFTime calculations
  // Default values if not read from param file
  Bunch_Spacing_Override = 1.5556; // Strangely specific value so we can check if it's set or not
  SHMS_RF_Offset = 0.0;
  
  // Particle specific RF offset values, set to a default value. The default doesn't really make sense as a choice, check after read in if it was set
  SHMS_eRF_Offset = -5.1114;
  SHMS_piRF_Offset = -5.1114;
  SHMS_KRF_Offset = -5.1114;
  SHMS_pRF_Offset = -5.1114;

  gHcParms->LoadParmValues((DBRequest*)&list, "");
  // If the override value has been set (i.e. it is NOT the default value), set it to whatever the read in is
  if ( Bunch_Spacing_Override != 1.5556){
  Bunch_Spacing = Bunch_Spacing_Override;
  }
  // If override wasn't set, assume the bunch spacing has some default value (will be overriden by Epics if it looks OK)
  else if (Bunch_Spacing_Override == 1.5556){
  Bunch_Spacing = 4.008;
  }
  if (SHMS_eRF_Offset == -5.1114){
  SHMS_eRF_Offset = SHMS_RF_Offset; // If electron SHMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (SHMS_piRF_Offset == -5.1114){
  SHMS_piRF_Offset = SHMS_RF_Offset; // If pion SHMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (SHMS_KRF_Offset == -5.1114){
  SHMS_KRF_Offset = SHMS_RF_Offset; // If kaon SHMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (SHMS_pRF_Offset == -5.1114){
  SHMS_pRF_Offset = SHMS_RF_Offset; // If proton SHMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  */
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
    {"epCoinTime_SHMS",    "SHMS Corrected ep Coincidence Time",  "fSHMS_epCoinTime"},
    {"epCoinTime_HMS",    "HMS Corrected ep Coincidence Time",  "fHMS_epCoinTime"},
  
    {"eKCoinTime_ROC1",    "ROC1 Corrected eK Coincidence Time",  "fROC1_eKCoinTime"},
    {"eKCoinTime_ROC2",    "ROC2 Corrected eK Coincidence Time",  "fROC2_eKCoinTime"},
    {"eKCoinTime_SHMS",    "SHMS Corrected eK Coincidence Time",  "fSHMS_eKCoinTime"},
    {"eKCoinTime_HMS",    "HMS Corrected eK Coincidence Time",  "fHMS_eKCoinTime"},
    
    {"ePiCoinTime_ROC1",    "ROC1 Corrected ePi Coincidence Time",  "fROC1_ePiCoinTime"},
    {"ePiCoinTime_ROC2",    "ROC2 Corrected ePi Coincidence Time",  "fROC2_ePiCoinTime"},
    {"ePiCoinTime_SHMS",    "SHMS Corrected ePi Coincidence Time",  "fSHMS_ePiCoinTime"},
    {"ePiCoinTime_HMS",    "HMS Corrected ePi Coincidence Time",  "fHMS_ePiCoinTime"},
      
    {"ePositronCoinTime_ROC1",    "ROC1 Corrected e-Positorn Coincidence Time",  "fROC1_ePosCoinTime"},
    {"ePositronCoinTime_ROC2",    "ROC2 Corrected e-Positron Coincidence Time",  "fROC2_ePosCoinTime"},
    {"ePositronCoinTime_SHMS",    "SHMS Corrected e-Positorn Coincidence Time",  "fSHMS_ePosCoinTime"},
    {"ePositronCoinTime_HMS",    "HMS Corrected e-Positron Coincidence Time",  "fHMS_ePosCoinTime"},
    
    {"CoinTime_RAW_ROC1",    "ROC1 RAW Coincidence Time",  "fROC1_RAW_CoinTime"},
    {"CoinTime_RAW_ROC2",    "ROC2 RAW Coincidence Time",  "fROC2_RAW_CoinTime"},
    {"CoinTime_RAW_SHMS",    "SHMS RAW Coincidence Time",  "fSHMS_RAW_CoinTime"},
    {"CoinTime_RAW_HMS",    "HMS RAW Coincidence Time",  "fHMS_RAW_CoinTime"},
    {"CoinTime_RAW_ROC1_NoTrack",    "ROC1 RAW Coincidence Time w/o Tracked Param",  "fROC1_RAW_CoinTime_NoTrack"},
    {"CoinTime_RAW_ROC2_NoTrack",    "ROC2 RAW Coincidence Time w/o Tracked Param",  "fROC2_RAW_CoinTime_NoTrack"},
    {"CoinTime_RAW_SHMS_NoTrack",    "SHMS RAW Coincidence Time w/o Tracked Param",  "fSHMS_RAW_CoinTime_NoTrack"},
    {"CoinTime_RAW_HMS_NoTrack",    "HMS RAW Coincidence Time w/o Tracked Param",  "fHMS_RAW_CoinTime_NoTrack"},
    {"DeltaSHMSPathLength","DeltaSHMSpathLength (cm)","DeltaSHMSpathLength"},
    {"DeltaHMSPathLength", "DeltaHMSpathLength (cm)","DeltaHMSpathLength"},
    {"had_coinCorr_Positron",    "",  "had_coinCorr_Positron"},
    {"elec_coinCorr",    "",  "elec_coinCorr"},

    // 31/01/23 - SJDK - Defined here just in case someone wants to take a closer look at these in the future
    /*
    {"SHMS_eRFtimeDist_CT", "SHMS Corrected RF time Distribution for PID (e)", "fSHMS_eRFtimeDist_CT"},
    {"SHMS_piRFtimeDist_CT", "SHMS Corrected RF time Distribution for PID (pi)", "fSHMS_piRFtimeDist_CT"},
    {"SHMS_KRFtimeDist_CT", "SHMS Corrected RF time Distribution for PID (K)", "fSHMS_KRFtimeDist_CT"},
    {"SHMS_pRFtimeDist_CT", "SHMS Corrected RF time Distribution for PID (p)", "fSHMS_pRFtimeDist_CT"},
    */
    {0}
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

  if( !had_trkifo) cout << " no hadron track " << endl;
  if( !elec_trkifo) cout << " no electron track " << endl;
  //Check if the hadron/electron arm had a track
  if( !had_trkifo || !had_trkifo->IsOK() ) return 1;
  if( !elec_trkifo || !elec_trkifo->IsOK() ) return 1;

  //Create THaTrack object for hadron/elec arms to get relevant golden track quantities
  if (felecArmName=="H") {
    theSHMSTrack =(fhadSpectro->GetGoldenTrack()); 
    theHMSTrack = (felecSpectro->GetGoldenTrack());
  } else{
    theSHMSTrack =(felecSpectro->GetGoldenTrack()); 
    theHMSTrack = (fhadSpectro->GetGoldenTrack());
  }
  //Gather relevant variables for Coincidence time calculation
  lightSpeed = 29.9792; // in cm/ns

  elec_coinCorr=kBig;
  had_coinCorr_Positron=kBig;
  //Particle Masses (HardCoded)
  elecMass =  0.510998/1000.0; // electron mass in GeV/c^2
  positronMass =  0.510998/1000.0;
  protonMass = 938.27208/1000.0; // proton mass in GeV/c^2	
  kaonMass = 493.677/1000.0;    //charged kaon mass in GeV/c^2
  pionMass = 139.570/1000.0;    //charged pion mass in GeV/c^2

  //Check if there was a golden track in both arms
  if (!theSHMSTrack || !theHMSTrack)
    {
      return 1;
    }

  //Check if Database is reading the correct elec-arm particle mass
  if (felecSpectro->GetParticleMass() > 0.00052) {
    cout << "ERROR CHECK PARTICLE MASS IN STANDARD.KINEMATICS ! ! !" << endl;
    return 1;
  }

      //SHMS arm
      Double_t shms_xptar = theSHMSTrack->GetTTheta();     
      Double_t shms_dP = theSHMSTrack->GetDp();            
      Double_t SHMS_FPtime = theSHMSTrack->GetFPTime();    
      
      //HMS arm
      Double_t hms_xptar = theHMSTrack->GetTTheta();           
      Double_t hms_dP = theHMSTrack->GetDp();            
      Double_t hms_xfp = theHMSTrack->GetX();           
      Double_t hms_xpfp = theHMSTrack->GetTheta();      
      Double_t hms_ypfp = theHMSTrack->GetPhi();        
      Double_t HMS_FPtime = theHMSTrack->GetFPTime();    
      
      if (SHMS_FPtime==-2000 || HMS_FPtime==-2000)  return 1;
      if (SHMS_FPtime==-1000 || HMS_FPtime==-1000)  return 1;
      
      //Get raw TDC Times for HMS/SHMS (3/4 trigger)
      pSHMS_TdcTime_ROC1 = fCoinDet->Get_CT_Trigtime(0);  //SHMS
      pHMS_TdcTime_ROC1 = fCoinDet->Get_CT_Trigtime(1);  //HMS
      pSHMS_TdcTime_ROC2 = fCoinDet->Get_CT_Trigtime(2);//SHMS pTrig1
      pHMS_TdcTime_ROC2 = fCoinDet->Get_CT_Trigtime(3);//HMS pTrig3
      	  
      DeltaSHMSpathLength = .11*shms_xptar*1000 +0.057*shms_dP/100.;
      // 16/11/22 - SJDK - The calculation below seems to be "correct" in the sense this it is accounting for the beam on target angle etc. However, it is only defined this was for the HMS. Also, it is is redefined on the next line
      // I've commented it out for now since it seems pointless to define it twice.
      //DeltaHMSpathLength = -1.0*(12.462*hms_xpfp + 0.1138*hms_xpfp*hms_xfp - 0.0154*hms_xfp - 72.292*hms_xpfp*hms_xpfp - 0.0000544*hms_xfp*had_xfp - 116.52*hms_ypfp*hms_ypfp);
      DeltaHMSpathLength = (.12*hms_xptar*1000 +0.17*hms_dP/100.);

          // default assume SHMS is electron arm
	  Double_t ElecPathLength=SHMScentralPathLen +  DeltaSHMSpathLength;
	  Double_t HadPathLength=HMScentralPathLen +  DeltaHMSpathLength;
          elec_P = theSHMSTrack->GetP();              //electron golden track arm momentum
          had_P = theHMSTrack->GetP();              //hadron golden track arm momentum
          Int_t sign=-1;
	  if (felecArmName=="H") {
             ElecPathLength=HMScentralPathLen +  DeltaHMSpathLength;
	     HadPathLength=SHMScentralPathLen +  DeltaSHMSpathLength;
             elec_P = theHMSTrack->GetP();              //electron golden track arm momentum
             had_P = theSHMSTrack->GetP();              //hadron golden track arm momentum
             sign=1;
	  }

	  //beta calculations beta = v/c = p/E
	  elecArm_BetaCalc = elec_P / sqrt(elec_P*elec_P + elecMass*elecMass);
	  hadArm_BetaCalc_proton = had_P / sqrt(had_P*had_P + protonMass*protonMass);
	  hadArm_BetaCalc_Kaon = had_P / sqrt(had_P*had_P + kaonMass*kaonMass);
	  hadArm_BetaCalc_Pion = had_P / sqrt(had_P*had_P + pionMass*pionMass);	
	  hadArm_BetaCalc_Positron = had_P / sqrt(had_P*had_P + positronMass*positronMass);

	  //Coincidence Corrections
	  elec_coinCorr = (ElecPathLength) / (lightSpeed * elecArm_BetaCalc );
	  had_coinCorr_proton = (HadPathLength) / (lightSpeed * hadArm_BetaCalc_proton );
	  had_coinCorr_Kaon =  (HadPathLength)/ (lightSpeed * hadArm_BetaCalc_Kaon );
	  had_coinCorr_Pion =  (HadPathLength)/ (lightSpeed * hadArm_BetaCalc_Pion );
	  had_coinCorr_Positron = (HadPathLength) / (lightSpeed * hadArm_BetaCalc_Positron );

	  //Raw, Uncorrected Coincidence Time
	  fROC1_RAW_CoinTime =  (pSHMS_TdcTime_ROC1 + SHMS_FPtime) - (pHMS_TdcTime_ROC1 + HMS_FPtime);
	  fROC2_RAW_CoinTime =  (pSHMS_TdcTime_ROC2 + SHMS_FPtime) - (pHMS_TdcTime_ROC2 + HMS_FPtime);
	  fSHMS_RAW_CoinTime =  (pSHMS_TdcTime_ROC1 + SHMS_FPtime) - (pSHMS_TdcTime_ROC2 + HMS_FPtime);
	  fHMS_RAW_CoinTime =  (pHMS_TdcTime_ROC1 + SHMS_FPtime) - (pHMS_TdcTime_ROC2 + HMS_FPtime);
	  // 04/05/21 - SJDK - Added for use in Report files for tracking efficiency calculations
	  //Raw, Uncorrected Coincidence Time WITHOUT any tracked quantities
	  fROC1_RAW_CoinTime_NoTrack =  (pSHMS_TdcTime_ROC1) - (pHMS_TdcTime_ROC1);
	  fROC2_RAW_CoinTime_NoTrack =  (pSHMS_TdcTime_ROC2) - (pHMS_TdcTime_ROC2);
	  fSHMS_RAW_CoinTime_NoTrack =  (pSHMS_TdcTime_ROC1) - (pSHMS_TdcTime_ROC2);
	  fHMS_RAW_CoinTime_NoTrack =  (pHMS_TdcTime_ROC1) - (pHMS_TdcTime_ROC2);
	  //Corrected Coincidence Time for ROC1/ROC2 (ROC1 Should be identical to ROC2)
          // 
	  //PROTON
	  fROC1_epCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fROC2_epCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fSHMS_epCoinTime = fSHMS_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fHMS_epCoinTime = fHMS_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 

	  //KAON
	  fROC1_eKCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fROC2_eKCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fSHMS_eKCoinTime = fSHMS_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fHMS_eKCoinTime = fHMS_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	
	  //PION
	  fROC1_ePiCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;	  
	  fROC2_ePiCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;
	  fSHMS_ePiCoinTime = fSHMS_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;	  
	  fHMS_ePiCoinTime = fHMS_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;

	  //POSITRON
	  fROC1_ePosCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;
	  fROC2_ePosCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;
	  fSHMS_ePosCoinTime = fSHMS_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;
	  fHMS_ePosCoinTime = fHMS_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;

	  /*
	  // SJDK - 31/01/23 - Tested a few different methods to correct the RF time for the Coin time based upon different particle types. Didn't have much luck with this. Shelving it for now
	  // SJDK - 18/11/22 - Note, this is just for testing, RF time calculations should be made more "generic" as the CT ones already are
	  // RF Time calculations - Testing
	  // 21/11/22 - From testing, adding fROC1_ePosCoinTime as a correction doesn't have the desired effect. It is not a sign issue. Adding/subtracting fails to flatten CT vs RFTime
	  // Will try using RAW times instead
	  // Adding it didn't quite work, trying subtraction, didn't work either
	  // Add coin time WITHOTUT adding correction? Being applied twice. Didn't work either
	  // Raw Untracked variable? fROC1_RAW_CoinTime_NoTrack
	  SHMS_RFtime = fCoinDet->Get_RF_TrigTime(0); // SHMS is ID 0

	  fSHMS_eRFtimeDist_CT = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_eRF_Offset + had_coinCorr_Positron + fROC1_RAW_CoinTime_NoTrack), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
	  fSHMS_piRFtimeDist_CT = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_piRF_Offset + had_coinCorr_Pion + fROC1_RAW_CoinTime_NoTrack), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
	  fSHMS_KRFtimeDist_CT = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_KRF_Offset + had_coinCorr_Kaon + fROC1_RAW_CoinTime_NoTrack), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
	  fSHMS_pRFtimeDist_CT = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_pRF_Offset + had_coinCorr_proton + fROC1_RAW_CoinTime_NoTrack), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
	  */

  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
