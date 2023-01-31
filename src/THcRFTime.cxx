/** \class THcRFTime
    \ingroup PhysMods

// Brief Class for calculating and adding the RF Time in the Tree.

// Author: Stephen JD Kay
// University of Regina
// Date: 12/04/21 (12th April 2021)
// Based on THcCoinTime class from C Yero

// 14/04/21 - Now works and reads in epics value for the accelerator frequency. This is used to determine the bunch spacing
// In future, want to determine the RF time with some corrections for each particle type (as is done in THcCoinTime)
*/

#include "THaEvData.h"
#include "THaCutList.h"
#include "THaDetMap.h"
#include "THaApparatus.h"
#include "VarDef.h"
#include "VarType.h"
#include "TClonesArray.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcRFTime.h"
#include "THcTrigDet.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcAnalyzer.h"

using namespace std;

//_____________________________________________________________________________
THcRFTime::THcRFTime (const char *name, const char* description, const char* hadArmName,
		      const char* elecArmName, const char* RFname) :

  THaPhysicsModule(name, description),
  fCoinDetName(RFname),
  fhadArmName(hadArmName),                 //initialize spectro names
  felecArmName(elecArmName),
  fhadSpectro(NULL),                      //initialize spectro objects
  felecSpectro(NULL),
  fCoinDet(NULL)
{
  Bunch_Spacing_Epics = 0.0001;
}

//_____________________________________________________________________________
THcRFTime::~THcRFTime()
{
  //Destructor

  RemoveVariables();

}
//_____________________________________________________________________________
void THcRFTime::Clear( Option_t* opt )
{
  // Clear all event-by-event variables.
  // Only need to clear variables that are actually used, will need to re-tweak this when the rest is set up
  //  THaPhysicsModule::Clear(opt);
  fHMS_RFtimeDist=kBig;
  fHMS_eRFtimeDist=kBig;
  fHMS_piRFtimeDist=kBig;
  fHMS_KRFtimeDist=kBig;
  fHMS_pRFtimeDist=kBig;
  fSHMS_RFtimeDist=kBig;
  fSHMS_eRFtimeDist=kBig;
  fSHMS_piRFtimeDist=kBig;
  fSHMS_KRFtimeDist=kBig;
  fSHMS_pRFtimeDist=kBig;  
}

//_____________________________________________________________________________
void THcRFTime::Reset( Option_t* opt)
// Clear event-by-event data
{
  Clear(opt);
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcRFTime::Init( const TDatime& run_time )
{
  // Initialize THcRFTime physics module

  cout << "*************************************************" << endl;
  cout << "Initializing THcRFTime Physics Modue" << endl;
  cout << "Hadron Arm   -------> " << fhadArmName << endl;
  cout << "Electron Arm -------> " << felecArmName << endl;
  cout << "TrigDet  -------> " << fCoinDetName << endl;
  fStatus = kOK;
  // For now, read in both spectrometers by default
  // In future, should be modified to read in one or the other (or both)
  fhadSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( fhadArmName.Data(), "THcHallCSpectrometer"));
  if( !fhadSpectro ) {
    cout << "THcRFTime module - Cannnot find Hadron Arm = " <<  fhadArmName.Data() << endl;
    fStatus = kInitError;
    return fStatus;
  }

  felecSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( felecArmName.Data(), "THcHallCSpectrometer"));
  if( !felecSpectro ) {
    cout << "THcRFTime module -  Cannnot find Electron Arm = " << felecArmName.Data() << endl;
    fStatus = kInitError;
    return fStatus;
  }
  // Switch to Trig Det
  fCoinDet = dynamic_cast<THcTrigDet*>
    ( FindModule( fCoinDetName.Data(), "THcTrigDet"));
  if( !fCoinDet ) {
    cout << "THcRFTime module -  Cannnot find TrigDet = " << fCoinDetName.Data() << endl;
    fStatus = kInitError;
    return fStatus;
  }

  if( (fStatus=THaPhysicsModule::Init( run_time )) != kOK ) {
    return fStatus;
  }

  return fStatus;
}

