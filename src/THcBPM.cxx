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

//______________________________________________________________
THcBPM::THcBPM( const char* name, const char* description, THaApparatus* a)
  : THaBeamDet(name, description, a)
{
  fAnalyzePedestals = 0;
  fNPedestalEvents = 0;
  frAdcPulseIntRaw = new TClonesArray("THcSignalHit", NCHAN);

}

//______________________________________________________________
THcBPM::~THcBPM()
{
  delete frAdcPulseIntRaw; frAdcPulseIntRaw = nullptr;

  delete [] fPedSum;   fPedSum = nullptr;
  delete [] fPedLimit; fPedLimit = nullptr;
  delete [] fPedCount; fPedCount = nullptr;
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

}

//______________________________________________________________
Int_t THcBPM::Decode( const THaEvData& evdata )
{
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
    THcRawAdcHit& rawPosAdcHit = hit->GetRawAdcHitPos();
    Int_t nsig = hit->fCounter;

    // pulse data
    for(UInt_t thit = 0; thit < rawPosAdcHit.GetNPulses(); thit++) {
      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(nsig, rawPosAdcHit.GetPulseIntRaw(thit));
      ++nrAdcHits;
    }
    
    // or using sample data
    if (rawPosAdcHit.GetNPulses()==0 &&rawPosAdcHit.GetNSamples()>0 ) {
      Int_t NSA= rawPosAdcHit.GetF250_NSA();
      UInt_t LS = 0;
      UInt_t HS = NSA;
      Int_t rawdata = rawPosAdcHit.GetIntegral(LS,HS);
      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(nsig,rawdata );
      ++nrAdcHits;
    }     

    ihit++;
  }// loop over hits

  for(Int_t ielem = 0; ielem < frAdcPulseIntRaw->GetEntries(); ielem++) {
    Int_t    pad_num = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseIntRaw  = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();

    // We expect N paddle = NCHAN = 4 
    if( pad_num > NCHAN-1 ) {
      Warning(Here("Decode"), "Number of fired channels out of range. Should be <= 4. Ignore the channel.");
      continue;
    }

    fRawSignal(pad_num) = pulseIntRaw;
   }

  return fNhits;
}

//______________________________________________________________
THaAnalysisObject::EStatus THcBPM::Init( const TDatime& date )
{
  char EngineDID[] = "xBPMDET";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID);

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

  // Hall C style LoadDB

  char prefix[2];
  prefix[0] = 'g';
  prefix[1] = '\0';
  
  InitializePedestals();

  Double_t pedestals[NCHAN], rotations[NCHAN];
  Double_t offsets[2]; // offset x, y

  memset( pedestals, 0, sizeof(pedestals) );
  memset( rotations, 0, sizeof(rotations) );
  memset( offsets  , 0, sizeof( offsets ) );
  DBRequest list[] = {
    {"_calib_rot", &fCalibRot},
    {"_pedestals", pedestals, kDouble, NCHAN, 1},
    {"_rotmatrix", rotations, kDouble, NCHAN, 1},
    {"_offsets",   offsets,   kDouble, 2, 1},
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
  /*
  RVarDef vars[] = {
    {"rawcur", "", ""},
    {},
    {},
    { nullptr }
  };

  return DefineVarsFromList( vars, mode );
  */
  return 0;
}

//______________________________________________________________
Int_t THcBPM::Process( )
{
  // Calculate position and directions

  
  return 0;
}

//______________________________________________________________
void THcBPM::InitializePedestals()
{

  fNPedestalEvents = 0;
  fMinPed = 500;
  fPedSum = new Int_t [NCHAN];
  fPedCount = new Int_t [NCHAN];
  fPedLimit = new Int_t [NCHAN];
  for(Int_t i = 0; i < NCHAN; i++) {
    fPedSum[i] = 0;
    fPedCount[i] = 0;
    fPedLimit[i] = 1000;    
  }

}

//______________________________________________________________
void THcBPM::AccumulatePedestals( TClonesArray* rawhits )
{
  Int_t nhits = rawhits->GetLast() + 1;
  for(Int_t ihit = 0; ihit < nhits; ihit++) {
    THcRasterRawHit* hit = (THcRasterRawHit*) rawhits->At(ihit);
    THcRawAdcHit&    rawAdcHit = hit->GetRawAdcHitPos();

    Int_t ielem = hit->fCounter -1;
    
    Int_t adc = rawAdcHit.GetPulseIntRaw();
    if(adc <= fPedLimit[ielem]) {
      fPedSum[ielem] += adc;
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
    fPedestals(i) = (Double_t)fPedSum[i]/TMath::Max(1, fPedCount[i]);
  }

}

ClassImp(THcBPM)
