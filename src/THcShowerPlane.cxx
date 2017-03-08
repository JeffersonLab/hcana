/** \class THcShowerPlane
    \group DetSupport

One plane of shower blocks with side readout

*/

#include "THcShowerPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
#include "math.h"
#include "THaTrack.h"
#include "THaTrackProj.h"

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
  fPosADCHits = new TClonesArray("THcSignalHit",fNelem);
  fNegADCHits = new TClonesArray("THcSignalHit",fNelem);

  frPosAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frPosAdcPed = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

  frNegAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frNegAdcPed = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

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

  frPosAdcPedRaw = NULL;
  frPosAdcPulseIntRaw = NULL;
  frPosAdcPulseAmpRaw = NULL;
  frPosAdcPulseTimeRaw = NULL;

  frPosAdcPed = NULL;
  frPosAdcPulseInt = NULL;
  frPosAdcPulseAmp = NULL;

  frNegAdcPedRaw = NULL;
  frNegAdcPulseIntRaw = NULL;
  frNegAdcPulseAmpRaw = NULL;
  frNegAdcPulseTimeRaw = NULL;

  frNegAdcPed = NULL;
  frNegAdcPulseInt = NULL;
  frNegAdcPulseAmp = NULL;

  delete [] fA_Pos;
  delete [] fA_Neg;
  delete [] fA_Pos_p;
  delete [] fA_Neg_p;

  delete [] fEpos;
  delete [] fEneg;
  delete [] fEmean;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShowerPlane::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

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

  // Retrieve FADC parameters.  In principle may want different dynamic
  // pedestal and integration range for preshower and shower, but for now
  // use same parameters
  char prefix[2];
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';
  fUsingFADC=0;
  fPedSampLow=0;
  fPedSampHigh=9;
  fDataSampLow=23;
  fDataSampHigh=49;
  DBRequest list[]={
    {"cal_using_fadc", &fUsingFADC, kInt, 0, 1},
    {"cal_ped_sample_low", &fPedSampLow, kInt, 0, 1},
    {"cal_ped_sample_high", &fPedSampHigh, kInt, 0, 1},
    {"cal_data_sample_low", &fDataSampLow, kInt, 0, 1},
    {"cal_data_sample_high", &fDataSampHigh, kInt, 0, 1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  // Retrieve more parameters we need from parent class
  //

  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  //  Find the number of elements
  fNelem = fParent->GetNBlocks(fLayerNum-1);

  // Origin of the plane:
  //
  // X is average of top X coordinates of the top and bottom blocks
  // shifted by a half of the block thickness;
  // Y is average of left and right edges;
  // Z is _front_ position of the plane along the beam.

  Double_t BlockThick = fParent->GetBlockThick(fLayerNum-1);

  Double_t xOrig = (fParent->GetXPos(fLayerNum-1,0) +
		    fParent->GetXPos(fLayerNum-1,fNelem-1))/2 +
    BlockThick/2;

  Double_t yOrig = (fParent->GetYPos(fLayerNum-1,0) +
		    fParent->GetYPos(fLayerNum-1,1))/2;

  Double_t zOrig = fParent->GetZPos(fLayerNum-1);

  fOrigin.SetXYZ(xOrig, yOrig, zOrig);

  // Create arrays to hold results here
  //

  // Pedestal limits per channel.

  fPosPedLimit = new Int_t [fNelem];
  fNegPedLimit = new Int_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPosPedLimit[i] = fParent->GetPedLimit(i,fLayerNum-1,0);
    fNegPedLimit[i] = fParent->GetPedLimit(i,fLayerNum-1,1);
  }

  fMinPeds = fParent->GetMinPeds();

  InitializePedestals();

  // ADC amplitudes per channel.

  fA_Pos = new Double_t[fNelem];
  fA_Neg = new Double_t[fNelem];
  fA_Pos_p = new Double_t[fNelem];
  fA_Neg_p = new Double_t[fNelem];

  // Energy depositions per block (not corrected for track coordinate)

  fEpos = new Double_t[fNelem];
  fEneg = new Double_t[fNelem];
  fEmean= new Double_t[fNelem];

  // Debug output.

  if (fParent->fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::ReadDatabase for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "  Layer #" << fLayerNum << ", number of elements " << dec << fNelem
	 << endl;

    cout << "  Origin of Layer at  X = " << fOrigin.X()
	 << "  Y = " << fOrigin.Y() << "  Z = " << fOrigin.Z() << endl;

    cout << "  fPosPedLimit:";
    for(Int_t i=0;i<fNelem;i++) cout << " " << fPosPedLimit[i];
    cout << endl;
    cout << "  fNegPedLimit:";
    for(Int_t i=0;i<fNelem;i++) cout << " " << fNegPedLimit[i];
    cout << endl;

    cout << "  fMinPeds = " << fMinPeds << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return kOK;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"posadchits", "List of Positive ADC hits","fPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits","fNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"apos",       "Raw Positive ADC Amplitudes",                   "fA_Pos"},
    {"aneg",       "Raw Negative ADC Amplitudes",                   "fA_Neg"},
    {"apos_p",     "Ped-subtracted Positive ADC Amplitudes",        "fA_Pos_p"},
    {"aneg_p",     "Ped-subtracted Negative ADC Amplitudes",        "fA_Neg_p"},
    {"epos",       "Energy Depositions from Positive Side PMTs",    "fEpos"},
    {"eneg",       "Energy Depositions from Negative Side PMTs",    "fEneg"},
    {"emean",      "Mean Energy Depositions",                       "fEmean"},
    {"eplane",     "Energy Deposition per plane",                   "fEplane"},
    {"eplane_pos", "Energy Deposition per plane from pos. PMTs","fEplane_pos"},
    {"eplane_neg", "Energy Deposition per plane from neg. PMTs","fEplane_neg"},

    {"posAdcCounter",      "List of positive ADC counter numbers.",      "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},
    {"negAdcCounter",      "List of negative ADC counter numbers.",      "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},

    {"posAdcPedRaw",       "List of positive raw ADC pedestals",         "frPosAdcPedRaw.THcSignalHit.GetData()"},
    {"posAdcPulseIntRaw",  "List of positive raw ADC pulse integrals.",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"},
    {"posAdcPulseAmpRaw",  "List of positive raw ADC pulse amplitudes.", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"},
    {"posAdcPulseTimeRaw", "List of positive raw ADC pulse times.",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"},

    {"posAdcPed",          "List of positive ADC pedestals",             "frPosAdcPed.THcSignalHit.GetData()"},
    {"posAdcPulseInt",     "List of positive ADC pulse integrals.",      "frPosAdcPulseInt.THcSignalHit.GetData()"},
    {"posAdcPulseAmp",     "List of positive ADC pulse amplitudes.",     "frPosAdcPulseAmp.THcSignalHit.GetData()"},

    {"negAdcPedRaw",       "List of negative raw ADC pedestals",         "frNegAdcPedRaw.THcSignalHit.GetData()"},
    {"negAdcPulseIntRaw",  "List of negative raw ADC pulse integrals.",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"},
    {"negAdcPulseAmpRaw",  "List of negative raw ADC pulse amplitudes.", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"},
    {"negAdcPulseTimeRaw", "List of negative raw ADC pulse times.",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"},

    {"negAdcPed",          "List of negative ADC pedestals",             "frNegAdcPed.THcSignalHit.GetData()"},
    {"negAdcPulseInt",     "List of negative ADC pulse integrals.",      "frNegAdcPulseInt.THcSignalHit.GetData()"},
    {"negAdcPulseAmp",     "List of negative ADC pulse amplitudes.",     "frNegAdcPulseAmp.THcSignalHit.GetData()"},

    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  // Clears the hit lists
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

  // Debug output.
  if ( ((THcShower*) GetParent())->fdbg_decoded_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::Clear for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << " Cleared ADC hits for plane " << GetName() << endl;
    cout << "---------------------------------------------------------------\n";
  }
}

//_____________________________________________________________________________
Int_t THcShowerPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead

  //Debug output.
  if ( ((THcShower*) GetParent())->fdbg_decoded_cal ) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::Decode for "
      	 << GetParent()->GetPrefix() << ":" << endl;

    cout << " Called for plane " << GetName() << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcess( TClonesArray& tracks )
{

  // Nothing is done here. See ProcessHits method instead.
  //

 return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list
  // Assumes that the hit list is sorted by layer, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  THcShower* fParent;
  fParent = (THcShower*) GetParent();

  // Initialize variables.

  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

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
  fEplane_pos = 0;
  fEplane_neg = 0;

  UInt_t nrPosAdcHits = 0;
  UInt_t nrNegAdcHits = 0;

  // Process raw hits. Get ADC hits for the plane, assign variables for each
  // channel.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // This is OK as far as the hit list is sorted by layer.
    //
    if(hit->fPlane > fLayerNum) {
      break;
    }

    Int_t padnum = hit->fCounter;

    THcRawAdcHit& rawPosAdcHit = hit->GetRawAdcHitPos();
    for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
      ((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPedRaw());
      ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPed());

      ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTimeRaw(thit));

      ++nrPosAdcHits;
    }
    THcRawAdcHit& rawNegAdcHit = hit->GetRawAdcHitNeg();
    for (UInt_t thit=0; thit<rawNegAdcHit.GetNPulses(); ++thit) {
      ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPedRaw());
      ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPed());

      ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTimeRaw(thit));

      ++nrNegAdcHits;
    }

    fADCMode=kADCDynamicPedestal;
    Double_t adc_pos;
    Double_t adc_neg;
    Double_t adc_pos_pedsub;
    Double_t adc_neg_pedsub;
    if(fADCMode == kADCDynamicPedestal) {
			adc_pos_pedsub = hit->GetRawAdcHitPos().GetPulseInt();
			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetPulseInt();
			adc_pos = hit->GetRawAdcHitPos().GetPulseIntRaw();
			adc_neg = hit->GetRawAdcHitNeg().GetPulseIntRaw();
    } else if (fADCMode == kADCSampleIntegral) {
			adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[hit->fCounter -1];
			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[hit->fCounter -1];
			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw();
			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw();
    } else if (fADCMode == kADCSampIntDynPed) {
      adc_pos = hit->GetRawAdcHitPos().GetSampleInt();
      adc_neg = hit->GetRawAdcHitNeg().GetSampleInt();
      adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw();
      adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw();
    } else {
      adc_pos_pedsub = hit->GetRawAdcHitPos().GetPulseIntRaw()-fPosPed[hit->fCounter -1];
      adc_neg_pedsub = hit->GetRawAdcHitNeg().GetPulseIntRaw()-fNegPed[hit->fCounter -1];
      adc_pos = hit->GetRawAdcHitPos().GetPulseIntRaw();
      adc_neg = hit->GetRawAdcHitNeg().GetPulseIntRaw();
    }
    //
    // Sparsify positive side hits, fill the hit list, compute the
    // energy depostion from positive side for the counter.
			fA_Pos[hit->fCounter-1] =adc_pos;
			fA_Neg[hit->fCounter-1] =adc_neg;

    Double_t thresh_pos = fPosThresh[hit->fCounter -1];
    thresh_pos =0;
    if(fA_Pos[hit->fCounter-1] >  thresh_pos) {

      fA_Pos_p[hit->fCounter-1] =adc_pos_pedsub ;

      //      cout << " pos " << hit->fCounter << " " << fA_Pos_p[hit->fCounter-1] << " " <<  fA_Pos[hit->fCounter-1] << " " << fPosPed[hit->fCounter -1] << "" << frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits-1))->Get(padnum)<< endl;
      fEpos[hit->fCounter-1] += fA_Pos_p[hit->fCounter-1]*
	fParent->GetGain(hit->fCounter-1,fLayerNum-1,0);
    }

    // Sparsify negative side hits, fill the hit list, compute the
    // energy depostion from negative side for the counter.

    Double_t thresh_neg = fNegThresh[hit->fCounter -1];
    thresh_neg=0;
    if(fA_Neg[hit->fCounter-1] >  thresh_neg) {


      fA_Neg_p[hit->fCounter-1] =adc_neg_pedsub ;

      fEneg[hit->fCounter-1] += fA_Neg_p[hit->fCounter-1]*
	fParent->GetGain(hit->fCounter-1,fLayerNum-1,1);
    }

    // Mean energy in the counter.

    fEmean[hit->fCounter-1] += (fEpos[hit->fCounter-1] + fEneg[hit->fCounter-1]);

    // Accumulate energies in the plane.

    fEplane += fEmean[hit->fCounter-1];
    fEplane_pos += fEpos[hit->fCounter-1];
    fEplane_neg += fEneg[hit->fCounter-1];

    ihit++;
  }

  //Debug output.

  if (fParent->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::ProcessHits for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  nrawhits =  " << nrawhits << "  nexthit =  " << nexthit << endl;
    cout << "  Sparsified hits for HMS calorimeter plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;
    for (Int_t jhit = nexthit; jhit < nrawhits; jhit++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(jhit);
      if(hit->fPlane > fLayerNum) {
	break;
      }

      if(fA_Pos[hit->fCounter-1] > fPosThresh[hit->fCounter -1] ||
	 fA_Neg[hit->fCounter-1] > fNegThresh[hit->fCounter -1]) {
	cout << "  plane =  " << hit->fPlane
	     << "  counter =  " << hit->fCounter
	     << "  Emean = " << fEmean[hit->fCounter-1]
	     << "  Epos = " << fEpos[hit->fCounter-1]
	     << "  Eneg = " << fEneg[hit->fCounter-1]
	     << endl;
	nspar++;
      }
    }

    if (nspar == 0) cout << "  No hits\n";

    cout << "  Eplane = " << fEplane
	 << "  Eplane_pos = " << fEplane_pos
	 << "  Eplane_neg = " << fEplane_neg
	 << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return(ihit);
}

//_____________________________________________________________________________
Int_t THcShowerPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list, accumulating into
  // arrays for calculating pedestals.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {

    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    // OK for hit list sorted by layer.
    if(hit->fPlane > fLayerNum) {
      break;
    }
    Int_t element = hit->fCounter - 1; // Should check if in range
    Int_t adcpos = hit->GetData(0);
    Int_t adcneg = hit->GetData(1);

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

  // Debug output.

  if ( ((THcShower*) GetParent())->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::AcculatePedestals for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "Processed hit list for plane " << GetName() << ":\n";

    for (Int_t ih=nexthit; ih<nrawhits; ih++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ih);

      // OK for hit list sorted by layer.
      if(hit->fPlane > fLayerNum) {
	break;
      }

      cout << "  hit " << ih << ":"
	   << "  plane =  " << hit->fPlane
	   << "  counter = " << hit->fCounter
	   << "  ADCpos = " << hit->GetData(0)
	   << "  ADCneg = " << hit->GetData(1)
	   << endl;
    }

    cout << "---------------------------------------------------------------\n";

  }

  return(ihit);
}

//_____________________________________________________________________________
void THcShowerPlane::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals
  // Later add check to see if pedestals have drifted ("Danger Will Robinson!")

  for(Int_t i=0; i<fNelem;i++) {

    // Positive tubes
    fPosPed[i] = ((Float_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosSig[i] = sqrt(((Float_t)fPosPedSum2[i])/TMath::Max(1, fPosPedCount[i])
		      - fPosPed[i]*fPosPed[i]);
    fPosThresh[i] = fPosPed[i] + TMath::Min(50., TMath::Max(10., 3.*fPosSig[i]));

    // Negative tubes
    fNegPed[i] = ((Float_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegSig[i] = sqrt(((Float_t)fNegPedSum2[i])/TMath::Max(1, fNegPedCount[i])
		      - fNegPed[i]*fNegPed[i]);
    fNegThresh[i] = fNegPed[i] + TMath::Min(50., TMath::Max(10., 3.*fNegSig[i]));

  }

  // Debug output.

  if ( ((THcShower*) GetParent())->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::CalculatePedestals for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "  ADC pedestals and thresholds for calorimeter plane "
	 << GetName() << endl;
    for(Int_t i=0; i<fNelem;i++) {
      cout << "  element " << i << ": "
	   << "  Pos. pedestal = " << fPosPed[i]
	   << "  Pos. threshold = " << fPosThresh[i]
	   << "  Neg. pedestal = " << fNegPed[i]
	   << "  Neg. threshold = " << fNegThresh[i]
	   << endl;
    }
    cout << "---------------------------------------------------------------\n";

  }

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

  fPosSig = new Float_t [fNelem];
  fNegSig = new Float_t [fNelem];
  fPosPed = new Float_t [fNelem];
  fNegPed = new Float_t [fNelem];
  fPosThresh = new Float_t [fNelem];
  fNegThresh = new Float_t [fNelem];
  for(Int_t i=0;i<fNelem;i++) {
    fPosPedSum[i] = 0;
    fPosPedSum2[i] = 0;
    fPosPedCount[i] = 0;
    fNegPedSum[i] = 0;
    fNegPedSum2[i] = 0;
    fNegPedCount[i] = 0;
  }
}
