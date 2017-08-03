#ifndef ROOT_THcRasterRawHit
#define ROOT_THcRasterRawHit


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRasterRawHit                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRawHit.h"

class THcRasterRawHit : public THcRawHodoHit {

 public:

 THcRasterRawHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter),
    fADC_xsig(-1), fADC_ysig(-1),
    fADC_xsync(-1), fADC_ysync(-1) {
  }

  THcRasterRawHit& operator=( const THcRasterRawHit& );
  ~THcRasterRawHit() {}

  void Clear( Option_t* opt="" )
  { fADC_xsig = -1; fADC_ysig = -1; fADC_xsync = -1; fADC_ysync = -1; }

  void SetData(Int_t signal, Int_t data);
  Int_t GetData(Int_t signal);

  // signals
  Int_t fADC_xsig;
  Int_t fADC_ysig;
  Int_t fADC_xsync;
  Int_t fADC_ysync;

  friend class THcRaster; // added

 protected:

 private:

  ClassDef(THcRasterRawHit, 0);	// Raw hit class for raster data
};

#endif
