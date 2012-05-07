//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcScintillatorPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
		    const char* description,
		    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor
  delete fPosTDCHits;
  delete fNegTDCHits;
  delete fPosADCHits;
  delete fNegADCHits;

}
THaAnalysisObject::EStatus THcScintillatorPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  cout << "THcScintillatorPlane::Init called " << GetName() << endl;

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,"scin_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);

  strcat(parname,"_nr");
  cout << " Getting value of " << parname << endl;
  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  parname[plen]='\0';
  strcat(parname,"_spacing");

  fSpacing =  gHcParms->Find(parname)->GetValue(0);
  
  // First letter of GetParent()->GetPrefix() tells us what prefix to
  // use on parameter names.  


  //  Find the number of elements
  
  // Think we will make special methods to pass most
  // How generic do we want to make this class?  
  // The way we get parameter data is going to be pretty specific to
  // our parameter file naming conventions.  But on the other hand,
  // the Hall A analyzer read database is pretty specific.
  // Is there any way for this class to know which spectrometer he
  // belongs too?


  // Create arrays to hold results here


  return kOK;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  cout << "THcScintillatorPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"postdchits", "List of Positive TDC hits", 
     "fPosTDCHits.THcSignalHit.GetPaddle()"},
    {"negtdchits", "List of Negative TDC hits", 
     "fNegTDCHits.THcSignalHit.GetPaddle()"},
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddle()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddle()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcScintillatorPlane::Clear( Option_t* )
{
  cout << " Calling THcScintillatorPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcScintillatorPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
