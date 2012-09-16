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
#if ROOT_VERSION_CODE < ROOT_VERSION(5,32,0)
  fPosADCHitsClass = fPosADCHits->GetClass();
  fNegADCHitsClass = fNegADCHits->GetClass();
#endif
  fPosADC1 = new TClonesArray("THcSignalHit",13);
  fPosADCHitsClass = fPosADC1->GetClass();

  fLayerNum = layernum;
}

//______________________________________________________________________________
THcShowerPlane::~THcShowerPlane()
{
  // Destructor
  delete fPosADCHits;
  delete fNegADCHits;
  delete fPosADC1;

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
fNelem = 13;
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

/*
char spos1[256];
char spos2[256];
char spos3[256];

  // Register variables in global list
  RVarDef vars[15];
  vars[0] = "posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()";

  vars[1] = "negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()";

  for(Int_t i=2;i<15;i++){
	sprintf(spos1,"posadc1%d",i-1);
	sprintf(spos2,"ADC%d hits",i-1);
	sprintf(spos3,"fPosADC1[%d].THcSignalHit.Get",i-1);
	vars[i] = spos1,spos2,spos3;
  }

*/
fA    = new Float_t[13];

char spos1[256];
char spos2[256];
char spos3[256];

sprintf(spos1,"posadc%d",1);
sprintf(spos2,"ADC%d hits",1);
sprintf(spos3,"fPosADC%d.THcSignalHit.GetPaddleNumber()",1);
Double_t *fa = new Double_t[4];
fa[0] = 0.0;
fa[1] = 1.0;
fa[2] = 2.0;
fa[3] = 3.0;

RVarDef vars[4];

vars[0].name = "posadchits";
vars[0].desc =  "List of Positive ADC hits"; 
vars[0].def =  "fPosADCHits.THcSignalHit.GetPaddleNumber()";

vars[1].name = "negadchits";
vars[1].desc =  "List of Negative ADC hits";
vars[1].def =  "fNegADCHits.THcSignalHit.GetPaddleNumber()";

vars[2].name = "posadc1";
vars[2].desc =  "ADC1 hits";
//vars[2].def = "fA";
vars[2].def =  "fPosADC1.THcSignalHit.GetPaddleNumber()";

// vars[2].name = spos1;
// vars[2].desc = spos2; 
// vars[2].def =  spos3;


vars[3].name = NULL;
vars[3].desc = NULL;
vars[3].def = NULL;


/*
  // Register variables in global list
  RVarDef vars[] = {
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadc1", "ADC1 hits", 
     "fPosADC1.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };
*/
  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  //cout << " Calling THcShowerPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosADCHits->Clear();
  fNegADCHits->Clear();
  fPosADC1->Clear();
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
  fPosADC1->Clear();

//-------Thresholds values are taken from ENGINE
double hcal_new_threshold_pos[52] = {
// 485.7,505.7,407.8,513.6,547.2,343.0,437.5,420.2,444.6,354.7,498.4,251.9,565.3,
// 430.5,485.1,286.7,388.4,425.9,452.1,486.8,456.3,285.9,381.8,250.9,261.6,424.7,
// 361.4,378.9,431.3,224.0,488.2,352.9,436.3,379.9,343.9,432.8,462.8,252.3,427.0,
// 399.9,296.9,378.6,367.4,514.9,564.8,561.5,475.8,390.1,433.9,334.4,407.4,516.2};

470.7+12.6, 490.7+24.1, 392.8+12.1, 498.6+10.9, 532.2+14.1, 328.0+14.5, 422.5+14.0, 405.2+12.5, 429.6+11.4, 339.7+12.9, 483.4+14.8, 236.9+11.5, 550.3+10.0,
415.5+12.5, 470.1+16.3, 271.7+11.7, 373.4+11.5, 410.9+10.7, 437.1+10.3, 471.8+10.0, 441.3+10.0, 270.9+11.0, 366.8+10.0, 235.9+12.5, 246.6+10.0, 409.7+11.6,
346.4+13.2, 363.9+11.1, 416.3+10.1, 209.0+10.0, 473.2+12.5, 337.9+14.4, 421.3+10.0, 364.9+10.1, 328.9+17.9, 417.8+11.6, 447.8+16.5, 237.3+11.5, 412.0+11.4,
349.9+50.0, 281.9+11.2, 363.6+18.1, 352.4+14.7, 499.9+14.7, 549.8+15.3, 546.5+14.6, 460.8+13.6, 375.1+13.4, 418.9+11.0, 319.4+14.1, 392.4+12.2, 501.2+16.0};


double hcal_new_threshold_neg[52] = {
520.8+10.0, 472.0+15.9, 450.6+15.1, 451.5+12.9, 523.0+14.0, 553.3+15.9, 573.0+17.7, 494.9+14.3, 493.8+16.2, 487.8+12.2, 436.5+16.1, 438.0+10.0, 573.1+13.5,
527.4+11.8, 465.0+13.7, 460.1+13.6, 390.8+12.7, 552.3+16.7, 623.3+13.1, 549.3+13.3, 632.9+10.0, 479.7+12.1, 481.9+10.3, 412.3+15.0, 410.4+11.8, 635.1+12.6};

CalADC1File = fopen("adc1_new.dat", "a");
//fprintf(CalADC1File, "%d\n", 1);
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

if(hit->fCounter == 1){
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
THcSignalHit *sighit1 = (THcSignalHit*) fPosADC1->ConstructedAt(nPosADCHits++);

#else
	TObject* obj = (*fPosADC1)[nPosADCHits++];
	R__ASSERT( obj );
        if(!obj->TestBit (TObject::kNotDeleted))
	fPosADCHitsClass->New(obj);
	THcSignalHit *sighit1 = (THcSignalHit*)obj;
#endif
//THcSignalHit *sighit1 = (THcSignalHit*) fA[1]->ConstructedAt(nPosADCHits++);
 sighit1->Set(1,(Int_t)(hit->fADC_pos - 470.7));
//fprintf(CalADC1File, "%d\n", hit->fADC_pos);
}

double thresh_pos = fPosThresh[hit->fCounter -1];
//double thresh_pos = hcal_new_threshold_pos[hit->fCounter + 13*(hit->fPlane -1) -1];
if(hit->fADC_pos >  thresh_pos) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);

#else
	TObject* obj = (*fPosADCHits)[nPosADCHits++];
	R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
	fPosADCHitsClass->New(obj);
	THcSignalHit *sighit = (THcSignalHit*)obj;
#endif

   sighit->Set(hit->fCounter, hit->fADC_pos);
}

double thresh_neg = fNegThresh[hit->fCounter -1];
//double thresh_neg = hcal_new_threshold_neg[hit->fCounter + 13*(hit->fPlane -1) -1];
if(hit->fADC_neg >  thresh_neg) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
	sighit->Set(hit->fCounter, hit->fADC_neg);
#else
	TObject* obj = (*fNegADCHits)[nNegADCHits++];
	R__ASSERT( obj );
if(!obj->TestBit (TObject::kNotDeleted))
	fNegADCHitsClass->New(obj);
	THcSignalHit *sighit = (THcSignalHit*)obj;
#endif
   sighit->Set(hit->fCounter, hit->fADC_neg);
}

    ihit++;
  }
fclose(CalADC1File);
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
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedLimit = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  fNegPedSum = new Int_t [fNelem];
  fNegPedSum2 = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];
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
    fPosPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fNegPedCount[i] = 0;
  }
} 
  

