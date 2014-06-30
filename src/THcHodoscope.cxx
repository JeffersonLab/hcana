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

#include "THcSignalHit.h"

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
  fStartTime=-1e5;
  fGoodStartTime=kFALSE;

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

  //  static const char* const here = "Setup()";
  //  static const char* const message = 
  //    "Must construct %s detector with valid name! Object construction failed.";

  cout << "In THcHodoscope::Setup()" << endl;
  // Base class constructor failed?
  if( IsZombie()) return;

  fDebug   = 1;  // Keep this at one while we're working on the code    

  char prefix[2];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  string planenamelist;
  DBRequest listextra[]={
    {"hodo_num_planes", &fNPlanes, kInt},
    {"hodo_plane_names",&planenamelist, kString},
    {0}
  };
  //fNPlanes = 4; 		// Default if not defined
  gHcParms->LoadParmValues((DBRequest*)&listextra,prefix);
  
  cout << "Plane Name List : " << planenamelist << endl;

  vector<string> plane_names = vsplit(planenamelist);
  // Plane names  
  if(plane_names.size() != (UInt_t) fNPlanes) {
    cout << "ERROR: Number of planes " << fNPlanes 
	 << " doesn't agree with number of plane names " << plane_names.size() << endl;
    // Should quit.  Is there an official way to quit?
  }
  fPlaneNames = new char* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {
    fPlaneNames[i] = new char[plane_names[i].length()];
    strcpy(fPlaneNames[i], plane_names[i].c_str());
  }
  /*  fPlaneNames = new char* [fNPlanes];
  for(Int_t i=0;i<fNPlanes;i++) {fPlaneNames[i] = new char[3];}
  // Should get the plane names from parameters.  
  // could try this: grep _zpos PARAM/hhodo.pos | sed 's/\_/\ /g' | awk '{print $2}'
  strcpy(fPlaneNames[0],"1x");  
  strcpy(fPlaneNames[1],"1y");
  strcpy(fPlaneNames[2],"2x");
  strcpy(fPlaneNames[3],"2y");
  */
  // Probably shouldn't assume that description is defined
  char* desc = new char[strlen(description)+100];
  fPlanes = new THcScintillatorPlane* [fNPlanes];
  for(Int_t i=0;i < fNPlanes;i++) {
    strcpy(desc, description);
    strcat(desc, " Plane ");
    strcat(desc, fPlaneNames[i]);
    fPlanes[i] = new THcScintillatorPlane(fPlaneNames[i], desc, i+1,fNPlanes,this); // Number planes starting from zero!!
    cout << "Created Scintillator Plane " << fPlaneNames[i] << ", " << desc << endl;
  }
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoscope::Init( const TDatime& date )
{
  cout << "In THcHodoscope::Init()" << endl;
  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  // But it needs to happen before the sub detectors are initialized
  // so that they can get the pointer to the hitlist.

  InitHitList(fDetMap, "THcHodoscopeHit", 100);

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

  char EngineDID[]="xSCIN";
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
  //  Int_t plen=strlen(parname);
  cout << " readdatabse hodo fnplanes = " << fNPlanes << endl;
  fNPaddle = new Int_t [fNPlanes];
  //  fSpacing = new Double_t [fNPlanes];
  //fCenter = new Double_t* [fNPlanes];

  // An alternate way to get these variables
  // Can add Xscin_P_center when LoadParmValues supports arrays
  
  FPTime     = new Double_t[fNPlanes];

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
    {"tof_tolerance", &fTofTolerance, kDouble,0,1},
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
    {"tofusinginvadc",&fTofUsingInvAdc,kInt,0,1},
    {0}
  };
  fTofUsingInvAdc = 0;		// Default if not defined
  fTofTolerance = 3.0;		// Default if not defined
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);
  if (fTofUsingInvAdc) {
    DBRequest list2[]={
      {"hodo_pos_invadc_offset",&fHodoPosInvAdcOffset[0],kDouble,fMaxHodoScin},
      {"hodo_neg_invadc_offset",&fHodoNegInvAdcOffset[0],kDouble,fMaxHodoScin},
      {"hodo_pos_invadc_linear",&fHodoPosInvAdcLinear[0],kDouble,fMaxHodoScin},
      {"hodo_neg_invadc_linear",&fHodoNegInvAdcLinear[0],kDouble,fMaxHodoScin},
      {"hodo_pos_invadc_adc",&fHodoPosInvAdcAdc[0],kDouble,fMaxHodoScin},
      {"hodo_neg_invadc_adc",&fHodoNegInvAdcAdc[0],kDouble,fMaxHodoScin},
      {0}
    };
    gHcParms->LoadParmValues((DBRequest*)&list2,prefix);
  };
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

  RVarDef vars[] = {
    {"fpHitsTime","Time at focal plane from all hits","FPTime"},
    {"fpTimeDif1","Time differnce betwwen plane 1 & 2","FPTimeDif1"},
    {"fpTimeDif2","Time differnce betwwen plane 1 & 3","FPTimeDif2"},
    {"fpTimeDif3","Time differnce betwwen plane 1 & 4","FPTimeDif3"},
    {"fpTimeDif4","Time differnce betwwen plane 2 & 3","FPTimeDif4"},
    {"fpTimeDif5","Time differnce betwwen plane 2 & 4","FPTimeDif5"},
    {"fpTimeDif6","Time differnce betwwen plane 3 & 4","FPTimeDif6"},
    {"starttime","Hodoscope Start Time","fStartTime"},
    {"hgoodstarttime","Hodoscope Good Start Time","fGoodStartTime"},
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
        { 0 }
   };
  return DefineVarsFromList( vars, mode );
  //  return kOK;
}

