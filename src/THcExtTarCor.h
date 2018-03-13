#ifndef ROOT_THcExtTarCor
#define ROOT_THcExtTarCor

//////////////////////////////////////////////////////////////////////////
//
// THcExtTarCor
//
//////////////////////////////////////////////////////////////////////////

#include "THaExtTarCor.h"

class THaVertexModule;

class THcExtTarCor : public THaExtTarCor {
  
public:
  THcExtTarCor( const char* name, const char* description,
		const char* spectro="", const char* vertex="" );
  virtual ~THcExtTarCor();
  
  virtual Int_t     Process( const THaEvData& );

protected:

  virtual Int_t DefineVariables( EMode mode = kDefine );
  virtual Int_t ReadDatabase( const TDatime& date );

  ClassDef(THcExtTarCor,0)   //Extended target corrections module
};

#endif
