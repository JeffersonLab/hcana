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

#if ROOT_VERSION_CODE < 332288  // 5.18/00
  virtual void PrintLogo();
#else
  virtual void PrintLogo(Bool_t lite = kFALSE);
#endif
  static const char* GetVersionString();

protected:

  ClassDef(THcInterface,0)  //Hall C Analyzer Interactive Interface
};

#endif
