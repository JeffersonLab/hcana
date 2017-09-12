/** \class THcRunParameters
    \ingroup Base

\brief Get the run parameters from the parameter database
so that db_run.dat is not needed in hcana.

\author S. A. Wood 15-June-2017

*/
#include "THcRunParameters.h"
#include "THaAnalysisObject.h"
#include "TMath.h"
#include "THcParmList.h"
#include "THcGlobals.h"
#include <iostream>
/*#include "TDatime.h"
#include "TError.h"
#include "TMath.h"
#include "THaEvData.h"
#include <iostream>
*/

using namespace std;

//_____________________________________________________________________________
THcRunParameters::THcRunParameters() : THaRunParameters()
{
  // Default constructor
}

//_____________________________________________________________________________
THcRunParameters::~THcRunParameters()
{
  // Destructor
}
Int_t THcRunParameters::ReadDatabase( const TDatime& date )
{
  // Query the run database for the beam and target parameters
  // The date/time is ignored as it is assumed that run number
  // parameter lookup has already occured.

  // Return 0 if success, <0 if file error, >0 if not all required data found.
  Double_t P, E, M = 0.511e-3, Q = -1.0, dE = 0.0;

   DBRequest request[] = {
     { "gpbeam",  &P },
     { "mbeam",  &M,  kDouble, 0, 1 },
     { "qbeam",  &Q,  kDouble, 0, 1 },
     { "dEbeam", &dE, kDouble, 0, 1 },
     { 0 }
   };

   gHcParms->LoadParmValues((DBRequest*)&request, "");

   E = TMath::Sqrt(P*P+M*M);
   Int_t iq = int(Q);
   SetBeam( E, M, iq, dE );

   return 0;
 }
 ClassImp(THcRunParameters)
