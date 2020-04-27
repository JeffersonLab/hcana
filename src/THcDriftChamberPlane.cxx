/** \class THcDriftChamberPlane
    \ingroup DetSupport

    \brief Class for a a single Hall C horizontal drift chamber plane

*/

#include "THcDC.h"
#include "THcDriftChamberPlane.h"
#include "THcDCWire.h"
#include "THcDCHit.h"
#include "THcDCLookupTTDConv.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THaApparatus.h"
#include "THcHodoscope.h"
#include "TClass.h"


#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcDriftChamberPlane)

//______________________________________________________________________________
THcDriftChamberPlane::THcDriftChamberPlane( const char* name,
					    const char* description,
					    const Int_t planenum,
					    THaDetectorBase* parent )
: THaSubDetector(name,description,parent), fTTDConv(0)
{
  // Normal constructor with name and description
  fHits = new TClonesArray("THcDCHit",100);
  fRawHits = new TClonesArray("THcDCHit",100);
  fWires = new TClonesArray("THcDCWire", 100);

  fPlaneNum = planenum;
}

//_____________________________________________________________________________
THcDriftChamberPlane::THcDriftChamberPlane() :
  THaSubDetector()
{
  // Constructor
  fHits = NULL;
  fRawHits = NULL;
  fWires = NULL;
  fTTDConv = NULL;
}
//______________________________________________________________________________
THcDriftChamberPlane::~THcDriftChamberPlane()
{
  // Destructor
  delete [] fTzeroWire;
  delete [] fSigmaWire;
  delete fWires;
  delete fHits;
  delete fRawHits;
  delete fTTDConv;

}
THaAnalysisObject::EStatus THcDriftChamberPlane::Init( const TDatime& date )
{
  // Extra initialization for scintillator plane: set up DataDest map

  // cout << "THcDriftChamberPlane::Init called " << GetName() << endl;

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
Int_t THcDriftChamberPlane::ReadDatabase( const TDatime& date )
{

  /**
     Load the drift time to drift distance map.

     Retrieve geometry parameters from main drift chamber detector object (THcDC)
  */
  
  char prefix[2];
  UInt_t NumDriftMapBins;
  Double_t DriftMapFirstBin;
  Double_t DriftMapBinSize;
  fUsingTzeroPerWire=0;
  fUsingSigmaPerWire=0;
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';
  DBRequest list[]={
    {"driftbins", &NumDriftMapBins, kInt},
    {"drift1stbin", &DriftMapFirstBin, kDouble},
    {"driftbinsz", &DriftMapBinSize, kDouble},
    {"_using_tzero_per_wire", &fUsingTzeroPerWire, kInt,0,1},
    {"_using_sigma_per_wire", &fUsingSigmaPerWire, kInt,0,1},
    {0}
  };



  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  Double_t *DriftMap = new Double_t[NumDriftMapBins];
  DBRequest list2[]={
    {Form("wc%sfract",GetName()),DriftMap,kDouble,NumDriftMapBins},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list2,prefix);


  // Retrieve parameters we need from parent class
  THcDC* fParent;

  fParent = (THcDC*) GetParent();
  // These are single variables here, but arrays in THcDriftChamber.
  fSigma = fParent->GetSigma(fPlaneNum);
  fChamberNum = fParent->GetNChamber(fPlaneNum);
  fNWires = fParent->GetNWires(fPlaneNum);
  fWireOrder = fParent->GetWireOrder(fPlaneNum);
  fPitch = fParent->GetPitch(fPlaneNum);
  fCentralWire = fParent->GetCentralWire(fPlaneNum);
  fTdcWinMin = fParent->GetTdcWinMin(fPlaneNum);
  fTdcWinMax = fParent->GetTdcWinMax(fPlaneNum);
  fPlaneTimeZero = fParent->GetPlaneTimeZero(fPlaneNum);
  fCenter = fParent->GetCenter(fPlaneNum);
  fCentralTime = fParent->GetCentralTime(fPlaneNum);
  fDriftTimeSign = fParent->GetDriftTimeSign(fPlaneNum);
  fReadoutLR = fParent->GetReadoutLR(fPlaneNum);
  fReadoutTB = fParent->GetReadoutTB(fPlaneNum);
  fVersion = fParent->GetVersion();

  fNSperChan = fParent->GetNSperChan();


  fTzeroWire = new Double_t [fNWires];
  fSigmaWire = new Double_t [fNWires];


  if (fUsingTzeroPerWire==1) {
     DBRequest list3[]={
    {Form("tzero%s",GetName()),fTzeroWire,kDouble,(UInt_t) fNWires},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list3,prefix);

  } else {
  for (Int_t iw=0;iw < fNWires;iw++) {
    fTzeroWire[iw]=0.0;
    } 
  }

  if (fUsingSigmaPerWire==1) {
    DBRequest list4[]={
      {Form("wire_sigma%s",GetName()),fSigmaWire,kDouble,(UInt_t) fNWires},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list4,prefix);
  }  else {
    for (Int_t iw=0;iw < fNWires;iw++) {
      fSigmaWire[iw]=fSigma;
    }
  }
  // Calculate Geometry Constants
  // Do we want to move all this to the Chamber of DC Package leve
  // as that is where these things will be needed?
  Double_t z0 = fParent->GetZPos(fPlaneNum);
  Double_t alpha = fParent->GetAlphaAngle(fPlaneNum);
  Double_t beta = fParent->GetBetaAngle(fPlaneNum);
  fBeta = beta;
  Double_t gamma = fParent->GetGammaAngle(fPlaneNum);
  Double_t cosalpha = TMath::Cos(alpha);
  Double_t sinalpha = TMath::Sin(alpha);
  Double_t cosbeta = TMath::Cos(beta);
  Double_t sinbeta = TMath::Sin(beta);
  Double_t cosgamma = TMath::Cos(gamma);
  Double_t singamma = TMath::Sin(gamma);

  Double_t hzchi = -cosalpha*sinbeta + sinalpha*cosbeta*singamma;
  Double_t hzpsi =  sinalpha*sinbeta + cosalpha*cosbeta*singamma;
  Double_t hxchi = -cosalpha*cosbeta - sinalpha*sinbeta*singamma;
  Double_t hxpsi =  sinalpha*cosbeta - cosalpha*sinbeta*singamma;
  Double_t hychi =  sinalpha*cosgamma;
  Double_t hypsi =  cosalpha*cosgamma;
  Double_t stubxchi = -cosalpha;
  Double_t stubxpsi = sinalpha;
  Double_t stubychi = sinalpha;
  Double_t stubypsi = cosalpha;

  if(cosalpha <= 0.707) { // x-like wire, need dist from x=0 line
    fReadoutX = 1;
    fReadoutCorr = 1/sinalpha;
  } else {
    fReadoutX = 0;
    fReadoutCorr = 1/cosalpha;
  }

  Double_t sumsqupsi = hzpsi*hzpsi+hxpsi*hxpsi+hypsi*hypsi;
  Double_t sumsquchi = hzchi*hzchi+hxchi*hxchi+hychi*hychi;
  Double_t sumcross = hzpsi*hzchi + hxpsi*hxchi + hypsi*hychi;
  Double_t denom1 = sumsqupsi*sumsquchi-sumcross*sumcross;
  fPsi0 = (-z0*hzpsi*sumsquchi
		    +z0*hzchi*sumcross) / denom1;
  Double_t hchi0 = (-z0*hzchi*sumsqupsi
		    +z0*hzpsi*sumcross) / denom1;
  Double_t hphi0 = TMath::Sqrt(pow(z0+hzpsi*fPsi0+hzchi*hchi0,2)
			       + pow(hxpsi*fPsi0+hxchi*hchi0,2)
			       + pow(hypsi*fPsi0+hychi*hchi0,2) );
  if(z0 < 0.0) hphi0 = -hphi0;
  
  Double_t denom2 = stubxpsi*stubychi - stubxchi*stubypsi;

  // Why are there 4, but only 3 used?
  fStubCoef[0] = stubychi/(fSigma*denom2);   // sin(a)/sigma
  fStubCoef[1] = -stubxchi/(fSigma*denom2);   // cos(a)/sigma
  fStubCoef[2] = hphi0*fStubCoef[0];     // z0*sin(a)/sig
  fStubCoef[3] = hphi0*fStubCoef[1];     // z0*cos(a)/sig

  fXsp = hychi/denom2;		// sin(a)
  fYsp = -hxchi/denom2;		// cos(a)

  // Comput track fitting coefficients
#define LOCRAYZT 0.0
  fPlaneCoef[0]= hzchi;		      // = 0.
  fPlaneCoef[1]=-hzchi;		      // = 0.
  fPlaneCoef[2]= hychi*(z0-LOCRAYZT);  // sin(a)*(z-hlocrayzt)
  fPlaneCoef[3]= hxchi*(LOCRAYZT-z0);  // cos(a)*(z-hlocrayzt)
  fPlaneCoef[4]= hychi;		       // sin(a)
  fPlaneCoef[5]=-hxchi;		       // cos(a)
  fPlaneCoef[6]= hzchi*hypsi - hychi*hzpsi; // 0.
  fPlaneCoef[7]=-hzchi*hxpsi + hxchi*hzpsi; // 0.
  fPlaneCoef[8]= hychi*hxpsi - hxchi*hypsi; // 1.

  //  cout << fPlaneNum << " " << fNWires << " " << fWireOrder << endl;

  fTTDConv = new THcDCLookupTTDConv(DriftMapFirstBin,fPitch/2,DriftMapBinSize,
				    NumDriftMapBins,DriftMap);
  delete [] DriftMap;

  Int_t nWires = fParent->GetNWires(fPlaneNum);
  // For HMS, wire numbers start with one, but arrays start with zero.
  // So wire number is index+1
  for (int i=0; i<nWires; i++) {
    Double_t pos = fPitch*( (fWireOrder==0?(i+1):fNWires-i)
			    - fCentralWire) - fCenter;
    Int_t readoutside = GetReadoutSide(i+1);
    new((*fWires)[i]) THcDCWire( i+1, pos , fTzeroWire[i], fSigmaWire[i], readoutside, fTTDConv);    //added fTzeroWire/fSigmaWire to be read in as fTOffset --Carlos
  }
  
  THaApparatus* app = GetApparatus();
  const char* nm = "hod";
  if(  !app ||
       !(fglHod = dynamic_cast<THcHodoscope*>(app->GetDetector(nm))) ) {
    static const char* const here = "ReadDatabase()";
    Warning(Here(here),"Hodoscope \"%s\" not found. "
	    "Event-by-event time offsets will NOT be used!!",nm);
  }
  
  return kOK;
}  
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  //  cout << "THcDriftChamberPlane::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"raw.wirenum", "List of TDC wire number of all hits in DC",
     "fRawHits.THcDCHit.GetWireNum()"},
    {"wirenum", "List of TDC wire number (select first hit in TDc window",
     "fHits.THcDCHit.GetWireNum()"},
    {"rawnorefcorrtdc", "Raw TDC Values",
     "fHits.THcDCHit.GetRawNoRefCorrTime()"},
    {"rawtdc", "Raw TDC with reference time subtracted Values",
     "fHits.THcDCHit.GetRawTime()"},
    {"time","Drift times",
     "fHits.THcDCHit.GetTime()"},
    {"dist","Drift distancess",
     "fHits.THcDCHit.GetDist()"},
    {"nhit", "Number of hits", "GetNHits()"},
    {"RefTime", "TDC reference time", "fTdcRefTime"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcDriftChamberPlane::Clear( Option_t* )
{
  //cout << " Calling THcDriftChamberPlane::Clear " << GetName() << endl;
  // Clears the hit lists
  fHits->Clear();
  fRawHits->Clear();
}

//_____________________________________________________________________________
Int_t THcDriftChamberPlane::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead
  cout << " Calling THcDriftChamberPlane::Decode " << GetName() << endl;

  return 0;
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::CoarseProcess( TClonesArray& tracks )
{

  //  HitCount();

 return 0;
}
//
Double_t THcDriftChamberPlane::CalcWireFromPos(Double_t pos) {
  Double_t wire_num_calc=-1000;
  if (fWireOrder==0) wire_num_calc = (pos+fCenter)/(fPitch)+fCentralWire;
  if (fWireOrder==1) wire_num_calc = 1-((pos+fCenter)/(fPitch)+fCentralWire-fNWires);
  return(wire_num_calc);
}
//_____________________________________________________________________________
Int_t THcDriftChamberPlane::FineProcess( TClonesArray& tracks )
{
  return 0;
}
Int_t THcDriftChamberPlane::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  /**
     Extract the data for this plane from hit list
     Assumes that the hit list is sorted by plane, so we stop when the
     plane doesn't agree and return the index for the next hit.
  */

  fHits->Clear();
  fRawHits->Clear();
  fTdcRefTime = kBig;
  Int_t nrawhits = rawhits->GetLast()+1;
  fNRawhits=0;
  Int_t ihit = nexthit;
  Int_t nextHit = 0;
  Int_t nextRawHit = 0;
  while(ihit < nrawhits) {
    THcRawDCHit* hit = (THcRawDCHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t wireNum = hit->fCounter;
    THcDCWire* wire = GetWire(wireNum);
    Bool_t First_Hit_In_Window = kTRUE;
    if (hit->GetRawTdcHit().HasRefTime()) fTdcRefTime = hit->GetRawTdcHit().GetRefTime();
    for(UInt_t mhit=0; mhit<hit->GetRawTdcHit().GetNHits(); mhit++) {
      fNRawhits++;
      /* Sort into early, late and ontime */
      Int_t rawnorefcorrtdc = hit->GetRawTdcHit().GetTimeRaw(mhit); // Get the ref time subtracted time
      Int_t rawtdc = hit->GetRawTdcHit().GetTime(mhit); // Get the ref time subtracted time
      Double_t time = - rawtdc*fNSperChan + fPlaneTimeZero - wire->GetTOffset(); // fNSperChan > 0 for 1877
      new( (*fRawHits)[nextRawHit++] ) THcDCHit(wire, rawnorefcorrtdc,rawtdc, time, this);	
     if(rawtdc < fTdcWinMin) {
	// Increment early counter  (Actually late because TDC is backward)
      } else if (rawtdc > fTdcWinMax) {
	// Increment late count
      } else {
	if (First_Hit_In_Window) {
	new( (*fHits)[nextHit++] ) THcDCHit(wire, rawnorefcorrtdc,rawtdc, time, this);
	First_Hit_In_Window = kFALSE;
	}
      }
    }
    ihit++;
  }
  return(ihit);
}
Int_t THcDriftChamberPlane::SubtractStartTime()
{
  Double_t StartTime = 0.0;
  if( fglHod ) StartTime = fglHod->GetStartTime();
  if (StartTime == -1000) StartTime = 0.0;
  for(Int_t ihit=0;ihit<GetNHits();ihit++) { 
    THcDCHit *thishit = (THcDCHit*) fHits->At(ihit);
    Double_t temptime= thishit->GetTime()-StartTime;
    thishit->SetTime(temptime);
    thishit->ConvertTimeToDist();
  }
  return 0;
}
Int_t THcDriftChamberPlane::GetReadoutSide(Int_t wirenum)
{
  Int_t readoutside;
  //if new HMS
  if (fVersion == 1) {
    if ((fPlaneNum>=3 && fPlaneNum<=4) || (fPlaneNum>=9 && fPlaneNum<=10)) {
      if (fReadoutTB>0) {
	if (wirenum < 60) {
	  readoutside = 2;
	} else {
	  readoutside = 4;
	}
      } else {
	if (wirenum < 44) {
	  readoutside = 4;
	} else {
	  readoutside = 2;
	}
      }
    } else {
      if (fReadoutTB>0) {
	if (wirenum < 51) {
	  readoutside = 2;
	} else if (wirenum >= 51 && wirenum <= 64) {
	  readoutside = 1;
	} else {
	  readoutside =4;
	}
      } else {
	if (wirenum < 33) {
	  readoutside = 4;
	} else if (wirenum >=33 && wirenum<=46) {
	  readoutside = 1;
	} else {
	  readoutside = 2;
	}
      }
    }
  } else {//appplies SHMS DC configuration
    //check if x board
    if ((fPlaneNum>=3 && fPlaneNum<=4) || (fPlaneNum>=9 && fPlaneNum<=10)) {
      if (fReadoutTB>0) {
	if (wirenum < 49) {
	  readoutside = 4;
	} else {
	  readoutside = 2;
	}
      } else {
	if (wirenum < 33) {
	  readoutside = 2;
	} else {
	  readoutside = 4;
	}
      }
    } else { //else is u board
      if (fReadoutTB>0) {
	if (wirenum < 41) {
	  readoutside = 4;
	} else if (wirenum >= 41 && wirenum <= 63) {
	  readoutside = 3;
	} else if (wirenum >=64 && wirenum <=69) {
	  readoutside = 1;
	} else {
	  readoutside = 2;
	}
      } else {
	if (wirenum < 39) {
	  readoutside = 2;
	} else if (wirenum >=39 && wirenum<=44) {
	  readoutside = 1;
	} else if (wirenum>=45 && wirenum<=67) {
	  readoutside = 3;
	} else {
	  readoutside = 4;
	}
      }
    }
  }
  return(readoutside);
}
