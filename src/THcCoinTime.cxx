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
#include "THaApparatus.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"

using namespace std;

//_____________________________________________________________________________
THcCoinTime::THcCoinTime (const char *name, const char* description,
			const char* coinname) :
  THaPhysicsModule(name, description), fName(coinname), fCoinTRG(NULL), fNevt(0)
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
Int_t THcCoinTime::Begin( THaRunBase* )
{
  // Start of analysis

  if (!IsOK() ) return -1;

  return 0;
}

//_____________________________________________________________________________
Int_t THcCoinTime::End( THaRunBase* )
{
  // End of analysis


  return 0;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCoinTime::Init( const TDatime& run_time )
{
  // Initialize THcCoinTime physics module

  //  const char* const here = "Init";

  // Standard initialization. Calls ReadDatabase(), ReadRunDatabase(),
  // and DefineVariables() (see THaAnalysisObject::Init)

  
  if( THaPhysicsModule::Init( run_time ) != kOK )
    return fStatus;
  

  return fStatus = kOK;
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

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );


  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
Int_t THcCoinTime::Process( const THaEvData& evdata )
{


  if( !IsOK() ) return -1;

  return 0;
}

//_____________________________________________________________________________

ClassImp(THcCoinTime)
////////////////////////////////////////////////////////////////////////////////
