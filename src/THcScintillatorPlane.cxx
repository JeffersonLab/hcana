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
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  // fetch the parameter from the temporary list

  // Retrieve parameters we need from parent class
  // Common for all planes
  fHodoSlop= ((THcHodoscope*) GetParent())->GetHodoSlop(fPlaneNum-1);
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
  fHodoSigma = new Double_t[fNelem];
  for(Int_t j=0;j<(Int_t) fNelem;j++) {
    Int_t index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
    fHodoPosMinPh[j] = ((THcHodoscope *)GetParent())->GetHodoPosMinPh(index);
    fHodoNegMinPh[j] = ((THcHodoscope *)GetParent())->GetHodoNegMinPh(index);
    fHodoPosPhcCoeff[j] = ((THcHodoscope *)GetParent())->GetHodoPosPhcCoeff(index);
    fHodoNegPhcCoeff[j] = ((THcHodoscope *)GetParent())->GetHodoNegPhcCoeff(index);
    fHodoPosTimeOffset[j] = ((THcHodoscope *)GetParent())->GetHodoPosTimeOffset(index);
    fHodoNegTimeOffset[j] = ((THcHodoscope *)GetParent())->GetHodoNegTimeOffset(index);
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
    {"postdchits", "List of Positive TDC hits",      "frPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdchits", "List of Negative TDC hits",      "frNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadchits", "List of Positive ADC hits",      "frPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits",      "frNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"fptime", "Time at focal plane",     "GetFpTime()"},
    {"nhits", "Number of paddle hits (passed TDC Min and Max cuts for both ends)",           "GetNScinHits() "},
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
  //stripped
  fNScinHits=0;
  fHodoHits->Clear();
  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
  Int_t ihit = nexthit;

  //  cout << "THcScintillatorPlane: raw htis = " << nrawhits << endl;
  
  while(ihit < nrawhits) {
    THcRawHodoHit* hit = (THcRawHodoHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t padnum=hit->fCounter;

    Int_t index=padnum-1;
    // Need to be finding first hit in TDC range, not the first hit overall
    if (hit->fNRawHits[2] > 0) 
      ((THcSignalHit*) frPosTDCHits->ConstructedAt(nrPosTDCHits++))->Set(padnum, hit->GetTDCPos());
    if (hit->fNRawHits[3] > 0) 
      ((THcSignalHit*) frNegTDCHits->ConstructedAt(nrNegTDCHits++))->Set(padnum, hit->GetTDCNeg());
    if ((hit->GetADCPos()-fPosPed[index]) >= 50) 
      ((THcSignalHit*) frPosADCHits->ConstructedAt(nrPosADCHits++))->Set(padnum, hit->GetADCPos()-fPosPed[index]);
    if ((hit->GetADCNeg()-fNegPed[index]) >= 50) 
      ((THcSignalHit*) frNegADCHits->ConstructedAt(nrNegADCHits++))->Set(padnum, hit->GetADCNeg()-fNegPed[index]);
    // check TDC values
    if (((hit->GetTDCPos() >= fScinTdcMin) && (hit->GetTDCPos() <= fScinTdcMax)) ||
	((hit->GetTDCNeg() >= fScinTdcMin) && (hit->GetTDCNeg() <= fScinTdcMax))) {

      // If TDC values are all good, transfer the raw hit to the HodoHit list
      new( (*fHodoHits)[fNScinHits]) THcHodoHit(hit, fPosPed[index], fNegPed[index], this);
      
      // Do the pulse height correction to the time.  (Position dependent corrections later)
      Double_t timec_pos = hit->GetTDCPos()*fScinTdcToTime - fHodoPosPhcCoeff[index]*
	TMath::Sqrt(TMath::Max(0.0,
			       (hit->GetADCPos()-fPosPed[index])/fHodoPosMinPh[index]-1.0))
	- fHodoPosTimeOffset[index];
      Double_t timec_neg = hit->GetTDCNeg()*fScinTdcToTime - fHodoNegPhcCoeff[index]*
	TMath::Sqrt(TMath::Max(0.0,
			       (hit->GetADCNeg()-fNegPed[index])/fHodoNegMinPh[index]-1.0))
	- fHodoNegTimeOffset[index];

      // Find hit position using ADCs
      // If postime larger, then hit was nearer negative side.
      Double_t dist_from_center=0.5*(timec_neg-timec_pos)*fHodoVelLight[index];
      Double_t scint_center=0.5*(fPosLeft+fPosRight);
      Double_t hit_position=scint_center+dist_from_center;
      hit_position=TMath::Min(hit_position,fPosLeft);
      hit_position=TMath::Max(hit_position,fPosRight);
      Double_t postime=timec_pos-(fPosLeft-hit_position)/fHodoVelLight[index];
      Double_t negtime=timec_neg-(hit_position-fPosRight)/fHodoVelLight[index];
      Double_t scin_corrected_time = 0.5*(postime+negtime);
      postime = postime-(fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);
      negtime = negtime-(fZpos+(index%2)*fDzpos)/(29.979*fBetaNominal);

      ((THcHodoHit*) fHodoHits->At(fNScinHits))->SetCorrectedTimes(timec_pos,timec_neg,
								 postime, negtime,
								 scin_corrected_time);
      fNScinHits++;
    }
    else {
    }
    ihit++;
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

