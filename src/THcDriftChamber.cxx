///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDriftChamber                                                           //
//                                                                           //
// Subdetector class to hold a bunch of planes constituting a chamber        //
// This class will be created by the THcDC class which will also create      //
// the plane objects.                                                        //
// The THcDC class will then pass this class a list of the planes.           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcDriftChamber.h"
#include "THcDC.h"
#include "THcDCHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TVectorD.h"
#include "THcSpacePoint.h"

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;

//_____________________________________________________________________________
THcDriftChamber::THcDriftChamber(
 const char* name, const char* description,
 const Int_t chambernum, THaDetectorBase* parent ) :
  THaSubDetector(name,description,parent)
{
  // Constructor

  //  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
  fNPlanes = 0;			// No planes until we make them

  fChamberNum = chambernum;

  fSpacePoints = new TClonesArray("THcSpacePoint",10);

}

//_____________________________________________________________________________
void THcDriftChamber::Setup(const char* name, const char* description)
{

  char prefix[2];

  THaApparatus *app = GetApparatus();
  if(app) {
    cout << app->GetName() << endl;
  } else {
    cout << "No apparatus found" << endl;
  }

  prefix[0]=tolower(app->GetName()[0]);
  prefix[1]='\0';

}

//_____________________________________________________________________________
THcDriftChamber::THcDriftChamber( ) :
  THaSubDetector()
{
  // Constructor
  fPlanes.clear();
  fSpacePoints = NULL;
}
//_____________________________________________________________________________
Int_t THcDriftChamber::Decode( const THaEvData& evdata )
{
  if (fhdebugflagpr) cout << "THcDriftChamber::Decode called" << endl;
  return 0;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcDriftChamber::Init( const TDatime& date )
{
  //  static const char* const here = "Init()";

  Setup(GetName(), GetTitle());
  
  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaSubDetector::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;
}

void THcDriftChamber::AddPlane(THcDriftChamberPlane *plane)
{
  if (fhdebugflagpr) cout << "Added plane " << plane->GetPlaneNum() << " to chamber " << fChamberNum << " " << fNPlanes << " " << YPlaneInd << " " << YPlanePInd << endl;
  plane->SetPlaneIndex(fNPlanes);
  fPlanes.push_back(plane);
 // HMS Specific
  // Hard code Y plane numbers.  Should be able to get from wire angle
  if(fChamberNum == 1) {
    YPlaneNum = 2;
    YPlanePNum = 5;
  } else {
    YPlaneNum = 8;
    YPlanePNum = 11;
  }

  // HMS Specific
  // Check if this is a Y Plane
  if(plane->GetPlaneNum() == YPlaneNum) {
    YPlaneInd = fNPlanes;
  } else if (plane->GetPlaneNum() == YPlanePNum) {
    YPlanePInd = fNPlanes;
  }
  fNPlanes++;
  return;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::ReadDatabase( const TDatime& date )
{

  cout << "THcDriftChamber::ReadDatabase()" << endl;
  char prefix[2];
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  DBRequest list[]={
    {"_remove_sppt_if_one_y_plane",&fRemove_Sppt_If_One_YPlane, kInt},
    {"dc_wire_velocity", &fWireVelocity, kDouble},
    {"SmallAngleApprox", &fSmallAngleApprox, kInt},
    {"stub_max_xpdiff", &fStubMaxXPDiff, kDouble},
    {"debugflagpr", &fhdebugflagpr, kDouble},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  // Get parameters parent knows about
  THcDC* fParent;
  fParent = (THcDC*) GetParent();
  fMinHits = fParent->GetMinHits(fChamberNum);
  fMaxHits = fParent->GetMaxHits(fChamberNum);
  fMinCombos = fParent->GetMinCombos(fChamberNum);
  fFixPropagationCorrection = fParent->GetFixPropagationCorrectionFlag();

  if (fhdebugflagpr) cout << "Chamber " << fChamberNum << "  Min/Max: " << fMinHits << "/" << fMaxHits << endl;
  fSpacePointCriterion = fParent->GetSpacePointCriterion(fChamberNum);
   
  fSpacePointCriterion2 = fSpacePointCriterion*fSpacePointCriterion;
   if (fhdebugflagpr) cout << " cham = " << fChamberNum << " Set yplane num " << YPlaneNum << " "<< YPlanePNum << endl; 
  // Generate the HAA3INV matrix for all the acceptable combinations
  // of hit planes.  Try to make it as generic as possible 
  // pindex=0 -> Plane 1 missing, pindex5 -> plane 6 missing.  Won't 
  // replicate the exact values used in the ENGINE, because the engine
  // had one big list of matrices for both chambers, while here we will
  // have a list just for one chamber.  Also, call pindex, pmindex as
  // we tend to use pindex as a plane index.
  fCosBeta = new Double_t [fNPlanes];
  fSinBeta = new Double_t [fNPlanes];
  fTanBeta = new Double_t [fNPlanes];
  fSigma = new Double_t [fNPlanes];
  fPsi0 = new Double_t [fNPlanes];
  fStubCoefs = new Double_t* [fNPlanes];
  Int_t allplanes=0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fCosBeta[ip] = TMath::Cos(fPlanes[ip]->GetBeta());
    fSinBeta[ip] = TMath::Sin(fPlanes[ip]->GetBeta());
    fTanBeta[ip] = fSinBeta[ip]/fCosBeta[ip];
    fSigma[ip] = fPlanes[ip]->GetSigma();
    fPsi0[ip] = fPlanes[ip]->GetPsi0();
    fStubCoefs[ip] = fPlanes[ip]->GetStubCoef();
    allplanes |= 1<<ip;
  }
  // Unordered map introduced in C++-11
  // Can use unordered_map if using C++-11
  // May not want to use map a all for performance, but using it now
  // for code clarity
  for(Int_t ipm1=0;ipm1<fNPlanes+1;ipm1++) { // Loop over missing plane1
    for(Int_t ipm2=ipm1;ipm2<fNPlanes+1;ipm2++) {
      if(ipm1==ipm2 && ipm1<fNPlanes) continue;
      TMatrixD* AA3 = new TMatrixD(3,3);
      for(Int_t i=0;i<3;i++) {
	for(Int_t j=i;j<3;j++) {
	  (*AA3)[i][j] = 0.0;
	  for(Int_t ip=0;ip<fNPlanes;ip++) {
	    if(ipm1 != ip && ipm2 != ip) {
	      (*AA3)[i][j] += fStubCoefs[ip][i]*fStubCoefs[ip][j];
	    }
	  }
	  (*AA3)[j][i] = (*AA3)[i][j];
	}
      }
      Int_t bitpat = allplanes & ~(1<<ipm1) & ~(1<<ipm2);
      // Should check that it is invertable
      //      if (fhdebugflagpr) cout << bitpat << " Determinant: " << AA3->Determinant() << endl;
      AA3->Invert();
      fAA3Inv[bitpat] = AA3;
    }
  }

#if 0  
  for(map<int,TMatrixD*>::iterator pm=fHaa3map.begin();
      pm != fHaa3map.end(); pm++) {
    if (fhdebugflagpr) cout << setbase(8) << pm->first << endl;
    fAA3Inv[pm->first]->Print();
  }
#endif

  fIsInit = true;
  return kOK;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );
  // Register variables in global list

  //  RVarDef vars[] = {
  //    { "nhit", "Number of DC hits",  "fNhits" },
  //    { 0 }
  //  };
  //  return DefineVarsFromList( vars, mode );
  return kOK;

}
void THcDriftChamber::ProcessHits( void)
{
  // Make a list of hits for whole chamber
  fNhits = 0;
  fHits.clear();
  fHits.reserve(10);

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    TClonesArray* hitsarray = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<fPlanes[ip]->GetNHits();ihit++) {
      fHits.push_back(static_cast<THcDCHit*>(hitsarray->At(ihit)));
      fNhits++;
    }
  }
  //  if (fhdebugflagpr) cout << "ThcDriftChamber::ProcessHits() " << fNhits << " hits" << endl;
}


