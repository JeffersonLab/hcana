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

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

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
}

//_____________________________________________________________________________
Int_t THcDriftChamber::Decode( const THaEvData& evdata )
{
  cout << "THcDriftChamber::Decode called" << endl;
  return 0;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcDriftChamber::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  Setup(GetName(), GetTitle());
  
  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaSubDetector::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;
}

void THcDriftChamber::AddPlane(THcDriftChamberPlane *plane)
{
  cout << "Added plane " << plane->GetPlaneNum() << " to chamber " << fChamberNum << endl;
  plane->SetPlaneIndex(fNPlanes);
  fPlanes[fNPlanes] = plane;

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
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  // Get parameters parent knows about
  THcDC* fParent;
  fParent = (THcDC*) GetParent();
  fMinHits = fParent->GetMinHits(fChamberNum);
  fMaxHits = fParent->GetMaxHits(fChamberNum);
  fMinCombos = fParent->GetMinCombos(fChamberNum);

  cout << "Chamber " << fChamberNum << "  Min/Max: " << fMinHits << "/" << fMaxHits << endl;

  fSpacePointCriterion = fParent->GetSpacePointCriterion(fChamberNum); 
  fSpacePointCriterion2 = fSpacePointCriterion*fSpacePointCriterion;

  // HMS Specific
  // Hard code Y plane numbers.  Should be able to get from wire angle
  if(fChamberNum == 1) {
    YPlaneNum = 2;
    YPlanePNum = 5;
  } else {
    YPlaneNum = 8;
    YPlanePNum = 11;
  }
    
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

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    TClonesArray* hitsarray = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<fPlanes[ip]->GetNHits();ihit++) {
      fHits[fNhits++] = static_cast<THcDCHit*>(hitsarray->At(ihit));
    }
  }
  //  cout << "ThcDriftChamber::ProcessHits() " << fNhits << " hits" << endl;
}


