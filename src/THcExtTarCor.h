#ifndef ROOT_THcExtTarCor
#define ROOT_THcExtTarCor

//////////////////////////////////////////////////////////////////////////
//
// THcExtTarCor
//
//////////////////////////////////////////////////////////////////////////

#include "THaExtTarCor.h"
#include "Logger.h"

class THaVertexModule;

class THcExtTarCor : public hcana::ConfigLogging<THaExtTarCor> {
  
public:
  THcExtTarCor( const char* name, const char* description,
		const char* spectro="", const char* vertex="" );
  virtual ~THcExtTarCor();
  
  virtual Int_t     Process( const THaEvData& );

  Double_t fxsieve,fysieve;

protected:

  virtual Int_t DefineVariables( EMode mode = kDefine );
  virtual Int_t ReadDatabase( const TDatime& date );
   virtual void      Clear( Option_t* opt="" );

  ClassDef(THcExtTarCor,0)   //Extended target corrections module
};

#endif
