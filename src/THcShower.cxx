/** \class THcShower
    \ingroup Detectors

\brief Generic segmented shower detector.

*/
 
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
#include "Helper.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <cassert>

using namespace std;

//_____________________________________________________________________________
THcShower::THcShower( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus),
  fPosAdcTimeWindowMin(0), fNegAdcTimeWindowMin(0),
  fPosAdcTimeWindowMax(0), fNegAdcTimeWindowMax(0),
  fPedPosDefault(0),fPedNegDefault(0),
  fShPosPedLimit(0), fShNegPedLimit(0), fPosGain(0), fNegGain(0),
  fClusterList(0), fLayerNames(0), fLayerZPos(0), BlockThick(0),
  fNBlocks(0), fXPos(0), fYPos(0), fZPos(0), fPlanes(0), fArray(0)
{
  // Constructor
  fNLayers = 0;			// No layers until we make them
  fNTotLayers = 0;
  fHasArray = 0;

  fClusterList = new THcShowerClusterList;
}

//_____________________________________________________________________________
THcShower::THcShower( ) :
  THaNonTrackingDetector(),
  fPosAdcTimeWindowMin(0), fNegAdcTimeWindowMin(0),
  fPosAdcTimeWindowMax(0), fNegAdcTimeWindowMax(0),
  fPedPosDefault(0),fPedNegDefault(0),
  fShPosPedLimit(0), fShNegPedLimit(0), fPosGain(0), fNegGain(0),
  fClusterList(0), fLayerNames(0), fLayerZPos(0), BlockThick(0),
  fNBlocks(0), fXPos(0), fYPos(0), fZPos(0), fPlanes(0), fArray(0)
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
    {"cal_adcrefcut", &fADC_RefTimeCut, kInt, 0, 1},
    {0}
  };

  fADC_RefTimeCut = 0;
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

    fArray = new THcShowerArray(fLayerNames[fNTotLayers-1], desc, fNTotLayers,
				this);
  } else {
    fArray = 0;
  }

  delete [] desc;

  // cout << "---------------------------------------------------------------\n";

  cout << "From THcShower::Setup: created Shower planes for "
       << GetApparatus()->GetName() << ": ";

  for(UInt_t i=0;i < fNTotLayers;i++) {
    cout << fLayerNames[i];
    i < fNTotLayers-1 ? cout << ", " : cout << ".\n";
  }

  // if(fHasArray)
  //   cout << fLayerNames[fNTotLayers-1] << " has fly\'s eye configuration\n";

  // cout << "---------------------------------------------------------------\n";

}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShower::Init( const TDatime& date )
{
  Setup(GetName(), GetTitle());

  char EngineDID[] = "xCAL";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID );
    return kInitError;
  }

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  InitHitList(fDetMap, "THcRawShowerHit", fDetMap->GetTotNumChan()+1,
	      0, fADC_RefTimeCut);

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

  if(fHasArray) {
    // cout << "THcShower::Init: adjustment of fiducial volume limits to the fly's eye part." << endl;
    // cout << "  Old limits:" << endl;
    // cout << "    Xmin = " << fvXmin << "  Xmax = " << fvXmax << endl;
    // cout << "    Ymin = " << fvYmin << "  Ymax = " << fvYmax << endl;
    fvXmin = TMath::Max(fvXmin, fArray->fvXmin());
    fvXmax = TMath::Min(fvXmax, fArray->fvXmax());
    fvYmin = TMath::Max(fvYmin, fArray->fvYmin());
    fvYmax = TMath::Min(fvYmax, fArray->fvYmax());
    // cout << "  New limits:" << endl;
    // cout << "    Xmin = " << fvXmin << "  Xmax = " << fvXmax << endl;
    // cout << "    Ymin = " << fvYmin << "  Ymax = " << fvYmax << endl;
  }

  // cout << "---------------------------------------------------------------\n";
  // cout << "From THcShower::Init: initialized " << GetApparatus()->GetName()
  //      <<  GetName() << endl;
  // cout << "---------------------------------------------------------------\n";

  fPresentP = 0;
  THaVar* vpresent = gHaVars->Find(Form("%s.present",GetApparatus()->GetName()));
  if(vpresent) {
    fPresentP = (Bool_t *) vpresent->GetValuePointer();
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

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  CreateMissReportParms(Form("%scal",prefix));

  fNegCols = fNLayers;		// If not defined assume tube on each end
                                // for every layer
  {
    DBRequest list[]={
      {"cal_num_neg_columns", &fNegCols, kInt, 0, 1},
      {"cal_slop", &fSlop, kDouble,0,1},
      {"cal_fv_test", &fvTest, kInt,0,1},
      {"cal_fv_delta", &fvDelta, kDouble,0,1},
      {"cal_ADCMode", &fADCMode, kInt, 0, 1},
      {"cal_adc_tdc_offset", &fAdcTdcOffset, kDouble, 0, 1},
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
    fAdcTdcOffset=0.0;
    fADCMode=kADCDynamicPedestal;
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

  //Calibration related parameters (from h(s)cal.param).

  fNTotBlocks=0;              //total number of blocks in the layers
  for (UInt_t i=0; i<fNLayers; i++) fNTotBlocks += fNBlocks[i];

  // Debug output.
  if (fdbg_init_cal)
    cout << "  Total number of blocks in the layers of calorimeter: " << dec
	 << fNTotBlocks << endl;

  //Pedestal limits from hcal.param.
  fShPosPedLimit = new Int_t [fNTotBlocks];
  fShNegPedLimit = new Int_t [fNTotBlocks];
  for (UInt_t i=0; i<fNTotBlocks; i++) {
    fShPosPedLimit[i]=0.;
    fShNegPedLimit[i]=0.;
  }

  //Calibration constants
  fPosGain = new Double_t [fNTotBlocks];
  fNegGain = new Double_t [fNTotBlocks];

  //Read in parameters from hcal.param
  Double_t hcal_pos_cal_const[fNTotBlocks];
  Double_t hcal_pos_gain_cor[fNTotBlocks];

  Double_t hcal_neg_cal_const[fNTotBlocks];
  Double_t hcal_neg_gain_cor[fNTotBlocks];

  fPosAdcTimeWindowMin = new Double_t [fNTotBlocks];
  fNegAdcTimeWindowMin = new Double_t [fNTotBlocks];
  fPosAdcTimeWindowMax = new Double_t [fNTotBlocks];
  fNegAdcTimeWindowMax = new Double_t [fNTotBlocks];
  fPedPosDefault = new Int_t [fNTotBlocks];
  fPedNegDefault = new Int_t [fNTotBlocks];

  DBRequest list[]={
    {"cal_pos_cal_const", hcal_pos_cal_const, kDouble, fNTotBlocks},
    {"cal_pos_ped_limit", fShPosPedLimit, kInt,    fNTotBlocks,1},
    {"cal_pos_gain_cor",  hcal_pos_gain_cor,  kDouble, fNTotBlocks},
    {"cal_neg_cal_const", hcal_neg_cal_const, kDouble, fNTotBlocks},
    {"cal_neg_ped_limit", fShNegPedLimit, kInt,    fNTotBlocks,1},
    {"cal_neg_gain_cor",  hcal_neg_gain_cor,  kDouble, fNTotBlocks},
    {"cal_pos_AdcTimeWindowMin", fPosAdcTimeWindowMin, kDouble, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_neg_AdcTimeWindowMin", fNegAdcTimeWindowMin, kDouble, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_pos_AdcTimeWindowMax", fPosAdcTimeWindowMax, kDouble, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_neg_AdcTimeWindowMax", fNegAdcTimeWindowMax, kDouble, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_PedNegDefault", fPedNegDefault, kInt, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_PedPosDefault", fPedNegDefault, kInt, static_cast<UInt_t>(fNTotBlocks),1},
    {"cal_min_peds", &fShMinPeds, kInt,0,1},
    {0}
  };
  fShMinPeds=0.;

  for(UInt_t ip=0;ip<fNTotBlocks;ip++) {
    fPosAdcTimeWindowMin[ip] = -1000.;
    fNegAdcTimeWindowMin[ip] = -1000.;
    fPosAdcTimeWindowMax[ip] = 1000.;
    fNegAdcTimeWindowMax[ip] = 1000.;
    fPedNegDefault[ip] = 0;
    fPedPosDefault[ip] = 0;
   }

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
    { "nclust", "Number of layer clusters",                            "fNclust" },
    { "nclusttrack", "Number of layer cluster which matched best track","fNclustTrack" },
    { "xclusttrack", "X pos of layer cluster which matched best track","fXclustTrack" },
    { "xtrack", "X pos of track which matched layer cluster", "fXTrack" },
    { "yclusttrack", "Y pos of layer cluster which matched best track","fYclustTrack" },
    { "ytrack", "Y pos of track which matched layer cluster", "fYTrack" },
    { "etot", "Total energy",                                    "fEtot" },
    { "etotnorm", "Total energy divided by Central Momentum",    "fEtotNorm" },
    { "etrack", "Track energy",                                  "fEtrack" },
    { "etracknorm", "Total energy divided by track momentum",    "fEtrackNorm" },
    { "eprtrack", "Track Preshower energy",                      "fEPRtrack" },
    { "eprtracknorm", "Preshower energy divided by track momentum", "fEPRtrackNorm" },
    { "etottracknorm", "Total energy divided by track momentum", "fETotTrackNorm" },
    { "ntracks", "Number of shower tracks",                      "fNtracks" },
    { 0 }
  };

  if(fHasArray) {

  RVarDef array_vars[] = {
    { "sizeclustarray", "Number of block in array cluster that matches track", "fSizeClustArray" },
    { "nclustarraytrack", "Number of cluster for Fly's eye which matched best track","fNclustArrayTrack" },
    { "nblock_high_ene", "Block number in array with highest energy which matched best track","fNblockHighEnergy" },
    { "xclustarraytrack", "X pos of cluster which matched best track","fXclustArrayTrack" },
    { "xtrackarray", "X pos of track which matched cluster", "fXTrackArray" },
    { "yclustarraytrack", "Y pos of cluster which matched best track","fYclustArrayTrack" },
    { "ytrackarray", "Y pos of track which matched cluster", "fYTrackArray" },
    { 0 }
  };

  DefineVarsFromList( array_vars, mode );
  }
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
THcShower::~THcShower()
{
  // Destructor. Remove variables from global list.

  Clear();
  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();

  for (THcShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    Podd::DeleteContainer(**i);
    delete *i;
  }
  delete fClusterList; fClusterList = 0;

  for( UInt_t i = 0; i<fNLayers; ++i) {
    delete fPlanes[i];
  }
  delete [] fPlanes; fPlanes = 0;
  delete fArray; fArray = 0;
}

//_____________________________________________________________________________
void THcShower::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  for (UInt_t i = 0; i<fNTotLayers; ++i)
    delete [] fLayerNames[i];
  delete [] fLayerNames; fLayerNames = 0;

  delete [] fPosAdcTimeWindowMin; fPosAdcTimeWindowMin = 0;
  delete [] fNegAdcTimeWindowMin; fNegAdcTimeWindowMin = 0;
  delete [] fPosAdcTimeWindowMax; fPosAdcTimeWindowMax = 0;
  delete [] fNegAdcTimeWindowMax; fNegAdcTimeWindowMax = 0;
  delete [] fPedNegDefault; fPedNegDefault = 0;
  delete [] fPedPosDefault; fPedPosDefault = 0;
  delete [] fShPosPedLimit; fShPosPedLimit = 0;
  delete [] fShNegPedLimit; fShNegPedLimit = 0;
  delete [] fPosGain; fPosGain = 0;
  delete [] fNegGain; fNegGain = 0;
  delete [] BlockThick;  BlockThick = NULL;
  delete [] fNBlocks;  fNBlocks = NULL;
  delete [] fLayerZPos;  fLayerZPos = NULL;
  for (UInt_t i = 0; i<fNLayers; ++i)
    delete [] fXPos[i];
  delete [] fXPos;  fXPos = NULL;
  delete [] fYPos;  fYPos = NULL;
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
  fNclustTrack = -2;
  fXclustTrack = -1000;
  fXTrack = -1000;
  fYclustTrack = -1000;
  fYTrack = -1000;
  fNclustArrayTrack = -2;
  fXclustArrayTrack = -1000;
  fXTrackArray = -1000;
  fYclustArrayTrack = -1000;
  fYTrackArray = -1000;
  fNtracks = 0;
  fEtot = 0.;
  fEtotNorm = 0.;
  fEtrack = 0.;
  fEtrackNorm = 0.;
  fEPRtrack = 0.;
  fEPRtrackNorm = 0.;
  fETotTrackNorm = 0.;
  fSizeClustArray = 0;
  fNblockHighEnergy = 0.;

  // Purge cluster list

  for (THcShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    Podd::DeleteContainer(**i);
    delete *i;
  }
  fClusterList->clear();
}

//_____________________________________________________________________________
Int_t THcShower::Decode( const THaEvData& evdata )
{

  Clear();

  // Get the Hall C style hitlist (fRawHitList) for this event
  Bool_t present = kTRUE;	// Suppress reference time warnings
  if(fPresentP) {		// if this spectrometer not part of trigger
    present = *fPresentP;
  }
  Int_t nhits = DecodeToHitList(evdata, !present);
  
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
  }
  if(fHasArray) {
    nexthit = fArray->ProcessHits(fRawHitList, nexthit);
  }

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
  for(UInt_t ip=0;ip<fNLayers;ip++) {
    fPlanes[ip]->CoarseProcessHits();
    fEtot += fPlanes[ip]->GetEplane();
  }
  if(fHasArray) {
    fArray->CoarseProcessHits();
    fEtot += fArray->GetEarray();
  }
  THcHallCSpectrometer *app = static_cast<THcHallCSpectrometer*>(GetApparatus());
  fEtotNorm=fEtot/(app->GetPcentral());
  //
  THcShowerHitSet HitSet;

  for(UInt_t j=0; j < fNLayers; j++) {

    for (UInt_t i=0; i<fNBlocks[j]; i++) {

      //
      if (fPlanes[j]->GetAposP(i) > 0  || fPlanes[j]->GetAnegP(i) >0) {    //hit

	Double_t Edep = fPlanes[j]->GetEmean(i);
	Double_t Epos = fPlanes[j]->GetEpos(i);
	Double_t Eneg = fPlanes[j]->GetEneg(i);
	Double_t x = fXPos[j][i] + BlockThick[j]/2.;        //top + thick/2
        Double_t y=-1000;
        if (fHasArray) {
	  if (Eneg>0) y = fYPos[2*j]/2 ;  
	  if (Epos>0) y = fYPos[2*j+1]/2 ;  
	  if (Epos>0 && Eneg>0) y = 0. ;  
        } else {
          y=0.;
	}
	Double_t z = fLayerZPos[j] + BlockThick[j]/2.;      //front + thick/2

	THcShowerHit* hit = new THcShowerHit(i,j,x,y,z,Edep,Epos,Eneg);

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

    cout << " event = " << fEvent << endl;
    cout << "  List of unclustered hits. Total hits:     " << fNhits << endl;
    THcShowerHitIt it = HitSet.begin();    //<set> version
    for (Int_t i=0; i!=fNhits; i++) {
      cout << "  hit " << i << ": ";
      (*(it++))->show();
    }
  }

  // Fill list of clusters.

  ClusterHits(HitSet, fClusterList);
  assert( HitSet.empty() );  // else bug in ClusterHits()

  fNclust = (*fClusterList).size();   //number of clusters

  //Debug output, print out the cluster list.

  if (fdbg_clusters_cal) {

    cout << " event = " << fEvent << "  Clustered hits. Number of clusters: " << fNclust << endl;

    UInt_t i = 0;
    for (THcShowerClusterListIt ppcl = (*fClusterList).begin();
	 ppcl != (*fClusterList).end(); ++ppcl) {

      cout << "  Cluster #" << i++
	   <<":  E=" << clE(*ppcl)
	   << "  Epr=" << clEpr(*ppcl)
	   << "  X=" << clX(*ppcl)
	   << "  Z=" << clZ(*ppcl)
	   << "  size=" << (**ppcl).size()
	   << endl;

      Int_t j=0;
      for (THcShowerClusterIt pph=(**ppcl).begin(); pph!=(**ppcl).end();
	   ++pph) {
	cout << "  hit " << j++ << ": ";
	(**pph).show();
      }

    }

    cout << "---------------------------------------------------------------\n";
  }

  // Do same for Array.

  if(fHasArray) fArray->CoarseProcess(tracks);

  //  
  Int_t Ntracks = tracks.GetLast()+1;   // Number of reconstructed tracks
  Double_t save_energy=0;
 for (Int_t itrk=0; itrk<Ntracks; itrk++) {

    THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );
    //     save_energy = GetShEnergy(theTrack);
    save_energy = GetShEnergy(theTrack, fNLayers);
      if (fHasArray) save_energy += fArray->GetShEnergy(theTrack);
    theTrack->SetEnergy(save_energy);
  }       //over tracks

  //
  return 0;
}

//-----------------------------------------------------------------------------

void THcShower::ClusterHits(THcShowerHitSet& HitSet,
			    THcShowerClusterList* ClusterList) {

  // Collect hits from the HitSet into the clusters. The resultant clusters
  // of hits are saved in the ClusterList.

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
	     ++k) {

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

    ClusterList->push_back(cluster);   //Put the cluster in the cluster list

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

Double_t addY(Double_t x, THcShowerHit* h) {
  return x + h->hitE() * h->hitY();
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

// Y coordinate of center of gravity of cluster, calculated as hit energy
// weighted average. Put X out of the calorimeter (-100 cm), if there is no
// energy deposition in the cluster.
//
Double_t clY(THcShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addY)/Etot : -100.);
}
// X coordinate of center of gravity of cluster, calculated as hit energy
// weighted average. Put X out of the calorimeter (-100 cm), if there is no
// energy deposition in the cluster.
//
Double_t clX(THcShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addX)/Etot : -100.);
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
	fNtracks++;  // lumber of shower tracks (Consistent with engine)
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
    cout << " event = " << fEvent << endl;

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
Float_t THcShower::GetShEnergy(THaTrack* Track, UInt_t NLayers, UInt_t L0) {

  // Get part of energy deposited in the cluster matched to the given
  // spectrometer Track, limited by range of layers from L0 to L0+NLayers-1.

  // Track coordinates at front of the calorimeter, initialize out of
  // acceptance.
  Double_t Xtr = -100.;
  Double_t Ytr = -100.;

  // Associate a cluster to the track.

  Int_t mclust = MatchCluster(Track, Xtr, Ytr);

  // Coordinate corrected total energy deposition in the cluster.

  Float_t Etrk = 0.;
  if (mclust >= 0) {         // if there is a matched cluster

    // Get matched cluster.

    THcShowerCluster* cluster = *(fClusterList->begin()+mclust);

    char prefix = GetApparatus()->GetName()[0];

    // Correct track energy depositions for the impact coordinate.

    //    for (UInt_t ip=0; ip<fNLayers; ip++) {
    for (UInt_t ip=L0; ip<L0+NLayers; ip++) {

      // Coordinate correction factors for positive and negative sides,
      // different for double PMT counters in the 1-st two HMS layes,
      // single PMT counters in the rear two HMS layers, and in the SHMS
      // Preshower.
      Float_t corpos = 1.;
      Float_t corneg = 1.;

      if (ip < fNegCols) {
	if (prefix == 'H') {              //HMS 1-st 2 layers
	  corpos = Ycor(Ytr,0);
	  corneg = Ycor(Ytr,1);
	}
	else {                            //SHMS Preshower
	  corpos = YcorPr(Ytr,0);
	  corneg = YcorPr(Ytr,1);
	}
      }
      else {
	corpos = Ycor(Ytr);
	corneg = 0.;
      }

      // cout << ip << " clust energy pos = " <<  clEplane(cluster,ip,0)<< " clust energy neg = " <<  clEplane(cluster,ip,1) << endl;
      Etrk += clEplane(cluster,ip,0) * corpos;
      Etrk += clEplane(cluster,ip,1) * corneg;

    }   //over planes

  }   //mclust>=0

  //Debug output.

  if (fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::GetShEnergy for "
	 << GetApparatus()->GetName() << endl;
    cout << " event = " << fEvent << endl;

    cout << "  Track at the calorimeter: X = " << Xtr << "  Y = " << Ytr;
    if (mclust >= 0)
      cout << ", matched cluster #" << mclust << "." << endl;
    else
      cout << ", no matched cluster found." << endl;

    cout << "  Layers from " << L0+1 << " to " << L0+NLayers << ".\n";
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
      Double_t Xtr = -100.;
      Double_t Ytr = -100.;
  for (Int_t itrk=0; itrk<Ntracks; itrk++) {
    THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );
    if (theTrack->GetIndex()==0) {
      fEtrack=theTrack->GetEnergy();
      fEtrackNorm=fEtrack/theTrack->GetP();
      fEPRtrack=GetShEnergy(theTrack,1);
      fEPRtrackNorm=fEPRtrack/theTrack->GetP();
      fETotTrackNorm=fEtot/theTrack->GetP();
      Xtr = -100.;
      Ytr = -100.;               
      fNclustTrack = MatchCluster(theTrack, Xtr, Ytr);
      fXTrack=Xtr;
      fYTrack=Ytr;
      if (fNclustTrack>=0) {
	THcShowerCluster* cluster = *(fClusterList->begin()+fNclustTrack);
	fXclustTrack=clX(cluster);
        fYclustTrack=clY(cluster);
      }
      if (fHasArray) {
	fNclustArrayTrack = fArray->MatchCluster(theTrack,Xtr,Ytr);      
        if (fNclustArrayTrack>=0) {
          fXclustArrayTrack=fArray->GetClX();
          fYclustArrayTrack=fArray->GetClY();
          fSizeClustArray=fArray->GetClSize();
          fNblockHighEnergy=fArray->GetClMaxEnergyBlock();
          fXTrackArray=Xtr;
          fYTrackArray=Ytr;
        }
      }
    }
  }       //over tracks

  if (Ntracks>0) {
    for(UInt_t ip=0;ip<fNLayers;ip++) {
      fPlanes[ip]-> AccumulateStat(tracks);
    }
    if(fHasArray) fArray->AccumulateStat(tracks);
  }
  
  //Debug output.

  if (fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShower::FineProcess for "
	 << GetApparatus()->GetName() << endl;
    cout << " event = " << fEvent << endl;
    cout << "  Number of tracks = " << Ntracks << endl;
    if (fNclustTrack>=0) {
      cout << " matching cluster info " << endl;
      cout << " X info = " << fXclustTrack << " " << Xtr << " " << fXclustTrack-Xtr << endl;
      cout << " Y info = " << fYclustTrack << " " << Ytr << " " << fYclustTrack-Ytr << endl;
    }

    for (Int_t itrk=0; itrk<Ntracks; itrk++) {
      THaTrack* theTrack = static_cast<THaTrack*>( tracks[itrk] );
      cout << "  Track " << itrk << ": "
	   << "  X = " << theTrack->GetX()
	   << "  Y = " << theTrack->GetY()
	   << "  Theta = " << theTrack->GetTheta()
	   << "  Phi = " << theTrack->GetPhi()
	   << "  Energy = " << theTrack->GetEnergy() 
	   << "  Energy/Ptrack = " <<  fEtrackNorm << endl;
    }

    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

Double_t THcShower::GetNormETot( ){
  return fEtotNorm;
}
//_____________________________________________________________________________
Int_t THcShower::End(THaRunBase* run)
{
  MissReport(Form("%s.%s", GetApparatus()->GetName(), GetName()));
  return 0;
}

ClassImp(THcShower)
////////////////////////////////////////////////////////////////////////////////
