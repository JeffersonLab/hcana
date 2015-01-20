///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// THcCherenkov                                                                      //
//                                                                                   //
// Class for an Cherenkov detector consisting of two PMT's                           //
//                                                                                   //
// Zafar Ahmed. Fourth pull request. January 30 2014.                                //
// Comment: No need to cahnge the Map file.                                          //
// Comment  New parameter hcer_tot_pmts = 2 is added to the hcana.param file         //
//                                                                                   //
// This code is for the coarse process of THcCherenkov class.                        //
//                                                                                   //
//                                                                                   //
// Variable Name   Description                                                       //
//                                                                                   //
// phototubes      Nuber of Cherenkov photo tubes                                    //
// adc             Raw ADC values                                                    //
// adc_p           Pedestal Subtracted ADC values                                    //
// npe             Number of Photo electrons                                         //
// npesum          Sum of Number of Photo electrons                                  //
// ncherhit        Number of Hits(Cherenkov)                                         //
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
  cout << "fADCHits " << fADCHits << endl;
  InitArrays();
  cout << "fADCHits " << fADCHits << endl;

}

//_____________________________________________________________________________
THcCherenkov::THcCherenkov( ) :
  THaNonTrackingDetector()
{
  // Constructor
  fADCHits = NULL;

  InitArrays();
}

