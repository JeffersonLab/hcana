#ifndef ROOT_THcGlobals
#define ROOT_THcGlobals

// Pick up definition of R__EXTERN
#ifndef ROOT_DllImport
#include "DllImport.h"
#endif

// Global Analyzer variables. Defined in THcInterface implementation file.

R__EXTERN class THcParmList*  gHcParms;      //List of global symbolic variables
R__EXTERN class THcDetectorMap*  gHcDetectorMap;   //Cached map file

#endif
