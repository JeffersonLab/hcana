/////////////////////////////////////////////////////////////////////////

#include "THcExtTarCor.h"
#include "THaVertexModule.h"
#include "THcHallCSpectrometer.h"
#include "THaTrack.h"
#include "THaTrackInfo.h"
#include "TMath.h"
#include "TVector3.h"
#include "VarDef.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

//_____________________________________________________________________________
THcExtTarCor::THcExtTarCor( const char* name, const char* description,
			    const char* spectro, const char* vertex ) :
  THaPhysicsModule(name,description), fThetaCorr(0.0), fDeltaCorr(0.0),
  fSpectroName(spectro), fVertexName(vertex), 
  fTrackModule(NULL), fVertexModule(NULL)
{
  // Normal constructor.

  Clear();
}

//_____________________________________________________________________________
THcExtTarCor::~THcExtTarCor()
{
  // Destructor

  DefineVariables( kDelete );
}

//_____________________________________________________________________________
void THcExtTarCor::Clear( Option_t* opt )
{
  // Clear all event-by-event variables.
  
  THaPhysicsModule::Clear(opt);
  TrkIfoClear();
  fDeltaTh = fDeltaDp = fDeltaP = 0.0;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcExtTarCor::Init( const TDatime& run_time )
{
  // Initialize the module.
  // Locate the spectrometer apparatus named in fSpectroName and save
  // pointer to it.
  // Also, if given, locate the vertex module given in fVertexName
  // and save pointer to it.

  fTrackModule = dynamic_cast<THaTrackingModule*>
    ( FindModule( fSpectroName.Data(), "THaTrackingModule"));
  if( !fTrackModule )
    return fStatus;

  fTrkIfo.SetSpectrometer( fTrackModule->GetTrackInfo()->GetSpectrometer() );

  // If no vertex module given, try to get the vertex info from the
  // same module as the tracks, e.g. from a spectrometer
  if( fVertexName.IsNull())  fVertexName = fSpectroName;

  fVertexModule = dynamic_cast<THaVertexModule*>
    ( FindModule( fVertexName.Data(), "THaVertexModule" ));
  if( !fVertexModule )
    return fStatus;
    
  // Standard initialization. Calls this object's DefineVariables().
  THaPhysicsModule::Init( run_time );

  return fStatus;
}

//_____________________________________________________________________________
Int_t THcExtTarCor::DefineVariables( EMode mode )
{
  // Define/delete global variables.

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  DefineVarsFromList( THaTrackingModule::GetRVarDef(), mode );

  const RVarDef var2[] = {
    { "delta_p",  "Size of momentum correction",    "fDeltaP" },
    { "delta_dp", "Size of delta correction (%)",       "fDeltaDp" },
    { "delta_xptar", "Size of xptar correction (rad)", "fDeltaTh" },
    { 0 }
  };
  DefineVarsFromList( var2, mode );
  return 0;
}

//_____________________________________________________________________________
Int_t THcExtTarCor::Process( const THaEvData& )
{
  // Calculate corrections and adjust the track parameters.

  if( !IsOK() ) return -1;

  THaTrackInfo* trkifo = fTrackModule->GetTrackInfo();
  if( !trkifo->IsOK() ) return 2;
  THcHallCSpectrometer* spectro = static_cast <THcHallCSpectrometer*>(trkifo->GetSpectrometer());
  if( !spectro ) return 3;

 
  Double_t ray[6];
  spectro->LabToTransport( fVertexModule->GetVertex(), 
			   trkifo->GetPvect(), ray );
  TVector3 vertex =  fVertexModule->GetVertex();
  TVector3 tempp =  trkifo->GetPvect();
  Double_t ztarg= vertex(2);
  /* Ignore junk
  if( TMath::Abs(ray[0]) > 0.1 || TMath::Abs(ray[1]) > 1.0 ||
      TMath::Abs(ray[2]) > 0.1 || TMath::Abs(ray[3]) > 1.0 ||
      TMath::Abs(ray[5]) > 1.0 ) 
    return 3;
  */
  Int_t ntracks = spectro->GetNTracks();
  if( ntracks == 0 ) return -2;
  Double_t xptar;
  Double_t ytar; 
  Double_t yptar;
  Double_t delta;
  Double_t p=0;
  TVector3 pvect;
  Double_t xtar_new=vertex[1];
  TClonesArray* tracks = spectro->GetTracks();
  if( !tracks ){
    return -2;
  }
  for( Int_t i = 0; i<ntracks; i++ ) {
    THaTrack* theTrack = static_cast<THaTrack*>( tracks->At(i) );
    if( theTrack == spectro->GetGoldenTrack() ) {
      // Calculate corrections & recalculate ,,,track parameters
      //cout << " orig" << spectro->GetName() << " " <<theTrack->GetTTheta()<< " " << theTrack->GetDp() << endl;
      Double_t x_tg = vertex[1];
      spectro->CalculateTargetQuantities(theTrack,x_tg,xptar,ytar,yptar,delta);
      p  = spectro->GetPcentral() * ( 1.0+delta );
      spectro->TransportToLab( p, xptar, yptar, pvect );
      Double_t theta=spectro->GetThetaSph();
      xtar_new = x_tg - xptar*ztarg*cos(theta);
      // Get a second-iteration value for x_tg based on the 
      spectro->CalculateTargetQuantities(theTrack,xtar_new,xptar,ytar,yptar,delta);
      fDeltaDp = delta*100 -theTrack->GetDp();
      fDeltaP = p - theTrack->GetP();
      fDeltaTh = xptar -  theTrack->GetTTheta();
     theTrack->SetTarget(0.0, ytar*100.0, xptar, yptar);
    theTrack->SetDp(delta*100.0);	// Percent.  
    Double_t ptemp =spectro->GetPcentral()*(1+theTrack->GetDp()/100.0);
      theTrack->SetMomentum(ptemp);
    TVector3 pvect_temp;
    spectro->TransportToLab(theTrack->GetP(),theTrack->GetTTheta(),theTrack->GetTPhi(),pvect_temp);
    theTrack->SetPvect(pvect_temp);
    }
  }
 // Save results in our TrackInfo
 // cout << spectro->GetName() << " exttarcor = " << xptar << " " << delta*100 << endl;
  trkifo->Set( p, delta*100, xtar_new,100*ytar, xptar, yptar, pvect );
  fTrkIfo.Set( p, delta*100, xtar_new,100*ytar, xptar, yptar, pvect );
  fDataValid = true;
  return 0;
}

Int_t THcExtTarCor::ReadDatabase( const TDatime& date )
{

#ifdef WITH_DEBUG
  cout << "In THcExtTarCor::ReadDatabase()" << endl;
#endif
  //

  // cout << " GetName() " << GetName() << endl;
  return kOK;
}
 
//_____________________________________________________________________________
ClassImp(THcExtTarCor)

