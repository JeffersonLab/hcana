#ifndef ROOT_THcRunParameters
#define ROOT_THcRunParameters

//////////////////////////////////////////////////////////////////////////
//
// THaRunParameters
//
//////////////////////////////////////////////////////////////////////////

#include "THaRunParameters.h"

class THcRunParameters : public THaRunParameters {
public:
  using THaRunParameters::operator=;
  THcRunParameters();
  virtual ~THcRunParameters();

  virtual Int_t ReadDatabase( const TDatime& date);

  ClassDef(THcRunParameters,0)
};

#endif
