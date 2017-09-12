/** \class THcRasteredBeam
    \ingroup Base

\brief  Apparatus for a rastered beam

\author Buddhini Waidyawansa

*/

#include "THcRasteredBeam.h"
#include "THcRaster.h"
#include "TMath.h"
#include "TDatime.h"
#include "TList.h"

#include "VarDef.h"

//_____________________________________________________________________________
THcRasteredBeam::THcRasteredBeam( const char* name, const char* description ) :
    THaBeam( name, description )
{
  AddDetector( new THcRaster("raster","raster",this) );
}


//_____________________________________________________________________________
Int_t THcRasteredBeam::Reconstruct()
{

  TIter nextDet( fDetectors );

  nextDet.Reset();

  // This apparatus assumes that there is only one detector
  // in the list. If someone adds detectors by hand, the first
  // detector in the list will be used to get the beam position
  // the others will be processed
  // -- Iam not sure why the code is written like this. But for now, I am
  // going to work with this code as it is since all I need for is to
  // decode the raster - Buddhini


  if (THaBeamDet* theBeamDet=
      static_cast<THaBeamDet*>( nextDet() )) {
    theBeamDet->Process();
    fPosition = theBeamDet->GetPosition();
    fDirection = theBeamDet->GetDirection();
  }
  else {
    Error( Here("THcRasteredBeam::Reconstruct"),
	   "Beamline Detectors Missing in Detector List" );
  }


  // Process any other detectors that may have been added (by default none)
  while (THaBeamDet * theBeamDet=
	 static_cast<THaBeamDet*>( nextDet() )) {
    theBeamDet->Process();
  }

  Update();

  return 0;

}

//_____________________________________________________________________________
ClassImp(THcRasteredBeam)


////////////////////////////////////////////////////////////////////////////////
