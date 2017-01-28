/** \class THcScintillatorPlane
    \ingroup DetSupport

This class implements a single plane of scintillators.  The THcHodoscope
class instatiates one object per plane.

*/
#include "TMath.h"
#include "THcScintillatorPlane.h"
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
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fHodoHits = new TClonesArray("THcHodoHit",16);
  frPosTDCHits = new TClonesArray("THcSignalHit",16);
  frNegTDCHits = new TClonesArray("THcSignalHit",16);
  frPosADCHits = new TClonesArray("THcSignalHit",16);
  frNegADCHits = new TClonesArray("THcSignalHit",16);
  frPosADCSums = new TClonesArray("THcSignalHit",16);
  frNegADCSums = new TClonesArray("THcSignalHit",16);
  frPosADCPeds = new TClonesArray("THcSignalHit",16);
  frNegADCPeds = new TClonesArray("THcSignalHit",16);

  frPosTdcTimeRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frPosTdcTime = new TClonesArray("THcSignalHit", 16);
  frPosAdcPed = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frPosAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

  frNegTdcTimeRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frNegTdcTime = new TClonesArray("THcSignalHit", 16);
  frNegAdcPed = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frNegAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

  fPlaneNum = planenum;
  fTotPlanes = planenum;
  fNScinHits = 0;
  //
  fMaxHits=53;

  fpTimes = new Double_t [fMaxHits];
  fScinTime = new Double_t [fMaxHits];
  fScinSigma = new Double_t [fMaxHits];
  fScinZpos = new Double_t [fMaxHits];
  fPosCenter = NULL;
}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor
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

  delete frNegTdcTimeRaw;
  delete frNegAdcPedRaw;
  delete frNegAdcPulseIntRaw;
  delete frNegAdcPulseAmpRaw;
  delete frNegAdcPulseTimeRaw;

  delete frNegTdcTime;
  delete frNegAdcPed;
  delete frNegAdcPulseInt;
  delete frNegAdcPulseAmp;

  delete fpTimes;
  delete [] fScinTime;
  delete [] fScinSigma;
  delete [] fScinZpos;
  delete [] fPosCenter;

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
  delete [] fHodoPosInvAdcAdc; fHodoPosInvAdcAdc = NULL;
  delete [] fHodoNegInvAdcAdc; fHodoNegInvAdcAdc = NULL;

  delete [] fHodoVelLight; fHodoVelLight = NULL;
  delete [] fHodoSigma; fHodoSigma = NULL;

}

