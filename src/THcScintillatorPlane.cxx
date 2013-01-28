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
#include "THcHitList.h"
#include "THcHodoscope.h"
#include "TClass.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  fPlaneNum = planenum;
  fTotPlanes = planenum;
}
//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    const Int_t totplanes,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  fPlaneNum = planenum;
  fTotPlanes = totplanes;
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

  // Get the Hodoscope hitlist
  // Can't seem to cast to THcHitList.  What to do if we want to use
  // THcScintillatorPlane as a subdetector to other than THcHodoscope?
  //  fParentHitList = static_cast<THcHodoscope*>(GetParent())->GetHitList();

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  //  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  /*  strcpy(parname,prefix);
  strcat(parname,"scin_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);

  strcat(parname,"_nr");
  cout << " Getting value of " << parname << endl;
  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();

  parname[plen]='\0';
  strcat(parname,"_spacing");

  fSpacing =  gHcParms->Find(parname)->GetValue(0);
  cout <<"fSpacing = "<<fSpacing<<endl;
  */

  // need this further down so read them first! GN
  strcpy(parname,prefix);
  strcat(parname,"scin_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);
  strcat(parname,"_nr");
  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();
  //
  char *tmpleft, *tmpright;
  if (fPlaneNum==1 || fPlaneNum==3) {
    tmpleft="left";
    tmpright="right";
  } 
  else {
    tmpleft="top";
    tmpright="bot";
  }

  Double_t tmpdouble[fTotPlanes];
    DBRequest list[]={
     {Form("scin_%s_zpos",GetName()), &fZpos, kDouble},
     {Form("scin_%s_dzpos",GetName()), &fDzpos, kDouble},
     {Form("scin_%s_size",GetName()), &fSize, kDouble},
     {Form("scin_%s_spacing",GetName()), &fSpacing, kDouble},
     {Form("scin_%s_%s",GetName(),tmpleft), &fPosLeft,kDouble},
     {Form("scin_%s_%s",GetName(),tmpright), &fPosRight,kDouble},
     {Form("scin_%s_offset",GetName()), &fPosOffset, kDouble},
     {Form("scin_%s_center",GetName()), &fPosCenter[0],kDouble,fNelem},
     // this is from Xhodo.param...
          {"hodo_slop",&tmpdouble[0],kDouble,fTotPlanes},
     {0}
    };
   gHcParms->LoadParmValues((DBRequest*)&list,prefix);
   // fetch the parameter from the temporary list
   fHodoSlop=tmpdouble[fPlaneNum-1];
   cout <<" plane num = "<<fPlaneNum<<endl;
   cout <<" nelem     = "<<fNelem<<endl;
   cout <<" zpos      = "<<fZpos<<endl;
   cout <<" dzpos     = "<<fDzpos<<endl;
   cout <<" spacing   = "<<fSpacing<<endl;
   cout <<" size      = "<<fSize<<endl;
   cout <<" hodoslop  = "<<fHodoSlop<<endl;
   cout <<"PosLeft = "<<fPosLeft<<endl;
   cout <<"PosRight = "<<fPosRight<<endl;
   cout <<"PosOffset = "<<fPosOffset<<endl;
   cout <<"PosCenter[0] = "<<fPosCenter[0]<<endl;

  /* for(Int_t i=0;i<fNPlanes;i++) {
    fCenter[i] = new Double_t[fNPaddle[i]];
    DBRequest list[]={
      {Form("scin_%s_spacing",fPlaneNames[i]), &fSpacing[i], kDouble},
      {Form("scin_%s_center",fPlaneNames[i]), fCenter[i], kDouble, fNPaddle[i]},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  }
  */
  ///  Int_t tmpint = new Int_t [GetParent()->GetNPlanes()]; 
  /// cout<<"Nplanes in scintplane!! = "<<fTotPlanes <<endl;

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
  InitializePedestals();

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
     "fPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdchits", "List of Negative TDC hits", 
     "fNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcScintillatorPlane::Clear( Option_t* )
{
  //cout << " Calling THcScintillatorPlane::Clear " << GetName() << endl;
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
 
  cout <<"*******************************\n";
  cout <<"NOW IN THcScintilatorPlane::CoarseProcess!!!!\n";
  cout <<"*******************************\n";
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }

    // TDC positive hit
    if(hit->fTDC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_pos);
    }

    // TDC negative hit
    if(hit->fTDC_neg >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_neg);
    }

    // ADC positive hit
    if(hit->fADC_pos >  0) {
      //     cout <<"adc pos hit!!\n";
      THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_pos);
    }

    // ADC negative hit
    if(hit->fADC_neg >  0) {   
      // cout <<"adc neg hit!!\n";
      THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_neg);
    }

    ihit++;
  }
  return(ihit);
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ProcessHits(TClonesArray* rawhits, Double_t mintdc, Double_t maxtdc,Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.
  // GN: Select only events that have at least one of their TDC signals in the
  // right range. 
  // NOTE: 01/24/2013: Not quite subtracting pedestals for now! Also subtract the pedestals from the adc signal

  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }

    // check TDC values
    if (((hit->fTDC_pos >= mintdc) && (hit->fTDC_pos <= maxtdc)) ||
	((hit->fTDC_neg >= mintdc) && (hit->fTDC_neg <= maxtdc))) {
      //TDC positive hit
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_pos);
      // TDC negative hit
      THcSignalHit *sighit2 = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
      sighit2->Set(hit->fCounter, hit->fTDC_neg);
      // ADC positive hit
      if(hit->fADC_pos >  0) {
	THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
	sighit->Set(hit->fCounter, hit->fADC_pos);
      }
      // ADC negative hit
      if(hit->fADC_neg >  0) {   
	// cout <<"adc neg hit!!\n";
	THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
	sighit->Set(hit->fCounter, hit->fADC_neg);
      }
      else {
	cout <<"skipping BAD tdc event\n";
      }
    }
    ihit++;
  }
  return(ihit);
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list, accumulating into
  // arrays for calculating pedestals.

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::AcculatePedestals " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t element = hit->fCounter - 1; // Should check if in range
    Int_t adcpos = hit->fADC_pos;
    Int_t adcneg = hit->fADC_neg;

    if(adcpos <= fPosPedLimit[element]) {
      fPosPedSum[element] += adcpos;
      fPosPedSum2[element] += adcpos*adcpos;
      fPosPedCount[element]++;
      if(fPosPedCount[element] == fMinPeds/5) {
	fPosPedLimit[element] = 100 + fPosPedSum[element]/fPosPedCount[element];
      }
    }
    if(adcneg <= fNegPedLimit[element]) {
      fNegPedSum[element] += adcneg;
      fNegPedSum2[element] += adcneg*adcneg;
      fNegPedCount[element]++;
      if(fNegPedCount[element] == fMinPeds/5) {
	fNegPedLimit[element] = 100 + fNegPedSum[element]/fNegPedCount[element];
      }
    }
    ihit++;
  }

  fNPedestalEvents++;

  return(ihit);
}

