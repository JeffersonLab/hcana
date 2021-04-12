/** \class THcRFTime
    \ingroup PhysMods

// Brief Class for calculating and adding the RF Time in the Tree.

// Author: Stephen JD Kay
// University of Regina
// Date: 12/04/21 (12th April 2021)
// Based on THcCoinTime class from C Yero
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

#include "THcRFTime.h"
#include "THcTrigDet.h"
#include "THaApparatus.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcAnalyzer.h"

using namespace std;

//_____________________________________________________________________________
THcRFTime::THcRFTime (const char *name, const char* description, const char* hadArmName, 
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
  cout << "**************************************************" << endl;

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
    {"Bunch_Spacing_Override",  &Bunch_Spacing_Override, kDouble, 0, 1}, // RF offset for SHMS
    {0}
  };
  
  //Default values if not read from param file
  Bunch_Spacing_Override = 4.008;
  HMS_RF_Offset = 0.0;
  SHMS_RF_Offset = 0.0;
  
  gHcParms->LoadParmValues((DBRequest*)&list, "");

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
Int_t THcRFTime::Decode( const THaEvData& evdata )   
{
  if (fEpicsHandler) {
    if (fEpicsHandler->IsLoaded("MOFC1DELTA")){                                                
      Bunch_Spacing = atof(fEpicsHandler->GetString("MOFC1DELTA"));                                     
    }   
  }
  else{
    Bunch_Spacing = Bunch_Spacing_Override;
  }
  return 0;
}

//_____________________________________________________________________________
Int_t THcRFTime::Process( const THaEvData& evdata )
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
  
  //Check if there was a golden track in both arms
  if (!theSHMSTrack || !theHMSTrack)
    {
      return 1;
    }

  //Check if Database is reading the correct elec-arm particle mass
  if (felecSpectro->GetParticleMass() > 0.00052) return 1;
        
  //SHMS arm
  Double_t SHMS_FPtime = theSHMSTrack->GetFPTime();

  //HMS arm
  Double_t HMS_FPtime = theHMSTrack->GetFPTime();
      
  if (SHMS_FPtime==-2000 || HMS_FPtime==-2000)  return 1;
  if (SHMS_FPtime==-1000 || HMS_FPtime==-1000)  return 1;
      
  //Get raw TDC Times for HMS/SHMS (3/4 trigger)
  SHMS_RFtime = fCoinDet->Get_RF_TrigTime(0); // SHMS
  HMS_RFtime = fCoinDet->Get_RF_TrigTime(1); // HMS

  fHMS_RFtimeDist = (HMS_RFtime - HMS_FPtime + HMS_RF_Offset);
  fSHMS_RFtimeDist = (SHMS_RFtime - SHMS_FPtime + SHMS_RF_Offset);
  
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcRFTime)
////////////////////////////////////////////////////////////////////////////////
