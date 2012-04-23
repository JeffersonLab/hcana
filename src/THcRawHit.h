#ifndef ROOT_THcRawHit
#define ROOT_THcRawHit

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRawHit                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include "TObject.h"

class THcRawHit : public TObject {

 public:
  THcRawHit();
  virtual ~THcRawHit();

  THcRawHit(Int_t plane, Int_t counter);
  
  // THcRawHit( const THcRawHit& rhs ) : TObject(rhs) {}

  //  THcRawHit& operator=( const THcRawHit& rhs )
  //    { TObject::operator=(rhs); return *this; }


  virtual void Clear( Option_t* opt="" )=0;

  //  virtual Bool_t  operator==( const THcRawHit& ) = 0;
  //  virtual Bool_t  operator!=( const THcRawHit& ) = 0;

  virtual void SetData(Int_t signal, Int_t data) {};
  virtual Int_t GetData(Int_t signal) {return 0;};

  // Derived objects must be sortable and supply Compare method
  virtual Bool_t  IsSortable () const {return kFALSE; }
  virtual Int_t   Compare(const TObject* obj) const {return 0;}

  Int_t fPlane;
  Int_t fCounter;

protected:

  ClassDef(THcRawHit,0)
};

#endif
