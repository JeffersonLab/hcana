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
  Setup(name, description);
//  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
}

//_____________________________________________________________________________
THcShower::THcShower( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

void THcShower::Setup(const char* name, const char* description)
{

  static const char* const here = "Setup()";
  static const char* const message = 
    "Must construct %s detector with valid name! Object construction failed.";

  // Base class constructor failed?
  if( IsZombie()) return;

  fNLayers = 4;	// Eventually get # layers and layer names from a DB
  fLayerNames = new char* [fNLayers];
  for(Int_t i=0;i<fNLayers;i++) {fLayerNames[i] = new char[3];}
  strcpy(fLayerNames[0],"1z");
  strcpy(fLayerNames[1],"2z");
  strcpy(fLayerNames[2],"3z");
  strcpy(fLayerNames[3],"4z");

  size_t nlen = strlen(name);
  size_t slen = 0;
  for(Int_t i=0;i < fNLayers;i++)
    {slen = TMath::Max(slen,strlen(fLayerNames[i]));}
  size_t len = nlen+slen+1;

  // Probably shouldn't assume that description is defined
  char* desc = new char[strlen(description)+50+slen];
  fPlanes = new THcShowerPlane* [fNLayers];
  for(Int_t i=0;i < fNLayers;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fLayerNames[i]);

    fPlanes[i] = new THcShowerPlane(fLayerNames[i], desc, i+1, this); 
    cout << "Created Shower Plane " << fLayerNames[i] << ", " << desc << endl;
  }
}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShower::Init( const TDatime& date )
{
  static const char* const here = "Init()";

  cout << "THcShower::Init " << GetName() << endl;

  if( THaNonTrackingDetector::Init( date ) )
    return fStatus;


  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  THcHitList::InitHitList(fDetMap, "THcShowerHit", 100);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  for(Int_t ip=0;ip<fNLayers;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }
  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HCAL") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HCAL");
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

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // We will probably want to add some kind of method to gHcParms to allow
  // bulk retrieval of parameters of interest.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

cout << "THcShower::ReadDatabase called " << GetName() << endl;

  fNLayers = 4;			// Hardwire for now

  BlockThick = new Double_t [fNLayers];

  BlockThick[0] = *(Double_t *)gHcParms->Find("hcal_1pr_thick")->GetValuePointer();
  BlockThick[1] = *(Double_t *)gHcParms->Find("hcal_2ta_thick")->GetValuePointer();
  BlockThick[2] = *(Double_t *)gHcParms->Find("hcal_3ta_thick")->GetValuePointer();
  BlockThick[3] = *(Double_t *)gHcParms->Find("hcal_4ta_thick")->GetValuePointer();

cout << "Block thickness: " << BlockThick[2]  << endl;

fNBlocks = new Int_t [fNLayers];

  fNBlocks[0] = *(Int_t *)gHcParms->Find("hcal_1pr_nr")->GetValuePointer();
  fNBlocks[1] = *(Int_t *)gHcParms->Find("hcal_2ta_nr")->GetValuePointer();
  fNBlocks[2] = *(Int_t *)gHcParms->Find("hcal_3ta_nr")->GetValuePointer();
  fNBlocks[3] = *(Int_t *)gHcParms->Find("hcal_4ta_nr")->GetValuePointer();

  cout << "Number of blocks per layer: " << fNBlocks[2]  << endl;

  fNLayerZPos = new Double_t [fNLayers];

  fNLayerZPos[0] = *(Double_t *)gHcParms->Find("hcal_1pr_zpos")->GetValuePointer();
  fNLayerZPos[1] = *(Double_t *)gHcParms->Find("hcal_2ta_zpos")->GetValuePointer();
  fNLayerZPos[2] = *(Double_t *)gHcParms->Find("hcal_3ta_zpos")->GetValuePointer();
  fNLayerZPos[3] = *(Double_t *)gHcParms->Find("hcal_4ta_zpos")->GetValuePointer();

cout << "Z Position: " << fNLayerZPos[2]  << endl;

  XPos = new Double_t [2*fNLayers];

  XPos[0] = *(Double_t *)gHcParms->Find("hcal_1pr_left")->GetValuePointer();
  XPos[1] = *(Double_t *)gHcParms->Find("hcal_1pr_right")->GetValuePointer();
  XPos[2] = *(Double_t *)gHcParms->Find("hcal_2ta_left")->GetValuePointer();
  XPos[3] = *(Double_t *)gHcParms->Find("hcal_2ta_right")->GetValuePointer();
  XPos[4] = *(Double_t *)gHcParms->Find("hcal_3ta_left")->GetValuePointer();
  XPos[5] = *(Double_t *)gHcParms->Find("hcal_3ta_right")->GetValuePointer();
  XPos[6] = *(Double_t *)gHcParms->Find("hcal_4ta_left")->GetValuePointer();
  XPos[7] = *(Double_t *)gHcParms->Find("hcal_4ta_right")->GetValuePointer();

