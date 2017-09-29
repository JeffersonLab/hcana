/** \class THcDriftChamberPlane
    \ingroup DetSupport

Class for a a single Hall C horizontal drift chamber plane

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
  : THaSubDetector(name,description,parent)
{
  // Normal constructor with name and description
  fHits = new TClonesArray("THcDCHit",100);
  fWires = new TClonesArray("THcDCWire", 100);
  fTTDConv = NULL;

  fPlaneNum = planenum;
}

//_____________________________________________________________________________
THcDriftChamberPlane::THcDriftChamberPlane() :
  THaSubDetector()
{
  // Constructor
  fHits = NULL;
  fWires = NULL;
  fTTDConv = NULL;
}
//______________________________________________________________________________
THcDriftChamberPlane::~THcDriftChamberPlane()
{
  // Destructor
  delete fWires;
  delete fHits;
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

  // See what file it looks for

  char prefix[2];
  UInt_t NumDriftMapBins;
  Double_t DriftMapFirstBin;
  Double_t DriftMapBinSize;
  fUsingTzeroPerWire=0;
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';
  DBRequest list[]={
    {"driftbins", &NumDriftMapBins, kInt},
    {"drift1stbin", &DriftMapFirstBin, kDouble},
    {"driftbinsz", &DriftMapBinSize, kDouble},
    {"_using_tzero_per_wire", &fUsingTzeroPerWire, kInt,0,1},
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

  fNSperChan = fParent->GetNSperChan();

  if (fUsingTzeroPerWire==1) {
  fTzeroWire = new Double_t [fNWires];
  DBRequest list3[]={
    {Form("tzero%s",GetName()),fTzeroWire,kDouble,(UInt_t) fNWires},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list3,prefix);
    // printf(" using tzero per wire plane = %s  nwires = %d  \n",GetName(),fNWires);
    //   for (Int_t iw=0;iw < fNWires;iw++) {
    // 	//	printf("%d  %f ",iw+1,fTzeroWire[iw]) ;
    // 	if ( iw!=0 && iw%8 == 0) printf("\n") ;
    // 	}
  } else {
  fTzeroWire = new Double_t [fNWires];
  for (Int_t iw=0;iw < fNWires;iw++) {
    fTzeroWire[iw]=0.0;
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

  if(abs(cosalpha) <= 0.707) { // x-like wire, need dist from x=0 line
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
    new((*fWires)[i]) THcDCWire( i+1, pos , 0.0, fTTDConv);
    //if( something < 0 ) wire->SetFlag(1);
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
    {"wirenum", "List of TDC wire number",
     "fHits.THcDCHit.GetWireNum()"},
    {"rawtdc", "Raw TDC Values",
     "fHits.THcDCHit.GetRawTime()"},
    {"time","Drift times",
     "fHits.THcDCHit.GetTime()"},
    {"dist","Drift distancess",
     "fHits.THcDCHit.GetDist()"},
    {"nhit", "Number of hits", "GetNHits()"},
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
  // Extract the data for this plane from hit list
  // Assumes that the hit list is sorted by plane, so we stop when the
  // plane doesn't agree and return the index for the next hit.

  Double_t StartTime = 0.0;
  // Would be nice to have a way to determine that the hodoscope decode was
  // actually called for this event.
  if( fglHod ) StartTime = fglHod->GetStartTime();
  //cout << "Start time " << StartTime << endl;

  //Int_t nTDCHits=0;
  fHits->Clear();

  Int_t nrawhits = rawhits->GetLast()+1;
  // cout << "THcDriftChamberPlane::ProcessHits " << fPlaneNum << " " << nexthit << "/" << nrawhits << endl;
  fNRawhits=0;
  Int_t ihit = nexthit;
  Int_t nextHit = 0;
  while(ihit < nrawhits) {
    THcRawDCHit* hit = (THcRawDCHit *) rawhits->At(ihit);
    if(hit->fPlane > fPlaneNum) {
      break;
    }
    Int_t wireNum = hit->fCounter;
    THcDCWire* wire = GetWire(wireNum);
    // Int_t reftime = hit->GetReference(0);
    for(UInt_t mhit=0; mhit<hit->GetRawTdcHit().GetNHits(); mhit++) {
      fNRawhits++;
      /* Sort into early, late and ontime */
      Int_t rawtdc = hit->GetRawTdcHit().GetTime(mhit); // Get the ref time subtracted time
      if(rawtdc < fTdcWinMin) {
	// Increment early counter  (Actually late because TDC is backward)
      } else if (rawtdc > fTdcWinMax) {
	// Increment late count
      } else {
	Double_t time = -StartTime   // (comes from h_trans_scin
	  - rawtdc*fNSperChan + fPlaneTimeZero - fTzeroWire[wireNum-1]; // fNSperChan > 0 for 1877
	// (cout << " Plane = " << GetName() << " wire = " << wireNum << " " <<  fPlaneTimeZero << " "  << fTzeroWire[wireNum-1] << endl;
	// < 0 for Caen1190.
	//	  - (rawtdc-reftime)*fNSperChan + fPlaneTimeZero;
	// How do we get this start time from the hodoscope to here
	// (or at least have it ready by coarse process)
	new( (*fHits)[nextHit++] ) THcDCHit(wire, rawtdc, time, this);
	break;			// Take just the first hit in the time window
      }
    }
    ihit++;
  }
  return(ihit);
}
