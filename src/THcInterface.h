#ifndef ROOT_THcInterface
#define ROOT_THcInterface

//////////////////////////////////////////////////////////////////////////
//
// THcInterface
// 
//////////////////////////////////////////////////////////////////////////

#include "TRint.h"
#include "Decoder.h"

class TClass;

class THcInterface : public TRint {

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
  static TClass* GetDecoder();
  static TClass* SetDecoder( TClass* c );

  virtual const char* SetPrompt(const char *newPrompt);

protected:
  static THcInterface*  fgAint;  //Pointer indicating that interface already exists

  ClassDef(THcInterface,0)  //Hall C Analyzer Interactive Interface
};

#endif
