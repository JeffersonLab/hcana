/** \class THcInterface
    \ingroup Base

\brief THcInterface is the interactive interface to the Hall C Analyzer.

Basically a rebranded copy of THaInterface with the addition of a global
for the Hall C style parameter database.

*/

#include "TROOT.h"
#include "TClass.h"
#include "TError.h"
#include "TSystem.h"
#include "TString.h"
#include "TRegexp.h"
#include "THcInterface.h"
#include "TInterpreter.h"
#include "THaVarList.h"
#include "THcParmList.h"
#include "THcDetectorMap.h"
#include "THaCutList.h"
#include "CodaDecoder.h"
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THaAnalyzer.h"
//#include "THaFileDB.h"
#include "THaTextvars.h"
#include "ha_compiledata.h"

#include "TTree.h"

#include <iostream>
//#include "TGXW.h"
 //#include "TVirtualX.h"

using namespace std;

//THaVarList*  gHaVars     = NULL;  // List of symbolic analyzer variables
//THaCutList*  gHaCuts     = NULL;  // List of global analyzer cuts/tests
//TList*       gHaApps     = NULL;  // List of Apparatuses
//TList*       gHaScalers  = NULL;  // List of scaler groups
//TList*       gHaPhysics  = NULL;  // List of physics modules
//THaRunBase*  gHaRun      = NULL;  // The currently active run
//TClass*      gHaDecoder  = NULL;  // Class(!) of decoder to use
//THaDB*       gHaDB       = NULL;  // Database system to use
//THaTextvars* gHaTextvars = NULL;  // Text variable definitions

THcParmList* gHcParms     = NULL;  // List of symbolic analyzer variables
THcDetectorMap* gHcDetectorMap = NULL; // Global (Hall C style) detector map

THcInterface* THcInterface::fgAint = NULL;  // Pointer to this interface

static TString fgTZ;

//_____________________________________________________________________________
THcInterface::THcInterface( const char* appClassName, int* argc, char** argv,
			    void* options, int numOptions, Bool_t noLogo ) :
  TRint( appClassName, argc, argv, options, numOptions, kTRUE )
{
  /**
Create the Hall A/C analyzer application environment. The THcInterface
environment provides an interface to the the interactive ROOT system
via inheritance of TRint as well as access to the Hall A/C analyzer classes.

This class is copy of THaInterface with the addition of of the global
`gHcParms` to hold parameters.  It does not inherit from THaInterface.
  */

  if( fgAint ) {
    Error("THcInterface", "only one instance of THcInterface allowed");
    MakeZombie();
    return;
  }

  if( !noLogo )
    PrintLogo();

  SetPrompt("hcana [%d] ");
  gHaVars    = new THaVarList;
  gHcParms    = new THcParmList;
  gHaCuts    = new THaCutList( gHaVars );
  gHaApps    = new TList;
  gHaPhysics = new TList;
  gHaEvtHandlers = new TList;
  // Use the standard CODA file decoder by default
  gHaDecoder = Decoder::CodaDecoder::Class();
  // File-based database by default
  //  gHaDB      = new THaFileDB();
  gHaTextvars = new THaTextvars;

  //  cout << "In THcInterface ... " << endl;
  //  cout << "Decoder => " << gHaDecoder << endl;

  // Set the maximum size for a file written by Podd contained by the TTree
  //  putting it to 1.5 GB, down from the default 1.9 GB since something odd
  //  happens for larger files
  //FIXME: investigate
  //TTree::SetMaxTreeSize(1500000000);
  // Jure update: 100 GB
  TTree::SetMaxTreeSize(100000000000LL);

  // Make the Podd header directory(s) available so scripts don't have to
  // specify an explicit path.
  // If $ANALYZER defined, we take our includes from there, otherwise we fall
  // back to the compile-time directories (which may have moved!)
  TString s = gSystem->Getenv("ANALYZER");
  if( s.IsNull() ) {
    s = HA_INCLUDEPATH;
  } else {
    // Give preference to $ANALYZER/include
    TString p = s+"/include";
    void* dp = gSystem->OpenDirectory(p);
    if( dp ) {
      gSystem->FreeDirectory(dp);
      s = p;
    } else
      s = s+"/src " + s+"/hana_decode ";
  }
  // Directories names separated by blanks.
  // FIXME: allow blanks
  TRegexp re("[^ ]+");
  TString ss = s(re);
  while( !ss.IsNull() ) {
    // Only add dirs that exist
    void* dp = gSystem->OpenDirectory(ss);
    if( dp ) {
      gInterpreter->AddIncludePath(ss);
      gSystem->FreeDirectory(dp);
    }
    s.Remove(0,s.Index(re)+ss.Length());
    ss = s(re);
  }

  // Because of lack of foresight, the analyzer uses TDatime objects,
  // which are kept in localtime() and hence are not portable, and also
  // uses localtime() directly in several places. As a result, database
  // lookups depend on the timezone of the machine that the replay is done on!
  // If this timezone is different from the one in which the data were taken,
  // mismatches may occur. This is bad.
  // FIXME: Use TTimeStamp to keep time in UTC internally.
  // To be done in version 1.5
  //
  // As a temporary workaround, we assume that all data were taken in
  // US/Eastern time, and that the database has US/Eastern timestamps.
  // This should be true for all JLab production data..
  fgTZ = gSystem->Getenv("TZ");
  gSystem->Setenv("TZ","US/Eastern");


  fgAint = this;
}

