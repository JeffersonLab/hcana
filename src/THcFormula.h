#ifndef ROOT_THcFormula
#define ROOT_THcFormula

//////////////////////////////////////////////////////////////////////////
//
// THcFormula
//
//////////////////////////////////////////////////////////////////////////

#include "THcGlobals.h"
#include "THaFormula.h"

class THaParmList;

class THcFormula : public THaFormula {

public:

  THcFormula( const char* name, const char* formula,
	      const THcParmList*, const THaVarList*,
	      const THaCutList* clst);
  THcFormula( const THcFormula& rhs );
  THcFormula& operator=( const THcFormula& rhs );
  virtual ~THcFormula();

  virtual Int_t    DefinedCut( TString& variable);
  virtual Int_t    DefinedGlobalVariable( TString& variable);

protected:

  const THcParmList* fParmList; // Pointer to list of parameters
  ClassDef(THcFormula,0) // Formula with cut scalers
};

#endif
