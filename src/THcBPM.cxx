// THcBPM: BPM class for Hall C
// Based on THaBPM but using Hall C style hit list in decoding

#include "THcBPM.h"
#include "THaEvData.h"
#include "THaApparatus.h"
#include "THaCutList.h"

#include "THcDetectorMap.h"
#include "THcRawAdcHit.h"
#include "THcRasterRawHit.h"
#include "THcSignalHit.h"

#include "THcParmList.h"
#include "THcGlobals.h"

static const Int_t NCHAN = 4; // number of channels

using namespace std;

//______________________________________________________________
THcBPM::THcBPM( const char* name, const char* description, THaApparatus* a)
  : THaBeamDet(name, description, a),
    fRawSignal(NCHAN), fPedestals(NCHAN), fCorSignal(NCHAN), fRotPos(NCHAN/2),
    fRot2HCSPos(NCHAN/2,NCHAN/2), fCalibRot(0)
{
  fNhits = 0;
  fAnalyzePedestals = 0;
  fNPedestalEvents = 0;
  fADCMode = 0; // default: kDBPed
  frAdcPulseIntRaw = new TClonesArray("THcSignalHit", NCHAN);
  frAdcPulseInt = new TClonesArray("THcSignalHit", NCHAN);

}

//______________________________________________________________
THcBPM::~THcBPM()
{
  delete frAdcPulseIntRaw; frAdcPulseIntRaw = nullptr;
  delete frAdcPulseInt; frAdcPulseInt = nullptr;

  delete [] fPed;   fPed = nullptr;
  delete [] fPedSum;   fPedSum = nullptr;
  delete [] fPedLimit; fPedLimit = nullptr;
  delete [] fPedCount; fPedCount = nullptr;
}

//______________________________________________________________
THaAnalysisObject::EStatus THcBPM::Init( const TDatime& date )
{
  //  cout << "THcBPM::Init" << endl;

  string EngineDID = string(GetApparatus()->GetName()).substr(0,1) + GetName();
  std::transform(EngineDID.begin(), EngineDID.end(), EngineDID.begin(), ::toupper);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID.c_str()) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID.c_str());
    return kInitError;
  }

  InitHitList(fDetMap,"THcRasterRawHit",fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaBeamDet::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;

}

//______________________________________________________________
Int_t THcBPM::ReadDatabase( const TDatime& date )
{

  // cout << "THcBPM::ReadDatabase" << endl;

  // Hall C style LoadDB
  char prefix[2];
  prefix[0] = 'g';
  prefix[1] = '\0';
  
  // default values
  fMinPed = 0;

  fOrigin.SetXYZ(0.0, 0.0, 0.0);

  InitializePedestals();

  Double_t pedestals[NCHAN], rotations[NCHAN];
  Double_t offsets[2]; // offset x, y

  memset( pedestals, 0, sizeof(pedestals) );
  memset( rotations, 0, sizeof(rotations) );
  memset( offsets  , 0, sizeof( offsets ) );
  DBRequest list[] = {
    {Form("%s_calib_rot",GetName()),     &fCalibRot},                   
    {Form("%s_pedestals",GetName()),     pedestals, kDouble, NCHAN, 1}, // optional
    {Form("%s_rotmatrix",GetName()),     rotations, kDouble, NCHAN, 1},
    {Form("%s_offsets",GetName()),       offsets,   kDouble, 2,     1},
    {Form("%s_mode",GetName()),          &fADCMode, kInt,    0,     1}, 
    {Form("%s_ped_limit",GetName()),     fPedLimit, kInt,    NCHAN, 1}, 
    {Form("%s_min_ped",GetName()),       &fMinPed,  kInt,    0,     1},
    {nullptr}
  };

  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  fOffset.SetXYZ(offsets[0], offsets[1], 0);
  fPedestals.SetElements( pedestals );

  fRot2HCSPos(0,0) = rotations[0];
  fRot2HCSPos(0,1) = rotations[1];
  fRot2HCSPos(1,0) = rotations[2];
  fRot2HCSPos(1,1) = rotations[3];

  return kOK;
}

//______________________________________________________________
Int_t THcBPM::DefineVariables( EMode mode )
{

  RVarDef vars[] = {
    {"rawcur.1", "Current in antenna 1", "GetRawSignal0()"},
    {"rawcur.2", "Current in antenna 2", "GetRawSignal1()"},
    {"rawcur.3", "Current in antenna 3", "GetRawSignal2()"},
    {"rawcur.4", "Current in antenna 4", "GetRawSignal3()"},
    {"x",        "reconstructed x position", "fPosition.fX"},
    {"y",        "reconstructed y position", "fPosition.fY"},
    {"z",        "reconstructed z position", "fPosition.fZ"},
    {"xl",       "local x position in bpm system", "GetRotPosX()"},
    {"yl",       "local y position in bpm system", "GetRotPosY()"},
    { nullptr }
  };

  return DefineVarsFromList( vars, mode );
}

//______________________________________________________________
void THcBPM::Clear( Option_t* opt )
{
  
  THaBeamDet::Clear(opt);
  fPosition.SetXYZ(0.,0.,-10000.);
  fDirection.SetXYZ(0.,0.,1.);
  for( UInt_t k=0; k<NCHAN; ++k ) {
    fRawSignal(k)=-1;
    fCorSignal(k)=-1;
  }

  fRotPos(0) = fRotPos(1) = 0.0;

  fNhits = 0;
  frAdcPulseIntRaw->Clear();
  frAdcPulseInt->Clear();

}

