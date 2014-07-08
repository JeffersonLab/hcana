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
#include <vector>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;
using std::vector;

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
    {"fpBeta","Beta of the track","fBeta"},
    {"fpBetaChisq","Chi square of the track","fBetaChisq"},
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

  delete [] FPTime;               fNPaddle = NULL;           // Ahmed
  delete [] fBeta;                fBeta = NULL;              // Ahmed
  delete [] fBetaChisq;           fBetaChisq = NULL;         // Ahmed
  delete [] fHitPaddle;           fHitPaddle = NULL;         // Ahmed
  delete [] fNScinHit;            fNScinHit = NULL;          // Ahmed
  delete [] fNPmtHit;             fNPmtHit = NULL;           // Ahmed
  delete [] fTimeHist;            fTimeHist = NULL;          // Ahmed
  delete [] fTimeAtFP;            fTimeAtFP = NULL;          // Ahmed

  delete [] fScinSigma;           fScinSigma = NULL;         // Ahmed
  delete [] fGoodScinTime;        fGoodScinTime = NULL;      // Ahmed
  delete [] fScinTime;            fScinTime = NULL;          // Ahmed
  delete [] fTime;                fTime = NULL;              // Ahmed
  delete [] adcPh;                adcPh = NULL;              // Ahmed
  delete [] fKeepPos;             fKeepPos = NULL;           // Ahmed
  delete [] fKeepNeg;             fKeepNeg = NULL;           // Ahmed
  delete [] fGoodPlaneTime;       fGoodPlaneTime = NULL;     // Ahmed
  delete [] fPath;                fPath = NULL;              // Ahmed
  delete [] fTimePos;             fTimePos = NULL;           // Ahmed
  delete [] fTimeNeg;             fTimeNeg = NULL;           // Ahmed
  delete [] fGoodTDCPos;          fGoodTDCPos = NULL;        // Ahmed
  delete [] fGoodTDCNeg;          fGoodTDCNeg = NULL;        // Ahmed
  delete [] fScinTimefp;          fScinTimefp = NULL;        // Ahmed
  delete [] fScinPosTime;         fScinPosTime = NULL;       // Ahmed
  delete [] fScinNegTime;         fScinNegTime = NULL;       // Ahmed
  delete [] fNPlaneTime;          fNPlaneTime = NULL;        // Ahmed
  delete [] fSumPlaneTime;        fSumPlaneTime = NULL;      // Ahmed

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
    
    // if ( !fPlanes[ip] )     // Ahmed
    //   return;               // Ahmed
    
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
  // if (evdata.GetEvNum()>1000) 
  //   cout <<"\nhcana event = " << evdata.GetEvNum()<<endl;
  
  if(gHaCuts->Result("Pedestal_event")) {
    Int_t nexthit = 0;
    for(Int_t ip=0;ip<fNPlanes;ip++) {
      
      // if ( !fPlanes[ip] )     // Ahmed
      // 	return -1;            // Ahmed
      
      nexthit = fPlanes[ip]->AccumulatePedestals(fRawHitList, nexthit);
    }
    fAnalyzePedestals = 1;	// Analyze pedestals first normal events
    return(0);
  }
  if(fAnalyzePedestals) {
    for(Int_t ip=0;ip<fNPlanes;ip++) {

      // if ( !fPlanes[ip] )     // Ahmed
      // 	return -1;            // Ahmed      
      
      fPlanes[ip]->CalculatePedestals();
    }
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  // Let each plane get its hits
  Int_t nexthit = 0;

  fStartTime=0;
  fNfptimes=0;
  for(Int_t ip=0;ip<fNPlanes;ip++) {

    // if ( !fPlanes[ip] )     // Ahmed
    //   return -1;               // Ahmed
    
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
  //  cout << endl;
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

  Int_t Ntracks = tracks.GetLast()+1; // Number of reconstructed tracks
  Int_t fPaddle = 0, fIndex, k, fJMax, fMaxHit, ip, ihit;
  Int_t fNfpTime, fSumfpTime, fNScinHits; 
  Double_t fScinTrnsCoord, fScinLongCoord, fScinCenter;
  Double_t fP, fBetaP, fXcoord, fYcoord, fTMin;

  //  Int_t fNtof, fNtofPairs;
  // -------------------------------------------------

  fBetaChisq = new Double_t [53];
  fBeta = new Double_t [53];
  
  fKeepPos = new Bool_t [53]; 
  fKeepNeg = new Bool_t [53]; 
  fGoodTDCPos = new Bool_t [53];
  fGoodTDCNeg = new Bool_t [53];

  FPTime = new Double_t [53];        
  fHitPaddle = new Int_t [53];    
  fNScinHit = new Int_t [53];     
  fNPmtHit = new Int_t [53];      
  fTimeHist = new Int_t [53];     
  fTimeAtFP = new Double_t [53];     
  
  fScinSigma = new Double_t [53];    
  fGoodScinTime = new Double_t [53]; 
  fScinTime = new Double_t [53];     
  fTime = new Double_t [53];         
  adcPh = new Double_t [53];  

  fPath = new Double_t [53];
  fTimePos = new Double_t [53];
  fTimeNeg = new Double_t [53];
  fScinTimefp = new Double_t [53];	
       
  fTimeHist = new Int_t [200];

  // -------------------------------------------------

  Double_t hpartmass=0.00051099; // Fix it
 
  for ( Int_t m = 0; m < 53; m++ ){

    fScinSigma[m] = 0.;
    fHitPaddle[m] = 0.;
    fScinTime[m] = 0.;

    fTime[m] = 0.;
    adcPh[m] = 0.;
    fBetaChisq[m] = 0.;
    fBeta[m] = 0.;
    fPath[m] = 0.;
    fTimePos[m] = 0.;
    fTimeNeg[m] = 0.;
    fScinTimefp[m] = 0.; 
    fGoodScinTime[m] = kFALSE;
    //    fScinGoodTime[m] = kFALSE;
    
    fGoodTDCPos[m] = kFALSE;
    fGoodTDCNeg[m] = kFALSE;
    fKeepPos[m] = kFALSE;
    fKeepNeg[m] = kFALSE;

  }


  if (tracks.GetLast()+1 > 0 ) {

    // **MAIN LOOP: Loop over all tracks and get corrected time, tof, beta...
    for ( Int_t itrack = 0; itrack < Ntracks; itrack++ ) { // Line 133

      THaTrack* theTrack = dynamic_cast<THaTrack*>( tracks.At(itrack) );
      if (!theTrack) return -1;
      
      fGoodPlaneTime = new Bool_t[4];
      for ( ip = 0; ip < fNPlanes; ip++ ){ fGoodPlaneTime[ip] = kFALSE; }

      fNfpTime = 0;
      fBetaChisq[itrack] = -3;
      fTimeAtFP[itrack] = 0.;
      fSumfpTime = 0.; // Line 138
      fNScinHit[itrack] = 0; // Line 140
      fP = theTrack->GetP(); // Line 142 Fix it
      fBetaP = fP/( TMath::Sqrt( fP * fP + hpartmass * hpartmass) );
      
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
      
      for (Int_t j=0; j<200; j++) { fTimeHist[j]=0; } // Line 176
      
      fNPlaneTime =  new Int_t [4];
      fSumPlaneTime =  new Double_t [4];

      for ( ip = 0; ip <  fNPlanes; ip++ ){ 
	fNPlaneTime[ip] = 0;
	fSumPlaneTime[ip] = 0.;
      }
      // Loop over scintillator planes.
      // In ENGINE, its loop over good scintillator hits.
      
      fGoodTimeIndex = -1;
      for( ip = 0; ip < fNPlanes; ip++ ) {
	
	fNScinHits = fPlanes[ip]->GetNScinHits();

	// first loop over hits with in a single plane
	for ( ihit = 0; ihit < fNScinHits; ihit++ ){
	  	  
	  fTimePos[ihit] = -99.;
	  fTimeNeg[ihit] = -99.;
	  fKeepPos[ihit] = kFALSE;
	  fKeepNeg[ihit] = kFALSE;
	  
	  scinPosADC = fPlanes[ip]->GetPosADC();
	  scinNegADC = fPlanes[ip]->GetNegADC();
	  scinPosTDC = fPlanes[ip]->GetPosTDC();
	  scinNegTDC = fPlanes[ip]->GetNegTDC();  
	  	  
	  fPaddle = ((THcSignalHit*)scinPosTDC->At(ihit))->GetPaddleNumber()-1;
	  
	  fXcoord = theTrack->GetX() + theTrack->GetTheta() *
	    ( fPlanes[ip]->GetZpos() +
	      ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 183
	  
	  fYcoord = theTrack->GetY() + theTrack->GetPhi() *
	    ( fPlanes[ip]->GetZpos() +
	      ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 184
	  	  
	  if ( ( ip == 0 ) || ( ip == 2 ) ){ // !x plane. Line 185
	    fScinTrnsCoord = fXcoord;
	    fScinLongCoord = fYcoord;
	  }
	  
	  else if ( ( ip == 1 ) || ( ip == 3 ) ){ // !y plane. Line 188
	    fScinTrnsCoord = fYcoord;
	    fScinLongCoord = fXcoord;
	  }
	  else { return -1; } // Line 195
	  
	  fScinCenter = fPlanes[ip]->GetPosCenter(fPaddle) + fPlanes[ip]->GetPosOffset();
	  fIndex = fNPlanes * fPaddle + ip;
	  

	  if ( TMath::Abs( fScinCenter - fScinTrnsCoord ) <
	       ( fPlanes[ip]->GetSize() * 0.5 + fPlanes[ip]->GetHodoSlop() ) ){ // Line 293
	    
	    if ( ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() < fScinTdcMax ) ) { // Line 199
	      
	      adcPh[ihit] = ((THcSignalHit*)scinPosADC->At(ihit))->GetData();
	      fPath[ihit] = fPlanes[ip]->GetPosLeft() - fScinLongCoord;
	      fTime[ihit] = ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() * fScinTdcToTime;
	      fTime[ihit] = fTime[ihit] - fHodoPosPhcCoeff[fIndex] *
		           TMath::Sqrt( TMath::Max( 0., ( ( adcPh[ihit] / fHodoPosMinPh[fIndex] ) - 1 ) ) );
	      fTime[ihit] = fTime[ihit] - ( fPath[ihit] / fHodoVelLight[fIndex] ) - ( fPlanes[ip]->GetZpos() +  
			   ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * fBetaP ) *
		           TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
			   theTrack->GetPhi() * theTrack->GetPhi() );
	      fTimePos[ihit] = fTime[ihit] - fHodoPosTimeOffset[fIndex];
	      
	      for ( k = 0; k < 200; k++ ){ // Line 211
		fTMin = 0.5 * ( k + 1 ) ;
		if ( ( fTimePos[ihit] > fTMin ) && ( fTimePos[ihit] < ( fTMin + fTofTolerance ) ) )
		  fTimeHist[k] ++;
	      }
	    } // TDC pos hit condition
	    
	    
	    if ( ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() < fScinTdcMax ) ) { // Line 218
	      
	      adcPh[ihit] = ((THcSignalHit*)scinNegADC->At(ihit))->GetData();
	      fPath[ihit] = fScinLongCoord - fPlanes[ip]->GetPosRight();
	      fTime[ihit] = ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() * fScinTdcToTime;
	      fTime[ihit] =fTime[ihit] - fHodoNegPhcCoeff[fIndex] * 
	                   TMath::Sqrt( TMath::Max( 0., ( ( adcPh[ihit] / fHodoNegMinPh[fIndex] ) - 1 ) ) );
	      fTime[ihit] = fTime[ihit] - ( fPath[ihit] / fHodoVelLight[fIndex] ) - ( fPlanes[ip]->GetZpos() +
			   ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) / ( 29.979 * fBetaP ) *
		           TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
			   theTrack->GetPhi() * theTrack->GetPhi() );
	      fTimeNeg[ihit] = fTime[ihit] - fHodoNegTimeOffset[fIndex];
	      
	      for ( k = 0; k < 200; k++ ){ // Line 230
		fTMin = 0.5 * ( k + 1 );
		if ( ( fTimeNeg[ihit] > fTMin ) && ( fTimeNeg[ihit] < ( fTMin + fTofTolerance ) ) )
		  fTimeHist[k] ++;
	      }
	    } // TDC neg hit condition
	  } // condition for cenetr on a paddle
	} // Loop over hits in a plane

	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	//------------- First large loop over scintillator hits in a plane ends here --------------------
	
	fJMax = 0; // Line 240
	fMaxHit = 0;
	
	for ( k = 0; k < 200; k++ ){
	  if ( fTimeHist[k] > fMaxHit ){
	    fJMax = k+1;
	    fMaxHit = fTimeHist[k];
	  }
	}
	
	if ( fJMax >= 0 ){ // Line 248. Here I followed the code of THcSCintilaltorPlane::PulseHeightCorrection
	  fTMin = 0.5 * fJMax;
	  for( ihit = 0; ihit < fNScinHits; ihit++) { // Loop over sinc. hits. in plane
	    if ( ( fTimePos[ihit] > fTMin ) && ( fTimePos[ihit] < ( fTMin + fTofTolerance ) ) ) {
	      fKeepPos[ihit]=kTRUE;
	    }	
	    if ( ( fTimeNeg[ihit] > fTMin ) && ( fTimeNeg[ihit] < ( fTMin + fTofTolerance ) ) ){
	      fKeepNeg[ihit] = kTRUE;
	    }	
	  }
	} // fJMax > 0 condition
	
	fScinPosTime = new Double_t [53];
	fScinNegTime = new Double_t [53];

	for ( k = 0; k < 53; k++ ){
	  fScinPosTime[k] = 0;
	  fScinNegTime[k] = 0;
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------	
	// ---------------------- Scond loop over scint. hits in a plane ------------------------------
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------


	// Second loop over hits with in a single plane

	for ( ihit = 0; ihit < fNScinHits; ihit++ ){
	  
	  fGoodTimeIndex ++;

	  fPaddle = ((THcSignalHit*)scinPosTDC->At(ihit))->GetPaddleNumber()-1;
	  fHitPaddle[fGoodTimeIndex] =  fPaddle;
	  
	  fXcoord = theTrack->GetX() + theTrack->GetTheta() *
	    ( fPlanes[ip]->GetZpos() + ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 277
	  fYcoord = theTrack->GetY() + theTrack->GetPhi() *
	    ( fPlanes[ip]->GetZpos() + ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ); // Line 278
	  
	  
	  if ( ( ip == 0 ) || ( ip == 2 ) ){ // !x plane. Line 278
	    fScinTrnsCoord = fXcoord;
	    fScinLongCoord = fYcoord;
	  }
	  else if ( ( ip == 1 ) || ( ip == 3 ) ){ // !y plane. Line 281
	    fScinTrnsCoord = fYcoord;
	    fScinLongCoord = fXcoord;
	  }
	  else { return -1; } // Line 288
	  
	  fScinCenter = fPlanes[ip]->GetPosCenter(fPaddle) + fPlanes[ip]->GetPosOffset();
	  fIndex = fNPlanes * fPaddle + ip;
	  
	  // ** Check if scin is on track
	  if ( TMath::Abs( fScinCenter - fScinTrnsCoord ) >
	       ( fPlanes[ip]->GetSize() * 0.5 + fPlanes[ip]->GetHodoSlop() ) ){ // Line 293
	    //	    scinOnTrack[itrack][ihit] = kFALSE;
	  }
	  else{
	    //	    scinOnTrack[itrack][ihit] = kTRUE;
	    
	    // * * Check for good TDC
	    if ( ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() < fScinTdcMax ) &&
		 ( fKeepPos[ihit] ) ) { // 301
	      
	      // ** Calculate time for each tube with a good tdc. 'pos' side first.
	      fGoodTDCPos[fGoodTimeIndex] = kTRUE;
	      //	      fNtof ++;
	      adcPh[ihit] = ((THcSignalHit*)scinPosADC->At(ihit))->GetData();
	      fPath[ihit] = fPlanes[ip]->GetPosLeft() - fScinLongCoord;
	      
	      // * Convert TDC value to time, do pulse height correction, correction for
	      // * propogation of light thru scintillator, and offset.
	      
	      fTime[ihit] = ((THcSignalHit*)scinPosTDC->At(ihit))->GetData() * fScinTdcToTime;
	      fTime[ihit] = fTime[ihit] - ( fHodoPosPhcCoeff[fIndex] * TMath::Sqrt( TMath::Max( 0. , 
					( ( adcPh[ihit] / fHodoPosMinPh[fIndex] ) - 1 ) ) ) );
	      fTime[ihit] = fTime[ihit] - ( fPath[ihit] / fHodoVelLight[fIndex] );
	      fScinPosTime[ihit] = fTime[ihit] - fHodoPosTimeOffset[fIndex];
	      
	    } // check for good pos TDC condition
	    
	    // ** Repeat for pmts on 'negative' side
	    if ( ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() > fScinTdcMin ) &&
		 ( ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() < fScinTdcMax ) &&
		 ( fKeepNeg[ihit] ) ) { //
	      
	      // ** Calculate time for each tube with a good tdc. 'pos' side first.
	      fGoodTDCNeg[fGoodTimeIndex] = kTRUE;
	      //	      fNtof ++;
	      adcPh[ihit] = ((THcSignalHit*)scinNegADC->At(ihit))->GetData();
	      fPath[ihit] = fScinLongCoord - fPlanes[ip]->GetPosRight(); // pos = left, neg = right
	      
	      // * Convert TDC value to time, do pulse height correction, correction for
	      // * propogation of light thru scintillator, and offset.
	      fTime[ihit] = ((THcSignalHit*)scinNegTDC->At(ihit))->GetData() * fScinTdcToTime;
	      fTime[ihit] = fTime[ihit] - ( fHodoNegPhcCoeff[fIndex] *
			   TMath::Sqrt( TMath::Max( 0. , ( ( adcPh[ihit] / fHodoNegMinPh[fIndex] ) - 1 ) ) ) );
	      fTime[ihit] = fTime[ihit] - ( fPath[ihit] / fHodoVelLight[fIndex] );
	      fScinNegTime[ihit] = fTime[ihit] - fHodoNegTimeOffset[fIndex];
	      
	    } // check for good neg TDC condition
	    
	    // ** Calculate ave time for scin and error.
	    if ( fGoodTDCPos[fGoodTimeIndex] ){
	      if ( fGoodTDCNeg[fGoodTimeIndex] ){	
		fScinTime[fGoodTimeIndex]  = ( fScinPosTime[ihit] + fScinNegTime[ihit] ) / 2.;
		fScinSigma[fGoodTimeIndex] = TMath::Sqrt( fHodoPosSigma[fIndex] * fHodoPosSigma[fIndex] + 
							  fHodoNegSigma[fIndex] * fHodoNegSigma[fIndex] )/2.;
		fGoodScinTime[fGoodTimeIndex] = kTRUE;
		//		fScinGoodTime[fGoodTimeIndex] = kTRUE;
		//		fNtofPairs ++;
	      }
	      else{
		fScinTime[fGoodTimeIndex] = fScinPosTime[ihit];
		fScinSigma[fGoodTimeIndex] = fHodoPosSigma[fIndex];
		fGoodScinTime[fGoodTimeIndex] = kTRUE;
		//		fScinGoodTime[fGoodTimeIndex] = kTRUE;
	      }
	    }
	    else {
	      if ( fGoodTDCNeg[fGoodTimeIndex] ){
		fScinTime[fGoodTimeIndex] = fScinNegTime[ihit];
		fScinSigma[fGoodTimeIndex] = fHodoNegSigma[fIndex];
		fGoodScinTime[fGoodTimeIndex] = kTRUE;
		//	fScinGoodTime[fGoodTimeIndex] = kTRUE;
	      }
	    } // In h_tof.f this includes the following if condition for time at focal plane
	    // // because it is written in FORTRAN code

	    // c     Get time at focal plane
	    if ( fGoodScinTime[fGoodTimeIndex] ){

	      // ---------------------------------------------------------------------------
	      // Date: July 8 2014
	      //
	      // Right now we do not need this code for beta and chisquare
	      //
	      // fScinTimefp[ihit] = fScinTime[fGoodTimeIndex] -
	      // 	( fPlanes[ip]->GetZpos() + ( fPaddle % 2 ) * fPlanes[ip]->GetDzpos() ) /
	      // 	( 29.979 * fBetaP ) *
	      // 	TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() +
	      // 		     theTrack->GetPhi() * theTrack->GetPhi() );
	      //
	      // fSumfpTime = fSumfpTime + fScinTimefp[ihit];
	      // fNfpTime ++;
	      //
	      // ---------------------------------------------------------------------------

	      fSumPlaneTime[ip] = fSumPlaneTime[ip] + fScinTimefp[ihit];
	      fNPlaneTime[ip] ++;
	      fNScinHit[itrack] ++;
	      //	      scinHit[itrack][fNScinHit[itrack]] = ihit;
	      //	      scinFPTime[itrack][fNScinHit[itrack]] = fScinTimefp[ihit];
	      

	      // ---------------------------------------------------------------------------
	      // Date: July 8 2014
	      // This counts the pmt hits. Right now we don't need it so it is commentd off
	      //
	      // if ( ( fGoodTDCPos[fGoodTimeIndex] ) && ( fGoodTDCNeg[fGoodTimeIndex] ) ){
	      // 	fNPmtHit[itrack] = fNPmtHit[itrack] + 2;
	      // }
	      // else {
	      // 	fNPmtHit[itrack] = fNPmtHit[itrack] + 1;
	      // }
	      // ---------------------------------------------------------------------------


	      // if ( fGoodTDCPos[itrack][ihit] ){
	      // 	if ( fGoodTDCNeg[itrack][ihit] ){
	      // 	  //		  dedX[itrack][fNScinHit[itrack]] =
	      // 	  //		    TMath::Sqrt( TMath::Max( 0., ((THcSignalHit*)scinPosADC->At(ihit))->GetData() *
	      // 	  //					     ((THcSignalHit*)scinNegADC->At(ihit))->GetData() ) );
	      // 	}
	      // 	else{
	      // 	  //		  dedX[itrack][fNScinHit[itrack]] =
	      // 	  //		    TMath::Max( 0., ((THcSignalHit*)scinPosADC->At(ihit))->GetData() );
	      // 	}
	      // }
	      // else{
	      // 	if ( fGoodTDCNeg[itrack][ihit] ){
	      // 	  //		  dedX[itrack][fNScinHit[itrack]] =
	      // 	  //		    TMath::Max( 0., ((THcSignalHit*)scinNegADC->At(ihit))->GetData() );
	      // 	}
	      // 	else{
	      // 	  //		  dedX[itrack][fNScinHit[itrack]] = 0.;
	      // 	}
	      // }


	    } // time at focal plane condition
	  } // on track else condition
	  
	  // ** See if there are any good time measurements in the plane.
	  if ( fGoodScinTime[fGoodTimeIndex] ){
	    fGoodPlaneTime[ip] = kTRUE;
	  }
	  	  
	} // Second loop over hits of a scintillator plane ends here
      } // Loop over scintillator planes ends here

      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------
      //------------------------------------------------------------------------------

      // * * Fit beta if there are enough time measurements (one upper, one lower)
      if ( ( ( fGoodPlaneTime[0] ) || ( fGoodPlaneTime[1] ) ) && 
	   ( ( fGoodPlaneTime[2] ) || ( fGoodPlaneTime[3] ) ) ){	
	
	Double_t sumw, sumt, sumz, sumzz, sumtz;
	Double_t scinWeight, tmp, t0, tmpDenom, pathNorm, zPosition, timeDif;
	
	sumw = 0.;	sumt = 0.;	sumz = 0.;	sumzz = 0.;	sumtz = 0.;
	
	fGoodTimeIndex = -1;  
	for ( ip = 0; ip < fNPlanes; ip++ ){

	  if (!fPlanes[ip])
	    return -1;
	  
	  fNScinHits = fPlanes[ip]->GetNScinHits();	  
	  for (ihit = 0; ihit < fNScinHits; ihit++ ){
	    fGoodTimeIndex ++;
	    
	    if ( fGoodScinTime[fGoodTimeIndex] ) {
	      
	      scinWeight = 1 / ( fScinSigma[fGoodTimeIndex] * fScinSigma[fGoodTimeIndex] );
	      zPosition = ( fPlanes[ip]->GetZpos() + ( fHitPaddle[fGoodTimeIndex] % 2 ) * fPlanes[ip]->GetDzpos() );
	      
	      sumw += scinWeight;
	      sumt += scinWeight * fScinTime[fGoodTimeIndex];
	      sumz += scinWeight * zPosition;
	      sumzz += scinWeight * ( zPosition * zPosition );
	      sumtz += scinWeight * zPosition * fScinTime[fGoodTimeIndex];
	      
	      	      
	    } // condition of good scin time
	  } // loop over hits of plane
	} // loop over planes

	tmp = sumw * sumzz - sumz * sumz ;
	t0 = ( sumt * sumzz - sumz * sumtz ) / tmp ;
	tmpDenom = sumw * sumtz - sumz * sumt;

	if ( TMath::Abs( tmpDenom ) > ( 1 / 10000000000.0 ) ) {
	  
	  fBeta[itrack] = tmp / tmpDenom;
	  fBetaChisq[itrack] = 0.;
	  
	  fGoodTimeIndex = -1;
	  for ( ip = 0; ip < fNPlanes; ip++ ){                           // Loop over planes
	    if (!fPlanes[ip])
	      return -1;
	    
	    fNScinHits = fPlanes[ip]->GetNScinHits();	  
	    for (ihit = 0; ihit < fNScinHits; ihit++ ){                    // Loop over hits of a plane
	      fGoodTimeIndex ++;
	      
	      if ( fGoodScinTime[fGoodTimeIndex] ){
		
		zPosition = ( fPlanes[ip]->GetZpos() + ( fHitPaddle[fGoodTimeIndex] % 2 ) * fPlanes[ip]->GetDzpos() );
		timeDif = ( fScinTime[fGoodTimeIndex] - t0 );
		
		fBetaChisq[itrack] += ( ( zPosition / fBeta[itrack] - timeDif ) * ( zPosition / fBeta[itrack] - timeDif ) )  / 
		  ( fScinSigma[fGoodTimeIndex] * fScinSigma[fGoodTimeIndex] );
		
	      } // condition for good scin time
	    } // loop over hits of a plane
	  } // loop over planes
	  
	  pathNorm = TMath::Sqrt( 1. + theTrack->GetTheta() * theTrack->GetTheta() + theTrack->GetPhi() * theTrack->GetPhi() );
	  fBeta[itrack] = fBeta[itrack] / pathNorm;
	  fBeta[itrack] = fBeta[itrack] / 29.979;    // velocity / c
	  
	  // cout << "track = " << itrack + 1 
	  //      << "   beta = " << fBeta[itrack] << endl;


	}  // condition for tmpDenom	
	else {
	  fBeta[itrack] = 0.;
	  fBetaChisq[itrack] = -2.;
	} // else condition for tmpDenom
	
	
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	// -------------------------------------------------------------------- 
	

      }
      else {
	fBeta[itrack] = 0.;
	fBetaChisq[itrack] = -1;
      }
      
      // ---------------------------------------------------------------------------
      // Date: July 8 2014
      //
      // Right now we do not need this code for beta and chisquare
      //
      // if ( fNfpTime != 0 ){
      // 	// fTimeAtFP[itrack] = ( fSumfpTime / fNfpTime ); // Right now we don't need fTimeAtFP
      // }
      //
      // ---------------------------------------------------------------------------
      
      
      for ( Int_t ind = 0; ind < fNPlanes; ind++ ){
	if ( fNPlaneTime[ind] != 0 ){
	  FPTime[ind] = ( fSumPlaneTime[ind] / fNPlaneTime[ind] );
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
  



  ApplyCorrections();

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoscope::FineProcess( TClonesArray& tracks )
{
  
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
