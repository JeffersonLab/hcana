//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcScintillatorPlane
//
//////////////////////////////////////////////////////////////////////////
#include "TMath.h"
#include "THcScintillatorPlane.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
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
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  frPosTDCHits = new TClonesArray("THcSignalHit",16);
  frNegTDCHits = new TClonesArray("THcSignalHit",16);
  frPosADCHits = new TClonesArray("THcSignalHit",16);
  frNegADCHits = new TClonesArray("THcSignalHit",16);
  fPlaneNum = planenum;
  fTotPlanes = planenum;
  fNScinHits = 0; 
  //
  fMaxHits=53;

  fGoodRawPadNum = new Int_t [fMaxHits]; // Ahmed

  fpTimes = new Double_t [fMaxHits];
  fScinTime = new Double_t [fMaxHits];
  fScinSigma = new Double_t [fMaxHits];
  fScinZpos = new Double_t [fMaxHits];
}
//______________________________________________________________________________
THcScintillatorPlane::THcScintillatorPlane( const char* name, 
					    const char* description,
					    const Int_t planenum,
					    const Int_t totplanes,
					    THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fPosTDCHits = new TClonesArray("THcSignalHit",16);
  fNegTDCHits = new TClonesArray("THcSignalHit",16);
  fPosADCHits = new TClonesArray("THcSignalHit",16);
  fNegADCHits = new TClonesArray("THcSignalHit",16);
  frPosTDCHits = new TClonesArray("THcSignalHit",16);
  frNegTDCHits = new TClonesArray("THcSignalHit",16);
  frPosADCHits = new TClonesArray("THcSignalHit",16);
  frNegADCHits = new TClonesArray("THcSignalHit",16);
  fPlaneNum = planenum;
  fTotPlanes = totplanes;
  fNScinHits = 0;
  //
  fMaxHits=53;

  fGoodRawPadNum = new Int_t [fMaxHits];

  fpTimes = new Double_t [fMaxHits];
  fScinTime = new Double_t [fMaxHits];
  fScinSigma = new Double_t [fMaxHits];
  fScinZpos = new Double_t [fMaxHits];
}