Int_t THcDriftChamber::FindSpacePoints( void )
{
  // Following h_pattern_recognition.f, but just for one chamber

  // Code below is specifically for HMS chambers with Y and Y' planes

  fSpacePoints->Clear();

  Int_t yplane_hitind=0;
  Int_t yplanep_hitind=0;

  fNSpacePoints=0;
  fEasySpacePoint = 0;
  if (fhdebugflagpr) cout << " starting  build up array of hits" << endl;
  // Should really build up array of hits
  if(fNhits >= fMinHits && fNhits < fMaxHits) {
    /* Has this list of hits already been cut the way it should have at this point? */
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      THcDCHit* thishit = fHits[ihit];
      Int_t ip=thishit->GetPlaneNum();  // This is the absolute plane mumber
      if(ip==YPlaneNum) yplane_hitind = ihit;
      if(ip==YPlanePNum) yplanep_hitind = ihit;
      if (fhdebugflagpr) cout << " hit  = " << ihit << " " << ip <<" " << thishit->GetWireNum() << " " << thishit->GetRawTime()<<" " << thishit->GetPos()<< " " << endl;
    }
    // fPlanes is the array of planes for this chamber.
    //    if (fhdebugflagpr) cout << fPlanes[YPlaneInd]->GetNHits() << " "
    //	 << fPlanes[YPlanePInd]->GetNHits() << " " << endl;
    //    if (fhdebugflagpr) cout << " Yplane ind " << YPlaneInd << " " << YPlanePInd << endl;
    if (fPlanes[YPlaneInd]->GetNHits() == 1 && fPlanes[YPlanePInd]->GetNHits() == 1) {
      //      if (fhdebugflagpr) cout <<" Yplane info :" << fHits[yplane_hitind]->GetWireNum() << " "
      //	   << fHits[yplane_hitind]->GetPos() << " "
      //	   << fHits[yplanep_hitind]->GetWireNum() << " "
      //	   << fHits[yplanep_hitind]->GetPos() << " " 
      //	   << fSpacePointCriterion << endl;
    }
    if(fPlanes[YPlaneInd]->GetNHits() == 1 && fPlanes[YPlanePInd]->GetNHits() == 1
       && TMath::Abs(fHits[yplane_hitind]->GetPos() - fHits[yplanep_hitind]->GetPos())
       < fSpacePointCriterion
       && fNhits <= 6) {	// An easy case, probably one hit per plane
      fEasySpacePoint = FindEasySpacePoint(yplane_hitind, yplanep_hitind);
      if (fhdebugflagpr) cout << " call FindEasySpacePoint" << " # of space points = " << fNSpacePoints << endl;
    }
    if(!fEasySpacePoint) {	// It's not easy
            if (fhdebugflagpr) cout << " call FindHardSpacePoints" << endl;
            if (fhdebugflagpr) cout << " nhits = " << fNhits << " " << fPlanes[YPlaneInd]->GetNHits() << " " << fPlanes[YPlanePInd]->GetNHits() << endl;
      FindHardSpacePoints();
      if (fhdebugflagpr) cout << " call FindHardSpacePoint" << " # of space points = " << fNSpacePoints << endl;
    }

    // We have our space points for this chamber
        if (fhdebugflagpr) cout << fNSpacePoints << " Space Points found" << endl;
    if(fNSpacePoints > 0) {
      if(fRemove_Sppt_If_One_YPlane == 1) {
	// The routine is specific to HMS
	Int_t ndest=DestroyPoorSpacePoints();
       	if (fhdebugflagpr) cout << ndest << " Poor space points destroyed" << " # of space points = " << fNSpacePoints << endl;
	// Loop over space points and remove those with less than 4 planes
	// hit and missing hits in Y,Y' planes
      }
      //      if(fNSpacePoints == 0) if (fhdebugflagpr) cout << "DestroyPoorSpacePoints() killed SP" << endl;
      Int_t nadded=SpacePointMultiWire();
      if (nadded) if (fhdebugflagpr) cout << nadded << " Space Points added with SpacePointMultiWire()" << endl;
      ChooseSingleHit();
       	if (fhdebugflagpr) cout << " After choose single hit " << " # of space points = " << fNSpacePoints << endl;
      SelectSpacePoints();
       	if (fhdebugflagpr) cout << " After select space point " << " # of space points = " << fNSpacePoints << endl;
      //      if(fNSpacePoints == 0) if (fhdebugflagpr) cout << "SelectSpacePoints() killed SP" << endl;
    }
    //    if (fhdebugflagpr) cout << fNSpacePoints << " Space Points remain" << endl;
    // Add these space points to the total list of space points for the
    // the DC package.  Do this in THcDC.cxx.
#if 0
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      Int_t np = fPlanes[ip]->GetPlaneNum(); // Actuall plane number of this plane
      // (0-11) or (1-12)?
      TClonesArray* fHits = fPlanes[ip]->GetHits();

      for(Int_t ihit=0;ihit<fNhits;ihit++) { // Looping over all hits in all planes of the chamber
	THcDCHit* hit = static_cast<THcDCHit*>(fHits->At(ihit));
	// 

      }
    }
#endif
  }
  return(fNSpacePoints);
}

