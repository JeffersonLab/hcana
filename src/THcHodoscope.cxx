///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope
//                                                                           //
// Class for a generic hodoscope consisting of multiple                      //
// planes with multiple paddles with phototubes on both ends.                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoscope.h"
#include "THaEvData.h"
#include "THaDetMap.h"
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
THcHodoscope::THcHodoscope( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THaNonTrackingDetector(name,description,apparatus)
{
  // Constructor
  fTWalkPar = 0;

  fTrackProj = new TClonesArray( "THaTrackProj", 5 );
}

//_____________________________________________________________________________
THcHodoscope::THcHodoscope( ) :
  THaNonTrackingDetector()
{
  // Constructor
  fTWalkPar = NULL;
  fTrackProj = NULL;
  fRA_c = fRA_p = fRA = fLA_c = fLA_p = fLA = NULL;
  fRT_c = fRT = fLT_c = fLT = NULL;
  fRGain = fLGain = fRPed = fLPed = fROff = fLOff = NULL;
  fTrigOff = fTime = fdTime = fYt = fYa = NULL;
  fHitPad = NULL;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoscope::Init( const TDatime& date )
{
  // Extra initialization for scintillators: set up DataDest map

  if( THaNonTrackingDetector::Init( date ) )
    return fStatus;

  const DataDest tmp[NDEST] = {
    { &fRTNhit, &fRANhit, fRT, fRT_c, fRA, fRA_p, fRA_c, fROff, fRPed, fRGain },
    { &fLTNhit, &fLANhit, fLT, fLT_c, fLA, fLA_p, fLA_c, fLOff, fLPed, fLGain }
  };
  memcpy( fDataDest, tmp, NDEST*sizeof(DataDest) );

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map

  THcHitList::InitHitList(fDetMap, "THcHodoscopeHit", 100);
  if( gHcDetectorMap->FillMap("detectorname",fDetMap) < 0 ) {
      Error( Here(here), "Error filling detectormap for %s.", 
	     "detectorname");
      return kInitError;
  }

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcHodoscope::ReadDatabase( const TDatime& date )
{
  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  static const char* const here = "ReadDatabase()";
  const int LEN = 200;
  char buf[LEN];
  Int_t nelem;

  // Read data from database 
  FILE* fi = OpenFile( date );
  if( !fi ) return kFileError;

  while ( ReadComment( fi, buf, LEN ) ) {}
  fscanf ( fi, "%d", &nelem );                        // Number of  paddles
  fgets ( buf, LEN, fi );

  // Reinitialization only possible for same basic configuration 
  if( fIsInit && nelem != fNelem ) {
    Error( Here(here), "Cannot re-initalize with different number of paddles. "
	   "(was: %d, now: %d). Detector not re-initialized.", fNelem, nelem );
    fclose(fi);
    return kInitError;
  }
  fNelem = nelem;

  // Read detector map. Unless a model-number is given
  // for the detector type, this assumes that the first half of the entries 
  // are for ADCs and the second half, for TDCs.
  while ( ReadComment( fi, buf, LEN ) ) {}
  int i = 0;
  fDetMap->Clear();
  while (1) {
    int pos;
    Int_t first_chan, model;
    Int_t crate, slot, first, last;
    fgets ( buf, LEN, fi );
    sscanf( buf, "%d%d%d%d%d%n", &crate, &slot, &first, &last, &first_chan, &pos );
    if( crate < 0 ) break;
    model=atoi(buf+pos); // if there is no model number given, set to zero
    
    if( fDetMap->AddModule( crate, slot, first, last, first_chan, model ) < 0 ) {
      Error( Here(here), "Too many DetMap modules (maximum allowed - %d).", 
	     THaDetMap::kDetMapSize);
      fclose(fi);
      return kInitError;
    }
  }
  while ( ReadComment( fi, buf, LEN ) ) {}

  Float_t x,y,z;
  fscanf ( fi, "%f%f%f", &x, &y, &z );             // Detector's X,Y,Z coord
  fgets ( buf, LEN, fi );
  fOrigin.SetXYZ( x, y, z );
  fgets ( buf, LEN, fi );
  while ( ReadComment( fi, buf, LEN ) ) {}
  fscanf ( fi, "%f%f%f", fSize, fSize+1, fSize+2 ); // Sizes of det on X,Y,Z
  fgets ( buf, LEN, fi );
  while ( ReadComment( fi, buf, LEN ) ) {}

  Float_t angle;
  fscanf ( fi, "%f", &angle );                     // Rotation angle of detector
  fgets ( buf, LEN, fi );
  const Float_t degrad = TMath::Pi()/180.0;
  tan_angle = TMath::Tan(angle*degrad);
  sin_angle = TMath::Sin(angle*degrad);
  cos_angle = TMath::Cos(angle*degrad);

  DefineAxes(angle*degrad);

  // Dimension arrays
  if( !fIsInit ) {
    // Calibration data
    fLOff  = new Double_t[ fNelem ];
    fROff  = new Double_t[ fNelem ];
    fLPed  = new Double_t[ fNelem ];
    fRPed  = new Double_t[ fNelem ];
    fLGain = new Double_t[ fNelem ];
    fRGain = new Double_t[ fNelem ];

    fTrigOff = new Double_t[ fNelem ];

    // Per-event data
    fLT   = new Double_t[ fNelem ];
    fLT_c = new Double_t[ fNelem ];
    fRT   = new Double_t[ fNelem ];
    fRT_c = new Double_t[ fNelem ];
    fLA   = new Double_t[ fNelem ];
    fLA_p = new Double_t[ fNelem ];
    fLA_c = new Double_t[ fNelem ];
    fRA   = new Double_t[ fNelem ];
    fRA_p = new Double_t[ fNelem ];
    fRA_c = new Double_t[ fNelem ];

    fNTWalkPar = 2*fNelem; // 1 paramter per paddle
    fTWalkPar = new Double_t[ fNTWalkPar ];

    fHitPad = new Int_t[ fNelem ];   
    fTime   = new Double_t[ fNelem ]; // analysis indexed by paddle (yes, inefficient)
    fdTime  = new Double_t[ fNelem ];
    fAmpl   = new Double_t[ fNelem ];
    
    fYt     = new Double_t[ fNelem ];
    fYa     = new Double_t[ fNelem ];
    
    fIsInit = true;
  }
  memset(fTrigOff,0,fNelem*sizeof(fTrigOff[0]));

  // Set DEFAULT values here
  // TDC resolution (s/channel)
  fTdc2T = 0.1e-9;      // seconds/channel
  fResolution = fTdc2T; // actual timing resolution 
  // Speed of light in the scintillator material
  fCn = 1.7e+8;    // meters/second
  // Attenuation length
  fAttenuation = 0.7; // inverse meters
  // Time-walk correction parameters
  fAdcMIP = 1.e10;    // large number for offset, so reference is effectively disabled
  // timewalk coefficients for tw = coeff*(1./sqrt(ADC-Ped)-1./sqrt(ADCMip))
  for (int i=0; i<fNTWalkPar; i++) fTWalkPar[i]=0;
  // trigger-timing offsets (s)
  for (int i=0; i<fNelem; i++) fTrigOff[i]=0;
  
  
  DBRequest list[] = {
    { "TDC_offsetsL", fLOff, kDouble, fNelem },
    { "TDC_offsetsR", fROff, kDouble, fNelem },
    { "ADC_pedsL", fLPed, kDouble, fNelem },
    { "ADC_pedsR", fRPed, kDouble, fNelem },
    { "ADC_coefL", fLGain, kDouble, fNelem },
    { "ADC_coefR", fRGain, kDouble, fNelem },
    { "TDC_res",   &fTdc2T },
    { "TransSpd",  &fCn },
    { "AdcMIP",    &fAdcMIP },
    { "NTWalk",    &fNTWalkPar, kInt },
    { "Timewalk",  fTWalkPar, kDouble, 2*fNelem },
    { "ReTimeOff", fTrigOff, kDouble, fNelem },
    { "AvgRes",    &fResolution },
    { "Atten",     &fAttenuation },
    { 0 }
  };

#if 0
  if ( gHaDB && gHaDB->LoadValues(GetPrefix(),list,date) ) {
    goto exit;  // the new database existed -- we're finished
  }
#endif
  
  // otherwise, gHaDB is unavailable, use the old file database
  
  // Read in the timing/adc calibration constants
  // For fine-tuning of these data, we seek to a matching time stamp, or
  // if no time stamp found, to a "configuration" section. Examples:
  // 
  // [ 2002-10-10 15:30:00 ]
  // #comment line goes here
  // <left TDC offsets>
  // <right TDC offsets>
  // <left ADC peds>
  // <rigth ADC peds>
  // <left ADC coeff>
  // <right ADC coeff>
  //
  // if below aren't present, 'default' values are used
  // <TDC resolution: seconds/channel>
  // <speed-of-light in medium m/s>
  // <attenuation length m^-1>
  // <ADC of MIP>
  // <number of timewalk parameters>
  // <timewalk paramters>
  //
  //
  // or
  // 
  // [ config=highmom ]
  // comment line
  // ...etc.
  //
  if( SeekDBdate( fi, date ) == 0 && fConfig.Length() > 0 && 
      SeekDBconfig( fi, fConfig.Data() )) {}

  while ( ReadComment( fi, buf, LEN ) ) {}
  // Read calibration data
  for (i=0;i<fNelem;i++) 
    fscanf(fi,"%lf",fLOff+i);                    // Left Pads TDC offsets
  fgets ( buf, LEN, fi );   // finish line
  while ( ReadComment( fi, buf, LEN ) ) {}
  for (i=0;i<fNelem;i++) 
    fscanf(fi,"%lf",fROff+i);                    // Right Pads TDC offsets
  fgets ( buf, LEN, fi );   // finish line
  while ( ReadComment( fi, buf, LEN ) ) {}
  for (i=0;i<fNelem;i++) 
    fscanf(fi,"%lf",fLPed+i);                    // Left Pads ADC Pedest-s
  fgets ( buf, LEN, fi );   // finish line, etc.
  while ( ReadComment( fi, buf, LEN ) ) {}
  for (i=0;i<fNelem;i++) 
    fscanf(fi,"%lf",fRPed+i);                    // Right Pads ADC Pedes-s
  fgets ( buf, LEN, fi );
  while ( ReadComment( fi, buf, LEN ) ) {}
  for (i=0;i<fNelem;i++) 
    fscanf (fi,"%lf",fLGain+i);                  // Left Pads ADC Coeff-s
  fgets ( buf, LEN, fi );
  while ( ReadComment( fi, buf, LEN ) ) {}
  for (i=0;i<fNelem;i++) 
    fscanf (fi,"%lf",fRGain+i);                  // Right Pads ADC Coeff-s
  fgets ( buf, LEN, fi ); 


  while ( ReadComment( fi, buf, LEN ) ) {}
  // Here on down, look ahead line-by-line
  // stop reading if a '[' is found on a line (corresponding to the next date-tag)
  // read in TDC resolution (s/channel)
  if ( ! fgets(buf, LEN, fi) || strchr(buf,'[') ) goto exit;
  sscanf(buf,"%lf",&fTdc2T);
  fResolution = 3.*fTdc2T;      // guess at timing resolution

  while ( ReadComment( fi, buf, LEN ) ) {}
  // Speed of light in the scintillator material
  if ( !fgets(buf, LEN, fi) ||  strchr(buf,'[') ) goto exit;
  sscanf(buf,"%lf",&fCn);
  
  // Attenuation length (inverse meters)
  while ( ReadComment( fi, buf, LEN ) ) {}
  if ( !fgets ( buf, LEN, fi ) ||  strchr(buf,'[') ) goto exit;
  sscanf(buf,"%lf",&fAttenuation);
  
  while ( ReadComment( fi, buf, LEN ) ) {}
  // Time-walk correction parameters
  if ( !fgets(buf, LEN, fi) ||  strchr(buf,'[') ) goto exit;
  sscanf(buf,"%lf",&fAdcMIP);
  
  while ( ReadComment( fi, buf, LEN ) ) {}
  // timewalk parameters
  {
    int cnt=0;
    while ( cnt<fNTWalkPar && fgets( buf, LEN, fi ) && ! strchr(buf,'[') ) {
      char *ptr = buf;
      int pos=0;
      while ( cnt < fNTWalkPar && sscanf(ptr,"%lf%n",&fTWalkPar[cnt],&pos)>0 ) {
	ptr += pos;
	cnt++;
      }
    }
  }

  while ( ReadComment( fi, buf, LEN ) ) {}
  // trigger timing offsets
  {
    int cnt=0;
    while ( cnt<fNelem && fgets( buf, LEN, fi ) && ! strchr(buf,'[') ) {
      char *ptr = buf;
      int pos=0;
      while ( cnt < fNelem && sscanf(ptr,"%lf%n",&fTrigOff[cnt],&pos)>0 ) {
	ptr += pos;
	cnt++;
      }
    }
  }
  
 exit:
  fclose(fi);

  if ( fDebug > 1 ) {
    cout << '\n' << GetPrefix() << " calibration parameters: " << endl;;
    for ( DBRequest *li = list; li->name; li++ ) {
      cout << "  " << li->name;
      int maxc = li->nelem;
      if (maxc==0)maxc=1;
      for (int i=0; i<maxc; i++) {
	if (li->type==kDouble) cout << "  " << ((Double_t*)li->var)[i];
	if (li->type==kInt) cout << "  " << ((Int_t*)li->var)[i];
      }
      cout << endl;
    }
  }
  
  return kOK;
}

//_____________________________________________________________________________
Int_t THcHodoscope::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    { "nlthit", "Number of Left paddles TDC times",  "fLTNhit" },
    { "nrthit", "Number of Right paddles TDC times", "fRTNhit" },
    { "nlahit", "Number of Left paddles ADCs amps",  "fLANhit" },
    { "nrahit", "Number of Right paddles ADCs amps", "fRANhit" },
    { "lt",     "TDC values left side",              "fLT" },
    { "lt_c",   "Corrected times left side",         "fLT_c" },
    { "rt",     "TDC values right side",             "fRT" },
    { "rt_c",   "Corrected times right side",        "fRT_c" },
    { "la",     "ADC values left side",              "fLA" },
    { "la_p",   "Corrected ADC values left side",    "fLA_p" },
    { "la_c",   "Corrected ADC values left side",    "fLA_c" },
    { "ra",     "ADC values right side",             "fRA" },
    { "ra_p",   "Corrected ADC values right side",   "fRA_p" },
    { "ra_c",   "Corrected ADC values right side",   "fRA_c" },
    { "nthit",  "Number of paddles with l&r TDCs",   "fNhit" },
    { "t_pads", "Paddles with l&r coincidence TDCs", "fHitPad" },
    { "y_t",    "y-position from timing (m)",        "fYt" },
    { "y_adc",  "y-position from amplitudes (m)",    "fYa" },
    { "time",   "Time of hit at plane (s)",          "fTime" },
    { "dtime",  "Est. uncertainty of time (s)",      "fdTime" },
    { "dedx",   "dEdX-like deposited in paddle",     "fAmpl" },
    { "troff",  "Trigger offset for paddles",        "fTrigOff"},
    { "trn",    "Number of tracks for hits",         "GetNTracks()" },
    { "trx",    "x-position of track in det plane",  "fTrackProj.THaTrackProj.fX" },
    { "try",    "y-position of track in det plane",  "fTrackProj.THaTrackProj.fY" },
    { "trpath", "TRCS pathlen of track to det plane","fTrackProj.THaTrackProj.fPathl" },
    { "trdx",   "track deviation in x-position (m)", "fTrackProj.THaTrackProj.fdX" },
    { "trpad",  "paddle-hit associated with track",  "fTrackProj.THaTrackProj.fChannel" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );
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

  delete [] fRA_c;    fRA_c    = NULL;
  delete [] fRA_p;    fRA_p    = NULL;
  delete [] fRA;      fRA      = NULL;
  delete [] fLA_c;    fLA_c    = NULL;
  delete [] fLA_p;    fLA_p    = NULL;
  delete [] fLA;      fLA      = NULL;
  delete [] fRT_c;    fRT_c    = NULL;
  delete [] fRT;      fRT      = NULL;
  delete [] fLT_c;    fLT_c    = NULL;
  delete [] fLT;      fLT      = NULL;
  
  delete [] fRGain;   fRGain   = NULL;
  delete [] fLGain;   fLGain   = NULL;
  delete [] fRPed;    fRPed    = NULL;
  delete [] fLPed;    fLPed    = NULL;
  delete [] fROff;    fROff    = NULL;
  delete [] fLOff;    fLOff    = NULL;
  delete [] fTWalkPar; fTWalkPar = NULL;
  delete [] fTrigOff; fTrigOff = NULL;

  delete [] fHitPad;  fHitPad  = NULL;
  delete [] fTime;    fTime    = NULL;
  delete [] fdTime;   fdTime   = NULL;
  delete [] fYt;      fYt      = NULL;
  delete [] fYa;      fYa      = NULL;
}

//_____________________________________________________________________________
inline 
void THcHodoscope::ClearEvent()
{
  // Reset per-event data.

  const int lf = fNelem*sizeof(Double_t);
  fLTNhit = 0;                            // Number of Left paddles TDC times
  memset( fLT, 0, lf );                   // Left paddles TDCs
  memset( fLT_c, 0, lf );                 // Left paddles corrected times
  fRTNhit = 0;                            // Number of Right paddles TDC times
  memset( fRT, 0, lf );                   // Right paddles TDCs
  memset( fRT_c, 0, lf );                 // Right paddles corrected times
  fLANhit = 0;                            // Number of Left paddles ADC amps
  memset( fLA, 0, lf );                   // Left paddles ADCs
  memset( fLA_p, 0, lf );                 // Left paddles ADC minus pedestal
  memset( fLA_c, 0, lf );                 // Left paddles corrected ADCs
  fRANhit = 0;                            // Number of Right paddles ADC smps
  memset( fRA, 0, lf );                   // Right paddles ADCs
  memset( fRA_p, 0, lf );                 // Right paddles ADC minus pedestal
  memset( fRA_c, 0, lf );                 // Right paddles corrected ADCs
  
  fNhit = 0;
  memset( fHitPad, 0, fNelem*sizeof(fHitPad[0]) );
  memset( fTime, 0, lf );
  memset( fdTime, 0, lf );
  memset( fYt, 0, lf );
  memset( fYa, 0, lf );
  
  fTrackProj->Clear();
}

//_____________________________________________________________________________
Int_t THcHodoscope::Decode( const THaEvData& evdata )
{

  // Get the hitlist (fRawHitList) for this event
  Int_t nhits = THcHitList::DecodeToHitList(evdata);

  return nhits;
}

//_____________________________________________________________________________
Int_t THcHodoscope::ApplyCorrections( void )
{
  // Apply the ADC/TDC corrections to get the 'REAL' relevant 
  // TDC and ADC values. No tracking needs to have been done yet.
  //
  // Permits the dividing up of the decoding step (events could come from
  // a different source) to the applying of corrections. For ease when
  // trying to optimize calibrations
  //
  Int_t nlt=0, nrt=0, nla=0, nra=0;
  for (Int_t i=0; i<fNelem; i++) {
    if (fLA[i] != 0.) {
      fLA_p[i] = fLA[i] - fLPed[i];
      fLA_c[i] = fLA_p[i]*fLGain[i];
      nla++;
    }
    if (fRA[i] != 0.) {
      fRA_p[i] = fRA[i] - fRPed[i];
      fRA_c[i] = fRA_p[i]*fRGain[i];
      nra++;
    }
    if (fLT[i] != 0.) {
      fLT_c[i] = (fLT[i] - fLOff[i])*fTdc2T - TimeWalkCorrection(i,kLeft);
      nlt++;
    }
    if (fRT[i] != 0.) {
      fRT_c[i] = (fRT[i] - fROff[i])*fTdc2T - TimeWalkCorrection(i,kRight);
      nrt++;
    }
  }
  // returns FALSE (0) if all matches up
  return !(fLTNhit==nlt && fLANhit==nla && fRTNhit==nrt && fRANhit==nra );
}

//_____________________________________________________________________________
Double_t THcHodoscope::TimeWalkCorrection(const Int_t& paddle,
					     const ESide side)
{
  // Calculate the time-walk correction. The timewalk might be
  // dependent upon the specific PMT, so information about exactly
  // which PMT fired is required.
  Double_t adc=0;
  if (side == kLeft)
    adc = fLA_p[paddle];
  else
    adc = fRA_p[paddle];

  if (fNTWalkPar<=0 || !fTWalkPar) return 0.; // uninitialized return safe 0

  // get the ADC value above the pedestal
  if ( adc <=0. ) return 0.;
  
  // we have an arbitrary timing offset, and will declare here that
  // for a MIP ( peak ~2000 ADC channels ) the timewalk correction is 0
  
  Double_t ref = fAdcMIP;
  Double_t tw(0), tw_ref(0.);
  int npar = fNTWalkPar/(2*fNelem);
  
  Double_t *par = &(fTWalkPar[npar*(side*fNelem+paddle)]);

  tw = par[0]*pow(adc,-.5);
  tw_ref = par[0]*pow(ref,-.5);

  return tw-tw_ref;
}

//_____________________________________________________________________________
Int_t THcHodoscope::CoarseProcess( TClonesArray& /* tracks */ )
{
  // Calculation of coordinates of particle track cross point with scint
  // plane in the detector coordinate system. For this, parameters of track 
  // reconstructed in THaVDC::CoarseTrack() are used.
  //
  // Apply corrections and reconstruct the complete hits.
  //
  static const Double_t sqrt2 = TMath::Sqrt(2.);
  
  ApplyCorrections();

  // count the number of paddles with complete TDC hits
  // Fill in information available from timing
  fNhit = 0;
  for (int i=0; i<fNelem; i++) {
    if (fLT[i]>0 && fRT[i]>0) {
      fHitPad[fNhit++] = i;
      fTime[i] = .5*(fLT_c[i]+fRT_c[i])-fSize[1]/fCn;
      fdTime[i] = fResolution/sqrt2;
      fYt[i] = .5*fCn*(fRT_c[i]-fLT_c[i]);
    }

    // rough calculation of position from ADC reading
    if (fLA_c[i]>0&&fRA_c[i]>0) {
      fYa[i] = TMath::Log(fLA_c[i]/fRA_c[i])/(2.*fAttenuation);
      // rough dE/dX-like quantity, not correcting for track angle
      fAmpl[i] = TMath::Sqrt(fLA_c[i]*fRA_c[i]*TMath::Exp(fAttenuation*2*fSize[1]))
	/ fSize[2];
    }
  }
  
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

  int n_track = tracks.GetLast()+1;   // Number of reconstructed tracks
  
  Double_t dpadx = (2.*fSize[0])/(fNelem); // width of a paddle
  // center of paddle '0'
  Double_t padx0 = -dpadx*(fNelem-1)*.5;
  
  for ( int i=0; i<n_track; i++ ) {
    THaTrack* theTrack = static_cast<THaTrack*>( tracks[i] );

    Double_t pathl=kBig, xc=kBig, yc=kBig, dx=kBig;
    Int_t pad=-1;
    
    if ( ! CalcTrackIntercept(theTrack, pathl, xc, yc) ) { // failed to hit
      new ( (*fTrackProj)[i] )
	THaTrackProj(xc,yc,pathl,dx,pad,this);
      continue;
    }
    
    // xc, yc are the positions of the track intercept
    //  _RELATIVE TO THE DETECTOR PLANE's_ origin.
    //
    // look through set of complete hits for closest match
    // loop through due to possible poor matches
    dx = kBig;
    for ( Int_t j=0; j<fNhit; j++ ) {
      Double_t dx2 = ( padx0 + fHitPad[j]*dpadx) - xc;
      if (TMath::Abs(dx2) < TMath::Abs(dx) ) {
	pad = fHitPad[j];
	dx = dx2;
      }
      else if (pad>=0) break; // stop after finding closest in X
    }

    // record information, found or not
    new ( (*fTrackProj)[i] )
      THaTrackProj(xc,yc,pathl,dx,pad,this);
  }
  
  return 0;
}

ClassImp(THcHodoscope)
////////////////////////////////////////////////////////////////////////////////
