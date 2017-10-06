#ifndef Scaler9250_
#define Scaler9250_

/////////////////////////////////////////////////////////////////////
//
//   Scaler9250
//   FADC250 scalers
//
/////////////////////////////////////////////////////////////////////

#include "GenScaler.h"

namespace Decoder {

class Scaler9250 : public GenScaler {

public:

   Scaler9250() {};
   Scaler9250(Int_t crate, Int_t slot);
   virtual ~Scaler9250();

   virtual void Init();

private:

   static TypeIter_t fgThisType;

   ClassDef(Scaler9250,0)  // FADC250 scalers

};

}

#endif
