///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
// Class for a generic hodoscope consisting of multiple                      //
// planes with multiple paddles with phototubes on both ends.                //
// This differs from Hall A scintillator class in that it is the whole       //
// hodoscope array, not just one plane.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoscope.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THaGlobals.h"
#include "THaCutList.h"
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
#include <fstream>

using namespace std;

//_____________________________________________________________________________
THcHodoscope::THcHodoscope( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor

  //fTrackProj = new TClonesArray( "THaTrackProj", 5 );
  // Construct the planes
  fNPlanes = 0;			// No planes until we make them

}

//_____________________________________________________________________________
THcHodoscope::THcHodoscope( ) :
  THaNonTrackingDetector()
{
  // Constructor
}

//_____________________________________________________________________________
void THcHodoscope::Setup(const char* name, const char* description)
{

  static const char* const here = "Setup()";
  static const char* const message = 
    "Must construct %s detector with valid name! Object construction failed.";

  cout << "In THcHodoscope::Setup()" << endl;
  // Base class constructor failed?
  if( IsZombie()) return;

  fDebug   = 1;  // Keep this at one while we're working on the code    

  char prefix[2];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  DBRequest listextra[]={
    {"hodo_num_planes", &fNPlanes, kInt},
    {0}
  };
  fNPlanes = 4; 		// Default if not defined
  gHcParms->LoadParmValues((DBRequest*)&listextra,prefix);
  
  // Plane names
  fPlaneNames = new char* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {fPlaneNames[i] = new char[3];}
  // Should get the plane names from parameters.  
  // could try this: grep _zpos PARAM/hhodo.pos | sed 's/\_/\ /g' | awk '{print $2}'
  strcpy(fPlaneNames[0],"1x");  
  strcpy(fPlaneNames[1],"1y");
  strcpy(fPlaneNames[2],"2x");
  strcpy(fPlaneNames[3],"2y");

  // Probably shouldn't assume that description is defined
  char* desc = new char[strlen(description)+50];
  fPlanes = new THcScintillatorPlane* [fNPlanes];
  for(Int_t i=0;i < fNPlanes;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);
    fPlanes[i] = new THcScintillatorPlane(fPlaneNames[i], desc, i+1,fNPlanes, this); 
    //fPlanes[i] = new THcScintillatorPlane(fPlaneNames[i], desc, i+1, this); 
    cout << "Created Scintillator Plane " << fPlaneNames[i] << ", " << desc << endl;
  }
}

#if 0
//_____________________________________________________________________________
void THcHodoscope::SetApparatus( THaApparatus* app )
{
  // Set the apparatus of this detector as well as the subdetectors

  cout << "In THcHodoscope::SetApparatus" << endl;
  THaDetector::SetApparatus( app );
  for(Int_t i=0;i < fNPlanes;i++) {
    fPlanes[i]->SetApparatus( app );
  }
  return;
}
#endif

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoscope::Init( const TDatime& date )
{
  static const char* const here = "Init()";
  cout << "In THcHodoscope::Init()" << endl;
  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  // But it needs to happen before the sub detectors are initialized
  // so that they can get the pointer to the hitlist.

  THcHitList::InitHitList(fDetMap, "THcHodoscopeHit", 100);

  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    if((status = fPlanes[ip]->Init( date ))) {
      return fStatus=status;
    }
  }

  // Replace with what we need for Hall C
  //  const DataDest tmp[NDEST] = {
  //    { &fRTNhit, &fRANhit, fRT, fRT_c, fRA, fRA_p, fRA_c, fROff, fRPed, fRGain },
  //    { &fLTNhit, &fLANhit, fLT, fLT_c, fLA, fLA_p, fLA_c, fLOff, fLPed, fLGain }
  //  };
  //  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );

  // Will need to determine which apparatus it belongs to and use the
  // appropriate detector ID in the FillMap call
  if( gHcDetectorMap->FillMap(fDetMap, "HSCIN") < 0 ) {
    Error( Here(here), "Error filling detectormap for %s.", 
	     "HSCIN");
      return kInitError;
  }

  return fStatus = kOK;
}
//_____________________________________________________________________________
Double_t THcHodoscope::DefineDoubleVariable(const char* fName)
{
  // Define a variale of type double by looking it up in the THcParmList
  char prefix[2];
  char parname[100];
  Double_t tmpvar=-1e6;
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,fName);
  if (gHcParms->Find(parname)) {
    tmpvar=*(Double_t *)gHcParms->Find(parname)->GetValuePointer();
    if (fDebug>=1)  cout << parname << " "<< tmpvar << endl;
  } else {
    cout << "*** ERROR!!! Could not find " << parname << " in the list of variables! ***" << endl;
  }
  return tmpvar;
}

