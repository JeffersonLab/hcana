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
#include "THcHallCSpectrometer.h"
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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>

using namespace std;

//_____________________________________________________________________________
THcShower::THcShower( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor
  fNLayers = 0;			// No layers until we make them
  fNTotLayers = 0;
  fHasArray = 0;

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
  char prefix[2];

  prefix[0] = tolower(GetApparatus()->GetName()[0]);
  prefix[1] = '\0';

  
  string layernamelist;
  fHasArray = 0;		// Flag for presence of fly's eye array
  DBRequest list[]={
    {"cal_num_layers", &fNLayers, kInt},
    {"cal_layer_names", &layernamelist, kString},
    {"cal_array",&fHasArray, kInt,0, 1}, 
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  fNTotLayers = (fNLayers+(fHasArray!=0?1:0));
  vector<string> layer_names = vsplit(layernamelist);

  if(layer_names.size() != fNTotLayers) {
    cout << "THcShower::Setup ERROR: Number of layers " << fNTotLayers 
	 << " doesn't agree with number of layer names "
	 << layer_names.size() << endl;
    // Should quit.  Is there an official way to quit?
  }

  fLayerNames = new char* [fNTotLayers];
  for(UInt_t i=0;i<fNTotLayers;i++) {
    fLayerNames[i] = new char[layer_names[i].length()+1];
    strcpy(fLayerNames[i], layer_names[i].c_str());
  }
  
  char *desc = new char[strlen(description)+100];
  fPlanes = new THcShowerPlane* [fNLayers];

  for(UInt_t i=0;i < fNLayers;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fLayerNames[i]);

    fPlanes[i] = new THcShowerPlane(fLayerNames[i], desc, i+1, this); 

  }
  if(fHasArray) {
    strcpy(desc, description);
    strcat(desc, " Array ");
    strcat(desc, fLayerNames[fNTotLayers-1]);

    fArray = new THcShowerArray(fLayerNames[fNTotLayers-1], desc, fNTotLayers, this);
  } else {
    fArray = 0;
  }

  delete [] desc;

  cout << "---------------------------------------------------------------\n";

  cout << "From THcShower::Setup: created Shower planes for "
       << GetApparatus()->GetName() << ": ";

  for(UInt_t i=0;i < fNTotLayers;i++) {
    cout << fLayerNames[i];
    i < fNTotLayers-1 ? cout << ", " : cout << ".\n";
  }

  if(fHasArray)
    cout << fLayerNames[fNTotLayers-1] << " has fly\'s eye configuration\n";

  cout << "---------------------------------------------------------------\n";

}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShower::Init( const TDatime& date )
{
  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  InitHitList(fDetMap, "THcRawShowerHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  for(UInt_t ip=0;ip<fNLayers;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }
  if(fHasArray) {
    if((status = fArray->Init( date ))) {
      return fStatus = status;
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


  cout << "---------------------------------------------------------------\n";
  cout << "From THcShower::Init: initialized " << GetApparatus()->GetName()
       <<  GetName() << endl;
  cout << "---------------------------------------------------------------\n";

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

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  fNegCols = fNLayers;		// If not defined assume tube on each end
                                // for every layer
  {
    DBRequest list[]={
      {"cal_num_neg_columns", &fNegCols, kInt, 0, 1},
      {"cal_slop", &fSlop, kDouble},
      {"cal_fv_test", &fvTest, kInt,0,1},
      {"cal_fv_delta", &fvDelta, kDouble},
      {"dbg_raw_cal", &fdbg_raw_cal, kInt, 0, 1},
      {"dbg_decoded_cal", &fdbg_decoded_cal, kInt, 0, 1},
      {"dbg_sparsified_cal", &fdbg_sparsified_cal, kInt, 0, 1},
      {"dbg_clusters_cal", &fdbg_clusters_cal, kInt, 0, 1},
      {"dbg_tracks_cal", &fdbg_tracks_cal, kInt, 0, 1},
      {"dbg_init_cal", &fdbg_init_cal, kInt, 0, 1},
      {0}
    };
    fvTest = 0;			// Default if not defined
    fdbg_raw_cal = 0;		// Debugging off by default
    fdbg_decoded_cal = 0;
    fdbg_sparsified_cal = 0;
    fdbg_clusters_cal = 0;
    fdbg_tracks_cal = 0;
    fdbg_init_cal = 0;

    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  // Debug output.
  if (fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::ReadDatabase for "
	 << GetApparatus()->GetName() << endl;

    cout << "  Number of neg. columns      = " << fNegCols << endl;
    cout << "  Slop parameter              = " << fSlop << endl;
    cout << "  Fiducial volume test flag   = " << fvTest << endl;
    cout << "  Fiducial volume excl. width = " << fvDelta << endl;
    cout << "  Initialize debug flag       = " << fdbg_init_cal  << endl;
    cout << "  Raw hit debug flag          = " << fdbg_raw_cal  << endl;
    cout << "  Decode debug flag           = " << fdbg_decoded_cal  << endl;
    cout << "  Sparsify debug flag         = " << fdbg_sparsified_cal  << endl;
    cout << "  Cluster debug flag          = " << fdbg_clusters_cal  << endl;
    cout << "  Tracking debug flag         = " << fdbg_tracks_cal  << endl;
  }

  {
    DBRequest list[]={
      {"cal_a_cor", &fAcor, kDouble},
      {"cal_b_cor", &fBcor, kDouble},
      {"cal_c_cor", fCcor, kDouble, 2},
      {"cal_d_cor", fDcor, kDouble, 2},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  // Debug output.
  if (fdbg_init_cal) {
    cout << "  Coordinate correction constants:\n";
    cout << "    fAcor = " << fAcor << endl;
    cout << "    fBcor = " << fBcor << endl;
    cout << "    fCcor = " << fCcor[0] << ", " << fCcor[1] << endl;
    cout << "    fDcor = " << fDcor[0] << ", " << fDcor[1] << endl;
  }

  BlockThick = new Double_t [fNLayers];
  fNBlocks = new UInt_t [fNLayers];
  fLayerZPos = new Double_t [fNLayers];
  fYPos = new Double_t [2*fNLayers];

  for(UInt_t i=0;i<fNLayers;i++) {
    DBRequest list[]={
      {Form("cal_%s_thick",fLayerNames[i]), &BlockThick[i], kDouble},
      {Form("cal_%s_nr",fLayerNames[i]), &fNBlocks[i], kInt},
      {Form("cal_%s_zpos",fLayerNames[i]), &fLayerZPos[i], kDouble},
      {Form("cal_%s_right",fLayerNames[i]), &fYPos[2*i], kDouble},
      {Form("cal_%s_left",fLayerNames[i]), &fYPos[2*i+1], kDouble},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  //Caution! Z positions (fronts) are off in hcal.param! Correct later on.

  fXPos = new Double_t* [fNLayers];
  for(UInt_t i=0;i<fNLayers;i++) {
    fXPos[i] = new Double_t [fNBlocks[i]];
    DBRequest list[]={
      {Form("cal_%s_top",fLayerNames[i]),fXPos[i], kDouble, fNBlocks[i]},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  }

  // Debug output.
  if (fdbg_init_cal) {
    for(UInt_t i=0;i<fNLayers;i++) {
      cout << "  Plane " << fLayerNames[i] << ":" << endl;
      cout << "    Block thickness: " << BlockThick[i] << endl;
      cout << "    NBlocks        : " << fNBlocks[i] << endl;
      cout << "    Z Position     : " << fLayerZPos[i] << endl;
      cout << "    Y Positions    : " << fYPos[2*i] << ", " << fYPos[2*i+1]
	   <<endl;
      cout << "    X Positions    :";
      for(UInt_t j=0; j<fNBlocks[i]; j++) {
	cout << " " << fXPos[i][j];
      }
      cout << endl;
    }
  }

  // Fiducial volume limits, based on Plane 1 positions.
  //

  fvXmin = fXPos[0][0] + fvDelta;
  fvXmax = fXPos[0][fNBlocks[0]-1] + BlockThick[0] - fvDelta;
  fvYmin = fYPos[0] + fvDelta;
  fvYmax = fYPos[1] - fvDelta;

  // Debug output.
  if (fdbg_init_cal) {
    cout << "  Fiducial volume limits:" << endl;
    cout << "    Xmin = " << fvXmin << "  Xmax = " << fvXmax << endl;
    cout << "    Ymin = " << fvYmin << "  Ymax = " << fvYmax << endl;
  }

  //Calibration related parameters (from hcal.param).

  fNTotBlocks=0;              //total number of blocks in the layers
  for (UInt_t i=0; i<fNLayers; i++) fNTotBlocks += fNBlocks[i];

  // Debug output.
  if (fdbg_init_cal) 
    cout << "  Total number of blocks in the layers of calorimeter: " << dec << fNTotBlocks
	 << endl;

  //Pedestal limits from hcal.param.
  fShPosPedLimit = new Int_t [fNTotBlocks];
  fShNegPedLimit = new Int_t [fNTotBlocks];

  //Calibration constants
  fPosGain = new Double_t [fNTotBlocks];
  fNegGain = new Double_t [fNTotBlocks];

  //Read in parameters from hcal.param
  Double_t hcal_pos_cal_const[fNTotBlocks];
  //  Double_t hcal_pos_gain_ini[fNTotBlocks];     not used
  //  Double_t hcal_pos_gain_cur[fNTotBlocks];     not used
  //  Int_t    hcal_pos_ped_limit[fNTotBlocks];    not used
  Double_t hcal_pos_gain_cor[fNTotBlocks];

  Double_t hcal_neg_cal_const[fNTotBlocks];
  //  Double_t hcal_neg_gain_ini[fNTotBlocks];     not used
  //  Double_t hcal_neg_gain_cur[fNTotBlocks];     not used
  //  Int_t    hcal_neg_ped_limit[fNTotBlocks];    not used
  Double_t hcal_neg_gain_cor[fNTotBlocks];

  DBRequest list[]={
    {"cal_pos_cal_const", hcal_pos_cal_const, kDouble, fNTotBlocks},
    //    {"cal_pos_gain_ini",  hcal_pos_gain_ini,  kDouble, fNTotBlocks},
    //    {"cal_pos_gain_cur",  hcal_pos_gain_cur,  kDouble, fNTotBlocks},
    {"cal_pos_ped_limit", fShPosPedLimit, kInt,    fNTotBlocks},
    {"cal_pos_gain_cor",  hcal_pos_gain_cor,  kDouble, fNTotBlocks},
    {"cal_neg_cal_const", hcal_neg_cal_const, kDouble, fNTotBlocks},
    //    {"cal_neg_gain_ini",  hcal_neg_gain_ini,  kDouble, fNTotBlocks},
    //    {"cal_neg_gain_cur",  hcal_neg_gain_cur,  kDouble, fNTotBlocks},
    {"cal_neg_ped_limit", fShNegPedLimit, kInt,    fNTotBlocks},
    {"cal_neg_gain_cor",  hcal_neg_gain_cor,  kDouble, fNTotBlocks},
    {"cal_min_peds", &fShMinPeds, kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

  // Debug output.
  if (fdbg_init_cal) {

    cout << "  hcal_pos_cal_const:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_pos_cal_const[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  fShPosPedLimit:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << fShPosPedLimit[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  hcal_pos_gain_cor:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_pos_gain_cor[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  hcal_neg_cal_const:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_neg_cal_const[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  fShNegPedLimit:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << fShNegPedLimit[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  hcal_neg_gain_cor:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << hcal_neg_gain_cor[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

  }    // end of debug output

  // Calibration constants (GeV / ADC channel).

  for (UInt_t i=0; i<fNTotBlocks; i++) {
    fPosGain[i] = hcal_pos_cal_const[i] *  hcal_pos_gain_cor[i];
    fNegGain[i] = hcal_neg_cal_const[i] *  hcal_neg_gain_cor[i];
  }

  // Debug output.
  if (fdbg_init_cal) {

    cout << "  fPosGain:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << fPosGain[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

    cout << "  fNegGain:" << endl;
    for (UInt_t j=0; j<fNLayers; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNBlocks[j]; i++) {
	cout << fNegGain[j*fNBlocks[j]+i] << " ";
      };
      cout <<  endl;
    };

  }

  // Origin of the calorimeter, at the centre of the face of the detector,
  // or at the centre of the front of the 1-st layer.
  //
  Double_t xOrig = (fXPos[0][0] + fXPos[0][fNBlocks[0]-1])/2 + BlockThick[0]/2;
  Double_t yOrig = (fYPos[0] + fYPos[1])/2;
  Double_t zOrig = fLayerZPos[0];

  fOrigin.SetXYZ(xOrig, yOrig, zOrig);

  // Debug output.
  if (fdbg_init_cal) {
    cout << "  Origin of the Calorimeter:" << endl;
    cout << "    Xorig = " << GetOrigin().X() << endl;
    cout << "    Yorig = " << GetOrigin().Y() << endl;
    cout << "    Zorig = " << GetOrigin().Z() << endl;
    cout << "---------------------------------------------------------------\n";
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

  // Register variables in global list

  RVarDef vars[] = {
    { "nhits", "Number of hits",                                 "fNhits" },
    { "nclust", "Number of clusters",                            "fNclust" },
    { "etot", "Total energy",                                    "fEtot" },
    { "etotnorm", "Total energy divided by Central Momentum",   "fEtotNorm" },
    { "ntracks", "Number of shower tracks",                      "fNtracks" },
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
  delete [] fLayerZPos;  fLayerZPos = NULL;
  delete [] fXPos;  fXPos = NULL;
  delete [] fZPos;  fZPos = NULL;
}

//_____________________________________________________________________________
inline 
void THcShower::Clear(Option_t* opt)
{

//   Reset per-event data.

  for(UInt_t ip=0;ip<fNLayers;ip++) {
    fPlanes[ip]->Clear(opt);
  }
  if(fHasArray) {
    fArray->Clear(opt);
  }

  fNhits = 0;
  fNclust = 0;
  fNtracks = 0;
  fEtot = 0.;
  fEtotNorm = 0.;

  // Purge cluster list

  for (THcShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    delete *i;
    *i = 0;
  }
  fClusterList->clear();

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
    for(UInt_t ip=0;ip<fNLayers;ip++) {
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
    }
    if(fHasArray) {
      nexthit = fArray->AccumulatePedestals(fRawHitList, nexthit);
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
    for(UInt_t ip=0;ip<fNLayers;ip++) {
      fPlanes[ip]->CalculatePedestals();
    }
    if(fHasArray) {
      fArray->CalculatePedestals();
    }
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t nexthit = 0;
  for(UInt_t ip=0;ip<fNLayers;ip++) {
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
    fEtot += fPlanes[ip]->GetEplane();
  }
  if(fHasArray) {
    nexthit = fArray->ProcessHits(fRawHitList, nexthit);
    fEtot += fArray->GetEplane();
  }
  THcHallCSpectrometer *app = static_cast<THcHallCSpectrometer*>(GetApparatus());
  fEtotNorm=fEtot/(app->GetPcentral());
 
  return nhits;
}

//_____________________________________________________________________________
Int_t THcShower::CoarseProcess( TClonesArray& tracks)
{
  // Calculation of coordinates of particle track cross point with shower
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  
  // Clustering of hits.
  //

  // Fill set of unclustered hits.

  // Ignore shower array (SHMS) for now

  THcShowerHitSet HitSet;

  for(UInt_t j=0; j < fNLayers; j++) {

    for (UInt_t i=0; i<fNBlocks[j]; i++) {

      //May be should be done this way.
      //
      //      Double_t Edep = fPlanes[j]->GetEmean(i);
      //      if (Edep > 0.) {                                 //hit
      //	Double_t x = fYPos[j][i] + BlockThick[j]/2.;    //top + thick/2
      //	Double_t z = fLayerZPos[j] + BlockThick[j]/2.;//front + thick/2
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
	Double_t x = fXPos[j][i] + BlockThick[j]/2.;        //top + thick/2
	Double_t z = fLayerZPos[j] + BlockThick[j]/2.;      //front + thick/2

	THcShowerHit* hit = new THcShowerHit(i,j,x,z,Edep,Epos,Eneg);

	HitSet.insert(hit);   //<set> version
      }

    }
  }

  fNhits = HitSet.size();

  //Debug output, print out hits before clustering.

  if (fdbg_clusters_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::CoarseProcess for "
	 << GetApparatus()->GetName() << endl;

    cout << "  List of unclustered hits. Total hits:     " << fNhits << endl;
    THcShowerHitIt it = HitSet.begin();    //<set> version
    for (Int_t i=0; i!=fNhits; i++) {
      cout << "  hit " << i << ": ";
      (*(it++))->show();
    }
  }

  // Fill list of clusters.

  ClusterHits(HitSet);

  fNclust = (*fClusterList).size();   //number of clusters

  //Debug output, print out the cluster list.

  if (fdbg_clusters_cal) {

    cout << "  Clustered hits. Number of clusters: " << fNclust << endl;

    UInt_t i = 0;
    for (THcShowerClusterListIt ppcl = (*fClusterList).begin();
	 ppcl != (*fClusterList).end(); ppcl++) {

      cout << "  Cluster #" << i++
	   <<":  E=" << clE(*ppcl) 
	   << "  Epr=" << clEpr(*ppcl)
	   << "  X=" << clX(*ppcl)
	   << "  Z=" << clZ(*ppcl)
	   << "  size=" << (**ppcl).size()
	   << endl;

      Int_t j=0;
      for (THcShowerClusterIt pph=(**ppcl).begin(); pph!=(**ppcl).end();
	   pph++) {
	cout << "  hit " << j++ << ": ";
	(**pph).show();
      }

    }

    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

//-----------------------------------------------------------------------------

void THcShower::ClusterHits(THcShowerHitSet& HitSet) {

  // Collect hits from the HitSet into the clusters. The resultant clusters
  // of hits are saved in the fClusterList.

  while (HitSet.size() != 0) {

    THcShowerCluster* cluster = new THcShowerCluster;

    THcShowerHitIt it = HitSet.end();
    (*cluster).insert(*(--it));   //Move the last hit from the hit list
    HitSet.erase(it);             //into the 1st cluster

    bool clustered = true;

    while (clustered) {                   //Proceed while a hit is clustered

      clustered = false;

      for (THcShowerHitIt i=HitSet.begin(); i!=HitSet.end(); ++i) {

	for (THcShowerClusterIt k=(*cluster).begin(); k!=(*cluster).end();
	     k++) {

	  if ((**i).isNeighbour(*k)) {

	    (*cluster).insert(*i);      //If the hit #i is neighbouring a hit
	    HitSet.erase(i);            //in the cluster, then move it
	                                //into the cluster.
	    clustered = true;
	  }

	  if (clustered) break;
	}                               //k

	if (clustered) break;
      }                                 //i

    }                                   //while clustered

    fClusterList->push_back(cluster);   //Put the cluster in the cluster list

  }                                     //While hit_list not exhausted

};

//-----------------------------------------------------------------------------

// Various helper functions to accumulate hit related quantities.

Double_t addE(Double_t x, THcShowerHit* h) {
  return x + h->hitE();
}

Double_t addX(Double_t x, THcShowerHit* h) {
  return x + h->hitE() * h->hitX();
}

Double_t addZ(Double_t x, THcShowerHit* h) {
  return x + h->hitE() * h->hitZ();
}

Double_t addEpr(Double_t x, THcShowerHit* h) {
  return h->hitColumn() == 0 ? x + h->hitE() : x;
}

Double_t addEpos(Double_t x, THcShowerHit* h) {
  return x + h->hitEpos();
}

Double_t addEneg(Double_t x, THcShowerHit* h) {
  return x + h->hitEneg();
}

// X coordinate of center of gravity of cluster, calculated as hit energy
// weighted average. Put X out of the calorimeter (-75 cm), if there is no
// energy deposition in the cluster.
//
Double_t clX(THcShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addX)/Etot : -75.);
}

// Z coordinate of center of gravity of cluster, calculated as a hit energy
// weighted average. Put Z out of the calorimeter (0 cm), if there is no energy
// deposition in the cluster.
//
Double_t clZ(THcShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addZ)/Etot : 0.);
}

//Energy depostion in a cluster
//
Double_t clE(THcShowerCluster* cluster) {
    return accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
}

//Energy deposition in the Preshower (1st plane) for a cluster
//
Double_t clEpr(THcShowerCluster* cluster) {
    return accumulate((*cluster).begin(),(*cluster).end(),0.,addEpr);
}

//Cluster energy deposition in plane iplane=0,..,3:
// side=0 -- from positive PMTs only;
// side=1 -- from negative PMTs only;
// side=2 -- from postive and negative PMTs.
//

Double_t clEplane(THcShowerCluster* cluster, Int_t iplane, Int_t side) {

  if (side!=0&&side!=1&&side!=2) {
    cout << "*** Wrong Side in clEplane:" << side << " ***" << endl;
    return -1;
  }

  THcShowerHitSet pcluster;
  for (THcShowerHitIt it=(*cluster).begin(); it!=(*cluster).end(); ++it) {
    if ((*it)->hitColumn() == iplane) pcluster.insert(*it);
  }

  Double_t Eplane;
  switch (side) {
  case 0 :
    Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addEpos);
    break;
  case 1 :
    Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addEneg);
    break;
  case 2 :
    Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addE);
    break;
  default :
    Eplane = 0. ;
  }

  return Eplane;
}

//-----------------------------------------------------------------------------

Int_t THcShower::MatchCluster(THaTrack* Track,
			      Double_t& XTrFront, Double_t& YTrFront)
{
  // Match a cluster to a given track. Return the cluster number,
  // and track coordinates at the front of calorimeter.

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

    inFidVol = (XTrFront <= fvXmax) && (XTrFront >= fvXmin) &&
               (YTrFront <= fvYmax) && (YTrFront >= fvYmin) &&
               (XTrBack <= fvXmax) && (XTrBack >= fvXmin) &&
               (YTrBack <= fvYmax) && (YTrBack >= fvYmin);

  }

  // Match a cluster to the track.

  Int_t mclust = -1;    // The match cluster #, initialize with a bogus value.
  Double_t deltaX = kBig;   // Track to cluster distance

  if (inFidVol) {

    // Since hits and clusters are in reverse order (with respect to Engine),
    // search backwards to be consistent with Engine.
    //
    for (Int_t i=fNclust-1; i>-1; i--) {

      THcShowerCluster* cluster = *(fClusterList->begin()+i);

      Double_t dx = TMath::Abs( clX(cluster) - XTrFront );

      if (dx <= (0.5*BlockThick[0] + fSlop)) {
	fNtracks++;  // number of shower tracks (Consistent with engine)
	if (dx < deltaX) {
	  mclust = i;
	  deltaX = dx;
	}
      }
    }
  }

  //Debug output.

  if (fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::MatchCluster for "
	 << GetApparatus()->GetName() << endl;

    cout << "  Track at DC:"
	 << "  X = " << Track->GetX()
	 << "  Y = " << Track->GetY()
	 << "  Theta = " << Track->GetTheta()
	 << "  Phi = " << Track->GetPhi()
	 << endl;
    cout << "  Track at the front of Calorimeter:"
	 << "  X = " << XTrFront
	 << "  Y = " << YTrFront
	 << "  Pathl = " << pathl
	 << endl;
    if (fvTest) 
      cout << "  Fiducial volume test: inFidVol = " << inFidVol << endl;

    cout << "  Matched cluster #" << mclust << ",  delatX= " << deltaX 
    	 << endl;
    cout << "---------------------------------------------------------------\n";
  }

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

  // Coordinate corrected total energy deposition in the cluster.

  Float_t Etrk = 0.;
  if (mclust >= 0) {         // if there is a matched cluster

    // Get matched cluster.

    THcShowerCluster* cluster = *(fClusterList->begin()+mclust);

    // Correct track energy depositions for the impact coordinate.

    for (UInt_t ip=0; ip<fNLayers; ip++) {

      // Coordinate correction factors for positive and negative sides,
      // different for double PMT counters in the 1-st two layes and for
      // single PMT counters in the rear two layers.
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

      Etrk += clEplane(cluster,ip,0) * corpos;
      Etrk += clEplane(cluster,ip,1) * corneg;

    }   //over planes

  }   //mclust>=0

  //Debug output.

  if (fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::GetShEnergy for "
	 << GetApparatus()->GetName() << endl;

    cout << "  Track at the calorimeter: X = " << Xtr << "  Y = " << Ytr;
    if (mclust >= 0)
      cout << ", matched cluster #" << mclust << "." << endl;
    else
      cout << ", no matched cluster found." << endl;

    cout << "  Coordinate corrected track energy = " << Etrk << "." << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return Etrk;
}

//_____________________________________________________________________________
Int_t THcShower::FineProcess( TClonesArray& tracks )
{

  // Shower energy assignment to the spectrometer tracks.
  //

  Int_t Ntracks = tracks.GetLast()+1;   // Number of reconstructed tracks

  for (Int_t itrk=0; itrk<Ntracks; itrk++) {

    THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );

    Float_t energy = GetShEnergy(theTrack);
    theTrack->SetEnergy(energy);

  }       //over tracks

  //Debug output.

  if (fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::FineProcess for "
	 << GetApparatus()->GetName() << endl;

    cout << "  Number of tracks = " << Ntracks << endl;

    for (Int_t itrk=0; itrk<Ntracks; itrk++) {
      THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );
      cout << "  Track " << itrk << ": "
	   << "  X = " << theTrack->GetX()
	   << "  Y = " << theTrack->GetY()
	   << "  Theta = " << theTrack->GetTheta()
	   << "  Phi = " << theTrack->GetPhi()
	   << "  Energy = " << theTrack->GetEnergy() << endl;
    }

    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

Double_t THcShower::GetNormETot( ){
  return fEtotNorm;
} 

ClassImp(THcShower)
////////////////////////////////////////////////////////////////////////////////
