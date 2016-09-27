/** \class THcAerogel
    \ingroup Detectors

Class for an Aerogel detector consisting of pairs of PMT's
attached to a diffuser box
Will have a fixed number of pairs, but need to later make this
configurable.

*/

#include "THcAerogel.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THaApparatus.h"
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

//_____________________________________________________________________________
THcAerogel::THcAerogel( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);

  InitArrays();

//  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
}

//_____________________________________________________________________________
THcAerogel::THcAerogel( ) :
  THaNonTrackingDetector()
{
  // Constructor
  fPosTDCHits = NULL;
  fNegTDCHits = NULL;
  fPosADCHits = NULL;
  fNegADCHits = NULL;

  InitArrays();

}

//_____________________________________________________________________________
THcAerogel::~THcAerogel()
{
  // Destructor
  DeleteArrays();

  delete fPosTDCHits; fPosTDCHits = NULL;
  delete fNegTDCHits; fNegTDCHits = NULL;
  delete fPosADCHits; fPosADCHits = NULL;
  delete fNegADCHits; fNegADCHits = NULL;
}

//_____________________________________________________________________________
void THcAerogel::InitArrays()
{
  fA_Pos = NULL;
  fA_Neg = NULL;
  fA_Pos_p = NULL;
  fA_Neg_p = NULL;
  fT_Pos = NULL;
  fT_Neg = NULL;
  fPosGain = NULL;
  fNegGain = NULL;
  fPosPedLimit = NULL;
  fNegPedLimit = NULL;
  fPosPedMean = NULL;
  fNegPedMean = NULL;
  fPosNpe = NULL;
  fNegNpe = NULL;
  fPosPedSum = NULL;
  fPosPedSum2 = NULL;
  fPosPedCount = NULL;
  fNegPedSum = NULL;
  fNegPedSum2 = NULL;
  fNegPedCount = NULL;
  fPosPed = NULL;
  fPosSig = NULL;
  fPosThresh = NULL;
  fNegPed = NULL;
  fNegSig = NULL;
  fNegThresh = NULL;
}
//_____________________________________________________________________________
void THcAerogel::DeleteArrays()
{
  delete [] fA_Pos; fA_Pos = NULL;
  delete [] fA_Neg; fA_Neg = NULL;
  delete [] fA_Pos_p; fA_Pos_p = NULL;
  delete [] fA_Neg_p; fA_Neg_p = NULL;
  delete [] fT_Pos; fT_Pos = NULL;
  delete [] fT_Neg; fT_Neg = NULL;
  delete [] fPosGain; fPosGain = NULL;
  delete [] fNegGain; fNegGain = NULL;
  delete [] fPosPedLimit; fPosPedLimit = NULL;
  delete [] fNegPedLimit; fNegPedLimit = NULL;
  delete [] fPosPedMean; fPosPedMean = NULL;
  delete [] fNegPedMean; fNegPedMean = NULL;
  delete [] fPosNpe; fPosNpe = NULL;
  delete [] fNegNpe; fNegNpe = NULL;
  delete [] fPosPedSum; fPosPedSum = NULL;
  delete [] fPosPedSum2; fPosPedSum2 = NULL;
  delete [] fPosPedCount; fPosPedCount = NULL;
  delete [] fNegPedSum; fNegPedSum = NULL;
  delete [] fNegPedSum2; fNegPedSum2 = NULL;
  delete [] fNegPedCount; fNegPedCount = NULL;
  delete [] fPosPed; fPosPed = NULL;
  delete [] fPosSig; fPosSig = NULL;
  delete [] fPosThresh; fPosThresh = NULL;
  delete [] fNegPed; fNegPed = NULL;
  delete [] fNegSig; fNegSig = NULL;
  delete [] fNegThresh; fNegThresh = NULL;
}  

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcAerogel::Init( const TDatime& date )
{
  cout << "THcAerogel::Init " << GetName() << endl;

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  InitHitList(fDetMap, "THcAerogelHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HAERO") < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HAERO");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::ReadDatabase( const TDatime& date )
{
  // This function is called by THaDetectorBase::Init() once at the beginning
  // of the analysis.

  cout << "THcAerogel::ReadDatabase " << GetName() << endl;

  char prefix[2];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  fNelem = 8;			// Default if not defined
  Bool_t optional=true ;
  DBRequest listextra[]={
    {"aero_num_pairs", &fNelem, kInt,0,optional},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&listextra,prefix);

  fA_Pos = new Float_t[fNelem];
  fA_Neg = new Float_t[fNelem];
  fA_Pos_p = new Float_t[fNelem];
  fA_Neg_p = new Float_t[fNelem];
  fT_Pos = new Float_t[fNelem];
  fT_Neg = new Float_t[fNelem];

  fPosGain = new Double_t[fNelem];
  fNegGain = new Double_t[fNelem];
  fPosPedLimit = new Int_t[fNelem];
  fNegPedLimit = new Int_t[fNelem];
  fPosPedMean = new Double_t[fNelem];
  fNegPedMean = new Double_t[fNelem];

  DBRequest list[]={
    {"aero_pos_gain", fPosGain, kDouble, (UInt_t) fNelem},
    {"aero_neg_gain", fNegGain, kDouble, (UInt_t) fNelem},
    {"aero_pos_ped_limit", fPosPedLimit, kInt, (UInt_t) fNelem},
    {"aero_neg_ped_limit", fNegPedLimit, kInt, (UInt_t) fNelem},
    {"aero_pos_ped_mean", fPosPedMean, kDouble, (UInt_t) fNelem,optional},
    {"aero_neg_ped_mean", fNegPedMean, kDouble, (UInt_t) fNelem,optional},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  fIsInit = true;

  // Create arrays to hold pedestal results
  InitializePedestals();

  return kOK;
}

//_____________________________________________________________________________
Int_t THcAerogel::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcAerogel::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );
  
  // Register variables in global list

  // Do we need to put the number of pos/neg TDC/ADC hits into the variables?
  // No.  They show up in tree as Ndata.H.aero.postdchits for example

  RVarDef vars[] = {
    {"postdchits", "List of Positive TDC hits", 
     "fPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdchits", "List of Negative TDC hits", 
     "fNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadchits", "List of Positive ADC hits", 
     "fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"apos",  "Raw Positive ADC Amplitudes",   "fA_Pos"},
    {"aneg",  "Raw Negative ADC Amplitudes",   "fA_Neg"},
    {"apos_p",  "Ped-subtracted Positive ADC Amplitudes",   "fA_Pos_p"},
    {"aneg_p",  "Ped-subtracted Negative ADC Amplitudes",   "fA_Neg_p"},
    {"tpos",  "Raw Positive TDC",   "fT_Pos"},
    {"tneg",  "Raw Negative TDC",   "fT_Neg"},
    {"pos_npe","PEs Positive Tube","fPosNpe"},
    {"neg_npe","PEs Negative Tube","fNegNpe"},
    {"pos_npe_sum", "Total Positive Tube PEs", "fPosNpeSum"},
    {"neg_npe_sum", "Total Negative Tube PEs", "fNegNpeSum"},
    {"npe_sum", "Total PEs", "fNpeSum"},
    {"ntdc_pos_hits", "Number of Positive Tube Hits", "fNTDCPosHits"},
    {"ntdc_neg_hits", "Number of Negative Tube Hits", "fNTDCNegHits"},
    {"ngood_hits", "Total number of good hits", "fNGoodHits"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}
//_____________________________________________________________________________
inline 
void THcAerogel::Clear(Option_t* opt)
{
  // Clear the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  // Clear Aerogel variables  from h_aero.f
  
  fNhits = 0;	     // Don't really need to do this.  (Be sure this called before Decode)

  fPosNpeSum = 0.0;
  fNegNpeSum = 0.0;
  fNpeSum = 0.0;
 
  fNGoodHits = 0;
    
  fNADCPosHits = 0;
  fNADCNegHits = 0;
  fNTDCPosHits = 0;
  fNTDCNegHits = 0;

  for(Int_t itube = 0;itube < fNelem;itube++) {
    fA_Pos[itube] = 0;
    fA_Neg[itube] = 0;
    fA_Pos_p[itube] = 0;
    fA_Neg_p[itube] = 0;
    fT_Pos[itube] = 0;
    fT_Neg[itube] = 0;
    fPosNpe[itube] = 0.0;
    fNegNpe[itube] = 0.0;
  }

}

//_____________________________________________________________________________
Int_t THcAerogel::Decode( const THaEvData& evdata )
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
  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  while(ihit < fNhits) {
    THcAerogelHit* hit = (THcAerogelHit *) fRawHitList->At(ihit);
    
   // TDC positive hit
    if(hit->fNRawHits[2] >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(hit->fCounter, hit->GetTDCPos());
    }

    // TDC negative hit
    if(hit->fNRawHits[3] >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
      sighit->Set(hit->fCounter, hit->GetTDCNeg());
    }

    // ADC positive hit
    if(hit->fADC_pos[0] >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit->Set(hit->fCounter, hit->GetADCPos());
    }

    // ADC negative hit
    if(hit->fADC_neg[1] >  0) {   
      THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
      sighit->Set(hit->fCounter, hit->GetADCNeg());
    }

    ihit++;
  }
  return ihit;
}

//_____________________________________________________________________________
Int_t THcAerogel::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Int_t THcAerogel::CoarseProcess( TClonesArray&  ) //tracks
{
  
  for(Int_t ihit=0; ihit < fNhits; ihit++) {
    THcAerogelHit* hit = (THcAerogelHit *) fRawHitList->At(ihit);

    // Pedestal subtraction and gain adjustment

    // An ADC value of less than zero occurs when that particular
    // channel has been sparsified away and has not been read. 
    // The NPE for that tube will be assigned zero by this code.
    // An ADC value of greater than 8192 occurs when the ADC overflows on
    // an input that is too large. Tubes with this characteristic will
    // be assigned NPE = 100.0.

    Int_t npmt = hit->fCounter - 1;
    // Should probably check that npmt is in range
    fA_Pos[npmt] = hit->GetADCPos();
    fA_Neg[npmt] = hit->GetADCNeg();
    fA_Pos_p[npmt] = hit->GetADCPos() - fPosPedMean[npmt];
    fA_Neg_p[npmt] = hit->GetADCNeg() - fNegPedMean[npmt];
    fT_Pos[npmt] = hit->GetTDCPos();
    fT_Neg[npmt] = hit->GetTDCNeg();

    if(fA_Pos[npmt] < 8000) {
      fPosNpe[npmt] = fPosGain[npmt]*fA_Pos_p[npmt];
    } else {
      fPosNpe[npmt] = 100.0;
    }

    if(fA_Neg[npmt] < 8000) {
      fNegNpe[npmt] = fNegGain[npmt]*fA_Neg_p[npmt];
    } else {
      fNegNpe[npmt] = 100.0;
    }

    fPosNpeSum += fPosNpe[npmt];
    fNegNpeSum += fNegNpe[npmt];

    // Sum positive and negative hits to fill tot_good_hits
    if(fPosNpe[npmt] > 0.3) {
      fNADCPosHits++;
      fNGoodHits++;
    }
    if(fNegNpe[npmt] > 0.3) {
      fNADCNegHits++;
      fNGoodHits++;
    }
    if(fT_Pos[npmt] > 0 && fT_Pos[npmt] < 8000) {
      fNTDCPosHits++;
    }
    if(fT_Neg[npmt] > 0 && fT_Neg[npmt] < 8000) {
      fNTDCNegHits++;
    }      

    // Fill raw adc variables with actual tubve values
    // mainly for diagnostic purposes
    


  }
      
  if(fPosNpeSum > 0.5 || fNegNpeSum > 0.5) {
    fNpeSum = fPosNpeSum + fNegNpeSum;
  } else {
    fNpeSum = 0.0;
  }

  // If total hits are 0, then give a noticable ridiculous NPE
  if(fNhits < 1) {
    fNpeSum = 0.0;
  }

  // The following code is in the fortran.  It probably doesn't work
  // right because the arrays are not cleared first and the aero_ep,
  // aero_en, ... lines make no sense.

  //* Next, fill the rawadc variables with the actual tube values
  //*       mainly for diagnostic purposes.
  //
  //      do ihit=1,haero_tot_hits
  //
  //         npmt=haero_pair_num(ihit)
  //
  //         haero_rawadc_pos(npmt)=haero_adc_pos(ihit)
  //         aero_ep(npmt)=haero_rawadc_pos(ihit)        
  //
  //         haero_rawadc_neg(npmt)=haero_adc_neg(ihit)
  //         aero_en(npmt)=haero_rawadc_neg(ihit)
  //
  //         haero_rawtdc_neg(npmt)=haero_tdc_neg(ihit)
  //         aero_tn(npmt)= haero_tdc_neg(ihit)
  //
  //         haero_rawtdc_pos(npmt)=haero_tdc_pos(ihit)
  //         aero_tp(npmt)= haero_tdc_pos(ihit)
  //
  //      enddo

  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcAerogel::FineProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
void THcAerogel::InitializePedestals( )
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

  fPosNpe = new Double_t [fNelem];
  fNegNpe = new Double_t [fNelem];
}

//_____________________________________________________________________________
void THcAerogel::AccumulatePedestals(TClonesArray* rawhits)
{
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = 0;
  while(ihit < nrawhits) {
    THcAerogelHit* hit = (THcAerogelHit *) rawhits->At(ihit);

    Int_t element = hit->fCounter - 1;
    Int_t adcpos = hit->GetADCPos();
    Int_t adcneg = hit->GetADCNeg();
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

  return;
}

//_____________________________________________________________________________
void THcAerogel::CalculatePedestals( )
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

    // Just a copy for now, but allow the possibility that fXXXPedMean is set
    // in a parameter file and only overwritten if there is a sufficient number of
    // pedestal events.  (So that pedestals are sensible even if the pedestal events were
    // not acquired.)
    if(fMinPeds > 0) {
      if(fPosPedCount[i] > fMinPeds) {
	fPosPedMean[i] = fPosPed[i];
      }
      if(fNegPedCount[i] > fMinPeds) {
	fNegPedMean[i] = fNegPed[i];
      }
    }
  }
  //  cout << " " << endl;
  
}
void THcAerogel::Print( const Option_t* opt) const {
  THaNonTrackingDetector::Print(opt);

  // Print out the pedestals

  cout << endl;
  cout << "Aerogel Pedestals" << endl;
  cout << "No.   Neg    Pos" << endl;
  for(Int_t i=0; i<fNelem; i++){
    cout << " " << i << "    " << fNegPed[i] << "    " << fPosPed[i] << endl;
  }
  cout << endl;
}

ClassImp(THcAerogel)
////////////////////////////////////////////////////////////////////////////////
 
