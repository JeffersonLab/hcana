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
  fCoinDet(NULL), 
  fNevt(0)
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

  cout << "^^^^THcCoinTime: NEW EVENT^^^^^^NEW EVENT^^^^^^^NEW EVENT^^^^^^^^" << endl;
  cout << ">>>Calling function Get_pTRG1_ROC1_rawTdctime(): " << fCoinDet->Get_pTRG1_ROC1_rawTdctime() << endl;
  cout << ">>>Calling function Get_pTRG4_ROC1_rawTdctime(): " << fCoinDet->Get_pTRG4_ROC1_rawTdctime() << endl;
  cout << ">>>Calling function Get_pTRG1_ROC2_rawTdctime(): " << fCoinDet->Get_pTRG1_ROC2_rawTdctime() << endl;
  cout << ">>>Calling function Get_pTRG4_ROC2_rawTdctime(): " << fCoinDet->Get_pTRG4_ROC2_rawTdctime() << endl;

  //  if( !IsOK() ) return -1;

  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
