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

  frPosAdcErrorFlag = new TClonesArray("THcSignalHit", 16);
  frPosAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcThreshold = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frPosAdcPed = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

  frNegAdcErrorFlag = new TClonesArray("THcSignalHit", 16);
  frNegAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcThreshold = new TClonesArray("THcSignalHit", 16);
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

  frPosAdcErrorFlag = NULL;
  frPosAdcPedRaw = NULL;
  frPosAdcThreshold = NULL;
  frPosAdcPulseIntRaw = NULL;
  frPosAdcPulseAmpRaw = NULL;
  frPosAdcPulseTimeRaw = NULL;

  frPosAdcPed = NULL;
  frPosAdcPulseInt = NULL;
  frPosAdcPulseAmp = NULL;

  frNegAdcErrorFlag = NULL;
  frNegAdcPedRaw = NULL;
  frNegAdcThreshold = NULL;
  frNegAdcPulseIntRaw = NULL;
  frNegAdcPulseAmpRaw = NULL;
  frNegAdcPulseTimeRaw = NULL;

  frNegAdcPed = NULL;
  frNegAdcPulseInt = NULL;
  frNegAdcPulseAmp = NULL;

 
  //  delete [] fEpos;
  // delete [] fEneg;
  // delete [] fEmean;
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
  fPedSampLow=0;
  fPedSampHigh=9;
  fDataSampLow=23;
  fDataSampHigh=49;
    fAdcNegThreshold=0.;
    fAdcPosThreshold=0.;
  DBRequest list[]={
    {"cal_AdcNegThreshold", &fAdcNegThreshold, kDouble, 0, 1},
    {"cal_AdcPosThreshold", &fAdcPosThreshold, kDouble, 0, 1},
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

  fNumGoodPosAdcHits          = vector<Int_t>    (fNelem, 0.0);
  fNumGoodNegAdcHits          = vector<Int_t>    (fNelem, 0.0);

  fGoodPosAdcPulseIntRaw      = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseIntRaw      = vector<Double_t> (fNelem, 0.0);

  fGoodPosAdcPed              = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseInt         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseAmp         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseTime        = vector<Double_t> (fNelem, 0.0);

  fGoodNegAdcPed              = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseInt         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseAmp         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseTime        = vector<Double_t> (fNelem, 0.0);

  // Energy depositions per block (not corrected for track coordinate)

  fEpos                       = vector<Double_t> (fNelem, 0.0);
  fEneg                       = vector<Double_t> (fNelem, 0.0);
  fEmean                      = vector<Double_t> (fNelem, 0.0);  
  //  fEpos = new Double_t[fNelem];
  // fEneg = new Double_t[fNelem];
  // fEmean= new Double_t[fNelem];

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
  vector<RVarDef> vars;

   vars.push_back(RVarDef{"numGoodPosAdcHits",    "Number of Good Positive ADC Hits Per PMT", "fNumGoodPosAdcHits"});    // PreSh occupancy
  vars.push_back(RVarDef{"numGoodNegAdcHits",    "Number of Good Negative ADC Hits Per PMT", "fNumGoodNegAdcHits"});    // PreSh occupancy
  vars.push_back(RVarDef{"totNumGoodPosAdcHits", "Total Number of Good Positive ADC Hits",   "fTotNumGoodPosAdcHits"}); // PreSh multiplicity
  vars.push_back(RVarDef{"totNumGoodNegAdcHits", "Total Number of Good Negative ADC Hits",   "fTotNumGoodNegAdcHits"}); // PreSh multiplicity
  vars.push_back(RVarDef{"totnumGoodAdcHits",   "TotalNumber of Good ADC Hits Per PMT",      "fTotNumGoodAdcHits"});    // PreSh multiplicity

  vars.push_back(RVarDef{"goodPosAdcPulseIntRaw",       "Good Positive Raw ADC Pulse Integrals",                   "fGoodPosAdcPulseIntRaw"});
  vars.push_back(RVarDef{"goodNegAdcPulseIntRaw",       "Good Negative Raw ADC Pulse Integrals",                   "fGoodNegAdcPulseIntRaw"});

  vars.push_back(RVarDef{"goodPosAdcPed",         "Good Positive ADC pedestals",           "fGoodPosAdcPed"});
  vars.push_back(RVarDef{"goodPosAdcPulseInt",         "Good Positive ADC integrals",           "fGoodPosAdcPulseInt"});
  vars.push_back(RVarDef{"goodPosAdcPulseAmp",         "Good Positive ADC amplitudes",           "fGoodPosAdcPulseAmp"});
  vars.push_back(RVarDef{"goodPosAdcPulseTime",         "Good Positive ADC times",           "fGoodPosAdcPulseTime"});  
  
  vars.push_back(RVarDef{"goodNegAdcPed",         "Good Negative ADC pedestals",           "fGoodNegAdcPed"});
  vars.push_back(RVarDef{"goodNegAdcPulseInt",         "Good Negative ADC integrals",           "fGoodNegAdcPulseInt"});
  vars.push_back(RVarDef{"goodNegAdcPulseAmp",         "Good Negative ADC amplitudes",           "fGoodNegAdcPulseAmp"});
  vars.push_back(RVarDef{"goodNegAdcPulseTime",         "Good Negative ADC times",           "fGoodNegAdcPulseTime"});  
  
  vars.push_back(RVarDef{"epos",       "Energy Depositions from Positive Side PMTs",    "fEpos"});
  vars.push_back(RVarDef{"eneg",       "Energy Depositions from Negative Side PMTs",    "fEneg"});
  vars.push_back(RVarDef{"emean",      "Mean Energy Depositions",                       "fEmean"});
  vars.push_back(RVarDef{"eplane",     "Energy Deposition per plane",                   "fEplane"});
  vars.push_back(RVarDef{"eplane_pos", "Energy Deposition per plane from pos. PMTs","fEplane_pos"});
  vars.push_back(RVarDef{"eplane_neg", "Energy Deposition per plane from neg. PMTs","fEplane_neg"});
  
  vars.push_back(RVarDef{"posAdcCounter",      "List of positive ADC counter numbers.",      "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"});
  vars.push_back(RVarDef{"negAdcCounter",      "List of negative ADC counter numbers.",      "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"});

  vars.push_back(RVarDef{"totNumPosAdcHits", "Total Number of Positive ADC Hits",   "fTotNumPosAdcHits"}); // PreSh+ raw multiplicity
  vars.push_back(RVarDef{"totNumNegAdcHits", "Total Number of Negative ADC Hits",   "fTotNumNegAdcHits"}); // PreSh+ raw multiplicity
  vars.push_back(RVarDef{"totnumAdcHits",   "TotalNumber of ADC Hits Per PMT",      "fTotNumAdcHits"});    // PreSh raw multiplicity

  vars.push_back(RVarDef{"posAdcErrorFlag",       "List of positive raw ADC Error Flags",         "frPosAdcErrorFlag.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPedRaw",       "List of positive raw ADC pedestals",         "frPosAdcPedRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPulseIntRaw",  "List of positive raw ADC pulse integrals.",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPulseAmpRaw",  "List of positive raw ADC pulse amplitudes.", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPulseTimeRaw", "List of positive raw ADC pulse times.",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"});

  vars.push_back(RVarDef{"posAdcPed",          "List of positive ADC pedestals",             "frPosAdcPed.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPulseInt",     "List of positive ADC pulse integrals.",      "frPosAdcPulseInt.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"posAdcPulseAmp",     "List of positive ADC pulse amplitudes.",     "frPosAdcPulseAmp.THcSignalHit.GetData()"});
  
  vars.push_back(RVarDef{"negAdcErrorFlag",       "List of negative raw ADC Error Flag ",         "frNegAdcErrorFlag.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPedRaw",       "List of negative raw ADC pedestals",         "frNegAdcPedRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPulseIntRaw",  "List of negative raw ADC pulse integrals.",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPulseAmpRaw",  "List of negative raw ADC pulse amplitudes.", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPulseTimeRaw", "List of negative raw ADC pulse times.",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"});

  vars.push_back(RVarDef{"negAdcPed",          "List of negative ADC pedestals",             "frNegAdcPed.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPulseInt",     "List of negative ADC pulse integrals.",      "frNegAdcPulseInt.THcSignalHit.GetData()"});
  vars.push_back(RVarDef{"negAdcPulseAmp",     "List of negative ADC pulse amplitudes.",     "frNegAdcPulseAmp.THcSignalHit.GetData()"});
    
  RVarDef end {0};
  vars.push_back(end);

  return DefineVarsFromList(vars.data(), mode);
}

//_____________________________________________________________________________
void THcShowerPlane::Clear( Option_t* )
{
  // Clears the hit lists
 
  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcErrorFlag->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcThreshold->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegAdcErrorFlag->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcThreshold->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

 for (UInt_t ielem = 0; ielem < fGoodPosAdcPed.size(); ielem++) {
   fGoodPosAdcPed.at(ielem)              = 0.0;
   fGoodPosAdcPulseIntRaw.at(ielem)      = 0.0;   
   fGoodPosAdcPulseInt.at(ielem)         = 0.0;
   fGoodPosAdcPulseAmp.at(ielem)         = 0.0;
   fGoodPosAdcPulseTime.at(ielem)        = 0.0;
   fEpos.at(ielem)                       = 0.0;
   fNumGoodPosAdcHits.at(ielem)          = 0.0;
 }
 
 for (UInt_t ielem = 0; ielem < fGoodNegAdcPed.size(); ielem++) {
   fGoodNegAdcPed.at(ielem)              = 0.0;
   fGoodNegAdcPulseIntRaw.at(ielem)      = 0.0;   
   fGoodNegAdcPulseInt.at(ielem)         = 0.0;
   fGoodNegAdcPulseAmp.at(ielem)         = 0.0;
   fGoodNegAdcPulseTime.at(ielem)        = 0.0;
   fEneg.at(ielem)                       = 0.0;
   fNumGoodNegAdcHits.at(ielem)          = 0.0;
 }

 for (UInt_t ielem = 0; ielem < fEmean.size(); ielem++) {
   fEmean.at(ielem)                                = 0.0;
 }
 
 fTotNumAdcHits       = 0;
 fTotNumPosAdcHits    = 0;
 fTotNumNegAdcHits    = 0; 
 
 fTotNumGoodAdcHits       = 0;
 fTotNumGoodPosAdcHits    = 0;
 fTotNumGoodNegAdcHits    = 0; 


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

  // Initialize variables.

  fPosADCHits->Clear();
  fNegADCHits->Clear();

  frPosAdcErrorFlag->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcThreshold->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegAdcErrorFlag->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcThreshold->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

  /*
    for(Int_t i=0;i<fNelem;i++) {

    fEpos[i] = 0;
    fEneg[i] = 0;
    fEmean[i] = 0;
  }
  */

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
      ((THcSignalHit*) frPosAdcThreshold->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetPedRaw()*rawPosAdcHit.GetF250_PeakPedestalRatio()+fAdcPosThreshold);
      ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPed());

      ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTimeRaw(thit));
      if (rawPosAdcHit.GetPulseAmp(thit)>0&&rawPosAdcHit.GetPulseIntRaw(thit)>0) {
	((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum,0);        
      } else {
	((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum,1);        
      }
      ++nrPosAdcHits;
      fTotNumAdcHits++;
      fTotNumPosAdcHits++;
     
    }
    THcRawAdcHit& rawNegAdcHit = hit->GetRawAdcHitNeg();
    for (UInt_t thit=0; thit<rawNegAdcHit.GetNPulses(); ++thit) {
      ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPedRaw());
      ((THcSignalHit*) frNegAdcThreshold->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetPedRaw()*rawNegAdcHit.GetF250_PeakPedestalRatio()+fAdcNegThreshold);
      ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPed());

      ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTimeRaw(thit));
      if (rawNegAdcHit.GetPulseAmp(thit)>0&&rawNegAdcHit.GetPulseIntRaw(thit)>0) {
	((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum,0);
      } else {
	((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum,1);
      }
      ++nrNegAdcHits;
      fTotNumAdcHits++;
      fTotNumNegAdcHits++;
      
    
    }
    ihit++;
  }
  return(ihit);
}
//_____________________________________________________________________________
Int_t THcShowerPlane::CoarseProcessHits()
{
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
    Int_t ADCMode=fParent->GetADCMode();
    if(ADCMode == kADCDynamicPedestal) {
      FillADC_DynamicPedestal();
    } else if (ADCMode == kADCSampleIntegral) {
      FillADC_SampleIntegral();
    } else if (ADCMode == kADCSampIntDynPed) {
      FillADC_SampIntDynPed();
        } else {
      FillADC_Standard();
    }
    //
  if (fParent->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerPlane::ProcessHits for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  Sparsified hits for HMS calorimeter plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;

    for (Int_t i=0; i<fNelem; i++) {

      if (GetAposP(i) > 0  || GetAnegP(i) >0) {    //hit
	cout << "  counter =  " << i
	     << "  Emean = " << fEmean[i]
	     << "  Epos = " << fEpos[i]
	     << "  Eneg = " << fEneg[i]
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
  //
  return 1;
    //
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_SampIntDynPed()
{
  //    adc_pos = hit->GetRawAdcHitPos().GetSampleInt();
  //    adc_neg = hit->GetRawAdcHitNeg().GetSampleInt();
  //   adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //   adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // Need to create
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_SampleIntegral()
{
  ///			adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[hit->fCounter -1];
  //			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[hit->fCounter -1];
  //			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // need to create
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_Standard()
{
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  for (Int_t ielem=0;ielem<frNegAdcPulseIntRaw->GetEntries();ielem++) {
    Int_t npad = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    fGoodNegAdcPulseIntRaw.at(npad) = pulseIntRaw;
      if(fGoodNegAdcPulseIntRaw.at(npad) >  fNegThresh[npad]) {
	fGoodNegAdcPulseInt.at(npad) = pulseIntRaw-fNegPed[npad];
	fEneg.at(npad) = fGoodNegAdcPulseInt.at(npad)*fParent->GetGain(npad,fLayerNum-1,1);
	fEmean.at(npad) += fEneg.at(npad);
	fEplane_neg += fEneg.at(npad);
      }
  }
  for (Int_t ielem=0;ielem<frPosAdcPulseIntRaw->GetEntries();ielem++) {
    Int_t npad = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    fGoodPosAdcPulseIntRaw.at(npad) =pulseIntRaw;
    if(fGoodPosAdcPulseIntRaw.at(npad) > fPosThresh[npad]) {
      fGoodPosAdcPulseInt.at(npad) =pulseIntRaw-fPosPed[npad] ;
      fEpos.at(npad) =fGoodPosAdcPulseInt.at(npad)*fParent->GetGain(npad,fLayerNum-1,0);
      fEmean.at(npad) += fEpos.at(npad);
      fEplane_pos += fEpos.at(npad);
      }
  }
    fEplane= fEplane_neg+fEplane_pos;
}
//_____________________________________________________________________________
void THcShowerPlane::FillADC_DynamicPedestal()
{
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  Double_t AdcTimeWindowMin=fParent->GetAdcTimeWindowMin();
  Double_t AdcTimeWindowMax=fParent->GetAdcTimeWindowMax();
  for (Int_t ielem=0;ielem<frNegAdcPulseInt->GetEntries();ielem++) {
    Int_t    npad         = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseInt     = ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulsePed     = ((THcSignalHit*) frNegAdcPed->ConstructedAt(ielem))->GetData(); 
    Double_t pulseAmp     = ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(ielem))->GetData(); 
    Double_t pulseIntRaw  = ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
    Double_t threshold    = ((THcSignalHit*) frNegAdcThreshold->ConstructedAt(ielem))->GetData();
    Bool_t   errorflag    = ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(ielem))->GetData();
    Bool_t   pulseTimeCut = (pulseTime > AdcTimeWindowMin) &&  (pulseTime < AdcTimeWindowMax);
    if (!errorflag && pulseTimeCut) {
      fGoodNegAdcPulseIntRaw.at(npad) =pulseIntRaw;
                 
      if(fGoodNegAdcPulseIntRaw.at(npad) >  threshold && fGoodNegAdcPulseInt.at(npad)==0) {
        fGoodNegAdcPulseInt.at(npad) =pulseInt ;
	fEneg.at(npad) =  fGoodNegAdcPulseInt.at(npad)*fParent->GetGain(npad,fLayerNum-1,1);
	fEmean.at(npad) += fEneg.at(npad);
	fEplane_neg += fEneg.at(npad);

      fGoodNegAdcPed.at(npad) = pulsePed; 
      fGoodNegAdcPulseAmp.at(npad) = pulseAmp; 
      fGoodNegAdcPulseTime.at(npad) = pulseTime; 

      fTotNumGoodAdcHits++;
      fTotNumGoodNegAdcHits++;
      fNumGoodNegAdcHits.at(npad) = npad + 1;

      }
      
    }        
  }
  //
  for (Int_t ielem=0;ielem<frPosAdcPulseInt->GetEntries();ielem++) {
    Int_t    npad         = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulsePed     = ((THcSignalHit*) frPosAdcPed->ConstructedAt(ielem))->GetData(); 
    Double_t threshold    = ((THcSignalHit*) frPosAdcThreshold->ConstructedAt(ielem))->GetData();
    Double_t pulseAmp     = ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(ielem))->GetData(); 
    Double_t pulseInt     = ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseTime    = ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
    Bool_t   errorflag    = ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(ielem))->GetData();
    Bool_t   pulseTimeCut = (pulseTime > AdcTimeWindowMin) &&  (pulseTime < AdcTimeWindowMax);
    if (!errorflag && pulseTimeCut) {
      fGoodPosAdcPulseIntRaw.at(npad) = pulseIntRaw;
     
      if(fGoodPosAdcPulseIntRaw.at(npad) >  threshold && fGoodPosAdcPulseInt.at(npad)==0) {
       
       	fGoodPosAdcPulseInt.at(npad) =pulseInt ;
	fEpos.at(npad) = fGoodPosAdcPulseInt.at(npad)*fParent->GetGain(npad,fLayerNum-1,0);
	fEmean.at(npad) += fEpos[npad];
	fEplane_pos += fEpos.at(npad);

      fGoodPosAdcPed.at(npad) = pulsePed; 
      fGoodPosAdcPulseAmp.at(npad) = pulseAmp; 
      fGoodPosAdcPulseTime.at(npad) = pulseTime;  

      fTotNumGoodAdcHits++;
      fTotNumGoodPosAdcHits++;
      fNumGoodPosAdcHits.at(npad) = npad + 1;

    }
    }        
  }
  //
    fEplane= fEplane_neg+fEplane_pos;

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
