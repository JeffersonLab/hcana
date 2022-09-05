/** \class THcInterface
    \ingroup Base

\brief THcInterface is the interactive interface to the Hall C Analyzer.

Basically a rebranded copy of THaInterface with the addition of a global
for the Hall C style parameter database.

*/

#include "TROOT.h"
#include "THcInterface.h"
#include "TInterpreter.h"
#include "THcParmList.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "ha_compiledata.h"
#include "hc_compiledata.h"
#include <sstream>

#include "TTree.h"

#include <iostream>
//#include "TGXW.h"
 //#include "TVirtualX.h"

// Workaround for SCons build
#ifndef HC_GITREV
#define HC_GITREV HC_GITVERS
#endif
#ifndef HC_BUILDTIME
#define HC_BUILDTIME HC_DATETIME
#endif
#ifndef HC_SOURCETIME
#define HC_SOURCETIME HC_DATETIME
#endif

using namespace std;

THcParmList* gHcParms     = NULL;  // List of symbolic analyzer variables
THcDetectorMap* gHcDetectorMap = NULL; // Global (Hall C style) detector map

//_____________________________________________________________________________
THcInterface::THcInterface( const char* appClassName, int* argc, char** argv,
			    void* options, int numOptions, Bool_t noLogo ) :
  THaInterface( appClassName, argc, argv, options, numOptions, kTRUE )

{
  /**
Create the Hall A/C analyzer application environment. The THcInterface
environment provides an interface to the the interactive ROOT system
via inheritance of TRint as well as access to the Hall A/C analyzer classes.

This class is copy of THaInterface with the addition of of the global
`gHcParms` to hold parameters.  It does not inherit from THaInterface.
  */

  if( !noLogo )
    THcInterface::PrintLogo();

  THaInterface::SetPrompt("hcana [%d] ");
  gHcParms    = new THcParmList;

  // Jure update: 100 GB
  TTree::SetMaxTreeSize(100000000000LL);

  fgAint = this;
}

//_____________________________________________________________________________
THcInterface::~THcInterface()
{
  // Destructor

  if( fgAint == this ) {
    delete gHcDetectorMap;   gHcDetectorMap=0;
  }
}

//_____________________________________________________________________________
void THcInterface::PrintLogo( Bool_t lite )
{
  /// Print the Hall C analyzer logo on standard output.

   Int_t iday,imonth,iyear,mille;
   static const char* months[] = {"Jan","Feb","Mar","Apr","May",
                                  "Jun","Jul","Aug","Sep","Oct",
                                  "Nov","Dec"};
   const char* root_version = gROOT->GetVersion();
   Int_t idatqq = gROOT->GetVersionDate();
   iday   = idatqq%100;
   imonth = (idatqq/100)%100;
   iyear  = (idatqq/10000);
   if ( iyear < 90 )
     mille = 2000 + iyear;
   else if ( iyear < 1900 )
     mille = 1900 + iyear;
   else
     mille = iyear;
   char* root_date = Form("%s %d %4d",months[imonth-1],iday,mille);

   if( !lite ) {
     Printf("  ************************************************");
     Printf("  *                                              *");
     Printf("  *            W E L C O M E  to  the            *");
     Printf("  *          H A L L C ++  A N A L Y Z E R       *");
     Printf("  *                                              *");
     Printf("  *  hcana release %12s %16s *", HC_VERSION, THcInterface::GetHcDate());
     Printf("  *  PODD release %13s %16s *", HA_VERSION, THaInterface::GetHaDate());
     Printf("  *  ROOT            %10s %16s *", root_version, root_date);
     Printf("  *                                              *");
     Printf("  *            For information visit             *");
     Printf("  *      http://hallcweb.jlab.org/hcana/docs/    *");
     Printf("  *                                              *");
     Printf("  ************************************************");
   }

   gInterpreter->PrintIntro();
}

//_____________________________________________________________________________
const char* THcInterface::GetHcDate()
{
  static TString hc_date;

  if( hc_date.IsNull() ) {
    bool use_buildtime = true;
    size_t len = strlen(HC_GITREV);
    if( len > 0 ) {
      const char* gitrev = HC_GITREV;
      use_buildtime = (len > 6 && strcmp(gitrev + len - 6, "-dirty") == 0);
    }
    if( use_buildtime )
      hc_date = extract_short_date(HC_BUILDTIME);
    else
      hc_date = extract_short_date(HC_SOURCETIME);
  }
  return hc_date.Data();
}

//_____________________________________________________________________________
const char* THcInterface::GetVersionString()
{
  // Get software version string (printed by analyzer -v)

  static TString version_string;

  if( version_string.IsNull() ) {
    ostringstream ostr;
    ostr << "hcana " << HC_VERSION;
    if( strlen(HC_GITREV) > 0 )
      ostr << " git@" << HC_GITREV;
    ostr << " " << GetHcDate() << endl;
    ostr << THaInterface::GetVersionString();
    version_string = ostr.str().c_str();
  }
  return version_string.Data();
}

//_____________________________________________________________________________
ClassImp(THcInterface)