//______________________________________________________________________________
THaAnalysisObject::EStatus THcScintillatorPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  cout << "THcScintillatorPlane::Init called " << GetName() << endl;

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
  char parname[100];

  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  // need this further down so read them first! GN
  strcpy(parname,prefix);
  strcat(parname,"scin_");
  strcat(parname,GetName());
  strcat(parname,"_nr");
  fNelem = *(Int_t *)gHcParms->Find(parname)->GetValuePointer();
  //
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
    {0}
  };

  fTofUsingInvAdc = 1;
  fADCMode = kADCStandard;
  fADCPedScaleFactor = 1.0;
  fADCDiagCut = 50.0;
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  // fetch the parameter from the temporary list

  // Retrieve parameters we need from parent class
  // Common for all planes
  fHodoSlop= ((THcHodoscope*) GetParent())->GetHodoSlop(fPlaneNum-1);
  fTdcOffset= ((THcHodoscope*) GetParent())->GetTdcOffset(fPlaneNum-1);
  fScinTdcMin=((THcHodoscope *)GetParent())->GetTdcMin();
  fScinTdcMax=((THcHodoscope *)GetParent())->GetTdcMax();
  fScinTdcToTime=((THcHodoscope *)GetParent())->GetTdcToTime();
  fTofTolerance=((THcHodoscope *)GetParent())->GetTofTolerance();
  fBetaNominal=((THcHodoscope *)GetParent())->GetBetaNominal();
  fStartTimeCenter=((THcHodoscope *)GetParent())->GetStartTimeCenter();
  fStartTimeSlop=((THcHodoscope *)GetParent())->GetStartTimeSlop();
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
  fHodoPosInvAdcAdc = new Double_t[fNelem];
  fHodoNegInvAdcAdc = new Double_t[fNelem];
  fHodoSigma = new Double_t[fNelem];
  for(Int_t j=0;j<(Int_t) fNelem;j++) {
    Int_t index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
    fHodoPosMinPh[j] = ((THcHodoscope *)GetParent())->GetHodoPosMinPh(index);
    fHodoNegMinPh[j] = ((THcHodoscope *)GetParent())->GetHodoNegMinPh(index);
    fHodoPosPhcCoeff[j] = ((THcHodoscope *)GetParent())->GetHodoPosPhcCoeff(index);
    fHodoNegPhcCoeff[j] = ((THcHodoscope *)GetParent())->GetHodoNegPhcCoeff(index);
    fHodoPosTimeOffset[j] = ((THcHodoscope *)GetParent())->GetHodoPosTimeOffset(index);
    fHodoNegTimeOffset[j] = ((THcHodoscope *)GetParent())->GetHodoNegTimeOffset(index);
    fHodoPosInvAdcOffset[j] = ((THcHodoscope *)GetParent())->GetHodoPosInvAdcOffset(index);
    fHodoNegInvAdcOffset[j] = ((THcHodoscope *)GetParent())->GetHodoNegInvAdcOffset(index);
    fHodoPosInvAdcLinear[j] = ((THcHodoscope *)GetParent())->GetHodoPosInvAdcLinear(index);
    fHodoNegInvAdcLinear[j] = ((THcHodoscope *)GetParent())->GetHodoNegInvAdcLinear(index);
    fHodoPosInvAdcAdc[j] = ((THcHodoscope *)GetParent())->GetHodoPosInvAdcAdc(index);
    fHodoNegInvAdcAdc[j] = ((THcHodoscope *)GetParent())->GetHodoNegInvAdcAdc(index);
    fHodoVelLight[j] = ((THcHodoscope *)GetParent())->GetHodoVelLight(index);
    Double_t possigma = ((THcHodoscope *)GetParent())->GetHodoPosSigma(index);
    Double_t negsigma = ((THcHodoscope *)GetParent())->GetHodoNegSigma(index);
    fHodoSigma[j] = TMath::Sqrt(possigma*possigma+negsigma*negsigma)/2.0;
  }

  cout <<" plane num = "<<fPlaneNum<<endl;
  cout <<" nelem     = "<<fNelem<<endl;
  cout <<" zpos      = "<<fZpos<<endl;
  cout <<" dzpos     = "<<fDzpos<<endl;
  cout <<" spacing   = "<<fSpacing<<endl;
  cout <<" size      = "<<fSize<<endl;
  cout <<" hodoslop  = "<<fHodoSlop<<endl;
  cout <<"PosLeft = "<<fPosLeft<<endl;
  cout <<"PosRight = "<<fPosRight<<endl;
  cout <<"PosOffset = "<<fPosOffset<<endl;
  cout <<"PosCenter[0] = "<<fPosCenter[0]<<endl;

  // Think we will make special methods to pass most
  // How generic do we want to make this class?
  // The way we get parameter data is going to be pretty specific to
  // our parameter file naming conventions.  But on the other hand,
  // the Hall A analyzer read database is pretty specific.
  // Is there any way for this class to know which spectrometer he
  // belongs too?

  // Create arrays to hold results here
  InitializePedestals();

  return kOK;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  cout << "THcScintillatorPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"postdcpad", "List of Positive TDC Counter Number",      "frPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdcpad", "List of Negative TDC Counter Number",      "frNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadcpad", "List of Positive ADC Counter Number",      "frPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadcpad", "List of Negative ADC Counter Number",      "frNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"postdcval", "List of Positive TDC Values",              "frPosTDCHits.THcSignalHit.GetData()"},
    {"negtdcval", "List of Negative TDC Values",              "frNegTDCHits.THcSignalHit.GetData()"},
    {"posadcval", "List of Positive ADC Values",              "frPosADCHits.THcSignalHit.GetData()"},
    {"negadcval", "List of Negative ADC Values",              "frNegADCHits.THcSignalHit.GetData()"},
    {"posadcsum", "List of Positive ADC Sample Sums",         "frPosADCSums.THcSignalHit.GetData()"},
    {"negadcsum", "List of Negative ADC Sample Sums",         "frNegADCSums.THcSignalHit.GetData()"},
    {"posadcped", "List of Positive ADC Pedestals",           "frPosADCPeds.THcSignalHit.GetData()"},
    {"negadcped", "List of Negative ADC Pedestals",           "frNegADCPeds.THcSignalHit.GetData()"},

    {"posTdcCounter", "List of positive TDC counter numbers.", "frPosTdcTimeRaw.THcSignalHit.GetPaddleNumber()"},
    {"posAdcCounter", "List of positive ADC counter numbers.", "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},
    {"negTdcCounter", "List of negative TDC counter numbers.", "frNegTdcTimeRaw.THcSignalHit.GetPaddleNumber()"},
    {"negAdcCounter", "List of negative ADC counter numbers.", "frNegAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},

    {"posTdcTimeRaw",      "List of positive raw TDC values.",           "frPosTdcTimeRaw.THcSignalHit.GetData()"},
    {"posAdcPedRaw",       "List of positive raw ADC pedestals",         "frPosAdcPedRaw.THcSignalHit.GetData()"},
    {"posAdcPulseIntRaw",  "List of positive raw ADC pulse integrals.",  "frPosAdcPulseIntRaw.THcSignalHit.GetData()"},
    {"posAdcPulseAmpRaw",  "List of positive raw ADC pulse amplitudes.", "frPosAdcPulseAmpRaw.THcSignalHit.GetData()"},
    {"posAdcPulseTimeRaw", "List of positive raw ADC pulse times.",      "frPosAdcPulseTimeRaw.THcSignalHit.GetData()"},

    {"posTdcTime",         "List of positive TDC values.",               "frPosTdcTime.THcSignalHit.GetData()"},
    {"posAdcPed",          "List of positive ADC pedestals",             "frPosAdcPed.THcSignalHit.GetData()"},
    {"posAdcPulseInt",     "List of positive ADC pulse integrals.",      "frPosAdcPulseInt.THcSignalHit.GetData()"},
    {"posAdcPulseAmp",     "List of positive ADC pulse amplitudes.",     "frPosAdcPulseAmp.THcSignalHit.GetData()"},

    {"negTdcTimeRaw",      "List of negative raw TDC values.",           "frNegTdcTimeRaw.THcSignalHit.GetData()"},
    {"negAdcPedRaw",       "List of negative raw ADC pedestals",         "frNegAdcPedRaw.THcSignalHit.GetData()"},
    {"negAdcPulseIntRaw",  "List of negative raw ADC pulse integrals.",  "frNegAdcPulseIntRaw.THcSignalHit.GetData()"},
    {"negAdcPulseAmpRaw",  "List of negative raw ADC pulse amplitudes.", "frNegAdcPulseAmpRaw.THcSignalHit.GetData()"},
    {"negAdcPulseTimeRaw", "List of negative raw ADC pulse times.",      "frNegAdcPulseTimeRaw.THcSignalHit.GetData()"},

    {"negTdcTime",         "List of negative TDC values.",               "frNegTdcTime.THcSignalHit.GetData()"},
    {"negAdcPed",          "List of negative ADC pedestals",             "frNegAdcPed.THcSignalHit.GetData()"},
    {"negAdcPulseInt",     "List of negative ADC pulse integrals.",      "frNegAdcPulseInt.THcSignalHit.GetData()"},
    {"negAdcPulseAmp",     "List of negative ADC pulse amplitudes.",     "frNegAdcPulseAmp.THcSignalHit.GetData()"},

    {"fptime", "Time at focal plane",     "GetFpTime()"},

    {"nhits", "Number of paddle hits (passed TDC Min and Max cuts for either end)",           "GetNScinHits() "},
    {"GoodPaddle",         "List of Paddle Numbers (passed TDC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetPaddleNumber()"},
    {"GoodNegTdcChan",         "List of negative TDC values (passed TDC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetNegTDC()"},
    {"GoodNegTdcTimeCorr",         "List of negative corrected TDC values (corrected for PMT offset and ADC)",               "fHodoHits.THcHodoHit.GetNegCorrectedTime()"},
    {"GoodNegTdcTimeTOFCorr",         "List of negative corrected TDC values (corrected for TOF)",               "fHodoHits.THcHodoHit.GetNegTOFCorrectedTime()"},
    {"GoodNegAdcPulseInt",         "List of negative ADC values (passed TDC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetNegADC()"},
    {"GoodPosTdcChan",         "List of positive TDC values (passed TDC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetPosTDC()"},
    {"GoodPosTdcTimeCorr",         "List of positive corrected TDC values (corrected for PMT offset and ADC)",               "fHodoHits.THcHodoHit.GetPosCorrectedTime()"},
    {"GoodPosTdcTimeTOFCorr",         "List of positive corrected TDC values (corrected for TOF)",               "fHodoHits.THcHodoHit.GetPosTOFCorrectedTime()"},
    {"GoodPosAdcPulseInt",         "List of positive ADC values (passed TDC Min and Max cuts for either end)",               "fHodoHits.THcHodoHit.GetPosADC()"},
    {"ngoodhits", "Number of paddle hits (passed tof tolerance and used to determine the focal plane time )",           "GetNGoodHits() "},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
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

  frNegTdcTimeRaw->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegTdcTime->Clear();
  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

  fpTime = -1.e4;
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
   * - Fills THcSignalHit objects frPosADCHits and frNegaDCHit with pedestal subtracted ADC when value larger than 50
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
  Int_t nrPosTDCHits=0;
  Int_t nrNegTDCHits=0;
  Int_t nrPosADCHits=0;
  Int_t nrNegADCHits=0;
  frPosTDCHits->Clear();
  frNegTDCHits->Clear();
  frPosADCHits->Clear();
  frNegADCHits->Clear();
  frPosADCSums->Clear();
  frNegADCSums->Clear();
  frPosADCPeds->Clear();
  frNegADCPeds->Clear();

  UInt_t nrPosAdcHits = 0;
  UInt_t nrNegAdcHits = 0;
  UInt_t nrPosTdcHits = 0;
  UInt_t nrNegTdcHits = 0;

  frPosTdcTimeRaw->Clear();
  frPosAdcPedRaw->Clear();
  frPosAdcPulseIntRaw->Clear();
  frPosAdcPulseAmpRaw->Clear();
  frPosAdcPulseTimeRaw->Clear();

  frPosTdcTime->Clear();
  frPosAdcPed->Clear();
  frPosAdcPulseInt->Clear();
  frPosAdcPulseAmp->Clear();

  frNegTdcTimeRaw->Clear();
  frNegAdcPedRaw->Clear();
  frNegAdcPulseIntRaw->Clear();
  frNegAdcPulseAmpRaw->Clear();
  frNegAdcPulseTimeRaw->Clear();

  frNegTdcTime->Clear();
  frNegAdcPed->Clear();
  frNegAdcPulseInt->Clear();
  frNegAdcPulseAmp->Clear();

  //stripped
  fNScinHits=0;
  fHodoHits->Clear();
  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
  Int_t ihit = nexthit;

  //  cout << "THcScintillatorPlane: raw htis = " << nrawhits << endl;

  // A THcRawHodoHit contains all the information (tdc and adc for both
  // pmts) for a single paddle for a single trigger.  The tdc information
  // might include multiple hits if it uses a multihit tdc.
  // Use "ihit" as the index over THcRawHodoHit objects.  Use
  // "thit" to index over multiple tdc hits within an "ihit".
  while(ihit < nrawhits) {
    THcRawHodoHit* hit = (THcRawHodoHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t padnum=hit->fCounter;

    Int_t index=padnum-1;

    THcRawTdcHit& rawPosTdcHit = hit->GetRawTdcHitPos();
    for (UInt_t thit=0; thit<rawPosTdcHit.GetNHits(); ++thit) {
      ((THcSignalHit*) frPosTdcTimeRaw->ConstructedAt(nrPosTdcHits))->Set(padnum, rawPosTdcHit.GetTimeRaw(thit));
      ((THcSignalHit*) frPosTdcTime->ConstructedAt(nrPosTdcHits))->Set(padnum, rawPosTdcHit.GetTime(thit));
      ++nrPosTdcHits;
    }
    THcRawTdcHit& rawNegTdcHit = hit->GetRawTdcHitNeg();
    for (UInt_t thit=0; thit<rawNegTdcHit.GetNHits(); ++thit) {
      ((THcSignalHit*) frNegTdcTimeRaw->ConstructedAt(nrNegTdcHits))->Set(padnum, rawNegTdcHit.GetTimeRaw(thit));
      ((THcSignalHit*) frNegTdcTime->ConstructedAt(nrNegTdcHits))->Set(padnum, rawNegTdcHit.GetTime(thit));
      ++nrNegTdcHits;
    }
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

    // Need to be finding first hit in TDC range, not the first hit overall
    if (hit->GetRawTdcHitPos().GetNHits() > 0)
      ((THcSignalHit*) frPosTDCHits->ConstructedAt(nrPosTDCHits++))->Set(padnum, hit->GetRawTdcHitPos().GetTime()+fTdcOffset);
    if (hit->GetRawTdcHitNeg().GetNHits() > 0)
      ((THcSignalHit*) frNegTDCHits->ConstructedAt(nrNegTDCHits++))->Set(padnum, hit->GetRawTdcHitNeg().GetTime()+fTdcOffset);
    // For making hit maps, we use >= 50 cut
    // For making raw hists, we don't want the cut
    // We can use a flag to turn on and off these without 50 cut
    // Value of 50 no long valid with different ADC type for FADC
    Double_t adc_pos;
    Double_t adc_neg;
    if(fADCMode == kADCDynamicPedestal) {
			adc_pos = hit->GetRawAdcHitPos().GetPulseInt();
			adc_neg = hit->GetRawAdcHitNeg().GetPulseInt();
    } else if (fADCMode == kADCSampleIntegral) {
			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[index];
			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[index];
    } else if (fADCMode == kADCSampIntDynPed) {
      adc_pos = hit->GetRawAdcHitPos().GetSampleInt();
      adc_neg = hit->GetRawAdcHitNeg().GetSampleInt();
    } else {
      adc_pos = hit->GetRawAdcHitPos().GetPulseIntRaw()-fPosPed[index];
      adc_neg = hit->GetRawAdcHitNeg().GetPulseIntRaw()-fNegPed[index];
    }
    if (adc_pos >= fADCDiagCut) {
      ((THcSignalHit*) frPosADCHits->ConstructedAt(nrPosADCHits))->Set(padnum, adc_pos);
      Double_t samplesum=hit->GetRawAdcHitPos().GetSampleIntRaw();
      Double_t pedestal=hit->GetRawAdcHitPos().GetPedRaw();
      ((THcSignalHit*) frPosADCSums->ConstructedAt(nrPosADCHits))->Set(padnum, samplesum);
      ((THcSignalHit*) frPosADCPeds->ConstructedAt(nrPosADCHits++))->Set(padnum, pedestal);
    }
    if (adc_neg >= fADCDiagCut) {
      ((THcSignalHit*) frNegADCHits->ConstructedAt(nrNegADCHits))->Set(padnum, adc_neg);
			Double_t samplesum=hit->GetRawAdcHitNeg().GetSampleIntRaw();
      Double_t pedestal=hit->GetRawAdcHitNeg().GetPedRaw();
      ((THcSignalHit*) frNegADCSums->ConstructedAt(nrNegADCHits))->Set(padnum, samplesum);
      ((THcSignalHit*) frNegADCPeds->ConstructedAt(nrNegADCHits++))->Set(padnum, pedestal);
    }

    Bool_t btdcraw_pos=kFALSE;
    Bool_t btdcraw_neg=kFALSE;
    Int_t tdc_pos=-999;
    Int_t tdc_neg=-999;
    // Find first in range hit from multihit tdc
    for(UInt_t thit=0; thit<hit->GetRawTdcHitPos().GetNHits(); thit++) {
      tdc_pos = hit->GetRawTdcHitPos().GetTime(thit)+fTdcOffset;
      if(tdc_pos >= fScinTdcMin && tdc_pos <= fScinTdcMax) {
	btdcraw_pos = kTRUE;
	break;
      }
    }
    for(UInt_t thit=0; thit<hit->GetRawTdcHitNeg().GetNHits(); thit++) {
      tdc_neg = hit->GetRawTdcHitNeg().GetTime(thit)+fTdcOffset;
      if(tdc_neg >= fScinTdcMin && tdc_neg <= fScinTdcMax) {
	btdcraw_neg = kTRUE;
	break;
      }
    }
    // Proceed if there is a valid TDC on either end of the bar
    //    cout << ihit << " " << hit->fCounter << " " << fNScinHits<< " " << tdc_neg << " " << btdcraw_neg << " " << tdc_pos << " " << btdcraw_pos << " " <<endl;
    if(btdcraw_pos || btdcraw_neg) {


      new( (*fHodoHits)[fNScinHits]) THcHodoHit(tdc_pos, tdc_neg,
						adc_pos, adc_neg,
						hit->fCounter, this);
    // Do corrections if valid TDC on both ends of bar
      if(btdcraw_pos && btdcraw_neg) {

	// Do the pulse height correction to the time.  (Position dependent corrections later)
	Double_t timec_pos, timec_neg;
	if(fTofUsingInvAdc) {
	  timec_pos = tdc_pos*fScinTdcToTime
	    - fHodoPosInvAdcOffset[index]
	    - fHodoPosInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0,adc_pos));
	  timec_neg = tdc_neg*fScinTdcToTime
	    - fHodoNegInvAdcOffset[index]
	    - fHodoNegInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0,adc_neg));
	} else {		// Old style
	  timec_pos = tdc_pos*fScinTdcToTime - fHodoPosPhcCoeff[index]*
	    TMath::Sqrt(TMath::Max(0.0,adc_pos/fHodoPosMinPh[index]-1.0))
	    - fHodoPosTimeOffset[index];
	  timec_neg = tdc_neg*fScinTdcToTime - fHodoNegPhcCoeff[index]*
	    TMath::Sqrt(TMath::Max(0.0,adc_neg/fHodoNegMinPh[index]-1.0))
	    - fHodoNegTimeOffset[index];
	}
	// Find hit position using ADCs
	// If postime larger, then hit was nearer negative side.
	// Some incarnations use fixed velocity of 15 cm/nsec
	Double_t vellight=fHodoVelLight[index];
	Double_t dist_from_center=0.5*(timec_neg-timec_pos)*vellight;
	Double_t scint_center=0.5*(fPosLeft+fPosRight);
	Double_t hit_position=scint_center+dist_from_center;
	hit_position=TMath::Min(hit_position,fPosLeft);
	hit_position=TMath::Max(hit_position,fPosRight);
	// Position depenent time corrections
	Double_t scin_corrected_time, postime, negtime;
	if(fTofUsingInvAdc) {
	  timec_pos -= (fPosLeft-hit_position)/
	    fHodoPosInvAdcLinear[index];
	  timec_neg -= (hit_position-fPosRight)/
	    fHodoNegInvAdcLinear[index];
	  scin_corrected_time = 0.5*(timec_pos+timec_neg);
	  postime = timec_pos - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  negtime = timec_neg - (fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	} else {
	  postime=timec_pos-(fPosLeft-hit_position)/fHodoVelLight[index];
	  negtime=timec_neg-(hit_position-fPosRight)/fHodoVelLight[index];
	  scin_corrected_time = 0.5*(postime+negtime);
	  postime = postime-(fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	  negtime = negtime-(fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
	}
	//        cout << fNScinHits<< " " << timec_pos << " " << timec_neg << endl;
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCorrectedTimes(timec_pos,timec_neg,
								     postime, negtime,
								     scin_corrected_time);
      } else {
	Double_t timec_pos,timec_neg;
        timec_pos=tdc_pos;
        timec_neg=tdc_neg;
      if(btdcraw_pos) {
	if(fTofUsingInvAdc) {
	  timec_pos = tdc_pos*fScinTdcToTime
	    - fHodoPosInvAdcOffset[index]
	    - fHodoPosInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0,adc_pos));
	} else {		// Old style
	  timec_pos = tdc_pos*fScinTdcToTime - fHodoPosPhcCoeff[index]*
	    TMath::Sqrt(TMath::Max(0.0,adc_pos/fHodoPosMinPh[index]-1.0))
	    - fHodoPosTimeOffset[index];
	}
      }
      if(btdcraw_neg) {
	if(fTofUsingInvAdc) {
	  timec_neg = tdc_neg*fScinTdcToTime
	    - fHodoNegInvAdcOffset[index]
	    - fHodoNegInvAdcAdc[index]/TMath::Sqrt(TMath::Max(20.0,adc_neg));
	} else {		// Old style
	  timec_neg = tdc_neg*fScinTdcToTime - fHodoNegPhcCoeff[index]*
	    TMath::Sqrt(TMath::Max(0.0,adc_neg/fHodoNegMinPh[index]-1.0))
	    - fHodoNegTimeOffset[index];
	}
      }
	((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCorrectedTimes(timec_pos,timec_neg,
								     timec_pos,timec_neg,
								     0.0);
      }
      fNScinHits++;		// One or more good time counter
    }
    ihit++;			// Raw hit counter
  }

  //  cout << "THcScintillatorPlane: ihit = " << ihit << endl;

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
