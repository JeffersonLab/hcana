/** \class THcHodoEff
    \ingroup PhysMods

\brief Class for accumulating statistics for and calculating hodoscope efficiencies.

*/
#include "THaEvData.h"
#include "THaCutList.h"
#include "VarDef.h"
#include "VarType.h"
#include "TClonesArray.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "THcHodoEff.h"
#include "THaApparatus.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"

using namespace std;

//_____________________________________________________________________________
THcHodoEff::THcHodoEff (const char *name, const char* description,
			const char* hodname) :
  THaPhysicsModule(name, description), fName(hodname), fHod(NULL), fNevt(0)
{

}

//_____________________________________________________________________________
THcHodoEff::~THcHodoEff()
{
  // Destructor

  delete [] fPlanes; fPlanes = 0;
  delete [] fPosZ; fPosZ = 0;
  delete [] fSpacing; fSpacing = 0;
  delete [] fCenterFirst; fCenterFirst = 0;
  delete [] fNCounters; fNCounters = 0;
  delete [] fHodoSlop; fHodoSlop = 0;
  delete [] fStatTrkSum; fStatTrkSum = 0;
  delete [] fStatAndSum; fStatAndSum = 0;
  delete [] fStatAndEff; fStatAndEff = 0;

  delete [] fHodoPosEffi; fHodoPosEffi = 0;
  delete [] fHodoNegEffi; fHodoNegEffi = 0;
  delete [] fHodoOrEffi; fHodoOrEffi = 0;
  delete [] fHodoAndEffi; fHodoAndEffi = 0;
  delete [] fStatTrk; fStatTrk = 0;

  delete [] fHitPlane; fHitPlane = 0;

  RemoveVariables();
}

//_____________________________________________________________________________
void THcHodoEff::Reset( Option_t* opt )
// Clear event-by-event data
{
  Clear(opt);
}

