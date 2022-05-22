/** \class THcScintillatorPlane
    \ingroup DetSupport

\brief A single plane of scintillators.

The THcHodoscope class instatiates one of these objects per plane.

*/
#include "TMath.h"
#include "THcScintillatorPlane.h"
#include "THcScintPlaneCluster.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcHodoscope.h"
#include "TClass.h"
#include "THcRawAdcHit.h"
#include "THcRawTdcHit.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcScintillatorPlane)

//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name,
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
: THaSubDetector(name,description,parent),
  fParentHitList(0), fCluster(0),frPosAdcErrorFlag(0), frNegAdcErrorFlag(0),
  frPosTDCHits(0), frNegTDCHits(0), frPosADCHits(0), frNegADCHits(0),
  frPosADCSums(0), frNegADCSums(0), frPosADCPeds(0), frNegADCPeds(0),
  fHodoHits(0), frPosTdcTimeRaw(0), frPosAdcPedRaw(0),
  frPosAdcPulseIntRaw(0), frPosAdcPulseAmpRaw(0),
  frPosAdcPulseTimeRaw(0), frPosTdcTime(0), frPosAdcPed(0),
  frPosAdcPulseInt(0), frPosAdcPulseAmp(0), frPosAdcPulseTime(0),
  frNegTdcTimeRaw(0), frNegAdcPedRaw(0), frNegAdcPulseIntRaw(0),
  frNegAdcPulseAmpRaw(0), frNegAdcPulseTimeRaw(0), frNegTdcTime(0),
  frNegAdcPed(0), frNegAdcPulseInt(0), frNegAdcPulseAmp(0),
  frNegAdcPulseTime(0), fPosCenter(0), fHodoPosMinPh(0),
  fHodoNegMinPh(0), fHodoPosPhcCoeff(0), fHodoNegPhcCoeff(0),
  fHodoPosTimeOffset(0), fHodoNegTimeOffset(0), fHodoVelLight(0),
  fHodoPosInvAdcOffset(0), fHodoNegInvAdcOffset(0),
  fHodoPosAdcTimeWindowMin(0), fHodoPosAdcTimeWindowMax(0),
  fHodoNegAdcTimeWindowMin(0), fHodoNegAdcTimeWindowMax(0),
  fHodoPosInvAdcLinear(0), fHodoNegInvAdcLinear(0),
  fHodoPosInvAdcAdc(0), fHodoNegInvAdcAdc(0), fHodoVelFit(0),
  fHodoCableFit(0), fHodo_LCoeff(0), fHodoPos_c1(0), fHodoNeg_c1(0),
  fHodoPos_c2(0), fHodoNeg_c2(0), fHodoSigma(0), fPosPedSum(0),
  fPosPedSum2(0), fPosPedLimit(0), fPosPedCount(0), fNegPedSum(0),
  fNegPedSum2(0), fNegPedLimit(0), fNegPedCount(0), fPosPed(0),
  fPosSig(0), fPosThresh(0), fNegPed(0), fNegSig(0), fNegThresh(0)
{
  // Normal constructor with name and description
  fHodoHits = new TClonesArray("THcHodoHit",16);

  fCluster = new TClonesArray("THcScintPlaneCluster", 10);

  frPosAdcErrorFlag = new TClonesArray("THcSignalHit", 16);
  frNegAdcErrorFlag = new TClonesArray("THcSignalHit", 16);

  frPosTDCHits = new TClonesArray("THcSignalHit",16);
  frNegTDCHits = new TClonesArray("THcSignalHit",16);
  frPosADCHits = new TClonesArray("THcSignalHit",16);
  frNegADCHits = new TClonesArray("THcSignalHit",16);
  frPosADCSums = new TClonesArray("THcSignalHit",16);
  frNegADCSums = new TClonesArray("THcSignalHit",16);
  frPosADCPeds = new TClonesArray("THcSignalHit",16);
  frNegADCPeds = new TClonesArray("THcSignalHit",16);

  frPosTdcTimeRaw      = new TClonesArray("THcSignalHit", 16);
  frPosAdcPedRaw       = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frPosTdcTime      = new TClonesArray("THcSignalHit", 16);
  frPosAdcPed       = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseInt  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmp  = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTime = new TClonesArray("THcSignalHit", 16);

  frNegTdcTimeRaw      = new TClonesArray("THcSignalHit", 16);
  frNegAdcPedRaw       = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frNegTdcTime      = new TClonesArray("THcSignalHit", 16);
  frNegAdcPed       = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseInt  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmp  = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTime = new TClonesArray("THcSignalHit", 16);

  frPosAdcSampPedRaw       = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPed          = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseInt	   = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseAmp	   = new TClonesArray("THcSignalHit", 16);
  frPosAdcSampPulseTime	   = new TClonesArray("THcSignalHit", 16);

  frNegAdcSampPedRaw       = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseIntRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseAmpRaw  = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseTimeRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPed 	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseInt	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseAmp	   = new TClonesArray("THcSignalHit", 16);
  frNegAdcSampPulseTime    = new TClonesArray("THcSignalHit", 16);

  fPlaneNum = planenum;
  fTotPlanes = planenum;
  fNScinHits = 0;
}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor
  if( fIsSetup )
    RemoveVariables();
  delete  frPosAdcErrorFlag; frPosAdcErrorFlag = NULL;
  delete  frNegAdcErrorFlag; frNegAdcErrorFlag = NULL;

  delete  fCluster; fCluster = NULL;

  delete fHodoHits;
  delete frPosTDCHits;
  delete frNegTDCHits;
  delete frPosADCHits;
  delete frNegADCHits;
  delete frPosADCSums;
  delete frNegADCSums;
  delete frPosADCPeds;
  delete frNegADCPeds;

  delete frPosTdcTimeRaw;
  delete frPosAdcPedRaw;
  delete frPosAdcPulseIntRaw;
  delete frPosAdcPulseAmpRaw;
  delete frPosAdcPulseTimeRaw;

  delete frPosTdcTime;
  delete frPosAdcPed;
  delete frPosAdcPulseInt;
  delete frPosAdcPulseAmp;
  delete frPosAdcPulseTime;

  delete frNegTdcTimeRaw;
  delete frNegAdcPedRaw;
  delete frNegAdcPulseIntRaw;
  delete frNegAdcPulseAmpRaw;
  delete frNegAdcPulseTimeRaw;

  delete frNegTdcTime;
  delete frNegAdcPed;
  delete frNegAdcPulseInt;
  delete frNegAdcPulseAmp;
  delete frNegAdcPulseTime;

  delete frPosAdcSampPedRaw;       frPosAdcSampPedRaw       = NULL;
  delete frPosAdcSampPulseIntRaw;  frPosAdcSampPulseIntRaw  = NULL;
  delete frPosAdcSampPulseAmpRaw;  frPosAdcSampPulseAmpRaw  = NULL;
  delete frPosAdcSampPulseTimeRaw; frPosAdcSampPulseTimeRaw = NULL;
  delete frPosAdcSampPed;          frPosAdcSampPed          = NULL;
  delete frPosAdcSampPulseInt;     frPosAdcSampPulseInt     = NULL;
  delete frPosAdcSampPulseAmp;     frPosAdcSampPulseAmp     = NULL;
  delete frPosAdcSampPulseTime;    frPosAdcSampPulseTime    = NULL;

  delete frNegAdcSampPedRaw;       frNegAdcSampPedRaw       = NULL;
  delete frNegAdcSampPulseIntRaw;  frNegAdcSampPulseIntRaw  = NULL;
  delete frNegAdcSampPulseAmpRaw;  frNegAdcSampPulseAmpRaw  = NULL;
  delete frNegAdcSampPulseTimeRaw; frNegAdcSampPulseTimeRaw = NULL;
  delete frNegAdcSampPed;          frNegAdcSampPed          = NULL;
  delete frNegAdcSampPulseInt;     frNegAdcSampPulseInt     = NULL;
  delete frNegAdcSampPulseAmp;     frNegAdcSampPulseAmp     = NULL;
  delete frNegAdcSampPulseTime;    frNegAdcSampPulseTime    = NULL;

  delete [] fPosCenter; fPosCenter = 0;

  delete [] fHodoPosMinPh; fHodoPosMinPh = NULL;
  delete [] fHodoNegMinPh; fHodoNegMinPh = NULL;
  delete [] fHodoPosPhcCoeff; fHodoPosPhcCoeff = NULL;
  delete [] fHodoNegPhcCoeff; fHodoNegPhcCoeff = NULL;
  delete [] fHodoPosTimeOffset; fHodoPosTimeOffset = NULL;
  delete [] fHodoNegTimeOffset; fHodoNegTimeOffset = NULL;
  delete [] fHodoPosInvAdcOffset; fHodoPosInvAdcOffset = NULL;
  delete [] fHodoNegInvAdcOffset; fHodoNegInvAdcOffset = NULL;
  delete [] fHodoPosInvAdcLinear; fHodoPosInvAdcLinear = NULL;
  delete [] fHodoNegInvAdcLinear; fHodoNegInvAdcLinear = NULL;
  delete [] fHodoPosAdcTimeWindowMax; fHodoPosAdcTimeWindowMax = NULL;
  delete [] fHodoPosAdcTimeWindowMin; fHodoPosAdcTimeWindowMin = NULL;
  delete [] fHodoNegAdcTimeWindowMax; fHodoNegAdcTimeWindowMax = NULL;
  delete [] fHodoNegAdcTimeWindowMin; fHodoNegAdcTimeWindowMin = NULL;
  delete [] fHodoNegInvAdcLinear; fHodoNegInvAdcLinear = NULL;
  delete [] fHodoPosInvAdcAdc; fHodoPosInvAdcAdc = NULL;
  delete [] fHodoNegInvAdcAdc; fHodoNegInvAdcAdc = NULL;
  delete [] fHodoVelFit;                 fHodoVelFit = NULL;
  delete [] fHodoCableFit;               fHodoCableFit = NULL;
  delete [] fHodo_LCoeff;                fHodo_LCoeff = NULL;
  delete [] fHodoPos_c1;                 fHodoPos_c1 = NULL;
  delete [] fHodoNeg_c1;                 fHodoNeg_c1 = NULL;
  delete [] fHodoPos_c2;                 fHodoPos_c2 = NULL;
  delete [] fHodoNeg_c2;                 fHodoNeg_c2 = NULL;


  delete [] fHodoVelLight; fHodoVelLight = NULL;
  delete [] fHodoSigma; fHodoSigma = NULL;

  delete [] fPosPedSum; fPosPedSum = 0;
  delete [] fPosPedSum2; fPosPedSum2 = 0;
  delete [] fPosPedLimit; fPosPedLimit = 0;
  delete [] fPosPedCount; fPosPedCount = 0;
  delete [] fNegPedSum; fNegPedSum = 0;
  delete [] fNegPedSum2; fNegPedSum2 = 0;
  delete [] fNegPedLimit; fNegPedLimit = 0;
  delete [] fNegPedCount; fNegPedCount = 0;
  delete [] fPosPed; fPosPed = 0;
  delete [] fNegPed; fNegPed = 0;
  delete [] fPosThresh; fPosThresh = 0;
  delete [] fNegThresh; fNegThresh = 0;
}