//_____________________________________________________________________________
void THcScintillatorPlane::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")
  //  cout << "Plane: " << fPlaneNum << endl;
  for(Int_t i=0; i<fNelem;i++) {
    
    // Positive tubes
    fPosPed[i] = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosThresh[i] = fPosPed[i] + 15;

    // Negative tubes
    fNegPed[i] = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegThresh[i] = fNegPed[i] + 15;

    //    cout << i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;
  }
  //  cout << " " << endl;
  
}

//_____________________________________________________________________________
void THcScintillatorPlane::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedLimit = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum = new Int_t [fNelem];
  fNegPedSum2 = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];
  fNegPedCount = new Int_t [fNelem];

  fPosPed = new Double_t [fNelem];
  fNegPed = new Double_t [fNelem];
  fPosThresh = new Double_t [fNelem];
  fNegThresh = new Double_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fNegPedCount[i] = 0;
  }
}
//____________________________________________________________________________
Int_t THcScintillatorPlane::GetNelem() 
{
  return fNelem;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetSpacing()
{
  return fSpacing;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetSize()
{
  return fSize;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetHodoSlop()
{
  return fHodoSlop;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetZpos()
{
  return fZpos;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetDzpos()
{
  return fDzpos;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosLeft() {
  return fPosLeft;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosRight() {
  return fPosRight;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosOffset() {
  return fPosOffset;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosCenter(Int_t PaddleNo) {
  return fPosCenter[PaddleNo];
}
//____________________________________________________________________________
ClassImp(THcScintillatorPlane)
////////////////////////////////////////////////////////////////////////////////