//_____________________________________________________________________________
Int_t THcHodoscope::DefineIntVariable(const char* fName)
{
  // Define a variale of type int by looking it up in the THcParmList
  char prefix[2];
  char parname[100];
  Int_t tmpvar=-100000;
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,fName);
  if (gHcParms->Find(parname)) {
    tmpvar=*(Int_t *)gHcParms->Find(parname)->GetValuePointer();
    if (fDebug>=1)  cout << parname << " "<< tmpvar << endl;
  } else {
    cout << "*** ERROR!!! Could not find " << parname << " in the list of variables! ***" << endl;
  }
  return tmpvar;
}

//_____________________________________________________________________________
void THcHodoscope::DefineArray(const char* fName, const Int_t index, Double_t *myArray)
{
  char prefix[2];
  char parname[100];
  //  Int_t tmpvar=-100000;
   prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,fName);
  if (gHcParms->Find(parname)) {
    if (fDebug >=1) cout <<parname;
    Double_t* p = (Double_t *)gHcParms->Find(parname)->GetValuePointer();
    for(Int_t i=0;i<index;i++) {
      myArray[i] = p[i];
      if (fDebug>=1)    cout << " " << myArray[i];
    }
    if (fDebug>=1)  cout << endl;

  }
  else {
    cout <<" Could not find "<<parname<<" in the DataBase!!!\n";
  }
}

//_____________________________________________________________________________
void THcHodoscope::DefineArray(const char* fName, char** Suffix, const Int_t index, Double_t *myArray)
{
  // Try to read an array made up of what used to be (in the f77 days) a number of variables
  // example: hscin_1x_center, hscin_1y_center, hscin_2x_center, hscin_2y_center will become scin_center
  //
  char prefix[2];
  char parname[100],parname2[100];
  //  
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,fName);
  for(Int_t i=0;i<index;i++) {
    strcpy(parname2,Form(parname,Suffix[i]));
    if (gHcParms->Find(parname2)) {
      if (fDebug >=1) cout <<parname2;
      myArray[i] = *(Double_t *)gHcParms->Find(parname2)->GetValuePointer();
      if (fDebug>=1)    cout << " " << myArray[i];
    }
    if (fDebug>=1)  cout << endl;
    else {
      cout <<" Could not find "<<parname2<<" in the DataBase!!!\n";
    }
  }
}

//_____________________________________________________________________________
void THcHodoscope::DefineArray(const char* fName, char** Suffix, const Int_t index, Int_t *myArray)
{
  // Try to read an array made up of what used to be (in the f77 days) a number of variables
  // example: hscin_1x_center, hscin_1y_center, hscin_2x_center, hscin_2y_center will become scin_center
  //
  char prefix[2];
  char parname[100],parname2[100];
  //  
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,fName);
  for(Int_t i=0;i<index;i++) {
    strcpy(parname2,Form(parname,Suffix[i]));
    if (gHcParms->Find(parname2)) {
      if (fDebug >=1) cout <<parname2;
      myArray[i] = *(Int_t *)gHcParms->Find(parname2)->GetValuePointer();
      if (fDebug>=1)    cout << " " << myArray[i];
    }
    if (fDebug>=1)  cout << endl;
    else {
      cout <<" Could not find "<<parname2<<" in the DataBase!!!\n";
    }
  }
}

