/** \class THcDriftChamber
    \ingroup DetSupport

    \brief Subdetector class for a single drift chamber with several planes.

    This class will be created by the THcDC class which will also create
    the plane objects.

    The THcDC class will then pass this class a list of the planes.

*/

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
#include "THaApparatus.h"

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
  fTrackProj = NULL;
  fNPlanes = 0;			// No planes until we make them

  fChamberNum = chambernum;

  fSpacePoints = new TClonesArray("THcSpacePoint",10);

  fHMSStyleChambers = 0;	// Default
}

//_____________________________________________________________________________
THcDriftChamber::THcDriftChamber() :
  THaSubDetector()
{
  // Constructor
  fTrackProj = NULL;
  fSpacePoints = NULL;
  fIsInit = 0;

}
//_____________________________________________________________________________
void THcDriftChamber::Setup(const char* name, const char* description)
{

}

//_____________________________________________________________________________
Int_t THcDriftChamber::Decode( const THaEvData& evdata )
{
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
  plane->SetPlaneIndex(fNPlanes);
  fPlanes.push_back(plane);
  // HMS Specific
  // Hard code Y plane numbers.  Eventually need to get from database
  if (fHMSStyleChambers) {
    if(fChamberNum == 1) {
      YPlaneNum=2;
      YPlanePNum=5;
      if(plane->GetPlaneNum() == 2) YPlaneInd = fNPlanes;
      if(plane->GetPlaneNum() == 5) YPlanePInd = fNPlanes;
    } else {
      YPlaneNum=8;
      YPlanePNum=11;
      if(plane->GetPlaneNum() == 8) YPlaneInd = fNPlanes;
      if(plane->GetPlaneNum() == 11) YPlanePInd = fNPlanes;
    }
  } else {
    // SOS Specific
    // Hard code X plane numbers.   Eventually need to get from database
    if(fChamberNum == 1) {
      XPlaneNum=3;
      XPlanePNum=4;
      if(plane->GetPlaneNum() == 3) XPlaneInd = fNPlanes;
      if(plane->GetPlaneNum() == 4) XPlanePInd = fNPlanes;
    } else {
      XPlaneNum=9;
      XPlanePNum=10;
      if(plane->GetPlaneNum() == 9) XPlaneInd = fNPlanes;
      if(plane->GetPlaneNum() == 10) XPlanePInd = fNPlanes;
    }
  }
  fNPlanes++;
  return;
}