//_____________________________________________________________________________
// HMS Specific
Int_t THcDriftChamber::FindEasySpacePoint(Int_t yplane_hitind,Int_t yplanep_hitind)
{
  // Simplified HMS find_space_point routing.  It is given all y hits and
  // checks to see if all x-like hits are close enough together to make
  // a space point.

  Int_t easy_space_point=0;
  //  if (fhdebugflagpr) cout << "Doing Find Easy Space Point" << endl;
  Double_t yt = (fHits[yplane_hitind]->GetPos() + fHits[yplanep_hitind]->GetPos())/2.0;
  Double_t xt = 0.0;
  Int_t num_xhits = 0;
  Double_t x_pos[MAX_HITS_PER_POINT];

  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    THcDCHit* thishit = fHits[ihit];
    if(ihit!=yplane_hitind && ihit!=yplanep_hitind) { // x-like hit
      // ysp and xsp are from h_generate_geometry
           if (fhdebugflagpr) cout << ihit << " " << thishit->GetPos() << " " << yt << " " << thishit->GetWirePlane()->GetYsp() << " " << thishit->GetWirePlane()->GetXsp() << endl;
      x_pos[ihit] = (thishit->GetPos()
		     -yt*thishit->GetWirePlane()->GetYsp())
	/thishit->GetWirePlane()->GetXsp();
      xt += x_pos[ihit];
      num_xhits++;
    } else {
      x_pos[ihit] = 0.0;
    }
  }
  Double_t max_dist = TMath::Sqrt(fSpacePointCriterion/2.);
  xt = (num_xhits>0?xt/num_xhits:0.0);
    if (fhdebugflagpr) cout << " mean x = "<< xt << " max dist = " << max_dist << endl;
  easy_space_point = 1; // Assume we have an easy space point
  // Rule it out if x points don't cluster well enough
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
        if (fhdebugflagpr) cout << "Hit " << ihit+1 << " " << x_pos[ihit] << " " << TMath::Abs(xt-x_pos[ihit]) << endl;
    if(ihit!=yplane_hitind && ihit!=yplanep_hitind) { // x-like hit
      if(TMath::Abs(xt-x_pos[ihit]) >= max_dist)
	{ easy_space_point=0; break;}
    }
  }
  if(easy_space_point) {	// Register the space point
    THcSpacePoint* sp = (THcSpacePoint*)fSpacePoints->ConstructedAt(fNSpacePoints++);
    if (fhdebugflagpr) cout << "Easy Space Point " << xt << " " << yt << " Space point # ="  << fNSpacePoints << endl;
    sp->Clear();
    sp->SetXY(xt, yt);
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      sp->AddHit(fHits[ihit]);
    }
  }
  return(easy_space_point);
}

//_____________________________________________________________________________
// Generic
Int_t THcDriftChamber::FindHardSpacePoints()
{
  Int_t MAX_NUMBER_PAIRS=1000; // Where does this get set?
  struct Pair {
    THcDCHit* hit1;
    THcDCHit* hit2;
    Double_t x, y;
  };
  Pair pairs[MAX_NUMBER_PAIRS];
  //	
  Int_t ntest_points=0;
  for(Int_t ihit1=0;ihit1<fNhits-1;ihit1++) {
    THcDCHit* hit1=fHits[ihit1];
    THcDriftChamberPlane* plane1 = hit1->GetWirePlane();
    for(Int_t ihit2=ihit1+1;ihit2<fNhits;ihit2++) {
      if(ntest_points < MAX_NUMBER_PAIRS) {
	THcDCHit* hit2=fHits[ihit2];
	THcDriftChamberPlane* plane2 = hit2->GetWirePlane();
	Double_t determinate = plane1->GetXsp()*plane2->GetYsp()
	  -plane1->GetYsp()*plane2->GetXsp();
	if(TMath::Abs(determinate) > 0.3) { // 0.3 is sin(alpha1-alpha2)=sin(17.5)
	  pairs[ntest_points].hit1 = hit1;
	  pairs[ntest_points].hit2 = hit2;
	  pairs[ntest_points].x = (hit1->GetPos()*plane2->GetYsp()
				   - hit2->GetPos()*plane1->GetYsp())
	    /determinate;
	  pairs[ntest_points].y = (hit2->GetPos()*plane1->GetXsp()
				   - hit1->GetPos()*plane2->GetXsp())
	    /determinate;
	  ntest_points++;
	}
      }
    }
  }
  Int_t ncombos=0;
  struct Combo {
    Pair* pair1;
    Pair* pair2;
  };
  Combo combos[10*MAX_NUMBER_PAIRS];
  for(Int_t ipair1=0;ipair1<ntest_points-1;ipair1++) {
    for(Int_t ipair2=ipair1+1;ipair2<ntest_points;ipair2++) {
      if(ncombos < 10*MAX_NUMBER_PAIRS) {
	Double_t dist2 = pow(pairs[ipair1].x - pairs[ipair2].x,2)
	  + pow(pairs[ipair1].y - pairs[ipair2].y,2);
	if(dist2 <= fSpacePointCriterion2) {
	  combos[ncombos].pair1 = &pairs[ipair1];
	  combos[ncombos].pair2 = &pairs[ipair2];
	  ncombos++;
	}
      }
    }
  }
  // Loop over all valid combinations and build space points
  if (fhdebugflagpr) cout << "looking for hard Space Point combos = " << ncombos << "# of sp pts = " << fNSpacePoints<< endl;
  for(Int_t icombo=0;icombo<ncombos;icombo++) {
    THcDCHit* hits[4];
    hits[0]=combos[icombo].pair1->hit1;
    hits[1]=combos[icombo].pair1->hit2;
    hits[2]=combos[icombo].pair2->hit1;
    hits[3]=combos[icombo].pair2->hit2;
    // Get Average Space point xt, yt
    Double_t xt = (combos[icombo].pair1->x + combos[icombo].pair2->x)/2.0;
    Double_t yt = (combos[icombo].pair1->y + combos[icombo].pair2->y)/2.0;
    // Loop over space points
    if(fNSpacePoints > 0) {
      Int_t add_flag=1;
      for(Int_t ispace=0;ispace<fNSpacePoints;ispace++) {
	THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[ispace];
	if(sp->GetNHits() > 0) {
	  Double_t sqdist_test = pow(xt - sp->GetX(),2) + pow(yt - sp->GetY(),2);
	  // I (who is I) want to be careful if sqdist_test is bvetween 1 and
	  // 3 fSpacePointCriterion2.  Let me ignore not add a new point the
	  if(sqdist_test < 3*fSpacePointCriterion2) {
	    add_flag = 0;	// do not add a new space point
	  }
	  if(sqdist_test < fSpacePointCriterion2) {
	    // This is a real match
	    // Add the new hits to the existing space point
	    Int_t iflag[4];
	    iflag[0]=0;iflag[1]=0;iflag[2]=0;iflag[3]=0;
	    // Find out which of the four hits in the combo are already
	    // in the space point under consideration so that we don't
	    // add duplicate hits to the space point
	    for(Int_t isp_hit=0;isp_hit<sp->GetNHits();isp_hit++) {
	      for(Int_t icm_hit=0;icm_hit<4;icm_hit++) { // Loop over combo hits
		if(sp->GetHit(isp_hit)==hits[icm_hit]) {
		  iflag[icm_hit] = 1;
		}
	      }
	    }
	    // Remove duplicated pionts in the combo so we don't add
	    // duplicate hits to the space point
	    for(Int_t icm1=0;icm1<3;icm1++) {
	      for(Int_t icm2=icm1+1;icm2<4;icm2++) {
		if(hits[icm1]==hits[icm2]) {
		  iflag[icm2] = 1;
		}
	      }
	    }
	    // Add the unique combo hits to the space point
	    for(Int_t icm=0;icm<4;icm++) {
	      if(iflag[icm]==0) {
		sp->AddHit(hits[icm]);
	      }
	    }
	    sp->IncCombos();
            //cout << " number of combos = " << sp->GetCombos() << endl;
	    // Terminate loop since this combo can only belong to one space point
	    break;
	  }
	}
      }// End of loop over existing space points
      // Create a new space point if more than 2*space_point_criteria
      if(fNSpacePoints < MAX_SPACE_POINTS) {
	if(add_flag) {
          if (fhdebugflagpr) cout << " add glag = " << add_flag << " space pts =  " << fNSpacePoints << endl ;
	  THcSpacePoint* sp = (THcSpacePoint*)fSpacePoints->ConstructedAt(fNSpacePoints++);
	  sp->Clear();
	  sp->SetXY(xt, yt);
	  sp->SetCombos(1);
	  sp->AddHit(hits[0]);
	  sp->AddHit(hits[1]);
	  if(hits[0] != hits[2] && hits[1] != hits[2]) {
	    sp->AddHit(hits[2]);
	  }
	  if(hits[0] != hits[3] && hits[1] != hits[3]) {
	    sp->AddHit(hits[3]);
	  }
	}
      }
    } else {// Create first space point
      // This duplicates code above.  Need to see if we can restructure
      // to avoid
      THcSpacePoint* sp = (THcSpacePoint*)fSpacePoints->ConstructedAt(fNSpacePoints++);
      sp->Clear();
      sp->SetXY(xt, yt);
      sp->SetCombos(1);
      sp->AddHit(hits[0]);
      sp->AddHit(hits[1]);
      if(hits[0] != hits[2] && hits[1] != hits[2]) {
	sp->AddHit(hits[2]);
      }
      if(hits[0] != hits[3] && hits[1] != hits[3]) {
	sp->AddHit(hits[3]);
      }
       if (fhdebugflagpr) cout << "1st hard Space Point " << xt << " " << yt << " Space point # ="  << fNSpacePoints << endl;
    }//End check on 0 space points
  }//End loop over combos
  if (fhdebugflagpr) cout << " finished findspacept # of sp pts = " << fNSpacePoints << endl;
  return(fNSpacePoints);
}

