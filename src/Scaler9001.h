#ifndef Scaler9001_
#define Scaler9001_

/////////////////////////////////////////////////////////////////////
//
//   Scaler9001
//   TI scalers
//
/////////////////////////////////////////////////////////////////////

#include "GenScaler.h"

namespace Decoder {

class Scaler9001 : public GenScaler {

public:

   Scaler9001() {};
   Scaler9001(Int_t crate, Int_t slot);
   virtual ~Scaler9001();

   virtual void Init();

private:

   static TypeIter_t fgThisType;

   ClassDef(Scaler9001,0)  // TI scalers

};

}

#endif