//_____________________________________________________________________________
Int_t THcDriftChamber::ReadDatabase( const TDatime& date )
{

  // cout << "THcDriftChamber::ReadDatabase()" << endl;
  char prefix[2];
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  DBRequest list[]={
    {"_remove_sppt_if_one_y_plane",&fRemove_Sppt_If_One_YPlane, kInt,0,1},
    {"dc_wire_velocity", &fWireVelocity, kDouble},
    {"SmallAngleApprox", &fSmallAngleApprox, kInt,0,1},
    {"stub_max_xpdiff", &fStubMaxXPDiff, kDouble,0,1},
    {"debugflagpr", &fhdebugflagpr, kInt},
    {"debugstubchisq", &fdebugstubchisq, kInt},
    {Form("dc_%d_zpos",fChamberNum), &fZPos, kDouble},
    {0}
  };
  fSmallAngleApprox=0;
  fRatio_xpfp_to_xfp=1.0;
  TString SHMS="p";
  TString HMS="h";
  TString test=prefix[0];
  if (test==SHMS ) fRatio_xpfp_to_xfp=0.0018; // SHMS 
  if (test == HMS ) fRatio_xpfp_to_xfp=0.0011; // HMS 
  fRemove_Sppt_If_One_YPlane = 0; // Default
  fStubMaxXPDiff = 999.;	  // 
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  // Get parameters parent knows about
  fParent = GetParent();
  fMinHits = static_cast<THcDC*>(fParent)->GetMinHits(fChamberNum);
  fMaxHits = static_cast<THcDC*>(fParent)->GetMaxHits(fChamberNum);
  fMinCombos = static_cast<THcDC*>(fParent)->GetMinCombos(fChamberNum);
  fFixPropagationCorrection = static_cast<THcDC*>(fParent)->GetFixPropagationCorrectionFlag();

  fSpacePointCriterion = static_cast<THcDC*>(fParent)->GetSpacePointCriterion(fChamberNum);
  fMaxDist = TMath::Sqrt(fSpacePointCriterion/2.0); // For easy space points

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
      TMatrixD AA3(3,3);
      for(Int_t i=0;i<3;i++) {
	for(Int_t j=i;j<3;j++) {
	  AA3[i][j] = 0.0;
	  for(Int_t ip=0;ip<fNPlanes;ip++) {
	    if(ipm1 != ip && ipm2 != ip) {
	      AA3[i][j] += fStubCoefs[ip][i]*fStubCoefs[ip][j];
	    }
	  }
	  AA3[j][i] = AA3[i][j];
	}
      }
      Int_t bitpat = allplanes & ~(1<<ipm1) & ~(1<<ipm2);
      // Should check that it is invertable
      //      if (fhdebugflagpr) cout << bitpat << " Determinant: " << AA3->Determinant() << endl;
      AA3.Invert();
      fAA3Inv[bitpat].ResizeTo(AA3);
      fAA3Inv[bitpat] = AA3;
    }
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

  RVarDef vars[] = {
    { "maxhits",     "Maximum hits allowed",    "fMaxHits" },
    { "spacepoints", "Space points of DC",      "fNSpacePoints" },
    { "nhit", "Number of DC hits",  "fNhits" },
    { "trawhit", "Number of True Raw hits", "fN_True_RawHits" },
    { "stub_x", "", "fSpacePoints.THcSpacePoint.GetStubX()" },
    { "stub_xp", "", "fSpacePoints.THcSpacePoint.GetStubXP()" },
    { "stub_y", "", "fSpacePoints.THcSpacePoint.GetStubY()" },
    { "stub_yp", "", "fSpacePoints.THcSpacePoint.GetStubYP()" },
    { "ncombos", "", "fSpacePoints.THcSpacePoint.GetCombos()" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );
  //return kOK;

}
void THcDriftChamber::ProcessHits( void)
{
  // Make a list of hits for whole chamber
  fNhits = 0;
  fHits.clear();
  fHits.reserve(40);

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    TClonesArray* hitsarray = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<fPlanes[ip]->GetNHits();ihit++) {
      fHits.push_back(static_cast<THcDCHit*>(hitsarray->At(ihit)));
      fNhits++;
    }
  }
  //  if (fhdebugflagpr) cout << "ThcDriftChamber::ProcessHits() " << fNhits << " hits" << endl;
}


void THcDriftChamber::PrintDecode( void )
{
  cout << " Num of nits = " << fNhits << endl;
  cout << " Num " << " Plane "  << " Wire " <<  "  Wire-Center  " << " RAW TDC " << " Drift time" << endl;
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    THcDCHit* thishit = fHits[ihit];
    cout << ihit << "       " <<thishit->GetPlaneNum()  << "     " << thishit->GetWireNum() << "     " <<  thishit->GetPos() << "    " << thishit->GetRawTime() << "    " << thishit->GetTime() << endl;
  }
}


