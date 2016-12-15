#ifndef ROOT_THcRasteredBeam
#define ROOT_THcRasteredBeam

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRasteredBeam                                                           //
// A beam with rastered beam, analyzed event by event using raster currents  //
// This is identical to THaRasteredBeam except that we need to use THcRaster //
// For raster signal processing.                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaBeam.h"


class THcRasteredBeam : public THaBeam {

 public:

  THcRasteredBeam( const char* name, const char* description) ;

  virtual ~THcRasteredBeam(){};

  virtual Int_t Reconstruct() ;

  ClassDef(THcRasteredBeam, 0);	// add THcRasteredBeam to ROOT library
};

////////////////////////////////////////////////////////////////////////////////

#endif