//_____________________________________________________________________________
Int_t THcHodoscope::ReadDatabase( const TDatime& date )
{
  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";
  char prefix[2];
  char parname[100];

  // Read data from database 
  // Pull values from the THcParmList instead of reading a database
  // file like Hall A does.

  // Will need to determine which spectrometer in order to construct
  // the parameter names (e.g. hscin_1x_nr vs. sscin_1x_nr)

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  //
  prefix[1]='\0';
  strcpy(parname,prefix);
  strcat(parname,"scin_");
  Int_t plen=strlen(parname);

  fNPaddle = new Int_t [fNPlanes];
  //  fSpacing = new Double_t [fNPlanes];
  //fCenter = new Double_t* [fNPlanes];

  // An alternate way to get these variables
  // Can add Xscin_P_center when LoadParmValues supports arrays
  
  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  //
  prefix[1]='\0';

  for(Int_t i=0;i<fNPlanes;i++) {
    
    DBRequest list[]={
      {Form("scin_%s_nr",fPlaneNames[i]), &fNPaddle[i], kInt},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  }

/*
#if 0
  fCenter = new Double_t* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {
    parname[plen] = '\0';
    strcat(parname,fPlaneNames[i]);
    strcat(parname,"_center");
    cout << parname << endl;
    Double_t* p = (Double_t *)gHcParms->Find(parname)->GetValuePointer();
    fCenter[i] = new Double_t [fNPaddle[i]];
    cout << parname;
    for(Int_t ipad=0;ipad<fNPaddle[i];ipad++) {
      fCenter[i][ipad] = p[ipad];
      if (fDebug>=1)    cout << " " << fCenter[i][ipad];
    }
    if (fDebug>=1)  cout << endl;
  }
#endif
*/
  // GN added
  // reading variables from *hodo.param
  fMaxScinPerPlane=fNPaddle[0];
  for (Int_t i=1;i<fNPlanes;i++) {
    fMaxScinPerPlane=(fMaxScinPerPlane > fNPaddle[i])? fMaxScinPerPlane : fNPaddle[i];
  }
// need this for "padded arrays" i.e. 4x16 lists of parameters (GN)
  fMaxHodoScin=fMaxScinPerPlane*fNPlanes; 
  if (fDebug>=1)  cout <<"fMaxScinPerPlane = "<<fMaxScinPerPlane<<" fMaxHodoScin = "<<fMaxHodoScin<<endl;
  
  fHodoVelLight=new Double_t [fMaxHodoScin];
  fHodoPosSigma=new Double_t [fMaxHodoScin];
  fHodoNegSigma=new Double_t [fMaxHodoScin];
  fHodoPosMinPh=new Double_t [fMaxHodoScin];
  fHodoNegMinPh=new Double_t [fMaxHodoScin];
  fHodoPosPhcCoeff=new Double_t [fMaxHodoScin];
  fHodoNegPhcCoeff=new Double_t [fMaxHodoScin];
  fHodoPosTimeOffset=new Double_t [fMaxHodoScin];
  fHodoNegTimeOffset=new Double_t [fMaxHodoScin];
  fHodoPosPedLimit=new Int_t [fMaxHodoScin];
  fHodoNegPedLimit=new Int_t [fMaxHodoScin];
  fHodoPosInvAdcOffset=new Double_t [fMaxHodoScin];
  fHodoNegInvAdcOffset=new Double_t [fMaxHodoScin];
  fHodoPosInvAdcLinear=new Double_t [fMaxHodoScin];
  fHodoNegInvAdcLinear=new Double_t [fMaxHodoScin];
  fHodoPosInvAdcAdc=new Double_t [fMaxHodoScin];
  fHodoNegInvAdcAdc=new Double_t [fMaxHodoScin];
  
  prefix[1]='\0';
  DBRequest list[]={
    {"start_time_center", &fStartTimeCenter, kDouble},
    {"start_time_slop", &fStartTimeSlop, kDouble},
    {"scin_tdc_to_time", &fScinTdcToTime, kDouble},
    {"scin_tdc_min", &fScinTdcMin, kDouble},
    {"scin_tdc_max", &fScinTdcMax, kDouble},
    {"tof_tolerance", &fTofTolerance, kDouble},
    {"pathlength_central", &fPathLengthCentral, kDouble},
    {"hodo_vel_light",&fHodoVelLight[0],kDouble,fMaxHodoScin},
    {"hodo_pos_sigma",&fHodoPosSigma[0],kDouble,fMaxHodoScin},
    {"hodo_neg_sigma",&fHodoNegSigma[0],kDouble,fMaxHodoScin},
    {"hodo_pos_minph",&fHodoPosMinPh[0],kDouble,fMaxHodoScin},
    {"hodo_neg_minph",&fHodoNegMinPh[0],kDouble,fMaxHodoScin},
    {"hodo_pos_phc_coeff",&fHodoPosPhcCoeff[0],kDouble,fMaxHodoScin},
    {"hodo_neg_phc_coeff",&fHodoNegPhcCoeff[0],kDouble,fMaxHodoScin},
    {"hodo_pos_time_offset",&fHodoPosTimeOffset[0],kDouble,fMaxHodoScin},
    {"hodo_neg_time_offset",&fHodoNegTimeOffset[0],kDouble,fMaxHodoScin},
    {"hodo_pos_ped_limit",&fHodoPosPedLimit[0],kInt,fMaxHodoScin},
    {"hodo_neg_ped_limit",&fHodoNegPedLimit[0],kInt,fMaxHodoScin},
    {"tofusinginvadc",&fTofUsingInvAdc,kInt},
    {"hodo_pos_invadc_offset",&fHodoPosInvAdcOffset[0],kDouble,fMaxHodoScin},
    {"hodo_neg_invadc_offset",&fHodoNegInvAdcOffset[0],kDouble,fMaxHodoScin},
    {"hodo_pos_invadc_linear",&fHodoPosInvAdcLinear[0],kDouble,fMaxHodoScin},
    {"hodo_neg_invadc_linear",&fHodoNegInvAdcLinear[0],kDouble,fMaxHodoScin},
    {"hodo_pos_invadc_adc",&fHodoPosInvAdcAdc[0],kDouble,fMaxHodoScin},
    {"hodo_neg_invadc_adc",&fHodoNegInvAdcAdc[0],kDouble,fMaxHodoScin},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  if (fDebug >=1) {
    cout <<"******* Testing Hodoscope Parameter Reading ***\n";
    cout<<"StarTimeCenter = "<<fStartTimeCenter<<endl;
    cout<<"StartTimeSlop = "<<fStartTimeSlop<<endl;
    cout <<"ScintTdcToTime = "<<fScinTdcToTime<<endl;
    cout <<"TdcMin = "<<fScinTdcMin<<" TdcMax = "<<fScinTdcMax<<endl;
    cout <<"TofTolerance = "<<fTofTolerance<<endl;
    cout <<"*** VelLight ***\n";
    for (int i1=0;i1<fNPlanes;i1++) {
      cout<<"Plane "<<i1<<endl;
      for (int i2=0;i2<fMaxScinPerPlane;i2++) {
	cout<<fHodoVelLight[GetScinIndex(i1,i2)]<<" ";
      }
      cout <<endl;
    }
 
    cout <<endl<<endl;
    // check fHodoPosPhcCoeff
    /*
    cout <<"fHodoPosPhcCoeff = ";
    for (int i1=0;i1<fMaxHodoScin;i1++) {
      cout<<this->GetHodoPosPhcCoeff(i1)<<" ";
    }
    cout<<endl;
    */
  }
  //
  if ((fTofTolerance > 0.5) && (fTofTolerance < 10000.)) {
    cout << "USING "<<fTofTolerance<<" NSEC WINDOW FOR FP NO_TRACK CALCULATIONS.\n";
  }
  else {
    fTofTolerance= 3.0;
    cout << "*** USING DEFAULT 3 NSEC WINDOW FOR FP NO_TRACK CALCULATIONS!! ***\n";
  }
  fIsInit = true;
  return kOK;
}

//_____________________________________________________________________________
Int_t THcHodoscope::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder
  cout << "THcHodoscope::DefineVariables called " << GetName() << endl;
  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  //  RVarDef vars[] = {
    //    hpostdc1 HMS s1x+ TDC hits
    //    hnegtdc1 HMS s1x+ TDC hits
    //...
    //    hnegtdc4 HMS s2y- TDC hits

  //  RVarDef vars[] = {
  //    { "nlthit", "Number of Left paddles TDC times",  "fLTNhit" },
  //    { "nrthit", "Number of Right paddles TDC times", "fRTNhit" },
  //    { "nlahit", "Number of Left paddles ADCs amps",  "fLANhit" },
  //    { "nrahit", "Number of Right paddles ADCs amps", "fRANhit" },
  //    { "lt",     "TDC values left side",              "fLT" },
  //    { "lt_c",   "Corrected times left side",         "fLT_c" },
  //    { "rt",     "TDC values right side",             "fRT" },
  //    { "rt_c",   "Corrected times right side",        "fRT_c" },
  //    { "la",     "ADC values left side",              "fLA" },
  //    { "la_p",   "Corrected ADC values left side",    "fLA_p" },
  //    { "la_c",   "Corrected ADC values left side",    "fLA_c" },
  //    { "ra",     "ADC values right side",             "fRA" },
  //    { "ra_p",   "Corrected ADC values right side",   "fRA_p" },
  //    { "ra_c",   "Corrected ADC values right side",   "fRA_c" },
  //    { "nthit",  "Number of paddles with l&r TDCs",   "fNhit" },
  //    { "t_pads", "Paddles with l&r coincidence TDCs", "fHitPad" },
  //    { "y_t",    "y-position from timing (m)",        "fYt" },
  //    { "y_adc",  "y-position from amplitudes (m)",    "fYa" },
  //    { "time",   "Time of hit at plane (s)",          "fTime" },
  //    { "dtime",  "Est. uncertainty of time (s)",      "fdTime" },
  //    { "dedx",   "dEdX-like deposited in paddle",     "fAmpl" },
  //    { "troff",  "Trigger offset for paddles",        "fTrigOff"},
  //    { "trn",    "Number of tracks for hits",         "GetNTracks()" },
  //    { "trx",    "x-position of track in det plane",  "fTrackProj.THaTrackProj.fX" },
  //    { "try",    "y-position of track in det plane",  "fTrackProj.THaTrackProj.fY" },
  //    { "trpath", "TRCS pathlen of track to det plane","fTrackProj.THaTrackProj.fPathl" },
  //    { "trdx",   "track deviation in x-position (m)", "fTrackProj.THaTrackProj.fdX" },
  //    { "trpad",  "paddle-hit associated with track",  "fTrackProj.THaTrackProj.fChannel" },
  //    { 0 }
  //  };
  //  return DefineVarsFromList( vars, mode );
  return kOK;
}

//_____________________________________________________________________________
THcHodoscope::~THcHodoscope()
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
void THcHodoscope::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

  delete [] fNPaddle;             fNPaddle = NULL;
  delete [] fHodoVelLight;        fHodoVelLight = NULL;
  delete [] fHodoPosSigma;        fHodoPosSigma = NULL;
  delete [] fHodoNegSigma;        fHodoNegSigma = NULL;
  delete [] fHodoPosMinPh;        fHodoPosMinPh = NULL;
  delete [] fHodoNegMinPh;        fHodoNegMinPh = NULL;
  delete [] fHodoPosPhcCoeff;     fHodoPosPhcCoeff = NULL;
  delete [] fHodoNegPhcCoeff;     fHodoNegPhcCoeff = NULL;
  delete [] fHodoPosTimeOffset;   fHodoPosTimeOffset = NULL;
  delete [] fHodoNegTimeOffset;   fHodoNegTimeOffset = NULL;
  delete [] fHodoPosPedLimit;     fHodoPosPedLimit = NULL;
  delete [] fHodoNegPedLimit;     fHodoNegPedLimit = NULL;
  delete [] fHodoPosInvAdcOffset; fHodoPosInvAdcOffset = NULL;
  delete [] fHodoNegInvAdcOffset; fHodoNegInvAdcOffset = NULL;
  delete [] fHodoPosInvAdcLinear; fHodoPosInvAdcLinear = NULL;
  delete [] fHodoNegInvAdcLinear; fHodoNegInvAdcLinear = NULL;
  delete [] fHodoPosInvAdcAdc;    fHodoPosInvAdcAdc = NULL;
  delete [] fHodoNegInvAdcAdc;    fHodoNegInvAdcAdc = NULL;
  //  delete [] fSpacing; fSpacing = NULL;
  //delete [] fCenter;  fCenter = NULL; // This 2D. What is correct way to delete?

  //  delete [] fRA_c;    fRA_c    = NULL;
  //  delete [] fRA_p;    fRA_p    = NULL;
  //  delete [] fRA;      fRA      = NULL;
  //  delete [] fLA_c;    fLA_c    = NULL;
  //  delete [] fLA_p;    fLA_p    = NULL;
  //  delete [] fLA;      fLA      = NULL;
  //  delete [] fRT_c;    fRT_c    = NULL;
  //  delete [] fRT;      fRT      = NULL;
  //  delete [] fLT_c;    fLT_c    = NULL;
  //  delete [] fLT;      fLT      = NULL;
  
  //  delete [] fRGain;   fRGain   = NULL;
  //  delete [] fLGain;   fLGain   = NULL;
  //  delete [] fRPed;    fRPed    = NULL;
  //  delete [] fLPed;    fLPed    = NULL;
  //  delete [] fROff;    fROff    = NULL;
  //  delete [] fLOff;    fLOff    = NULL;
  //  delete [] fTWalkPar; fTWalkPar = NULL;
  //  delete [] fTrigOff; fTrigOff = NULL;

  //  delete [] fHitPad;  fHitPad  = NULL;
  //  delete [] fTime;    fTime    = NULL;
  //  delete [] fdTime;   fdTime   = NULL;
  //  delete [] fYt;      fYt      = NULL;
  //  delete [] fYa;      fYa      = NULL;
}