//_____________________________________________________________________________
// HMS Specific?
Int_t THcDriftChamber::DestroyPoorSpacePoints()
{
  Int_t nhitsperplane[fNPlanes];

  Int_t spacepointsgood[fNSpacePoints];
  Int_t ngood=0;

  for(Int_t i=0;i<fNSpacePoints;i++) {
    spacepointsgood[i] = 0;
  }
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    Int_t nplanes_hit = 0;
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      nhitsperplane[ip] = 0;
    }
    // Count # hits in each plane for this space point
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
      THcDCHit* hit=sp->GetHit(ihit);
      // hit_order(hit) = ihit;
      Int_t ip = hit->GetPlaneIndex();
      nhitsperplane[ip]++;
    }
    // Count # planes that have hits
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      if(nhitsperplane[ip] > 0) {
	nplanes_hit++;
      }
    }
    if(nplanes_hit >= fMinHits && nhitsperplane[YPlaneInd]>0
       && nhitsperplane[YPlanePInd] > 0) {
      spacepointsgood[ngood++] = isp; // Build list of good points
    } else {
      //      if (fhdebugflagpr) cout << "Missing Y-hit!!";
    }
  }

  // Remove the bad space points
  Int_t nremoved=fNSpacePoints-ngood;
  fNSpacePoints = ngood;
  for(Int_t isp=0;isp<fNSpacePoints;isp++) { // New index num ber
    Int_t osp=spacepointsgood[isp]; // Original index number
    if(osp > isp) {
      // Does this work, or do we have to copy each member?
      // If it doesn't we should overload the = operator
      //(*fSpacePoints)[isp] = (*fSpacePoints)[osp];
        THcSpacePoint* spi = (THcSpacePoint*)(*fSpacePoints)[isp];
        THcSpacePoint* spo = (THcSpacePoint*)(*fSpacePoints)[osp];
        spi->Clear();
        Double_t xt,yt;
        xt=spo->GetX();
        yt=spo->GetY();
        spi->SetXY(xt, yt);
        for(Int_t ihit=0;ihit<spo->GetNHits();ihit++) {
            THcDCHit* hit = spo->GetHit(ihit);
           spi->AddHit(hit);
	}         
    }
  }
  return nremoved;
}
    
//_____________________________________________________________________________
// HMS Specific?
  /*
   Purpose and Methods :  This routine loops over space points and 
                          looks at all hits in the space
                          point. If more than 1 hit is in the same 
                          plane then the space point is cloned with
                          all combinations of 1 wire per plane.  The 
                          requirements for cloning are:  1) at least 
                          4 planes fire, and 2) no more than 6 planes 
                          have multiple hits.      
  */
