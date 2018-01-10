#ifndef ROOT_THcPrimaryKine
#define ROOT_THcPrimaryKine

//////////////////////////////////////////////////////////////////////////
//
// THaPrimaryKine
//
//////////////////////////////////////////////////////////////////////////

#include "THaPhysicsModule.h"
#include "TLorentzVector.h"
#include "TString.h"

class THcHallCSpectrometer;
class THaBeamModule;
typedef TLorentzVector FourVect;

class THcPrimaryKine : public THaPhysicsModule {
  
public:
  THcPrimaryKine( const char* name, const char* description,
		  const char* spectro = "",
		  Double_t particle_mass = 0.0, /* GeV */
		  Double_t target_mass = 0.0 /* GeV */ );
  THcPrimaryKine( const char* name, const char* description,
		  const char* spectro, const char* beam,
		  Double_t target_mass = 0.0 /* GeV */ );
  virtual ~THcPrimaryKine();
  
  virtual void      Clear( Option_t* opt="" );

  Double_t          GetQ2()         const { return fQ2; }
  Double_t          GetOmega()      const { return fOmega; }
  Double_t          GetNu()         const { return fOmega; }
  Double_t          GetW2()         const { return fW2; }
  Double_t          GetXbj()        const { return fXbj; }
  Double_t          GetScatAngle()  const { return fScatAngle; }
  Double_t          GetEpsilon()    const { return fEpsilon; }
  Double_t          GetQ3mag()      const { return fQ3mag; }
  Double_t          GetThetaQ()     const { return fThetaQ; }
  Double_t          GetPhiQ()       const { return fPhiQ; }
  Double_t          GetMass()       const { return fM; }
  Double_t          GetTargetMass() const { return fMA; }

  const FourVect*   GetP0()         const { return &fP0; }
  const FourVect*   GetP1()         const { return &fP1; }
  const FourVect*   GetA()          const { return &fA; }
  const FourVect*   GetA1()         const { return &fA1; }
  const FourVect*   GetQ()          const { return &fQ; }

  virtual EStatus   Init( const TDatime& run_time );
  virtual Int_t     Process( const THaEvData& );
  virtual Int_t   ReadDatabase(const TDatime& date);
          void      SetMass( Double_t m );
          void      SetTargetMass( Double_t m );
          void      SetSpectrometer( const char* name );
          void      SetBeam( const char* name );

protected:

  Double_t          fQ2;           // 4-momentum transfer squared (GeV^2)
  Double_t          fOmega;        // Energy transfer (GeV)
  Double_t          fW2;           // s = Invariant mass using Mp (GeV^2)
  Double_t          fW;            // sqrt(s) (GeV)
  Double_t          fXbj;          // x Bjorken
  Double_t          fScatAngle;    // Scattering angle (rad)
  Double_t          fScatAngle_deg;    // Scattering angle (deg)
  Double_t          fEpsilon;      // Virtual photon polarization factor
  Double_t          fQ3mag;        // Magnitude of 3-momentum transfer
  Double_t          fThetaQ;       // Theta of 3-momentum vector (rad)
  Double_t          fPhiQ;         // Phi of 3-momentum transfer (rad)
  FourVect          fP0;           // Beam 4-momentum
  FourVect          fP1;           // Scattered electron 4-momentum
  FourVect          fA;            // Target 4-momentum
  FourVect          fA1;           // Recoil system 4-momentum
  FourVect          fQ;            // Momentum transfer 4-vector
  FourVect          fMp;            // Mp 4-momentum
  FourVect          fMp1;            // Recoil Mp 4-momentum
  
  Double_t          fM;            // Mass of incident particle (GeV/c^2)
  Double_t          fMA;           // Target mass (GeV/c^2)
  Double_t          fMA_amu;           // Target mass (amu)
  Double_t          fOopCentralOffset; // Out plane offset of spectrometer

  virtual Int_t DefineVariables( EMode mode = kDefine );

  TString                 fSpectroName;  // Name of spectrometer to consider
  TString                 fBeamName;     // Name of beam position apparatus
  THcHallCSpectrometer*   fSpectro;      // Pointer to spectrometer object
  THaBeamModule*          fBeam;         // Pointer to beam position apparatus

  ClassDef(THcPrimaryKine,0)   //Single arm kinematics module
};

#endif