//_____________________________________________________________________________
inline 
void THcHodoscope::Clear( Option_t* opt)
{
  // Reset per-event data.
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fPlanes[ip]->Clear(opt);
  }
}

//_____________________________________________________________________________
Int_t THcHodoscope::Decode( const THaEvData& evdata )
{

  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

  if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }
  if(fAnalyzePedestals) {
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      fPlanes[ip]->CalculatePedestals();
    }
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  // Let each plane get its hits
  Int_t nexthit = 0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    //    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
    // GN: select only events that have reasonable TDC values to start with
    // as per the Engine h_strip_scin.f
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList,nexthit);
        if (fPlanes[ip]->GetNScinHits()>0) {
      fPlanes[ip]->PulseHeightCorrection();
      }
  }

  // fRawHitList is TClones array of THcHodoscopeHit objects
#if 0
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_pos << " " << hit->fADC_neg << " "  <<  hit->fTDC_pos
	 << " " <<  hit->fTDC_neg << endl;
  }
  cout << endl;
#endif

  fStartTime = 500;		// Drift Chamber will need this

  return nhits;
}

//_____________________________________________________________________________
Int_t THcHodoscope::ApplyCorrections( void )
{
  return(0);
}
//_____________________________________________________________________________
Double_t THcHodoscope::TimeWalkCorrection(const Int_t& paddle,
					     const ESide side)
{
  return(0.0);
}