Int_t THcDriftChamber::SpacePointMultiWire()
{
  Int_t nhitsperplane[fNPlanes];
  THcDCHit* hits_plane[fNPlanes][MAX_HITS_PER_POINT];

  Int_t nsp_check;
  Int_t nplanes_single;

  Int_t nsp_tot=fNSpacePoints;
  Int_t nsp_totl=fNSpacePoints;
  if (fhdebugflagpr) cout << "Start  Multiwire # of sp pts = " << nsp_totl << endl; 

  for(Int_t isp=0;isp<nsp_totl;isp++) {
    Int_t nplanes_hit = 0;	// Number of planes with hits
    Int_t nplanes_mult = 0;	// Number of planes with multiple hits
    Int_t nsp_new = 1;
    Int_t newsp_num=0;
    if (fhdebugflagpr) cout << "Looping thru space pts at # = " << isp << " total = " << fNSpacePoints << endl; 

    for(Int_t ip=0;ip<fNPlanes;ip++) {
      nhitsperplane[ip] = 0;
       for(Int_t ih=0;ih<MAX_HITS_PER_POINT;ih++) {
         hits_plane[ip][ih] = 0;
       }
    }
    // Sort Space Points hits by plane
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) { // All hits in SP
      THcDCHit* hit=sp->GetHit(ihit);
      //      hit_order Make a hash
      // hash(hit) = ihit;
      Int_t ip = hit->GetPlaneIndex();
      hits_plane[ip][nhitsperplane[ip]++] = hit;
      //if (fhdebugflagpr) cout << " hit = " << ihit+1 << " plane index = " << ip << " nhitsperplane = " << nhitsperplane[ip] << endl;
    }
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      if(nhitsperplane[ip] > 0) {
	nplanes_hit++;
	nsp_new *= nhitsperplane[ip];
	if(nhitsperplane[ip] > 1) nplanes_mult++;
        //if (fhdebugflagpr) cout << "Found plane with multi hits plane =" << ip+1 << " nplane_hit = "<< nplanes_hit << " nsp_new = " <<nsp_new << " nplane_mult = "<< nplanes_mult  << endl; 
      }
    }
    --nsp_new;
    nsp_check=nsp_tot + nsp_new;
    nplanes_single = nplanes_hit - nplanes_mult;
    if (fhdebugflagpr) cout << " # of new space points = " << nsp_new << " total now = " << nsp_tot<< endl;
    // Check if cloning conditions are met
    Int_t ntot = 0;
    if(nplanes_hit >= 4 && nplanes_mult < 4 && nplanes_mult >0
       && nsp_check < 20) {
      if (fhdebugflagpr) cout << " Cloning space point " << endl;      
      // Order planes by decreasing # of hits
      
      Int_t maxplane[fNPlanes];
      for(Int_t ip=0;ip<fNPlanes;ip++) {
	maxplane[ip] = ip;
      }
      // Sort by decreasing # of hits
      for(Int_t ip1=0;ip1<fNPlanes-1;ip1++) {
	for(Int_t ip2=ip1+1;ip2<fNPlanes;ip2++) {
	  if(nhitsperplane[maxplane[ip2]] > nhitsperplane[maxplane[ip1]]) {
	    Int_t temp = maxplane[ip1];
	    maxplane[ip1] = maxplane[ip2];
	    maxplane[ip2] = temp;
	  }
	}
      }
      if (fhdebugflagpr) cout << " Max plane  and hits " << maxplane[0] << " " << nhitsperplane[maxplane[0]]<< " " << maxplane[1] << " " << nhitsperplane[maxplane[1]]<< " "<< maxplane[2] << " " << nhitsperplane[maxplane[2]]<< endl;      
      // First fill clones with 1 hit each from the 3 planes with the most hits
      for(Int_t n1=0;n1<nhitsperplane[maxplane[0]];n1++) {
	for(Int_t n2=0;n2<nhitsperplane[maxplane[1]];n2++) {
	  for(Int_t n3=0;n3<nhitsperplane[maxplane[2]];n3++) {
	    ntot++;
	    newsp_num = fNSpacePoints; // 
	    if (fhdebugflagpr) cout << " new space pt num = " << newsp_num  << " " << fNSpacePoints <<  endl;
	    //THcSpacePoint* newsp;
	    if(n1==0 && n2==0 && n3==0) {
	      newsp_num = isp; // Copy over the original SP
	      THcSpacePoint* newsp = (THcSpacePoint*)fSpacePoints->ConstructedAt(newsp_num);//= (THcSpacePoint*)(*fSpacePoints)[newsp_num];
              if (fhdebugflagpr) cout << " Copy over original SP " << endl;
	      // newsp = sp;
	      Int_t combos_save=sp->GetCombos();
	      newsp->Clear();	// Clear doesn't clear X, Y
	      if (fhdebugflagpr) cout << " original sp #hits combos X y " << sp->GetCombos() << sp->GetNHits() << sp->GetX() << " " <<  sp->GetY() << endl;
              newsp->SetXY(sp->GetX(), sp->GetY());
	    newsp->SetCombos(combos_save);
	    newsp->AddHit(hits_plane[maxplane[0]][n1]);
	    newsp->AddHit(hits_plane[maxplane[1]][n2]);
	    newsp->AddHit(hits_plane[maxplane[2]][n3]);
	    newsp->AddHit(hits_plane[maxplane[3]][0]);
	    if(nhitsperplane[maxplane[4]] == 1) {
	      newsp->AddHit(hits_plane[maxplane[4]][0]);
	      if(nhitsperplane[maxplane[5]] == 1) 
		newsp->AddHit(hits_plane[maxplane[5]][0]);
	    }
	      if (fhdebugflagpr)  {
		THcSpacePoint* spt = (THcSpacePoint*)fSpacePoints->ConstructedAt(newsp_num);//(THcSpacePoint*)(*fSpacePoints)[isp];
	      cout << " new space pt num = " <<newsp_num  << " " << spt->GetNHits() << endl;
                for(Int_t ihit=0;ihit<spt->GetNHits();ihit++) {
                    THcDCHit* hit = spt->GetHit(ihit);
		    cout << " hit = " << ihit+1 << " "  <<  hit->GetDist() << endl;
                }
	      } // fhdebugflagpr 
	    } else {
              if (fhdebugflagpr) cout << " setting other sp " << "# space pts now = " << fNSpacePoints << endl;
	      THcSpacePoint* newsp = (THcSpacePoint*)fSpacePoints->ConstructedAt(newsp_num);
              fNSpacePoints++; 
	      Int_t combos_save=sp->GetCombos();
	      newsp->Clear();
	      newsp->SetXY(sp->GetX(), sp->GetY());
	    newsp->SetCombos(combos_save);
	    newsp->AddHit(hits_plane[maxplane[0]][n1]);
	    newsp->AddHit(hits_plane[maxplane[1]][n2]);
	    newsp->AddHit(hits_plane[maxplane[2]][n3]);
	    newsp->AddHit(hits_plane[maxplane[3]][0]);
	    if(nhitsperplane[maxplane[4]] == 1) {
	      newsp->AddHit(hits_plane[maxplane[4]][0]);
	      if(nhitsperplane[maxplane[5]] == 1) 
		newsp->AddHit(hits_plane[maxplane[5]][0]);
	    }
	      if (fhdebugflagpr)  {
              THcSpacePoint* spt = (THcSpacePoint*)fSpacePoints->ConstructedAt(newsp_num);
	      cout << " new space pt num = " << fNSpacePoints << " " << spt->GetNHits() << endl;
                for(Int_t ihit=0;ihit<spt->GetNHits();ihit++) {
                    THcDCHit* hit = spt->GetHit(ihit);
		    cout << " hit = " << ihit+1 << " "  <<  hit->GetDist() << endl;
                }
	      } // fhdebugflagpr 
            }
	  }
	}
      }
#if 0
      // Loop over clones and order hits in the same way as parent SP
      // Why do we have to order the hits.
      for(Int_t i=0;i<ntot;i++) {
	Int_t newsp_num= nsp_tot + i;
	if(i == 1) newsp_num = isp;
	for(Int_t j=0;j<nplanes_hit;j++) {
	  for(Int_t k=0;k<nplanes_hit;k++) {
	    THcDCHit* hit1 = fSpacePointHits[newsp_num][j];
	    THcDCHit* hit2 = fSpacePointHits[newsp_num][k];
	    if(hit_order(hit1) > hit_order(hit2)) {
	      THcDCHit* temp = fSpacePoints[newsp_num].hits[k];
	      fSpacePoints[newsp_num].hits[k] = fSpacePoints[newsp_num].hits[j];
	      fSpacePoints[newsp_num].hits[j] = temp;
	    }
	  }
	}
      }
#endif
      nsp_tot += (ntot-1);
    } else {
      ntot=1; // space point not to be cloned
    }
  }
  assert (nsp_tot > 0); // program terminates if nsp_tot <=0
  Int_t nadded=0;
  if(nsp_tot <= 20) {
    nadded = nsp_tot - fNSpacePoints;
    // fNSpacePoints = nsp_tot;
  }
  if (fhdebugflagpr) cout << " Added space pts " << nadded << " total space pts = " << fNSpacePoints << endl;      
 
  // In Fortran, fill in zeros.
  return(nadded);
}