//_____________________________________________________________________________
void THcCherenkov::InitArrays()
{
  fGain = NULL;
  fCerWidth = NULL;
  fNPMT = NULL;
  fADC = NULL;
  fADC_P = NULL;
  fNPE = NULL;
  fPedSum = NULL;
  fPedSum2 = NULL;
  fPedLimit = NULL;
  fPedMean = NULL;
  fPedCount = NULL;
  fPed = NULL;
  fThresh = NULL;
}
//_____________________________________________________________________________
void THcCherenkov::DeleteArrays()
{
  delete [] fGain; fGain = NULL;
  delete [] fCerWidth; fCerWidth = NULL;
  delete [] fNPMT; fNPMT = NULL;
  delete [] fADC; fADC = NULL;
  delete [] fADC; fADC_P = NULL;
  delete [] fNPE; fNPE = NULL;
  delete [] fPedSum; fPedSum = NULL;
  delete [] fPedSum2; fPedSum2 = NULL;
  delete [] fPedLimit; fPedLimit = NULL;
  delete [] fPedMean; fPedMean = NULL;
  delete [] fPedCount; fPedCount = NULL;
  delete [] fPed; fPed = NULL;
  delete [] fThresh; fThresh = NULL;
}
//_____________________________________________________________________________
THcCherenkov::~THcCherenkov()
{
  // Destructor
  delete fADCHits; fADCHits = NULL;

  DeleteArrays();

}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcCherenkov::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  cout << "THcCherenkov::Init " << GetName() << endl;

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  InitHitList(fDetMap, "THcCherenkovHit", 100); // 100 is max hits

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
  
  fNPMT = new Int_t[fNelem];
  fADC = new Double_t[fNelem];
  fADC_P = new Double_t[fNelem];
  fNPE = new Double_t[fNelem];
  fCerWidth = new Double_t[fNelem];
  fGain = new Double_t[fNelem];
  fPedLimit = new Int_t[fNelem];
  fPedMean = new Double_t[fNelem];
  
  fCerNRegions = 3; // This value should be in parameter file

  fCerTrackCounter = new Int_t [fCerNRegions];
  fCerFiredCounter = new Int_t [fCerNRegions];
  for ( Int_t ireg = 0; ireg < fCerNRegions; ireg++ ) {
    fCerTrackCounter[ireg] = 0;
    fCerFiredCounter[ireg] = 0;
  }


  fCerRegionsValueMax = fCerNRegions * 8; // This value 8 should also be in paramter file
  fCerRegionValue = new Double_t [fCerRegionsValueMax];

  DBRequest list[]={
    {"cer_adc_to_npe", fGain,     kDouble, (UInt_t) fNelem},              // Ahmed
    {"cer_ped_limit",  fPedLimit, kInt,    (UInt_t) fNelem},              // Ahmed
    {"cer_width",      fCerWidth, kDouble, (UInt_t) fNelem},              // Ahmed
    {"cer_chi2max",     &fCerChi2Max,        kDouble},                       // Ahmed
    {"cer_beta_min",    &fCerBetaMin,        kDouble},                       // Ahmed
    {"cer_beta_max",    &fCerBetaMax,        kDouble},                       // Ahmed
    {"cer_et_min",      &fCerETMin,          kDouble},                       // Ahmed
    {"cer_et_max",      &fCerETMax,          kDouble},                       // Ahmed
    {"cer_mirror_zpos", &fCerMirrorZPos,     kDouble},                       // Ahmed
    {"cer_region",      &fCerRegionValue[0], kDouble, (UInt_t) fCerRegionsValueMax},  // Ahmed
    {"cer_threshold",   &fCerThresh,         kDouble},                       // Ahmed
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  fIsInit = true;

  for (Int_t i1 = 0; i1 < fCerNRegions; i1++ ) {
    cout << "Region " << i1 << endl;
    for (Int_t i2 = 0; i2 < 8; i2++ ) {
      cout << fCerRegionValue[GetCerIndex( i1, i2 )] << " ";
    }
    cout <<endl;
  }

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
    {"phototubes",  "Nuber of Cherenkov photo tubes",            "fNPMT"},
    {"adc",         "Raw ADC values",                            "fADC"},
    {"adc_p",       "Pedestal Subtracted ADC values",            "fADC_P"},
    {"npe",         "Number of Photo electrons",                 "fNPE"},
    {"npesum",      "Sum of Number of Photo electrons",          "fNPEsum"},
    {"ncherhit",    "Number of Hits(Cherenkov)",                 "fNCherHit"},
    {"certrackcounter", "Tracks inside Cherenkov region",        "fCerTrackCounter"},
    {"cerfiredcounter", "Tracks with engough Cherenkov NPEs ",   "fCerFiredCounter"},
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
  fNhits = DecodeToHitList(evdata);

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

  Double_t fCerX, fCerY;
   
  if ( tracks.GetLast() > -1 ) {

    THaTrack* theTrack = dynamic_cast<THaTrack*>( tracks.At(0) );
    if (!theTrack) return -1;
    
    if ( ( ( tracks.GetLast() + 1 ) == 1 ) && 
	 ( theTrack->GetChi2()/theTrack->GetNDoF() > 0. ) && 
	 ( theTrack->GetChi2()/theTrack->GetNDoF() <  fCerChi2Max ) && 
	 ( theTrack->GetBeta() > fCerBetaMin ) &&
	 ( theTrack->GetBeta() < fCerBetaMax ) &&
	 ( ( theTrack->GetEnergy() / theTrack->GetP() ) > fCerETMin ) &&
	 ( ( theTrack->GetEnergy() / theTrack->GetP() ) < fCerETMax ) 
	 ) {
      
      fCerX = theTrack->GetX() + theTrack->GetTheta() * fCerMirrorZPos;
      fCerY = theTrack->GetY() + theTrack->GetPhi()   * fCerMirrorZPos;
      
      for ( Int_t ir = 0; ir < fCerNRegions; ir++ ) {
	
	//	*     hit must be inside the region in order to continue.   

	if ( ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 0 )] - fCerX ) < 
	       fCerRegionValue[GetCerIndex( ir, 4 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 1 )] - fCerY ) < 
	       fCerRegionValue[GetCerIndex( ir, 5 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 2 )] - theTrack->GetTheta() ) < 
	       fCerRegionValue[GetCerIndex( ir, 6 )] ) &&
	     ( TMath::Abs( fCerRegionValue[GetCerIndex( ir, 3 )] - theTrack->GetPhi() ) < 
	       fCerRegionValue[GetCerIndex( ir, 7 )] ) 
	     ) {
	
	  // *     increment the 'should have fired' counters 
	  fCerTrackCounter[ir] ++;	  
	  
	  // *     increment the 'did fire' counters

	  if ( fNPEsum > fCerThresh ) {
	    fCerFiredCounter[ir] ++;
	  }

	}
	
	// if ( fCerEvent > 5880 ) {
	// cout << "Event = " << fCerEvent 
	//      << "   region = " << ir + 1
	//      << "   track counter = " << fCerTrackCounter[ir]
	//      << "   fired coutner = " << fCerFiredCounter[ir]
	//      << endl;
	// }
	
      } // loop over regions
      //      cout << endl;
      
    }
    
  }

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

//_____________________________________________________________________________
Int_t THcCherenkov::GetCerIndex( Int_t nRegion, Int_t nValue ) {

  return fCerNRegions * nValue + nRegion;
}

//_____________________________________________________________________________
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
 
