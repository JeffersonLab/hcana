/** \class THcFormula
    \ingroup Base

\brief Enhanced THaFormula for use in report files.

In addition to global variables (gHaVars) and cuts (gHaCuts),
THcFormula expressions have access hcana parameters (gHcParms).

The number of times a cut has been try, as well as the number of times
that the cut has been tested can be accessed with cutname.`scaler` (or
.`npassed`) and cutname.`ncalled`.

\author S. A. Wood

*/

#include "THcFormula.h"
#include "THcParmList.h"
#include "THaArrayString.h"
#include "THaVarList.h"
#include "THaCutList.h"
#include "THaCut.h"
#include "TMath.h"

#include <iostream>
#include <cassert>
#include <numeric>

using namespace std;

//static const Double_t kBig = 1e38; // Error value

enum EFuncCode { kLength, kSum, kMean, kStdDev, kMax, kMin,
		 kGeoMean, kMedian, kIteration, kNumSetBits };

#define ALL(c) (c).begin(), (c).end()

//_____________________________________________________________________________
THcFormula::THcFormula(const char* name, const char* expression,
		       const THcParmList* plst, const THaVarList* vlst,
		       const THaCutList* clst ) :
  THaFormula()
{
  Bool_t do_register=0;

  fParmList = plst;
  fVarList = vlst;
  fCutList = clst;

  if( Init( name, expression ) != 0 ) {
    RegisterFormula(false);
    return;
  }

  SetBit(kNotGlobal,!do_register);

  Compile();   // This calls our own Compile()

  if( do_register )
    RegisterFormula();
}

//_____________________________________________________________________________
THcFormula& THcFormula::operator=( const THcFormula& rhs )
{
  if( this != &rhs ) {
    THaFormula::operator=(rhs);
    fParmList = rhs.fParmList;
  }
  return *this;
}

//_____________________________________________________________________________
THcFormula::THcFormula( const THcFormula& rhs ) :
  THaFormula(rhs), fParmList(rhs.fParmList)
{
  // Copy ctor
}

//_____________________________________________________________________________
THcFormula::~THcFormula()
{
  // Destructor
}

//_____________________________________________________________________________
Int_t THcFormula::DefinedCut( TString& name )
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
      thistype = kCut;
    }
  }
  return DefinedCutWithType(realname, thistype);
}

//_____________________________________________________________________________
Int_t THcFormula::DefinedGlobalVariable( TString& name )
{

  return DefinedGlobalVariableExtraList(name, (THaVarList*) fParmList);
}


//_____________________________________________________________________________

ClassImp(THcFormula)