//_____________________________________________________________________________
// HMS Specific?
void THcDriftChamber::ChooseSingleHit()
{
  // Look at all hits in a space point.  If two hits are in the same plane,
  // reject the one with the longer drift time.

  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    Int_t startnum = sp->GetNHits();
    Int_t goodhit[startnum];
    
    for(Int_t ihit=0;ihit<startnum;ihit++) {
      goodhit[ihit] = 1;
    }
    // For each plane, mark all hits longer than the shortest drift time
    for(Int_t ihit1=0;ihit1<startnum-1;ihit1++) {
      THcDCHit* hit1 = sp->GetHit(ihit1);
      Int_t plane1=hit1->GetPlaneIndex();
      Double_t tdrift1 = hit1->GetTime();
      for(Int_t ihit2=ihit1+1;ihit2<startnum;ihit2++) {
	THcDCHit* hit2 = sp->GetHit(ihit2);
	Int_t plane2=hit2->GetPlaneIndex();
	Double_t tdrift2 = hit2->GetTime();
	if(plane1 == plane2) {
	  if(tdrift1 > tdrift2) {
	    goodhit[ihit1] = 0;
	  } else {
	    goodhit[ihit2] = 0;
	  }
	  if (fhdebugflagpr) cout << " Rejecting hit " << ihit1 << " " << tdrift1 << " " << ihit2 << " " << tdrift2 << endl; 
	}
      }
    }
    // Gather the remaining hits
    Int_t finalnum = 0;
    for(Int_t ihit=0;ihit<startnum;ihit++) {
	THcDCHit* hit = sp->GetHit(ihit);
	if (fhdebugflagpr) cout << " good hit = "<< ihit << " " << goodhit[ihit] << " time = " << hit->GetTime() << endl;
      if(goodhit[ihit] > 0) {	// Keep this hit
	if (ihit > finalnum) {	// Move hit 
	  sp->ReplaceHit(finalnum++, sp->GetHit(ihit));
	} else {
          finalnum++ ;
        }
      }
    }
    sp->SetNHits(finalnum);
    if (fhdebugflagpr) cout << " choose single hit start # of hits = " <<  startnum << " final # = " <<finalnum << endl; 
  }
}
//_____________________________________________________________________________
// Generic
void THcDriftChamber::SelectSpacePoints()
//    This routine goes through the list of space_points and space_point_hits
//    found by find_space_points and only accepts those with 
//    number of hits > min_hits
//    number of combinations > min_combos
{
  Int_t sp_count=0;
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    // Include fEasySpacePoint because ncombos not filled in
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    if (fhdebugflagpr) cout << " looping sp points " << sp->GetCombos() << " " << fMinCombos << " " << fEasySpacePoint << " " << sp->GetNHits() << " " <<  fMinHits << endl;
    if(sp->GetCombos() >= fMinCombos || fEasySpacePoint) {
      if(sp->GetNHits() >= fMinHits) {
        if (fhdebugflagpr) cout << " select space pt = " << isp << endl;
	if(isp > sp_count) {
	  //	  (*fSpacePoints)[sp_count] = (*fSpacePoints)[isp];
        THcSpacePoint* sp1 = (THcSpacePoint*)(*fSpacePoints)[sp_count];
        sp1->Clear();
        Double_t xt,yt;
        xt=sp->GetX();
        yt=sp->GetY();
        sp1->SetXY(xt, yt);
        sp1->SetCombos(sp->GetCombos());
        for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
            THcDCHit* hit = sp->GetHit(ihit);
           sp1->AddHit(hit);
	}         
	}     
	sp_count++;
      }
    }
  }
  if(sp_count < fNSpacePoints)    if (fhdebugflagpr) cout << "Reduced from " << fNSpacePoints << " to " << sp_count << " space points" << endl;
  fNSpacePoints = sp_count;
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    if (fhdebugflagpr) cout << " sp pt = " << isp+1 << " # of hits = " << sp->GetNHits() << endl;
    for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
      THcDCHit* hit = sp->GetHit(ihit);
      THcDriftChamberPlane* plane=hit->GetWirePlane();
        if (fhdebugflagpr) cout << ihit+1 << "selecting " << plane->GetPlaneNum() << " " << plane->GetChamberNum() << " " << hit->GetTime() << " " << hit->GetDist() << " " << plane->GetCentralTime() << " " << plane->GetDriftTimeSign() << endl;
    }
  }
}

