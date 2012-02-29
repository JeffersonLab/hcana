#ifndef ROOT_THcParmList
#define ROOT_THcParmList

//////////////////////////////////////////////////////////////////////////
//
// THcParmList
//
//////////////////////////////////////////////////////////////////////////

#include "THaVarList.h"

class THcParmList : public THaVarList {

public:
 
  THcParmList() : THaVarList() {}
  virtual ~THcParmList() { Clear(); }

  virtual void Load( const char *fname);

protected:

  ClassDef(THcParmList,0) // List of analyzer global parameters

};
#endif

