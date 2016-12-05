/** \class THcFormula
    \ingroup Base

Enhanced THaFormula for use in report files.

In addition to global variables (gHaVars) and cuts (gHaCuts),
THcFormula expressions have access hcana parameters (gHcParms).

The number of times a cut has been try, as well as the number of times
that the cut has been tested can be accessed with cutname.`scaler` (or
.`npassed`) and cutname.`ncalled`.  

\author S. A. Wood

*/

#include "THcFormula.h"
#include "THcParmList.h"
#include "THaVarList.h"
#include "THaCutList.h"
#include "THaCut.h"
#include "TMath.h"

#include <iostream>
#include <numeric>

using namespace std;

static const Double_t kBig = 1e38; // Error value

enum EFuncCode { kLength, kSum, kMean, kStdDev, kMax, kMin,
		 kGeoMean, kMedian, kIteration, kNumSetBits };

#define ALL(c) (c).begin(), (c).end()

//_____________________________________________________________________________
static inline Int_t NumberOfSetBits( UInt_t v )
{
  /// Count number of bits set in 32-bit integer. From
  /// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel

  v = v - ((v >> 1) & 0x55555555);
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
  return (((v + (v >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

//_____________________________________________________________________________
static inline Int_t NumberOfSetBits( ULong64_t v )
{
  /// Count number of bits in 64-bit integer

  const ULong64_t mask32 = (1LL<<32)-1;
  return NumberOfSetBits( static_cast<UInt_t>(mask32 & v) ) +
    NumberOfSetBits( static_cast<UInt_t>(mask32 & (v>>32)) );
}

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
    TFormula::operator=(rhs);
    fParmList = rhs.fParmList;
    fVarList = rhs.fVarList;
    fCutList = rhs.fCutList;
    fInstance = 0;
  }
  return *this;
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

  // Cut names are obviously only valid if there is a list of existing cuts
  if( fCutList ) {
    THaCut* pcut = fCutList->FindCut( realname );
    if( pcut ) {
      // See if this cut already used earlier in the expression
      for( vector<FVarDef_t>::size_type i=0; i<fVarDef.size(); ++i ) {
	FVarDef_t& def = fVarDef[i];
	if( def.type == thistype && pcut == def.obj )
	  return i;
      }
      fVarDef.push_back( FVarDef_t(thistype,pcut,0) );
      fNpar = 0;
      return fVarDef.size()-1;
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
  
  typedef vector<Double_t>::size_type vsiz_t;
  typedef vector<Double_t>::iterator  viter_t;

  assert( i>=0 && i<(Int_t)fVarDef.size() );

  if( IsInvalid() )
    return 1.0;

  FVarDef_t& def = fVarDef[i];
  switch( (Int_t) def.type ) {
  case kVariable:
  case kString:
  case kArray:
    {
      const THaVar* var = static_cast<const THaVar*>(def.obj);
      assert(var);
      Int_t index = (def.type == kArray) ? fInstance : def.index;
      assert(index >= 0);
      if( index >= var->GetLen() ) {
	SetBit(kInvalid);
	return 1.0; // safer than kBig to prevent overflow
      }
      return var->GetValue( index );
    }
    break;
  case kCut:
    {
      const THaCut* cut = static_cast<const THaCut*>(def.obj);
      assert(cut);
      return cut->GetResult();
    }
    break;
  case kCutScaler:
    {
      const THaCut* cut = static_cast<const THaCut*>(def.obj);
      assert(cut);
      return cut->GetNPassed();
    }
    break;
  case kCutNCalled:
    {
      const THaCut* cut = static_cast<const THaCut*>(def.obj);
      assert(cut);
      return cut->GetNCalled();
    }
    break;
  case kFunction:
    {
      EFuncCode code = static_cast<EFuncCode>(def.index);
      switch( code ) {
      case kIteration:
	return fInstance;
      default:
	assert(false); // not reached
	break;
      }
    }
    break;
  case kFormula:
  case kVarFormula:
    {
      EFuncCode code = static_cast<EFuncCode>(def.index);
      THaFormula* func = static_cast<THaFormula*>(def.obj);
      assert(func);

      vsiz_t ndata = func->GetNdata();
      if( code == kLength )
	return ndata;

      if( ndata == 0 ) {
	//FIXME: needs thought
	SetBit(kInvalid);
	return 1.0;
      }
      Double_t y = 0.0;
      if( code == kNumSetBits ) {
	// Number of set bits is intended for unsigned int-type expressions
	y = func->EvalInstance(fInstance);
	if( y > kMaxULong64 || y < kMinLong64 ) {
	  return 0;
	}
	return NumberOfSetBits( static_cast<ULong64_t>(y) );
      }

      vector<Double_t> values;
      values.reserve(ndata);
      for( vsiz_t i = 0; i < ndata; ++i ) {
	values.push_back( func->EvalInstance(i) );
      }
      if( func->IsInvalid() ) {
	SetBit(kInvalid);
	return 1.0;
      }
      switch( code ) {
      case kSum:
	y = accumulate( ALL(values), static_cast<Double_t>(0.0) );
	break;
      case kMean:
	y = TMath::Mean( ndata, &values[0] );
	break;
      case kStdDev:
	y = TMath::RMS( ndata, &values[0] );
	break;
      case kMax:
	y = *max_element( ALL(values) );
	break;
      case kMin:
	y = *min_element( ALL(values) );
	break;
      case kGeoMean:
	y = TMath::GeomMean( ndata, &values[0] );
	break;
      case kMedian:
	y = TMath::Median( ndata, &values[0] );
	break;
      default:
	assert(false); // not reached
	break;
      }
      return y;
    }
    break;
  }
  assert(false); // not reached
  return kBig;
}  


//_____________________________________________________________________________
Int_t THcFormula::DefinedGlobalVariable( TString& name )
{
  // Check if 'name' is a known global variable. If so, enter it in the
  // local list of variables used in this formula.

  // No list of variables or too many variables in this formula?
  if( !fVarList && !fParmList )
    return -1;

  // Parse name for array syntax
  THaArrayString parsed_name(name);
  if( parsed_name.IsError() ) return -1;

  // First check if this name is a Parameter
  THaVar* var = fParmList->Find( parsed_name.GetName() );
  if ( !var) {  // If not, find a global variable with this name
    var = fVarList->Find( parsed_name.GetName() );
    if( !var )
      return -1;
  }

  EVariableType type = kVariable;
  Int_t index = 0;
  if( parsed_name.IsArray() ) {
    // Requesting an array element
    if( !var->IsArray() )
      // Element requested but the corresponding variable is not an array
      return -2;
    if( var->IsVarArray() ) {
      // Variable-size arrays are always 1-d
      assert( var->GetNdim() == 1 );
      if( parsed_name.GetNdim() != 1 )
	return -2;
      // For variable-size arrays, we allow requests for any element and
      // check at evaluation time whether the element actually exists
      index = parsed_name[0];
    } else {
      // Fixed-size arrays: check if subscript(s) within bounds?
      //TODO: allow fixing some dimensions
      index = var->Index( parsed_name );
      if( index < 0 )
	return -2;
    }
  } else if( var->IsArray() ) {
    // Asking for an entire array
    type = kArray;
    SetBit(kArrayFormula);
    if( var->IsVarArray() )
      SetBit(kVarArray);
  }

  // Check if this variable already used in this formula
  for( vector<FVarDef_t>::size_type i=0; i<fVarDef.size(); ++i ) {
    FVarDef_t& def = fVarDef[i];
    if( var == def.obj && index == def.index ) {
      assert( type == def.type );
      return i;
    }
  }
  // If this is a new variable, add it to the list
  fVarDef.push_back( FVarDef_t(type,var,index) );

  // No parameters ever for a THaFormula
  fNpar = 0;

  return fVarDef.size()-1;
}


//_____________________________________________________________________________

ClassImp(THcFormula)