cout << "X Positions: " << XPos[0]  << ", " << XPos[1] << endl;

  YPos = new Double_t* [4];
  cout << "Y Positions:";

  Double_t* p;
  Int_t ilayer;

  ilayer = 0; 
  p = (Double_t *)gHcParms->Find("hcal_1pr_top")->GetValuePointer();
  YPos[ilayer] = new Double_t [fNBlocks[ilayer]];
  // Print out some parameters just to demonstrate that it works
  
  for(Int_t i=0;i<fNBlocks[ilayer];i++) {
    YPos[ilayer][i] = p[i];
    cout << " " << YPos[ilayer][i];
  }
  cout << endl;

  ilayer = 1; 
  p = (Double_t *)gHcParms->Find("hcal_2ta_top")->GetValuePointer();
  YPos[ilayer] = new Double_t [fNBlocks[ilayer]];
  
   for(Int_t i=0;i<fNBlocks[ilayer];i++) {
    YPos[ilayer][i] = p[i];
  }
 
  ilayer = 2; 
  p = (Double_t *)gHcParms->Find("hcal_3ta_top")->GetValuePointer();
  YPos[ilayer] = new Double_t [fNBlocks[ilayer]];

  for(Int_t i=0;i<fNBlocks[ilayer];i++) {
    YPos[ilayer][i] = p[i];
  }

  ilayer = 3; 
  p = (Double_t *)gHcParms->Find("hcal_4ta_top")->GetValuePointer();
  YPos[ilayer] = new Double_t [fNBlocks[ilayer]];

  for(Int_t i=0;i<fNBlocks[ilayer];i++) {
    YPos[ilayer][i] = p[i];
  }

  fIsInit = true;

  return kOK;
}



//_____________________________________________________________________________
Int_t THcShower::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  cout << "THcShower::DefineVariables called " << GetName() << endl;

  // Register variables in global list

 //  RVarDef vars[] = {
 //   { "nhit",   "Number of hits",                     "fNhits" },
 //   { "a",      "Raw ADC amplitudes",                 "fA" },
 //   { "a_p",    "Ped-subtracted ADC amplitudes",      "fA_p" },
 //   { "a_c",    "Calibrated ADC amplitudes",          "fA_c" },
 //   { "asum_p", "Sum of ped-subtracted ADCs",         "fAsum_p" },
 //   { "asum_c", "Sum of calibrated ADCs",             "fAsum_c" },
 //   { "nclust", "Number of clusters",                 "fNclust" },
 //   { "e",      "Energy (MeV) of largest cluster",    "fE" },
 //   { "x",      "x-position (cm) of largest cluster", "fX" },
 //   { "y",      "y-position (cm) of largest cluster", "fY" },
 //   { "mult",   "Multiplicity of largest cluster",    "fMult" },
 //   { "nblk",   "Numbers of blocks in main cluster",  "fNblk" },
 //   { "eblk",   "Energies of blocks in main cluster", "fEblk" },
 //   { "trx",    "track x-position in det plane",      "fTRX" },
 //   { "try",    "track y-position in det plane",      "fTRY" },
 //   { 0 }
 // };
 //  return DefineVarsFromList( vars, mode );
  return kOK;
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
  delete [] YPos;  YPos = NULL;
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
}

//_____________________________________________________________________________
Int_t THcShower::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNLayers;ip++) {
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
//cout << "nexthit = " << nexthit << endl;
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
/*
//   fRawHitList is TClones array of THcShowerHit objects
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcShowerHit* hit = (THcShowerHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_pos << " " << hit->fADC_neg << " "  << endl;
  }
  cout << endl;
*/
  return nhits;
}

//_____________________________________________________________________________
Int_t THcShower::ApplyCorrections( void )
{
  return(0);
}

//_____________________________________________________________________________
Double_t THcShower::TimeWalkCorrection(const Int_t& paddle,
					     const ESide side)
{
  return(0.0);
}

//_____________________________________________________________________________
Int_t THcShower::CoarseProcess( TClonesArray&  ) //tracks
{
  // Calculation of coordinates of particle track cross point with shower
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  //  static const Double_t sqrt2 = TMath::Sqrt(2.);
  
  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcShower::FineProcess( TClonesArray& tracks )
{
  // Reconstruct coordinates of particle track cross point with shower
  // plane, and copy the data into the following local data structure:
  //
  // Units of measurements are meters.

  // Calculation of coordinates of particle track cross point with shower
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::FineTrack() are used.

  return 0;
}

ClassImp(THcShower)
////////////////////////////////////////////////////////////////////////////////
 
