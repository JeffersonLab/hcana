//Author : Burcu Duran - Melanie Rehfuss (2017)

#ifndef ROOT_THcRasterRawHit
#define ROOT_THcRasterRawHit


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRasterRawHit                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawHodoHit.h"

class THcRasterRawHit : public THcRawHodoHit {

 public:

   friend class THcRaster;

 protected:

 private:

  ClassDef(THcRasterRawHit, 0);	// Raw hit class for raster data
};

#endif
