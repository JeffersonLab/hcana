/** \class THcInterface
    \ingroup Base

\brief THcInterface is the interactive interface to the Hall C Analyzer.

Basically a rebranded copy of THaInterface with the addition of a global
for the Hall C style parameter database.

*/

#include "TROOT.h"
#include "THcInterface.h"
#include "THcParmList.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "ha_compiledata.h"

#include "TTree.h"

#include <iostream>
//#include "TGXW.h"
 //#include "TVirtualX.h"

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
    PrintLogo();

  SetPrompt("hcana [%d] ");
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
#if ROOT_VERSION_CODE < ROOT_VERSION(5,18,0)
void THcInterface::PrintLogo()
#else
void THcInterface::PrintLogo( Bool_t lite )
#endif
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

   const char* halla_version = HA_VERSION;
   //   const char* halla_date = Form("%d %s %4d",24,months[2-1],2003);

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
   if( !lite ) {
#endif
     Printf("  ************************************************");
     Printf("  *                                              *");
     Printf("  *            W E L C O M E  to  the            *");
     Printf("  *          H A L L C ++  A N A L Y Z E R       *");
     Printf("  *                                              *");
     Printf("  *            Based on                          *");
     Printf("  *  PODD Release  %10s %18s *",halla_version,__DATE__);
     Printf("  *  Based on ROOT %8s %20s *",root_version,root_date);
     Printf("  *                                              *");
     Printf("  *            For information visit             *");
     Printf("  *      http://hallcweb.jlab.org/hcana/docs/    *");
     Printf("  *                                              *");
     Printf("  ************************************************");
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
   }
#endif

#ifdef R__UNIX
   //   if (!strcmp(gGXW->GetName(), "X11TTF"))
   //   Printf("\nFreeType Engine v1.1 used to render TrueType fonts.");
#endif

   gInterpreter->PrintIntro();

}

//_____________________________________________________________________________
ClassImp(THcInterface)

