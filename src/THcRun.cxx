/** \class THcRun
    \ingroup Base

\brief Description of a CODA run on disk with Hall C parameter DB

\author S. A. Wood, 31-October-2017

*/
#include "THcRun.h"
#include "THcGlobals.h"
#include "TSystem.h"

using namespace std;

//_____________________________________________________________________________
THcRun::THcRun( const char* fname, const char* description ) :
  THaRun(fname, description)
{
  // Normal & default constructor
  
  fHcParms = gHcParms;
}

//_____________________________________________________________________________
THcRun::~THcRun()
{
  // Destructor.

}

//_____________________________________________________________________________
void THcRun::Print( Option_t* opt ) const
{
  THaRun::Print( opt );
  fHcParms->Print();
}

ClassImp(THaRun)