//_____________________________________________________________________________
Int_t THcRFTime::ReadDatabase( const TDatime& date )
{
  // Read database. Gets variable needed for RFTime calculation

  DBRequest list[]={
    {"HMS_RF_Offset",  &HMS_RF_Offset, kDouble, 0, 1},   // RF offset for HMS
    {"HMS_eRF_Offset",  &HMS_eRF_Offset, kDouble, 0, 1},   // Electron/Positron RF offset for HMS
    {"HMS_piRF_Offset",  &HMS_piRF_Offset, kDouble, 0, 1},   // Pion RF offset for HMS
    {"HMS_KRF_Offset",  &HMS_KRF_Offset, kDouble, 0, 1},   //  Kaon RF offset for HMS
    {"HMS_pRF_Offset",  &HMS_pRF_Offset, kDouble, 0, 1},   // Proton RF offset for HMS
    {"SHMS_RF_Offset",  &SHMS_RF_Offset, kDouble, 0, 1}, // RF offset for SHMS
    {"SHMS_eRF_Offset",  &SHMS_eRF_Offset, kDouble, 0, 1},   // Electron/Positron RF offset for SHMS
    {"SHMS_piRF_Offset",  &SHMS_piRF_Offset, kDouble, 0, 1},   // Pion RF offset for SHMS
    {"SHMS_KRF_Offset",  &SHMS_KRF_Offset, kDouble, 0, 1},   //  Kaon RF offset for SHMS
    {"SHMS_pRF_Offset",  &SHMS_pRF_Offset, kDouble, 0, 1},   // Proton RF offset for SHMS
    {"Bunch_Spacing_Override",  &Bunch_Spacing_Override, kDouble, 0, 1}, // Bunch spacing value, can be manually set to override the Epics read in,
    {"HMS_CentralPathLen",  &HMS_CentralPathLen, kDouble, 0, 1},
    {"SHMS_CentralPathLen", &SHMS_CentralPathLen, kDouble, 0, 1},
    {0}
  };

  // Default values if not read from param file
  Bunch_Spacing_Override = 1.5556; // Strangely specific value so we can check if it's set or not
  HMS_RF_Offset = 0.0;
  SHMS_RF_Offset = 0.0;
  
  // Particle specific RF offset values, set to a default value. The default doesn't really make sense as a choice, check after read in if it was set
  HMS_eRF_Offset = -5.1114;
  HMS_piRF_Offset = -5.1114;
  HMS_KRF_Offset = -5.1114;
  HMS_pRF_Offset = -5.1114;
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

  // Slightly tedious way to do this, almost certainly a better/nicer way to check if it was defined in a param file
  if (HMS_eRF_Offset == -5.1114){
    HMS_eRF_Offset = HMS_RF_Offset; // If electron HMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (HMS_piRF_Offset == -5.1114){
    HMS_piRF_Offset = HMS_RF_Offset; // If pion HMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (HMS_KRF_Offset == -5.1114){
    HMS_KRF_Offset = HMS_RF_Offset; // If kaon HMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
  }
  if (HMS_pRF_Offset == -5.1114){
    HMS_pRF_Offset = HMS_RF_Offset; // If proton HMS offset wasn't explicitly set, set it to the same value as the generic spectrometer offset
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
  
  // Add warning if strange offset value specified
  if(abs(HMS_RF_Offset) > Bunch_Spacing || abs(SHMS_RF_Offset) > Bunch_Spacing){
    cout << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "NOTICE : One (or both) RF offsets are set to a value which is larger than the bunch spacing for the run -" << endl;
    cout << "Bunch Spacing = " << Bunch_Spacing << " (ns)" << endl;
    cout << "HMS RF Offset = " << HMS_RF_Offset << " (ns)" << endl;
    cout << "SHMS RF Offset = " << SHMS_RF_Offset << " (ns)" << endl;
    cout << "This doesn't actually make much sense, the RFDistribution takes the modulo of the bunch spacing." << endl;
    cout << "Any value larger than the bunch spacing is effectively pointless, consider choosing the appropriate value within the range 0 to " << Bunch_Spacing << " instead." << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << endl;
  }

  // SJDK 16/11/22 - This must be to a specific point? Where? The first hodo plane?
  HMS_CentralPathLen = 22.0*100.; // Path length in cm
  SHMS_CentralPathLen = 18.1*100.; // Path legnth in cm

  THcAnalyzer *analyzer = dynamic_cast<THcAnalyzer*>(THcAnalyzer::GetInstance());
  fEpicsHandler = analyzer->GetEpicsEvtHandler();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcRFTime::DefineVariables( EMode mode )
{
  // Define variables that this script will actually calculate
  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );
  const RVarDef vars[] = {
    {"HMS_RFtimeDist", "HMS RF Time Distribution for PID", "fHMS_RFtimeDist"},
    {"HMS_eRFtimeDist", "HMS Corrected RF time Distribution for PID (e)", "fHMS_eRFtimeDist"},
    {"HMS_piRFtimeDist", "HMS Corrected RF time Distribution for PID (pi)", "fHMS_piRFtimeDist"},
    {"HMS_KRFtimeDist", "HMS Corrected RF time Distribution for PID (K)", "fHMS_KRFtimeDist"},
    {"HMS_pRFtimeDist", "HMS Corrected RF time Distribution for PID (p)", "fHMS_pRFtimeDist"},
    {"SHMS_RFtimeDist", "SHMS RF time Distribution for PID", "fSHMS_RFtimeDist"},
    {"SHMS_eRFtimeDist", "SHMS Corrected RF time Distribution for PID (e)", "fSHMS_eRFtimeDist"},
    {"SHMS_piRFtimeDist", "SHMS Corrected RF time Distribution for PID (pi)", "fSHMS_piRFtimeDist"},
    {"SHMS_KRFtimeDist", "SHMS Corrected RF time Distribution for PID (K)", "fSHMS_KRFtimeDist"},
    {"SHMS_pRFtimeDist", "SHMS Corrected RF time Distribution for PID (p)", "fSHMS_pRFtimeDist"},
    {0}
  };

  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
Int_t THcRFTime::Process( const THaEvData& evdata )
{
  static const char* const epics_tag = "MOFC1FREQ";

  if( !IsOK() || !gHaRun ) return -1;

  if (Bunch_Spacing_Epics == 0.0001 && fEpicsHandler->IsLoaded(epics_tag)){ // If Bunch_Spacing_Epics is still the default value (0.0001), set it to the Epics read in, this is to ensure it is only set ONCE
    Bunch_Spacing_Epics = (2.0/(atof(fEpicsHandler->GetString(epics_tag))))*(pow(10, 9)); // Calculation of Bunch Spacing, 2/f as we get every OTHER bucket typically, converted to ns
    if (Bunch_Spacing_Epics > 0.1 && Bunch_Spacing_Epics < 4.5){ // If the value looks sensible, check if an override value has been set
      if (Bunch_Spacing_Override == 1.5556){ // If the Bunch_Spacing_override has NOT been set by the user, set the bunch spacing to be the epics value
	Bunch_Spacing = Bunch_Spacing_Epics;
      }
    }
  }

  //Declare track information objects for hadron/electron arm
  THaTrackInfo* had_trkifo = fhadSpectro->GetTrackInfo();
  THaTrackInfo* elec_trkifo = felecSpectro->GetTrackInfo();

  if( !had_trkifo) cout << " no hadron track " << endl;
  if( !elec_trkifo) cout << " no electron track " << endl;
  // Check if the hadron/electron arm had a track
  if( !had_trkifo || !had_trkifo->IsOK() ) return 1;
  if( !elec_trkifo || !elec_trkifo->IsOK() ) return 1;

  //Create THaTrack object for hadron/elec arms to get relevant golden track quantities
  // 16/11/22 SJDK - Which arm is which is a little irrelevant for most of the RFTime variables
  // However, if coin time dependent corrections are implemented, then this will matter
  if (felecArmName=="H") {
    theSHMSTrack = (fhadSpectro->GetGoldenTrack());
    theHMSTrack = (felecSpectro->GetGoldenTrack());
  } else{
    theSHMSTrack = (felecSpectro->GetGoldenTrack());
    theHMSTrack = (fhadSpectro->GetGoldenTrack());
  }

  lightSpeed = 29.9792; // in cm/ns
  // Particle Masses (Hardcoded - should be able to grab from elsewhere?)
  elecMass =  0.510998/1000.0; // electron mass in GeV/c^2
  pionMass = 139.570/1000.0;    //charged pion mass in GeV/c^2
  kaonMass = 493.677/1000.0;    //charged kaon mass in GeV/c^2
  protonMass = 938.27208/1000.0; // proton mass in GeV/c^2	

  // HMS Arm
  Double_t HMS_xptar = theHMSTrack->GetTTheta();
  Double_t HMS_P = theHMSTrack->GetP();
  Double_t HMS_dP = theHMSTrack->GetDp();     
  Double_t HMS_FPtime = theHMSTrack->GetFPTime();
  // SHMS Arm
  Double_t SHMS_xptar = theSHMSTrack->GetTTheta();
  Double_t SHMS_P = theSHMSTrack->GetP();
  Double_t SHMS_dP = theSHMSTrack->GetDp();
  Double_t SHMS_FPtime = theSHMSTrack->GetFPTime();

  if (SHMS_FPtime==-2000 || HMS_FPtime==-2000)  return 1;
  if (SHMS_FPtime==-1000 || HMS_FPtime==-1000)  return 1;

  HMS_RFtime = fCoinDet->Get_RF_TrigTime(1); // HMS is ID 1
  SHMS_RFtime = fCoinDet->Get_RF_TrigTime(0); // SHMS is ID 0
  
  // These calculations are taken from the CoinTime class by C.Yero. There is another calculation which uses xfp, xpfp and ypfp, but this does not seem to be used?
  HMS_DeltaPathLen = HMS_CentralPathLen + (.12*HMS_xptar*1000) + (0.17*(HMS_dP/100.)); // Path length in cm
  SHMS_DeltaPathLen = SHMS_CentralPathLen + (.11*SHMS_xptar*1000) + (0.057*(SHMS_dP/100.));

  // beta calculations beta = v/c = p/E
  // Note that this is the TRACK momentum and not the spectrometer momentum
  HMS_Beta_Calc_e = HMS_P / sqrt((HMS_P*HMS_P)+(elecMass*elecMass));
  HMS_Beta_Calc_pi = HMS_P / sqrt((HMS_P*HMS_P)+(pionMass*pionMass));
  HMS_Beta_Calc_K = HMS_P / sqrt((HMS_P*HMS_P)+(kaonMass*kaonMass));
  HMS_Beta_Calc_p = HMS_P / sqrt((HMS_P*HMS_P)+(protonMass*protonMass));
  SHMS_Beta_Calc_e = SHMS_P / sqrt((SHMS_P*SHMS_P)+(elecMass*elecMass));
  SHMS_Beta_Calc_pi = SHMS_P / sqrt((SHMS_P*SHMS_P)+(pionMass*pionMass));
  SHMS_Beta_Calc_K = SHMS_P / sqrt((SHMS_P*SHMS_P)+(kaonMass*kaonMass));
  SHMS_Beta_Calc_p = SHMS_P / sqrt((SHMS_P*SHMS_P)+(protonMass*protonMass));

  // ToF for delta path by particle species
  HMS_RFDeltaTime_e = (HMS_DeltaPathLen)/(lightSpeed*HMS_Beta_Calc_e);
  HMS_RFDeltaTime_pi = (HMS_DeltaPathLen)/(lightSpeed*HMS_Beta_Calc_pi);
  HMS_RFDeltaTime_K = (HMS_DeltaPathLen)/(lightSpeed*HMS_Beta_Calc_K);
  HMS_RFDeltaTime_p = (HMS_DeltaPathLen)/(lightSpeed*HMS_Beta_Calc_p);
  SHMS_RFDeltaTime_e = (SHMS_DeltaPathLen)/(lightSpeed*SHMS_Beta_Calc_e);
  SHMS_RFDeltaTime_pi = (SHMS_DeltaPathLen)/(lightSpeed*SHMS_Beta_Calc_pi);
  SHMS_RFDeltaTime_K = (SHMS_DeltaPathLen)/(lightSpeed*SHMS_Beta_Calc_K);
  SHMS_RFDeltaTime_p = (SHMS_DeltaPathLen)/(lightSpeed*SHMS_Beta_Calc_p);

  // RF Time dist can be utilised for PID, offsets should be set in Standard.kinematics, these are just used to "center" the distribution at a desired point
  // Typically, other PID info needs to be utilised to establish where the relevant peaks for each particle species are in the distribution
  // Note, this expression looks a bit odd but this is to achieve the same results as the Python a % b operation
  // The result of this calculation will ALWAYS be a POSITIVE number between 0 and Bunch_Spacing
  // See - https://stackoverflow.com/questions/1907565/c-and-python-different-behaviour-of-the-modulo-operation for more
  fHMS_RFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_RF_Offset), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_RFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_RF_Offset), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);

  // 21/10/22 - Determine a corrected RFtime based upon the path length/velocity for different particle species.
  // Based upon the CTime correction by C. Yero and work by S. Jia
  // Beta is calculated for each particle species and the path length is determined from delta and xptar. From this, a ToF for each particle species is calculated and added in the modulo. This produces a flat distribution across delta when appropriate PID is applied
  // An offset can be applied for each particle type if desired to centre their distributions arbitrarily
  fHMS_eRFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_eRF_Offset + HMS_RFDeltaTime_e), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fHMS_piRFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_piRF_Offset + HMS_RFDeltaTime_pi), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fHMS_KRFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_KRF_Offset + HMS_RFDeltaTime_K), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fHMS_pRFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_pRF_Offset + HMS_RFDeltaTime_p), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_eRFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_eRF_Offset + SHMS_RFDeltaTime_e), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_piRFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_piRF_Offset + SHMS_RFDeltaTime_pi), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_KRFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_KRF_Offset + SHMS_RFDeltaTime_K), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_pRFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_pRF_Offset + SHMS_RFDeltaTime_p), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcRFTime)
////////////////////////////////////////////////////////////////////////////////
