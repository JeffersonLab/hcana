/** \class THcPrimaryKine
    \ingroup PhysMods

\brief Class for the Calculate kinematics of scattering of the primary (beam) particle.
These are usually the electron kinematics.
*/

#include "THcPrimaryKine.h"
#include "THcHallCSpectrometer.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THaRunBase.h"
#include "THaRunParameters.h"
#include "THaBeam.h"
#include "VarDef.h"
#include "TMath.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

ClassImp(THcPrimaryKine)

//_____________________________________________________________________________
THcPrimaryKine::THcPrimaryKine( const char* name, const char* description,
				const char* spectro, Double_t particle_mass,
				Double_t target_mass ) :
  THaPhysicsModule(name,description), fM(particle_mass), 
  fMA(target_mass), fSpectroName(spectro), fSpectro(NULL), fBeam(NULL)
{
  // Standard constructor. Must specify particle mass. Incident particles
  // are assumed to be along z_lab.

}

//_____________________________________________________________________________
THcPrimaryKine::THcPrimaryKine( const char* name, const char* description,
				const char* spectro, const char* beam, 
				Double_t target_mass ) 
  : THaPhysicsModule(name,description), fM(-1.0), fMA(target_mass), 
    fSpectroName(spectro), fBeamName(beam), fSpectro(NULL), fBeam(NULL)
{
  // Constructor with specification of optional beam module.
  // Particle mass will normally come from the beam module.

}
//_____________________________________________________________________________

//_____________________________________________________________________________
THcPrimaryKine::~THcPrimaryKine()
{
  // Destructor

  DefineVariables( kDelete );
}

//_____________________________________________________________________________
void THcPrimaryKine::Clear( Option_t* opt )
{
  // Clear event-by-event variables.

  THaPhysicsModule::Clear(opt);
  fQ2 = fOmega = fW = fW2 = fXbj = fScatAngle = fScatAngle_deg = fEpsilon = fQ3mag
    = fThetaQ = fPhiQ = kBig;
  // Clear the 4-vectors  
  fP0.SetXYZT(kBig,kBig,kBig,kBig);
  fP1 = fA = fA1 = fQ = fP0;

}

