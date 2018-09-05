/**
   \class THcReactionPoint
   \ingroup PhysMods

   \brief Calculate vertex coordinates.

   \author Mark Jones
*/

#include "THcReactionPoint.h"
#include "THaSpectrometer.h"
#include "THaTrack.h"
#include "THaBeam.h"
#include "TMath.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


using namespace std;

ClassImp(THcReactionPoint)

//_____________________________________________________________________________
THcReactionPoint::THcReactionPoint( const char* name, const char* description,
				    const char* spectro, const char* beam ) :
THaReactionPoint(name, description, spectro, beam)
{
  // Normal constructor.

}

//_____________________________________________________________________________
THcReactionPoint::~THcReactionPoint()
{
  // Destructor

  DefineVariables( kDelete );
}

//_____________________________________________________________________________
Int_t THcReactionPoint::Process( const THaEvData& )
{
  // Calculate the vertex coordinates.

  if( !IsOK() ) return -1;

  Int_t ntracks = fSpectro->GetNTracks();
  if( ntracks == 0 ) return 0;

  TClonesArray* tracks = fSpectro->GetTracks();
  if( !tracks ) return -2;

  TVector3 beam_org, beam_ray( 0.0, 0.0, 1.0 );
  if( fBeam ) {
    beam_org = fBeam->GetPosition();
    beam_ray = fBeam->GetDirection();
  }
  TVector3 v; 

  for( Int_t i = 0; i<ntracks; i++ ) {
    THaTrack* theTrack = static_cast<THaTrack*>( tracks->At(i) );
    // Ignore junk tracks
    if( !theTrack || !theTrack->HasTarget() ) 
      continue;  
    TVector3 pointing_off=fSpectro->GetPointingOffset();
    Double_t costheta=TMath::Cos(fSpectro->GetThetaSph());
    Double_t sintheta=TMath::Cos(fSpectro->GetPhiSph())*TMath::Sin(fSpectro->GetThetaSph());
    Double_t ytar_off=theTrack->GetTY()+pointing_off(1);
    Double_t yptar = theTrack->GetTPhi();
    Double_t ztarg=(ytar_off-beam_org(0)*(costheta-yptar*sintheta))/(-sintheta-yptar*costheta);
    v.SetXYZ(beam_org(0),beam_org(1),ztarg);
    theTrack->SetVertex(v);
    if( theTrack == fSpectro->GetGoldenTrack() ) {
      fVertex = theTrack->GetVertex();
      fVertexOK = kTRUE;
    }
  }
  return 0;
}
  
