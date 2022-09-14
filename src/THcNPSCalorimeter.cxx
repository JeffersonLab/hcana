/** \class THcNPSCalorimeter
    \ingroup Detectors

\brief Generic segmented shower detector.

*/

#include "THcNPSCalorimeter.h"
#include "THcHallCSpectrometer.h"
#include "THcRawShowerHit.h"
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
#include "TRandom.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <cassert>

using namespace std;

//_____________________________________________________________________________
THcNPSCalorimeter::THcNPSCalorimeter( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus),
  //  fClusterList(0), fLayerNames(0), fLayerZPos(0), BlockThick(0),
  //fNBlocks(0),
  //fXPos(0), fYPos(0), fZPos(0),
  fCalReplicate(0), fAnalyzer(0), fArray(0)
{
  
  //cout << "Calling Constructor | THcNPSCalorimeter::THcNPSCalorimeter() . . . " << endl;

  // Constructor
  //  fNLayers = 0;			// No layers until we make them
  fNTotLayers = 0;
  fHasArray = 0;

  fClusterList = new THcNPSShowerClusterList;
}

//_____________________________________________________________________________
THcNPSCalorimeter::THcNPSCalorimeter( ) :
  THaNonTrackingDetector(),
  fClusterList(0), fLayerNames(0),
  //fLayerZPos(0), BlockThick(0),
  //fNBlocks(0),
  //fXPos(0), fYPos(0), fZPos(0),
  fArray(0)
{
  // Constructor
}

