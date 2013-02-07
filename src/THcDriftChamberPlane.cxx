//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcDriftChamberPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcDriftChamberPlane.h"
#include "THcDCWire.h"
#include "THcDCHit.h"
#include "THcDCLookupTTDConv.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcDriftChamber.h"
#include "THcHodoscope.h"
#include "TClass.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcDriftChamberPlane)

//______________________________________________________________________________
THcDriftChamberPlane::THcDriftChamberPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fHits = new TClonesArray("THcDCHit",100);
  fWires = new TClonesArray("THcDCWire", 100);

  fPlaneNum = planenum;
}

//______________________________________________________________________________
THcDriftChamberPlane::~THcDriftChamberPlane()
{
  // Destructor
  delete fWires;
  delete fHits;
  delete fTTDConv;

}
THaAnalysisObject::EStatus THcDriftChamberPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  cout << "THcDriftChamberPlane::Init called " << GetName() << endl;

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
Int_t THcDriftChamberPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  // Retrieve parameters we need
  THcDriftChamber* fParent;

  fParent = (THcDriftChamber*) GetParent();
  // These are single variables here, but arrays in THcDriftChamber.
  fNChamber = fParent->GetNChamber(fPlaneNum);
  fNWires = fParent->GetNWires(fPlaneNum);
  fWireOrder = fParent->GetWireOrder(fPlaneNum);
  fPitch = fParent->GetPitch(fPlaneNum);
  fCentralWire = fParent->GetCentralWire(fPlaneNum);
  fTdcWinMin = fParent->GetTdcWinMin(fPlaneNum);
  fTdcWinMax = fParent->GetTdcWinMax(fPlaneNum);
  fPlaneTimeZero = fParent->GetPlaneTimeZero(fPlaneNum);
  fCenter = fParent->GetCenter(fPlaneNum);

  fNSperChan = fParent->GetNSperChan();

  cout << fPlaneNum << " " << fNWires << endl;

  fTTDConv = new THcDCLookupTTDConv();// Need to pass the lookup table

  Int_t nWires = fParent->GetNWires(fPlaneNum);
  // For HMS, wire numbers start with one, but arrays start with zero.
  // So wire number is index+1
  for (int i=0; i<nWires; i++) {
    Double_t pos = fPitch*( (fWireOrder==0?(i+1):fNWires-i) 
			    - fCentralWire) - fCenter;
    THcDCWire* wire = new((*fWires)[i])
      THcDCWire( i+1, pos , 0.0, fTTDConv);
    //if( something < 0 ) wire->SetFlag(1);
  }

  THaApparatus* app = GetApparatus();
  const char* nm = "hod";
  if(  !app || 
      !(fglHod = dynamic_cast<THcHodoscope*>(app->GetDetector(nm))) ) {
    Warning(Here(here),"Hodoscope \"%s\" not found. "
	    "Event-by-event time offsets will NOT be used!!",nm);
  }

  return kOK;
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  //  cout << "THcDriftChamberPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"tdchits", "List of TDC hits", 
     "fHits.THcDCHit.GetWireNum()"},
    {"rawtdc", "Raw TDC Values", 
     "fHits.THcDCHit.GetRawTime()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcDriftChamberPlane::Clear( Option_t* )
{
  //cout << " Calling THcDriftChamberPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fHits->Clear();
}

//_____________________________________________________________________________
Int_t THcDriftChamberPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcDriftChamberPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcDriftChamberPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
Int_t THcDriftChamberPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Double_t StartTime = 0.0;
  // Would be nice to have a way to determine that the hodoscope decode was
  // actually called for this event.
  if( fglHod ) StartTime = fglHod->GetStartTime();
  //cout << "Start time " << StartTime << endl;

  //Int_t nTDCHits=0;
  fHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcDriftChamberPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  Int_t nextHit = 0;
  while(ihit < nrawhits) {
    THcRawDCHit* hit = (THcRawDCHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t wireNum = hit->fCounter;
    THcDCWire* wire = GetWire(wireNum);
    Int_t wire_last = -1;
    for(Int_t mhit=0; mhit<hit->fNHits; mhit++) {
      /* Sort into early, late and ontime */
      Int_t rawtdc = hit->fTDC[mhit];
      if(rawtdc < fTdcWinMin) {
	// Increment early counter  (Actually late because TDC is backward)
      } else if (rawtdc > fTdcWinMax) {
	// Increment late count 
      } else {
	// A good hit
	if(wire_last == wireNum) {
	  // Increment extra hit counter 
	  // Are we choosing the correct hit in the case of multiple hits?
	  // Are we choose the same hit that ENGINE chooses?
	  // cout << "Extra hit " << fPlaneNum << " " << wireNum << " " << rawtdc << endl;
	} else {
	  Double_t time = -StartTime   // (comes from h_trans_scin
	    - rawtdc*fNSperChan + fPlaneTimeZero;
	  // How do we get this start time from the hodoscope to here
	  // (or at least have it ready by coarse process)
	  new( (*fHits)[nextHit++] ) THcDCHit(wire, rawtdc, time);
	}
	wire_last = wireNum;
      }
    }
    ihit++;
  }
  return(ihit);
}

    
  
  