//_____________________________________________________________________________
Int_t THcPrimaryKine::DefineVariables( EMode mode )
{
  // Define/delete global variables.

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  RVarDef vars[] = {
    { "Q2",      "4-momentum transfer squared (GeV^2)",     "fQ2" },
    { "omega",   "Energy transfer (GeV)",                   "fOmega" },
    { "W2",      "Invariant mass (GeV^2) for Mp      ",     "fW2" }, 
    { "W",      "Sqrt(Invariant mass)  for Mp      ",     "fW" }, 
    { "x_bj",    "Bjorken x",                               "fXbj" },
    { "scat_ang_rad",   "Scattering angle (rad)",                  "fScatAngle" },
    { "scat_ang_deg",   "Scattering angle (deg)",                  "fScatAngle_deg" },
    { "epsilon", "Virtual photon polarization factor",      "fEpsilon" },
    { "q3m",     "Magnitude of 3-momentum transfer",        "fQ3mag" },
    { "th_q",    "Theta of 3-momentum vector (rad)",        "fThetaQ" },
    { "ph_q",    "Phi of 3-momentum vector (rad)",          "fPhiQ" },
    { "nu",      "Energy transfer (GeV)",                   "fOmega" },
    { "q_x",     "x-cmp of Photon vector in the lab",       "fQ.X()" },
    { "q_y",     "y-cmp of Photon vector in the lab",       "fQ.Y()" },
    { "q_z",     "z-cmp of Photon vector in the lab",       "fQ.Z()" },
    { 0 }
  };
  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcPrimaryKine::Init( const TDatime& run_time )
{
  // Initialize the module.
  // Locate the spectrometer apparatus named in fSpectroName and save
  // pointer to it.

  fSpectro = dynamic_cast<THcHallCSpectrometer*>
    ( FindModule( fSpectroName.Data(), "THcHallCSpectrometer"));
  if( !fSpectro ) {
    fStatus = kInitError;
    return fStatus;
  }

  // Optional beam apparatus
  if( fBeamName.Length() > 0 ) {
    fBeam = dynamic_cast<THaBeamModule*>
      ( FindModule( fBeamName.Data(), "THaBeamModule") );
    if( !fBeam ) {
      fStatus = kInitError;
      return fStatus;
    }
    if( fM <= 0.0 )
      fM = fBeam->GetBeamInfo()->GetM();
  }

  // Standard initialization. Calls this object's DefineVariables().
  if( THaPhysicsModule::Init( run_time ) != kOK )
    return fStatus;

  if( fM <= 0.0 ) {
    Error( Here("Init"), "Particle mass not defined. Module "
	   "initialization failed" );
    fStatus = kInitError;
  }
  return fStatus;
}

//_____________________________________________________________________________
Int_t THcPrimaryKine::Process( const THaEvData& )
{
  // Calculate electron kinematics for the Golden Track of the spectrometer
  if( !IsOK() || !gHaRun ) return -1;

  THaTrackInfo* trkifo = fSpectro->GetTrackInfo();
  if( !trkifo || !trkifo->IsOK() ) return 1;
  // Determine 4-momentum of incident particle. 
  // If a beam module given, use it to get the beam momentum. This 
  // module may apply corrections for beam energy loss, variations, etc.

  Double_t xptar = trkifo->GetTheta() + fOopCentralOffset;
  TVector3 pvect;
  fSpectro->TransportToLab(trkifo->GetP(), xptar, trkifo->GetPhi(), pvect);

  if( fBeam ) {
    fP0.SetVectM( fBeam->GetBeamInfo()->GetPvect(), fM );
  } else {
    // If no beam given, assume beam along z_lab
    Double_t p_in  = gHaRun->GetParameters()->GetBeamP();
    fP0.SetXYZM( 0.0, 0.0, p_in, fM );
  }

  fP1.SetVectM( pvect, fM );
  fA.SetXYZM( 0.0, 0.0, 0.0, fMA );         // Assume target at rest

  // proton mass (for x_bj)
  const Double_t Mp = 0.93827;
  fMp.SetXYZM( 0.0, 0.0, 0.0, Mp );         // Assume target at rest


  // Standard electron kinematics
  fQ         = fP0 - fP1;  // cqx, cqy, cqz, omega
  fQ2        = -fQ.M2();
  fQ3mag     = fQ.P();
  fOmega     = fQ.E();
  // cqxzabs = TMath::Sqrt(fQ.X()*fQ.X() + fQ.Y()*fQ.Y());
  fA1        = fA + fQ;
  //  fW2        = fA1.M2();
  fMp1        = fMp + fQ;
  fW2        = fMp1.M2();
  if (fW2>0)  fW = TMath::Sqrt(fW2);
  fScatAngle = fP0.Angle( fP1.Vect() );
  fEpsilon   = 1.0 / ( 1.0 + 2.0*fQ3mag*fQ3mag/fQ2*
		       TMath::Power( TMath::Tan(fScatAngle/2.0), 2.0 ));
  fScatAngle_deg = fScatAngle*TMath::RadToDeg();
  fThetaQ    = fQ.Theta();
  fPhiQ      = fQ.Phi();
  fXbj       = fQ2/(2.0*Mp*fOmega);

  fDataValid = true;
  return 0;
}

Int_t THcPrimaryKine::ReadDatabase( const TDatime& date )
{

#ifdef WITH_DEBUG
  cout << "In THcPrimaryKine::ReadDatabase()" << endl;
#endif

  char prefix[2];

  prefix[0] = tolower(GetName()[0]);
  prefix[1] = '\0';
  
  fOopCentralOffset = 0.0;
  DBRequest list[]={
    {"gtargmass_amu",          &fMA_amu,             kDouble,         0,  1},
    {Form("%s_oopcentral_offset",prefix), &fOopCentralOffset,kDouble, 0, 1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list);
    //
  fMA= fMA_amu*931.5/1000.;
  return kOK;
}
  
//_____________________________________________________________________________
void THcPrimaryKine::SetMass( Double_t m ) 
{
  if( !IsInit())
    fM = m; 
  else
    PrintInitError("SetMass()");
}

//_____________________________________________________________________________
void THcPrimaryKine::SetTargetMass( Double_t m ) 
{
  if( !IsInit())
    fMA = m; 
  else
    PrintInitError("SetTargetMass()");
}

//_____________________________________________________________________________
void THcPrimaryKine::SetSpectrometer( const char* name ) 
{
  if( !IsInit())
    fSpectroName = name; 
  else
    PrintInitError("SetSpectrometer()");
}

//_____________________________________________________________________________
void THcPrimaryKine::SetBeam( const char* name ) 
{
  if( !IsInit())
    fBeamName = name; 
  else
    PrintInitError("SetBeam()");
}