//_____________________________________________________________________________
Int_t THcHodoscope::CoarseProcess( TClonesArray&  tracks  )
{
  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  //  static const Double_t sqrt2 = TMath::Sqrt(2.);
  //  cout <<"**** in THcHodoscope CoarseProcess ********\n"; 
  /*  
  for(Int_t i=0;i<fNPlanes;i++) {
    cout<<i<<" ";
    fPlanes[i]->CoarseProcess(tracks);
    }*/
  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoscope::FineProcess( TClonesArray& tracks )
{
  // Reconstruct coordinates of particle track cross point with scintillator
  // plane, and copy the data into the following local data structure:
  //
  // Units of measurements are meters.

  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::FineTrack() are used.

  return 0;
}
//_____________________________________________________________________________
Int_t THcHodoscope::GetScinIndex( Int_t nPlane, Int_t nPaddle ) {
  // GN: Return the index of a scintillator given the plane # and the paddle #
  // This assumes that both planes and 
  // paddles start counting from 0!
  // Result also counts from 0.
  return fNPlanes*nPaddle+nPlane;
}
//_____________________________________________________________________________
Int_t THcHodoscope::GetScinIndex( Int_t nSide, Int_t nPlane, Int_t nPaddle ) {
  return nSide*fMaxHodoScin+fNPlanes*nPaddle+nPlane-1;
}
//_____________________________________________________________________________
Double_t THcHodoscope::GetPathLengthCentral() {
  return fPathLengthCentral;
}
ClassImp(THcHodoscope)
////////////////////////////////////////////////////////////////////////////////
