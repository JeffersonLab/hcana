#ifndef ROOT_THcRawHit
#define ROOT_THcRawHit

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawHit                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include "TObject.h"

class THaVDCUVTrack;

class THcRawHit : public TObject {

public:
 THcRawHit(Int_t plane, Int_t counter) :
  fPlane(plane), fCounter(counter) {}

 THcRawHit( const THcRawHit& rhs ) : TObject(rhs) {}
  THcRawHit& operator=( const THcRawHit& rhs )
  { TObject::operator=(rhs); return *this; }

  virtual ~THcRawHit() {}

  virtual Bool_t  operator==( const THcRawHit& ) = 0;
  virtual Bool_t  operator!=( const THcRawHit& ) = 0;

  virtual void SetData(Int_t signal, Int_t data);
  virtual Int_t GetData(Int_t signal);

  Int_t fPlane;
  Int_t fCounter;

protected:

  ClassDef(THcRawHit,0)      // Track ID abstract base class
};

#endif