void THcDriftChamber::CorrectHitTimes()
{
  // Use the rough hit positions in the chambers to correct the drift time
  // for hits in the space points.

  // Assume all wires for a plane are read out on the same side (l/r or t/b).
  // If the wire is closer to horizontal, read out left/right.  If nearer
  // vertical, assume top/bottom.  (Note, this is not always true for the
  // SOS u and v planes.  They have 1 card each on the side, but the overall
  // time offset per card will cancel much of the error caused by this.  The
  // alternative is to check by card, rather than by plane and this is harder.
  //if (fhdebugflagpr) cout << "In correcthittimes fNSpacePoints = " << fNSpacePoints << endl;
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    Double_t x = sp->GetX();
    Double_t y = sp->GetY();
    for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
      THcDCHit* hit = sp->GetHit(ihit);
      THcDriftChamberPlane* plane=hit->GetWirePlane();

      // How do we know this correction only gets applied once?  Is
      // it determined that a given hit can only belong to one space point?
      Double_t time_corr = plane->GetReadoutX() ?
	y*plane->GetReadoutCorr()/fWireVelocity :
	x*plane->GetReadoutCorr()/fWireVelocity;
      
      //     if (fhdebugflagpr) cout << "Correcting hit " << hit << " " << plane->GetPlaneNum() << " " << isp << "/" << ihit << "  " << x << "," << y << endl;
      // Fortran ENGINE does not do this check, so hits can get "corrected"
      // multiple times if they belong to multiple space points.
      // To reproduce the precise ENGINE behavior, remove this if condition.
      if(fFixPropagationCorrection==0) { // ENGINE behavior
	hit->SetTime(hit->GetTime() - plane->GetCentralTime()
		     + plane->GetDriftTimeSign()*time_corr);
	hit->ConvertTimeToDist();
	//      hit->SetCorrectedStatus(1);
      } else {
	// New behavior: Save corrected distance with the hit in the space point
	// so that the same hit can have a different correction depending on
	// which space point it is in.
	//
	// This is a hack now because the converttimetodist method is connected to the hit
	// so I compute the corrected time and distance, and then restore the original
	// time and distance.  Can probably add a method to hit that does a conversion on a time
	// but does not modify the hit data.
	Double_t time=hit->GetTime();
	Double_t dist=hit->GetDist();
	hit->SetTime(time - plane->GetCentralTime()
		     + plane->GetDriftTimeSign()*time_corr);
	hit->ConvertTimeToDist();
	sp->SetHitDist(ihit, hit->GetDist());
	hit->SetTime(time);	// Restore time
	hit->SetDist(dist);	// Restore distance
      }
    }
  }
}	   
UInt_t THcDriftChamber::Count1Bits(UInt_t x)
// From http://graphics.stanford.edu/~seander/bithacks.html
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

//_____________________________________________________________________________
// HMS Specific
void THcDriftChamber::LeftRight()
{
  // For each space point,
  // Fit stubs to all possible left-right combinations of drift distances
  // and choose the set with the minimum chi**2.

  for(Int_t isp=0; isp<fNSpacePoints; isp++) {
    // Build a bit pattern of which planes are hit
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    Int_t nhits = sp->GetNHits();
    UInt_t bitpat  = 0;		// Bit pattern of which planes are hit
    Double_t minchi2 = 1.0e10;
    Double_t tmp_minchi2;
    Double_t minxp = 0.25;
    Int_t hasy1 = -1;
    Int_t hasy2 = -1;
    Int_t plusminusknown[nhits];
    Int_t plusminusbest[nhits];
    Int_t plusminus[nhits];	// ENGINE makes this array float.  Why?
    Int_t tmp_plusminus[nhits];
    Int_t plane_list[nhits];
    Double_t stub[4];
    Double_t tmp_stub[4];
    Int_t nplusminus;

    if(nhits < 0) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight() nhits < 0" << endl;
    } else if (nhits==0) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight() nhits = 0" << endl;
    }
    for(Int_t ihit=0;ihit < nhits;ihit++) {
      THcDCHit* hit = sp->GetHit(ihit);
      Int_t pindex = hit->GetPlaneIndex();
      plane_list[ihit] = pindex;

      bitpat |= 1<<pindex;

      plusminusknown[ihit] = 0;

      if(pindex == YPlaneInd) hasy1 = ihit;
      if(pindex == YPlanePInd) hasy2 = ihit;
    }
    nplusminus = 1<<nhits;
    Int_t smallAngOK = (hasy1>=0) && (hasy2>=0);
    if(fSmallAngleApprox !=0 && smallAngOK) { // to small Angle L/R for Y,Y' planes
      if(sp->GetHit(hasy2)->GetPos() <=
	 sp->GetHit(hasy1)->GetPos()) {
	plusminusknown[hasy1] = -1;
	plusminusknown[hasy2] = 1;
      } else {
	plusminusknown[hasy1] = 1;
	plusminusknown[hasy2] = -1;
      }
      nplusminus = 1<<(nhits-2);
      if (fhdebugflagpr) cout << " Small angle approx = " << smallAngOK << " " << plusminusknown[hasy1] << endl;
      if (fhdebugflagpr) cout << "pm =  " << plusminusknown[hasy2] << " " << hasy1 << " " << hasy2 << endl;
      if (fhdebugflagpr) cout << " Plane index " << YPlaneInd << " " << YPlanePInd << endl;
    }
    if(nhits < 2) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight: numhits-2 < 0" << endl;
    } else if (nhits == 2) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight: numhits-2 = 0" << endl;
    }
    Int_t nplaneshit = Count1Bits(bitpat);
    if (fhdebugflagpr) cout << " num of pm = " << nplusminus << " num of hits =" << nhits << endl;
    // Use bit value of integer word to set + or -
    // Loop over all combinations of left right.
    for(Int_t pmloop=0;pmloop<nplusminus;pmloop++) {
      Int_t iswhit = 1;
      for(Int_t ihit=0;ihit<nhits;ihit++) {
	if(plusminusknown[ihit]!=0) {
	  plusminus[ihit] = plusminusknown[ihit];
	} else {
	  // Max hits per point has to be less than 32.  
	  if(pmloop & iswhit) {
	    plusminus[ihit] = 1;
	  } else {
	    plusminus[ihit] = -1;
	  }
	  iswhit <<= 1;
	}
      }
      if (nplaneshit >= fNPlanes-1) {
	Double_t chi2 = FindStub(nhits, sp,
				     plane_list, bitpat, plusminus, stub);
				     
	//if(debugging)
	// Take best chi2 IF x' of the stub agrees with x' as expected from x.
	// Sometimes an incorrect x' gives a good chi2 for the stub, even though it is
	// not the correct left/right combination for the real track.
	// Rotate x'(=stub(3)) to hut coordinates and compare to x' expected from x.
	// THIS ASSUMES STANDARD HMS TUNE!!!!, for which x' is approx. x/875.
	if(chi2 < minchi2) {
	  if(stub[2]*fTanBeta[plane_list[0]]==-1.0) {
	    if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight() Error 3" << endl;
	  }
	  Double_t xp_fit=stub[2]-fTanBeta[plane_list[0]]
	    /(1+stub[2]*fTanBeta[plane_list[0]]);
	  // Tune depdendent.  Definitely HMS specific
	  Double_t xp_expect = sp->GetX()/875.0;
	  if(TMath::Abs(xp_fit-xp_expect)<fStubMaxXPDiff) {
	    minchi2 = chi2;
	    for(Int_t ihit=0;ihit<nhits;ihit++) {
	      plusminusbest[ihit] = plusminus[ihit];
	    }
	    Double_t *spstub = sp->GetStubP();
	    for(Int_t i=0;i<4;i++) {
	      spstub[i] = stub[i];
	    }
	  } else {		// Record best stub failing angle cut
	    tmp_minchi2 = chi2;
	    for(Int_t ihit=0;ihit<nhits;ihit++) {
	      tmp_plusminus[ihit] = plusminus[ihit];
	    }
	    for(Int_t i=0;i<4;i++) {
	      tmp_stub[i] = stub[i];
	    }
	  }
	}
      } else if (nplaneshit >= fNPlanes-2) { // Two planes missing
	Double_t chi2 = FindStub(nhits, sp,
				     plane_list, bitpat, plusminus, stub); 
	//if(debugging)
	//if (fhdebugflagpr) cout << "pmloop=" << pmloop << " Chi2=" << chi2 << endl;
	// Isn't this a bad idea, doing == with reals
	if(stub[2]*fTanBeta[plane_list[0]] == -1.0) {
	  if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight() Error 3" << endl;
	}
	Double_t xp_fit=stub[2]-fTanBeta[plane_list[0]]
	  /(1+stub[2]*fTanBeta[plane_list[0]]);
	if(TMath::Abs(xp_fit) <= minxp) {
	  minxp = TMath::Abs(xp_fit);
	  minchi2 = chi2;
	  for(Int_t ihit=0;ihit<nhits;ihit++) {
	    plusminusbest[ihit] = plusminus[ihit];
	  }
	  Double_t *spstub = sp->GetStubP();
	  for(Int_t i=0;i<4;i++) {
	    spstub[i] = stub[i];
	  }
	}
      } else {
	if (fhdebugflagpr) cout << "Insufficient planes hit in THcDriftChamber::LeftRight()" << bitpat <<endl;
      }
    } // End loop of pm combinations

    Double_t *spstub = sp->GetStubP();
    if(minchi2 > 9.9e9) {	// No track passed angle cut
      minchi2 = tmp_minchi2;
      for(Int_t ihit=0;ihit<nhits;ihit++) {
	plusminusbest[ihit] = tmp_plusminus[ihit];
      }
      for(Int_t i=0;i<4;i++) {
	spstub[i] = tmp_stub[i];
      }
      
    }

    // Calculate final coordinate based on plusminusbest
    // Update the hit positions in the space points
    for(Int_t ihit=0; ihit<nhits; ihit++) {
      // Save left/right status with the hit and in the space point
      // In THcDC will decide which to used based on fix_lr flag
      sp->GetHit(ihit)->SetLeftRight(plusminusbest[ihit]);
      sp->SetHitLR(ihit, plusminusbest[ihit]);
    }

    // Stubs are calculated in rotated coordinate system
    // (I think this rotates in case chambers not perpendicular to central ray)
    Int_t pindex=plane_list[0];
    if(spstub[2] - fTanBeta[pindex] == -1.0) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight(): stub3 error" << endl;
    }
    stub[2] = (spstub[2] - fTanBeta[pindex])
      / (1.0 + spstub[2]*fTanBeta[pindex]);
    if(spstub[2]*fSinBeta[pindex] ==  -fCosBeta[pindex]) {
      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight(): stub4 error" << endl;
    }
    stub[3] = spstub[3]
      / (spstub[2]*fSinBeta[pindex]+fCosBeta[pindex]);
    stub[0] = spstub[0]*fCosBeta[pindex]
      - spstub[0]*stub[2]*fSinBeta[pindex];
    stub[1] = spstub[1]
      - spstub[1]*stub[3]*fSinBeta[pindex];
    for(Int_t i=0;i<4;i++) {
      spstub[i] = stub[i];
    }
    if (fhdebugflagpr) cout << " Left/Right space pt " << isp+1 << " " << stub[0]<< " " << stub[1] << " " << stub[2]<< " " << stub[3] << endl;
      }
  // Option to print stubs
}
    //    if(fAA3Inv.find(bitpat) != fAAInv.end()) { // Valid hit combination
