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
  fSHMS_RFtimeDist=kBig;
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
    {"SHMS_RF_Offset",  &SHMS_RF_Offset, kDouble, 0, 1}, // RF offset for SHMS
    {"Bunch_Spacing_Override",  &Bunch_Spacing_Override, kDouble, 0, 1}, // Bunch spacing value, can be manually set to override the Epics read in
    {0}
  };

  // Default values if not read from param file
  Bunch_Spacing_Override = 1.5556; // Strangely specific value so we can check if it's set or not
  HMS_RF_Offset = 0.0;
  SHMS_RF_Offset = 0.0;
  gHcParms->LoadParmValues((DBRequest*)&list, "");
  // If the override value has been set (i.e. it is NOT the default value), set it to whatever the read in is
  if ( Bunch_Spacing_Override != 1.5556){
    Bunch_Spacing = Bunch_Spacing_Override;
  }
  // If override wasn't set, assume the bunch spacing has some default value (will be overriden by Epics if it looks OK)
  else if (Bunch_Spacing_Override == 1.5556){
    Bunch_Spacing = 4.008;
  }

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
    {"HMS_RFtimeDist",    "HMS RF Time Disttribution for PID",  "fHMS_RFtimeDist"},
    {"SHMS_RFtimeDist",    "SHMS RF Time Disttribution for PID",  "fSHMS_RFtimeDist"},
    { 0 }
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
  // May want to consider dropping this condition
  if( !had_trkifo || !had_trkifo->IsOK() ) return 1;
  if( !elec_trkifo || !elec_trkifo->IsOK() ) return 1;

  //Create THaTrack object for hadron/elec arms to get relevant golden track quantities
  if (felecArmName=="H") {
    theSHMSTrack = (fhadSpectro->GetGoldenTrack());
    theHMSTrack = (felecSpectro->GetGoldenTrack());
  } else{
    theSHMSTrack = (felecSpectro->GetGoldenTrack());
    theHMSTrack = (fhadSpectro->GetGoldenTrack());
  }

  //Check if Database is reading the correct elec-arm particle mass
  // if (felecSpectro->GetParticleMass() > 0.00052) return 1;

  Double_t SHMS_FPtime = theSHMSTrack->GetFPTime();// SHMS arm
  Double_t HMS_FPtime = theHMSTrack->GetFPTime();  // HMS arm

  if (SHMS_FPtime==-2000 || HMS_FPtime==-2000)  return 1;
  if (SHMS_FPtime==-1000 || HMS_FPtime==-1000)  return 1;

  SHMS_RFtime = fCoinDet->Get_RF_TrigTime(0); // SHMS is ID 0
  HMS_RFtime = fCoinDet->Get_RF_TrigTime(1); // HMS is ID 1

  // RF Time dist can be utilised for PID, offsets should be set in Standard.kinematics, these are just used to "center" the distribution at a desired point
  // Typically, other PID info needs to be utilised to establish where the relevant peaks for each particle species are in the distribution
  // Note, this expression looks a bit odd but this is to achieve the same results as the Python a % b operation
  // The result of this calculation will ALWAYS be a POSITIVE number between 0 and Bunch_Spacing
  // See - https://stackoverflow.com/questions/1907565/c-and-python-different-behaviour-of-the-modulo-operation for more
  fHMS_RFtimeDist = fmod((fmod((HMS_RFtime - HMS_FPtime + HMS_RF_Offset), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);
  fSHMS_RFtimeDist = fmod((fmod((SHMS_RFtime - SHMS_FPtime + SHMS_RF_Offset), Bunch_Spacing) + Bunch_Spacing), Bunch_Spacing);

  return 0;
}

//_____________________________________________________________________________

ClassImp(THcRFTime)
////////////////////////////////////////////////////////////////////////////////