//______________________________________________________________
Int_t THcBPM::Decode( const THaEvData& evdata )
{
  //  cout << "THcBPM::Decode" << endl;

  fNhits = DecodeToHitList(evdata);

  if(gHaCuts->Result("Pedestal_event")) {
    AccumulatePedestals(fRawHitList);
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    fNPedestalEvents++;
    return(0);
  }

  if(fAnalyzePedestals) {
    CalculatePedestals();
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }
  
  Int_t  ihit = 0;
  UInt_t nrAdcHits = 0;
  while(ihit < fNhits) {
    THcRasterRawHit* hit = (THcRasterRawHit*)fRawHitList->At(ihit);
    THcRawAdcHit& rawAdcHit = hit->GetRawAdcHitPos();
    Int_t nsig = hit->fCounter;

    Double_t adcTopC = rawAdcHit.GetAdcTopC();

    // pulse data
    for(UInt_t thit = 0; thit < rawAdcHit.GetNPulses(); thit++) {
      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(nsig, rawAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(nsig, rawAdcHit.GetPulseInt(thit)/adcTopC); // convert back from pC to ADC
      ++nrAdcHits;
    }
    
    // or using simple integral of sample data 
    if (rawAdcHit.GetNPulses()==0 && rawAdcHit.GetNSamples()>0 ) {
      Int_t NSA = rawAdcHit.GetF250_NSA();
      UInt_t LS = 0;
      UInt_t HS = NSA;
      Int_t rawdata = rawAdcHit.GetIntegral(LS,HS);

      UInt_t   SampPed = rawAdcHit.GetSampPedRaw(); 
      // NSA+1: because we call GetIntegral for [0, NSA]; NSA+1 samples
      Double_t PeakPedestalRatio = 1.0 * (NSA+1)/rawAdcHit.GetF250_NPedestalSamples();
      Double_t data = (rawdata - SampPed * PeakPedestalRatio);
      
      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(nsig, rawdata);
      ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(nsig, data);
      ++nrAdcHits;
    }
    ihit++;
  }// loop over hits

  // subtract pedestal and fill vectors
  for(Int_t ielem = 0; ielem < frAdcPulseIntRaw->GetEntries(); ielem++) {
    Int_t    pad_num = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw  = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseInt  = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetData();

    // We expect N paddle = NCHAN = 4 
    if( pad_num > NCHAN-1 ) {
      Warning(Here("Decode"), "Number of fired channels out of range. Should be <= 4. Ignore the channel.");
      continue;
    }

    fRawSignal(pad_num) = pulseIntRaw;

    // Subtract pedestals
    if(fADCMode == kDynamicPed){
      // Use event by event ped subtraction
      fCorSignal(pad_num) = pulseInt;
    } else if (fADCMode == kDBPed ) {
      // Use ped values from DB file (default)
      fCorSignal(pad_num) = fRawSignal(pad_num) - fPedestals(pad_num);
    } else {
      // kCalculatePed
      fCorSignal(pad_num) = fRawSignal(pad_num) - fPed[pad_num];
    }
  }
    
  return fNhits;
}

//______________________________________________________________
Int_t THcBPM::Process( )
{
  // Calculate position and directions
  // (x, y)_lab = Cij * (x, y)_bpm + (offset_x, offset_y)

  // First calculate position in bpm coord system
  // assume 0:x+ 1:x- 2:y+ 3:y-
  for(Int_t k = 0; k < NCHAN; k += 2) {
    Double_t ap = fCorSignal(k);
    Double_t am = fCorSignal(k+1);

    fRotPos(k / 2) = 0.0;
    if( ap +  am != 0.0 )
      fRotPos(k / 2) = fCalibRot * (ap - am) / (ap + am);
  }

  // transform it into the HCS
  TVectorD temp(fRotPos); 
  temp *= fRot2HCSPos;
  fPosition.SetXYZ( temp(0) + fOrigin(0) + fOffset(0),
		    temp(1) + fOrigin(1) + fOffset(1),
		    fOrigin(2) );
  
  return 0;
}

//______________________________________________________________
void THcBPM::InitializePedestals()
{

  fNPedestalEvents = 0;
  fPed = new Int_t [NCHAN];
  fPedSum = new Int_t [NCHAN];
  fPedCount = new Int_t [NCHAN];
  fPedLimit = new Int_t [NCHAN];
  for(Int_t i = 0; i < NCHAN; i++) {
    fPed[i] = 0;
    fPedSum[i] = 0;
    fPedCount[i] = 0;
    fPedLimit[i] = 1000;    
  }

}

//______________________________________________________________
void THcBPM::AccumulatePedestals( TClonesArray* rawhits )
{

  UInt_t nhits = rawhits->GetLast() + 1;

  for(UInt_t ihit = 0; ihit < nhits; ihit++) {
    THcRasterRawHit* hit = (THcRasterRawHit*)fRawHitList->At(ihit);
    
    Int_t ielem = hit->fCounter -1;
    Int_t rawadc = hit->GetRawAdcHitPos().GetPulseIntRaw();

    if(rawadc <= fPedLimit[ielem]) {
      fPedSum[ielem] += rawadc;
      fPedCount[ielem]++;

      //fMinPed is hard-coded value, 500
      if(fPedCount[ielem] == fMinPed/5) {
	fPedLimit[ielem] = 100 + fPedSum[ielem]/fPedCount[ielem];
      }
    }
  }//loop over hits

}

//______________________________________________________________
void THcBPM::CalculatePedestals()
{
  // Calculate pedestal mean
  for(Int_t i = 0; i < NCHAN; i++) {
    fPed[i] = (Double_t)fPedSum[i]/TMath::Max(1, fPedCount[i]);
  }

}

ClassImp(THcBPM)