//______________________________________________________________________________
THcScintillatorPlane::~THcScintillatorPlane()
{
  // Destructor
  delete fPosTDCHits;
  delete fNegTDCHits;
  delete fPosADCHits;
  delete fNegADCHits;
  delete frPosTDCHits;
  delete frNegTDCHits;
  delete frPosADCHits;
  delete frNegADCHits;
  delete fpTimes;
  delete fScinTime;
  delete fScinSigma;
  delete fScinZpos;

  delete fGoodRawPadNum;


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

  Double_t tmpdouble[fTotPlanes];
    DBRequest list[]={
     {Form("scin_%s_zpos",GetName()), &fZpos, kDouble},
     {Form("scin_%s_dzpos",GetName()), &fDzpos, kDouble},
     {Form("scin_%s_size",GetName()), &fSize, kDouble},
     {Form("scin_%s_spacing",GetName()), &fSpacing, kDouble},
     {Form("scin_%s_%s",GetName(),tmpleft), &fPosLeft,kDouble},
     {Form("scin_%s_%s",GetName(),tmpright), &fPosRight,kDouble},
     {Form("scin_%s_offset",GetName()), &fPosOffset, kDouble},
     {Form("scin_%s_center",GetName()), &fPosCenter[0],kDouble,fNelem},
     // this is from Xhodo.param...
          {"hodo_slop",&tmpdouble[0],kDouble,fTotPlanes},
     {0}
    };
   gHcParms->LoadParmValues((DBRequest*)&list,prefix);
   // fetch the parameter from the temporary list
   fHodoSlop=tmpdouble[fPlaneNum-1];
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
    {"postdchits", "List of Positive TDC hits", 
     "frPosTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"negtdchits", "List of Negative TDC hits", 
     "frNegTDCHits.THcSignalHit.GetPaddleNumber()"},
    {"posadchits", "List of Positive ADC hits", 
     "frPosADCHits.THcSignalHit.GetPaddleNumber()"},
    {"negadchits", "List of Negative ADC hits", 
     "frNegADCHits.THcSignalHit.GetPaddleNumber()"},
    {"fptime", "Time at focal plane", 
     "GetFpTime()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcScintillatorPlane::Clear( Option_t* )
{
  //cout << " Calling THcScintillatorPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();
  frPosTDCHits->Clear();
  frNegTDCHits->Clear();
  frPosADCHits->Clear();
  frNegADCHits->Clear();
  fpTime = -1.e4;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcScintillatorPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::CoarseProcess( TClonesArray& tracks )
{
 
  cout <<"*******************************\n";
  cout <<"NOW IN THcScintilatorPlane::CoarseProcess!!!!\n";
  cout <<"*******************************\n";
  //  HitCount();

 return 0;
}

//_____________________________________________________________________________
Int_t THcScintillatorPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.
  // GN: Select only events that have at least one of their TDC signals in the
  // right range. 
  // Also subtract the pedestals from the adc signal (as per the ENGINE) we're not checking here
  // if the actual ADC is larger than the pedestal value we subtract!!

  Double_t mintdc, maxtdc;
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
  Int_t nPosTDCHits=0;
  Int_t nNegTDCHits=0;
  Int_t nPosADCHits=0;
  Int_t nNegADCHits=0;
  fNScinHits=0;
  fPosTDCHits->Clear();
  fNegTDCHits->Clear();
  fPosADCHits->Clear();
  fNegADCHits->Clear();
  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
  mintdc=((THcHodoscope *)GetParent())->GetTdcMin();
  maxtdc=((THcHodoscope *)GetParent())->GetTdcMax();
  Int_t ihit = nexthit;

  //  cout << "THcScintillatorPlane: raw htis = " << nrawhits << endl;
  
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t padnum=hit->fCounter;

    Int_t index=padnum-1;
    if (hit->fTDC_pos > 0) 
      ((THcSignalHit*) frPosTDCHits->ConstructedAt(nrPosTDCHits++))->Set(padnum, hit->fTDC_pos);
    if (hit->fTDC_neg > 0) 
      ((THcSignalHit*) frNegTDCHits->ConstructedAt(nrNegTDCHits++))->Set(padnum, hit->fTDC_neg);
    if ((hit->fADC_pos-fPosPed[index]) >= 50) 
      ((THcSignalHit*) frPosADCHits->ConstructedAt(nrPosADCHits++))->Set(padnum, hit->fADC_pos-fPosPed[index]);
    if ((hit->fADC_neg-fNegPed[index]) >= 50) 
      ((THcSignalHit*) frNegADCHits->ConstructedAt(nrNegADCHits++))->Set(padnum, hit->fADC_neg-fNegPed[index]);
    // check TDC values
    if (((hit->fTDC_pos >= mintdc) && (hit->fTDC_pos <= maxtdc)) ||
	((hit->fTDC_neg >= mintdc) && (hit->fTDC_neg <= maxtdc))) {

      fGoodRawPadNum[fNScinHits] = hit->fCounter;
      
      //TDC positive hit
      THcSignalHit *sighit = (THcSignalHit*) fPosTDCHits->ConstructedAt(nPosTDCHits++);
      sighit->Set(padnum, hit->fTDC_pos);
      // TDC negative hit
      THcSignalHit *sighit2 = (THcSignalHit*) fNegTDCHits->ConstructedAt(nNegTDCHits++);
      sighit2->Set(padnum, hit->fTDC_neg);
      // ADC positive hit
      THcSignalHit *sighit3 = (THcSignalHit*) fPosADCHits->ConstructedAt(nPosADCHits++);
      sighit3->Set(padnum, hit->fADC_pos-fPosPed[index]);
      // ADC negative hit
      THcSignalHit *sighit4 = (THcSignalHit*) fNegADCHits->ConstructedAt(nNegADCHits++);
      sighit4->Set(padnum, hit->fADC_neg-fNegPed[index]);      
      fNScinHits++;
    }
    else {
    }
    ihit++;
  }

  //  cout << "THcScintillatorPlane: ihit = " << ihit << endl;

  return(ihit);
}
//________________________________________________________________________________

Int_t THcScintillatorPlane::PulseHeightCorrection()
{
  // Perform pulse height correction of the TDC values as in the original h_trans_scin
  // see original comments below
  /*! Calculate all corrected hit times and histogram
    ! This uses a copy of code below. Results are save in time_pos,neg
    ! including the z-pos. correction assuming nominal value of betap
    ! Code is currently hard-wired to look for a peak in the
    ! range of 0 to 100 nsec, with a group of times that all
    ! agree withing a time_tolerance of time_tolerance nsec. The normal
    ! peak position appears to be around 35 nsec (SOS0 or 31 nsec (HMS)
    ! NOTE: if want to find particles with beta different than
    !       reference particle, need to make sure this is big enough
    !       to accomodate difference in TOF for other particles
    ! Default value in case user hasn't defined something reasonable */
  Int_t i,j,index;
  Double_t mintdc, maxtdc,tdctotime,toftolerance,tmin;
  Double_t pos_ph[53],neg_ph[53],postime[53],negtime[53],scin_corrected_time[53]; // the 53 should go in a param file (was hmax_scin_hits originally)
  Bool_t keep_pos[53],keep_neg[53],two_good_times[53]; // are these all really needed?
  Double_t dist_from_center,scint_center,hit_position,time_pos[100],time_neg[100],hbeta_pcent;
  Int_t timehist[200],jmax,maxhit,nfound=0; // This seems as a pretty old-fashioned way of doing things. Is there a better way?


  // protect against spam events
  if (fNScinHits>1000) {
    cout <<"Too many hits "<<fNScinHits<<" in this event! Skipping!\n";
    return -1;
  }
  // zero out histogram 
  for (i=0;i<200;i++) {
    timehist[i]=0;
  }
  for (i=0;i<fMaxHits;i++) {
    keep_pos[i]=kFALSE;
    keep_neg[i]=kFALSE;
    two_good_times[i]=kFALSE;
  }

  mintdc=((THcHodoscope *)GetParent())->GetTdcMin();
  maxtdc=((THcHodoscope *)GetParent())->GetTdcMax();
  tdctotime=((THcHodoscope *)GetParent())->GetTdcToTime();
  toftolerance=((THcHodoscope *)GetParent())->GetTofTolerance();
  //  hbeta_pcent=(TH((THcHodoscope *)GetParent())->GetParent()
  // Horrible hack until I find out where to get the central beta from momentum!! GN
  hbeta_pcent=1.0;
  fpTime=-1e5;
  for (i=0;i<fNScinHits;i++) {
    if ((((THcSignalHit*) fPosTDCHits->At(i))->GetData()>=mintdc) &&
	(((THcSignalHit*) fPosTDCHits->At(i))->GetData()<=maxtdc) &&
	(((THcSignalHit*) fNegTDCHits->At(i))->GetData()>=mintdc) &&
	(((THcSignalHit*) fNegTDCHits->At(i))->GetData()<=maxtdc)) {
	  pos_ph[i]=((THcSignalHit*) fPosADCHits->At(i))->GetData();
	  postime[i]=((THcSignalHit*) fPosTDCHits->At(i))->GetData()*tdctotime;
	  j=((THcSignalHit*)fPosTDCHits->At(i))->GetPaddleNumber()-1;
	  index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
	  postime[i]=postime[i]-((THcHodoscope *)GetParent())->GetHodoPosPhcCoeff(index)*
	    TMath::Sqrt(TMath::Max(0.,(pos_ph[i]/((THcHodoscope *)GetParent())->GetHodoPosMinPh(index)-1)));
	  postime[i]=postime[i]-((THcHodoscope *)GetParent())->GetHodoPosTimeOffset(index);

	  neg_ph[i]=((THcSignalHit*) fNegADCHits->At(i))->GetData();
	  negtime[i]=((THcSignalHit*) fNegTDCHits->At(i))->GetData()*tdctotime;
	  j=((THcSignalHit*)fNegTDCHits->At(i))->GetPaddleNumber()-1;
	  index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
	  negtime[i]=negtime[i]-((THcHodoscope *)GetParent())->GetHodoNegPhcCoeff(index)*
	    TMath::Sqrt(TMath::Max(0.,(neg_ph[i]/((THcHodoscope *)GetParent())->GetHodoNegMinPh(index)-1)));
	  negtime[i]=negtime[i]-((THcHodoscope *)GetParent())->GetHodoNegTimeOffset(index);

	  // ***************
	  ///	  cout <<"hcana i = "<<i<<endl;
	  ///  cout <<"hcana tdc_pos = "<<((THcSignalHit*) fPosTDCHits->At(i))->GetData()<<endl;
	  ///cout <<"hcana pos_ph = "<<pos_ph[i]<<endl;
	  //	  cout <<"hcana pos_phc_coeff = "<<((THcHodoscope *)GetParent())->GetHodoPosPhcCoeff(index)<<endl;
	  //	  cout <<"hcana postime = "<<postime[i]<<endl;
	  //cout <<"hcana negtime = "<<negtime[i]<<endl;
	  //*************

	  // Find hit position.  If postime larger, then hit was nearer negative side.
	  dist_from_center=0.5*(negtime[i]-postime[i])*((THcHodoscope *)GetParent())->GetHodoVelLight(index);
	  scint_center=0.5*(fPosLeft+fPosRight);
	  hit_position=scint_center+dist_from_center;
	  hit_position=TMath::Min(hit_position,fPosLeft);
	  hit_position=TMath::Max(hit_position,fPosRight);
	  postime[i]=postime[i]-(fPosLeft-hit_position)/((THcHodoscope *)GetParent())->GetHodoVelLight(index);
	  negtime[i]=negtime[i]-(hit_position-fPosRight)/((THcHodoscope *)GetParent())->GetHodoVelLight(index);

	  time_pos[i]=postime[i]-(fZpos+(j%2)*fDzpos)/(29.979*hbeta_pcent);
	  time_neg[i]=negtime[i]-(fZpos+(j%2)*fDzpos)/(29.979*hbeta_pcent);
	  nfound++;
	  for (int k=0;k<200;k++) {
	    tmin=0.5*k;
	    if ((time_pos[i]> tmin) && (time_pos[i] < tmin+toftolerance)) {
	      timehist[k]++;
	    }
	    if ((time_neg[i]> tmin) && (time_neg[i] < tmin+toftolerance)) {
	      timehist[k]++;
	    }
	  }
	  nfound++;
    }
  }
  // Find the bin with most hits
  jmax=0;
  maxhit=0;
  for (i=0;i<200;i++) {
    if (timehist[i]>maxhit) {
      jmax=i;
      maxhit=timehist[i];
    }
  }
  if (jmax>=0) {
    tmin=0.5*jmax;
    for (i=0;i<fNScinHits;i++) {
      if ((time_pos[i]>tmin) && (time_pos[i]<tmin+toftolerance)) {
	keep_pos[i]=kTRUE;
      }
      if ((time_neg[i]>tmin) && (time_neg[i]<tmin+toftolerance)) {
	keep_neg[i]=kTRUE;
      }
    }
  }
  // Resume regular tof code, now using time filer(?) from above
  // Check for TWO good TDC hits
  for (i=0;i<fNScinHits;i++) {
    if ((((THcSignalHit*) fPosTDCHits->At(i))->GetData()>=mintdc) &&
	(((THcSignalHit*) fPosTDCHits->At(i))->GetData()<=maxtdc) &&
	(((THcSignalHit*) fNegTDCHits->At(i))->GetData()>=mintdc) &&
	(((THcSignalHit*) fNegTDCHits->At(i))->GetData()<=maxtdc) &&
	keep_pos[i] && keep_neg[i]) {
      two_good_times[i]=kTRUE;
    }
  } // end of loop that finds tube setting time
  for (i=0;i<fNScinHits;i++) {
    if (two_good_times[i]) { // both tubes fired
      // correct time for everything except veloc. correction in order
      // to find hit location from difference in TDC.
      pos_ph[i]=((THcSignalHit*) fPosADCHits->At(i))->GetData();
      postime[i]=((THcSignalHit*) fPosTDCHits->At(i))->GetData()*tdctotime;
      j=((THcSignalHit*)fPosTDCHits->At(i))->GetPaddleNumber()-1;
      index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
      postime[i]=postime[i]-((THcHodoscope *)GetParent())->GetHodoPosPhcCoeff(index)*
	TMath::Sqrt(TMath::Max(0.,(pos_ph[i]/((THcHodoscope *)GetParent())->GetHodoPosMinPh(index)-1)));
      postime[i]=postime[i]-((THcHodoscope *)GetParent())->GetHodoPosTimeOffset(index);
      //
      neg_ph[i]=((THcSignalHit*) fNegADCHits->At(i))->GetData();
      negtime[i]=((THcSignalHit*) fNegTDCHits->At(i))->GetData()*tdctotime;
      j=((THcSignalHit*)fNegTDCHits->At(i))->GetPaddleNumber()-1;
      index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
      negtime[i]=negtime[i]-((THcHodoscope *)GetParent())->GetHodoNegPhcCoeff(index)*
	TMath::Sqrt(TMath::Max(0.,(neg_ph[i]/((THcHodoscope *)GetParent())->GetHodoNegMinPh(index)-1)));
      negtime[i]=negtime[i]-((THcHodoscope *)GetParent())->GetHodoNegTimeOffset(index);
      // find hit position. If postime larger, then hit was nearer negative side
      dist_from_center=0.5*(negtime[i]-postime[i])*((THcHodoscope *)GetParent())->GetHodoVelLight(index);
      scint_center=0.5*(fPosLeft+fPosRight);
      hit_position=scint_center+dist_from_center;
      hit_position=TMath::Min(hit_position,fPosLeft);
      hit_position=TMath::Max(hit_position,fPosRight);
      postime[i]=postime[i]-(fPosLeft-hit_position)/((THcHodoscope *)GetParent())->GetHodoVelLight(index);
      negtime[i]=negtime[i]-(hit_position-fPosRight)/((THcHodoscope *)GetParent())->GetHodoVelLight(index);
      scin_corrected_time[i]=0.5*(postime[i]+negtime[i]);
    }
    else { // only one tube fired
      scin_corrected_time[i]=0.0; // not a very good "flag" but there is the logical two_good_hits...
      // no fpTimes for U!
    }
  }
  //start time calculation.  assume xp=yp=0 radians.  project all
  //time values to focal plane.  use average for start time.

  fNScinGoodHits=0;
  for (i=0;i<fNScinHits;i++) {
    j=((THcSignalHit*)fNegTDCHits->At(i))->GetPaddleNumber()-1;  
    index=((THcHodoscope *)GetParent())->GetScinIndex(fPlaneNum-1,j);
    if (two_good_times[i]) { // both tubes fired
      fpTimes[fNScinGoodHits]=scin_corrected_time[i]-(fZpos+(j%2)*fDzpos)/(29.979*hbeta_pcent);
      fScinTime[fNScinGoodHits]=scin_corrected_time[i];
      fScinSigma[fNScinGoodHits]=TMath::Sqrt(((THcHodoscope *)GetParent())->GetHodoPosSigma(index)*((THcHodoscope *)GetParent())->GetHodoPosSigma(index)+((THcHodoscope *)GetParent())->GetHodoNegSigma(index)*((THcHodoscope *)GetParent())->GetHodoNegSigma(index)); // not ideal by any stretch!!!
      fScinZpos[fNScinGoodHits]=fZpos+(j%2)*fDzpos; // see comment above
      //        h_rfptime(hscin_plane_num(ihit))=fptime
      fNScinGoodHits++; // increment the number of good hits
    }
  }
  CalcFpTime();
  return 0;
}
//_____________________________________________________________________________
Int_t THcScintillatorPlane::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this plane from hit list, accumulating into
  // arrays for calculating pedestals.

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcScintillatorPlane::AcculatePedestals " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;

  Int_t ihit = nexthit;
  while(ihit < nrawhits) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t element = hit->fCounter - 1; // Should check if in range
    Int_t adcpos = hit->fADC_pos;
    Int_t adcneg = hit->fADC_neg;

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

    //    cout <<"Pedestals "<< i+1 << " " << fPosPed[i] << " " << fNegPed[i] << endl;
  }
  //  cout << " " << endl;
  
}