//_____________________________________________________________________________
void THcNPSCalorimeter::Setup(const char* name, const char* description)
{
  // cout<< "Calling THcNPSCalorimeter::Setup() . . ." << endl;
  
  //Get apparatus prefix name
  string kwPrefix = GetApparatus()->GetName();
  std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::tolower);
  fKwPrefix = kwPrefix;
  
  string layernamelist;
  fHasArray = 0;		// Flag for presence of fly's eye array
  fCalReplicate = 0;
  DBRequest list[]={
    //    {"cal_num_layers", &fNLayers, kInt},
    {"_cal_arr_nrows", &fNRows, kInt},
    {"_cal_arr_ncolumns", &fNColumns, kInt},
    {"_cal_layer_names", &layernamelist, kString},
    {"_cal_array",&fHasArray, kInt,0, 1},
    {"_cal_adcrefcut", &fADC_RefTimeCut, kInt, 0, 1},
    {"_cal_replicate", &fCalReplicate, kInt, 0, 1},
    {0}
  };

  fADC_RefTimeCut = 0;
  gHcParms->LoadParmValues((DBRequest*)&list,fKwPrefix.c_str());
  //  fNTotLayers = (fNLayers+(fHasArray!=0?1:0));
  fNTotLayers = (fHasArray!=0?1:0);
  vector<string> layer_names = Podd::vsplit(layernamelist);

  if(layer_names.size() != fNTotLayers) {
    cout << "THcNPSCalorimeter::Setup ERROR: Number of layers " << fNTotLayers
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
  //  fPlanes = new THcShowerPlane* [fNLayers];

  if(fHasArray) {
    strcpy(desc, description);
    strcat(desc, " Array ");
    strcat(desc, fLayerNames[fNTotLayers-1]);

    fArray = new THcNPSArray(fLayerNames[fNTotLayers-1], desc, fNTotLayers,
				this);
  } else {
    fArray = 0;
  }

  delete [] desc;

  // cout << "---------------------------------------------------------------\n";

  // if(fHasArray)
  //   cout << fLayerNames[fNTotLayers-1] << " has fly\'s eye configuration\n";

  // cout << "---------------------------------------------------------------\n";

}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcNPSCalorimeter::Init( const TDatime& date )
{

  // cout<< "Calling THcNPSCalorimeter::Init() . . ." << endl;

  Setup(GetName(), GetTitle());

  // DJH 09/04/22 -- only create dedicated analyzer object if using SHMS data
  if(fCalReplicate==0)
    fAnalyzer = static_cast<THcNPSAnalyzer *>(THcNPSAnalyzer::GetInstance());

  string kwPrefix = GetApparatus()->GetName();
  //std::transform(kwPrefix.begin(), kwPrefix.end(), kwPrefix.begin(), ::toupper);

  const char* EngineDID = Form("%sCAL", kwPrefix.c_str());
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
Int_t THcNPSCalorimeter::ReadDatabase( const TDatime& date )
{
  // cout<< "Calling THcNPSCalorimeter::ReadDatabase() . . ." << endl;

  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";

  // Read data from database
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  CreateMissReportParms(Form("%s_cal",fKwPrefix.c_str()));

  //fNegCols = fNLayers;		// If not defined assume tube on each end
                                // for every layer
  {
    DBRequest list[]={
      //      {"_cal_num_neg_columns", &fNegCols, kInt, 0, 1},
      {"_cal_clustering", &fClustMethod, kInt, 0, 1},
      {"_cal_make_grid", &fMakeGrid, kInt, 0, 1},
      {"_cal_slop", &fSlop, kDouble,0,1},
      {"_cal_fv_test", &fvTest, kInt,0,1},
      {"_cal_fv_delta", &fvDelta, kDouble,0,1},
      {"_cal_ADCMode", &fADCMode, kInt, 0, 1},
      {"_cal_adc_tdc_offset", &fAdcTdcOffset, kDouble, 0, 1},
      {"_dbg_raw_cal", &fdbg_raw_cal, kInt, 0, 1},
      {"_dbg_decoded_cal", &fdbg_decoded_cal, kInt, 0, 1},
      {"_dbg_sparsified_cal", &fdbg_sparsified_cal, kInt, 0, 1},
      {"_dbg_clusters_cal", &fdbg_clusters_cal, kInt, 0, 1},
      {"_dbg_tracks_cal", &fdbg_tracks_cal, kInt, 0, 1},
      {"_dbg_init_cal", &fdbg_init_cal, kInt, 0, 1},
      {0}
    };
    fClustMethod = 0;           //Set default clustering method to HMS/SHMS original 
    fMakeGrid = 0;              //Default if not defined
    fvTest = 0;			// Default if not defined
    fdbg_raw_cal = 0;		// Debugging off by default
    fdbg_decoded_cal = 0;
    fdbg_sparsified_cal = 0;
    fdbg_clusters_cal = 0;
    fdbg_tracks_cal = 0;
    fdbg_init_cal = 0;
    fAdcTdcOffset=0.0;
    fADCMode=kADCDynamicPedestal;
    gHcParms->LoadParmValues((DBRequest*)&list, fKwPrefix.c_str());
  }

  // Debug output.
  if (fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcNPSCalorimeter::ReadDatabase for "
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

  //C.Y. Feb 22, 2021: Define output .csv file to write grid
  if(fMakeGrid){
    fOutFile = new ofstream;

    
    //Write header if using Standard HMS/SHMS Algorithm
    //(developed by Tsolak Amatuni some time before 1995, and re-written by Vardan T. in ROOT C++)
    if(fClustMethod == 0){
      
      //fOutFile->open("SHMS_Standard_Cal_Grid.csv");
      cout << "Clustering Algorithm:  HCANA STANDARD " << endl;
      if(fCalReplicate==0) { fOutFile->open("SHMS_Standard_Cal_Grid.csv"); }
      else if(fCalReplicate>0) { fOutFile->open("NPS_Standard_Cal_Grid.csv"); }
      //Write header information 

      *fOutFile << "#! evNum[i,0]/, \t Nclusters[i,1]/, \t iCluster[i,2]/, \t BlockID[i,3]/, \t fT[f,4]/, \t fPI[f,5]/, \t fE[f,6]/" << endl;

    }
    
    //Write header if using Cellular Automata Clustering Algorithm (Same approach used in Hall A DVCS)
    if(fClustMethod == 1){
      
      //fOutFile->open("SHMS_CellularAutomata_Cal_Grid.csv");
      cout << "Clustering Algorithm:  CELLULAR AUTOMATA " << endl;
      if(fCalReplicate==0) { fOutFile->open("SHMS_CellularAutomata_Cal_Grid.csv"); }
      else if(fCalReplicate>0) { fOutFile->open("NPS_CellularAutomata_Cal_Grid.csv"); }
      //Write header information 
      *fOutFile << "#! evNum[i,0]/, \t Nclusters[i,1]/, \t iCluster[i,2]/, \t BlockID[i,3]/, \t fT[f,4]/, \t fPI[f,5]/, \t fE[f,6]/" << endl;

    }


    
  }
  
  /*---- C.Y. I think these can be removed (check with S. Wood) 
  {
    DBRequest list[]={
      {"_cal_a_cor", &fAcor, kDouble},
      {"_cal_b_cor", &fBcor, kDouble},
      {"_cal_c_cor", fCcor, kDouble, 2},
      {"_cal_d_cor", fDcor, kDouble, 2},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list, fKwPrefix.c_str());
  }

  // Debug output.
  if (fdbg_init_cal) {
    cout << "  Coordinate correction constants:\n";
    cout << "    fAcor = " << fAcor << endl;
    cout << "    fBcor = " << fBcor << endl;
    cout << "    fCcor = " << fCcor[0] << ", " << fCcor[1] << endl;
    cout << "    fDcor = " << fDcor[0] << ", " << fDcor[1] << endl;
  }
  -------------------*/

  //  BlockThick = new Double_t [fNLayers];
  //  fNBlocks = new UInt_t [fNLayers];
  //  fLayerZPos = new Double_t [fNLayers];
  //  fYPos = new Double_t [2*fNLayers];

  //Caution! Z positions (fronts) are off in hcal.param! Correct later on.

  // Fiducial volume limits, based on Plane 1 positions.
  //

  //NPS base this on array dimensions
  //Are these only needed if we have tracking detectors in front?

  //Calibration related parameters (from h(s)cal.param).

  DBRequest list[]={
    {"_cal_min_peds", &fShMinPeds, kInt,0,1},
    {0}
  };
  fShMinPeds=0.;

  gHcParms->LoadParmValues((DBRequest*)&list, fKwPrefix.c_str());

  // Detector axes. Assume no rotation.
  //
  DefineAxes(0.);

  fIsInit = true;

  return kOK;
}


//_____________________________________________________________________________
Int_t THcNPSCalorimeter::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  // cout<< "Calling THcNPSCalorimeter::DefineVariables() . . ." << endl;

  
  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "nhits", "Number of hits",                                 "fNhits" },
    { "nclust", "Number of layer clusters",                            "fNclust" },
    { "etot", "Total energy",                                    "fEtot" },
    { "etrack", "Track energy",                                  "fEtrack" },
    { "eprtrack", "Track Preshower energy",                      "fEPRtrack" },
    { "eprtracknorm", "Preshower energy divided by track momentum", "fEPRtrackNorm" },
    { "etottracknorm", "Total energy divided by track momentum", "fETotTrackNorm" },
    //{ "ntracks", "Number of shower tracks",                      "fNtracks" }, //C.Y. Jan 13, 2021, comment out to prevent replay errors for now.
    { 0 }
  };

  if(fHasArray) {

  RVarDef array_vars[] = {

    //---C.Y. Jan 13, 2021:  Commented out for now to avoid replay errors as some of these variable are NOT defined in the header file
    //(The NPS tracks to match a cluster needs to be given some thought since there is no tracking detector in NPS, so tracks
    //are simply reconstructed from clusters in NPS calorimeter) : See THcShower.cxx(.h) for info on use of these variables
    //{ "sizeclustarray", "Number of block in array cluster that matches track", "fSizeClustArray" },
    //{ "nclustarraytrack", "Number of cluster for Fly's eye which matched best track","fNclustArrayTrack" },
    //{ "nblock_high_ene", "Block number in array with highest energy which matched best track","fNblockHighEnergy" },
    //{ "xclustarraytrack", "X pos of cluster which matched best track","fXclustArrayTrack" },
    //{ "xtrackarray", "X pos of track which matched cluster", "fXTrackArray" },
    //{ "yclustarraytrack", "Y pos of cluster which matched best track","fYclustArrayTrack" },
    //{ "ytrackarray", "Y pos of track which matched cluster", "fYTrackArray" },
    { 0 }
  };

  DefineVarsFromList( array_vars, mode );
  }
  return DefineVarsFromList( vars, mode );

}

