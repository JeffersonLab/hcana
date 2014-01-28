///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// THcCherenkov                                                                      //
//                                                                                   //
// Class for an Cherenkov detector consisting of onw pair of PMT's                   //
//                                                                                   //
// Zafar Ahmed. Second attempt. November 14 2013.                                    //
// Comment:No need to cahnge the Map file but may need to change the parameter file  //
//                                                                                   //
// This code is written for following variables:                                     //
//                                                                                   //
// Variable Name     Description                                                     //
// adc_1             Raw value of adc 1                                              //
// adc_2             Raw value of adc 2                                              //
// adc_p_1           Pedestal substracted value of adc 1                             //
// adc_p_2           Pedestal substracted value of adc 2                             //
// npe_1             Number of photo electrons of adc 1                              //
// npe_2             Number of photo electrons of adc 2                              //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////

#include "THcCherenkov.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

using std::cout;
using std::cin;
using std::endl;

#include <iomanip>
using std::setw;
using std::setprecision;

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  fADCHits = new TClonesArray("THcSignalHit",16);

}

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
THcCherenkov::~THcCherenkov()
{
  // Destructor
  delete [] fNPMT;
  delete [] fADC;
  delete [] fADC_P;
  delete [] fNPE;

  delete [] fCerWidth;
  delete [] fGain;
  delete [] fPedLimit;
  delete [] fPedMean;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCherenkov::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  cout << "THcCherenkov::Init " << GetName() << endl;

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  THcHitList::InitHitList(fDetMap, "THcCherenkovHit", 100); // 100 is max hits

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HCER") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HCER");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  cout << "THcCherenkov::ReadDatabase " << GetName() << endl; // Ahmed

  char prefix[2];
  char parname[100];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  strcpy(parname,prefix);                              // This is taken from 
  strcat(parname,"cer_tot_pmts");                      // THcScintillatorPlane
  fNelem = (Int_t)gHcParms->Find(parname)->GetValue(); // class.

  //    fNelem = 2;      // Default if not defined                                                                    

  fNPMT = new Int_t[fNelem];
  fADC = new Double_t[fNelem];
  fADC_P = new Double_t[fNelem];
  fNPE = new Double_t[fNelem];

  fCerWidth = new Double_t[fNelem];
  fGain = new Double_t[fNelem];
  fPedLimit = new Int_t[fNelem];
  fPedMean = new Double_t[fNelem];
  
  DBRequest list[]={
    {"cer_adc_to_npe", fGain,     kDouble, fNelem},              // Ahmed
    {"cer_ped_limit",  fPedLimit, kInt,    fNelem},              // Ahmed
    {"cer_width",      fCerWidth, kDouble, fNelem},              // Ahmed
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  fIsInit = true;

  // Create arrays to hold pedestal results
  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcCherenkov::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcCherenkov::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );
  
  // Register variables in global list

  // Do we need to put the number of pos/neg TDC/ADC hits into the variables?
  // No.  They show up in tree as Ndata.H.aero.postdchits for example

  RVarDef vars[] = {
    {"PhotoTubes",  "Nuber of Cherenkov photo tubes",            "fNPMT"},
    {"ADC",         "Raw ADC values",                            "fADC"},
    {"ADC_P",       "Pedestal Subtracted ADC values",            "fADC_P"},
    {"NPE",         "Number of Photo electrons",                 "fNPE"},
    {"NPEsum",      "Sum of Number of Photo electrons",          "fNPEsum"},
    {"NCherHit",    "Number of Hits(Cherenkov)",                 "fNCherHit"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}
//_____________________________________________________________________________
inline 
void THcCherenkov::Clear(Option_t* opt)
{
  // Clear the hit lists
  fADCHits->Clear();

  // Clear Cherenkov variables  from h_trans_cer.f
  
  fNhits = 0;	     // Don't really need to do this.  (Be sure this called before Decode)
  fNPEsum = 0;
  fNCherHit = 0;

  for(Int_t itube = 0;itube < fNelem;itube++) {
    fNPMT[itube] = 0;
    fADC[itube] = 0;
    fADC_P[itube] = 0;
    fNPE[itube] = 0;
  }

}

//_____________________________________________________________________________
Int_t THcCherenkov::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  fNhits = THcHitList::DecodeToHitList(evdata);

  if(gHaCuts->Result("Pedestal_event")) {
    AccumulatePedestals(fRawHitList);
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
    CalculatePedestals();
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }


  Int_t ihit = 0;
  Int_t nADCHits=0;
  while(ihit < fNhits) {
    THcCherenkovHit* hit = (THcCherenkovHit *) fRawHitList->At(ihit);
    
    // ADC hit
    if(hit->fADC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fADCHits->ConstructedAt(nADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_pos);
    }

    ihit++;
  }
  return ihit;
}