Int_t THcDriftChamber::FindSpacePoints( void )
{
  /**
1. First check if number of hits is between fMinHits=min_hit and fMaxHits=max_pr_hits
   if fails then fNSpacePoints=0
2. Determines if it is an easy spacepoint.
  1. if HMS style chambers finds the Y and Y' planes
     if not HMS style chambers finds the X and X' planes
  2. if both planes ahve only one hit and the difference in hit position
     between the hits < fSpacePointCriterion and less than 6 hits .
     then easy spacepoint and calls either method FindEasySpacePoint_HMS or FindEasySpacePoint_SOS
  3. FindEasySpacePoint_SOS
     1. Loops through hits and gets y_pos from the non-X planes and determines average Yt
     2. Loops through hits and sees if any of "Y-planes" hits has 
          difference of y_pos -Yt > TMath::Sqrt(fSpacePointCriterion/2.0)
     3. Adds a spacepoint and set ncombos to zero.
3. if not  EasySpacePoint calls FindHardSpacePoints
  1. loops though hits and determines pairs of hits in planes with angles grerater then 17.5 degs
      between them. These are test pairs and stores the x and y position of pair
  1. Double loops through the test pairs to determine number of pair combinations.
     1. Calculates d2 = (xi -xj)^2 + (yi-yj)^2 from the two pairs (i,j).
     2. If d2 <  fSpacePointCriterion then fills combos structure with pair info
         and increments ncombos.
  c. Loop through ncombos
     1. First combo is set as spacepoint which is loaded with hit info from combos.
     2. Next combo 
        1. Loops through previous space points
        2. calculates  d2 = (x_c -x_sp)^2 + (y_c-y_sp)^2 between combos and spacepoint
           if d2 < fSpacePointCriterion then adds combos hit info to that spacepoint
           which is not already in the spacepoint.
        3. if that combo is not already added to existing spacepoint
           then new spacepoint is made from the combo.
4. If it found a spacepoint
    1. For HMS-style chamber it would DestroyPoorSpacePoints if fRemove_Sppt_If_One_YPlane
    2. Presently if  HMS-style chamber calls SpacePointMultiWire()
    3. Calls ChooseSingleHit this looks to see if two hits in the same plane.
             If two hits then rejects on with longer drift time.
    4. calls SelectSpacePoints. Goes through the spacepoints and eliminates spacepoints
          that do not have nhits >  min_hits and ncombos> min_combos 
          ( exception for easyspacepoint)
  */
  // fSpacePoints->Clear();
  fSpacePoints->Delete();

  Int_t plane_hitind=0;
  Int_t planep_hitind=0;


  fNSpacePoints=0;
  fEasySpacePoint = 0;
  if(fNhits >= fMinHits && fNhits < fMaxHits) {
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      THcDCHit* thishit = fHits[ihit];
      Int_t ip=thishit->GetPlaneNum();  // This is the absolute plane mumber
      if(ip==YPlaneNum  && fHMSStyleChambers) plane_hitind = ihit;
      if(ip==YPlanePNum && fHMSStyleChambers) planep_hitind = ihit;
      if(ip==XPlaneNum  && !fHMSStyleChambers) plane_hitind = ihit;
      if(ip==XPlanePNum && !fHMSStyleChambers) planep_hitind = ihit;
    }
    Int_t PlaneInd=0,PlanePInd=0;
    if (fHMSStyleChambers) {
      PlaneInd=YPlaneInd;
      PlanePInd=YPlanePInd;
    } else {
      PlaneInd=XPlaneInd;
      PlanePInd=XPlanePInd;
    }
    if(fPlanes[PlaneInd]->GetNHits() == 1 && fPlanes[PlanePInd]->GetNHits() == 1
       && pow( (fHits[plane_hitind]->GetPos() - fHits[planep_hitind]->GetPos()),2)
       < fSpacePointCriterion
       && fNhits <= 6) {	// An easy case, probably one hit per plane
      if(fHMSStyleChambers) fEasySpacePoint = FindEasySpacePoint_HMS(plane_hitind, planep_hitind);
      if(!fHMSStyleChambers) fEasySpacePoint = FindEasySpacePoint_SOS(plane_hitind, planep_hitind);
    }
    if(!fEasySpacePoint) {	// It's not easy
      FindHardSpacePoints();
    }
    // We have our space points for this chamber
    if(fNSpacePoints > 0) {
      if(fHMSStyleChambers) {
	if(fRemove_Sppt_If_One_YPlane == 1) {
	  // The routine is specific to HMS
	  //Int_t ndest=
	  DestroyPoorSpacePoints(); // Only for HMS?
	  // Loop over space points and remove those with less than 4 planes
	  // hit and missing hits in Y,Y' planes
	}
	Int_t nadded=SpacePointMultiWire(); // Only for HMS?
	if (nadded) if (fhdebugflagpr) cout << nadded << " Space Points added with SpacePointMultiWire()" << endl;
      }
      ChooseSingleHit();
      SelectSpacePoints();
      //      if(fNSpacePoints == 0) if (fhdebugflagpr) cout << "SelectSpacePoints() killed SP" << endl;
    }
    //    if (fhdebugflagpr) cout << fNSpacePoints << " Space Points remain" << endl;
  }
  return(fNSpacePoints);
}