//_____________________________________________________________________________
Int_t THcHodoEff::Begin( THaRunBase* )
{
  // Start of analysis

  if (!IsOK() ) return -1;

  // Book any special histograms here

  fNevt = 0;

  // Clear all the accumulators here
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fHitPlane[ip] = 0;
    for(Int_t ic=0;ic<fNCounters[ip];ic++) {
      fStatPosHit[ip][ic] = 0;
      fStatNegHit[ip][ic] = 0;
      fStatAndHit[ip][ic] = 0;
      fStatOrHit[ip][ic] = 0;
      fBothGood[ip][ic] = 0;
      fPosGood[ip][ic] = 0;
      fNegGood[ip][ic] = 0;
      for(Int_t idel=0;idel<20;idel++) {
	fStatTrkDel[ip][ic][idel] = 0;
	fStatAndHitDel[ip][ic][idel] = 0;
      }
    }
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoEff::End( THaRunBase* )
{
  // End of analysis
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fStatAndEff[ip]=0;
    for(Int_t ic=0;ic<fNCounters[ip];ic++) {
      fStatTrkSum[ip]+=fStatTrk[fHod->GetScinIndex(ip,ic)];
      fStatAndSum[ip]+=fHodoAndEffi[fHod->GetScinIndex(ip,ic)];
    }
    if (fStatTrkSum[ip] !=0) fStatAndEff[ip]=float(fStatAndSum[ip])/float(fStatTrkSum[ip]);
  }
  //
  Double_t     p1=fStatAndEff[0];
  Double_t     p2=fStatAndEff[1];
  Double_t     p3=fStatAndEff[2];
  Double_t     p4=fStatAndEff[3];
  // probability that ONLY the listed planes had triggers
  Double_t     p1234= p1*p2*p3*p4;
  Double_t     p123 = p1*p2*p3*(1.-p4);
  Double_t     p124 = p1*p2*(1.-p3)*p4;
  Double_t    p134 = p1*(1.-p2)*p3*p4;
  Double_t    p234 = (1.-p1)*p2*p3*p4;
  fHodoEff_s1 = 1. - ((1.-p1)*(1.-p2));
  fHodoEff_s2 = 1. - ((1.-p3)*(1.-p4));
  fHodoEff_tof=fHodoEff_s1 * fHodoEff_s2;
  fHodoEff_3_of_4=p1234+p123+p124+p134+p234;
  fHodoEff_4_of_4=p1234;
  return 0;
}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoEff::Init( const TDatime& run_time )
{
  // Initialize THcHodoEff physics module

  //  const char* const here = "Init";

  // Standard initialization. Calls ReadDatabase(), ReadRunDatabase(),
  // and DefineVariables() (see THaAnalysisObject::Init)

  fHod = dynamic_cast<THcHodoscope*>
    ( FindModule( fName.Data(), "THcHodoscope"));

  fSpectro = static_cast<THaSpectrometer*>(fHod->GetApparatus());

  if( THaPhysicsModule::Init( run_time ) != kOK )
    return fStatus;

  cout << "THcHodoEff::Init nplanes=" << fHod->GetNPlanes() << endl;
  cout << "THcHodoEff::Init Apparatus = " << fHod->GetName() <<
    " " <<
    (fHod->GetApparatus())->GetName() << endl;

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcHodoEff::ReadDatabase( const TDatime& date )
{
  // Read database. Gets variable needed for efficiency calculation
  // Get # of planes and their z positions here.

  fNPlanes = fHod->GetNPlanes();
  fPlanes = new THcScintillatorPlane* [fNPlanes];
  fPosZ = new Double_t[fNPlanes];
  fSpacing = new Double_t[fNPlanes];
  fCenterFirst = new Double_t[fNPlanes];
  fNCounters = new Int_t[fNPlanes];
  fHodoSlop = new Double_t[fNPlanes];
  fStatTrkSum = new Int_t[fNPlanes];
  fStatAndSum = new Int_t[fNPlanes];
  fStatAndEff = new Double_t[fNPlanes];

  Int_t maxcountersperplane=0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fStatTrkSum[ip]=0.;
    fStatAndSum[ip]=0.;
    fStatAndEff[ip]=0.;
    fPlanes[ip] = fHod->GetPlane(ip);
    fPosZ[ip] = fPlanes[ip]->GetZpos() + 0.5*fPlanes[ip]->GetDzpos();
    fSpacing[ip] = fPlanes[ip]->GetSpacing();
    fCenterFirst[ip] = fPlanes[ip]->GetPosCenter(0) + fPlanes[ip]->GetPosOffset();
    fNCounters[ip] = fPlanes[ip]->GetNelem();
    maxcountersperplane = TMath::Max(maxcountersperplane,fNCounters[ip]);
  }
  Int_t totalpaddles = fNPlanes*maxcountersperplane;
  fHodoPosEffi = new Int_t[totalpaddles];
  fHodoNegEffi = new Int_t[totalpaddles];
  fHodoOrEffi = new Int_t[totalpaddles];
  fHodoAndEffi = new Int_t[totalpaddles];
  fStatTrk = new Int_t[totalpaddles];

  char prefix[2];
  prefix[0] = tolower((fHod->GetApparatus())->GetName()[0]);
  prefix[1] = '\0';

  DBRequest list[]={
    {"stat_slop", &fStatSlop, kDouble},
    {"stat_maxchisq",&fMaxChisq, kDouble},
    {"HodoEff_CalEnergy_Cut",&fHodoEff_CalEnergy_Cut, kDouble,0,1},
    {"hodo_slop", fHodoSlop, kDouble, (UInt_t)fNPlanes},
    {0}
  };
  fHodoEff_CalEnergy_Cut=0.050; // set default value
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  cout << "\n\nTHcHodoEff::ReadDatabase nplanes=" << fHod->GetNPlanes() << endl;
  // Setup statistics arrays
  // Better method to put this in?
  // These all need to be cleared in Begin
  fHitPlane = new Int_t[fNPlanes];
  fStatTrkDel.resize(fNPlanes);
  fStatAndHitDel.resize(fNPlanes);
  fStatPosHit.resize(fNPlanes);
  fStatNegHit.resize(fNPlanes);
  fStatAndHit.resize(fNPlanes);
  fStatOrHit.resize(fNPlanes);
  fBothGood.resize(fNPlanes);
  fPosGood.resize(fNPlanes);
  fNegGood.resize(fNPlanes);

  for(Int_t ip=0;ip<fNPlanes;ip++) {

    cout << "Plane = " << ip + 1 << "    counters = " << fNCounters[ip] << endl;

    fStatTrkDel[ip].resize(fNCounters[ip]);
    fStatAndHitDel[ip].resize(fNCounters[ip]);
    fStatPosHit[ip].resize(fNCounters[ip]);
    fStatNegHit[ip].resize(fNCounters[ip]);
    fStatAndHit[ip].resize(fNCounters[ip]);
    fStatOrHit[ip].resize(fNCounters[ip]);
    fBothGood[ip].resize(fNCounters[ip]);
    fPosGood[ip].resize(fNCounters[ip]);
    fNegGood[ip].resize(fNCounters[ip]);
    for(Int_t ic=0;ic<fNCounters[ip];ic++) {
      fStatTrkDel[ip][ic].resize(20); // Max this settable
      fStatAndHitDel[ip][ic].resize(20); // Max this settable
      
      fHodoPosEffi[fHod->GetScinIndex(ip,ic)] = 0;
      fHodoNegEffi[fHod->GetScinIndex(ip,ic)] = 0;
      fHodoOrEffi[fHod->GetScinIndex(ip,ic)] = 0;
      fHodoAndEffi[fHod->GetScinIndex(ip,ic)] = 0;
      fStatTrk[fHod->GetScinIndex(ip,ic)] = 0;
    }
  }

  // Int_t fHodPaddles = fNCounters[0];
  // gHcParms->Define(Form("%shodo_pos_hits[%d][%d]",fPrefix,fNPlanes,fHodPaddles),
  // 		        "Golden track's pos pmt hit",*&fStatPosHit);

  gHcParms->Define(Form("%shodo_pos_eff[%d]",  prefix,totalpaddles), "Hodo positive effi",*fHodoPosEffi);
  gHcParms->Define(Form("%shodo_neg_eff[%d]",  prefix,totalpaddles), "Hodo negative effi",*fHodoNegEffi);
  gHcParms->Define(Form("%shodo_or_eff[%d]",   prefix,totalpaddles), "Hodo or effi",      *fHodoOrEffi);
  gHcParms->Define(Form("%shodo_and_eff[%d]",  prefix,totalpaddles), "Hodo and effi",     *fHodoAndEffi);
  gHcParms->Define(Form("%shodo_plane_AND_eff[%d]",prefix,fNPlanes), "Hodo plane AND eff",  *fStatAndEff);
  gHcParms->Define(Form("%shodo_gold_hits[%d]",prefix,totalpaddles), "Hodo golden hits",  *fStatTrk);
  gHcParms->Define(Form("%shodo_s1XY_eff",prefix), "Efficiency for S1XY",fHodoEff_s1);
  gHcParms->Define(Form("%shodo_s2XY_eff",prefix), "Efficiency for S2XY",fHodoEff_s2);
  gHcParms->Define(Form("%shodo_stof_eff",prefix), "Efficiency for STOF",fHodoEff_tof);
  gHcParms->Define(Form("%shodo_3_of_4_eff",prefix), "Efficiency for 3 of 4",fHodoEff_3_of_4);
  gHcParms->Define(Form("%shodo_4_of_4_eff",prefix), "Efficiency for 4 of 4",fHodoEff_4_of_4);

  return kOK;
}