//_____________________________________________________________________________
void THcScintillatorPlane::InitializePedestals( )
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
}
//____________________________________________________________________________
Int_t THcScintillatorPlane::GetNelem() 
{
  return fNelem;
}
//____________________________________________________________________________
Int_t THcScintillatorPlane::GetNScinHits() 
{
  return fNScinHits;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetSpacing()
{
  return fSpacing;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetSize()
{
  return fSize;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetHodoSlop()
{
  return fHodoSlop;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetZpos()
{
  return fZpos;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetDzpos()
{
  return fDzpos;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosLeft() {
  return fPosLeft;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosRight() {
  return fPosRight;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosOffset() {
  return fPosOffset;
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::GetPosCenter(Int_t PaddleNo) {
  return fPosCenter[PaddleNo];
}
//____________________________________________________________________________
Double_t THcScintillatorPlane::CalcFpTime() 
{
  Double_t tmp=0;
  Int_t i,counter=0;
  for (i=0;i<fNScinGoodHits;i++) {
    if (TMath::Abs(fpTimes[i]-((THcHodoscope *)GetParent())->GetStartTimeCenter())<=((THcHodoscope *)GetParent())->GetStartTimeSlop()) {
      tmp+=fpTimes[i];
      counter++;
    }
  }
  if (counter>0) {
    fpTime=tmp/counter;
  } else {
    fpTime=-10000.;
  }
  return fpTime;
}
//____________________________________________________________________________
ClassImp(THcScintillatorPlane)
////////////////////////////////////////////////////////////////////////////////