//_____________________________________________________________________________
THcNPSCalorimeter::~THcNPSCalorimeter()
{
  // Destructor. Remove variables from global list.
  // cout<< "Calling Destructor | THcNPSCalorimeter::~THcNPSCalorimeter() . . ." << endl;

  Clear();
  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();

  for (THcNPSShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    Podd::DeleteContainer(**i);
    delete *i;
  }
  delete fClusterList; fClusterList = 0;
  delete fArray; fArray = 0;

  //close output file 
  if(fMakeGrid){
    fOutFile->close();
  }

}

//_____________________________________________________________________________
void THcNPSCalorimeter::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  for (UInt_t i = 0; i<fNTotLayers; ++i)
    delete [] fLayerNames[i];
  delete [] fLayerNames; fLayerNames = 0;

}

//_____________________________________________________________________________
inline
void THcNPSCalorimeter::Clear(Option_t* opt)
{

//   Reset per-event data.

  if(fHasArray) {
    fArray->Clear(opt);
  }

  fNhits = 0;
  fNclust = 0;
  fEtot = 0.;
  fEtotNorm = 0.;
  fEtrack = 0.;
  fEtrackNorm = 0.;
  fEPRtrack = 0.;
  fEPRtrackNorm = 0.;
  fETotTrackNorm = 0.;
  fNblockHighEnergy = 0.;

  // Purge cluster list

  for (THcNPSShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    Podd::DeleteContainer(**i);
    delete *i;
  }
  fClusterList->clear();
}