//_____________________________________________________________________________
// HMS Specific
Int_t THcDriftChamber::FindEasySpacePoint_HMS(Int_t yplane_hitind,Int_t yplanep_hitind)
{
  /**
     Simplified HMS find_space_point routing.  It is given all y hits and
     checks to see if all x-like hits are close enough together to make
     a space point.

     This is used for the old style HMS drift chambers, so is not used for any
     data since the new HMS chambers were installed before the Fall 2017 run.
  */

  Int_t easy_space_point=0;
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
  easy_space_point = 1; // Assume we have an easy space point
  // Rule it out if x points don't cluster well enough
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    if(ihit!=yplane_hitind && ihit!=yplanep_hitind) { // select x-like hit
      if(TMath::Abs(xt-x_pos[ihit]) >= fMaxDist)
	{ easy_space_point=0; break;}
    }
  }
  if(easy_space_point) {	// Register the space point
    THcSpacePoint* sp = (THcSpacePoint*)fSpacePoints->ConstructedAt(fNSpacePoints++);
    sp->Clear();
    sp->SetXY(xt, yt);
    sp->SetCombos(0);
    for(Int_t ihit=0;ihit<fNhits;ihit++) {
      sp->AddHit(fHits[ihit]);
    }
  }
  return(easy_space_point);
}
// SOS Specific
Int_t THcDriftChamber::FindEasySpacePoint_SOS(Int_t xplane_hitind,Int_t xplanep_hitind)
{
  /**
     Simplified SOS find_space_point routing.  It is given all x hits and
     checks to see if all y-like hits are close enough together to make
     a space point.

     This is used for the SHMS and HMS (since Fall 2017) drift chambers which are
     of the same style as the old SOS chambers (XUV).
  */

  Int_t easy_space_point=0;
  Double_t xt = (fHits[xplane_hitind]->GetPos() + fHits[xplanep_hitind]->GetPos())/2.0;
  Double_t yt = 0.0;
  Int_t num_yhits = 0;
  Double_t y_pos[MAX_HITS_PER_POINT];

  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    THcDCHit* thishit = fHits[ihit];
    if(ihit!=xplane_hitind && ihit!=xplanep_hitind) { // y-like hit
      // ysp and xsp are from h_generate_geometry
      y_pos[ihit] = (thishit->GetPos()
		     -xt*thishit->GetWirePlane()->GetXsp())
	/thishit->GetWirePlane()->GetYsp();
      yt += y_pos[ihit];
      num_yhits++;
    } else {
      y_pos[ihit] = 0.0;
    }
  }
  yt = (num_yhits>0?yt/num_yhits:0.0);
  easy_space_point = 1; // Assume we have an easy space point
  // Rule it out if x points don't cluster well enough
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    if(ihit!=xplane_hitind && ihit!=xplanep_hitind) { // select y-like hit
      if(TMath::Abs(yt-y_pos[ihit]) >= fMaxDist)
	{ easy_space_point=0; break;}
    }
  }
  if(easy_space_point) {	// Register the space point
    THcSpacePoint* sp = (THcSpacePoint*)fSpacePoints->ConstructedAt(fNSpacePoints++);
    sp->Clear();
    sp->SetXY(xt, yt);
    sp->SetCombos(0);
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
	if(dist2 <= fSpacePointCriterion) {
	  combos[ncombos].pair1 = &pairs[ipair1];
	  combos[ncombos].pair2 = &pairs[ipair2];
	  ncombos++;
	}
      }
    }
  }
  // Loop over all valid combinations and build space points
  //if (fhdebugflagpr) cout << "looking for hard Space Point combos = " << ncombos << endl;
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
	  // 3 fSpacePointCriterion.  Let me ignore not add a new point the
	  if(sqdist_test < 3*fSpacePointCriterion) {
	    add_flag = 0;	// do not add a new space point
	  }
	  if(sqdist_test < fSpacePointCriterion) {
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
	    //            cout << " number of combos = " << sp->GetCombos() << endl;
	    // Terminate loop since this combo can only belong to one space point
	    break;
	  }
	}
      }// End of loop over existing space points
      // Create a new space point if more than 2*space_point_criteria
      if(fNSpacePoints < MAX_SPACE_POINTS) {
	if(add_flag) {
          //if (fhdebugflagpr) cout << " add glag = " << add_flag << " space pts =  " << fNSpacePoints << endl ;
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
      //if (fhdebugflagpr) cout << "1st hard Space Point " << xt << " " << yt << " Space point # ="  << fNSpacePoints << " combos = " << sp->GetCombos() << endl;
    }//End check on 0 space points
  }//End loop over combos
  //if (fhdebugflagpr) cout << " finished findspacept # of sp pts = " << fNSpacePoints << endl;
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
Int_t THcDriftChamber::SpacePointMultiWire()
{
/**
   This method is only used for the old style HMS chambers (before Fall 2017).

   This routine loops over space points and
   looks at all hits in the space
   point. If more than 1 hit is in the same
   plane then the space point is cloned with
   all combinations of 1 wire per plane.  The
   requirements for cloning are:  1) at least
   4 planes fire, and 2) no more than 6 planes
   have multiple hits.
*/
  Int_t nhitsperplane[fNPlanes];
  THcDCHit* hits_plane[fNPlanes][MAX_HITS_PER_POINT];

  Int_t nsp_check;
  //Int_t nplanes_single;

  Int_t nsp_tot=fNSpacePoints;
  Int_t nsp_totl=fNSpacePoints;
  //if (fhdebugflagpr) cout << "Start  Multiwire # of sp pts = " << nsp_totl << endl;

  for(Int_t isp=0;isp<nsp_totl;isp++) {
    Int_t nplanes_hit = 0;	// Number of planes with hits
    Int_t nplanes_mult = 0;	// Number of planes with multiple hits
    Int_t nsp_new = 1;
    Int_t newsp_num=0;
    //if (fhdebugflagpr) cout << "Looping thru space pts at # = " << isp << " total = " << fNSpacePoints << endl;

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
    //nplanes_single = nplanes_hit - nplanes_mult;
    //if (fhdebugflagpr) cout << " # of new space points = " << nsp_new << " total now = " << nsp_tot<< endl;
    // Check if cloning conditions are met
    Int_t ntot = 0;
    if(nplanes_hit >= 4 && nplanes_mult < 4 && nplanes_mult >0
       && nsp_check < 20) {
      //if (fhdebugflagpr) cout << " Cloning space point " << endl;
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
	    newsp_num = fNSpacePoints; //
	    //if (fhdebugflagpr) cout << " new space pt num = " << newsp_num  << " " << fNSpacePoints <<  endl;
	    //THcSpacePoint* newsp;
	    if(n1==0 && n2==0 && n3==0) {
	      newsp_num = isp; // Copy over the original SP
	      THcSpacePoint* newsp = (THcSpacePoint*)fSpacePoints->ConstructedAt(newsp_num);//= (THcSpacePoint*)(*fSpacePoints)[newsp_num];
              //if (fhdebugflagpr) cout << " Copy over original SP " << endl;
	      // newsp = sp;
	      Int_t combos_save=sp->GetCombos();
	      newsp->Clear();	// Clear doesn't clear X, Y
	      // if (fhdebugflagpr) cout << " original sp #hits combos X y " << sp->GetCombos() << sp->GetNHits() << sp->GetX() << " " <<  sp->GetY() << endl;
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
	    } else {
	      // if (fhdebugflagpr) cout << " setting other sp " << "# space pts now = " << fNSpacePoints << endl;
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
	    }
	  }
	}
      }
      // In the ENGINE, we loop over the clones and order the hits in the
      // same order as the parent SP.  It is not done here because it is a little
      // tricky.  Is it necessary?
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
  //if (fhdebugflagpr) cout << " Added space pts " << nadded << " total space pts = " << fNSpacePoints << endl;

  // In Fortran, fill in zeros.
  return(nadded);
}

