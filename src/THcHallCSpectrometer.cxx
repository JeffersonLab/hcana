/**
   \class THcHallCSpectrometer
   \ingroup Apparatuses

   \brief A standard Hall C spectrometer apparatus
    
   The usual name of this object is either "H", "P", "S"
   for HMS, suPerHMS, or SOS respectively

   Defines the functions FindVertices() and TrackCalc(), which are common
   to the HMS, SHMS and SOS.

   FindVertices() transports all the tracks in focal plane transport coordinates to
   the target using the method CalculateTargetQuantities() which uses the standard
   optics polynomials.  The tracks are then rotated to the LAB coordinate system
   where +Z points to beam dump, +Y is up, and +X is beam left.  The Golden track
   is then selected using one of the methods BestTrackSimple(),
   BestTrackUsingPrune(), BestTrackUsingScin(), depending on parameter settings.

   \author S. A. Wood

   \fn THcHallCSpectrometer::ReadDatabase( const TDatime& date )
   \brief Loads parameters to characterize a Hall C spectrometer.

   \fn THcHallCSpectrometer::EnforcePruneLimits()
   \brief Enforce minimum values for the prune cuts

   \fn THcHallCSpectrometer::FindVertices( TClonesArray& tracks )
   \brief Reconstruct target coordinates.

   \fn THcHallCSpectrometer::CalculateTargetQuantities(THaTrack* track,Double_t& xtar,Double_t&  xptar,Double_t& ytar,Double_t& yptar,Double_t& delta)
   \brief Transport focal plane track to target.

   \fn THcHallCSpectrometer::BestTrackSimple()
   \brief Choose best track based on Chisq.

   \fn THcHallCSpectrometer::BestTrackUsingScin()
   \brief Choose best track using closeness to scintillator hits.

   \fn THcHallCSpectrometer::BestTrackUsingPrune()
   \brief Choose best track after pruning.

*/

/*
  THESE comments need to be reviewed.

 th_geo is rotation about the Y axis in XZ plane to coordinates X',Y=Y',Z'
 After th_geo rotation, ph_geo rotation is about the X' axis in the  Y'Z' plane.


 Calls THaAnalysisObject::GeoToSph to calculate the spherical angles. th_sph and ph_sph
 th_sph is rotation about the Y axis in XZ plane to coordinates X',Y=Y',Z
 After th_sph rotation, ph_sph rotation is about the original Z axis
 In Lab coordinate system:

      X = r*sin(th_geo)*cos(ph_geo)         X = r*sin(th_sph)*cos(ph_sph)
      Y = r*sin(ph_geo)                     Y = r*sin(th_sph)*sin(ph_sph)
      Z = r*cos(th_geo)*cos(ph_geo)         Z = r*cos(th_sph)

      cos(th_sph) = cos(th_geo)*cos(ph_geo)
      cos(ph_sph) = sin(th_geo)*cos(ph_geo)/sqrt(1-cos^2(th_geo)*cos^2(ph_geo))

 GeoToSph is coded so that 
 1. negative th_geo and ph_geo = 0 returns th_sph=abs(th_geo) and ph_sph =180
 2. positive th_geo and ph_geo = 0 returns th_sph=th_geo and ph_sph =0

 Using the spherical angles, the TRotation fToLabRot and inverse fToTraRot are calculated
 fToLabRot is rotation matrix from the spectrometer TRANSPORT system to Lab system
 TRANSPORT coordinates are +X_tra points vertically down, +Z_tra is along the central ray and +Y_tra = ZxX

      For ph_sph = 0    X_lab =  Y_tra*cos(th_sph) + Z_tra*sin(th_sph)
                        Y_lab = -X_tra
                        Z_lab = -Y_tra*sin(th_sph) + Z_tra*cos(th_sph)
      For ph_sph = 180  X_lab =  Y_tra*cos(th_sph) - Z_tra*sin(th_sph)
                        Y_lab = -X_tra
                        Z_lab =  Y_tra*sin(th_sph) + Z_tra*cos(th_sph)


*/
//////////////////////////////////////////////////////////////////////////

#include "THcHallCSpectrometer.h"
#include "THaTrackingDetector.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THaTrack.h"
#include "THaTrackProj.h"
#include "THaTriggerTime.h"
#include "TMath.h"
#include "TList.h"

#include "THcRawShowerHit.h"
#include "THcSignalHit.h"
#include "THcShower.h"
#include "THcHitList.h"
#include "THcHodoscope.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using std::vector;
using namespace std;

//_____________________________________________________________________________
THcHallCSpectrometer::THcHallCSpectrometer( const char* name, const char* description ) :
  THaSpectrometer( name, description ), fPresent(kTRUE)
{
  // Constructor. Defines the standard detectors for the HRS.
  //  AddDetector( new THaTriggerTime("trg","Trigger-based time offset"));

  //sc_ref = static_cast<THaScintillator*>(GetDetector("s1"));

  SetTrSorting(kTRUE);
  eventtypes.clear();
}

