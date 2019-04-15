/** \class THcCoinTime
    \ingroup PhysMods

\brief Class for calculating and adding the Coincidence Time in the Tree.

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
    {0}
  };
  
  //Default values if not read from param file
  eHad_CT_Offset = 0.0;

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
    {"epCoinTime_TRIG1",    "TRIG1 Corrected ep Coincidence Time",  "fTRIG1_epCoinTime"},
    {"epCoinTime_TRIG4",    "TRIG4 Corrected ep Coincidence Time",  "fTRIG4_epCoinTime"},
  
    {"eKCoinTime_ROC1",    "ROC1 Corrected eK Coincidence Time",  "fROC1_eKCoinTime"},
    {"eKCoinTime_ROC2",    "ROC2 Corrected eK Coincidence Time",  "fROC2_eKCoinTime"},
    {"eKCoinTime_TRIG1",    "TRIG1 Corrected eK Coincidence Time",  "fTRIG1_eKCoinTime"},
    {"eKCoinTime_TRIG4",    "TRIG4 Corrected eK Coincidence Time",  "fTRIG4_eKCoinTime"},
    
    {"ePiCoinTime_ROC1",    "ROC1 Corrected ePi Coincidence Time",  "fROC1_ePiCoinTime"},
    {"ePiCoinTime_ROC2",    "ROC2 Corrected ePi Coincidence Time",  "fROC2_ePiCoinTime"},
    {"ePiCoinTime_TRIG1",    "TRIG1 Corrected ePi Coincidence Time",  "fTRIG1_ePiCoinTime"},
    {"ePiCoinTime_TRIG4",    "TRIG4 Corrected ePi Coincidence Time",  "fTRIG4_ePiCoinTime"},
      
    {"ePositronCoinTime_ROC1",    "ROC1 Corrected e-Positorn Coincidence Time",  "fROC1_ePosCoinTime"},
    {"ePositronCoinTime_ROC2",    "ROC2 Corrected e-Positron Coincidence Time",  "fROC2_ePosCoinTime"},
    {"ePositronCoinTime_TRIG1",    "TRIG1 Corrected e-Positorn Coincidence Time",  "fTRIG1_ePosCoinTime"},
    {"ePositronCoinTime_TRIG4",    "TRIG4 Corrected e-Positron Coincidence Time",  "fTRIG4_ePosCoinTime"},
    
    {"CoinTime_RAW_ROC1",    "ROC1 RAW Coincidence Time",  "fROC1_RAW_CoinTime"},
    {"CoinTime_RAW_ROC2",    "ROC2 RAW Coincidence Time",  "fROC2_RAW_CoinTime"},
    {"CoinTime_RAW_TRIG1",    "TRIG1 RAW Coincidence Time",  "fTRIG1_RAW_CoinTime"},
    {"CoinTime_RAW_TRIG4",    "TRIG4 RAW Coincidence Time",  "fTRIG4_RAW_CoinTime"},
    {"DeltaSHMSPathLength","DeltaSHMSpathLength (cm)","DeltaSHMSpathLength"},
    {"DeltaHMSPathLength", "DeltaHMSpathLength (cm)","DeltaHMSpathLength"},
    {"had_coinCorr_Positron",    "",  "had_coinCorr_Positron"},
    {"elec_coinCorr",    "",  "elec_coinCorr"},
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
  if (felecSpectro->GetParticleMass() > 0.00052) return 1;
   
     
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
      
      //Get raw TDC Times for HMS/SHMS (3/4 trigger)
      pTRIG1_TdcTime_ROC1 = fCoinDet->Get_CT_Trigtime(0);  //SHMS
      pTRIG4_TdcTime_ROC1 = fCoinDet->Get_CT_Trigtime(1);  //HMS
      pTRIG1_TdcTime_ROC2 = fCoinDet->Get_CT_Trigtime(2);//SHMS
      pTRIG4_TdcTime_ROC2 = fCoinDet->Get_CT_Trigtime(3);//HMS

      DeltaSHMSpathLength = .11*shms_xptar*1000 +0.057*shms_dP/100.;
      DeltaHMSpathLength = -1.0*(12.462*hms_xpfp + 0.1138*hms_xpfp*hms_xfp - 0.0154*hms_xfp - 72.292*hms_xpfp*hms_xpfp - 0.0000544*hms_xfp*had_xfp - 116.52*hms_ypfp*hms_ypfp);
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
	  fROC1_RAW_CoinTime =  (pTRIG1_TdcTime_ROC1 + SHMS_FPtime) - (pTRIG4_TdcTime_ROC1 + HMS_FPtime);
	  fROC2_RAW_CoinTime =  (pTRIG1_TdcTime_ROC2 + SHMS_FPtime) - (pTRIG4_TdcTime_ROC2 + HMS_FPtime);
	  fTRIG1_RAW_CoinTime =  (pTRIG1_TdcTime_ROC1 + SHMS_FPtime) - (pTRIG1_TdcTime_ROC2 + HMS_FPtime);
	  fTRIG4_RAW_CoinTime =  (pTRIG4_TdcTime_ROC1 + SHMS_FPtime) - (pTRIG4_TdcTime_ROC2 + HMS_FPtime);
	  
	  
	  //Corrected Coincidence Time for ROC1/ROC2 (ROC1 Should be identical to ROC2)
          // 
	  //PROTON
	  fROC1_epCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fROC2_epCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fTRIG1_epCoinTime = fTRIG1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 
	  fTRIG4_epCoinTime = fTRIG4_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_proton) - eHad_CT_Offset; 

	  //KAON
	  fROC1_eKCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fROC2_eKCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fTRIG1_eKCoinTime = fTRIG1_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	  fTRIG4_eKCoinTime = fTRIG4_RAW_CoinTime + sign*( elec_coinCorr-had_coinCorr_Kaon) - eHad_CT_Offset;
	
	  //PION
	  fROC1_ePiCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;	  
	  fROC2_ePiCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;
	  fTRIG1_ePiCoinTime = fTRIG1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;	  
	  fTRIG4_ePiCoinTime = fTRIG4_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Pion) - eHad_CT_Offset;

	  //POSITRON
	  fROC1_ePosCoinTime = fROC1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset ;	  
	  fROC2_ePosCoinTime = fROC2_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;
	  fTRIG1_ePosCoinTime = fTRIG1_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset ;	  
	  fTRIG4_ePosCoinTime = fTRIG4_RAW_CoinTime + sign*( elec_coinCorr - had_coinCorr_Positron) - eHad_CT_Offset;
         
   
  
  
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