//_____________________________________________________________________________
// Generic
void THcDriftChamber::ChooseSingleHit()
{
  /**
     Look at all hits in a space point.  If two hits are in the same plane,
     reject the one with the longer drift time.
  */
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
	}
      }
    }
    // Gather the remaining hits
    Int_t finalnum = 0;
    for(Int_t ihit=0;ihit<startnum;ihit++) {
      if(goodhit[ihit] > 0) {	// Keep this hit
	if (ihit > finalnum) {	// Move hit
	  sp->ReplaceHit(finalnum++, sp->GetHit(ihit));
	} else {
          finalnum++ ;
        }
      }
    }
    sp->SetNHits(finalnum);
    // if (fhdebugflagpr) cout << " choose single hit start # of hits = " <<  startnum << " final # = " <<finalnum << endl;
  }
}
//_____________________________________________________________________________
// Generic
void THcDriftChamber::SelectSpacePoints()
{
  /**
     This routine goes through the list of space_points and space_point_hits
     found by find_space_points and only accepts those with
     number of hits > min_hits
     number of combinations > min_combos
  */
  Int_t sp_count=0;
  for(Int_t isp=0;isp<fNSpacePoints;isp++) {
    // Include fEasySpacePoint because ncombos not filled in
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    if(sp->GetCombos() >= fMinCombos || fEasySpacePoint) {
      if(sp->GetNHits() >= fMinHits) {
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
  fNSpacePoints = sp_count;
  //for(Int_t isp=0;isp<fNSpacePoints;isp++) {
  //  THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    //if (fhdebugflagpr) cout << " sp pt = " << isp+1 << " # of hits = " << sp->GetNHits() << endl;
    //for(Int_t ihit=0;ihit<sp->GetNHits();ihit++) {
  //THcDCHit* hit = sp->GetHit(ihit);
      //THcDriftChamberPlane* plane=hit->GetWirePlane();
      //        if (fhdebugflagpr) cout << ihit+1 << "selecting " << plane->GetPlaneNum() << " " << plane->GetChamberNum() << " " << hit->GetTime() << " " << hit->GetDist() << " " << plane->GetCentralTime() << " " << plane->GetDriftTimeSign() << endl;
  //    }
  //  }
}

void THcDriftChamber::CorrectHitTimes()
{
  /**
   Use the rough hit positions in the chambers to correct the drift time
   for hits in the space points.

   Assume all wires for a plane are read out on the same side (l/r or t/b).
   If the wire is closer to horizontal, read out left/right.  If nearer
   vertical, assume top/bottom.  (Note, this is not always true for the
   SOS u and v planes.  They have 1 card each on the side, but the overall
   time offset per card will cancel much of the error caused by this.  The
   alternative is to check by card, rather than by plane and this is harder.
  */

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

      // This applies the wire velocity correction for new SHMS chambers --hszumila, SEP17
      if (!fHMSStyleChambers){
	Int_t pln = hit->GetPlaneNum();
	Int_t readoutSide = hit->GetReadoutSide();

	Double_t posn = hit->GetPos();
	//The following values are determined from param file as permutations on planes 5 and 10
	Int_t readhoriz = plane->GetReadoutLR();
	Int_t readvert = plane->GetReadoutTB();

	//+x is up and +y is beam right!
	double alpha = static_cast<THcDC*>(fParent)->GetAlphaAngle(pln);
	double xc = posn*TMath::Sin(alpha);
	double yc = posn*TMath::Cos(alpha);

	Double_t wireDistance = plane->GetReadoutX() ?
	  (abs(y-yc))*abs(plane->GetReadoutCorr()) :
	  (abs(x-xc))*abs(plane->GetReadoutCorr());

	//Readout side is based off wiring diagrams
	switch (readoutSide){
	case 1: //readout from top of chamber
	  if (x>xc){wireDistance = -readvert*wireDistance;}
	  else{wireDistance = readvert*wireDistance;}
	  
	  break;
	case 2://readout from right of chamber
	  if (y>yc){wireDistance = -readhoriz*wireDistance;}
	  else{wireDistance = readhoriz*wireDistance;}
  
	  break;
	case 3: //readout from bottom of chamber
	  if (xc>x){wireDistance= -readvert*wireDistance;}
	  else{wireDistance = readvert*wireDistance;}

	  break;
	case 4: //readout from left of chamber
	  if(yc>y){wireDistance = -readhoriz*wireDistance;}
	  else{wireDistance = readhoriz*wireDistance;}

	  break;
	default:
	  wireDistance = 0.0;
	}

	Double_t timeCorrection = wireDistance/fWireVelocity;

	if(fFixPropagationCorrection==0) { // ENGINE behavior
	  Double_t time=hit->GetTime();
	  hit->SetTime(time - timeCorrection);
	  hit->ConvertTimeToDist();
	} else {
	  Double_t time=hit->GetTime();
	  Double_t dist=hit->GetDist();

	  hit->SetTime(time - timeCorrection);
	  //double usingOldTime = time-time_corr;
	  //hit->SetTime(time- time_corr);

	  hit->ConvertTimeToDist();
	  sp->SetHitDist(ihit, hit->GetDist());

	  hit->SetTime(time);	// Restore time
	  hit->SetDist(dist);	// Restore distance
	}

      } else {
	/////////////////////////////////////////////////////////////
	
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
}
UInt_t THcDriftChamber::Count1Bits(UInt_t x)
// From http://graphics.stanford.edu/~seander/bithacks.html
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

//_____________________________________________________________________________
void THcDriftChamber::LeftRight()
{
  /**
     For each space point,
     Fit stubs to all possible left-right combinations of drift distances
     and choose the set with the minimum chi**2.
  */

  for(Int_t isp=0; isp<fNSpacePoints; isp++) {
    // Build a bit pattern of which planes are hit
    THcSpacePoint* sp = (THcSpacePoint*)(*fSpacePoints)[isp];
    Int_t nhits = sp->GetNHits();
    UInt_t bitpat  = 0;		// Bit pattern of which planes are hit
    Double_t maxchi2= 1.0e10;
    Double_t minchi2 = maxchi2;
    Double_t tmp_minchi2=maxchi2;
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
    if(fHMSStyleChambers) {
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
	//	if (fhdebugflagpr) cout << " Small angle approx = " << smallAngOK << " " << plusminusknown[hasy1] << endl;
	//if (fhdebugflagpr) cout << "pm =  " << plusminusknown[hasy2] << " " << hasy1 << " " << hasy2 << endl;
	//if (fhdebugflagpr) cout << " Plane index " << YPlaneInd << " " << YPlanePInd << endl;
      }
    } else {			// SOS Style
      if(fSmallAngleApprox !=0) {
	// Brookhaven style chamber L/R code
	Int_t npaired=0;
	for(Int_t ihit1=0;ihit1 < nhits;ihit1++) {
	  THcDCHit* hit1 = sp->GetHit(ihit1);
	  Int_t pindex1=hit1->GetPlaneIndex();
	  if((pindex1%2)==0) { // Odd plane (or even index)
	    for(Int_t ihit2=0;ihit2<nhits;ihit2++) {
	      THcDCHit* hit2 = sp->GetHit(ihit2);
	      if(hit2->GetPlaneIndex()-pindex1 == 1 && TMath::Abs(hit2->GetPos()-hit1->GetPos())<0.51) { // Adjacent plane
		if(hit2->GetPos() <= hit1->GetPos() ) {
		  plusminusknown[ihit1] = -1;
		  plusminusknown[ihit2] = 1;
		} else {
		  plusminusknown[ihit1] = 1;
		  plusminusknown[ihit2] = -1;
		}
		npaired+=2;
	      }
	    }
	  }
	}
	nplusminus = 1 << (nhits-npaired);
      }//end if fSmallAngleApprox!=0
    }//end else SOS style
    if(nhits < 2) {
      if (fdebugstubchisq) cout << "THcDriftChamber::LeftRight: numhits-2 < 0" << endl;
    } else if (nhits == 2) {
      if (fdebugstubchisq) cout << "THcDriftChamber::LeftRight: numhits-2 = 0" << endl;
    }
    Int_t nplaneshit = Count1Bits(bitpat);
    //if (fhdebugflagpr) cout << " num of pm = " << nplusminus << " num of hits =" << nhits << endl;
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
      if ( (nplaneshit >= fNPlanes-1) || (nplaneshit >= fNPlanes-2 && !fHMSStyleChambers)) {
	Double_t chi2;
	chi2 = FindStub(nhits, sp,plane_list, bitpat, plusminus, stub);
	if (fdebugstubchisq) cout << " pmloop = " << pmloop << " chi2 = " << chi2 << endl;
	if(chi2 < minchi2) {
	  if (fStubMaxXPDiff<100. ) {
	    // Take best chi2 IF x' of the stub agrees with x' as expected from x.
	    // Sometimes an incorrect x' gives a good chi2 for the stub, even though it is
	    // not the correct left/right combination for the real track.
	    // Rotate x'(=stub(3)) to hut coordinates and compare to x' expected from x.
	    // THIS ASSUMES STANDARD HMS TUNE!!!!, for which x' is approx. x/875.
	    if(stub[2]*fTanBeta[plane_list[0]]==-1.0) {
	      if (fhdebugflagpr) cout << "THcDriftChamber::LeftRight() Error 3" << endl;
	    }
	    Double_t xp_fit=stub[2]-fTanBeta[plane_list[0]]
	      /(1+stub[2]*fTanBeta[plane_list[0]]);
	    Double_t xp_expect = sp->GetX()*fRatio_xpfp_to_xfp;
	    if(TMath::Abs(xp_fit-xp_expect)<fStubMaxXPDiff) {
	      minchi2 = chi2;
	      for(Int_t ihit=0;ihit<nhits;ihit++) {
		plusminusbest[ihit] = plusminus[ihit];
	      }
              sp->SetStub(stub);
	    } else {		// Record best stub failing angle cut
              if (chi2 < tmp_minchi2) {
		tmp_minchi2 = chi2;
		for(Int_t ihit=0;ihit<nhits;ihit++) {
		  tmp_plusminus[ihit] = plusminus[ihit];
		}
		for(Int_t i=0;i<4;i++) {
		  tmp_stub[i] = stub[i];
		}
	      }
	    }
	  } else { // Not HMS specific
	    minchi2 = chi2;
	    for(Int_t ihit=0;ihit<nhits;ihit++) {
	      plusminusbest[ihit] = plusminus[ihit];
	    }
            sp->SetStub(stub);
	  }
	}
	///////////////
      } else if (nplaneshit >= fNPlanes-2 && fHMSStyleChambers) { // Two planes missing
	Double_t chi2 = FindStub(nhits, sp,plane_list, bitpat, plusminus, stub);
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
          sp->SetStub(stub);
	}
      } else {
	if (fhdebugflagpr) cout << "Insufficient planes hit in THcDriftChamber::LeftRight()" << bitpat <<endl;
      }
    } // End loop of pm combinations

    if (minchi2 == maxchi2 && tmp_minchi2 == maxchi2) {
 
    } else {
      if(minchi2 == maxchi2 ) {	// No track passed angle cut
	minchi2 = tmp_minchi2;
	for(Int_t ihit=0;ihit<nhits;ihit++) {
	  plusminusbest[ihit] = tmp_plusminus[ihit];
	}
	sp->SetStub(tmp_stub);
      }
      Double_t *spstub = sp->GetStubP();

      // Calculate final coordinate based on plusminusbest
      // Update the hit positions in the space points
      for(Int_t ihit=0; ihit<nhits; ihit++) {
	// Save left/right status with the hit and in the spaleftce point
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
      sp->SetStub(stub);
      //if (fhdebugflagpr) cout << " Left/Right space pt " << isp+1 << " " << stub[0]<< " " << stub[1] << " " << stub[2]<< " " << stub[3] << endl;
    }
  }
  // Option to print stubs
}
//_____________________________________________________________________________
Double_t THcDriftChamber::FindStub(Int_t nhits, THcSpacePoint *sp,
				       Int_t* plane_list, UInt_t bitpat,
				       Int_t* plusminus, Double_t* stub)
{
  // For a given combination of L/R, fit a stub to the space point
  // This method does a linear least squares fit of a line to the
  // hits in an individual chamber.  It assumes that the y slope is 0
  // The wire coordinate is calculated by
  //          wire center + plusminus*(drift distance).
  // Method is called in a loop over all combinations of plusminus
  Double_t zeros[] = {0.0,0.0,0.0};
  TVectorD TT; TT.Use(3, zeros); // X, X', Y
  Double_t dpos[nhits];
  for(Int_t ihit=0;ihit<nhits; ihit++) {
    dpos[ihit] = sp->GetHit(ihit)->GetPos()
      + plusminus[ihit]*sp->GetHitDist(ihit)
      - fPsi0[plane_list[ihit]];
    for(Int_t index=0;index<3;index++) {
      TT[index]+= dpos[ihit]*fStubCoefs[plane_list[ihit]][index]
	/fSigma[plane_list[ihit]];
    }
  }
  //  fAA3Inv[bitpat].Print();
  //  if (fhdebugflagpr) cout << TT[0] << " " << TT[1] << " " << TT[2] << endl;
  //  TT.Print();

  TT *= fAA3Inv[bitpat];
  // if (fhdebugflagpr) cout << TT[0] << " " << TT[1] << " " << TT[2] << endl;

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
  delete [] fCosBeta; fCosBeta = NULL;
  delete [] fSinBeta; fSinBeta = NULL;
  delete [] fTanBeta; fTanBeta = NULL;
  delete [] fSigma; fSigma = NULL;
  delete [] fPsi0; fPsi0 = NULL;
  delete [] fStubCoefs; fStubCoefs = NULL;

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