//_____________________________________________________________________________
THcHallCSpectrometer::~THcHallCSpectrometer()
{
  // Destructor

  DefineVariables( kDelete );
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::DefineVariables( EMode mode )
{
  // Define/delete standard variables for a spectrometer (tracks etc.)
  // Can be overridden or extended by derived (actual) apparatuses
  if( mode == kDefine && fIsSetup ) return kOK;
  THaSpectrometer::DefineVariables( mode );
  fIsSetup = ( mode == kDefine );
  RVarDef vars[] = {
    { "tr.betachisq", "Chi2 of beta", "fTracks.THaTrack.GetBetaChi2()"},
    { "tr.PruneSelect", "Prune Select ID", "fPruneSelect"},
    { "present", "Trigger Type includes this spectrometer", "fPresent"},
    { 0 }
  };


  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
Bool_t THcHallCSpectrometer::SetTrSorting( Bool_t set )
{
  if( set )
    fProperties |= kSortTracks;
  else
    fProperties &= ~kSortTracks;

  return set;
}

//_____________________________________________________________________________
Bool_t THcHallCSpectrometer::GetTrSorting() const
{
  return ((fProperties & kSortTracks) != 0);
}

//_____________________________________________________________________________
void THcHallCSpectrometer::InitializeReconstruction()
{
  fNReconTerms = 0;
  fReconTerms.clear();
  fAngSlope_x = 0.0;
  fAngSlope_y = 0.0;
  fAngOffset_x = 0.0;
  fAngOffset_y = 0.0;
  fDetOffset_x = 0.0;
  fDetOffset_y = 0.0;
  fZTrueFocus = 0.0;
}
//_____________________________________________________________________________
Int_t THcHallCSpectrometer::ReadDatabase( const TDatime& date )
{
  /**
     Loads parameters including the
     angle settings of the spectrometer, various offsets, and the name
     of the file containing the hut to target transformation polynomials.
     Also loads parameters to control best track selection.

     Reads the hut to target transformation polynomial coefficients for
     use by CalculateTargetQuantities().

     Calls SetCentralAngles(th_geo,ph_geo,bend_down)
     where bend_down is a flag that should equal kFALSE for Hall C
     spectrometers.  th_geo and ph_geo are in degrees.
  */

  static const char* const here = "THcHallCSpectrometer::ReadDatabase";

#ifdef WITH_DEBUG
  cout << "In THcHallCSpectrometer::ReadDatabase()" << endl;
#endif

  const char* detector_name = "hod";
  //THaApparatus* app = GetDetector();
  THaDetector* det = GetDetector("hod");
  // THaDetector* det = app->GetDetector( shower_detector_name );

  if( dynamic_cast<THcHodoscope*>(det) ) {
    fHodo = static_cast<THcHodoscope*>(det);     // fHodo is a membervariable
  } else {
    Error("THcHallCSpectrometer", "Cannot find hodoscope detector %s",
    	  detector_name );
    fHodo = NULL;
  }


  THaDetector* detdc = GetDetector("dc");
  if( dynamic_cast<THcDC*>(detdc) ) {
    fDC = static_cast<THcDC*>(detdc);     // fHodo is a membervariable
  } else {
    Error("THcHallCSpectrometer", "Cannot find detector DC");
    fDC = NULL;
  }


  // Get the matrix element filename from the variable store
  // Read in the matrix

  InitializeReconstruction();

  char prefix[2];

#ifdef WITH_DEBUG
  cout << " GetName() " << GetName() << endl;
#endif
  
  prefix[0]=tolower(GetName()[0]);
  prefix[1]='\0';

  string reconCoeffFilename;
  DBRequest list[]={
    {"_recon_coeff_filename", &reconCoeffFilename,     kString               },
    {"theta_offset",          &fThetaOffset,           kDouble               },
    {"phi_offset",            &fPhiOffset,             kDouble               },
    {"delta_offset",          &fDeltaOffset,           kDouble               },
    {"thetacentral_offset",   &fThetaCentralOffset,    kDouble               },
    {"_oopcentral_offset",    &fOopCentralOffset,      kDouble               },
    {"pcentral_offset",       &fPCentralOffset,        kDouble               },
    {"pcentral",              &fPcentral,              kDouble               },
    {"satcorr",               &fSatCorr,               kDouble,         0,  1},
    {"theta_lab",             &fTheta_lab,             kDouble               },
    {"partmass",              &fPartMass,              kDouble               },
    {"phi_lab",               &fPhi_lab,               kDouble,         0,  1},
    {"mispointing_x",               &fMispointing_x,               kDouble,         0,  1},
    {"mispointing_y",               &fMispointing_y,               kDouble,         0,  1},
    {"sel_using_scin",        &fSelUsingScin,          kInt,            0,  1},
    {"sel_using_prune",       &fSelUsingPrune,         kInt,            0,  1},
    {"sel_ndegreesmin",       &fSelNDegreesMin,        kDouble,         0,  1},
    {"sel_dedx1min",          &fSeldEdX1Min,           kDouble,         0,  1},
    {"sel_dedx1max",          &fSeldEdX1Max,           kDouble,         0,  1},
    {"sel_betamin",           &fSelBetaMin,            kDouble,         0,  1},
    {"sel_betamax",           &fSelBetaMax,            kDouble,         0,  1},
    {"sel_etmin",             &fSelEtMin,              kDouble,         0,  1},
    {"sel_etmax",             &fSelEtMax,              kDouble,         0,  1},
    {"hodo_num_planes",       &fNPlanes,               kInt                  },
    {"scin_2x_zpos",          &fScin2XZpos,            kDouble,         0,  1},
    {"scin_2x_dzpos",         &fScin2XdZpos,           kDouble,         0,  1},
    {"scin_2y_zpos",          &fScin2YZpos,            kDouble,         0,  1},
    {"scin_2y_dzpos",         &fScin2YdZpos,           kDouble,         0,  1},
    {"prune_xp",              &fPruneXp,               kDouble,         0,  1},
    {"prune_yp",              &fPruneYp,               kDouble,         0,  1},
    {"prune_ytar",            &fPruneYtar,             kDouble,         0,  1},
    {"prune_delta",           &fPruneDelta,            kDouble,         0,  1},
    {"prune_beta",            &fPruneBeta,             kDouble,         0,  1},
    {"prune_df",              &fPruneDf,               kDouble,         0,  1},
    {"prune_chibeta",         &fPruneChiBeta,          kDouble,         0,  1},
    {"prune_npmt",            &fPruneNPMT,           kDouble,         0,  1},
    {"prune_fptime",          &fPruneFpTime,             kDouble,         0,  1},
    {0}
  };

  // Default values
  fSelUsingScin = 0;
  fSelUsingPrune = 0;
  fPruneXp = .2;
  fPruneYp = .2;
  fPruneYtar = 20.;
  fPruneDelta = 30.;
  fPruneBeta = 30.;
  fPruneDf= 1;
  fPruneChiBeta= 100.;
  fPruneNPMT= 6;
  fPruneFpTime= 1000.;
  fPhi_lab = 0.;
  fSatCorr=0.;
  fMispointing_x=999.;
  fMispointing_y=999.;
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
 
  //  mispointing in transport system y is horizontal and +x is vertical down
  if (fMispointing_y == 999.) {
    
    if (prefix[0]=='h') {
      
      if (TMath::Abs(fTheta_lab) < 40) {fMispointing_y = 0.1*(0.52-0.012*TMath::Abs(fTheta_lab)+0.002*TMath::Abs(fTheta_lab)*TMath::Abs(fTheta_lab));}
      else {fMispointing_y = 0.1*(0.52-0.012*40.+0.002*40.*40.);}
      
      gHcParms->Define("hmispointing_y","HMS Y-Mispointing", fMispointing_y);
    }
    
    if (prefix[0]=='p') {
      
      fMispointing_y = 0.1*(-0.6);
      gHcParms->Define("pmispointing_y","SHMS Y-Mispointing", fMispointing_y);
      cout << prefix[0] << " From Formula Mispointing_y = " << fMispointing_y << endl;
    }
    
  }
  
  else {
    cout << prefix[0] << " From Parameter Set Mispointing_y = " << fMispointing_y << endl;
  }
  
  
  if (fMispointing_x == 999.) {
    
    if (prefix[0]=='h')  {
         
      
      if (TMath::Abs(fTheta_lab) < 50) {fMispointing_x = 0.1*(2.37-0.086*TMath::Abs(fTheta_lab)+0.0012*TMath::Abs(fTheta_lab)*TMath::Abs(fTheta_lab));}
      else {fMispointing_x = 0.1*(2.37-0.086*50+0.0012*50.*50.);}
      
      gHcParms->Define("hmispointing_x","HMS X-Mispointing", fMispointing_x);
      cout << prefix[0] << " From Formula Mispointing_x = " << fMispointing_x << endl;
      
    }
    
    if (prefix[0]=='p') {
      
      fMispointing_x = 0.1*(-1.26);
      
      gHcParms->Define("pmispointing_x","SHMS X-Mispointing", fMispointing_x);
      cout << prefix[0] << " From Formula Mispointing_x = " << fMispointing_x << endl;
      
    }
    
  }
  
  else {
    cout << prefix[0] << " From Parameter Set Mispointing_x = " << fMispointing_x << endl;
  }
  //
  
  
  //EnforcePruneLimits();

#ifdef WITH_DEBUG
  cout <<  "\n\n\nhodo planes = " <<  fNPlanes << endl;
  cout <<  "sel using scin = "    <<  fSelUsingScin << endl;
  cout <<  "fPruneXp = "          <<  fPruneXp << endl;
  cout <<  "fPruneYp = "          <<  fPruneYp << endl;
  cout <<  "fPruneYtar = "        <<  fPruneYtar << endl;
  cout <<  "fPruneDelta = "       <<  fPruneDelta << endl;
  cout <<  "fPruneBeta = "        <<  fPruneBeta << endl;
  cout <<  "fPruneDf = "          <<  fPruneDf << endl;
  cout <<  "fPruneChiBeta = "     <<  fPruneChiBeta << endl;
  cout <<  "fPruneFpTime = "      <<  fPruneFpTime << endl;
  cout <<  "fPruneNPMT = "        <<  fPruneNPMT << endl;
  cout <<  "sel using prune = "   <<  fSelUsingPrune << endl;
#endif
  cout <<  "fPartMass = "         <<  fPartMass << endl;
  cout <<  "fPcentral = "         <<  fPcentral << " " <<fPCentralOffset << endl;
  cout <<  "fThetalab = "         <<  fTheta_lab << " " <<fThetaCentralOffset << endl;
  fPcentral= fPcentral*(1.+fPCentralOffset/100.);
  // Check that these offsets are in radians
  fTheta_lab=fTheta_lab + fThetaCentralOffset*TMath::RadToDeg();
  Double_t ph = fPhi_lab+fPhiOffset*TMath::RadToDeg();
  // SetCentralAngles method in podd THaSpectrometer
  // fTheta_lab and ph are geographical angles, converts to spherical coordinates
  // Need to set fTheta_lab to negative for spectrometer like HMS on beam right
  //   This gives phi_sph = 180 th_sph=abs(th_geo)
  // Computes TRotation fToLabRot and fToTraRot
  Bool_t bend_down = kFALSE;
  SetCentralAngles(fTheta_lab, ph, bend_down);
  Double_t off_z = 0.0;
  fPointingOffset.SetXYZ( fMispointing_x, fMispointing_y, off_z );
  //
  ifstream ifile;
  ifile.open(reconCoeffFilename.c_str());
  if(!ifile.is_open()) {
    Error(here, "error opening reconstruction coefficient file %s",reconCoeffFilename.c_str());
    //    return kInitError; // Is this the right return code?
    return kOK;
  }

  string line="!";
  int good=1;
  while(good && line[0]=='!') {
    good = getline(ifile,line).good();
    //    cout << line << endl;
  }
  // Read in focal plane rotation coefficients
  // Probably not used, so for now, just paste in fortran code as a comment
  while(good && line.compare(0,4," ---")!=0) {
    //  if(line(1:13).eq.'h_ang_slope_x')read(line,1201,err=94)h_ang_slope_x
    //  if(line(1:13).eq.'h_ang_slope_y')read(line,1201,err=94)h_ang_slope_y
    //  if(line(1:14).eq.'h_ang_offset_x')read(line,1201,err=94)h_ang_offset_x
    //  if(line(1:14).eq.'h_ang_offset_y')read(line,1201,err=94)h_ang_offset_y
    //  if(line(1:14).eq.'h_det_offset_x')read(line,1201,err=94)h_det_offset_x
    //  if(line(1:14).eq.'h_det_offset_y')read(line,1201,err=94)h_det_offset_y
    //  if(line(1:14).eq.'h_z_true_focus')read(line,1201,err=94)h_z_true_focus
    good = getline(ifile,line).good();
  }
  // Read in reconstruction coefficients and exponents
  line=" ";
  good = getline(ifile,line).good();
  //  cout << line << endl;
  fNReconTerms = 0;
  fReconTerms.clear();
  fReconTerms.reserve(500);
  //cout << "Reading matrix elements" << endl;
  while(good && line.compare(0,4," ---")!=0) {
    fReconTerms.push_back(reconTerm());
    sscanf(line.c_str()," %le %le %le %le %1d%1d%1d%1d%1d"
	   ,&fReconTerms[fNReconTerms].Coeff[0],&fReconTerms[fNReconTerms].Coeff[1]
	   ,&fReconTerms[fNReconTerms].Coeff[2],&fReconTerms[fNReconTerms].Coeff[3]
	   ,&fReconTerms[fNReconTerms].Exp[0]
	   ,&fReconTerms[fNReconTerms].Exp[1]
	   ,&fReconTerms[fNReconTerms].Exp[2]
	   ,&fReconTerms[fNReconTerms].Exp[3]
	   ,&fReconTerms[fNReconTerms].Exp[4]);
    fNReconTerms++;
    good = getline(ifile,line).good();
  }
  cout << "Read " << fNReconTerms << " matrix element terms"  << endl;
  if(!good) {
    Error(here, "Error processing reconstruction coefficient file %s",reconCoeffFilename.c_str());
    return kInitError; // Is this the right return code?
  }
  return kOK;
}

//_____________________________________________________________________________
void THcHallCSpectrometer::EnforcePruneLimits()
{

  fPruneXp      = TMath::Max( 0.08, fPruneXp);
  fPruneYp      = TMath::Max( 0.04, fPruneYp);
  fPruneYtar    = TMath::Max( 4.0,  fPruneYtar);
  fPruneDelta   = TMath::Max( 13.0, fPruneDelta);
  fPruneBeta    = TMath::Max( 0.1,  fPruneBeta);
  fPruneDf      = TMath::Max( 1.0,  fPruneDf);
  fPruneChiBeta = TMath::Max( 2.0,  fPruneChiBeta);
  fPruneFpTime  = TMath::Max( 5.0,  fPruneFpTime);
  fPruneNPMT    = TMath::Max( 6.0,  fPruneNPMT);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::FindVertices( TClonesArray& tracks )
{
  /** 
      Reconstruct target coordinates for all tracks found in the focal plane.

      In Hall A, this is passed off to the tracking detectors.
  
      In Hall C, we do the target traceback here since the traceback should
      not depend on which tracking detectors are used.

      Select the best track.

  */

  fNtracks = tracks.GetLast()+1;

  for (Int_t it=0;it<tracks.GetLast()+1;it++) {
    THaTrack* track = static_cast<THaTrack*>( tracks[it] );
    Double_t xptar=kBig,yptar=kBig,ytar=kBig,delta=kBig;
    Double_t xtar=0;
    CalculateTargetQuantities(track,xtar,xptar,ytar,yptar,delta); 
    // Transfer results to track
    // No beam raster yet
    //; In transport coordinates phi = hyptar = dy/dz and theta = hxptar = dx/dz
    //;    but for unknown reasons the yp offset is named  htheta_offset
    //;    and  the xp offset is named  hphi_offset

    track->SetTarget(0.0, ytar*100.0, xptar, yptar);
    track->SetDp(delta*100.0);	// Percent.  
    Double_t ptemp = fPcentral*(1+track->GetDp()/100.0);
    track->SetMomentum(ptemp);
    TVector3 pvect_temp;
    TransportToLab(track->GetP(),track->GetTTheta(),track->GetTPhi(),pvect_temp);
    track->SetPvect(pvect_temp);
  }
  fPruneSelect=-1.;
  if (fHodo==0 || (( fSelUsingScin == 0 ) && ( fSelUsingPrune == 0 )) ) {
    BestTrackSimple();
  } else if (fHodo!=0 && fSelUsingPrune !=0) {
    BestTrackUsingPrune();
  } else if (fHodo!=0){
    BestTrackUsingScin();
  }


  return 0;
}
//
void THcHallCSpectrometer::CalculateTargetQuantities(THaTrack* track,Double_t& xtar,Double_t&  xptar,Double_t& ytar,Double_t& yptar,Double_t& delta) 
{
  /**
     Transport a track in the the focal plane coordinate system to the
     target using the reconstruction matrix elements.

     If Xsatcorr is 2000, apply a specific correction to delta for
     saturation effects.
  */

  Double_t hut[5];
  Double_t hut_rot[5];

  hut[0] = track->GetX()/100.0 + fZTrueFocus*track->GetTheta() + fDetOffset_x;//m
  hut[1] = track->GetTheta() + fAngOffset_x;//radians
  hut[2] = track->GetY()/100.0 + fZTrueFocus*track->GetPhi() + fDetOffset_y;//m
  hut[3] = track->GetPhi() + fAngOffset_y;//radians

  hut[4] = xtar/100.0;

  // Retrieve the focal plane coordnates
  // Do the transformation
  // Stuff results into track
  hut_rot[0] = hut[0];
  hut_rot[1] = hut[1] + hut[0]*fAngSlope_x;
  hut_rot[2] = hut[2];
  hut_rot[3] = hut[3] + hut[2]*fAngSlope_y;
  hut_rot[4] = hut[4];

  // Compute COSY sums
  Double_t sum[4];
  for(Int_t k=0;k<4;k++) {
    sum[k] = 0.0;
  }
  for(Int_t iterm=0;iterm<fNReconTerms;iterm++) {
    Double_t term=1.0;
    for(Int_t j=0;j<5;j++) {
      if(fReconTerms[iterm].Exp[j]!=0) {
	term *= pow(hut_rot[j],fReconTerms[iterm].Exp[j]);
      }
    }
    for(Int_t k=0;k<4;k++) {
      sum[k] += term*fReconTerms[iterm].Coeff[k];
    }
  }
  xptar=sum[0] + fPhiOffset;
  ytar=sum[1];
  yptar=sum[2] + fThetaOffset;
  delta=sum[3] + fDeltaOffset;
  if (fSatCorr == 2000) {
    Double_t p0corr = 0.82825*fPcentral-1.223  ;    
    delta = delta + p0corr*xptar/100.;
  }
}
//
//_____________________________________________________________________________
Int_t THcHallCSpectrometer::TrackCalc()
{
  if( fNtracks > 0 ) {
    Int_t hit_gold_track=0; // find track with index =0 which is best track
    for (Int_t itrack = 0; itrack < fNtracks; itrack++ ){
      THaTrack* aTrack = static_cast<THaTrack*>( fTracks->At(itrack) );
      if (aTrack->GetIndex()==0) hit_gold_track=itrack;  
    }
    fDC->SetFocalPlaneBestTrack(hit_gold_track);
    fGoldenTrack = static_cast<THaTrack*>( fTracks->At(hit_gold_track) );
    fTrkIfo      = *fGoldenTrack;
    fTrk         = fGoldenTrack;
  } else {
    fGoldenTrack = NULL;
  }

  return TrackTimes( fTracks );
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackSimple()
{
  /**
     Choose the track with the lowest tracking fitting Chisq to be
     the "best track"
  */

  if( GetTrSorting() )   fTracks->Sort();

  // Assign index=0 to the best track, 
  for (Int_t itrack = 0; itrack < fNtracks; itrack++ ){
    THaTrack* aTrack = static_cast<THaTrack*>( fTracks->At(itrack) );
    aTrack->SetIndex(1);  
    if (itrack==0) aTrack->SetIndex(0);  
  }

  return(0);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackUsingScin()
{
  /**
     Select as best track the track closest to a S2Y hit.  If there
     are no S2Y hits, select the track closest to a S2X hit.

     Also reject tracks if they fail dEdx, beta, or calorimeter energy cuts.
  */

  Double_t chi2Min;

  if( fNtracks > 0 ) {
    fGoodTrack = -1;
    chi2Min = 10000000000.0;
    Int_t y2Dmin = 100;
    Int_t x2Dmin = 100;

    Bool_t* x2Hits        = new Bool_t [fHodo->GetNPaddles(2)];
    Bool_t* y2Hits        = new Bool_t [fHodo->GetNPaddles(3)];
    for (UInt_t j = 0; j < fHodo->GetNPaddles(2); j++ ){
      x2Hits[j] = kFALSE;
    }
    for (UInt_t j = 0; j < fHodo->GetNPaddles(3); j++ ){
      y2Hits[j] = kFALSE;
    }

    for (Int_t rawindex=0; rawindex<fHodo->GetTotHits(); rawindex++) {
      Int_t ip = fHodo->GetGoodRawPlane(rawindex);
      if(ip==2) {	// X2
	Int_t goodRawPad = fHodo->GetGoodRawPad(rawindex);
	x2Hits[goodRawPad] = kTRUE;
      } else if (ip==3) { // Y2
	Int_t goodRawPad = fHodo->GetGoodRawPad(rawindex);
	y2Hits[goodRawPad] = kTRUE;
      }
    }

    for (Int_t itrack = 0; itrack < fNtracks; itrack++ ){
      Double_t chi2PerDeg;

      THaTrack* aTrack = static_cast<THaTrack*>( fTracks->At(itrack) );
      if (!aTrack) {delete[] x2Hits; delete[] y2Hits; return -1;}

      if ( aTrack->GetNDoF() > fSelNDegreesMin ){
	chi2PerDeg =  aTrack->GetChi2() / aTrack->GetNDoF();

	if( ( aTrack->GetDedx()    > fSeldEdX1Min )   &&
	    ( aTrack->GetDedx()    < fSeldEdX1Max )   &&
	    ( aTrack->GetBeta()    > fSelBetaMin  )   &&
	    ( aTrack->GetBeta()    < fSelBetaMax  )   &&
	    ( aTrack->GetEnergy()  > fSelEtMin    )   &&
	    ( aTrack->GetEnergy()  < fSelEtMax    ) ) {
	  Int_t x2D, y2D;

	  if ( fNtracks > 1 ){
	    Double_t hitpos3  = aTrack->GetX() + aTrack->GetTheta() * ( fScin2XZpos + 0.5 * fScin2XdZpos );
	    Int_t icounter3  = TMath::Nint( ( hitpos3 - fHodo->GetPlaneCenter(2) ) / fHodo->GetPlaneSpacing(2) ) + 1;
	    Int_t hitCnt3  = TMath::Max( TMath::Min(icounter3, (Int_t) fHodo->GetNPaddles(2) ) , 1); // scin_2x_nr = 16
	    //	      fHitDist3 = fHitPos3 - ( fHodo->GetPlaneSpacing(2) * ( hitCnt3 - 1 ) + fHodo->GetPlaneCenter(2) );
	    Double_t hitpos4 = aTrack->GetY() + aTrack->GetPhi() * ( fScin2YZpos + 0.5 * fScin2YdZpos );
	    Int_t icounter4  = TMath::Nint( ( fHodo->GetPlaneCenter(3) - hitpos4 ) / fHodo->GetPlaneSpacing(3) ) + 1;
	    Int_t hitCnt4  = TMath::Max( TMath::Min(icounter4, (Int_t) fHodo->GetNPaddles(3) ) , 1); // scin_2y_nr = 10
	    //	      fHitDist4 = fHitPos4 - ( fHodo->GetPlaneCenter(3) - fHodo->GetPlaneSpacing(3) * ( hitCnt4 - 1 ) );
	    // Plane 3
	    Int_t mindiff=1000;
	    for (UInt_t i = 0; i <  fHodo->GetNPaddles(2); i++ ){
	      if ( x2Hits[i] ) {
		Int_t diff = TMath::Abs((Int_t)hitCnt3-(Int_t)i-1);
		if (diff < mindiff) mindiff = diff;
	      }
	    }
	    if(mindiff < 1000) {
	      x2D = mindiff;
	    } else {
	      x2D = 0;	// Is this what we really want if there were no hits on this plane?
	    }

	    // Plane 4
	    mindiff = 1000;
	    for (UInt_t i = 0; i < fHodo->GetNPaddles(3); i++ ){
	      if ( y2Hits[i] ) {
		Int_t diff = TMath::Abs((Int_t)hitCnt4-(Int_t)i-1);
		if (diff < mindiff) mindiff = diff;
	      }
	    }
	    if(mindiff < 1000) {
	      y2D = mindiff;
	    } else {
	      y2D = 0;	// Is this what we really want if there were no hits on this plane?
	    }
	  } else { // Only a single track
	    x2D = 0.;
	    y2D = 0.;
	  }

	  if ( y2D <= y2Dmin ) {
	    if ( y2D < y2Dmin ) {
	      x2Dmin = 100;
	      chi2Min = 10000000000.;
	    } // y2D min

	    if ( x2D <= x2Dmin ){
	      if ( x2D < x2Dmin ){
		chi2Min = 10000000000.0;
	      } // condition x2D
	      if ( chi2PerDeg < chi2Min ){

		fGoodTrack = itrack; // fGoodTrack = itrack
		y2Dmin = y2D;
		x2Dmin = x2D;
		chi2Min = chi2PerDeg;

		fGoldenTrack = static_cast<THaTrack*>( fTracks->At( fGoodTrack ) );
		fTrkIfo      = *fGoldenTrack;
		fTrk         = fGoldenTrack;
	      }
	    } // condition x2D
	  } // condition y2D
	} // conditions for dedx, beta and trac energy
      } // confition for fNFreeFP greater than fSelNDegreesMin
    } // loop over tracks

    // Fall back to using track with minimum chi2
    if ( fGoodTrack == -1 ){

      chi2Min = 10000000000.0;
      for (Int_t iitrack = 0; iitrack < fNtracks; iitrack++ ){
	Double_t chi2PerDeg;
	THaTrack* aTrack = dynamic_cast<THaTrack*>( fTracks->At(iitrack) );
	if (!aTrack) return -1;

	if ( aTrack->GetNDoF() > fSelNDegreesMin ){
	  chi2PerDeg =  aTrack->GetChi2() / aTrack->GetNDoF();

	  if ( chi2PerDeg < chi2Min ){

	    fGoodTrack = iitrack;
	    chi2Min = chi2PerDeg;

	  }
	}
      } // loop over trakcs
      // Set index for fGoodTrack = 0
      for (Int_t iitrack = 0; iitrack < fNtracks; iitrack++ ){
 	THaTrack* aTrack = dynamic_cast<THaTrack*>( fTracks->At(iitrack) );
        aTrack->SetIndex(1);
	if (iitrack==fGoodTrack) aTrack->SetIndex(0);
      }
      //
    }

  }

  return(0);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::BestTrackUsingPrune()
{
  /**
     Select as best track the track with the lowest Chisq after pruning
     tracks that don't meet various criteria such as xptar, yptar, ytar
     delta, beta, degrees of freedom (of track fit), difference between
     measured beta and beta from p, chisq of beta fit, focal plane time
     and number of PMT hit.
  */

  Int_t nGood;
  Double_t chi2Min;

  if ( fNtracks > 0 ) {
    chi2Min   = 10000000000.0;
    fGoodTrack = 0;
    vector<bool> keep(fNtracks);
    vector<Int_t> reject(fNtracks);

    THaTrack *testTracks[fNtracks];

    // ! Initialize all tracks to be good
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      keep[ptrack] = kTRUE;
      reject[ptrack] = 0;
      testTracks[ptrack] = static_cast<THaTrack*>( fTracks->At(ptrack) );
      if (!testTracks[ptrack]) return -1;
    }
    fPruneSelect = 0;
    Double_t PruneSelect=0;
    // ! Prune on xptar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetTTheta() ) < fPruneXp ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if ( nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTTheta() ) >= fPruneXp ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 1;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;
    // ! Prune on yptar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetTPhi() ) < fPruneYp ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTPhi() ) >= fPruneYp ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 2;

	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on ytar
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetTY() ) < fPruneYtar ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetTY() ) >= fPruneYtar ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 10;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on delta
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetDp() ) < fPruneDelta ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetDp() ) >= fPruneDelta ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 20;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on beta
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      Double_t p = testTracks[ptrack]->GetP();
      Double_t betaP = p / TMath::Sqrt( p * p + fPartMass * fPartMass );
      if ( ( TMath::Abs( testTracks[ptrack]->GetBeta() - betaP ) < fPruneBeta ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	Double_t p = testTracks[ptrack]->GetP();
	Double_t betaP = p / TMath::Sqrt( p * p + fPartMass * fPartMass );
	if ( TMath::Abs( testTracks[ptrack]->GetBeta() - betaP ) >= fPruneBeta ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 100;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on deg. freedom for track chisq
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( testTracks[ptrack]->GetNDoF() >= fPruneDf ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( testTracks[ptrack]->GetNDoF() < fPruneDf ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 200;
	}
      }
    }

    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;
    //!     Prune on num pmt hits
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( testTracks[ptrack]->GetNPMT() >= fPruneNPMT ) && ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( testTracks[ptrack]->GetNPMT() < fPruneNPMT ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 100000;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on beta chisqr
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( testTracks[ptrack]->GetBetaChi2() < fPruneChiBeta ) &&
	   ( testTracks[ptrack]->GetBetaChi2() > 0.01 )          &&   ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( ( testTracks[ptrack]->GetBetaChi2() >= fPruneChiBeta ) ||
	     ( testTracks[ptrack]->GetBetaChi2() <= 0.01          ) ){
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 1000;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on fptime
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( TMath::Abs( testTracks[ptrack]->GetFPTime() - fHodo->GetStartTimeCenter() ) < fPruneFpTime )  &&
	   ( keep[ptrack] ) ){
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( TMath::Abs( testTracks[ptrack]->GetFPTime() - fHodo->GetStartTimeCenter() ) >= fPruneFpTime ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 2000;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on Y2 being hit
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( testTracks[ptrack]->GetGoodPlane4() == 1 )  &&  keep[ptrack] ) {
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( testTracks[ptrack]->GetGoodPlane4() != 1 ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 10000;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Prune on X2 being hit
    nGood = 0;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
      if ( ( testTracks[ptrack]->GetGoodPlane3() == 1 )  &&  keep[ptrack] ) {
	nGood ++;
      }
    }
    if (nGood > 0 ) {
      for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){
	if ( testTracks[ptrack]->GetGoodPlane3() != 1 ) {
	  keep[ptrack] = kFALSE;
	  reject[ptrack] += 20000;
	}
      }
    }
    PruneSelect++;
    if (nGood==1 && fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;

    // !     Pick track with best chisq if more than one track passed prune tests
    Double_t chi2PerDeg = 0.;
    for (Int_t ptrack = 0; ptrack < fNtracks; ptrack++ ){

      chi2PerDeg =  testTracks[ptrack]->GetChi2() / testTracks[ptrack]->GetNDoF();

      if ( ( chi2PerDeg < chi2Min ) && ( keep[ptrack] ) ){
	fGoodTrack = ptrack;
	chi2Min = chi2PerDeg;
      }
    }
     PruneSelect++;
    if (fPruneSelect ==0 && fNtracks>1) fPruneSelect=PruneSelect;
   // Set index=0 for fGoodTrack 
    for (Int_t iitrack = 0; iitrack < fNtracks; iitrack++ ){
      THaTrack* aTrack = dynamic_cast<THaTrack*>( fTracks->At(iitrack) );
      aTrack->SetIndex(1);
      if (iitrack==fGoodTrack) aTrack->SetIndex(0);
    }
    //


  }

  return(0);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::TrackTimes( TClonesArray* Tracks ) {
  // Do the actual track-timing (beta) calculation.
  // Use multiple scintillators to average together and get "best" time at S1.
  //
  // To be useful, a meaningful timing resolution should be assigned
  // to each Scintillator object (part of the database).


  return 0;
}

Int_t THcHallCSpectrometer::Decode( const THaEvData& evdata )
{

  fPresent=kTRUE;
  if(eventtypes.size()!=0) {
    Int_t evtype = evdata.GetEvType();
    if(!IsMyEvent(evtype)) {
      fPresent = kFALSE;
    }
  }

  return THaSpectrometer::Decode(evdata);
}

//_____________________________________________________________________________
Int_t THcHallCSpectrometer::ReadRunDatabase( const TDatime& date )
{
  // Override THaSpectrometer with nop method.  All needed kinamatics
  // read in ReadDatabase.

  return kOK;
}

void THcHallCSpectrometer::AddEvtType(int evtype) {
  eventtypes.push_back(evtype);
}
  
void THcHallCSpectrometer::SetEvtType(int evtype) {
  eventtypes.clear();
  AddEvtType(evtype);
}

Bool_t THcHallCSpectrometer::IsMyEvent(Int_t evtype) const
{
  for (UInt_t i=0; i < eventtypes.size(); i++) {
    if (evtype == eventtypes[i]) return kTRUE;
  }

  return kFALSE;
}

//_____________________________________________________________________________
ClassImp(THcHallCSpectrometer)
