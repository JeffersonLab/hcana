///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcShower                                                                 //
//                                                                           //
// Shower counter class, describing a generic segmented shower detector.     //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcShower.h"
#include "THcShowerCluster.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "THaTrackProj.h"
#include "TMath.h"

#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//_____________________________________________________________________________
THcShower::THcShower( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor
  fNLayers = 0;			// No layers until we make them

  fClusterList = new THcShowerClusterList;
}

//_____________________________________________________________________________
THcShower::THcShower( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
void THcShower::Setup(const char* name, const char* description)
{
  if (fdbg_init_cal) 
    cout << "THcShower::Setup called " << GetName() << endl;

  char prefix[2];

  prefix[0] = tolower(GetApparatus()->GetName()[0]);
  prefix[1] = '\0';

  string layernamelist;
  DBRequest list[]={
    {"cal_num_layers", &fNLayers, kInt},
    {"cal_layer_names", &layernamelist, kString},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  if (fdbg_init_cal) {
    cout << layernamelist << endl;
    cout << "Shower Counter: " << fNLayers << " layers" << endl;
  }

  vector<string> layer_names = vsplit(layernamelist);

  if(layer_names.size() != (UInt_t) fNLayers) {
    cout << "THcShower::Setup ERROR: Number of layers " << fNLayers 
	 << " doesn't agree with number of layer names "
	 << layer_names.size() << endl;
    // Should quit.  Is there an official way to quit?
  }

  fLayerNames = new char* [fNLayers];
  for(Int_t i=0;i<fNLayers;i++) {
    fLayerNames[i] = new char[layer_names[i].length()+1];
    strcpy(fLayerNames[i], layer_names[i].c_str());
  }
  
  char *desc = new char[strlen(description)+100];
  fPlanes = new THcShowerPlane* [fNLayers];

  for(Int_t i=0;i < fNLayers;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fLayerNames[i]);

    fPlanes[i] = new THcShowerPlane(fLayerNames[i], desc, i+1, this); 

    if (fdbg_init_cal)
      cout << "Created Shower Plane " << fLayerNames[i] << ", " << desc << endl;
  }
  delete [] desc;

  if (fdbg_init_cal) 
    cout << "THcShower::Setup Return " << GetName() << endl;
}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShower::Init( const TDatime& date )
{
  if (fdbg_init_cal) 
    cout << "THcShower::Init " << GetName() << endl;

  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  InitHitList(fDetMap, "THcRawShowerHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  for(Int_t ip=0;ip<fNLayers;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }

  char EngineDID[] = " CAL";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);

  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", 
	     EngineDID);
      return kInitError;
  }
  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcShower::ReadDatabase( const TDatime& date )
{
  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";
  char prefix[2];

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  if (fdbg_init_cal) 
    cout << "THcShower::ReadDatabase called " << GetName() << endl;

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  {
    DBRequest list[]={
      {"cal_num_neg_columns", &fNegCols, kInt},
      {"cal_slop", &fSlop, kDouble},
      {"cal_fv_test", &fvTest, kInt,0,1},
      {"cal_fv_delta", &fvDelta, kDouble},
      {"dbg_raw_cal", &fdbg_raw_cal, kInt},
      {"dbg_decoded_cal", &fdbg_decoded_cal, kInt},
      {"dbg_sparsified_cal", &fdbg_sparsified_cal, kInt},
      {"dbg_clusters_cal", &fdbg_clusters_cal, kInt},
      {"dbg_tracks_cal", &fdbg_tracks_cal, kInt},
      {"debugflaggeometry", &fdbg_init_cal, kInt},
      {0}
    };
    fvTest = 0;			// Default if not defined
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  if (fdbg_init_cal) {
    cout << "Number of neg. columns      = " << fNegCols << endl;
    cout << "Slop parameter              = " << fSlop << endl;
    cout << "Fiducial volume test flag   = " << fvTest << endl;
    cout << "Fiducial volume excl. width = " << fvDelta << endl;
    cout << "Decode debug flag           = " << fdbg_decoded_cal  << endl;
    cout << "Sparsify debug flag         = " << fdbg_sparsified_cal  << endl;
    cout << "Cluster debug flag          = " << fdbg_clusters_cal  << endl;
    cout << "Tracking debug flag         = " << fdbg_tracks_cal  << endl;
  }

  {
    DBRequest list[]={
      {"cal_a_cor", &fAcor, kDouble},
      {"cal_b_cor", &fBcor, kDouble},
      {"cal_c_cor", &fCcor, kDouble},
      {"cal_d_cor", &fDcor, kDouble},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  if (fdbg_init_cal) {
    cout << "HMS Calorimeter coordinate correction constants:" << endl;
    cout << "    fAcor = " << fAcor << endl;
    cout << "    fBcor = " << fBcor << endl;
    cout << "    fCcor = " << fCcor << endl;
    cout << "    fDcor = " << fDcor << endl;
  }

  BlockThick = new Double_t [fNLayers];
  fNBlocks = new Int_t [fNLayers];
  fNLayerZPos = new Double_t [fNLayers];
  YPos = new Double_t [2*fNLayers];

  for(Int_t i=0;i<fNLayers;i++) {
    DBRequest list[]={
      {Form("cal_%s_thick",fLayerNames[i]), &BlockThick[i], kDouble},
      {Form("cal_%s_nr",fLayerNames[i]), &fNBlocks[i], kInt},
      {Form("cal_%s_zpos",fLayerNames[i]), &fNLayerZPos[i], kDouble},
      {Form("cal_%s_right",fLayerNames[i]), &YPos[2*i], kDouble},
      {Form("cal_%s_left",fLayerNames[i]), &YPos[2*i+1], kDouble},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  //Caution! Z positions (fronts) are off in hcal.param! Correct later on.

  XPos = new Double_t* [fNLayers];
  for(Int_t i=0;i<fNLayers;i++) {
    XPos[i] = new Double_t [fNBlocks[i]];
    DBRequest list[]={
      {Form("cal_%s_top",fLayerNames[i]),XPos[i], kDouble, fNBlocks[i]},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  if (fdbg_init_cal) {
    for(Int_t i=0;i<fNLayers;i++) {
      cout << "Plane " << fLayerNames[i] << ":" << endl;
      cout << "    Block thickness: " << BlockThick[i] << endl;
      cout << "    NBlocks        : " << fNBlocks[i] << endl;
      cout << "    Z Position     : " << fNLayerZPos[i] << endl;
      cout << "    Y Positions    : " << YPos[2*i] << ", " << YPos[2*i+1]
	   <<endl;
      cout << "    X Positions    :";
      for(Int_t j=0; j<fNBlocks[i]; j++) {
	cout << " " << XPos[i][j];
      }
      cout << endl;
    }
  }

  // Fiducial volume limits, based on Plane 1 positions.
  //

  fvXmin = XPos[0][0] + fvDelta;
  fvXmax = XPos[0][fNBlocks[0]-1] + BlockThick[0] - fvDelta;
  fvYmin = YPos[0] + fvDelta;
  fvYmax = YPos[1] - fvDelta;

  if (fdbg_init_cal) {
    cout << "Fiducial volume limits:" << endl;
    cout << "   Xmin = " << fvXmin << "  Xmax = " << fvXmax << endl;
    cout << "   Ymin = " << fvYmin << "  Ymax = " << fvYmax << endl;
  }

  //Calibration related parameters (from hcal.param).

  fNtotBlocks=0;              //total number of blocks
  for (Int_t i=0; i<fNLayers; i++) fNtotBlocks += fNBlocks[i];

  if (fdbg_init_cal) 
    cout << "Total number of blocks in the calorimeter: " << fNtotBlocks
	 << endl;

  //Pedestal limits from hcal.param.
  fShPosPedLimit = new Int_t [fNtotBlocks];
  fShNegPedLimit = new Int_t [fNtotBlocks];

  //Calibration constants
  fPosGain = new Double_t [fNtotBlocks];
  fNegGain = new Double_t [fNtotBlocks];

  //Read in parameters from hcal.param
  Double_t hcal_pos_cal_const[fNtotBlocks];
  //  Double_t hcal_pos_gain_ini[fNtotBlocks];     not used
  //  Double_t hcal_pos_gain_cur[fNtotBlocks];     not used
  //  Int_t    hcal_pos_ped_limit[fNtotBlocks];    not used
  Double_t hcal_pos_gain_cor[fNtotBlocks];

  Double_t hcal_neg_cal_const[fNtotBlocks];
  //  Double_t hcal_neg_gain_ini[fNtotBlocks];     not used
  //  Double_t hcal_neg_gain_cur[fNtotBlocks];     not used
  //  Int_t    hcal_neg_ped_limit[fNtotBlocks];    not used
  Double_t hcal_neg_gain_cor[fNtotBlocks];

  DBRequest list[]={
    {"cal_pos_cal_const", hcal_pos_cal_const, kDouble, fNtotBlocks},
    //    {"cal_pos_gain_ini",  hcal_pos_gain_ini,  kDouble, fNtotBlocks},
    //    {"cal_pos_gain_cur",  hcal_pos_gain_cur,  kDouble, fNtotBlocks},
    {"cal_pos_ped_limit", fShPosPedLimit, kInt,    fNtotBlocks},
    {"cal_pos_gain_cor",  hcal_pos_gain_cor,  kDouble, fNtotBlocks},
    {"cal_neg_cal_const", hcal_neg_cal_const, kDouble, fNtotBlocks},
    //    {"cal_neg_gain_ini",  hcal_neg_gain_ini,  kDouble, fNtotBlocks},
    //    {"cal_neg_gain_cur",  hcal_neg_gain_cur,  kDouble, fNtotBlocks},
    {"cal_neg_ped_limit", fShNegPedLimit, kInt,    fNtotBlocks},
    {"cal_neg_gain_cor",  hcal_neg_gain_cor,  kDouble, fNtotBlocks},
    {"cal_min_peds", &fShMinPeds, kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  if (fdbg_init_cal) {

    cout << "hcal_pos_cal_const:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_pos_cal_const[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "fShPosPedLimit:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << fShPosPedLimit[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "hcal_pos_gain_cor:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_pos_gain_cor[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "hcal_neg_cal_const:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_neg_cal_const[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "fShNegPedLimit:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << fShNegPedLimit[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "hcal_neg_gain_cor:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_neg_gain_cor[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

  }    // end of debug output

  // Calibration constants (GeV / ADC channel).

  for (Int_t i=0; i<fNtotBlocks; i++) {
    fPosGain[i] = hcal_pos_cal_const[i] *  hcal_pos_gain_cor[i];
    fNegGain[i] = hcal_neg_cal_const[i] *  hcal_neg_gain_cor[i];
  }

  if (fdbg_init_cal) {

    cout << "fPosGain:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << fPosGain[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "fNegGain:" << endl;
    for (Int_t j=0; j<fNLayers; j++) {
      for (Int_t i=0; i<fNBlocks[j]; i++) {
	cout << fNegGain[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

  }

  // Corrdinate corrected track energies per plane

  fTREpl_cor     = new Double_t [fNLayers];
  fTREpl_pos_cor = new Double_t [fNLayers];
  fTREpl_neg_cor = new Double_t [fNLayers];

  // Origin of the calorimeter, at the centre of the face of the detector,
  // or at the centre of the front of the 1-st layer.
  //
  Double_t xOrig = (XPos[0][0] + XPos[0][fNBlocks[0]-1])/2 + BlockThick[0]/2;
  Double_t yOrig = (YPos[0] + YPos[1])/2;
  Double_t zOrig = fNLayerZPos[0];

  fOrigin.SetXYZ(xOrig, yOrig, zOrig);

  if (fdbg_init_cal) {
    cout << "Origin of the Calorimeter:" << endl;
    cout << "       Xorig = " << GetOrigin().X() << endl;
    cout << "       Yorig = " << GetOrigin().Y() << endl;
    cout << "       Zorig = " << GetOrigin().Z() << endl;
    cout << endl;
  }

  // Detector axes. Assume no rotation.
  //
  DefineAxes(0.);

  fIsInit = true;

  return kOK;
}


//_____________________________________________________________________________
Int_t THcShower::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  if (fdbg_init_cal)
    cout << "THcShower::DefineVariables called " << GetName() << endl;

  // Register variables in global list

  RVarDef vars[] = {
    { "nhits", "Number of hits",                                 "fNhits" },
 //   { "a",      "Raw ADC amplitudes",                 "fA" },
 //   { "a_p",    "Ped-subtracted ADC amplitudes",      "fA_p" },
 //   { "a_c",    "Calibrated ADC amplitudes",          "fA_c" },
 //   { "asum_p", "Sum of ped-subtracted ADCs",         "fAsum_p" },
 //   { "asum_c", "Sum of calibrated ADCs",             "fAsum_c" },
    { "nclust", "Number of clusters",                            "fNclust" },
    { "ntracks", "Number of shower tracks",                      "fNtracks" },
    { "emax",   "Energy of largest cluster",                     "fE" },
    { "eprmax",   "Preshower Energy of largest cluster",         "fEpr" },
    { "xmax",      "x-position (cm) of largest cluster",         "fX" },
 //   { "z",      "z-position (cm) of largest cluster", "fZ" },
    { "mult",   "Multiplicity of largest cluster",               "fMult" },
 //   { "nblk",   "Numbers of blocks in main cluster",  "fNblk" },
 //   { "eblk",   "Energies of blocks in main cluster", "fEblk" },
    { "trx",      "track x-position in det plane (1st track)",   "fTRX" },
    { "try",      "track y-position in det plane (1st track)",   "fTRY" },
    { "tre",      "track energy in the calorimeter (1st track)", "fTRE" },
    { "trepr",    "track energy in the preshower (1st track)",   "fTREpr" },
    { "trecor",   "Y-corrected track Edep (1st track)",          "fTRE_cor" },
    { "treprcor", "Y-corrected track Epr (1st track)",           "fTREpr_cor" },
    { "treplcor", "Y-corrected track Edep for planes",           "fTREpl_cor" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
THcShower::~THcShower()
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
void THcShower::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  delete [] BlockThick;  BlockThick = NULL;
  delete [] fNBlocks;  fNBlocks = NULL;
  delete [] fNLayerZPos;  fNLayerZPos = NULL;
  delete [] XPos;  XPos = NULL;
  delete [] ZPos;  ZPos = NULL;
  //delete [] fSpacing;  fSpacing = NULL;
  //delete [] fCenter;   fCenter = NULL; // This 2D. What is correct way to delete?
}

//_____________________________________________________________________________
inline 
void THcShower::Clear(Option_t* opt)
{

//   Reset per-event data.

  for(Int_t ip=0;ip<fNLayers;ip++) {
    fPlanes[ip]->Clear(opt);
  }

 // fTrackProj->Clear();

  fNhits = 0;
  fNclust = 0;
  fNtracks = 0;
  fMult = 0;
  fE = 0.;
  fEpr = 0.;
  fX = -75.;    //out of acceptance
  fTRX = -75.;  //out of acceptance
  fTRY = -40.;  //out of acceptance
  fTRE = -0.;
  fTREpr = -0.;
  fTRE_cor = -0.;
  fTREpr_cor = -0.;
  for(Int_t ip=0;ip<fNLayers;ip++) {
    fTREpl_cor[ip] = -0.;
    fTREpl_pos_cor[ip] = -0.;
    fTREpl_neg_cor[ip] = -0.;
  }

  fClusterList->purge();

}

//_____________________________________________________________________________
Int_t THcShower::Decode( const THaEvData& evdata )
{

  Clear();

  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = DecodeToHitList(evdata);

  fEvent = evdata.GetEvNum();

  if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNLayers;ip++) {
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
      if (fdbg_decoded_cal) {
	cout << "THcShower::Decode: nexthit = " << nexthit << endl;
      }
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
    for(Int_t ip=0;ip<fNLayers;ip++) {
      fPlanes[ip]->CalculatePedestals();
    }
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t nexthit = 0;
  for(Int_t ip=0;ip<fNLayers;ip++) {
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
  }

  //   fRawHitList is TClones array of THcRawShowerHit objects

  // This debug output does not work, needs to be corrected.

  //  if (fdbg_decoded_cal) {
  //    cout << "THcShower::Decode: Shower raw hit list:" << endl;
  //    for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
  //      THcRawShowerHit* hit = (THcRawShowerHit *) fRawHitList->At(ihit);
  //      cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
  //	   << hit->fADC_pos << " " << hit->fADC_neg << " "  << endl;
  //    }
  //    cout << endl;
  //  }

  return nhits;
}

//_____________________________________________________________________________
Int_t THcShower::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
//Double_t THcShower::TimeWalkCorrection(const Int_t& paddle,
//					     const ESide side)
//{
//  return(0.0);
//}

//_____________________________________________________________________________
Int_t THcShower::CoarseProcess( TClonesArray& tracks)
{
  // Calculation of coordinates of particle track cross point with shower
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  
 if (fdbg_clusters_cal)
  cout << "THcShower::CoarseProcess called ---------------------------" <<endl;

 // Clustering of hits.
 //

 // Fill list of unclustered hits.

 if (fdbg_clusters_cal) cout << "Filling list of unclustered hits..."  << endl;

  THcShowerHitList HitList;

  for(Int_t j=0; j < fNLayers; j++) {

    if (fdbg_clusters_cal)cout << "  Plane " << j << "  Eplane = " 
			       << fPlanes[j]->GetEplane() << endl;

    for (Int_t i=0; i<fNBlocks[j]; i++) {

      //May be should be done this way.
      //
      //      Double_t Edep = fPlanes[j]->GetEmean(i);
      //      if (Edep > 0.) {                                 //hit
      //	Double_t x = YPos[j][i] + BlockThick[j]/2.;    //top + thick/2
      //	Double_t z = fNLayerZPos[j] + BlockThick[j]/2.;//front + thick/2
      //      	THcShowerHit* hit = new THcShowerHit(i,j,x,z,Edep);

      //ENGINE way.
      //
      if (fPlanes[j]->GetApos(i) - fPlanes[j]->GetPosPed(i) >
	  fPlanes[j]->GetPosThr(i) - fPlanes[j]->GetPosPed(i) ||
	  fPlanes[j]->GetAneg(i) - fPlanes[j]->GetNegPed(i) >
	  fPlanes[j]->GetNegThr(i) - fPlanes[j]->GetNegPed(i)) {    //hit
	Double_t Edep = fPlanes[j]->GetEmean(i);
	Double_t Epos = fPlanes[j]->GetEpos(i);
	Double_t Eneg = fPlanes[j]->GetEneg(i);
	Double_t x = XPos[j][i] + BlockThick[j]/2.;        //top + thick/2
	Double_t z = fNLayerZPos[j] + BlockThick[j]/2.;    //front + thick/2
	THcShowerHit* hit = new THcShowerHit(i,j,x,z,Edep,Epos,Eneg);

	HitList.push_back(hit);

	if (fdbg_clusters_cal)
	  cout << "    Hit: Edep = " << Edep << " X = " << x << " Z = " << z
	       << " Block " << i << " Layer " << j << endl;
      };

    }
  }

  //Print out hits before clustering.

  fNhits = HitList.size();

  if (fdbg_clusters_cal) {
    cout << "Total hits:     " << fNhits << endl;
    for (Int_t i=0; i!=fNhits; i++) {
      cout << "unclustered hit " << i << ": ";
      (*(HitList.begin()+i))->show();
    }
  }

  // Create list of clusters and fill it.

  //  fClusterList = new THcShowerClusterList; //shall be allocated before
  fClusterList->ClusterHits(HitList);

  fNclust = (*fClusterList).NbClusters();   //number of clusters

  //Print out the cluster list.

  if (fdbg_clusters_cal) {

    cout << "Cluster_list size: " << fNclust << endl;

    for (Int_t i=0; i!=fNclust; i++) {

      THcShowerCluster* cluster = (*fClusterList).ListedCluster(i);

      //cout << "Cluster #" << i 
	   //<<":  E=" << (*cluster).clE() 
	   //<< "  Epr=" << (*cluster).clEpr()
	   //<< "  X=" << (*cluster).clX()
	   //<< "  Z=" << (*cluster).clZ()
	   //<< "  size=" << (*cluster).clSize()
	   //<< endl;

      for (UInt_t j=0; j!=(*cluster).clSize(); j++) {
       	//THcShowerHit* hit = (*cluster).ClusteredHit(j);
       	//cout << "  hit #" << j << ":  "; (*hit).show();
      }

    }
  }

  // The following is for testing the code, by comparison with Engine.
  
  // The largest cluster by deposited energy, its size and energy
  // depositions.

  if (fNclust != 0) {

    THcShowerCluster* MaxCluster =  (*fClusterList).ListedCluster(0);
    fE = (*MaxCluster).clE();

    for (Int_t i=1; i<fNclust; i++) {


      THcShowerCluster* cluster = (*fClusterList).ListedCluster(i);

      Double_t E = (*cluster).clE();

      if (fE < E) {
	fE = E;
	MaxCluster = cluster;
      }
    }

    fMult = (*MaxCluster).clSize();    // number of hit counters
    fEpr = (*MaxCluster).clEpr();      // Preshower energy
    fX = (*MaxCluster).clX();          // X coordinate (in vertical direction)
  }

  if (fdbg_clusters_cal)
    cout << "Max.cluster: fEpr = " << fEpr << " fE = " << fE << " fX = " << fX
	 << endl;

  // Track-to-cluster  matching.
  //

  Int_t Ntracks = tracks.GetLast()+1;   // Number of reconstructed tracks

  if (fdbg_tracks_cal) {
    cout << endl;
    cout << "Number of reconstructed tracks = " << Ntracks << endl;
  }

  for (Int_t itrk=0; itrk<Ntracks; itrk++) {

    THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );

    if (fdbg_tracks_cal) {
      cout << "   Track " << itrk << ": "
	   << "  X = " << theTrack->GetX()
	   << "  Y = " << theTrack->GetY()
	   << "  Theta = " << theTrack->GetTheta()
	   << "  Phi = " << theTrack->GetPhi()
	   << endl;
    }

    // Track coordinates at front of the calorimeter, initialize out of
    // acceptance.
    Double_t Xtr = -75.;
    Double_t Ytr = -40.;

    // Associate a cluster to the track.

    Int_t mclust = MatchCluster(theTrack, Xtr, Ytr);

    //    if (mclust >= 0) fNtracks++;  // number of shower tracks (This is not consistent with engine!)

    // Do this for the 1-st track only for now.
    //
    if (itrk==0) {

      fTRX = Xtr;                // track coordinates at the calorimeter
      fTRY = Ytr;

      if (mclust >= 0) {         // if there is a matched cluster

	//Assign energies (not Y corrected) of the matched cluster to the track.
	THcShowerCluster* cluster = (*fClusterList).ListedCluster(mclust);
	fTRE = (*cluster).clE();
	fTREpr = (*cluster).clEpr();

	// Correct track energy depositions for the impact coordinate.

	fTRE_cor = 0.;           // Coord. corrected total energy depostion

	for (Int_t ip=0; ip<fNLayers; ip++) {

	  // Coordinate correction factors for positive and negative sides,
	  // different for single PMT counters in the 1-st two layes and for
	  // 2 PMT counters in the rear two layers.
	  Float_t corpos = 1.;   
	  Float_t corneg = 1.;
	  if (ip < fNegCols) {
	    corpos = Ycor(Ytr,0);
	    corneg = Ycor(Ytr,1);
	  }
	  else {
	    corpos = Ycor(Ytr);
	    corneg = 0.;
	  }

	  if (fdbg_tracks_cal) {
	    cout << "   Plane " << ip << "  Ytr = " << Ytr 
		 << "  corpos = " << corpos 
		 << "  corneg = " << corneg << endl;
	  }

	  fTREpl_pos_cor[ip] = (*cluster).clEplane(ip,0) * corpos;
	  fTREpl_neg_cor[ip] = (*cluster).clEplane(ip,1) * corneg;;
	  fTREpl_cor[ip] = fTREpl_pos_cor[ip] + fTREpl_neg_cor[ip];

	  if (fdbg_tracks_cal) {
	    cout << "   fTREpl_pos_cor = " << fTREpl_pos_cor[ip]
		 << "   fTREpl_neg_cor = " << fTREpl_neg_cor[ip] << endl;
	  }

	  fTRE_cor += fTREpl_cor[ip];

	}   //over planes

	fTREpr_cor = fTREpl_cor[0];   // Coord. corrected Edep in Preshower

	if (fdbg_tracks_cal)
	  cout << "   fTREpr = " << fTREpr 
	       << "   fTREpr_cor = " << fTREpr_cor << endl;

      }   //mclust>=0

    }     //itrk==0

  }       //over tracks

  if (fdbg_tracks_cal)
    cout << "1st track cluster:  fTRX = " << fTRX << "  fTRY = " << fTRY 
	 << endl;

  if (fdbg_clusters_cal)
  cout << "THcShower::CoarseProcess return ---------------------------" <<endl;

  return 0;
}

//-----------------------------------------------------------------------------

Int_t THcShower::MatchCluster(THaTrack* Track,
			      Double_t& XTrFront, Double_t& YTrFront)
{
  // Match a cluster to a given track. Return the cluster number,
  // and track coordinates at the front of calorimeter.

  if (fdbg_tracks_cal) {
    cout << "THcShower::MatchCluster: Track at DC:"
	 << "  X = " << Track->GetX()
	 << "  Y = " << Track->GetY()
	 << "  Theta = " << Track->GetTheta()
	 << "  Phi = " << Track->GetPhi()
	 << endl;
  }

  XTrFront = kBig;
  YTrFront = kBig;
  Double_t pathl = kBig;

  // Track interception with face of the calorimeter. The coordinates are
  // in the calorimeter's local system.

  fOrigin = fPlanes[0]->GetOrigin();

  CalcTrackIntercept(Track, pathl, XTrFront, YTrFront);

  // Transform coordiantes to the spectrometer's coordinate system.

  XTrFront += GetOrigin().X();
  YTrFront += GetOrigin().Y();

  if (fdbg_tracks_cal)
    cout << "  Track at the front of Calorimeter:"
	 << "  X = " << XTrFront
	 << "  Y = " << YTrFront
	 << "  Pathl = " << pathl
	 << endl;

  Bool_t inFidVol = true;            // In Fiducial Volume flag

  // Re-evaluate Fid. Volume Flag if fid. volume test is requested

  if (fvTest) {

    // Track coordinates at the back of the detector.

    // Origin at the front of the last layer.
    fOrigin = fPlanes[fNLayers-1]->GetOrigin();

    Double_t XTrBack = kBig;
    Double_t YTrBack = kBig;

    CalcTrackIntercept(Track, pathl, XTrBack, YTrBack);

    XTrBack += GetOrigin().X();   // from local coord. system
    YTrBack += GetOrigin().Y();   // to the spectrometer system

    if (fdbg_tracks_cal)
      cout << "  Track at the back of Calorimeter:"
	   << "  X = " << XTrBack
	   << "  Y = " << YTrBack
	   << "  Pathl = " << pathl
	   << endl;

    inFidVol = (XTrFront < fvXmax) && (XTrFront > fvXmin) &&
               (YTrFront < fvYmax) && (YTrFront > fvYmin) &&
               (XTrBack < fvXmax) && (XTrBack > fvXmin) &&
               (YTrBack < fvYmax) && (YTrBack > fvYmin);

    if (fdbg_tracks_cal) 
      cout << "  Fiducial volume test: inFidVol = " << inFidVol << endl;
  }

  // Match a cluster to the track.

  Int_t mclust = -1;    // The match cluster #, initialize with a bogus value.
  Double_t deltaX = kBig;   // Track to cluster distance

  if (inFidVol) {

    for (Int_t i=0; i<fNclust; i++) {

      THcShowerCluster* cluster = (*fClusterList).ListedCluster(i);

      Double_t dx = TMath::Abs( (*cluster).clX() - XTrFront );

      if (dx <= (0.5*BlockThick[0] + fSlop)) {
	fNtracks++;  // number of shower tracks (Consistent with engine)
	if (dx <= deltaX) {
	  mclust = i;
	  deltaX = dx;
	}
      }
    }
  }

  if (fdbg_tracks_cal)
    cout << "MatchCluster: mclust= " << mclust << "  delatX= " << deltaX 
	 << endl;

  return mclust;
}

//_____________________________________________________________________________
Float_t THcShower::GetShEnergy(THaTrack* Track) {

  // Get total energy deposited in the cluster matched to the given
  // spectrometer Track.

  // Track coordinates at front of the calorimeter, initialize out of
  // acceptance.
  Double_t Xtr = -75.;
  Double_t Ytr = -40.;

  // Associate a cluster to the track.

  Int_t mclust = MatchCluster(Track, Xtr, Ytr);

  if (fdbg_tracks_cal) {
    cout << "GetShEnergy: Track X = " << Xtr << "  Y = " << Ytr;
    if (mclust >= 0)
      cout << "  matched cluster # " << mclust << endl;
    else
      cout << " no matched cluster found" << endl;
  }

  // Coordinate corrected total energy deposition in the cluster.

  Float_t Etrk = 0.;
  if (mclust >= 0) {         // if there is a matched cluster

    // Get matched cluster.
    THcShowerCluster* cluster = (*fClusterList).ListedCluster(mclust);

    // Correct track energy depositions for the impact coordinate.

    for (Int_t ip=0; ip<fNLayers; ip++) {

      // Coordinate correction factors for positive and negative sides,
      // different for single PMT counters in the 1-st two layes and for
      // 2 PMT counters in the rear two layers.
      Float_t corpos = 1.;   
      Float_t corneg = 1.;
      if (ip < fNegCols) {
	corpos = Ycor(Ytr,0);
	corneg = Ycor(Ytr,1);
      }
      else {
	corpos = Ycor(Ytr);
	corneg = 0.;
      }

      if (fdbg_tracks_cal) {
	cout << "   Plane " << ip << "  Ytr = " << Ytr 
	     << "  corpos = " << corpos 
	     << "  corneg = " << corneg << endl;
      }

      Etrk += (*cluster).clEplane(ip,0) * corpos;
      Etrk += (*cluster).clEplane(ip,1) * corneg;;

    }   //over planes

  }   //mclust>=0

  if (fdbg_tracks_cal) cout << "   Etrk = " << Etrk << endl;

  return Etrk;
}

//_____________________________________________________________________________
Int_t THcShower::FineProcess( TClonesArray& tracks )
{

  // Shower energy assignment to the spectrometer tracks.
  //

  Int_t Ntracks = tracks.GetLast()+1;   // Number of reconstructed tracks

  if (fdbg_tracks_cal) {
    cout << endl;
    cout << "THcShower::FineProcess: Number of tracks = " << Ntracks << endl;
  }

  for (Int_t itrk=0; itrk<Ntracks; itrk++) {

    THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );

    Float_t energy = GetShEnergy(theTrack);
    theTrack->SetEnergy(energy);

    // if ( fEvent == 13252 )
    //   cout << "THcShower: track = " << itrk + 1 
    // 	   << "   energy = " << energy << endl;


    if (fdbg_tracks_cal) {
      cout << "THcShower::FineProcess, Track " << itrk << ": "
	   << "  X = " << theTrack->GetX()
	   << "  Y = " << theTrack->GetY()
	   << "  Theta = " << theTrack->GetTheta()
	   << "  Phi = " << theTrack->GetPhi()
	   << "  Enegy = " << energy << endl;
    }


  }       //over tracks

  return 0;
}

ClassImp(THcShower)
////////////////////////////////////////////////////////////////////////////////
 