//______________________________________________________________________________
THaAnalysisObject::EStatus THcScintillatorPlane::Init( const TDatime& date )
{

  // cout << "THcScintillatorPlane::Init called " << GetName() << endl;

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  // Get the Hodoscope hitlist
  // Can't seem to cast to THcHitList.  What to do if we want to use
  // THcScintillatorPlane as a subdetector to other than THcHodoscope?
  //  fParentHitList = static_cast<THcHodoscope*>(GetParent())->GetHitList();

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::ReadDatabase( const TDatime& date )
{

  // See what file it looks for

  //  static const char* const here = "ReadDatabase()";
  char prefix[2];

  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  // need this further down so read them first! GN
  string parname = "scin_" + string(GetName()) + "_nr";
  DBRequest list_1[] = {
    {parname.c_str(), &fNelem, kInt},
    {0}
  };
  gHcParms->LoadParmValues(list_1, prefix);

  // Based on the signs of these quantities in the .pos file the correspondence
  // should be bot=>left  and top=>right when comparing x and y-type scintillators
  char tmpleft[6], tmpright[6];
  if (fPlaneNum==1 || fPlaneNum==3) {
    strcpy(tmpleft,"left");
    strcpy(tmpright,"right");
  }
  else {
    strcpy(tmpleft,"bot");
    strcpy(tmpright,"top");
  }

  delete [] fPosCenter; fPosCenter = new Double_t[fNelem];

  DBRequest list[]={
    {Form("scin_%s_zpos",GetName()), &fZpos, kDouble},
    {Form("scin_%s_dzpos",GetName()), &fDzpos, kDouble},
    {Form("scin_%s_size",GetName()), &fSize, kDouble},
    {Form("scin_%s_spacing",GetName()), &fSpacing, kDouble},
    {Form("scin_%s_%s",GetName(),tmpleft), &fPosLeft,kDouble},
    {Form("scin_%s_%s",GetName(),tmpright), &fPosRight,kDouble},
    {Form("scin_%s_offset",GetName()), &fPosOffset, kDouble},
    {Form("scin_%s_center",GetName()), fPosCenter,kDouble,fNelem},
    {"tofusinginvadc",   &fTofUsingInvAdc,        kInt,            0,  1},
    {"hodo_adc_mode", &fADCMode, kInt, 0, 1},
    {"hodo_pedestal_scale", &fADCPedScaleFactor, kDouble, 0, 1},
    {"hodo_adc_diag_cut", &fADCDiagCut, kInt, 0, 1},
    {"cosmicflag", &fCosmicFlag, kInt, 0, 1},
    {"hodo_debug_adc",  &fDebugAdc, kInt, 0, 1},
    {"hodo_SampThreshold",     &fSampThreshold,       kDouble,0,1},
    {"hodo_SampNSA",     &fSampNSA,       kInt,0,1},
    {"hodo_SampNSAT",     &fSampNSAT,       kInt,0,1},
    {"hodo_SampNSB",     &fSampNSB,       kInt,0,1},
    {"hodo_OutputSampWaveform",     &fOutputSampWaveform,       kInt,0,1},
    {"hodo_UseSampWaveform",     &fUseSampWaveform,       kInt,0,1},
    {0}
  };

  fDebugAdc = 0; // Set ADC debug parameter to false unless set in parameter file
  fTofUsingInvAdc = 1;
  //fADCMode = kADCStandard;
  fADCMode = kADCDynamicPedestal;
  fADCPedScaleFactor = 1.0;
  fADCDiagCut = 50.0;
  fCosmicFlag=0;

  fSampThreshold = 5.;
  fSampNSA = 0; // use value stored in event 125 info
  fSampNSB = 0; // use value stored in event 125 info
  fSampNSAT = 2; // default value in THcRawHit::SetF250Params
  fOutputSampWaveform = 0; // 0= no output , 1 = output Sample Waveform
  fUseSampWaveform = 0; // 0= do not use , 1 = use Sample Waveform

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  if (fCosmicFlag==1) cout << " setup for cosmics in scint plane"<< endl;
  // cout << " cosmic flag = " << fCosmicFlag << endl;
  // fetch the parameter from the temporary list

  // Retrieve parameters we need from parent class
  // Common for all planes

  THcHodoscope* parent = (THcHodoscope*)GetParent();
  fHodoSlop= parent->GetHodoSlop(fPlaneNum-1);
  fTdcOffset= parent->GetTdcOffset(fPlaneNum-1);
  fAdcTdcOffset= parent->GetAdcTdcOffset(fPlaneNum-1);
  fScinTdcMin=parent->GetTdcMin();
  fScinTdcMax=parent->GetTdcMax();
  fScinTdcToTime=parent->GetTdcToTime();
  fTofTolerance=parent->GetTofTolerance();
  fBetaNominal=parent->GetBetaNominal();
  fStartTimeCenter=parent->GetStartTimeCenter();
  fStartTimeSlop=parent->GetStartTimeSlop();
  // Parameters for this plane
  fHodoPosMinPh = new Double_t[fNelem];
  fHodoNegMinPh = new Double_t[fNelem];
  fHodoPosPhcCoeff = new Double_t[fNelem];
  fHodoNegPhcCoeff = new Double_t[fNelem];
  fHodoPosTimeOffset = new Double_t[fNelem];
  fHodoNegTimeOffset = new Double_t[fNelem];
  fHodoVelLight = new Double_t[fNelem];
  fHodoPosInvAdcOffset = new Double_t[fNelem];
  fHodoNegInvAdcOffset = new Double_t[fNelem];
  fHodoPosInvAdcLinear = new Double_t[fNelem];
  fHodoNegInvAdcLinear = new Double_t[fNelem];
  fHodoPosAdcTimeWindowMin = new Double_t[fNelem];
  fHodoNegAdcTimeWindowMin = new Double_t[fNelem];
  fHodoPosAdcTimeWindowMax = new Double_t[fNelem];
  fHodoNegAdcTimeWindowMax = new Double_t[fNelem];
  fHodoPosInvAdcAdc = new Double_t[fNelem];
  fHodoNegInvAdcAdc = new Double_t[fNelem];
  fHodoSigma = new Double_t[fNelem];
  
  //New Time-Walk Calibration Parameters
  fHodoVelFit=new Double_t [fNelem];
  fHodoCableFit=new Double_t [fNelem];
  fHodo_LCoeff=new Double_t [fNelem];
  fHodoPos_c1=new Double_t [fNelem];
  fHodoNeg_c1=new Double_t [fNelem];
  fHodoPos_c2=new Double_t [fNelem];
  fHodoNeg_c2=new Double_t [fNelem];

  for(Int_t j=0;j<(Int_t) fNelem;j++) {
    Int_t index=parent->GetScinIndex(fPlaneNum-1,j);
    fHodoPosAdcTimeWindowMin[j] = parent->GetHodoPosAdcTimeWindowMin(index);
    fHodoPosAdcTimeWindowMax[j] = parent->GetHodoPosAdcTimeWindowMax(index);
    fHodoNegAdcTimeWindowMin[j] = parent->GetHodoNegAdcTimeWindowMin(index);
    fHodoNegAdcTimeWindowMax[j] = parent->GetHodoNegAdcTimeWindowMax(index);
    fHodoPosMinPh[j] = parent->GetHodoPosMinPh(index);
    fHodoNegMinPh[j] = parent->GetHodoNegMinPh(index);
    fHodoPosPhcCoeff[j] = parent->GetHodoPosPhcCoeff(index);
    fHodoNegPhcCoeff[j] = parent->GetHodoNegPhcCoeff(index);
    fHodoPosTimeOffset[j] = parent->GetHodoPosTimeOffset(index);
    fHodoNegTimeOffset[j] = parent->GetHodoNegTimeOffset(index);
    fHodoPosInvAdcOffset[j] = parent->GetHodoPosInvAdcOffset(index);
    fHodoNegInvAdcOffset[j] = parent->GetHodoNegInvAdcOffset(index);
    fHodoPosInvAdcLinear[j] = parent->GetHodoPosInvAdcLinear(index);
    fHodoNegInvAdcLinear[j] = parent->GetHodoNegInvAdcLinear(index);
    fHodoPosInvAdcAdc[j] = parent->GetHodoPosInvAdcAdc(index);
    fHodoNegInvAdcAdc[j] = parent->GetHodoNegInvAdcAdc(index);
    fHodoVelLight[j] = parent->GetHodoVelLight(index);
    //Get Time-Walk correction param
    fHodoVelFit[j] = parent->GetHodoVelFit(index);
    fHodoCableFit[j] = parent->GetHodoCableFit(index);
    fHodo_LCoeff[j] =  parent->GetHodoLCoeff(index);
    fHodoPos_c1[j] = parent->GetHodoPos_c1(index);
    fHodoNeg_c1[j] = parent->GetHodoNeg_c1(index);
    fHodoPos_c2[j] = parent->GetHodoPos_c2(index);
    fHodoNeg_c2[j] = parent->GetHodoNeg_c2(index);
   
    Double_t possigma = parent->GetHodoPosSigma(index);
    Double_t negsigma = parent->GetHodoNegSigma(index);
    fHodoSigma[j] = TMath::Sqrt(possigma*possigma+negsigma*negsigma)/2.0;
    

  

  }

  fTdc_Thrs = parent->GetTDCThrs();
  // cout <<" plane num = "<<fPlaneNum<<endl;
  // cout <<" nelem     = "<<fNelem<<endl;
  // cout <<" zpos      = "<<fZpos<<endl;
  // cout <<" dzpos     = "<<fDzpos<<endl;
  // cout <<" spacing   = "<<fSpacing<<endl;
  // cout <<" size      = "<<fSize<<endl;
  // cout <<" hodoslop  = "<<fHodoSlop<<endl;
  // cout <<"PosLeft = "<<fPosLeft<<endl;
  // cout <<"PosRight = "<<fPosRight<<endl;
  // cout <<"PosOffset = "<<fPosOffset<<endl;
  // cout <<"PosCenter[0] = "<<fPosCenter[0]<<endl;

  // Think we will make special methods to pass most
  // How generic do we want to make this class?
  // The way we get parameter data is going to be pretty specific to
  // our parameter file naming conventions.  But on the other hand,
  // the Hall A analyzer read database is pretty specific.
  // Is there any way for this class to know which spectrometer he
  // belongs too?

  // Create arrays to hold results here
  InitializePedestals();


  fNumGoodPosAdcHits     = vector<Int_t> (fNelem, 0.0);
  fNumGoodNegAdcHits     = vector<Int_t> (fNelem, 0.0);
  fNumGoodPosTdcHits     = vector<Int_t> (fNelem, 0.0);
  fNumGoodNegTdcHits     = vector<Int_t> (fNelem, 0.0);

  fGoodPosAdcPed         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPed         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcMult         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcMult         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcHitUsed         = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcHitUsed         = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseAmp    = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseAmp    = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseInt    = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseInt    = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcPulseTime   = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcPulseTime   = vector<Double_t> (fNelem, 0.0);
  fGoodPosAdcTdcDiffTime   = vector<Double_t> (fNelem, 0.0);
  fGoodNegAdcTdcDiffTime   = vector<Double_t> (fNelem, 0.0);

  fGoodPosTdcTimeUnCorr        = vector<Double_t> (fNelem, 0.0);
  fGoodNegTdcTimeUnCorr        = vector<Double_t> (fNelem, 0.0);
  fGoodPosTdcTimeCorr    = vector<Double_t> (fNelem, 0.0);
  fGoodNegTdcTimeCorr    = vector<Double_t> (fNelem, 0.0);
  fGoodPosTdcTimeTOFCorr = vector<Double_t> (fNelem, 0.0);
  fGoodNegTdcTimeTOFCorr = vector<Double_t> (fNelem, 0.0);
  fGoodPosTdcTimeWalkCorr = vector<Double_t> (fNelem, 0.0);
  fGoodNegTdcTimeWalkCorr = vector<Double_t> (fNelem, 0.0);
  fGoodDiffDistTrack = vector<Double_t> (fNelem, 0.0);
  return kOK;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::DefineVariables( EMode mode )
{
  /**
    Initialize global variables for histograms and Root tree
  */

  // cout << "THcScintillatorPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  if (fDebugAdc) {
    RVarDef vars[] = {
      {"posAdcErrorFlag", "Error Flag for When FPGA Fails", "frPosAdcErrorFlag.THcSignalHit.GetData()"},
      {"negAdcErrorFlag", "Error Flag for When FPGA Fails", "frNegAdcErrorFlag.THcSignalHit.GetData()"},

      {"posTdcTimeRaw",      "List of positive raw TDC values.",           "frPosTdcTimeRaw.THcSignalHit.GetData()"},
      {"posAdcPedRaw",       "List of positive raw ADC pedestals",         "frPosAdcPedRaw.THcSignalHit.GetData()"},
      {"posAdcPulseIntRaw",  "List of positive raw ADC pulse integrals.",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"posAdcPulseAmpRaw",  "List of positive raw ADC pulse amplitudes.", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"posAdcPulseTimeRaw", "List of positive raw ADC pulse times.",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"},

      {"posTdcTime",         "List of positive TDC values.",               "frPosTdcTime.THcSignalHit.GetData()"},
      {"posAdcPed",          "List of positive ADC pedestals",             "frPosAdcPed.THcSignalHit.GetData()"},
      {"posAdcPulseInt",     "List of positive ADC pulse integrals.",      "frPosAdcPulseInt.THcSignalHit.GetData()"},
      {"posAdcPulseAmp",     "List of positive ADC pulse amplitudes.",     "frPosAdcPulseAmp.THcSignalHit.GetData()"},
      {"posAdcPulseTime",    "List of positive ADC pulse times.",          "frPosAdcPulseTime.THcSignalHit.GetData()"},

      {"posAdcSampPedRaw",       "Positive Raw Samp ADC pedestals",        "frPosAdcSampPedRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseIntRaw",  "Positive Raw Samp ADC pulse integrals",  "frPosAdcSampPulseIntRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseAmpRaw",  "Positive Raw Samp ADC pulse amplitudes", "frPosAdcSampPulseAmpRaw.THcSignalHit.GetData()"},
      {"posAdcSampPulseTimeRaw", "Positive Raw Samp ADC pulse times",      "frPosAdcSampPulseTimeRaw.THcSignalHit.GetData()"},
      {"posAdcSampPed",          "Positive Samp ADC pedestals",            "frPosAdcSampPed.THcSignalHit.GetData()"},
      {"posAdcSampPulseInt",     "Positive Samp ADC pulse integrals",      "frPosAdcSampPulseInt.THcSignalHit.GetData()"},
      {"posAdcSampPulseAmp",     "Positive Samp ADC pulse amplitudes",     "frPosAdcSampPulseAmp.THcSignalHit.GetData()"},
      {"posAdcSampPulseTime",    "Positive Samp ADC pulse times",          "frPosAdcSampPulseTime.THcSignalHit.GetData()"},

      {"negTdcTimeRaw",      "List of negative raw TDC values.",           "frNegTdcTimeRaw.THcSignalHit.GetData()"},
      {"negAdcPedRaw",       "List of negative raw ADC pedestals",         "frNegAdcPedRaw.THcSignalHit.GetData()"},
      {"negAdcPulseIntRaw",  "List of negative raw ADC pulse integrals.",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"},
      {"negAdcPulseAmpRaw",  "List of negative raw ADC pulse amplitudes.", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"},
      {"negAdcPulseTimeRaw", "List of negative raw ADC pulse times.",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"},

      {"negTdcTime",         "List of negative TDC values.",               "frNegTdcTime.THcSignalHit.GetData()"},
      {"negAdcPed",          "List of negative ADC pedestals",             "frNegAdcPed.THcSignalHit.GetData()"},
      {"negAdcPulseInt",     "List of negative ADC pulse integrals.",      "frNegAdcPulseInt.THcSignalHit.GetData()"},
      {"negAdcPulseAmp",     "List of negative ADC pulse amplitudes.",     "frNegAdcPulseAmp.THcSignalHit.GetData()"},
      {"negAdcPulseTime",    "List of negative ADC pulse times.",          "frNegAdcPulseTime.THcSignalHit.GetData()"},

      {"negAdcSampPedRaw",       "Negative Raw Samp ADC pedestals",        "frNegAdcSampPedRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseIntRaw",  "Negative Raw Samp ADC pulse integrals",  "frNegAdcSampPulseIntRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseAmpRaw",  "Negative Raw Samp ADC pulse amplitudes", "frNegAdcSampPulseAmpRaw.THcSignalHit.GetData()"},
      {"negAdcSampPulseTimeRaw", "Negative Raw Samp ADC pulse times",      "frNegAdcSampPulseTimeRaw.THcSignalHit.GetData()"},
      {"negAdcSampPed",          "Negative Samp ADC pedestals",            "frNegAdcSampPed.THcSignalHit.GetData()"},
      {"negAdcSampPulseInt",     "Negative Samp ADC pulse integrals",      "frNegAdcSampPulseInt.THcSignalHit.GetData()"},
      {"negAdcSampPulseAmp",     "Negative Samp ADC pulse amplitudes",     "frNegAdcSampPulseAmp.THcSignalHit.GetData()"},
      {"negAdcSampPulseTime",    "Negative Samp ADC pulse times",          "frNegAdcSampPulseTime.THcSignalHit.GetData()"},

      {"totNumPosAdcHits", "Total Number of Positive ADC Hits",   "fTotNumPosAdcHits"}, // Hodo+ raw ADC multiplicity Int_t
      {"totNumNegAdcHits", "Total Number of Negative ADC Hits",   "fTotNumNegAdcHits"}, // Hodo- raw ADC multiplicity  ""
      {"totNumAdcHits",   "Total Number of PMTs Hit (as measured by ADCs)",      "fTotNumAdcHits"},    // Hodo raw ADC multiplicity  ""

      {"totNumPosTdcHits", "Total Number of Positive TDC Hits",   "fTotNumPosTdcHits"}, // Hodo+ raw TDC multiplicity  ""
      {"totNumNegTdcHits", "Total Number of Negative TDC Hits",   "fTotNumNegTdcHits"}, // Hodo- raw TDC multiplicity  ""
      {"totNumTdcHits",   "Total Number of PMTs Hits (as measured by TDCs)",      "fTotNumTdcHits"},    // Hodo raw TDC multiplicity  ""
      { 0 }
    };
    DefineVarsFromList( vars, mode);
  } //end debug statement

  if (fOutputSampWaveform==1) {
  RVarDef vars[] = {
    {"adcNegSampWaveform",          "FADC Neg ADCSample Waveform",           "fNegAdcSampWaveform"},
    {"adcPosSampWaveform",          "FADC Pos ADCSample Waveform",           "fPosAdcSampWaveform"},
      { 0 }
    };
    DefineVarsFromList( vars, mode);
  }

  RVarDef vars[] = {
    {"nhits", "Number of paddle hits (passed TDC && ADC Min and Max cuts for either end)",           "GetNScinHits() "},

    {"posTdcCounter", "List of positive TDC counter numbers.", "frPosTdcTimeRaw.THcSignalHit.GetPaddleNumber()"},   //Hodo+ raw TDC occupancy
    {"posAdcCounter", "List of positive ADC counter numbers.", "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"}, //Hodo+ raw ADC occupancy
    {"negTdcCounter", "List of negative TDC counter numbers.", "frNegTdcTimeRaw.THcSignalHit.GetPaddleNumber()"},     //Hodo- raw TDC occupancy
    {"negAdcCounter", "List of negative ADC counter numbers.", "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},  //Hodo- raw ADC occupancy

    {"fptime", "Time at focal plane",     "GetFpTime()"},

    {"numGoodPosAdcHits",    "Number of Good Positive ADC Hits Per PMT", "fNumGoodPosAdcHits"},    // Hodo+ good ADC occupancy - vector<Int_t>
    {"numGoodNegAdcHits",    "Number of Good Negative ADC Hits Per PMT", "fNumGoodNegAdcHits"},   // Hodo- good ADC occupancy - vector <Int_t>

    {"numGoodPosTdcHits",    "Number of Good Positive TDC Hits Per PMT", "fNumGoodPosTdcHits"},    // Hodo+ good TDC occupancy - vector<Int_t>
    {"numGoodNegTdcHits",    "Number of Good Negative TDC Hits Per PMT", "fNumGoodNegTdcHits"},   // Hodo- good TDC occupancy - vector <Int_t>


    {"totNumGoodPosAdcHits", "Total Number of Good Positive ADC Hits",   "fTotNumGoodPosAdcHits"}, // Hodo+ good ADC multiplicity - Int_t
    {"totNumGoodNegAdcHits", "Total Number of Good Negative ADC Hits",   "fTotNumGoodNegAdcHits"}, // Hodo- good ADC multiplicity - Int_t
    {"totNumGoodAdcHits",   "TotalNumber of Good ADC Hits Per PMT",      "fTotNumGoodAdcHits"},    // Hodo good ADC multiplicity - Int_t

    {"totNumGoodPosTdcHits", "Total Number of Good Positive TDC Hits",   "fTotNumGoodPosTdcHits"}, // Hodo+ good TDC multiplicity - Int_t
    {"totNumGoodNegTdcHits", "Total Number of Good Negative TDC Hits",   "fTotNumGoodNegTdcHits"}, // Hodo- good TDC multiplicity - Int_t
    {"totNumGoodTdcHits",   "TotalNumber of Good TDC Hits Per PMT",      "fTotNumGoodTdcHits"},    // Hodo good TDC multiplicity - Int_t



    // {"GoodPaddle",         "List of Paddle Numbers (passed TDC && ADC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetPaddleNumber()"},

    {"GoodPosAdcPed",  "List of Positive ADC pedestals (passed TDC && ADC Min and Max cuts for either end)",           "fGoodPosAdcPed"}, //vector<Double_t>
    {"GoodNegAdcPed",  "List of Negative ADC pedestals (passed TDC && ADC Min and Max cuts for either end)",           "fGoodNegAdcPed"}, //vector<Double_t>
    {"GoodPosAdcMult",  "List of Positive ADC Mult (passed TDC && ADC Min and Max cuts for either end)",           "fGoodPosAdcMult"}, //vector<Double_t>
    {"GoodNegAdcMult",  "List of Negative ADC Mult (passed TDC && ADC Min and Max cuts for either end)",           "fGoodNegAdcMult"}, //vector<Double_t>
    {"GoodPosAdcHitUsed",  "List of Positive ADC Hit Used (passed TDC && ADC Min and Max cuts for either end)",           "fGoodPosAdcHitUsed"}, //vector<Double_t>
    {"GoodNegAdcHitUsed",  "List of Negative ADC Hit Used (passed TDC && ADC Min and Max cuts for either end)",           "fGoodNegAdcHitUsed"}, //vector<Double_t>

    {"GoodNegTdcTimeUnCorr",  "List of negative TDC values (passed TDC && ADC Min and Max cuts for either end)",        "fGoodNegTdcTimeUnCorr"},  //Units ns
    {"GoodNegTdcTimeCorr",    "List of negative corrected TDC values (corrected for PMT offset and ADC)",           "fGoodNegTdcTimeCorr"},
    {"GoodNegTdcTimeTOFCorr", "List of negative corrected TDC values (corrected for TOF)",                       "fGoodNegTdcTimeTOFCorr"},
    {"GoodNegTdcTimeWalkCorr", "List of negative corrected TDC values (corrected for Time-Walk)",                       "fGoodNegTdcTimeWalkCorr"},
    {"GoodNegAdcPulseInt",    "List of negative ADC values (passed TDC && ADC Min and Max cuts for either end)",    "fGoodNegAdcPulseInt"},
    {"GoodPosTdcTimeUnCorr",  "List of positive TDC values (passed TDC && ADC Min and Max cuts for either end)",        "fGoodPosTdcTimeUnCorr"},
    {"GoodPosTdcTimeCorr",    "List of positive corrected TDC values (corrected for PMT offset and ADC)",           "fGoodPosTdcTimeCorr"},
    {"GoodPosTdcTimeTOFCorr", "List of positive corrected TDC values (corrected for TOF)",                       "fGoodPosTdcTimeTOFCorr"},
    {"GoodPosTdcTimeWalkCorr", "List of positive corrected TDC values (corrected for Time-Walk)",                       "fGoodPosTdcTimeWalkCorr"},
    {"GoodPosAdcPulseInt",    "List of positive ADC values (passed TDC && ADC Min and Max cuts for either end)",    "fGoodPosAdcPulseInt"},
    {"GoodPosAdcPulseAmp",    "List of positive ADC peak amp (passed TDC && ADC Min and Max cuts for either end)",  "fGoodPosAdcPulseAmp"},
    {"GoodNegAdcPulseAmp",    "List of Negative ADC peak amp (passed TDC && ADC Min and Max cuts for either end)",  "fGoodNegAdcPulseAmp"},
    {"GoodPosAdcPulseTime",   "List of positive ADC time (passed TDC && ADC Min and Max cuts for either end)", "fGoodPosAdcPulseTime"},
    {"GoodNegAdcPulseTime",   "List of Negative ADC time (passed TDC && ADC Min and Max cuts for either end)", "fGoodNegAdcPulseTime"},
    {"GoodPosAdcTdcDiffTime",   "List of positive TDC - ADC time (passed TDC && ADC Min and Max cuts for either end)", "fGoodPosAdcTdcDiffTime"},
    {"GoodNegAdcTdcDiffTime",   "List of Negative TDC - ADC time (passed TDC && ADC Min and Max cuts for either end)", "fGoodNegAdcTdcDiffTime"},
    {"DiffDisTrack",   "Difference between track and scintillator position (cm)", "fHitDistance"},
    {"DiffDisTrackCorr",   "TW Corrected Dist Difference between track and scintillator position (cm)", "fGoodDiffDistTrack"},
    {"TrackXPos",   "Track X position at plane (cm)", "fTrackXPosition"},
    {"TrackYPos",   "Track Y position at plane (cm)", "fTrackYPosition"},
    {"ScinXPos",   "Scint Average Y position at plane (cm)", "fScinXPos"},
    {"ScinYPos",   "Scint Average Xposition at plane (cm)", "fScinYPos"},
    {"NumClus",   "Number of clusters", "fNumberClusters"},
    {"Clus.Pos",   "Position of each paddle clusters", "fCluster.THcScintPlaneCluster.GetClusterPosition()"},
    {"Clus.Size",   "Size of each paddle clusters", "fCluster.THcScintPlaneCluster.GetClusterSize()"},
    {"Clus.Flag",   "Flag of each paddle clusters", "fCluster.THcScintPlaneCluster.GetClusterFlag()"},
    {"Clus.UsedFlag",   "USed Flag of each paddle clusters", "fCluster.THcScintPlaneCluster.GetClusterUsedFlag()"},
    {"PosTdcRefTime",   "Reference time of Pos TDC", "fPosTdcRefTime"},
    {"NegTdcRefTime",   "Reference time of Neg TDC", "fNegTdcRefTime"},
    {"PosAdcRefTime",   "Reference time of Pos ADC", "fPosAdcRefTime"},
    {"NegAdcRefTime",   "Reference time of Neg aDC", "fNegAdcRefTime"},
    {"PosTdcRefDiffTime",   "Reference Diff time of Pos TDC", "fPosTdcRefDiffTime"},
    {"NegTdcRefDiffTime",   "Reference Diff time of Neg TDC", "fNegTdcRefDiffTime"},
    {"PosAdcRefDiffTime",   "Reference Diff time of Pos ADC", "fPosAdcRefDiffTime"},
    {"NegAdcRefDiffTime",   "Reference Diff time of Neg aDC", "fNegAdcRefDiffTime"},
   //{"ngoodhits", "Number of paddle hits (passed tof tolerance and used to determine the focal plane time )",           "GetNGoodHits() "},
    { 0 }
  };

  return DefineVarsFromList(vars, mode);

}
//_____________________________________________________________________________
void THcScintillatorPlane::Clear( Option_t* )
{
  /*! \brief Clears fHodoHits,frPosTDCHits,frNegTDCHits,frPosADCHits,frNegADCHits
   *
   * -  Clears fHodoHits,frPosTDCHits,frNegTDCHits,frPosADCHits,frNegADCHits
   */
  //cout << " Calling THcScintillatorPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fCluster->Clear();

  frPosAdcErrorFlag->Clear();
  frNegAdcErrorFlag->Clear();

  fHodoHits->Clear();
  frPosTDCHits->Clear();
  frNegTDCHits->Clear();
  frPosADCHits->Clear();
  frNegADCHits->Clear();

  frPosTdcTimeRaw->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosTdcTime->Clear();
  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();
  frPosAdcPulseTime->Clear();

  frPosAdcSampPedRaw->Clear();
  frPosAdcSampPulseIntRaw->Clear();
  frPosAdcSampPulseAmpRaw->Clear();
  frPosAdcSampPulseTimeRaw->Clear();
  frPosAdcSampPed->Clear();
  frPosAdcSampPulseInt->Clear();
  frPosAdcSampPulseAmp->Clear();
  frPosAdcSampPulseTime->Clear();

  frNegTdcTimeRaw->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegTdcTime->Clear();
  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();
  frNegAdcPulseTime->Clear();

  frNegAdcSampPedRaw->Clear();
  frNegAdcSampPulseIntRaw->Clear();
  frNegAdcSampPulseAmpRaw->Clear();
  frNegAdcSampPulseTimeRaw->Clear();
  frNegAdcSampPed->Clear();
  frNegAdcSampPulseInt->Clear();
  frNegAdcSampPulseAmp->Clear();
  frNegAdcSampPulseTime->Clear();


  //Clear occupancies
  for (UInt_t ielem = 0; ielem < fNumGoodPosAdcHits.size(); ielem++)
    fNumGoodPosAdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumGoodNegAdcHits.size(); ielem++)
    fNumGoodNegAdcHits.at(ielem) = 0;

  for (UInt_t ielem = 0; ielem < fNumGoodPosTdcHits.size(); ielem++)
    fNumGoodPosTdcHits.at(ielem) = 0;
  for (UInt_t ielem = 0; ielem < fNumGoodNegTdcHits.size(); ielem++)
    fNumGoodNegTdcHits.at(ielem) = 0;

  //Clear Ped/Amps/Int/Time
  for (UInt_t ielem = 0; ielem < fGoodPosAdcPed.size(); ielem++) {
    fGoodPosAdcPed.at(ielem)         = 0.0;
    fGoodPosAdcMult.at(ielem)         = 0.0;
    fGoodPosAdcHitUsed.at(ielem)         = 0.0;
    fGoodPosAdcPulseInt.at(ielem)    = 0.0;
    fGoodPosAdcPulseAmp.at(ielem)    = 0.0;
    fGoodPosAdcPulseTime.at(ielem)   = kBig;
    fGoodPosAdcTdcDiffTime.at(ielem)   = kBig;
  }
  for (UInt_t ielem = 0; ielem < fGoodNegAdcPed.size(); ielem++) {
    fGoodNegAdcPed.at(ielem)         = 0.0;
    fGoodNegAdcMult.at(ielem)         = 0.0;
    fGoodNegAdcHitUsed.at(ielem)         = 0.0;
    fGoodNegAdcPulseInt.at(ielem)    = 0.0;
    fGoodNegAdcPulseAmp.at(ielem)    = 0.0;
    fGoodNegAdcPulseTime.at(ielem)   = kBig;
    fGoodNegAdcTdcDiffTime.at(ielem)   = kBig;
  }

  //Clear Good TDC Variables
  for (UInt_t ielem = 0; ielem < fGoodPosTdcTimeUnCorr.size(); ielem++) {
    fGoodPosTdcTimeUnCorr.at(ielem)          = kBig;
    fGoodPosTdcTimeCorr.at(ielem)      = kBig;
    fGoodPosTdcTimeTOFCorr.at(ielem)   = kBig;
    fGoodPosTdcTimeWalkCorr.at(ielem)  = kBig;
  }

  for (UInt_t ielem = 0; ielem < fGoodNegTdcTimeUnCorr.size(); ielem++) {
    fGoodNegTdcTimeUnCorr.at(ielem)          = kBig;
    fGoodNegTdcTimeCorr.at(ielem)      = kBig;
    fGoodNegTdcTimeTOFCorr.at(ielem)   = kBig;
    fGoodNegTdcTimeWalkCorr.at(ielem)  = kBig;
  }

   for (UInt_t ielem = 0; ielem < fGoodDiffDistTrack.size(); ielem++) {
    fGoodDiffDistTrack.at(ielem) = kBig;
   }

  fpTime = -1.e4;
  fHitDistance = kBig;
  fScinYPos = kBig;
  fScinXPos = kBig;
  fTrackXPosition = kBig;
  fTrackYPosition = kBig;
  fNumberClusters=0;
  fPosTdcRefTime = kBig;
  fPosAdcRefTime = kBig;
  fNegTdcRefTime = kBig;
  fNegAdcRefTime = kBig;
  fPosTdcRefDiffTime = kBig;
  fPosAdcRefDiffTime = kBig;
  fNegTdcRefDiffTime = kBig;
  fNegAdcRefDiffTime = kBig;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::Decode( const THaEvData& evdata )
{
  /*! \brief Does nothing. Data decode in  THcScintillatorPlane::Processhits which is called by THcHodoscope::Decode
   */
  cout << " Calling THcScintillatorPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::CoarseProcess( TClonesArray& tracks )
{
  /*! \brief Does nothing
   */

  cout <<"*******************************\n";
  cout <<"NOW IN THcScintilatorPlane::CoarseProcess!!!!\n";
  cout <<"*******************************\n";
  //  HitCount();

  return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::FineProcess( TClonesArray& tracks )
{
  /*! \brief Does nothing
   */
  return 0;
}


//_____________________________________________________________________________
Int_t THcScintillatorPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  /*! \brief Extract scintillator paddle hits from raw data starting at "nexthit"
   * - Called by THcHodoscope::Decode
   * - Loops through "rawhits" array  starting at index of "nexthit"
   * - Assumes that the hit list is sorted by plane and looping ends when plane number of hit doesn't match fPlaneNum
   * - Fills THcSignalHit objects frPosTDCHits and frNegTDCHits when TDC > 0
   * - Fills THcSignalHit objects frPosADCHits and frNegADCHit with pedestal subtracted ADC when value larger than 50
   * - For hits that have TDC value for either positive or negative PMT within  fScinTdcMin and fScinTdcMax
   *  + Creates new  fHodoHits[fNScinHits] =  THcHodoHit
   *  + Calculates pulse height correction to the positive and negative PMT times
   *  + Correct times for time traveled in paddle
   *  + Correct times for time of flight using beta from central spectrometer momentum and particle type
   *  + Calls  SetCorrectedTime method of THcHodoHit
   *  + Increments fNScinHits
   * - Returns value of nexthit + number of hits processed
   *
   */
  //raw
  fPosTdcRefTime = kBig;
  fPosAdcRefTime = kBig;
  fNegTdcRefTime = kBig;
  fNegAdcRefTime = kBig;
  fPosTdcRefDiffTime = kBig;
  fPosAdcRefDiffTime = kBig;
  fNegTdcRefDiffTime = kBig;
  fNegAdcRefDiffTime = kBig;
  Int_t nrPosTDCHits=0;
  Int_t nrNegTDCHits=0;
  Int_t nrPosADCHits=0;
  Int_t nrNegADCHits=0;
  UInt_t nrPosAdcHits = 0;
  UInt_t nrNegAdcHits = 0;
  UInt_t nrPosTdcHits = 0;
  UInt_t nrNegTdcHits = 0;
  UInt_t nrSampPosAdcHits = 0;
  UInt_t nrSampNegAdcHits = 0;
  fPosAdcSampWaveform.clear();
  fNegAdcSampWaveform.clear();
  frPosTDCHits->Clear();
  frNegTDCHits->Clear();
  frPosADCHits->Clear();
  frNegADCHits->Clear();
  frPosADCSums->Clear();
  frNegADCSums->Clear();
  frPosADCPeds->Clear();
  frNegADCPeds->Clear();


  frPosTdcTimeRaw->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosTdcTime->Clear();
  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();
  frPosAdcPulseTime->Clear();

  frPosAdcSampPedRaw->Clear();
  frPosAdcSampPulseIntRaw->Clear();
  frPosAdcSampPulseAmpRaw->Clear();
  frPosAdcSampPulseTimeRaw->Clear();
  frPosAdcSampPed->Clear();
  frPosAdcSampPulseInt->Clear();
  frPosAdcSampPulseAmp->Clear();
  frPosAdcSampPulseTime->Clear();

  frNegTdcTimeRaw->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegTdcTime->Clear();
  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();
  frNegAdcPulseTime->Clear();

  frNegAdcSampPedRaw->Clear();
  frNegAdcSampPulseIntRaw->Clear();
  frNegAdcSampPulseAmpRaw->Clear();
  frNegAdcSampPulseTimeRaw->Clear();
  frNegAdcSampPed->Clear();
  frNegAdcSampPulseInt->Clear();
  frNegAdcSampPulseAmp->Clear();
  frNegAdcSampPulseTime->Clear();

  //stripped
  fNScinHits=0;

  fTotNumGoodPosAdcHits = 0;
  fTotNumGoodNegAdcHits = 0;
  fTotNumGoodAdcHits = 0;

  fTotNumPosAdcHits = 0;
  fTotNumNegAdcHits = 0;
  fTotNumAdcHits = 0;


  fTotNumPosTdcHits = 0;
  fTotNumNegTdcHits = 0;
  fTotNumTdcHits = 0;

  fTotNumGoodPosTdcHits = 0;
  fTotNumGoodNegTdcHits = 0;
  fTotNumGoodTdcHits = 0;


  fHodoHits->Clear();
  Int_t nrawhits = rawhits->GetLast()+1;
  //   cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
  Int_t ihit = nexthit;

  // cout << "THcScintillatorPlane: " << GetName() << " raw hits = " << nrawhits << endl;

  // A THcRawHodoHit contains all the information (tdc and adc for both
  // pmts) for a single paddle for a single trigger.  The tdc information
  // might include multiple hits if it uses a multihit tdc.
  // Use "ihit" as the index over THcRawHodoHit objects.  Use
  // "thit" to index over multiple tdc hits within an "ihit".
  Bool_t problem_flag=kFALSE;

  while(ihit < nrawhits) {

    THcRawHodoHit* hit = (THcRawHodoHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t padnum=hit->fCounter;

    Int_t index=padnum-1;


    THcRawTdcHit& rawPosTdcHit = hit->GetRawTdcHitPos();
    if (rawPosTdcHit.GetNHits() >0 && rawPosTdcHit.HasRefTime()) {
      if (fPosTdcRefTime == kBig)  {
	fPosTdcRefTime=rawPosTdcHit.GetRefTime() ;
	fPosTdcRefDiffTime=rawPosTdcHit.GetRefDiffTime() ;
      }
    if (fPosTdcRefTime != rawPosTdcHit.GetRefTime()) {
      // cout <<  "THcScintillatorPlane: " << GetName() << " reftime problem at paddle num = " << padnum << " TDC pos hits = " << rawPosTdcHit.GetNHits() << endl;
        problem_flag=kTRUE;
    }
    }
     // cout << " paddle num = " << padnum << " TDC Pos hits = " << rawPosTdcHit.GetNHits() << endl;
    for (UInt_t thit=0; thit<rawPosTdcHit.GetNHits(); ++thit) {
      ((THcSignalHit*) frPosTdcTimeRaw->ConstructedAt(nrPosTdcHits))->Set(padnum, rawPosTdcHit.GetTimeRaw(thit));
      ((THcSignalHit*) frPosTdcTime->ConstructedAt(nrPosTdcHits))->Set(padnum, rawPosTdcHit.GetTime(thit));
      ++nrPosTdcHits;
      fTotNumTdcHits++;
      fTotNumPosTdcHits++;
    }
    THcRawTdcHit& rawNegTdcHit = hit->GetRawTdcHitNeg();
    if (rawNegTdcHit.GetNHits() >0 && rawNegTdcHit.HasRefTime()) {
      if (fNegTdcRefTime == kBig) {
	fNegTdcRefTime=rawNegTdcHit.GetRefTime() ;
	fNegTdcRefDiffTime=rawNegTdcHit.GetRefDiffTime() ;
      }
    if (fNegTdcRefTime != rawNegTdcHit.GetRefTime()) {
        // cout <<  "THcScintillatorPlane: " << GetName()<< " Neg TDC reftime problem at paddle num = " << padnum << " TDC neg hits = " << rawNegTdcHit.GetNHits() << endl;
        problem_flag=kTRUE;
    }
    }
     // cout << " paddle num = " << padnum << " TDC Neg hits = " << rawNegTdcHit.GetNHits() << endl;
    for (UInt_t thit=0; thit<rawNegTdcHit.GetNHits(); ++thit) {
      ((THcSignalHit*) frNegTdcTimeRaw->ConstructedAt(nrNegTdcHits))->Set(padnum, rawNegTdcHit.GetTimeRaw(thit));
      ((THcSignalHit*) frNegTdcTime->ConstructedAt(nrNegTdcHits))->Set(padnum, rawNegTdcHit.GetTime(thit));
      ++nrNegTdcHits;
      fTotNumTdcHits++;
      fTotNumNegTdcHits++;
    }
    
    THcRawAdcHit& rawPosAdcHit = hit->GetRawAdcHitPos();

    if ( (rawPosAdcHit.GetNPulses() >0 || rawPosAdcHit.GetNSamples() >0) && rawPosAdcHit.HasRefTime()) {
      if (fPosAdcRefTime == kBig  ) {
	fPosAdcRefTime=rawPosAdcHit.GetRefTime() ;
	fPosAdcRefDiffTime=rawPosAdcHit.GetRefDiffTime() ;
      }
    if (fPosAdcRefTime != rawPosAdcHit.GetRefTime()) {
      // cout <<  "THcScintillatorPlane: " << GetName()<< " Pos ADC reftime problem at paddle num = " << padnum << " ADC pos hits = " << rawPosAdcHit.GetNPulses() << endl;
        problem_flag=kTRUE;
      }
    }
     // cout << " paddle num = " << padnum << " ADC Pos hits = " << rawPosAdcHit.GetNPulses() << endl;
    if ( fUseSampWaveform == 0 ) {
      
      for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
	((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPedRaw());
	((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPed());
	
	((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseIntRaw(thit));
	((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseInt(thit));
	
	((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmpRaw(thit));
	
	((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseAmp(thit));
	
	((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTimeRaw(thit));
	((THcSignalHit*) frPosAdcPulseTime->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetPulseTime(thit)+fAdcTdcOffset);
	
	if (rawPosAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 0);
	if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 1);
	if (rawPosAdcHit.GetPulseAmpRaw(thit) <= 0 && rawPosAdcHit.GetNSamples() >0) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 2);
	
	++nrPosAdcHits;
	fTotNumAdcHits++;
	fTotNumPosAdcHits++;
      }
    }
    //
     // cout << " paddle num = " << padnum << "Pos ADC GetNSamples = " << rawPosAdcHit.GetNSamples() << endl;
    if (rawPosAdcHit.GetNSamples()>0 ) {
      rawPosAdcHit.SetSampThreshold(fSampThreshold);
      if (fSampNSA == 0) fSampNSA=rawPosAdcHit.GetF250_NSA();
      if (fSampNSB == 0) fSampNSB=rawPosAdcHit.GetF250_NSB();
      rawPosAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
      if (fSampNSAT != 2) rawPosAdcHit.SetSampNSAT(fSampNSAT);
      rawPosAdcHit.SetSampIntTimePedestalPeak();
      fPosAdcSampWaveform.push_back(float(padnum));
      fPosAdcSampWaveform.push_back(float(rawPosAdcHit.GetNSamples()));
      for (UInt_t thit = 0; thit < rawPosAdcHit.GetNSamples(); thit++) {
	fPosAdcSampWaveform.push_back(rawPosAdcHit.GetSample(thit)); // ped subtracted sample (mV)
      }
      for (UInt_t thit = 0; thit < rawPosAdcHit.GetNSampPulses(); thit++) {
	((THcSignalHit*) frPosAdcSampPedRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPedRaw());
	((THcSignalHit*) frPosAdcSampPed->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPed());
	
	((THcSignalHit*) frPosAdcSampPulseIntRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseIntRaw(thit));
	((THcSignalHit*) frPosAdcSampPulseInt->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseInt(thit));
	
	((THcSignalHit*) frPosAdcSampPulseAmpRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseAmpRaw(thit));
	((THcSignalHit*) frPosAdcSampPulseAmp->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseAmp(thit));
	((THcSignalHit*) frPosAdcSampPulseTimeRaw->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTimeRaw(thit));
	((THcSignalHit*) frPosAdcSampPulseTime->ConstructedAt(nrSampPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);
	//
	if ( rawPosAdcHit.GetNPulses() ==0 || fUseSampWaveform ==1 ) {
	  ((THcSignalHit*) frPosAdcPedRaw->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPedRaw());
	  ((THcSignalHit*) frPosAdcPed->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPed());
	  
	  ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseIntRaw(thit));
	  ((THcSignalHit*) frPosAdcPulseInt->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseInt(thit));
	  
	  ((THcSignalHit*) frPosAdcPulseAmpRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseAmpRaw(thit));
	  ((THcSignalHit*) frPosAdcPulseAmp->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseAmp(thit) );
	  
	  ((THcSignalHit*) frPosAdcPulseTimeRaw->ConstructedAt(nrPosAdcHits))->Set(padnum,rawPosAdcHit.GetSampPulseTimeRaw(thit) );
	  ((THcSignalHit*) frPosAdcPulseTime->ConstructedAt(nrPosAdcHits))->Set(padnum, rawPosAdcHit.GetSampPulseTime(thit)+fAdcTdcOffset);
	  ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 3);  
	  if (fUseSampWaveform ==1) ((THcSignalHit*) frPosAdcErrorFlag->ConstructedAt(nrPosAdcHits))->Set(padnum, 0);  

	  ++nrPosAdcHits;
	  fTotNumPosAdcHits++;
	  fTotNumAdcHits++;
	}
	++nrSampPosAdcHits;
      }	
    }
    

    THcRawAdcHit& rawNegAdcHit = hit->GetRawAdcHitNeg();

      // cout << " paddle num = " << padnum << " ADC Neg hits = " << rawNegAdcHit.GetNPulses() << endl;
   if ( (rawNegAdcHit.GetNPulses()>0 || rawNegAdcHit.GetNSamples()>0) && rawNegAdcHit.HasRefTime()) {
      if (fNegAdcRefTime == kBig) {
	fNegAdcRefTime=rawNegAdcHit.GetRefTime() ;
	fNegAdcRefDiffTime=rawNegAdcHit.GetRefDiffTime() ;
      }
    if (fNegAdcRefTime != rawNegAdcHit.GetRefTime()) {
      cout <<  "THcScintillatorPlane: " << GetName()<< " Neg ADC reftime problem at paddle num = " << padnum << " TDC pos hits = " << rawNegAdcHit.GetNPulses() << endl;
        problem_flag=kTRUE;
      }
    }

    if ( fUseSampWaveform == 0 ) {
      for (UInt_t thit=0; thit<rawNegAdcHit.GetNPulses(); ++thit) {
	// cout << "neg adc  thit = " << thit << endl;
	
	((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPedRaw());
	((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPed());
	
	((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseIntRaw(thit));
	((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseInt(thit));
	
	((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmpRaw(thit));
	((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseAmp(thit));
	((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTimeRaw(thit));
	((THcSignalHit*) frNegAdcPulseTime->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetPulseTime(thit)+fAdcTdcOffset);
	
	if (rawNegAdcHit.GetPulseAmpRaw(thit) > 0)  ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 0);
	if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 1);
	if (rawNegAdcHit.GetPulseAmpRaw(thit) <= 0 && rawNegAdcHit.GetNSamples() >0) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 2);
	
	++nrNegAdcHits;
	fTotNumAdcHits++;
	fTotNumNegAdcHits++;
      }
    }

     // cout << " paddle num = " << padnum << "Neg ADC GetNSamples = " << rawNegAdcHit.GetNSamples() << endl;
    if (rawNegAdcHit.GetNSamples()>0 ) {
      rawNegAdcHit.SetSampThreshold(fSampThreshold);
      if (fSampNSA == 0) fSampNSA=rawNegAdcHit.GetF250_NSA();
      if (fSampNSB == 0) fSampNSB=rawNegAdcHit.GetF250_NSB();
      rawNegAdcHit.SetF250Params(fSampNSA,fSampNSB,4); // Set NPED =4
      if (fSampNSAT != 2) rawNegAdcHit.SetSampNSAT(fSampNSAT);
      rawNegAdcHit.SetSampIntTimePedestalPeak();
      fNegAdcSampWaveform.push_back(float(padnum));
      fNegAdcSampWaveform.push_back(float(rawNegAdcHit.GetNSamples()));
      for (UInt_t thit = 0; thit < rawNegAdcHit.GetNSamples(); thit++) {
	fNegAdcSampWaveform.push_back(rawNegAdcHit.GetSample(thit)); // ped subtracted sample (mV)
      }
      for (UInt_t thit = 0; thit < rawNegAdcHit.GetNSampPulses(); thit++) {
	((THcSignalHit*) frNegAdcSampPedRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPedRaw());
	((THcSignalHit*) frNegAdcSampPed->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPed());
	((THcSignalHit*) frNegAdcSampPulseIntRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseIntRaw(thit));
	((THcSignalHit*) frNegAdcSampPulseInt->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseInt(thit));
	
	((THcSignalHit*) frNegAdcSampPulseAmpRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseAmpRaw(thit));
	((THcSignalHit*) frNegAdcSampPulseAmp->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseAmp(thit));
	((THcSignalHit*) frNegAdcSampPulseTimeRaw->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTimeRaw(thit));
	((THcSignalHit*) frNegAdcSampPulseTime->ConstructedAt(nrSampNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTime(thit));
	//
	if ( rawNegAdcHit.GetNPulses() ==0 || fUseSampWaveform ==1 ) {
	  ((THcSignalHit*) frNegAdcPedRaw->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPedRaw());
	  ((THcSignalHit*) frNegAdcPed->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPed());
	  
	  ((THcSignalHit*) frNegAdcPulseIntRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseIntRaw(thit));
	  ((THcSignalHit*) frNegAdcPulseInt->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseInt(thit));
	  
	  ((THcSignalHit*) frNegAdcPulseAmpRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseAmpRaw(thit));
	  ((THcSignalHit*) frNegAdcPulseAmp->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseAmp(thit) );
	  
	  ((THcSignalHit*) frNegAdcPulseTimeRaw->ConstructedAt(nrNegAdcHits))->Set(padnum,rawNegAdcHit.GetSampPulseTimeRaw(thit) );
	  ((THcSignalHit*) frNegAdcPulseTime->ConstructedAt(nrNegAdcHits))->Set(padnum, rawNegAdcHit.GetSampPulseTime(thit));
	  ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 3);  
	  if (fUseSampWaveform ==1) ((THcSignalHit*) frNegAdcErrorFlag->ConstructedAt(nrNegAdcHits))->Set(padnum, 0);  
	  ++nrNegAdcHits;
	  fTotNumNegAdcHits++;
	  fTotNumAdcHits++;
	}
	++nrSampNegAdcHits;
      }	
    }
    
     // cout << " Finding tdc hit " << endl;
     // Need to be finding first hit in TDC range, not the first hit overall
    if (hit->GetRawTdcHitPos().GetNHits() > 0)
      ((THcSignalHit*) frPosTDCHits->ConstructedAt(nrPosTDCHits++))->Set(padnum, hit->GetRawTdcHitPos().GetTime()+fTdcOffset);
    if (hit->GetRawTdcHitNeg().GetNHits() > 0)
      ((THcSignalHit*) frNegTDCHits->ConstructedAt(nrNegTDCHits++))->Set(padnum, hit->GetRawTdcHitNeg().GetTime()+fTdcOffset);
    // Should we make lists of offset corrected ADC Pulse times here too?  For now
    // the frNegAdcPulseTime frPosAdcPulseTime have that offset correction.
    //
    Bool_t badcraw_pos=kFALSE;
    Bool_t badcraw_neg=kFALSE;
    Double_t adcped_pos=-999;
    Double_t adcped_neg=-999;
    Int_t adcmult_pos=0;
    Int_t adcmult_neg=0;
    Int_t adchitused_pos=0;
    Int_t adchitused_neg=0;
    Double_t adcint_pos=-999;
    Double_t adcint_neg=-999;
    Double_t adcamp_pos=-kBig;
    Double_t adcamp_neg=-kBig;
    Double_t adctime_pos=kBig;
    Double_t adctime_neg=kBig;
    Double_t adctdcdifftime_pos=kBig;
    Double_t adctdcdifftime_neg=kBig;
    Double_t good_ielem_posadc = -1;
    Double_t good_ielem_negadc = -1;
    Bool_t btdcraw_pos=kFALSE;
    Bool_t btdcraw_neg=kFALSE;
    // Determine good tdc pos and neg times
    Int_t tdc_pos=-999;
    Int_t tdc_neg=-999;
    Double_t good_ielem_postdc = -1;
    Double_t good_ielem_negtdc = -1;
    for(UInt_t thit=0; thit<hit->GetRawTdcHitPos().GetNHits(); thit++) {
      tdc_pos = hit->GetRawTdcHitPos().GetTime(thit)+fTdcOffset;
      if(tdc_pos >= fScinTdcMin && tdc_pos <= fScinTdcMax) {
	btdcraw_pos = kTRUE;
	good_ielem_postdc = thit;
       	break;
      }
    }
    for(UInt_t thit=0; thit<hit->GetRawTdcHitNeg().GetNHits(); thit++) {
      tdc_neg = hit->GetRawTdcHitNeg().GetTime(thit)+fTdcOffset;
      if(tdc_neg >= fScinTdcMin && tdc_neg <= fScinTdcMax) {
	btdcraw_neg = kTRUE;
	good_ielem_negtdc = thit;
       	break;
      }
    }
    //
    if(fADCMode == kADCDynamicPedestal) {
      Int_t good_ielem_negadc_test2=-1;
      Int_t good_ielem_posadc_test2=-1;
     //Loop Here over all hits per event for neg side of plane
     if (good_ielem_negtdc != -1) {
	Double_t max_adcamp_test=-1000.;
	Double_t max_adctdcdiff_test=1000.;
      for (UInt_t ielem=0;ielem<rawNegAdcHit.GetNPulses();ielem++) {
       	Double_t pulseAmp     = rawNegAdcHit.GetPulseAmp(ielem);
	Double_t pulseTime    = rawNegAdcHit.GetPulseTime(ielem)+fAdcTdcOffset;
        Double_t TdcAdcTimeDiff = tdc_neg*fScinTdcToTime-pulseTime;
        if (rawNegAdcHit.GetPulseAmpRaw(ielem) <= 0)pulseAmp= 200.;
	Bool_t   pulseTimeCut =( TdcAdcTimeDiff > fHodoNegAdcTimeWindowMin[index]) &&  (TdcAdcTimeDiff < fHodoNegAdcTimeWindowMax[index]);
	if (pulseTimeCut &&  pulseAmp>max_adcamp_test) {
	  good_ielem_negadc = ielem;
	  max_adcamp_test=pulseAmp;
	}
	if (abs(TdcAdcTimeDiff) < max_adctdcdiff_test) {
	  good_ielem_negadc_test2 = ielem;
	  max_adctdcdiff_test=abs(TdcAdcTimeDiff);
	}
      }
     }

       //
      if ( good_ielem_negadc == -1 &&  good_ielem_negadc_test2 != -1)   good_ielem_negadc=good_ielem_negadc_test2;
      if ( good_ielem_negadc == -1 && good_ielem_negadc_test2 == -1 && rawNegAdcHit.GetNPulses()>0) {
	good_ielem_negadc=0;
      }
      //
      if (good_ielem_negadc != -1 && good_ielem_negadc<rawNegAdcHit.GetNPulses()) {
	  adcped_neg = rawNegAdcHit.GetPed();
	  adcmult_neg = rawNegAdcHit.GetNPulses();
	  adchitused_neg = good_ielem_negadc+1;
	  adcint_neg = rawNegAdcHit.GetPulseInt(good_ielem_negadc);
	  adcamp_neg = rawNegAdcHit.GetPulseAmp(good_ielem_negadc);
	  if (rawNegAdcHit.GetPulseAmpRaw(good_ielem_negadc) <= 0) adcamp_neg= 200.;
	  adctime_neg = rawNegAdcHit.GetPulseTime(good_ielem_negadc)+fAdcTdcOffset;
	  badcraw_neg = kTRUE;
	  adctdcdifftime_neg=tdc_neg*fScinTdcToTime-adctime_neg;
      }
        //
      //Loop Here over all hits per event for pos side of plane
      if (good_ielem_postdc != -1) {
	Double_t max_adcamp_test=-1000.;
	Double_t max_adctdcdiff_test=1000.;
	//
     for (UInt_t ielem=0;ielem<rawPosAdcHit.GetNPulses();ielem++) {
       	Double_t pulseAmp     = rawPosAdcHit.GetPulseAmp(ielem);
	Double_t pulseTime    = rawPosAdcHit.GetPulseTime(ielem)+fAdcTdcOffset;
        Double_t TdcAdcTimeDiff = tdc_pos*fScinTdcToTime-pulseTime;
	Bool_t   pulseTimeCut =( TdcAdcTimeDiff > fHodoPosAdcTimeWindowMin[index]) &&  (TdcAdcTimeDiff < fHodoPosAdcTimeWindowMax[index]);
       if (rawPosAdcHit.GetPulseAmpRaw(ielem) <= 0)pulseAmp= 200.;
	if (pulseTimeCut && pulseAmp>max_adcamp_test) {
	  good_ielem_posadc = ielem;
	  max_adcamp_test=pulseAmp;
	}
	if (abs(TdcAdcTimeDiff) < max_adctdcdiff_test) {
	  good_ielem_posadc_test2 = ielem;
	  max_adctdcdiff_test=abs(TdcAdcTimeDiff);
	}
      }
      }       //
      if ( good_ielem_posadc == -1 &&  good_ielem_posadc_test2 != -1)   good_ielem_posadc=good_ielem_posadc_test2;
       if ( good_ielem_posadc == -1 &&  good_ielem_posadc_test2 == -1 && rawPosAdcHit.GetNPulses()>0)   good_ielem_posadc=0;
     if (good_ielem_posadc != -1 && good_ielem_posadc<rawPosAdcHit.GetNPulses()) {
	  adcped_pos = rawPosAdcHit.GetPed();
	  adcmult_pos = rawPosAdcHit.GetNPulses();
	  adchitused_pos = good_ielem_posadc+1;
	  adcint_pos = rawPosAdcHit.GetPulseInt(good_ielem_posadc);
	  adcamp_pos = rawPosAdcHit.GetPulseAmp(good_ielem_posadc);
	  if (rawPosAdcHit.GetPulseAmpRaw(good_ielem_posadc) <= 0) adcamp_pos= 200.;
	  adctime_pos = rawPosAdcHit.GetPulseTime(good_ielem_posadc)+fAdcTdcOffset;
	  badcraw_pos = kTRUE;
	  adctdcdifftime_pos=tdc_pos*fScinTdcToTime-adctime_pos;
        //
      }
    } else if (fADCMode == kADCSampleIntegral) {
      adcint_pos = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[index];
      adcint_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[index];
      badcraw_pos = badcraw_neg = kTRUE;
    } else if (fADCMode == kADCSampIntDynPed) {
      adcint_pos = hit->GetRawAdcHitPos().GetSampleInt();
      adcint_neg = hit->GetRawAdcHitNeg().GetSampleInt();
      badcraw_pos = badcraw_neg = kTRUE;
    } else {
      adcint_pos = hit->GetRawAdcHitPos().GetPulseIntRaw()-fPosPed[index];
      adcint_neg = hit->GetRawAdcHitNeg().GetPulseIntRaw()-fNegPed[index];
      badcraw_pos = badcraw_neg = kTRUE;
    }
    if (adcint_pos >= fADCDiagCut) {
      ((THcSignalHit*) frPosADCHits->ConstructedAt(nrPosADCHits))->Set(padnum, adcint_pos);
      Double_t samplesum=hit->GetRawAdcHitPos().GetSampleIntRaw();
      Double_t pedestal=hit->GetRawAdcHitPos().GetPedRaw();
      ((THcSignalHit*) frPosADCSums->ConstructedAt(nrPosADCHits))->Set(padnum, samplesum);
      ((THcSignalHit*) frPosADCPeds->ConstructedAt(nrPosADCHits++))->Set(padnum, pedestal);
    }
    if (adcint_neg >= fADCDiagCut) {
      ((THcSignalHit*) frNegADCHits->ConstructedAt(nrNegADCHits))->Set(padnum, adcint_neg);
      Double_t samplesum=hit->GetRawAdcHitNeg().GetSampleIntRaw();
      Double_t pedestal=hit->GetRawAdcHitNeg().GetPedRaw();
      ((THcSignalHit*) frNegADCSums->ConstructedAt(nrNegADCHits))->Set(padnum, samplesum);
      ((THcSignalHit*) frNegADCPeds->ConstructedAt(nrNegADCHits++))->Set(padnum, pedestal);
    }
    //
    if((btdcraw_pos && badcraw_pos) || (btdcraw_neg && badcraw_neg )) {
     if (good_ielem_posadc != -1) {
	//good adc multiplicity
	fTotNumGoodPosAdcHits++;
	fTotNumGoodAdcHits++;
	//good adc occupancy
	fNumGoodPosAdcHits.at(padnum-1) = padnum;
	fGoodPosAdcPed.at(padnum-1)       = adcped_pos;
	fGoodPosAdcMult.at(padnum-1)       = adcmult_pos;
	fGoodPosAdcHitUsed.at(padnum-1)       = adchitused_pos;
	fGoodPosAdcPulseInt.at(padnum-1)  = adcint_pos;
	fGoodPosAdcPulseAmp.at(padnum-1)  = adcamp_pos;
	fGoodPosAdcPulseTime.at(padnum-1) = adctime_pos;
	fGoodPosAdcTdcDiffTime.at(padnum-1) = adctdcdifftime_pos;
      }
      if (good_ielem_negadc != -1) {
	//good adc multiplicity
	fTotNumGoodNegAdcHits++;
	fTotNumGoodAdcHits++;
	//good adc occupancy
	fNumGoodNegAdcHits.at(padnum-1) = padnum;
	fGoodNegAdcPed.at(padnum-1)       = adcped_neg;
	fGoodNegAdcMult.at(padnum-1)       = adcmult_neg;
	fGoodNegAdcHitUsed.at(padnum-1)       = adchitused_neg;
	fGoodNegAdcPulseInt.at(padnum-1)  = adcint_neg;
	fGoodNegAdcPulseAmp.at(padnum-1)  = adcamp_neg;
	fGoodNegAdcPulseTime.at(padnum-1) = adctime_neg;
 	fGoodNegAdcTdcDiffTime.at(padnum-1) = adctdcdifftime_neg;
      }

      //DEFINE THE "GOOD +TDC Multiplicities and Occupancies"
      if (good_ielem_postdc != -1) {
	fTotNumGoodPosTdcHits++;
	fTotNumGoodTdcHits++;
	//good tdc occupancy
	fNumGoodPosTdcHits.at(padnum-1) = padnum;
      }

      //DEFINE THE "GOOD -TDC  Multiplicities and Occupancies"
      if (good_ielem_negtdc != -1) {
	fTotNumGoodNegTdcHits++;
	fTotNumGoodTdcHits++;
	//good tdc occupancy
	fNumGoodNegTdcHits.at(padnum-1) = padnum;
      }
      new( (*fHodoHits)[fNScinHits]) THcHodoHit(tdc_pos, tdc_neg,
						adcint_pos, adcint_neg,
						hit->fCounter, this);
      ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCpeak(adcamp_pos);
      ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCpeak(adcamp_neg);
      ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCtime(adctime_pos);
      ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCtime(adctime_neg);

      //CALCULATE TIME-WALK CORRECTIONS HERE!!!!
      //Define GoodTdcUnCorrTime
      if(btdcraw_pos&&badcraw_pos) {
	fGoodPosTdcTimeUnCorr.at(padnum-1) = tdc_pos*fScinTdcToTime;
	//tw_corr_pos = fHodoPos_c1[padnum-1]/pow(adcamp_pos/fTdc_Thrs,fHodoPos_c2[padnum-1]) -  fHodoPos_c1[padnum-1]/pow(200./fTdc_Thrs, fHodoPos_c2[padnum-1]);
	
	tw_corr_pos =  1./pow(adcamp_pos/fTdc_Thrs,fHodoPos_c2[padnum-1]) -  1./pow(200./fTdc_Thrs, fHodoPos_c2[padnum-1]);

	fGoodPosTdcTimeWalkCorr.at(padnum-1) = tdc_pos*fScinTdcToTime -tw_corr_pos;
      }
      if(btdcraw_neg&&badcraw_neg) {
	fGoodNegTdcTimeUnCorr.at(padnum-1) = tdc_neg*fScinTdcToTime;
	
	//tw_corr_neg = fHodoNeg_c1[padnum-1]/pow(adcamp_neg/fTdc_Thrs,fHodoNeg_c2[padnum-1]) -  fHodoNeg_c1[padnum-1]/pow(200./fTdc_Thrs, fHodoNeg_c2[padnum-1]);
		
	tw_corr_neg =  1./pow(adcamp_neg/fTdc_Thrs,fHodoNeg_c2[padnum-1]) - 1./pow(200./fTdc_Thrs, fHodoNeg_c2[padnum-1]);

	fGoodNegTdcTimeWalkCorr.at(padnum-1) = tdc_neg*fScinTdcToTime -tw_corr_neg;

      }


      // Do corrections if valid TDC on both ends of bar
      if( (btdcraw_pos && btdcraw_neg) && (badcraw_pos && badcraw_neg) ) {
	// Do the pulse height correction to the time.  (Position dependent corrections later)
        Double_t adc_timec_pos= adctime_pos;
        Double_t adc_timec_neg= adctime_neg;
	Double_t timec_pos, timec_neg;
	if(fTofUsingInvAdc) {
	  timec_pos = tdc_pos*fScinTdcToTime
	    - fHodoPosInvAdcOffset[index]
	    - fHodoPosInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0*.020,adcint_pos));
	  timec_neg = tdc_neg*fScinTdcToTime
	    - fHodoNegInvAdcOffset[index]
	    - fHodoNegInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0*.020,adcint_neg));
	} else {		// FADC style
	  timec_pos =  tdc_pos*fScinTdcToTime -tw_corr_pos + fHodo_LCoeff[index];
	  timec_neg =  tdc_neg*fScinTdcToTime -tw_corr_neg- 2*fHodoCableFit[index] + fHodo_LCoeff[index];
	  adc_timec_pos =  adc_timec_pos -tw_corr_pos + fHodo_LCoeff[index];
	  adc_timec_neg =  adc_timec_neg -tw_corr_neg- 2*fHodoCableFit[index] + fHodo_LCoeff[index];
	}

 	Double_t TWCorrDiff = fGoodNegTdcTimeWalkCorr.at(padnum-1) - 2*fHodoCableFit[index] - fGoodPosTdcTimeWalkCorr.at(padnum-1); 
     	
        Double_t fHitDistCorr = 0.5*TWCorrDiff*fHodoVelFit[index];  

	

	fGoodDiffDistTrack.at(index) =  fHitDistCorr;
	
	Double_t vellight=fHodoVelLight[index]; //read from hodo_cuts.param, where it is set fixed to 15.0 
	
	Double_t dist_from_center=0.5*(timec_neg-timec_pos)*vellight;
	Double_t scint_center=0.5*(fPosLeft+fPosRight);
	Double_t hit_position=scint_center+dist_from_center;
	hit_position=TMath::Min(hit_position,fPosLeft);
	hit_position=TMath::Max(hit_position,fPosRight);
	Double_t scin_corrected_time, postime, negtime;
	  Double_t adc_postime=adc_timec_pos;
	  Double_t adc_negtime=adc_timec_neg;	  
	if(fTofUsingInvAdc) {
	  timec_pos -= (fPosLeft-hit_position)/
	    fHodoPosInvAdcLinear[index];
	  timec_neg -= (hit_position-fPosRight)/
	    fHodoNegInvAdcLinear[index];
	  scin_corrected_time = 0.5*(timec_pos+timec_neg);
	  if (fCosmicFlag) {
	    postime = timec_pos + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    negtime = timec_neg + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  } else {
	    postime = timec_pos - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    negtime = timec_neg - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  }
	} else {
	  scin_corrected_time = 0.5*(timec_neg+timec_pos);  
	  timec_pos= scin_corrected_time;    
	  timec_neg= scin_corrected_time;
	  Double_t adc_time_corrected = 0.5*(adc_timec_pos+adc_timec_neg);
	  if (fCosmicFlag) {
	    postime = timec_pos + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    negtime = timec_neg + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    adc_postime = adc_time_corrected + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    adc_negtime = adc_time_corrected + (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  } else {
	    postime = timec_pos - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    negtime = timec_neg - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    adc_postime = adc_time_corrected - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	    adc_negtime = adc_time_corrected - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  }
	}
        ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPaddleCenter(fPosCenter[index]);
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCorrectedTimes(timec_pos,timec_neg,         
								     postime, negtime,
								     scin_corrected_time);
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCpeak(adcamp_pos); 
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCpeak(adcamp_neg); 
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCCorrtime(adc_postime); 
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCCorrtime(adc_negtime); 
        ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCalcPosition(fHitDistCorr); // 

	fGoodPosTdcTimeCorr.at(padnum-1) = timec_pos;
	fGoodNegTdcTimeCorr.at(padnum-1) = timec_neg;
	fGoodPosTdcTimeTOFCorr.at(padnum-1) = postime;
	fGoodNegTdcTimeTOFCorr.at(padnum-1) = negtime;
      } else {
	Double_t timec_pos,timec_neg;
        timec_pos=tdc_pos;
        timec_neg=tdc_neg;
	if(btdcraw_pos&& badcraw_pos) {
	  if(fTofUsingInvAdc) {
	    timec_pos = tdc_pos*fScinTdcToTime
	      - fHodoPosInvAdcOffset[index]
	      - fHodoPosInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0*.020,adcint_pos));
	  } else {		// FADC style
	  timec_pos =  tdc_pos*fScinTdcToTime -tw_corr_pos + fHodo_LCoeff[index];
	  }
	}
	if(btdcraw_neg && badcraw_neg) {
	  if(fTofUsingInvAdc) {
	    timec_neg = tdc_neg*fScinTdcToTime
	      - fHodoNegInvAdcOffset[index]
	      - fHodoNegInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0*.020,adcint_neg));
	  } else {		// FADC style
	  timec_neg =  tdc_neg*fScinTdcToTime -tw_corr_neg- 2*fHodoCableFit[index] + fHodo_LCoeff[index];
	  }
	}
        Double_t adc_neg=0.,adc_pos=0.;
	if (badcraw_neg) adc_neg=adcamp_neg;
	if (badcraw_pos) adc_pos=adcamp_pos;
        ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPaddleCenter(fPosCenter[index]);
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCorrectedTimes(timec_pos,timec_neg);
        ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCpeak(adc_neg); // needed for new TWCOrr
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCpeak(adc_pos); // needed for new TWCOrr
	if (badcraw_neg)  {
          ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCtime(adctime_neg);
         } else {
           ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetNegADCtime(-999.);
        }
	if (badcraw_pos) {
           ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCtime(adctime_pos); 
         } else {
           ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetPosADCtime(-999.); 
         }
        ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCalcPosition(kBig); // 
	fGoodPosTdcTimeCorr.at(padnum-1) = timec_pos;
	fGoodNegTdcTimeCorr.at(padnum-1) = timec_neg;
	fGoodPosTdcTimeTOFCorr.at(padnum-1) = kBig;
	fGoodNegTdcTimeTOFCorr.at(padnum-1) = kBig;
      }
      //      if ( ((THcHodoHit*) fHodoHits->At(fNScinHits))->GetPosTOFCorrectedTime() != ((THcHodoHit*) fHodoHits->At(fNScinHits))->GetPosTOFCorrectedTime()) cout << " ihit = " << ihit << " scinhit = " << fNScinHits << " plane = " << fPlaneNum << " padnum = " << padnum << " " << tdc_pos<< " "<< tdc_neg<< " " << ((THcHodoHit*) fHodoHits->At(fNScinHits))->GetPosTOFCorrectedTime() << endl;
      fNScinHits++;		// One or more good time counter
      //
    }
    ihit++;			// Raw hit counter
  }
  //  cout << "THcScintillatorPlane: ihit = " << ihit << endl;
  if (problem_flag) {
 cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
cout << " Ref problem end *******" << endl;
  }
  return(ihit);
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  /*! \brief Extract the data for this plane from raw hit list THcRawHodoHit, accumulating into arrays for calculating pedestals.
   *
   * - Loop through raw data for scintillator plane
   */
  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::AcculatePedestals " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcRawHodoHit* hit = (THcRawHodoHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t element = hit->fCounter - 1; // Should check if in range
    Int_t adcpos = hit->GetRawAdcHitPos().GetPulseIntRaw();
    Int_t adcneg = hit->GetRawAdcHitNeg().GetPulseIntRaw();

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
void THcScintillatorPlane::CalculatePedestals( )
{
  /*! \brief   Calculate pedestals from arrays made in THcScintillatorPlane::AccumulatePedestals
   *
   * - Calculate pedestals from arrays made in THcScintillatorPlane::AccumulatePedestals
   * - In old fortran ENGINE code, a comparison was made between calculated pedestals and the pedestals read in by the FASTBUS modules for zero supression. This is not implemented.
   */
  for(UInt_t i=0; i<fNelem;i++) {

    // Positive tubes
    fPosPed[i] = ((Double_t) fPosPedSum[i]) / TMath::Max(1, fPosPedCount[i]);
    fPosThresh[i] = fPosPed[i] + 15;

    // Negative tubes
    fNegPed[i] = ((Double_t) fNegPedSum[i]) / TMath::Max(1, fNegPedCount[i]);
    fNegThresh[i] = fNegPed[i] + 15;

    //    cout <<"Pedestals "<< i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;
  }
  //  cout << " " << endl;

}

//_____________________________________________________________________________
void THcScintillatorPlane::InitializePedestals( )
{
  /*! \brief   called by THcScintillatorPlane::ReadDatabase
   *
   * - Initialize variables used in  THcScintillatorPlane::AccumulatePedestals and THcScintillatorPlane::CalculatePedestals
   * - Minimum number of pedestal events needed for calculation, fMinPeds, hadrcoded to 500
   */
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
  for(UInt_t i=0;i<fNelem;i++) {
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
//____________________________________________________________________________
ClassImp(THcScintillatorPlane)
////////////////////////////////////////////////////////////////////////////////