//_____________________________________________________________________________
Double_t THcDriftChamber::FindStub(Int_t nhits, THcSpacePoint *sp,
				       Int_t* plane_list, UInt_t bitpat,
				       Int_t* plusminus, Double_t* stub)
{
  // For a given combination of L/R, fit a stub to the space point
  Double_t zeros[] = {0.0,0.0,0.0};
  TVectorD TT; TT.Use(3, zeros);
  TVectorD dstub; dstub.Use(3, zeros);
  Double_t dpos[nhits];

  // This isn't right.  dpos only goes up to 2.
  for(Int_t ihit=0;ihit<nhits; ihit++) {
    dpos[ihit] = sp->GetHit(ihit)->GetPos()
      + plusminus[ihit]*sp->GetHit(ihit)->GetDist()
      - fPsi0[plane_list[ihit]];
    for(Int_t index=0;index<3;index++) {
      TT[index]+= dpos[ihit]*fStubCoefs[plane_list[ihit]][index]
	/fSigma[plane_list[ihit]];
    }
  }
  //  fAA3Inv[bitpat]->Print();
  //  if (fhdebugflagpr) cout << TT[0] << " " << TT[1] << " " << TT[2] << endl;
  //  TT->Print();

  //dstub = *(fAA3Inv[bitpat]) * TT;
  TT *= *fAA3Inv[bitpat];
  // if (fhdebugflagpr) cout << TT[0] << " " << TT[1] << " " << TT[2] << endl;
 //  if (fhdebugflagpr) cout << dstub[0] << " " << dstub[1] << " " << dstub[2] << endl;

  // Calculate Chi2.  Remember one power of sigma is in fStubCoefs
  stub[0] = TT[0];
  stub[1] = TT[1];
  stub[2] = TT[2];
  stub[3] = 0.0;
  Double_t chi2=0.0;
  for(Int_t ihit=0;ihit<nhits; ihit++) {
    chi2 += pow( dpos[ihit]/fSigma[plane_list[ihit]]
		 - fStubCoefs[plane_list[ihit]][0]*stub[0]
		 - fStubCoefs[plane_list[ihit]][1]*stub[1]
		 - fStubCoefs[plane_list[ihit]][2]*stub[2]
		 , 2);
  }
  return(chi2);
}

//_____________________________________________________________________________
THcDriftChamber::~THcDriftChamber()
{
  // Destructor. Remove variables from global list.

  if (fhdebugflagpr) cout << fChamberNum << ": delete fSpacePoints: " << hex << fSpacePoints << endl;
  delete fSpacePoints;
  // Should delete the matrices

  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();
  if (fTrackProj) {
    fTrackProj->Clear();
    delete fTrackProj; fTrackProj = 0;
  }
}

//_____________________________________________________________________________
void THcDriftChamber::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

}

//_____________________________________________________________________________
inline 
void THcDriftChamber::Clear( const Option_t* )
{
  // Reset per-event data.
  //  fNhits = 0;

  //  fTrackProj->Clear();
  fNhits = 0;

}

//_____________________________________________________________________________
Int_t THcDriftChamber::ApplyCorrections( void )
{
  return(0);
}

ClassImp(THcDriftChamber)
////////////////////////////////////////////////////////////////////////////////