//_____________________________________________________________________________
THcHodoscope::~THcHodoscope()
{
  // Destructor. Remove variables from global list.

  delete [] FPTime;

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
    FPTime[ip]=0.;
    FPTimeDif1=0.;
    FPTimeDif2=0.;
    FPTimeDif3=0.;
    FPTimeDif4=0.;
    FPTimeDif5=0.;
    FPTimeDif6=0.;
  }
}

//_____________________________________________________________________________
Int_t THcHodoscope::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t nhits = DecodeToHitList(evdata);
  //
  // GN: print event number so we can cross-check with engine
  // if (evdata.GetEvNum()>1000) cout <<"hcana event no = "<<evdata.GetEvNum()<<endl;

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

  fStartTime=0;
  fNfptimes=0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    //    nexthit = fPlanes[ip]->ProcessHits(fRawHitList, nexthit);
    // GN: select only events that have reasonable TDC values to start with
    // as per the Engine h_strip_scin.f
    nexthit = fPlanes[ip]->ProcessHits(fRawHitList,nexthit);
    if (fPlanes[ip]->GetNScinHits()>0) {
      fPlanes[ip]->PulseHeightCorrection();
      // GN: allow for more than one fptime per plane!!
      for (Int_t i=0;i<fPlanes[ip]->GetNScinGoodHits();i++) {
	if (TMath::Abs(fPlanes[ip]->GetFpTime(i)-fStartTimeCenter)<=fStartTimeSlop) {
	  fStartTime=fStartTime+fPlanes[ip]->GetFpTime(i);
	  // GN write stuff out so I can compare with engine
	  ///	  cout<<"hcana event= "<<evdata.GetEvNum()<<" fNfptimes= "<<fNfptimes<<" fptime= "<<fPlanes[ip]->GetFpTime(i)<<endl;
	  fNfptimes++;
	}
      }
    }
  }
  if (fNfptimes>0) {
    fStartTime=fStartTime/fNfptimes;
    fGoodStartTime=kTRUE;
  } else {
    fGoodStartTime=kFALSE;
    fStartTime=fStartTimeCenter;
  }