//_____________________________________________________________________________
Int_t THcNPSCalorimeter::Decode( const THaEvData& evdata )
{

  //cout << "--------------------> New Event Number: " << evdata.GetEvNum() << endl;
  Bool_t clear;

  // DJH 09/04/22 -- only create dedicated analyzer object if using SHMS data
  if(fCalReplicate==0)
    clear = fAnalyzer->GetClearThisEvent();

  if(clear) Clear();
  // Should move this up to the apparatus level.

  // Get the Hall C style hitlist (fRawHitList) for this event
  Bool_t present = kTRUE;	// Suppress reference time warnings
  if(fPresentP) {		// if this spectrometer not part of trigger
    present = *fPresentP;
  }
  Int_t nhits = DecodeToHitList(evdata, !present);

  //cout<< "total number of blocks hit = " << nhits << endl;
  fEvent = evdata.GetEvNum();

  if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    if(fHasArray) {
      nexthit = fArray->AccumulatePedestals(fRawHitList, nexthit);
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }

  if(fAnalyzePedestals) {
    if(fHasArray) {
      fArray->CalculatePedestals();
    }
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t nexthit = 0;
  if(fHasArray) {
    if(clear) fArray->ClearProcessedHits();
    quadrant = -1;
    if(fCalReplicate>0) {
      quadrant = gRandom->Integer(4) + (fCalReplicate>1?4:0);
    }
    // cout<< "----> quadrant = " << quadrant << endl;
    nexthit = fArray->AccumulateHits(fRawHitList, nexthit, quadrant);  
  }
  //  cout << "Nhits=" << nhits << endl;
  return nhits;
}

//_____________________________________________________________________________
Int_t THcNPSCalorimeter::CoarseProcess(TClonesArray& tracks )
{

  // cout<< "Calling THcNPSCalorimeter::CoarseProcess() . . . " << endl;

  // Calculation of coordinates of particle track cross point with shower
  // plane in the detector coordinate system. For this, parameters of track
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.

  // Clustering of hits.
  //

  // Fill set of unclustered hits.
  if(fHasArray) {
    fArray->CoarseProcessHits();
    //C.Y. Feb 07 2021: I think there should NOT be a +=, since fArray->GetEarray() already returns the total deposited energy
    //for some reason, fEtot is slighly different from fArray in the ROOTfile, where they should actually be exactly the same
    fEtot += fArray->GetEarray();  
  }
  //THcHallCSpectrometer *app = static_cast<THcHallCSpectrometer*>(GetApparatus());
  //  fEtotNorm=fEtot/(app->GetPcentral());
  fEtotNorm = 1.0;
  //
  
  // Do same for Array.
  
  //C.Y. Feb 07 2021: The tracks input on this method is irrelevant as NPS doe NOT use tracks (should eventually be taken out)
  if(fHasArray) fArray->CoarseProcess(tracks);

  //
  return 0;
}

//-----------------------------------------------------------------------------

void THcNPSCalorimeter::ClusterHits(THcNPSShowerHitSet& HitSet,
			    THcNPSShowerClusterList* ClusterList) {

  //cout<< "Calling THcNPSCalorimeter::ClusterHits() . . . " << endl;
    
  // Collect hits from the HitSet into the clusters. The resultant clusters
  // of hits are saved in the ClusterList.

  //cout << "HitSet Size = " <<  HitSet.size() << endl;
  while (HitSet.size() != 0) {

    THcNPSShowerCluster* cluster = new THcNPSShowerCluster;

    THcNPSShowerHitIt it = HitSet.end();
    (*cluster).insert(*(--it));   //Move the last hit from the hit list
    HitSet.erase(it);             //into the 1st cluster

    bool clustered = true;

    while (clustered) {                   //Proceed while a hit is clustered

      clustered = false;

      for (THcNPSShowerHitIt i=HitSet.begin(); i!=HitSet.end(); ++i) {

	for (THcNPSShowerClusterIt k=(*cluster).begin(); k!=(*cluster).end();
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

  } //While hit_list not exhausted

  
  // C.Y. Apr 07, 2021: Added lines to write clusters to file for plotting in 2D grid (and comparing to NPS clustering approach)
  if(fMakeGrid) {

    //cout << "ClusterList Size = " << ClusterList->size() << endl;
    
    //Loop over ALL clusters per event
    for (int i=0; i<ClusterList->size(); i++){
      
      //Loop over ALL hit blocks per cluster per event
      for(THcNPSShowerClusterIt j=(*ClusterList->at(i)).begin(); j!=(*ClusterList->at(i)).end(); ++j){
		
	*fOutFile << "\t" << fEvent << ",\t\t\t" << ClusterList->size() << ",\t\t\t" << (i+1) << ",\t\t\t" << (*j)->hitID() << ",\t\t\t" << (*j)->hitT() << ",\t\t\t" << (*j)->hitPI() << ",\t\t\t" << (*j)->hitE() <<endl;
	
	
      } //end loop over hit blocks per cluster
      
    } //end loop over clusters

  } //end fMakeGrid requirement
  
   
};

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void THcNPSCalorimeter::ClusterNPS_Hits(THcNPSShowerHitSet& HitSet, THcNPSShowerClusterList* ClusterList) {

  /*
    C.Y. Feb 09, 2021
    Brief: This method does the Neutral Particle Spectrometer (NPS) clustering following
    the Cellular Automata approach (https://inis.iaea.org/collection/NCLCollectionStore/_Public/29/006/29006611.pdf?r=1&r=1). 
    In addition, as was done in Hall A DVCS, 3D clustering is done by selecting block adc 
    pulse hits that fall within a certain adc pulse time window (timemin, timemax) and raw pulse integral that pass a threshold
    
    C.Y. Feb 21, 2021
    ** TO-DO NOTE ** : Currently, to select the best possible hit (if a block has multiple adc hits per event) is to loop over all raw 
    hits per block, and select the hit that is closest to the adc pulse time average (timemin+timemax)/2. The issue is that the loop
    over raw hits is already done in AccumulateHits(), and it is best practive NOT to repeat the same loop (memory efficiency). Eventually,
    it is best to determine the best adc hit per block within the AccumulateHits() method itself, and pass that information to this method. 
    In addition, the selection of the best hit in a block must also be thought more carefully, as the following scenario is possible:
    "Imagine a cluster of hits on blocks 2, 3 and 4, all at 20 ns.  Then another cluster of hits with blocks 4, 5 and 6, all at 40 ns.  
    We want to make sure that both hits on block 4 are used. " --S.A. Wood
    
    **NOTE 2:  Recall, for NPS, the fADC Dynamic Range (variable name: fAdcRange) will be set to 2 V. Currently, in THcRawAdcHit.cxx(.h), 
    a dynamic range of 1 V is used (for HMS/SHMS fADC). Will need to add an option for 2V if analyzing NPS.

    C.Y. Mar 28, 2021
    How is the set of pulse Integral hits (fNPSPulseInt)  constructed? 
    The fNPSPulseInt is a vector of total number of blocks, where each block, contains a vector of hits for that particular block per event.
    This vector of vectors variable is filled in the THcNPSArray.cxx class under the THcNPSArray::FillADC_DynamicPedestal()
    method, given that certain conditions are fulfilled (i.e., errorflag==0, hit is within adctdcdiffTime window, and good
    adc pulse integral raw is above threshold).


    C.Y. Apr 04, 2021 : Event 987 of Run 10398 is a good event to check code,as it only has a single cluster of 4 hits 
    and can be used to understand how the algorithm contaminates other blocks based on a central virus block.
  */

    
  //********************************************
  // PHASE 1 PREP: ARRAY/VECTORS INITIALIZATION
  //********************************************
  
  //total number of blocks
  Int_t fNelem = fNRows * fNColumns;
  Int_t fNbBlocks = 0; //counter of number of blocks hit

  //Define vectors to store hit blocks info
  vector<Double_t> blk_pulseInt(fNelem, -1);        //store max pulse integral for ALL blocks (by default, is -1 for all elements)
  vector<Int_t>    good_blk_id;                     //store block id that was hit (variable-size), specifies (index hit, block_id hit)
  vector<Double_t> good_blk_pulseInt;               //store max pulse integral per block hit (if there are multiple hits) (index hit, max pulse Int)

  //clusters counter
  Int_t NbClusters=0;

  //Hit Block indexing (e.g, for a given event, if hit blocks are: 34, 65, 78, then corresponding indices are: = 0, 1, 2)
  Int_t blk_hit_idx[fNelem];

  //initialize arrays
  for(int ielem=0; ielem<fNelem; ielem++){
    blk_hit_idx[ielem]=-1;
  }
  
  
  //---------TESTING: ASSIGN PULSE INT USING HIT SET-------
  //Loop over ALL Hit blocks
  for (THcNPSShowerHitIt ihit=HitSet.begin(); ihit!=HitSet.end(); ++ihit) {

    //fill  vectors
    blk_pulseInt[ (*ihit)->hitID() ]    =  (*ihit)->hitPI();
    good_blk_id.push_back( (*ihit)->hitID() );
    good_blk_pulseInt.push_back( (*ihit)->hitPI() );
    
    //Set hit block index
    blk_hit_idx[ (*ihit)->hitID() ] = fNbBlocks;
    
    //increment hit block counter
    fNbBlocks++;
    
  }
  
  //Boolean arrays to "tag" blocks of interest as viruses when forming clusters 
  Bool_t virus[fNbBlocks], virustmp[fNbBlocks], ill[fNbBlocks];  

  //********************************
  // PHASE 1: VIRUS IDENTIFICATION
  //********************************
  
  //Loop over ONLY NPS blocks that have a good hit to identify which of the blocks hit
  //are a local maxima (known as the virus in cellular automata method, and have the
  //highest pulse signal compared to its nearest neighbors)
  // cout << "" << endl;
  // cout <<  "********************************\n"
  // "PHASE 1: VIRUS IDENTIFICATION\n"
  // "********************************" << endl;
    
  for(UInt_t j=0; j < fNbBlocks; j++){ 

    //cout << "--------CENTRAL J-th Block------ " << endl;
    //cout << "(j-index, Block ID, pulseInt) = " << j << ", " << good_blk_id[j] << ", " << good_blk_pulseInt[j] << endl;   
    //cout << "-------------------------------- " << endl;
    //By default, each j-th block that has a good hit is hit is considered a 
    //virus (highest pulse integral compared to nearest neighbors)
    ill[j]     = false;
    virus[j]   = true;    
    virustmp[j]= true;

    //For each good 'j-th' block hit, Loop over each of the (at most) 8 neighboring blocks (or cells)
    //If at a corner (only 3 neighboring blocks) and if at an edge (only 5 neighboring blocks)
    for(Int_t k=0;k<8;k++){

      // cout<< "(k-index, Neighbor Block ID) = " << k << ", " <<  fArray->GetNeighbor(good_blk_id[j], k)  << endl; 

      //Check if kth neighbor block exists (is physically real) and  was actually hit (has good pulse integral)
      //if( fArray->GetNeighbor(good_blk_id[j], k)!=-1  && blk_pulseInt[ fArray->GetNeighbor(good_blk_id[j], k) ]>0 ){
       if( fArray->GetNeighbor(good_blk_id[j], k)!=-1  && blk_hit_idx[ fArray->GetNeighbor(good_blk_id[j], k) ]!=-1 ){

	 // cout<< "Valid Neighbor Found : (k-index, Neighbor Block ID, pulseInt) = " << k << ", " <<  fArray->GetNeighbor(good_blk_id[j], k)  << ", " << blk_pulseInt[ fArray->GetNeighbor(good_blk_id[j], k) ] << endl; 

	//Check if the kth neighbor block good pulse integral is greater than the ith central block (if so, then the central block is NOT a virus)
	 //if( blk_pulseInt[ fArray->GetNeighbor(good_blk_id[j], k) ] > blk_pulseInt[ good_blk_id[j] ] ){
	 if( blk_pulseInt[ fArray->GetNeighbor(good_blk_id[j], k) ] > good_blk_pulseInt[j] ){
	   //cout<< " k-th Neighbor Pulse Integral > j-th Central Block Pulse Integral | Set 'virus[j]=false' for central block !" << endl;
	  //set to false if ith central block is NOT a virus (If there's one neighbour with more energy, it's not a local maximum)
	  virus[j]   = false;
	  virustmp[j]= false;
	 }
	
      }

    }//end loop over k neighbors
    
    //If none of the k neighbors pulse integral is greater than the central block, then
    //the central block will remain a virus (true) and the number of clusters is incremented 
    if (virus[j]==true) { NbClusters++; }
     
  }//end loop over hit fNbBlocks

  
  //********************************************
  // PHASE 2 PREP: ARRAY/VECTORS INITIALIZATION
  //********************************************

  
  //Float arrays to pulse integrals during the "contamination phase" of the algorithm
  Float_t pIpas[fNelem], pIpastmp[fNelem], pInei[8];

  //pulseInt Array of number of viruses (NbClusters) identified above
  Float_t pIvirus[NbClusters];
  Int_t cp = 0;   //virus counter

  //cout << "-------------------" << endl;
  //cout << "Check Virus Blocks " << endl;
  //cout << "-------------------" << endl;
  //Loop over all blocks that have adc hit and check if they are a virus.
  //If they are a virus, assign the corresponding pulseInt to pIvirus
  for(Int_t j=0; j<fNbBlocks; j++){

    //check if 'jth' hit block is a virus
    if(virus[j]==true){
      //cout << "(Virus Block ID, Pulse Int) = " << good_blk_id[j] << ", " << good_blk_pulseInt[j] << endl;
      pIvirus[cp] = good_blk_pulseInt[j]; //Virus' energies
      cp++;
      
    }
  }

  //Loop over ALL calorimeter blocks to initialize pulse Int arrays to be used in contamination phase
  for(Int_t ielem = 0; ielem < fNelem; ielem++){

    //assign initial values to arrays
    pIpas[ielem]    = 0.;
    pIpastmp[ielem] =-1.;
  }
  

  //Loop over all blocks that had a hit
  for(Int_t j=0; j<fNbBlocks; j++){

    //assign pulse integral to pIpas
    pIpas[ good_blk_id[j] ] = good_blk_pulseInt[j]; 

    //If the 'jth' block was identified as a virus previously, then set ill[j] = true, and assign a temporary pulseInt pIpastmp
    //corresponding to the virus pulse integral.  The virus block will be identified as already ill in the contamination phase.
    if(virus[j]) {

      ill[j] = true;
      pIpastmp[ good_blk_id[j] ] = good_blk_pulseInt[j];

    }
  }

  //********************************
  // PHASE 2: VIRUS CONTAMINATION
  //********************************

  //cout << "" << endl;
  //cout <<  "********************************\n"
  //  "PHASE 2: VIRUS CONTAMINATION\n"
  //  "********************************" << endl;

  
  //Contamination: Now that the virus cells are identified, its neighbouring cells must
  //now be contaminated (i.e., become ill), and must somehow be tagged with the virus cell
  //until all cells contaminated by a virus become ill


  //Set starting values for the iterative contamination
  Int_t safe=0, nbfine=1, virus_blk=-2;

  //Initialize iterations, with an upper limit of 15 iterations
  while(nbfine>0  && safe<15){
    
    //increment safe iterator
    safe++;

    //cout << "---> Begin Safe Iteration Number =  " << safe << endl; 
     //Loop over blocks that have a hit
    for(Int_t j=0; j<fNbBlocks; j++){ 

      //cout << "(j-th index, Block ID Hit, virus?, ill? ) = " << j << ", " << good_blk_id[j] << ", " << virus[j] << ", " << ill[j] << endl;
      //----BEGIN 'ILL BLOCK' CHECK-----  

      /*Check if block has been contaminated (ill). The virus blocks have
	already been set to ill=true previously. The neighboring blocks,
	however, have not yet been determined to be ill, so they must be 
	identified and contaminated*/
      
      if(!ill[j]){   //if hit block has not been contaminated (i.e., not a virus and therefore, not ill) . . .

	//cout << "NON-ILL BLOCK FOUND | j-th index, Non-Ill Block ID = " << j << ", " << good_blk_id[j] << endl;
	//variables to store pulse int and block id of neighbor with higher pulseInt
	Float_t max=0.;
	Int_t nei[8];
	
	//Loop over (at most 8) neighboring blocks
	for(Int_t k=0; k<8; k++){

	  //cout<< "(k-index, Neighbor Block ID) = " << k << ", " <<  fArray->GetNeighbor(good_blk_id[j], k)  << endl;
	  
	  //Check if kth neighbor block exists (is physically real) and  was actually hit (has good pulse integral)
	  if( fArray->GetNeighbor(good_blk_id[j], k)!=-1  && blk_hit_idx[ fArray->GetNeighbor(good_blk_id[j], k) ]!=-1 ){

	    //cout<< "Valid Neighbor Found : (k-index, Neighbor Block ID, pulseInt) = " << k << ", " <<  fArray->GetNeighbor(good_blk_id[j], k)  << ", " << blk_pulseInt[ fArray->GetNeighbor(good_blk_id[j], k) ] << endl; 

	    //Set pulseInt of kth neighbour block
	    pInei[k] = pIpas[ fArray->GetNeighbor(good_blk_id[j], k) ];
	  }
	  //If kth neighbor does not exist or has no hit, set to zero
	  else{ 
	    pInei[k] = 0.;
	  }

	  //--------

	  //Check if kth neighbor block exists (is physically real)
	  if( fArray->GetNeighbor(good_blk_id[j], k)!=-1 ){
	    //assign a hit index for the 'kth' the neighbor of the 'jth' block
	    nei[k] = blk_hit_idx[ fArray->GetNeighbor(good_blk_id[j], k) ];
	  }
	  else{
	    nei[k] = -1;
	  }

	}//end loop over (at most 8) neighboring blocks
	
	//get the highest neighboring cell block hit index (virus_blk) and corresponding pulseInt (max)
	fArray->GetMax(pInei, nei, virus_blk, max);  
	//Set temporaty pulse Int container to the highest pulse Int of the neighbor block.
	//Here is where the j-th hit block (that was not ill) is tagged' with the neighboring block highest pulseInt
	pIpastmp[ good_blk_id[j] ]=max;  

	// cout << "Neighbor w/ Highest Pulse Int | (Block Hit Index, PulseInt) =  " << virus_blk << ", " << max << endl;
	//Check if the found neighboring block (virus_blk) with the highest pulse Integral has been previously tagged as a virus 
	if(virus[virus_blk]){

	  //cout << "Neighbor w/ Highest Pulse Int is a VIRUS ! ! ! " << endl;
	  //cout << "Set j-th block index and virustmp[j] to TRUE (Contaminate j-th block) | (j-th index, Block ID) = " << j << ", " <<  good_blk_id[j] << endl;

	  //If the neighboring 'kth' block has been tagged as a virus, then the central 'jth' block will become ill (contaminated) with the virus
	  ill[j]      = true;
	  virustmp[j] = true;
	  
	}
	
      }//end (!ill[j] check)


      //----END 'ILL BLOCK' CHECK-----
      
    } //end loop over hit blocks (fNbBlocks)

    //Update arrays before the next iteration, after looping over all hits blocks (fNbBlocks)
    for(Int_t i=0; i<fNelem; i++){

      //Update virus flag of the ith hit index (note in this context, i is a hit index, not a block number) 
      if(i<fNbBlocks) { virus[i] = virustmp[i]; }

      //check if ith element has a hit (pulseInt!=-1)
      if(pIpastmp[i]!=-1){
	//assign pulse Int array the updated pulse Int for the ith block id
	pIpas[i]=pIpastmp[i];
      }
	
    }
    
    //Test for still ill blocks. Otherwise iteration is complete !
    nbfine=0;  //set to zero (no next iteration) unless proven otherwise
    
    for(Int_t i=0; i<fNbBlocks; i++){

      //If the ith hit index block is not ill, then iterative process must continue untill all blocks are ill (contaminated)
      //or an upper safety limit is reached

      if(!ill[i]) {
  	nbfine++;  //increment nbfine (so that nbfine>0 to do next iteration) 
      }

    }

  }//end iterative loop


  //***********************************************************
  //
  // ADD CLUSTERS (CORRELATED GROUP OF HIT BLOCKS) TO A LIST
  //
  //***********************************************************
  
  //Check if HitSet (containing hit objects of blocks that were hit)
  while (HitSet.size() != 0) {

    //initialize a set called 'cluster' to stored clustered hits
    THcNPSShowerCluster* cluster = new THcNPSShowerCluster;

    
    THcNPSShowerHitIt it = HitSet.end();  //Get last hit in the set
    (*cluster).insert(*(--it));   //Move the last hit from the hit set into the 1st cluster (as starting point)
    HitSet.erase(it);             //Erase the hit from the HitSet (to keep track) of which hits have been added to cluster 
    
    bool clustered = true;
    
    while (clustered) {                   //Proceed while a hit is clustered
      
      clustered = false;

      //Iterate over unclustered set of hits
      for (THcNPSShowerHitIt i=HitSet.begin(); i!=HitSet.end(); ++i) {

	//Iterate over clustered set
	for (THcNPSShowerClusterIt k=(*cluster).begin(); k!=(*cluster).end();
	     ++k) {

	  //cout << "TESTING 1: (**i)->hitID = " << (*i)->hitID() << endl;
	  //cout << "TESTING 2: (*k)->hitID = " << (*k)->hitID() << endl;
	  if(pIpas[ (*i)->hitID() ] == pIpas[ (*k)->hitID() ]){
	    //	  if ((**i).isNeighbour(*k)) {
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
    
  }


  
  // C.Y. Apr 07, 2021: Added lines to write clusters to file for plotting in 2D grid (and comparing to NPS clustering approach)
  if(fMakeGrid) {

    //cout << "fEvent = " << fEvent << endl;
    //cout << "ClusterList Size = " << ClusterList->size() << endl;
    
    //Loop over ALL clusters per event
    for (int i=0; i<ClusterList->size(); i++){
      
      //cout << "----> Cluster i = " << i << endl;
      //cout << "# of hits in cluster = " << (*ClusterList->at(i)).size() << endl;
      
      //Loop over ALL hit blocks per cluster per event
      for(THcNPSShowerClusterIt j=(*ClusterList->at(i)).begin(); j!=(*ClusterList->at(i)).end(); ++j){
	
	//cout << "blockID = " << (*j)->hitID() << endl;
	//cout << "blockE = " << (*j)->hitE() << endl;
	//cout << "blockT = " << (*j)->hitT() << endl;
	
	*fOutFile << "\t" << fEvent << ",\t\t\t" << ClusterList->size() << ",\t\t\t" << (i+1) << ",\t\t\t" << (*j)->hitID() << ",\t\t\t" << (*j)->hitT() << ",\t\t\t" << (*j)->hitPI() << ",\t\t\t" << (*j)->hitE() << endl;
	
	
      } //end loop over hit blocks per cluster
      
    } //end loop over clusters
    
  } //end fMakeGrid requirement
  
 
}
//-----------------------------------------------------------------------------




// Various helper functions to accumulate hit related quantities.

Double_t addE(Double_t x, THcNPSShowerHit* h) {
  return x + h->hitE();
}

Double_t addX(Double_t x, THcNPSShowerHit* h) {
  return x + h->hitE() * h->hitX();
}

Double_t addY(Double_t x, THcNPSShowerHit* h) {
  return x + h->hitE() * h->hitY();
}

Double_t addZ(Double_t x, THcNPSShowerHit* h) {
  return x + h->hitE() * h->hitZ();
}

Double_t addEpr(Double_t x, THcNPSShowerHit* h) {
  return h->hitColumn() == 0 ? x + h->hitE() : x;
}



// Y coordinate of center of gravity of cluster, calculated as hit energy
// weighted average. Put X out of the calorimeter (-100 cm), if there is no
// energy deposition in the cluster.
//
Double_t clY(THcNPSShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addY)/Etot : -100.);
}
// X coordinate of center of gravity of cluster, calculated as hit energy
// weighted average. Put X out of the calorimeter (-100 cm), if there is no
// energy deposition in the cluster.
//
Double_t clX(THcNPSShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addX)/Etot : -100.);
}

// Z coordinate of center of gravity of cluster, calculated as a hit energy
// weighted average. Put Z out of the calorimeter (0 cm), if there is no energy
// deposition in the cluster.
//
Double_t clZ(THcNPSShowerCluster* cluster) {
  Double_t Etot = accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
  return (Etot != 0. ?
	  accumulate((*cluster).begin(),(*cluster).end(),0.,addZ)/Etot : 0.);
}

//Energy depostion in a cluster
//
Double_t clE(THcNPSShowerCluster* cluster) {
    return accumulate((*cluster).begin(),(*cluster).end(),0.,addE);
}

//Energy deposition in the Preshower (1st plane) for a cluster
//
Double_t clEpr(THcNPSShowerCluster* cluster) {
    return accumulate((*cluster).begin(),(*cluster).end(),0.,addEpr);
}

//Cluster energy deposition in plane iplane=0,..,3:
// side=0 -- from positive PMTs only;
// side=1 -- from negative PMTs only;
// side=2 -- from postive and negative PMTs.
//

Double_t clEplane(THcNPSShowerCluster* cluster, Int_t iplane, Int_t side) {

  if (side!=0&&side!=1&&side!=2) {
    cout << "*** Wrong Side in clEplane:" << side << " ***" << endl;
    return -1;
  }

  THcNPSShowerHitSet pcluster;
  for (THcNPSShowerHitIt it=(*cluster).begin(); it!=(*cluster).end(); ++it) {
    if ((*it)->hitColumn() == iplane) pcluster.insert(*it);
  }

  Double_t Eplane;
  switch (side) {
    /*
      case 0 :
      Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addEpos);
      break;
      case 1 :
      Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addEneg);
      break;
    */
  case 2 :
    Eplane = accumulate(pcluster.begin(),pcluster.end(),0.,addE);
    break;
    
  default :
    Eplane = 0. ;
  }

  return Eplane;
}

//_____________________________________________________________________________
Int_t THcNPSCalorimeter::FineProcess( TClonesArray& tracks )
{

  //cout << "Calling THcNPSCalorimeter::FineProcess() . . . " << endl;

  // Shower energy assignment to the spectrometer tracks.
  //

  
  //Debug output.

  return 0;
}

Double_t THcNPSCalorimeter::GetNormETot( ){
  return fEtotNorm;
}
//_____________________________________________________________________________
Int_t THcNPSCalorimeter::End(THaRunBase* run)
{
  // cout<< "Calling THcNPSCalorimeter::End() . . . " << endl;

  MissReport(Form("%s.%s", GetApparatus()->GetName(), GetName()));
  return 0;
}

ClassImp(THcNPSCalorimeter)
////////////////////////////////////////////////////////////////////////////////