//_____________________________________________________________________________
THcInterface::~THcInterface()
{
  // Destructor

  if( fgAint == this ) {
    // Restore the user's original TZ
    gSystem->Setenv("TZ",fgTZ.Data());
    // Clean up the analyzer object if defined
    delete THaAnalyzer::GetInstance();
    // Delete all global lists and objects contained in them
    delete gHaTextvars; gHaTextvars=0;
    //    delete gHaDB;           gHaDB = 0;
    delete gHaPhysics;   gHaPhysics=0;
    delete gHaEvtHandlers; gHaEvtHandlers=0;
    delete gHaApps;         gHaApps=0;
    delete gHaVars;         gHaVars=0;
    delete gHaCuts;         gHaCuts=0;

    delete gHcParms;        gHcParms=0;
    delete gHcDetectorMap;   gHcDetectorMap=0;

    fgAint = NULL;
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
     Printf("  *  PODD Release %10s %18s *",halla_version,__DATE__);
     Printf("  *  Based on ROOT %8s %20s *",root_version,root_date);
     //   Printf("  *             Development version              *");
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
TClass* THcInterface::GetDecoder()
{
  /// Get class of the current decoder
  cout << "In THcInterface::GetDecoder ... " << gHaDecoder << endl;
  return gHaDecoder;
}

//_____________________________________________________________________________
TClass* THcInterface::SetDecoder( TClass* c )
{
  /// Set the type of decoder to be used. Make sure the specified class
  /// actually inherits from the standard THaEvData decoder.
  /// Returns the decoder class (i.e. its argument) or NULL if error.

  if( !c ) {
    ::Error("THcInterface::SetDecoder", "argument is NULL");
    return NULL;
  }
  if( !c->InheritsFrom("THaEvData")) {
    ::Error("THcInterface::SetDecoder",
	    "decoder class must inherit from THaEvData");
    return NULL;
  }

  cout << "In THcInterface::SetDecoder ... " << c << endl;

  gHaDecoder = c;
  return gHaDecoder;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
const char* THcInterface::SetPrompt( const char* newPrompt )
{
  /// Make sure the prompt is and stays "hcana [%d]". ROOT 6 resets the
  /// interpreter prompt for every line without respect to any user-set
  /// default prompt.

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  return TRint::SetPrompt(newPrompt);
#else
  TString s;
  if( newPrompt ) {
    s = newPrompt;
    if( s.Index("root") == 0 )
      s.Replace(0,4,"hcana");
  }
  return TRint::SetPrompt(s.Data());
#endif
}

//_____________________________________________________________________________
ClassImp(THcInterface)

