#ifndef ROOT_THcFormula
#define ROOT_THcFormula

//////////////////////////////////////////////////////////////////////////
//
// THcFormula
// 
//////////////////////////////////////////////////////////////////////////

#include "THaFormula.h"

class THcFormula : public THaFormula {

public:

  THcFormula( const char* name, const char* formula, 
	      const THaVarList*, const THaCutList* clst);
  virtual ~THcFormula();

  virtual Double_t DefinedValue( Int_t i);
  virtual Int_t    DefinedCut( const TString& variable);

protected:

  enum {kCutScaler = kString+1};
  enum {kCutNCalled = kCutScaler+1};
  ClassDef(THcFormula,0) // Formula with cut scalers
};

#endif
