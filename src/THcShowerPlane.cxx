//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcShowerPlane
//
//////////////////////////////////////////////////////////////////////////

#include "THcShowerPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "TClass.h"
#include "math.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <fstream>
using namespace std;

ClassImp(THcShowerPlane)

//______________________________________________________________________________
THcShowerPlane::THcShowerPlane( const char* name, 
					    const char* description,
					    const Int_t layernum,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosADCHits = new TClonesArray("THcSignalHit",13);
  fNegADCHits = new TClonesArray("THcSignalHit",13);

  //#if ROOT_VERSION_CODE < ROOT_VERSION(5,32,0)
  //  fPosADCHitsClass = fPosADCHits->GetClass();
  //  fNegADCHitsClass = fNegADCHits->GetClass();
  //#endif

  fLayerNum = layernum;
}

//______________________________________________________________________________
THcShowerPlane::~THcShowerPlane()
{
  // Destructor
  delete fPosADCHits;
  delete fNegADCHits;

  delete [] fA_Pos;
  delete [] fA_Neg;
  delete [] fA_Pos_p;
  delete [] fA_Neg_p;

  delete [] fEpos;
  delete [] fEneg;
  delete [] fEmean;
}

THaAnalysisObject::EStatus THcShowerPlane::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

  cout << "THcShowerPlane::Init called " << GetName() << endl;

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
Int_t THcShowerPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for
  
  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];
  
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);
  strcat(parname,"cal_");
  strcat(parname,GetName());
  Int_t plen=strlen(parname);

  strcat(parname,"_nr");
  cout << " Getting value of SHOWER!!!" << parname << endl;

  // Retrieve parameters we need from parent class
  THcShower* fParent;

  fParent = (THcShower*) GetParent();

  fNelem = fParent->GetNBlocks(fLayerNum-1);

  //  cout << "THcShowerPlane::ReadDatabase: fLayerNum=" << fLayerNum 
  //       << "  fNelem=" << fNelem << endl;

  fA_Pos = new Float_t[fNelem];
  fA_Neg = new Float_t[fNelem];
  fA_Pos_p = new Float_t[fNelem];
  fA_Neg_p = new Float_t[fNelem];

  fEpos = new Float_t[fNelem];
  fEneg = new Float_t[fNelem];
  fEmean= new Float_t[fNelem];

 //  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();
// 
//   parname[plen]='\0';
//   strcat(parname,"_spacing");
// 
//   fSpacing =  gHcParms->Find(parname)->GetValue(0);
  
  // First letter of GetParent()->GetPrefix() tells us what prefix to
  // use on parameter names.  


  //  Find the number of elements
  
  // Create arrays to hold results here

  fPosPedLimit = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPosPedLimit[i] = fParent->GetPedLimit(i,fLayerNum-1,0);
    fNegPedLimit[i] = fParent->GetPedLimit(i,fLayerNum-1,1);
  }

  cout << "   fPosPedLimit:";
  for(Int_t i=0;i<fNelem;i++) cout << " " << fPosPedLimit[i];
  cout << endl;
  cout << "   fNegPedLimit:";
  for(Int_t i=0;i<fNelem;i++) cout << " " << fNegPedLimit[i];
  cout << endl;

  fMinPeds = fParent->GetMinPeds();
  cout << "   fMinPeds = " << fMinPeds << endl;

  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  cout << "THcShowerPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"posadchits", "List of Positive ADC hits","fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits","fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"apos",   "Raw Positive ADC Amplitudes",            "fA_Pos"},
    {"aneg",   "Raw Negative ADC Amplitudes",            "fA_Neg"},
    {"apos_p", "Ped-subtracted Positive ADC Amplitudes", "fA_Pos_p"},
    {"aneg_p", "Ped-subtracted Negative ADC Amplitudes", "fA_Neg_p"},
    {"epos",   "Energy Depositions from Positive Side PMTs", "fEpos"},
    {"eneg",   "Energy Depositions from Negative Side PMTs", "fEneg"},
    {"emean",  "Mean Energy Depositions",                    "fEMean"},
    {"eplane", "Energy Deposition per plane",                "fEplane"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  //cout << " Calling THcShowerPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosADCHits->Clear();
  fNegADCHits->Clear();
}

//_____________________________________________________________________________
Int_t THcShowerPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcShowerPlane::Decode " << GetName() << endl;

  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcess( TClonesArray& tracks )
{
 
  //  HitCount();

  cout << "THcShowerPlane::CoarseProcess called ---------------------" << endl;

 return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}

