//*-- Author :    Stephen Wood  17-Oct-2013

//////////////////////////////////////////////////////////////////////////
//
// THcFormula
//
// Tweaked THaFormula.  If cutname.scaler is used in a formula, then
// it is evaluated as the number of times that the cut passed.
// Use EVariableType of kUndefined to indicate cut scaler in list of
// variables used in the formula
//
//////////////////////////////////////////////////////////////////////////

#include "THcFormula.h"
#include "THcParmList.h"
#include "THaVarList.h"
#include "THaCutList.h"
#include "THaCut.h"

#include <iostream>

using namespace std;

static const Double_t kBig = 1e38; // Error value

//_____________________________________________________________________________
THcFormula::THcFormula(const char* name, const char* expression, 
		       const THcParmList* plst, const THaVarList* vlst,
		       const THaCutList* clst ) :
  THaFormula()
{

  // We have to duplicate the TFormula constructor code here because of
  // the calls DefinedVariable.  Our version will only get called if
  // to Compile(). Compile() only works if fParmList is set. 
  fParmList = plst;
  fVarList = vlst;
  fCutList = clst;

  SetName(name);

  //eliminate blanks in expression
  Int_t nch = strlen(expression);
  char *expr = new char[nch+1];
  Int_t j = 0;
  for (Int_t i=0;i<nch;i++) {
     if (expression[i] == ' ') continue;
     if (i > 0 && (expression[i] == '*') && (expression[i-1] == '*')) {
        expr[j-1] = '^';
        continue;
     }
     expr[j] = expression[i]; j++;
   }
  expr[j] = 0;
  if (j) SetTitle(expr);
  delete [] expr;

  Compile();   // This calls our own Compile()
}

//_____________________________________________________________________________
THcFormula& THcFormula::operator=( const THcFormula& rhs )
{
  if( this != &rhs ) {
    TFormula::operator=(rhs);
    fNcodes = rhs.fNcodes;
    fParmList = rhs.fParmList;
    fVarList = rhs.fVarList;
    fCutList = rhs.fCutList;
    fError = rhs.fError;
    fRegister = rhs.fRegister;
    delete [] fVarDef;
    fVarDef = new FVarDef_t[ kMAXCODES ];
    memcpy( fVarDef, rhs.fVarDef, kMAXCODES*sizeof(FVarDef_t));
  }
  return *this;
}

//_____________________________________________________________________________
THcFormula::~THcFormula()
{
  // Destructor
}

//_____________________________________________________________________________
Int_t THcFormula::DefinedCut( const TString& name )
{
  // Check if 'name' is a known cut. If so, enter it in the local list of 
  // variables used in this formula.

  EVariableType thistype;
  TString realname;
  Int_t period = name.Index('.');
  if(period < 0) {
    realname = name;
    thistype = kCut;
  } else {
    realname = name(0,period);
    TString attribute(name(period+1,name.Length()-period-1));
    if(attribute.CompareTo("scaler")==0 || attribute.CompareTo("npassed")==0) {
      thistype = (EVariableType) kCutScaler;
    } else if (attribute.CompareTo("ncalled")==0) {
      thistype = (EVariableType) kCutNCalled;
    } else {
      thistype = kUndefined;
    }
  }

  // Cut names are obviously only valid if there is a list of existing cuts
  if( fCutList ) {
    const THaCut* pcut = fCutList->FindCut( realname );
    if( pcut ) {
      if( fNcodes >= kMAXCODES ) return -1;
      // See if this cut already used earlier in this new cut
      FVarDef_t* def = fVarDef;
      for( Int_t i=0; i<fNcodes; i++, def++ ) {
	if( def->type == thistype && pcut == def->code )
	  return i;
      }
      def->type = thistype;
      def->code  = pcut;
      def->index = 0;
      fNpar = 0;
      return fNcodes++;
    }
  }
  return -1;
}

//_____________________________________________________________________________
Double_t THcFormula::DefinedValue( Int_t i )
{
  // Get value of i-th variable in the formula
  // If the i-th variable is a cut, return its last result
  // (calculated at the last evaluation).
  // If the variable is a string, return value of its character value
  
#ifdef WITH_DEBUG
  R__ASSERT( i>=0 && i<fNcodes );
#endif
  FVarDef_t* def = fVarDef+i;
  const void* ptr = def->code;
  if( !ptr ) return kBig;
  switch( (Int_t) def->type ) {
  case kVariable:
  case kString:
    return reinterpret_cast<const THaVar*>(ptr)->GetValue( def->index );
    break;
  case kCut:
    return reinterpret_cast<const THaCut*>(ptr)->GetResult();
    break;
  case kCutScaler:
    return reinterpret_cast<const THaCut*>(ptr)->GetNPassed();
    break;
  case kCutNCalled:
    return reinterpret_cast<const THaCut*>(ptr)->GetNCalled();
    break;
  default:
    return kBig;
  }
}  


//_____________________________________________________________________________
Int_t THcFormula::DefinedGlobalVariable( const TString& name )
{
  // Check if 'name' is a known global variable. If so, enter it in the
  // local list of variables used in this formula.

  // No list of variables or too many variables in this formula?
  if( (!fVarList && !fParmList) || fNcodes >= kMAXCODES )
    return -2;


  // Parse name for array syntax
  THaArrayString var(name);
  if( var.IsError() )
    return -1;

  // First check if this name is a Parameter
  const THaVar* obj = fParmList->Find( var.GetName() );
  if ( !obj) {  // If not, find a global variable with this name
    obj = fVarList->Find( var.GetName() );
    if( !obj )
      return -1;
  }

  // Error if array requested but the corresponding variable is not an array
  if( var.IsArray() && !obj->IsArray() )
    return -2;

  // Subscript(s) within bounds?
  Int_t index = 0;
  if( var.IsArray() 
      && (index = obj->Index( var )) <0 ) return -2;
		    
  // Check if this variable already used in this formula
  FVarDef_t* def = fVarDef;
  for( Int_t i=0; i<fNcodes; i++, def++ ) {
    if( obj == def->code && index == def->index )
      return i;
  }
  // If this is a new variable, add it to the list
  def->type = kVariable;
  def->code = obj;
  def->index = index;

  // No parameters ever for a THaFormula
  fNpar = 0;

  return fNcodes++;
}


//_____________________________________________________________________________

ClassImp(THcFormula)