Int_t THcDriftChamber::FindSpacePoints( void )
{
  // Following h_pattern_recognition.f, but just for one chamber

  // Code below is specifically for HMS chambers with Y and Y' planes

  Int_t yplane_hitind=0;
  Int_t yplanep_hitind=0;

  fNSpacePoints=0;
  fEasySpacePoint = 0;
  // Should really build up array of hits
  if(fNhits >= fMinHits && fNhits < fMaxHits) {
    /* Has this list of hits already been cut the way it should have at this point? */
    
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      THcDCHit* thishit = fHits[ihit];
      Int_t ip=thishit->GetPlaneNum();  // This is the absolute plane mumber
      if(ip==YPlaneNum) yplane_hitind = ihit;
      if(ip==YPlanePNum) yplanep_hitind = ihit;
    }
    // fPlanes is the array of planes for this chamber.
    //    cout << fPlanes[YPlaneInd]->GetNHits() << " "
    //	 << fPlanes[YPlanePInd]->GetNHits() << " " << endl;
    if (fPlanes[YPlaneInd]->GetNHits() == 1 && fPlanes[YPlanePInd]->GetNHits() == 1) {
      cout << fHits[yplane_hitind]->GetWireNum() << " "
	   << fHits[yplane_hitind]->GetPos() << " "
	   << fHits[yplanep_hitind]->GetWireNum() << " "
	   << fHits[yplanep_hitind]->GetPos() << " " 
	   << fSpacePointCriterion << endl;
    }
    if(fPlanes[YPlaneInd]->GetNHits() == 1 && fPlanes[YPlanePInd]->GetNHits() == 1
       && TMath::Abs(fHits[yplane_hitind]->GetPos() - fHits[yplanep_hitind]->GetPos())
       < fSpacePointCriterion
       && fNhits <= 6) {	// An easy case, probably one hit per plane
      fEasySpacePoint = FindEasySpacePoint(yplane_hitind, yplanep_hitind);
    }
    if(!fEasySpacePoint) {	// It's not easy
      FindHardSpacePoints();
    }

    // We have our space points for this chamber
    cout << fNSpacePoints << " Space Points found" << endl;
    if(fNSpacePoints > 0) {
      if(fRemove_Sppt_If_One_YPlane == 1) {
	// The routine is specific to HMS
	Int_t ndest=DestroyPoorSpacePoints();
	cout << ndest << " Poor space points destroyed" << endl;
	// Loop over space points and remove those with less than 4 planes
	// hit and missing hits in Y,Y' planes
      }
      if(fNSpacePoints == 0) cout << "DestroyPoorSpacePoints() killed SP" << endl;
      Int_t nadded=SpacePointMultiWire();
      if (nadded) cout << nadded << " Space Points added with SpacePointMultiWire()" << endl;
      ChooseSingleHit();
      SelectSpacePoints();
      if(fNSpacePoints == 0) cout << "SelectSpacePoints() killed SP" << endl;
    }
    cout << fNSpacePoints << " Space Points remain" << endl;
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
  cout << "Doing Find Easy Space Point" << endl;
  Double_t yt = (fHits[yplane_hitind]->GetPos() + fHits[yplanep_hitind]->GetPos())/2.0;
  Double_t xt = 0.0;
  Int_t num_xhits = 0;
  Double_t x_pos[MAX_HITS_PER_POINT];

  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    THcDCHit* thishit = fHits[ihit];
    if(ihit!=yplane_hitind && ihit!=yplanep_hitind) { // x-like hit
      // ysp and xsp are from h_generate_geometry
      x_pos[ihit] = (thishit->GetPos()
		     -yt*thishit->GetWirePlane()->GetYsp())
	/thishit->GetWirePlane()->GetXsp();
      xt += x_pos[ihit];
      num_xhits++;
    } else {
      x_pos[ihit] = 0.0;
    }
  }
  xt = (num_xhits>0?xt/num_xhits:0.0);
  cout << "xt=" << xt << endl;
  easy_space_point = 1; // Assume we have an easy space point
  // Rule it out if x points don't cluster well enough
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    cout << "Hit " << ihit << " " << x_pos[ihit] << " " << xt << endl;
    if(ihit!=yplane_hitind && ihit!=yplanep_hitind) { // x-like hit
      if(TMath::Abs(xt-x_pos[ihit]) >= fSpacePointCriterion)
	{ easy_space_point=0; break;}
    }
  }
  if(easy_space_point) {	// Register the space point
    cout << "Easy Space Point " << xt << " " << yt << endl;
    fNSpacePoints = 1;
    fSpacePoints[0].x = xt;
    fSpacePoints[0].y = yt;
    fSpacePoints[0].nhits = fNhits;
    fSpacePoints[0].ncombos = 0; // No combos
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      THcDCHit* thishit = fHits[ihit];
      fSpacePoints[0].hits[ihit] = thishit;
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
      Double_t dist2 = pow(pairs[ipair1].x - pairs[ipair2].x,2)
	+ pow(pairs[ipair1].y - pairs[ipair2].y,2);
      if(dist2 <= fSpacePointCriterion2) {
	combos[ncombos].pair1 = &pairs[ipair1];
	combos[ncombos].pair2 = &pairs[ipair2];
	ncombos++;
      }
    }
  }
  // Loop over all valid combinations and build space points
  for(Int_t icombo=0;icombo<ncombos;icombo++) {
    THcDCHit* hits[4];
    hits[0]=combos[icombo].pair1->hit1;
    hits[1]=combos[icombo].pair1->hit2;
    hits[2]=combos[icombo].pair2->hit1;
    hits[3]=combos[icombo].pair2->hit2;
    // Get Average Space point xt, yt
    Double_t xt = combos[icombo].pair1->x + combos[icombo].pair2->x;
    Double_t yt = combos[icombo].pair1->y + combos[icombo].pair2->y;
    // Loop over space points
    if(fNSpacePoints > 0) {
      Int_t add_flag=1;
      for(Int_t ispace=0;ispace<fNSpacePoints;ispace++) {
	if(fSpacePoints[ispace].nhits > 0) {
	  Double_t sqdist_test = pow(xt - fSpacePoints[ispace].x,2) +
	    pow(yt - fSpacePoints[ispace].y,2);
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
	    for(Int_t isp_hit=0;isp_hit<fSpacePoints[ispace].nhits;isp_hit++) {
	      for(Int_t icm_hit=0;icm_hit<4;icm_hit++) { // Loop over combo hits
		if(fSpacePoints[ispace].hits[isp_hit]==hits[icm_hit]) {
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
		fSpacePoints[ispace].hits[fSpacePoints[ispace].nhits++] = hits[icm];
	      }
	      fSpacePoints[ispace].ncombos++;
	    }
	  }
	}
      }// End of loop over existing space points
      // Create a new space point if more than 2*space_point_criteria
      if(fNSpacePoints < MAX_SPACE_POINTS) {
	if(add_flag) {
	  fSpacePoints[fNSpacePoints].nhits=2;
	  fSpacePoints[fNSpacePoints].ncombos=1;
	  fSpacePoints[fNSpacePoints].hits[0]=hits[0];
	  fSpacePoints[fNSpacePoints].hits[1]=hits[1];
	  fSpacePoints[fNSpacePoints].x = xt;
	  fSpacePoints[fNSpacePoints].y = yt;
	  if(hits[0] != hits[2] && hits[1] != hits[2]) {
	    fSpacePoints[fNSpacePoints].hits[fSpacePoints[fNSpacePoints].nhits++] = hits[2];
	  }
	  if(hits[0] != hits[3] && hits[1] != hits[3]) {
	    fSpacePoints[fNSpacePoints].hits[fSpacePoints[fNSpacePoints].nhits++] = hits[3];
	  }
	  fNSpacePoints++;
	}
      }
    } else {// Create first space point
      // This duplicates code above.  Need to see if we can restructure
      // to avoid
      fSpacePoints[fNSpacePoints].nhits=2;
      fSpacePoints[fNSpacePoints].ncombos=1;
      fSpacePoints[fNSpacePoints].hits[0]=hits[0];
      fSpacePoints[fNSpacePoints].hits[1]=hits[1];
      fSpacePoints[fNSpacePoints].x = xt;
      fSpacePoints[fNSpacePoints].y = yt;
      if(hits[0] != hits[2] && hits[1] != hits[2]) {
	fSpacePoints[fNSpacePoints].hits[fSpacePoints[fNSpacePoints].nhits++] = hits[2];
      }
      if(hits[0] != hits[3] && hits[1] != hits[3]) {
	fSpacePoints[fNSpacePoints].hits[fSpacePoints[fNSpacePoints].nhits++] = hits[3];
      }
      fNSpacePoints++;
    }//End check on 0 space points
  }//End loop over combos
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
    for(Int_t ihit=0;ihit<fSpacePoints[isp].nhits;ihit++) {
      THcDCHit* hit=fSpacePoints[isp].hits[ihit];
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
      //      cout << "Missing Y-hit!!";
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
      fSpacePoints[isp] = fSpacePoints[osp];
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

  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    Int_t nplanes_hit = 0;	// Number of planes with hits
    Int_t nplanes_mult = 0;	// Number of planes with multiple hits
    Int_t nsp_new = 1;
    Int_t newsp_num=0;

    for(Int_t ip=0;ip<fNPlanes;ip++) {
      nhitsperplane[ip] = 0;
    }
    // Sort Space Points hits by plane
    for(Int_t ihit=0;ihit<fSpacePoints[isp].nhits;ihit++) { // All hits in SP
      THcDCHit* hit=fSpacePoints[isp].hits[ihit];
      //      hit_order Make a hash
      // hash(hit) = ihit;
      Int_t ip = hit->GetPlaneIndex();
      hits_plane[ip][nhitsperplane[ip]++] = hit;
    }
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      if(nhitsperplane[ip] > 0) {
	nplanes_hit++;
	nsp_new *= nhitsperplane[ip];
	if(nhitsperplane[ip] > 1) nplanes_mult++;
      }
    }
    --nsp_new;
    nsp_check=nsp_tot + nsp_new;
    nplanes_single = nplanes_hit - nplanes_mult;
    
    // Check if cloning conditions are met
    Int_t ntot = 0;
    if(nplanes_hit >= 4 && nplanes_mult < 4 && nplanes_mult >0
       && nsp_check < 20) {
      
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

      // First fill clones with 1 hit each from the 3 planes with the most hits
      for(Int_t n1=0;n1<nhitsperplane[maxplane[0]];n1++) {
	for(Int_t n2=0;n2<nhitsperplane[maxplane[1]];n2++) {
	  for(Int_t n3=0;n3<nhitsperplane[maxplane[2]];n3++) {
	    ntot++;
	    newsp_num = nsp_tot + ntot - 2; // ntot will be 2 for first new
	    if(n1==0 && n2==0 && n3==0) newsp_num = isp; // Copy over original SP
	    fSpacePoints[newsp_num].x = fSpacePoints[isp].x;
	    fSpacePoints[newsp_num].y = fSpacePoints[isp].y;
	    fSpacePoints[newsp_num].nhits = nplanes_hit;
	    fSpacePoints[newsp_num].ncombos = fSpacePoints[isp].ncombos;
	    fSpacePoints[newsp_num].hits[0] = hits_plane[maxplane[0]][n1];
	    fSpacePoints[newsp_num].hits[1] = hits_plane[maxplane[1]][n2];
	    fSpacePoints[newsp_num].hits[2] = hits_plane[maxplane[2]][n3];
	    fSpacePoints[newsp_num].hits[3] = hits_plane[maxplane[3]][0];
	    if(nhitsperplane[maxplane[4]] == 1)
	      fSpacePoints[newsp_num].hits[4] = hits_plane[maxplane[4]][0];
	    if(nhitsperplane[maxplane[5]] == 1)
	      fSpacePoints[newsp_num].hits[5] = hits_plane[maxplane[5]][0];
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
      ntot=1;
    }
  }
  assert (nsp_tot > 0);
  Int_t nadded=0;
  if(nsp_tot <= 20) {
    nadded = nsp_tot - fNSpacePoints;
    fNSpacePoints = nsp_tot;
  }

  // In Fortran, fill in zeros.
  return(nadded);
}

//_____________________________________________________________________________
// HMS Specific?
void THcDriftChamber::ChooseSingleHit()
{
  // Look at all hits in a space point.  If two hits are in the same plane,
  // reject the one with the longer drift time.
  Int_t goodhit[MAX_HITS_PER_POINT];

  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    Int_t startnum = fSpacePoints[isp].nhits;
    
    for(Int_t ihit=0;ihit<startnum;ihit++) {
      goodhit[ihit] = 1;
    }
    // For each plane, mark all hits longer than the shortest drift time
    for(Int_t ihit1=0;ihit1<startnum-1;ihit1++) {
      THcDCHit* hit1 = fSpacePoints[isp].hits[ihit1];
      Int_t plane1=hit1->GetPlaneIndex();
      Double_t tdrift1 = hit1->GetTime();
      for(Int_t ihit2=ihit1+1;ihit2<startnum;ihit2++) {
	THcDCHit* hit2 = fSpacePoints[isp].hits[ihit2];
	Int_t plane2=hit2->GetPlaneIndex();
	Double_t tdrift2 = hit2->GetTime();
	if(plane1 == plane2) {
	  if(tdrift1 > tdrift2) {
	    goodhit[ihit1] = 0;
	  } else {
	    goodhit[ihit2] = 0;
	  }
	}
      }
    }
    // Gather the remaining hits
    Int_t finalnum = 0;
    for(Int_t ihit=0;ihit<startnum;ihit++) {
      if(goodhit[ihit] > 0) {	// Keep this hit
	if (ihit > finalnum) {	// Move hit 
	  fSpacePoints[isp].hits[finalnum++] = fSpacePoints[isp].hits[ihit];
	} else {
	  finalnum++;
	}
      }
    }
    fSpacePoints[isp].nhits = finalnum;
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
    if(fSpacePoints[isp].ncombos >= fMinCombos || fEasySpacePoint) {
      if(fSpacePoints[isp].nhits >= fMinHits) {
	fSpacePoints[sp_count++] = fSpacePoints[isp];
      }
    }
  }
  fNSpacePoints = sp_count;
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
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    Double_t x = fSpacePoints[isp].x;
    Double_t y = fSpacePoints[isp].y;
    for(Int_t ihit=0;ihit<fSpacePoints[isp].nhits;ihit++) {
      THcDCHit* hit = fSpacePoints[isp].hits[ihit];
      THcDriftChamberPlane* plane=hit->GetWirePlane();

      // How do we know this correction only gets applied once?  Is
      // it determined that a given hit can only belong to one space point?
      Double_t time_corr = plane->GetReadoutX() ?
	y*plane->GetReadoutCorr()/fWireVelocity :
	x*plane->GetReadoutCorr()/fWireVelocity;
      
      //      cout << "Correcting hit " << hit << " " << plane->GetPlaneNum() << " " << isp << "/" << ihit << "  " << x << "," << y << endl;
      // Fortran ENGINE does not do this check, so hits can get "corrected"
      // multiple times if they belong to multiple space points.
      // To reproduce the precise ENGINE behavior, remove this if condition.
      if(! hit->GetCorrectedStatus()) {
	hit->SetTime(hit->GetTime() - plane->GetCentralTime()
		     + plane->GetDriftTimeSign()*time_corr);
	hit->ConvertTimeToDist();
	hit->SetCorrectedStatus(1);
      }
    }
  }
}	   
//_____________________________________________________________________________
THcDriftChamber::~THcDriftChamber()
{
  // Destructor. Remove variables from global list.

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
