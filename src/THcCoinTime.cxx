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
  //Get coin time offset
  //pathlenghths for HMS/SHMS

  return kOK;
}

//_____________________________________________________________________________
Int_t THcCoinTime::DefineVariables( EMode mode )
{

  //if( mode == kDefine && fIsSetup ) return kOK;
  //fIsSetup = ( mode == kDefine );


  //return DefineVarsFromList( vars, mode );
  return 0;
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

  SHMScentralPathLen =  18.1*100;    //cm       //TODO: put in a GEN Param file, and read it from there 
  HMScentralPathLen =  22.*100;   //cm          //TODO: put in a GEN Param file, and read it from there 
 
  //Check if there was a golden track in both arms
  if (!theHadTrack && !theElecTrack) return 1;

  //Check if Database is reading the correct elec-arm particle mass
  if (felecSpectro->GetParticleMass() < 0.00052)
   {

     
      //electron arm
      elec_P = theElecTrack->GetP();              //electron golden track arm momentum
      elec_th = theElecTrack->GetTTheta();        //xp_tar
      elec_dP = theElecTrack->GetDp();            //electron arm deltaP 
      elec_FPtime = theElecTrack->GetFPTime();    //electron arm focal plane time
      
      //hadron arm
      had_P = theHadTrack->GetP();              //hadron golden track arm momentum
      had_xfp = theHadTrack->GetRX();           //x_fp
      had_xpfp = theHadTrack->GetRTheta();      //xp_fp
      had_ypfp = theHadTrack->GetRPhi();        //yp_fp
      had_FPtime = theHadTrack->GetFPTime();    //hadron-arm focal plane time


     //HMS/SHMS pathlength correction
      //DeltaSHMSpathLength = -0.11*atan2(elec_th);
     
      
      
      elecArm_BetaCalc = elec_P / sqrt(elec_P*elec_P + elecMass*elecMass);



   }
  
  
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