//_____________________________________________________________________________
Int_t THcHodoEff::DefineVariables( EMode mode )
{

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  //  fEffiTest = 0;
  //  gHcParms->Define(Form("hodoeffi"),"Testing effi",fEffiTest);

  const RVarDef vars[] = {
    // Move these into THcHallCSpectrometer using track fTracks
    // {"effitestvar",    "efficiency test var",      "fEffiTest"},
    // {"goldhodposhit",    "pos pmt hit in hodo",      "fStatPosHit"},
    { 0 }
  };
  return DefineVarsFromList( vars, mode );
  //  return kOK;
}

//_____________________________________________________________________________
Int_t THcHodoEff::Process( const THaEvData& evdata )
{
  // Accumulate statistics for efficiency

  // const char* const here = "Process";

  if( !IsOK() ) return -1;


  // Project the golden track to each
  // plane.  Need to get track at Focal Plane, not tgt.
  //
  // Assumes that planes are X, Y, X, Y
  THaTrack* theTrack = fSpectro->GetGoldenTrack();
  // Since fSpectro knows the index of the golden track, we can
  // get other information about the track from fSpectro.
  // Need to remove the specialized stuff from fGoldenTrack

  if(!theTrack) return 0;
  Int_t trackIndex = theTrack->GetTrkNum()-1;

  // May make these member variables
  Double_t hitPos[fNPlanes];
  Double_t hitDistance[fNPlanes];
  Int_t hitCounter[fNPlanes];
  Int_t checkHit[fNPlanes];
  // Bool_t goodTdcBothSides[fNPlanes];
  // Bool_t goodTdcOneSide[fNPlanes];

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    // Should really have plane object self identify as X or Y
    if(ip%2 == 0) {		// X Plane
      hitPos[ip] = theTrack->GetX() + theTrack->GetTheta()*fPosZ[ip];
      hitCounter[ip] = TMath::Max(
			TMath::Min(
			 TMath::Nint((hitPos[ip]-fCenterFirst[ip])/
				     fSpacing[ip]+1),fNCounters[ip] ),1);
      hitDistance[ip] =  hitPos[ip] - (fSpacing[ip]*(hitCounter[ip]-1) +
				       fCenterFirst[ip]);
    } else {			// Y Plane
      hitPos[ip] = theTrack->GetY() + theTrack->GetPhi()*fPosZ[ip];
      hitCounter[ip] = TMath::Max(
		        TMath::Min(
		         TMath::Nint((fCenterFirst[ip]-hitPos[ip])/
				     fSpacing[ip]+1), fNCounters[ip] ),1);
      hitDistance[ip] =  hitPos[ip] -(fCenterFirst[ip] -
				      fSpacing[ip]*(hitCounter[ip]-1));
    }


  }

  // Fill dpos histograms and set checkHit for each plane.
  // dpos stuff not implemented
  // Why do dpos stuff here, does any other part need the dpos historgrams
  // Look to VDCEff code to see how to create and fill histograms

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    Int_t hitcounter=hitCounter[ip];
    // goodTdcBothSides[ip] = kFALSE;
    // goodTdcOneSide[ip] = kFALSE;
    checkHit[ip] = 2;
    Int_t nphits=fPlanes[ip]->GetNScinHits();
    TClonesArray* hodoHits = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<nphits;ihit++) {
      THcHodoHit* hit=(THcHodoHit*) hodoHits->At(ihit);
      Int_t counter = hit->GetPaddleNumber();
      if(counter == hitcounter) {
	checkHit[ip] = 0;
      } else {
	if(TMath::Abs(counter-hitcounter) == 1 && checkHit[ip] != 0) {
	  checkHit[ip] = 1;
	}
      }
    }
  }

  // Record position differences between track and center of scin
  // and increment 'should have hit' counters
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    // Int_t hitcounter = hitCounter[ip];
    Double_t dist = hitDistance[ip];
    if(TMath::Abs(dist) <= fStatSlop &&
       theTrack->GetChi2()/theTrack->GetNDoF() <= fMaxChisq &&
       theTrack->GetEnergy() >= fHodoEff_CalEnergy_Cut )
      {
	fStatTrk[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	// Double_t delta = theTrack->GetDp();
	// Int_t idel = TMath::Floor(delta+10.0);
	// Should
	// if(idel >=0 && idel < 20) {
	//   fStatTrkDel[ip][hitcounter][idel]++;
	// }
	// lookat[ip] = TRUE;
      }
    fHitPlane[ip] = 0;
  }
  // Is there a hit on or adjacent to paddle that track
  // passes through?

  // May collapse this loop into last

  // record the hits as a "didhit" if track is near center of
  // scintillator, the chisqared of the track is good and it is the
  // first "didhit" in that plane.

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    Int_t hitcounter = hitCounter[ip];
    if (hitcounter>=fNCounters[ip]) hitcounter=fNCounters[ip]-1;
    if (hitcounter<0) hitcounter=0;    
    Double_t dist = hitDistance[ip];
    Int_t nphits=fPlanes[ip]->GetNScinHits();
    TClonesArray* hodoHits = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<nphits;ihit++) {
      THcHodoHit* hit=(THcHodoHit*) hodoHits->At(ihit);
      Int_t counter = hit->GetPaddleNumber();
      // Finds first best hit
      Bool_t onTrack, goodScinTime, goodTdcNeg, goodTdcPos;
      fHod->GetFlags(trackIndex,ip,ihit,
		     onTrack, goodScinTime, goodTdcNeg, goodTdcPos);
      if(TMath::Abs(dist) <= fStatSlop &&
	 TMath::Abs(hitcounter-counter) <= checkHit[ip] &&
	 fHitPlane[ip] == 0 &&
	 theTrack->GetChi2()/theTrack->GetNDoF() <= fMaxChisq &&
	 theTrack->GetEnergy() >= fHodoEff_CalEnergy_Cut) {
	fHitPlane[ip]++;

	// Need to find out hgood_tdc_pos(igoldentrack,ihit) and neg
	if(goodTdcPos) {
	  if(goodTdcNeg) {	// Both fired
	    fStatPosHit[ip][hitcounter]++;
	    fStatNegHit[ip][hitcounter]++;
	    fStatAndHit[ip][hitcounter]++;
	    fStatOrHit[ip][hitcounter]++;

	    fHodoPosEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	    fHodoNegEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	    fHodoAndEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	    fHodoOrEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;

	    // Double_t delta = theTrack->GetDp();
	    // Int_t idel = TMath::Floor(delta+10.0);
	    // if(idel >=0 && idel < 20) {
	    //   fStatAndHitDel[ip][hitcounter][idel]++;
	    // }
	  } else {
	    fStatPosHit[ip][hitcounter]++;
	    fStatOrHit[ip][hitcounter]++;
	    fHodoPosEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	    fHodoOrEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	  }
	} else if (goodTdcNeg) {
	  fStatNegHit[ip][hitcounter]++;
	  fStatOrHit[ip][hitcounter]++;
	  fHodoNegEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	  fHodoOrEffi[fHod->GetScinIndex(ip,hitCounter[ip]-1)]++;
	}

	// Increment pos/neg/both fired.  Track independent, so
	// no chisquared cut, but note that only scintillators on the
	// track are examined.
	if(goodTdcPos) {
	  if(goodTdcNeg) {
	    fBothGood[ip][hitcounter]++;
	  } else {
	    fPosGood[ip][hitcounter]++;
	  }
	} else if (goodTdcNeg) {
	  fNegGood[ip][hitcounter]++;
	}
	// Determine if one or both PMTs had a good tdc

	// if(goodTdcPos && goodTdcNeg) {
	//  goodTdcBothSides[ip] = kTRUE;
	// }
	// if(goodTdcPos || goodTdcNeg) {
	//  goodTdcOneSide[ip] = kTRUE;
	// }
      }

      /*
	For each plane, see of other 3 fired.  This means that they were enough
	to form a 3/4 trigger, and so the fraction of times this plane fired is
	the plane trigger efficiency.  NOTE: we only require a TDC hit, not a
	TDC hit within the SCIN 3/4 trigger window, so high rates will make
	this seem better than it is.  Also, make sure we're not near the edge
	of the hodoscope (at the last plane), using the same hhodo_slop param.
	as for h_tof.f
	NOTE ALSO: to make this check simpler, we are assuming that all planes
	have identical active areas.  y_scin = y_cent + y_offset, so shift track
	position by offset for comparing to edges.
      */

      // Need to add calculation and cuts on
      // xatback and yatback in order to set the
      // htrig_hododidflag, htrig_hodoshouldflag and otherthreehit flags
      //

      ++fNevt;
    }
  }
  return 0;
}

//_____________________________________________________________________________

ClassImp(THcHodoEff)
////////////////////////////////////////////////////////////////////////////////

