///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// THcCherenkov                                                                      //
//                                                                                   //
// Class for an Cherenkov detector consisting of onw pair of PMT's                   //
//                                                                                   //
// Zafar Ahmed. First attempt. September 27 2013.                                    //
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
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  //  fNegTDCHits = new TClonesArray("THcSignalHit",16); // check
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  //  fNegADCHits = new TClonesArray("THcSignalHit",16); // check

//  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
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
  delete [] fA_Pos;
  delete [] fA_Pos_1;
  //  delete [] fA_Neg;                                   // check
  delete [] fA_Pos_p;
  //  delete [] fA_Neg_p;                                 // check
  delete [] fT_Pos;
  //  delete [] fT_Neg;                                   // check
  delete [] fPosGain;
  //  delete [] fNegGain;                                 // check
  delete [] fPosPedLimit;
  //  delete [] fNegPedLimit;                             // check
  delete [] fPosPedMean;
  //  delete [] fNegPedMean;                              // check
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

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  // DBRequest listextra[]={
  //   {"cher_num_pairs", &fNelem, kInt},
  //   {0}
  // };
  fNelem = 2;			// Ahmed. One pair of PMTs for Cherenkov detctor
  // gHcParms->LoadParmValues((DBRequest*)&listextra,prefix);

  fA_Pos = new Float_t[fNelem];
  fA_Pos_1 = new Float_t[fNelem];
  //  fA_Neg = new Float_t[fNelem];                  // check
  fA_Pos_p = new Float_t[fNelem];
  //  fA_Neg_p = new Float_t[fNelem];                // check
  fT_Pos = new Float_t[fNelem]; 
  //  fT_Neg = new Float_t[fNelem];                  // check

  fPosGain = new Double_t[fNelem];
  //  fNegGain = new Double_t[fNelem];               // check
  fPosPedLimit = new Int_t[fNelem];
  //  fNegPedLimit = new Int_t[fNelem];              // check
  fPosPedMean = new Double_t[fNelem];
  //  fNegPedMean = new Double_t[fNelem];            // check

  DBRequest list[]={
    {"cer_adc_to_npe", fPosGain, kDouble},              // Ahmed
    {"cer_ped_limit", fPosPedLimit, kInt},             // Ahmed
    //    {"cher_pos_gain", fPosGain, kDouble},         // Ahmed
    //    {"cher_neg_gain", fNegGain, kDouble},         // Ahmed check
    //    {"cher_pos_ped_limit", fPosPedLimit, kInt},         // Ahmed
    //    {"cher_neg_ped_limit", fNegPedLimit, kInt},         // Ahmed check
    //    {"aero_pos_ped_mean", fPosPedMean, kDouble},
    //    {"aero_neg_ped_mean", fNegPedMean, kDouble},
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
    {"postdchits", "List of Positive TDC hits","fPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    //    {"negtdchits", "List of Negative TDC hits","fNegTDCHits.THcSignalHit.GetPaddleNumber()"}, // check
    {"posadchits", "List of Positive ADC hits","fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    //    {"negadchits", "List of Negative ADC hits","fNegADCHits.THcSignalHit.GetPaddleNumber()"}, // check
    {"apos",  "Raw Positive ADC Amplitudes",   "fA_Pos"},
    {"adc_1",  "Raw First ADC Amplitudes",   "fA_Pos_1"},
    //    {"aneg",  "Raw Negative ADC Amplitudes",   "fA_Neg"}, // check
    {"apos_p",  "Ped-subtracted Positive ADC Amplitudes",   "fA_Pos_p"},
    //    {"aneg_p",  "Ped-subtracted Negative ADC Amplitudes",   "fA_Neg_p"}, // check
    {"tpos",  "Raw Positive TDC",   "fT_Pos"},
    //    {"tneg",  "Raw Negative TDC",   "fT_Neg"}, // check
    {"pos_npe","PEs Positive Tube","fPosNpe"},
    //    {"neg_npe","PEs Negative Tube","fNegNpe"}, // check
    {"pos_npe_sum", "Total Positive Tube PEs", "fPosNpeSum"},
    //    {"neg_npe_sum", "Total Negative Tube PEs", "fNegNpeSum"}, // check
    {"npe_sum", "Total PEs", "fNpeSum"},
    {"ntdc_pos_hits", "Number of Positive Tube Hits", "fNTDCPosHits"},
    //    {"ntdc_neg_hits", "Number of Negative Tube Hits", "fNTDCNegHits"}, // check
    {"ngood_hits", "Total number of good hits", "fNGoodHits"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}
//_____________________________________________________________________________
inline 
void THcCherenkov::Clear(Option_t* opt)
{
  // Clear the hit lists
  fPosTDCHits->Clear();
  //  fNegTDCHits->Clear(); // check
  fPosADCHits->Clear();
  //  fNegADCHits->Clear(); // check

  // Clear Cherenkov variables  from h_trans_cer.f
  
  fNhits = 0;	     // Don't really need to do this.  (Be sure this called before Decode)

  fPosNpeSum = 0.0;
  //  fNegNpeSum = 0.0; // check
  fNpeSum = 0.0;
 
  fNGoodHits = 0;
    
  fNADCPosHits = 0;
  //  fNADCNegHits = 0; // check
  fNTDCPosHits = 0;
  //  fNTDCNegHits = 0; // check

  for(Int_t itube = 0;itube < fNelem;itube++) {
    fA_Pos[itube] = 0;
    fA_Pos_1[itube] = 0;
    //    fA_Neg[itube] = 0; // check
    fA_Pos_p[itube] = 0;
    //    fA_Neg_p[itube] = 0; // check
    fT_Pos[itube] = 0;
    //    fT_Neg[itube] = 0; // check
    fPosNpe[itube] = 0.0;
    //    fNegNpe[itube] = 0.0; // check
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
  Int_t nPosTDCHits=0;
  //  Int_t nNegTDCHits=0; // check
  Int_t nPosADCHits=0;
  //  Int_t nNegADCHits=0; // check
  while(ihit < fNhits) {
    THcCherenkovHit* hit = (THcCherenkovHit *) fRawHitList->At(ihit);
    
   // TDC positive hit
    if(hit->fTDC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(hit->fCounter, hit->fTDC_pos);
    }

    // TDC negative hit  // check
    // if(hit->fTDC_neg >  0) {
    //   THcSignalHit *sighit = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
    //   sighit->Set(hit->fCounter, hit->fTDC_neg);
    // }

    // ADC positive hit
    if(hit->fADC_pos >  0) {
      THcSignalHit *sighit = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit->Set(hit->fCounter, hit->fADC_pos);
    }

    // ADC negative hit  // check
    // if(hit->fADC_neg >  0) {   
    //   THcSignalHit *sighit = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
    //   sighit->Set(hit->fCounter, hit->fADC_neg);
    // }

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
     Conversion of h_trans_cer.f code:

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

    cout << "fNhits            = " << fNhits << endl;
    cout << "ihit              = " << ihit << endl;
    cout << "hit->fCounter     = " << hit->fCounter << endl;
    cout << "hit->fADC_pos     = " << hit->fADC_pos << endl;
    cout << "fPosPedMean[npmt] = " << fPosPedMean[0] << endl;
    if ( ihit == 1 )
      cout << "--------------------------" << endl;

    fA_Pos[npmt] = hit->fADC_pos;
    //    fA_Neg[npmt] = hit->fADC_neg;  // check
    fA_Pos_p[npmt] = hit->fADC_pos - fPosPedMean[npmt];         // hcer_adc(tube) = hcer_raw_adc(nhit) - hcer_ped(tube)
    //    fA_Neg_p[npmt] = hit->fADC_neg - fNegPedMean[npmt];         // hcer_adc(tube) = hcer_raw_adc(nhit) - hcer_ped(tube)  // check
    fT_Pos[npmt] = hit->fTDC_pos;
    //    fT_Neg[npmt] = hit->fTDC_neg; // check

    if( ( fA_Pos_p[npmt] > 50 ) && ( hit->fADC_pos < 8000 ) ) { // if (hcer_adc(tube) .gt. hcer_width(tube)) then
      fNADCPosHits++;                                           // hcer_num_hits = hcer_num_hits + 1
      fNGoodHits++;
      fPosNpe[npmt] = fPosGain[npmt]*fA_Pos_p[npmt];            // hcer_npe(tube) = hcer_adc(tube) * hcer_adc_to_npe(tube)
    } else if ( hit->fADC_pos > 8000 ) {
      fPosNpe[npmt] = 100.0;
    } else {
      fPosNpe[npmt] = 0.0;
    }
    
    if ( ihit == 0 )
      {

	fA_Pos_1[npmt] = hit->fADC_pos;

      }

     // check
    // if( ( fA_Neg_p[npmt] > 50 ) && ( hit->fADC_neg < 8000 ) ) { // if (hcer_adc(tube) .gt. hcer_width(tube)) then
    //   fNADCNegHits++;                                           // hcer_num_hits = hcer_num_hits + 1
    //   fNGoodHits++;
    //   fNegNpe[npmt] = fNegGain[npmt]*fA_Neg_p[npmt];            // hcer_npe(tube) = hcer_adc(tube) * hcer_adc_to_npe(tube)
    // } else if ( hit->fADC_neg > 8000 ) { 
    //   fNegNpe[npmt] = 100.0;
    // } else {
    //   fNegNpe[npmt] = 0.0;
    // }

    fPosNpeSum += fPosNpe[npmt];                                // hcer_npe_sum = hcer_npe_sum + hcer_npe(tube)
    //    fNegNpeSum += fNegNpe[npmt];                                // hcer_npe_sum = hcer_npe_sum + hcer_npe(tube) // check

    // Sum positive and negative hits to fill tot_good_hits
    //    if(fPosNpe[npmt] > 0.3) {
    //      fNADCPosHits++;
    //      fNGoodHits++;
    //    }
    //    if(fNegNpe[npmt] > 0.3) {
    //      fNADCNegHits++;
    //      fNGoodHits++;
    //    }

    if(hit->fTDC_pos > 0 && hit->fTDC_pos < 8000) {
      fNTDCPosHits++;
    }
     // check
    // if(hit->fTDC_neg > 0 && hit->fTDC_neg < 8000) {
    //   fNTDCNegHits++;
    // }      

    // Fill raw adc variables with actual tubve values
    // mainly for diagnostic purposes
    
  }
      
  // check
  // if(fPosNpeSum > 0.5 || fNegNpeSum > 0.5) {
  //   fNpeSum = fPosNpeSum + fNegNpeSum;
  // } else {
  //   fNpeSum = 0.0;
  // }

  // Ahmed
  if(fPosNpeSum > 0.5 ) {
    fNpeSum = fPosNpeSum;
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
Int_t THcCherenkov::FineProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
void THcCherenkov::InitializePedestals( )
{
  fNPedestalEvents = 0;
  fMinPeds = 500; 		// In engine, this is set in parameter file
  fPosPedSum = new Int_t [fNelem];
  fPosPedSum2 = new Int_t [fNelem];
  fPosPedLimit = new Int_t [fNelem];
  fPosPedCount = new Int_t [fNelem];
  // fNegPedSum = new Int_t [fNelem]; // check
  // fNegPedSum2 = new Int_t [fNelem]; // check
  // fNegPedLimit = new Int_t [fNelem]; // check
  // fNegPedCount = new Int_t [fNelem]; // check

  fPosPed = new Double_t [fNelem];
  //  fNegPed = new Double_t [fNelem]; // check
  fPosThresh = new Double_t [fNelem];
  //  fNegThresh = new Double_t [fNelem]; // check
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedLimit[i] = 1000;	// In engine, this are set in parameter file
    fPosPedCount[i] = 0;
    // fNegPedSum[i] = 0; // check
    // fNegPedSum2[i] = 0; // check
    // fNegPedLimit[i] = 1000; // check	// In engine, this are set in parameter file
    // fNegPedCount[i] = 0; // check
  }

  fPosNpe = new Double_t [fNelem];
  //  fNegNpe = new Double_t [fNelem]; // check
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
    Int_t adcpos = hit->fADC_pos;
    //    Int_t adcneg = hit->fADC_neg; // check
    if(adcpos <= fPosPedLimit[element]) {
      fPosPedSum[element] += adcpos;
      fPosPedSum2[element] += adcpos*adcpos;
      fPosPedCount[element]++;
      if(fPosPedCount[element] == fMinPeds/5) {
	fPosPedLimit[element] = 100 + fPosPedSum[element]/fPosPedCount[element];
      }
    }
    // check
    // if(adcneg <= fNegPedLimit[element]) {
    //   fNegPedSum[element] += adcneg;
    //   fNegPedSum2[element] += adcneg*adcneg;
    //   fNegPedCount[element]++;
    //   if(fNegPedCount[element] == fMinPeds/5) {
    // 	fNegPedLimit[element] = 100 + fNegPedSum[element]/fNegPedCount[element];
    //   }
    // }
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
    
    // Positive tubes
    fPosPed[i] = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosThresh[i] = fPosPed[i] + 15;

    // check    // Negative tubes
    // fNegPed[i] = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    // fNegThresh[i] = fNegPed[i] + 15;

    //    cout << i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;

    // Just a copy for now, but allow the possibility that fXXXPedMean is set
    // in a parameter file and only overwritten if there is a sufficient number of
    // pedestal events.  (So that pedestals are sensible even if the pedestal events were
    // not acquired.)
    if(fMinPeds > 0) {
      if(fPosPedCount[i] > fMinPeds) {
	fPosPedMean[i] = fPosPed[i];
      }
      // check
      // if(fNegPedCount[i] > fMinPeds) {
      // 	fNegPedMean[i] = fNegPed[i];
      // }
    }
  }
  //  cout << " " << endl;
  
}
void THcCherenkov::Print( const Option_t* opt) const {
  THaNonTrackingDetector::Print(opt);

  // Print out the pedestals

  cout << endl;
  cout << "Cherenkov Pedestals" << endl;

  // check
  // cout << "No.   Neg    Pos" << endl;
  // for(Int_t i=0; i<fNelem; i++){
  //   cout << " " << i << "    " << fNegPed[i] << "    " << fPosPed[i] << endl;
  // }

  // Ahmed
  cout << "No.   Pos" << endl;
  for(Int_t i=0; i<fNelem; i++){
    cout << " " << i << "    " << fPosPed[i] << endl;
  }

  cout << endl;
}

ClassImp(THcCherenkov)
////////////////////////////////////////////////////////////////////////////////
 
