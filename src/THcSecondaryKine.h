#ifndef ROOT_THcSecondaryKine
#define ROOT_THcSecondaryKine

//////////////////////////////////////////////////////////////////////////
//
// THcSecondaryKine
//
//////////////////////////////////////////////////////////////////////////

#include "THaPhysicsModule.h"
#include "THcHallCSpectrometer.h"
#include "TLorentzVector.h"
#include "TString.h"

class THcPrimaryKine;
class THcHallCSpectrometer;
typedef TLorentzVector FourVect;

class THcSecondaryKine : public THaPhysicsModule {
  
public:
  THcSecondaryKine( const char* name, const char* description,
		    const char* secondary_spectro = "", 
		    const char* primary_kine = "", 
		    Double_t secondary_mass = 0.0 /* GeV */ );
  virtual ~THcSecondaryKine();
  
  virtual void      Clear( Option_t* opt="" );

  Double_t          GetTheta_xq()   const { return fTheta_xq; }
  Double_t          GetPhi_xq()     const { return fPhi_xq; }
  Double_t          GetTheta_bq()   const { return fTheta_bq; }
  Double_t          GetPhi_bq()     const { return fPhi_bq; }
  Double_t          GetXangle()     const { return fXangle; }
  Double_t          GetPmiss()      const { return fPmiss; }
  Double_t          GetPmiss_x()    const { return fPmiss_x; }
  Double_t          GetPmiss_y()    const { return fPmiss_y; }
  Double_t          GetPmiss_z()    const { return fPmiss_z; }
  Double_t          GetEmiss()      const { return fEmiss; }
  Double_t          GetMrecoil()    const { return fMrecoil; }
  Double_t          GetErecoil()    const { return fErecoil; }
  Double_t          GetPrecoil_x()  const { return fB.X(); }
  Double_t          GetPrecoil_y()  const { return fB.Y(); }
  Double_t          GetPrecoil_z()  const { return fB.Z(); }
  Double_t          GetTX()         const { return fTX; }
  Double_t          GetTB()         const { return fTB; }
  Double_t          GetPX_cm()      const { return fPX_cm; }
  Double_t          GetTheta_x_cm() const { return fTheta_x_cm; }
  Double_t          GetPhi_x_cm()   const { return fPhi_x_cm; }
  Double_t          GetTheta_b_cm() const { return fTheta_b_cm; }
  Double_t          GetPhi_b_cm()   const { return fPhi_b_cm; }
  Double_t          GetTX_cm()      const { return fTX_cm; }
  Double_t          GetTB_cm()      const { return fTB_cm; }
  Double_t          GetTtot_cm()    const { return fTtot_cm; }
  Double_t          GetMandelS()    const { return fMandelS; }
  Double_t          GetMandelT()    const { return fMandelT; }
  Double_t          GetMandelU()    const { return fMandelU; }

  Double_t          GetMX()         const { return fMX; }

  const TLorentzVector* GetPX()     const { return &fX; }
  const TLorentzVector* GetPB()     const { return &fB; }

  virtual EStatus   Init( const TDatime& run_time );
  virtual Int_t     Process( const THaEvData& );
  
  void      SetSpectrometer( const char* name );
  void      SetPrimary( const char* name );
  void      SetMX( Double_t m );
  
  THcPrimaryKine*   GetPrimary() const { return fPrimary; }
  
 protected:
  
  // Event data
  Double_t fTheta_xq;   // Polar angle of detected particle with q (rad)
  Double_t fPhi_xq;     // Azimuth of detected particle with scattering plane (rad)
  Double_t fTheta_bq;   // Polar angle of recoil system with q (rad)
  Double_t fPhi_bq;     // Azimuth of recoil system with scattering plane (rad)
  Double_t fXangle;     // Angle of detected particle with scattered electron (rad)
  Double_t fPmiss;      // Missing momentum magnitude (GeV), nuclear physics definition (-pB)
  Double_t fPmiss_x;    // x-component of p_miss wrt q (GeV)
  Double_t fPmiss_y;    // y-component of p_miss wrt q (GeV)
  Double_t fPmiss_z;    // z-component of p_miss, along q (GeV)
  Double_t fEmiss_nuc;      // Missing energy (GeV), nuclear physics definition omega-Tx-Tb
  Double_t fEmiss; // Missing energy (GeV), correct definition omega+Mt-Ex
  Double_t fMrecoil;    // Invariant mass of recoil system (GeV)
  Double_t fErecoil;    // Total energy of recoil system (GeV)
  Double_t fTX;         // Kinetic energy of detected particle (GeV)
  Double_t fTB;         // Kinetic energy of recoil system (GeV)
  Double_t fPX_cm;      // Magnitude of X momentum in CM system (GeV)
  Double_t fTheta_x_cm; // Polar angle of X in CM system wrt q (rad)
  Double_t fPhi_x_cm;   // Azimuth of X in CM system wrt q (rad)
  Double_t fTheta_b_cm; // Polar angle of recoil systm in CM wrt q (rad)
  Double_t fPhi_b_cm;   // Azimuth of recoil system in CM wrt q (rad)
  Double_t fTX_cm;      // Kinetic energy of X in CM (GeV)
  Double_t fTB_cm;      // Kinetic energy of B in CM (GeV)
  Double_t fTtot_cm;    // Total CM kinetic energy
  Double_t fMandelS;    // Mandelstam s for secondary vertex (GeV^2)
  Double_t fMandelT;    // Mandelstam t for secondary vertex (GeV^2)
  Double_t fMandelU;    // Mandelstam u for secondary vertex (GeV^2)

  TLorentzVector fX;    // Detected secondary particle 4-momentum (GeV)
  TLorentzVector fB;    // Recoil system 4-momentum (GeV)

  // Parameters
  Double_t fMX;         // Mass of secondary particle (GeV)
  Double_t fOopCentralOffset; //Offset of central out-of-plane angle (rad)

  TString            fSpectroName;  // Name of spectrometer for secondary particle
  THcHallCSpectrometer* fSpectro;   // Pointer to spectrometer object
  TString            fPrimaryName;  // Name of module for primary interaction kinematics
  THcPrimaryKine*    fPrimary;      // Pointer to primary kinematics module

  virtual Int_t DefineVariables( EMode mode = kDefine );
  virtual Int_t ReadDatabase( const TDatime& date );

  ClassDef(THcSecondaryKine,0)   //Secondary particle kinematics module
};

#endif