#if 0
  for(Int_t ihit = 0; ihit < fNRawHits ; ihit++) {
    THcHodoscopeHit* hit = (THcHodoscopeHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_pos << " " << hit->fADC_neg << " "  <<  hit->fTDC_pos
	 << " " <<  hit->fTDC_neg << endl;
  }
  cout << endl;
#endif
  ///  fStartTime = 500;		// Drift Chamber will need this

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

  //  cout << "------------------------------------" << endl;
  // Loop over tracks then loop over scintillator planes
  // **MAIN LOOP: Loop over all tracks and get corrected time, tof, beta...
  Int_t Ntracks = tracks.GetLast()+1; // Number of reconstructed tracks
  Double_t dedX[Ntracks][53];
  Double_t p, betap, sumFPTime, hBetaPcent, xhitCoord, yhitCoord, tmin;
  Int_t numScinHit[Ntracks], numPmtHit[Ntracks], scinHit[Ntracks][53] ; // check the second index
  Int_t padNum,nFound, timeHist[200], hitPaddle, sIndex,k, jmax, maxhit, ihit;
  Int_t hntof, hntofPairs, numFPTime;
  Double_t scinTrnsCoord,scinLongCoord,scinCenter;
  Double_t scinFPTime[Ntracks][53], timeAtFP[Ntracks];
  Bool_t keepPos[53], keepNeg[53], goodPlaneTime[Ntracks][fNPlanes];
  Bool_t goodScinTime[Ntracks][53],scinOnTrack[Ntracks][53];
  Double_t beta[Ntracks], betaChisq[Ntracks];
  
  Double_t hpartmass=0.00051099; // Fix it

  // Double_t sumPlaneTime[fNPlanes], , sum_fp_time, num_fp_time;
  
  if (tracks.GetLast()+1 > 0 ) {
    for ( Int_t itrack = 0; itrack < Ntracks; itrack++ ) { // Line 133
      THaTrack* theTrack = dynamic_cast<THaTrack*>( tracks.At(itrack) );
      if (!theTrack) return -1;
      
      numFPTime = 0;
      beta[itrack] = 0.;
      betaChisq[itrack] = -3.;
      timeAtFP[itrack] = 0.;
      sumFPTime = 0.; // Line 138
      numScinHit[itrack] = 0; // Line 140
      numPmtHit[itrack] = 0; // Line 141
      p = theTrack->GetP(); // Line 142 Fix it
      //      p = 0.78;
      betap = p/( TMath::Sqrt( p * p + hpartmass * hpartmass) );
      
      //! Calculate all corrected hit times and histogram
      //! This uses a copy of code below. Results are save in time_pos,neg
      //! including the z-pos. correction assuming nominal value of betap
      //! Code is currently hard-wired to look for a peak in the
      //! range of 0 to 100 nsec, with a group of times that all
      //! agree withing a time_tolerance of time_tolerance nsec. The normal
      //! peak position appears to be around 35 nsec.
      //! NOTE: if want to find farticles with beta different than
      //! reference particle, need to make sure this is big enough
      //! to accomodate difference in TOF for other particles
      //! Default value in case user hasnt definedd something reasonable
      // Line 162 to 171 is already done above in ReadDatabase
      
      for (Int_t j=0; j<200; j++) { timeHist[j]=0; } // Line 176
      
      nFound = 0;
      Int_t numPlaneTime[fNPlanes];
      Double_t sumPlaneTime[fNPlanes];
      for ( Int_t dp = 0; dp <  fNPlanes; dp++ ){ 
	numPlaneTime[dp] = 0;
	sumPlaneTime[dp] = 0.;
      }
      // Loop over scintillator planes.
      // In ENGINE, its loop over good scintillator hits.
      for( Int_t ip = 0; ip < fNPlanes; ip++ ) {
	
	Int_t scinHits = fPlanes[ip]->GetNScinHits();

	Double_t adcPh[scinHits], path[scinHits], time[scinHits], betaPcent;
	Double_t timePos[scinHits], timeNeg[scinHits];
	Bool_t goodScinTime[Ntracks][scinHits],scinOnTrack[Ntracks][scinHits];
	Bool_t goodTDCPos[Ntracks][scinHits], goodTDCNeg[Ntracks][scinHits];
	Double_t scinTime[scinHits],scinSigma[scinHits],scinTimeFP[scinHits];
	
	betaPcent = 1.0;
	// first loop over hits with in a single plane
	for ( ihit = 0; ihit < scinHits; ihit++ ){
	  
	  timePos[ihit] = -99.;
	  timeNeg[ihit] = -99.;
	  keepPos[ihit] = kFALSE;
	  keepNeg[ihit] = kFALSE;
	  
	  scinPosADC = fPlanes[ip]->GetPosADC();
	  scinNegADC = fPlanes[ip]->GetNegADC();
	  scinPosTDC = fPlanes[ip]->GetPosTDC();
	  scinNegTDC = fPlanes[ip]->GetNegTDC();
	  
	  
	  hitPaddle = ((THcSignalHit*)scinPosTDC->At(ihit))->GetPaddleNumber()-1;
	  
	  xhitCoord = theTrack->GetX() + theTrack->GetTheta() *
	    ( fPlanes[ip]->GetZpos() +
	      ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 183
	  
	  yhitCoord = theTrack->GetY() + theTrack->GetPhi() *
	    ( fPlanes[ip]->GetZpos() +
	      ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 184
	  
	  
	  if ( ( ip == 0 ) || ( ip == 2 ) ){ // !x plane. Line 185
	    scinTrnsCoord = xhitCoord;
	    scinLongCoord = yhitCoord;
	  }
	  
	  else if ( ( ip == 1 ) || ( ip == 3 ) ){ // !y plane. Line 188
	    scinTrnsCoord = yhitCoord;
	    scinLongCoord = xhitCoord;
	  }
	  else { return -1; } // Line 195
	  
	  scinCenter = fPlanes[ip]->GetPosCenter(hitPaddle) + fPlanes[ip]->GetPosOffset();
	  sIndex = fNPlanes * hitPaddle + ip;
	  

	  if ( TMath::Abs( scinCenter - scinTrnsCoord ) <
	       ( fPlanes[ip]->GetSize() * 0.5 + fPlanes[ip]->GetHodoSlop() ) ){ // Line 293
	    scinOnTrack[itrack][ihit] = kFALSE;   
	    
	    if ( ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() < fScinTdcMax ) ) { // Line 199
	      
	      adcPh[ihit] = ((THcSignalHit*)scinPosADC->At(ihit))->GetData();
	      path[ihit] = fPlanes[ip]->GetPosLeft() - scinLongCoord;
	      time[ihit] = ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() * fScinTdcToTime;
	      time[ihit] = time[ihit] - fHodoPosPhcCoeff[sIndex] *
		           TMath::Sqrt( TMath::Max( 0., ( ( adcPh[ihit] / fHodoPosMinPh[sIndex] ) - 1 ) ) );
	      time[ihit] = time[ihit] - ( path[ihit] / fHodoVelLight[sIndex] ) - ( fPlanes[ip]->GetZpos() +  
			   ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * betap ) *
		//( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * betaPcent ) *
		           TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
			   theTrack->GetPhi() * theTrack->GetPhi() );
	      timePos[ihit] = time[ihit] - fHodoPosTimeOffset[sIndex];
	      nFound ++; // Line 210
	      
	      for ( k = 0; k < 200; k++ ){ // Line 211
		tmin = 0.5 * ( k + 1 ) ;
		if ( ( timePos[ihit] > tmin ) && ( timePos[ihit] < ( tmin + fTofTolerance ) ) )
		  timeHist[k] ++;
	      }
	    } // TDC pos hit condition
	    
	    
	    if ( ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() < fScinTdcMax ) ) { // Line 218
	      
	      adcPh[ihit] = ((THcSignalHit*)scinNegADC->At(ihit))->GetData();
	      path[ihit] = scinLongCoord - fPlanes[ip]->GetPosRight();
	      time[ihit] = ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() * fScinTdcToTime;
	      time[ihit] = time[ihit] - fHodoNegPhcCoeff[sIndex] *
                           TMath::Sqrt( TMath::Max( 0., ( ( adcPh[ihit] / fHodoNegMinPh[sIndex] ) - 1 ) ) );
	      time[ihit] = time[ihit] - ( path[ihit] / fHodoVelLight[sIndex] ) - ( fPlanes[ip]->GetZpos() +
			   //( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * betaPcent ) *
			   ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * betap ) *
		           TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
			   theTrack->GetPhi() * theTrack->GetPhi() );
	      timeNeg[ihit] = time[ihit] - fHodoNegTimeOffset[sIndex];
	      nFound ++; // Line 229
	      
	      for ( k = 0; k < 200; k++ ){ // Line 230
		tmin = 0.5 * ( k + 1 );
		if ( ( timeNeg[ihit] > tmin ) && ( timeNeg[ihit] < ( tmin + fTofTolerance ) ) )
		  timeHist[k] ++;
	      }
	    } // TDC neg hit condition
	  }
	} // Loop over hits in a plane
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	
	jmax = 0; // Line 240
	maxhit = 0;
	
	for ( k = 0; k < 200; k++ ){
	  if ( timeHist[k] > maxhit ){
	    jmax = k+1;
	    maxhit = timeHist[k];
	  }
	}
	
	if ( jmax >= 0 ){ // Line 248. Here I followed the code of THcSCintilaltorPlane::PulseHeightCorrection
	  tmin = 0.5 * jmax;
	  for( ihit = 0; ihit < scinHits; ihit++) { // Loop over sinc. hits. in plane
	    if ( ( timePos[ihit] > tmin ) && ( timePos[ihit] < ( tmin + fTofTolerance ) ) ) {
	      keepPos[ihit]=kTRUE;
	    }	
	    if ( ( timeNeg[ihit] > tmin ) && ( timeNeg[ihit] < ( tmin + fTofTolerance ) ) ){
	      keepNeg[ihit] = kTRUE;
	    }	
	  }
	} // jmax > 0 condition
	
	// ! Resume regular tof code, now using time filer from above
	for ( ihit = 0; ihit < scinHits; ihit++ ){
	  goodScinTime[itrack][ihit] = kFALSE;
	  goodTDCPos[itrack][ihit] = kFALSE;
	  goodTDCNeg[itrack][ihit] = kFALSE;
	  scinTime[ihit] = 0.;
	  scinSigma[ihit] = 0.;
	}
	
	
	// ---------------------- Scond loop over scint. hits in a plane ------------------------------
	
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
	
	// Second loop over hits with in a single plane
	Double_t scinPosTime[scinHits], scinNegTime[scinHits];
	for ( ihit = 0; ihit < scinHits; ihit++ ){
	  
	  hitPaddle = ((THcSignalHit*)scinPosTDC->At(ihit))->GetPaddleNumber()-1;
	  
	  xhitCoord = theTrack->GetX() + theTrack->GetTheta() *
	    ( fPlanes[ip]->GetZpos() +
	      ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 277
	  yhitCoord = theTrack->GetY() + theTrack->GetPhi() *
	    ( fPlanes[ip]->GetZpos() +
	      ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 278
	  
	  
	  if ( ( ip == 0 ) || ( ip == 2 ) ){ // !x plane. Line 278
	    scinTrnsCoord = xhitCoord;
	    scinLongCoord = yhitCoord;
	  }
	  else if ( ( ip == 1 ) || ( ip == 3 ) ){ // !y plane. Line 281
	    scinTrnsCoord = yhitCoord;
	    scinLongCoord = xhitCoord;
	  }
	  else { return -1; } // Line 288
	  
	  scinCenter = fPlanes[ip]->GetPosCenter(hitPaddle) + fPlanes[ip]->GetPosOffset();
	  sIndex = fNPlanes * hitPaddle + ip;
	  
	  // ** Check if scin is on track
	  if ( TMath::Abs( scinCenter - scinTrnsCoord ) >
	       ( fPlanes[ip]->GetSize() * 0.5 + fPlanes[ip]->GetHodoSlop() ) ){ // Line 293
	    scinOnTrack[itrack][ihit] = kFALSE;
	  }
	  else{
	    scinOnTrack[itrack][ihit] = kTRUE;
	    
	    // * * Check for good TDC
	    if ( ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() < fScinTdcMax ) &&
		 ( keepPos[ihit] ) ) { // 301
	      
	      // ** Calculate time for each tube with a good tdc. 'pos' side first.
	      goodTDCPos[itrack][ihit] = kTRUE;
	      hntof ++;
	      adcPh[ihit] = ((THcSignalHit*)scinPosADC->At(ihit))->GetData();
	      path[ihit] = fPlanes[ip]->GetPosLeft() - scinLongCoord;
	      
	      //* Convert TDC value to time, do pulse height correction, correction for
	      //* propogation of light thru scintillator, and offset.
	      
	      time[ihit] = ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() * fScinTdcToTime;
	      time[ihit] = time[ihit] - ( fHodoPosPhcCoeff[sIndex] *
			   TMath::Sqrt( TMath::Max( 0. , ( ( adcPh[ihit] / fHodoPosMinPh[sIndex] ) - 1 ) ) ) );
	      time[ihit] = time[ihit] - ( path[ihit] / fHodoVelLight[sIndex] );
	      scinPosTime[ihit] = time[ihit] - fHodoPosTimeOffset[sIndex];
	      
	    } // check for good pos TDC condition
	    
	    // ** Repeat for pmts on 'negative' side
	    if ( ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() < fScinTdcMax ) &&
		 ( keepNeg[ihit] ) ) { //
	      
	      // ** Calculate time for each tube with a good tdc. 'pos' side first.
	      goodTDCNeg[itrack][ihit] = kTRUE;
	      hntof ++;
	      adcPh[ihit] = ((THcSignalHit*)scinNegADC->At(ihit))->GetData();
	      path[ihit] = scinLongCoord - fPlanes[ip]->GetPosRight(); // pos = left, neg = right
	      
	      //* Convert TDC value to time, do pulse height correction, correction for
	      //* propogation of light thru scintillator, and offset.
	      time[ihit] = ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() * fScinTdcToTime;
	      time[ihit] = time[ihit] - ( fHodoNegPhcCoeff[sIndex] *
			   TMath::Sqrt( TMath::Max( 0. , ( ( adcPh[ihit] / fHodoNegMinPh[sIndex] ) - 1 ) ) ) );
	      time[ihit] = time[ihit] - ( path[ihit] / fHodoVelLight[sIndex] );
	      scinNegTime[ihit] = time[ihit] - fHodoNegTimeOffset[sIndex];
	      
	    } // check for good neg TDC condition
	    
	    // ** Calculate ave time for scin and error.
	    if ( goodTDCPos[itrack][ihit] ){
	      if ( goodTDCNeg[itrack][ihit] ){	
		scinTime[ihit] = ( scinPosTime[ihit] + scinNegTime[ihit] ) / 2.;
		scinSigma[ihit] = TMath::Sqrt( fHodoPosSigma[sIndex] * fHodoPosSigma[sIndex] +
					       fHodoNegSigma[sIndex] * fHodoNegSigma[sIndex] )/2.;
		goodScinTime[itrack][ihit] = kTRUE;
		hntofPairs ++;
	      }
	      else{
		scinTime[ihit] = scinPosTime[ihit];
		scinSigma[ihit] = fHodoPosSigma[sIndex];
		goodScinTime[itrack][ihit] = kTRUE;	
	      }
	    }
	    else {
	      if ( goodTDCNeg[itrack][ihit] ){
		scinTime[ihit] = scinNegTime[ihit];
		scinSigma[ihit] = fHodoNegSigma[sIndex];
		goodScinTime[itrack][ihit] = kTRUE;	
	      }
	    } // In h_tof.f this is includes the following if condition for time at focal plane
	    // // because it is written in FORTRAN code

	    // c     Get time at focal plane
	    if ( goodScinTime[itrack][ihit] ){
	      scinTimeFP[ihit] = scinTime[ihit] -
		( fPlanes[ip]->GetZpos() + ( hitPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) /
		//( 29.979 * betaPcent ) *
		( 29.979 * betap ) *
		TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
			     theTrack->GetPhi() * theTrack->GetPhi() );

	      sumFPTime = sumFPTime + scinTimeFP[ihit];
	      numFPTime ++;
	      sumPlaneTime[ip] = sumPlaneTime[ip] + scinTimeFP[ihit];
	      numPlaneTime[ip] ++;
	      numScinHit[itrack] ++;
	      scinHit[itrack][numScinHit[itrack]] = ihit;
	      scinFPTime[itrack][numScinHit[itrack]] = scinTimeFP[ihit];
	      

	      if ( ( goodTDCPos[itrack][ihit] ) && ( goodTDCNeg[itrack][ihit] ) ){
		numPmtHit[itrack] = numPmtHit[itrack] + 2;
	      }
	      else {
		numPmtHit[itrack] = numPmtHit[itrack] + 1;
	      }

	      if ( goodTDCPos[itrack][ihit] ){
		if ( goodTDCNeg[itrack][ihit] ){
		  dedX[itrack][numScinHit[itrack]] =
		    TMath::Sqrt( TMath::Max( 0., ((THcSignalHit*)scinPosADC->At(ihit))->GetData() *
					     ((THcSignalHit*)scinNegADC->At(ihit))->GetData() ) );
		}
		else{
		  dedX[itrack][numScinHit[itrack]] =
		    TMath::Max( 0., ((THcSignalHit*)scinPosADC->At(ihit))->GetData() );
		}
	      }
	      else{
		if ( goodTDCNeg[itrack][ihit] ){
		  dedX[itrack][numScinHit[itrack]] =
		    TMath::Max( 0., ((THcSignalHit*)scinNegADC->At(ihit))->GetData() );
		}
		else{
		  dedX[itrack][numScinHit[itrack]] = 0.;
		}
	      }
	    } // time at focal plane condition
	  } // on track else condition
	  
	  // ** See if there are any good time measurements in the plane.
	  if ( goodScinTime[itrack][ihit] ){
	    goodPlaneTime[itrack][ip] = kTRUE;
	  }
	  	  
	} // Second loop over hits of a scintillator plane ends here
      } // Loop over scintillator planes ends here
      
      // * * Fit beta if there are enough time measurements (one upper, one lower)
      if ( ( goodPlaneTime[itrack][0] ) || ( goodPlaneTime[itrack][1] ) ||
	   ( goodPlaneTime[itrack][2] ) || ( goodPlaneTime[itrack][3] ) ){
	
	// FineProcess();
      }
      else {
	beta[itrack] = 0.;
	betaChisq[itrack] = -1.;
      }
      if ( numFPTime != 0 ){
	timeAtFP[itrack] = ( sumFPTime / numFPTime );
      }
      
      for ( Int_t ind = 0; ind < fNPlanes; ind++ ){
	if ( numPlaneTime[ind] != 0 ){
	  FPTime[ind] = ( sumPlaneTime[ind] / numPlaneTime[ind] );
	}
	else{
	  FPTime[ind] = 1000. * ( ind + 1 );
	}
      }
      

      FPTimeDif1 = FPTime[0] - FPTime[1];
      FPTimeDif2 = FPTime[0] - FPTime[2];
      FPTimeDif3 = FPTime[0] - FPTime[3];
      FPTimeDif4 = FPTime[1] - FPTime[2];
      FPTimeDif5 = FPTime[1] - FPTime[3];
      FPTimeDif6 = FPTime[2] - FPTime[3];
           
    } // Main loop over tracks ends here.    
  } // If condition for at least one track
  
  
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


  Double_t sumw, sumt, sumz, sumzz, sumtz;
  Double_t scinWeight, tmp, t0, tmpDenom, parthNorm;
  Int_t i, itrack;

  sumw = 0.;
  sumt = 0.;
  sumz = 0.;
  sumzz = 0.;
  sumtz = 0.;

  Int_t Ntracks = tracks.GetLast()+1; // Number of reconstructed tracks

  if ( Ntracks > 0 )
    cout << "scin good time = " << endl; // goodScinTime[0][0] << endl;

  //  Int_t scinHits = fPlanes[ip]->GetNScinHits();

  // for ( i = 0; i < scinHits; i++ ){

  // }


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
