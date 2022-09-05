#ifndef ROOT_THcInterface
#define ROOT_THcInterface

//////////////////////////////////////////////////////////////////////////
//
// THcInterface
//
//////////////////////////////////////////////////////////////////////////

#include "THaInterface.h"

class TClass;

class THcInterface : public THaInterface {

public:
  THcInterface( const char* appClassName, int* argc, char** argv,
		void* options = NULL, int numOptions = 0,
		Bool_t noLogo = kFALSE );
  virtual ~THcInterface();

  virtual void PrintLogo(Bool_t lite = kFALSE);
  static const char* GetHcDate();
  static const char* GetVersionString();

protected:

  ClassDef(THcInterface,0)  //Hall C Analyzer Interactive Interface
};

#endif