Int_t THcShowerPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list
  // Assumes that the hit list is sorted by layer, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  for(Int_t i=0;i<fNelem;i++) {
    fA_Pos[i] = 0;
    fA_Neg[i] = 0;
    fA_Pos_p[i] = 0;
    fA_Neg_p[i] = 0;
    fEpos[i] = 0;
    fEneg[i] = 0;
    fEmean[i] = 0;
  }

  fEplane = 0;

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;
  //cout << "nrawhits =  " << nrawhits << endl;
  //cout << "nexthit =  " << nexthit << endl;
  while(ihit < nrawhits) {
    THcShowerHit* hit = (THcShowerHit *) rawhits->At(ihit);

    //cout << "fplane =  " << hit->fPlane << " Num = " << fLayerNum << endl;
    if(hit->fPlane > fLayerNum) {
      break;
    }
    
    // Should probably check that counter # is in range
    fA_Pos[hit->fCounter-1] = hit->fADC_pos;
    fA_Neg[hit->fCounter-1] = hit->fADC_neg;

    fA_Pos_p[hit->fCounter-1] = hit->fADC_pos - fPosPed[hit->fCounter -1];
    fA_Neg_p[hit->fCounter-1] = hit->fADC_neg - fNegPed[hit->fCounter -1];

    THcShower* fParent;
    fParent = (THcShower*) GetParent();

    double thresh_pos = fPosThresh[hit->fCounter -1];
    if(hit->fADC_pos >  thresh_pos) {

      THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_pos);

      fEpos[hit->fCounter-1] += fA_Pos_p[hit->fCounter-1]*
	fParent->GetGain(hit->fCounter-1,fLayerNum-1,0);
    }

    double thresh_neg = fNegThresh[hit->fCounter -1];
    if(hit->fADC_neg >  thresh_neg) {

      THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_neg);

      fEneg[hit->fCounter-1] += fA_Neg_p[hit->fCounter-1]*
	fParent->GetGain(hit->fCounter-1,fLayerNum-1,1);
    }

    fEmean[hit->fCounter-1] += (fEpos[hit->fCounter-1] + fEneg[hit->fCounter-1]);
    fEplane += fEmean[hit->fCounter-1];

    ihit++;
  }
  return(ihit);
}

//_____________________________________________________________________________
Int_t THcShowerPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list, accumulating into
  // arrays for calculating pedestals.

  Int_t nrawhits = rawhits->GetLast()+1;
  //  cout << "THcScintillatorPlane::AcculatePedestals " << fLayerNum << " " << nexthit << "/" << nrawhits << endl;
  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcShowerHit* hit = (THcShowerHit *) rawhits->At(ihit);
    //cout << "fPlane =  " << hit->fPlane << " Limit = " << fPlaneNum << endl;
    if(hit->fPlane > fLayerNum) {
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
void THcShowerPlane::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")

  for(Int_t i=0; i<fNelem;i++) {
    
    // Positive tubes
    fPosPed[i] = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosSig[i] = sqrt((fPosPedSum2[i] - 2.*fPosPed[i]*fPosPedSum[i])/TMath::Max(1, fPosPedCount[i]) + fPosPed[i]*fPosPed[i]);
    //  fPosThresh[i] = fPosPed[i] + 15;
    fPosThresh[i] = fPosPed[i] + TMath::Min(50., TMath::Max(10., 3.*fPosSig[i]));

    // Negative tubes
    fNegPed[i] = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegSig[i] = sqrt((fNegPedSum2[i] - 2.*fNegPed[i]*fNegPedSum[i])/TMath::Max(1, fNegPedCount[i]) + fNegPed[i]*fNegPed[i]);
    //  fNegThresh[i] = fNegPed[i] + 15;
    fNegThresh[i] = fNegPed[i] + TMath::Min(50., TMath::Max(10., 3.*fNegSig[i]));

    //    cout << i+1 << " " << 3.*fPosSig[i] << " " << 3.*fNegSig[i] << endl;
  }
  //  cout << " " << endl;
  
}

//_____________________________________________________________________________
void THcShowerPlane::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum = new Int_t [fNelem];
  fNegPedSum2 = new Int_t [fNelem];
  fNegPedCount = new Int_t [fNelem];

  fPosSig = new Double_t [fNelem];
  fNegSig = new Double_t [fNelem];
  fPosPed = new Double_t [fNelem];
  fNegPed = new Double_t [fNelem];
  fPosThresh = new Double_t [fNelem];
  fNegThresh = new Double_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedCount[i] = 0;
  }
} 