//_____________________________________________________________________________
Int_t THcCherenkov::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcCherenkov::CoarseProcess( TClonesArray&  ) //tracks
{
  /* 
     ------------------------------------------------------------------------------------------------------------------
     h_trans_cer.f code:

      hcer_num_hits = 0                                      <--- clear event
      do tube=1,hcer_num_mirrors                             
        hcer_npe(tube) = 0.                                  <--- clear event
        hcer_adc(tube) = 0.                                  <--- clear event
      enddo
      hcer_npe_sum = 0.                                      <--- clear event
      do nhit = 1, hcer_tot_hits                             <--- loop over total hits. Very first line of this method
        tube = hcer_tube_num(nhit)                           <--- tube is number of PMT on either side and it is this 
	                                                          line:  Int_t npmt = hit->fCounter - 1
        hcer_adc(tube) = hcer_raw_adc(nhit) - hcer_ped(tube) <--- This is done above:
                                                                  fA_Pos_p[npmt] = hit->fADC_pos - fPosPedMean[npmt];
                                                                  fA_Neg_p[npmt] = hit->fADC_neg - fNegPedMean[npmt];
        if (hcer_adc(tube) .gt. hcer_width(tube)) then       <--- This needs to convert in hcana
          hcer_num_hits = hcer_num_hits + 1
          hcer_tube_num(hcer_num_hits) = tube
          hcer_npe(tube) = hcer_adc(tube) * hcer_adc_to_npe(tube)
          hcer_npe_sum = hcer_npe_sum + hcer_npe(tube)
        endif
      enddo
     ------------------------------------------------------------------------------------------------------------------
    */
  for(Int_t ihit=0; ihit < fNhits; ihit++) {
    THcCherenkovHit* hit = (THcCherenkovHit *) fRawHitList->At(ihit); // nhit = 1, hcer_tot_hits

    // Pedestal subtraction and gain adjustment

    // An ADC value of less than zero occurs when that particular
    // channel has been sparsified away and has not been read. 
    // The NPE for that tube will be assigned zero by this code.
    // An ADC value of greater than 8192 occurs when the ADC overflows on
    // an input that is too large. Tubes with this characteristic will
    // be assigned NPE = 100.0.
    Int_t npmt = hit->fCounter - 1;                             // tube = hcer_tube_num(nhit)
    // Should probably check that npmt is in range
    if ( ihit != npmt )
      cout << "ihit != npmt " << endl;

    fNPMT[npmt] = hit->fCounter;
    fADC[npmt] = hit->fADC_pos;
    fADC_P[npmt] = hit->fADC_pos - fPedMean[npmt];
    
    if ( ( fADC_P[npmt] > fCerWidth[npmt] ) && ( hit->fADC_pos < 8000 ) ) {
      fNPE[npmt] = fGain[npmt]*fADC_P[npmt];
      fNCherHit ++;
    } else if (  hit->fADC_pos > 8000 ) {
      fNPE[npmt] = 100.0;
    } else {
      fNPE[npmt] = 0.0;
    }
    
    fNPEsum += fNPE[npmt];

  }

  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcCherenkov::FineProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
void THcCherenkov::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPedSum = new Int_t [fNelem];
  fPedSum2 = new Int_t [fNelem];
  fPedCount = new Int_t [fNelem];

  fPed = new Double_t [fNelem];
  fThresh = new Double_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPedSum[i] = 0;
    fPedSum2[i] = 0;
    fPedCount[i] = 0;
  }

}

//_____________________________________________________________________________
void THcCherenkov::AccumulatePedestals(TClonesArray* rawhits)
{
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = 0;
  while(ihit < nrawhits) {
    THcCherenkovHit* hit = (THcCherenkovHit *) rawhits->At(ihit);

    Int_t element = hit->fCounter - 1;
    Int_t nadc = hit->fADC_pos;
    if(nadc <= fPedLimit[element]) {
      fPedSum[element] += nadc;
      fPedSum2[element] += nadc*nadc;
      fPedCount[element]++;
      if(fPedCount[element] == fMinPeds/5) {
	fPedLimit[element] = 100 + fPedSum[element]/fPedCount[element];
      }
    }
    ihit++;
  }

  fNPedestalEvents++;

  return;
}

//_____________________________________________________________________________
void THcCherenkov::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")
  //  cout << "Plane: " << fPlaneNum << endl;
  for(Int_t i=0; i<fNelem;i++) {
    
    // PMT tubes
    fPed[i] = ((Double_t) fPedSum[i]) / TMath::Max(1, fPedCount[i]);
    fThresh[i] = fPed[i] + 15;

    // Just a copy for now, but allow the possibility that fXXXPedMean is set
    // in a parameter file and only overwritten if there is a sufficient number of
    // pedestal events.  (So that pedestals are sensible even if the pedestal events were
    // not acquired.)
    if(fMinPeds > 0) {
      if(fPedCount[i] > fMinPeds) {
	fPedMean[i] = fPed[i];
      }
    }
  }
  //  cout << " " << endl;
  
}
void THcCherenkov::Print( const Option_t* opt) const {
  THaNonTrackingDetector::Print(opt);

  // Print out the pedestals

  cout << endl;
  cout << "Cherenkov Pedestals" << endl;

  // Ahmed
  cout << "No.   ADC" << endl;
  for(Int_t i=0; i<fNelem; i++){
    cout << " " << i << "    " << fPed[i] << endl;
  }

  cout << endl;
}

ClassImp(THcCherenkov)
////////////////////////////////////////////////////////////////////////////////
 
